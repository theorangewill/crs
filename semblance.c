/** @file semblance.c
 *  @brief Implementação da biblioteca semblance.h
 *
 *  @author William F. C. Tavares (theorangewill).
 */

#ifndef SEMBLANCE_H
#include "semblance.h"
#define SEMBLANCE_H
#endif

float time2D(float A, float B, float C, float t0, float h, float md)
{
    float temp;
    temp = t0+A*md;
    temp = temp*temp;
    temp += B*md*md;
    temp += C*h*h;
    if(temp < 0) return -1;
    return sqrt(temp);
}

float HalfOffset(Traco *traco, float azimuth)
{
    float hx, hy;
    //Calcula os eixos x e y
    OffsetSU(traco,&hx,&hy);
    //Metade
    hx/=2;
    hy/=2;
    //Retorna a raiz quadrada do quadrado dos eixos
    return hx * sin(azimuth) + hy * cos(azimuth);
    //return sqrt(hx*hx + hy*hy);
}

void InterpolacaoLinear(float *x, float x0, float x1, float y, float y0, float y1)
{
    *x = x0 + (x1- x0) * (y - y0) / (y1 - y0);
}


float SemblanceCMP(ListaTracos *lista, float A, float B, float C, float t0, float wind, float seg, float *pilha, float azimuth)
{
    int traco;
    float t, h;
    int amostra, k;
    int w = (int) (wind/seg);
    int janela = 2*w+1;
    int N;
    float numerador[janela], denominador;
    float num;
    float valor;
    int j;
    int erro;
    int vizinho;
    float md, mx, my, vx, vy, dx, dy;

    //Numerador e denominador da funcao semblance zerados
    memset(&numerador,0,sizeof(numerador));
    denominador = 0;
    N = 0;

    erro = 0;
    //Para cada traco do conjunto
    for(traco=0; traco<lista->tamanho; traco++){
        //Calcular metade do offset do traco
        h = HalfOffset(lista->tracos[traco],azimuth);
        //Calcular o tempo de acordo com a funcao da hiperbole
        t = time2D(0.0,0.0,C,t0,h,0.0);
        if(t < 0) return -1;
        //Calcular a amostra equivalente ao tempo calculado
        amostra = (int) (t/seg);
        //Se a janela da amostra cobre os dados sismicos
        if(amostra - w >= 0 && amostra + w < lista->tracos[traco]->ns){
            //Para cada amostra dentro da janela
            for(j=0; j<janela; j++){
                k = amostra - w + j;
                //Interpolacao linear entre as duas amostras
                InterpolacaoLinear(&valor,lista->tracos[traco]->dados[k],lista->tracos[traco]->dados[k+1], t/seg-w+j, k, k+1);
                numerador[j] += valor;
                denominador += valor*valor;
                *pilha += valor;
            }
            N++;
        }
        else{
            erro++;
        }
        if(erro == 2) return -1;
    }

    num = 0;
    for(j=0; j<janela; j++){
        num += numerador[j]*numerador[j];
    }
    *pilha = (*pilha)/N/janela;
    return num / N / denominador;
}

float Semblance(ListaTracos *lista, float A, float B, float C, float t0, float wind, float seg, float *pilha, float azimuth)
{
    int traco;
    float t, h;
    int amostra, k;
    int w = (int) (wind/seg);
    int janela = 2*w+1;
    int N;
    float numerador[janela], denominador;
    float num;
    float valor;
    int j;
    int erro;
    int vizinho;
    float md, mx, my, vx, vy, dx, dy, m0, v0;

    //Numerador e denominador da funcao semblance zerados
    memset(&numerador,0.0,sizeof(numerador));
    denominador = 0.0;
    N = 0;

    erro = 0;
    //Para cada traco do conjunto
    for(traco=0; traco<lista->tamanho; traco++){
        //Calcular metade do offset do traco

        h = HalfOffset(lista->tracos[traco], azimuth);
        //Calcular o tempo de acordo com a funcao da hiperbole
        t = time2D(A,B,C,t0,h,0.0);
        if(t < 0){
              printf("TEMPO NEGATIVO\n");
              return -1;
          }
        //Calcular a amostra equivalente ao tempo calculado
        amostra = (int) (t/seg);
        //Se a janela da amostra cobre os dados sismicos
        if(amostra - w >= 0 && amostra + w < lista->tracos[traco]->ns){
            //Para cada amostra dentro da janela
            for(j=0; j<janela; j++){
                k = amostra - w + j;
                //Interpolacao linear entre as duas amostras
                InterpolacaoLinear(&valor,lista->tracos[traco]->dados[k],lista->tracos[traco]->dados[k+1], t/seg-w+j, k, k+1);
                numerador[j] += valor;
                denominador += valor*valor;
                *pilha += valor;
            }
            N++;
        }
        else{
            erro++;
        }
        if(erro == 2) return -1;
    }

    //Para cada vizinho, se é CMP, vizinhos = 0;
    MidpointSU(lista->tracos[0],&mx,&my);
    m0 = mx * sin(azimuth) + my * cos(azimuth);
    printf("M: %lf %lf\n", mx, my);
    for(vizinho=0; vizinho<lista->numeroVizinhos; vizinho++){
      MidpointSU(lista->vizinhos[vizinho]->tracos[0],&vx,&vy);
      v0 = vx * sin(azimuth) + vy * cos(azimuth);
    printf("V: %lf %lf\n", vx, vy);
      //dx = vx - mx;
      //dy = vy - my;
      //md = sqrt(dx*dx + dy*dy);
      md = v0 - m0;
      erro = 0;
      for(traco=0; traco<lista->vizinhos[vizinho]->tamanho; traco++){
          //Calcular metade do offset do traco
          h = HalfOffset(lista->vizinhos[vizinho]->tracos[traco],azimuth);
          //Calcular o tempo de acordo com a funcao da hiperbole
          t = time2D(A,B,C,t0,h,md);
          if(t < 0){
            printf("%.20lf %.20lf %.20lf %.20lf %.20lf %.20lf \n", A, B, C, t0, h, md);
              printf("TEMPO NEGATIVO VIZINHO %d\n", traco);
              return -1;
          }
          //Calcular a amostra equivalente ao tempo calculado
          amostra = (int) (t/seg);
          //Se a janela da amostra cobre os dados sismicos
          if(amostra - w >= 0 && amostra + w < lista->vizinhos[vizinho]->tracos[traco]->ns){
              //Para cada amostra dentro da janela
              for(j=0; j<janela; j++){
                  k = amostra - w + j;
                  //Interpolacao linear entre as duas amostras
                  InterpolacaoLinear(&valor,lista->vizinhos[vizinho]->tracos[traco]->dados[k],lista->vizinhos[vizinho]->tracos[traco]->dados[k+1], t/seg-w+j, k, k+1);
                  numerador[j] += valor;
                  denominador += valor*valor;
                  *pilha += valor;
              }
              N++;
          }
          else{
              erro++;
          }
          if(erro == 2) return -1;
      }
    }
    num = 0;
    for(j=0; j<janela; j++){
        num += numerador[j]*numerador[j];
    }
    *pilha = (*pilha)/N/janela;
    return num / (N * denominador);
}
