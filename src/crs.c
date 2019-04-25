/** @file crs.c
 *  @brief Algoritmo CRS
 *
 *  @author William F. C. Tavares (theorangewill).
 */

#ifndef SEISMICUNIX_H
#include "seismicunix.h"
#define SEISMICUNIX_H
#endif

#ifndef SEMBLANCE_H
#include "semblance.h"
#define SEMBLANCE_H
#endif

#ifndef MATH_H
#include <math.h>
#define MATH_H
#endif

#ifndef STRING_H
#include <string.h>
#define STRING_H
#endif

#ifndef OMP_H
#include "omp.h"
#define OMP_H
#endif

#define NUM_THREADS 4
#define PI 3.14159265
/*
 * Algoritmo CRS.
 */
void CRS(ListaTracos *lista, float *Avector, int Aint, float *Angvector,
          float Bpctg, int Bint,
          float *Vvector, int Vint,
          float *Cvector,
          float wind, float azimuth, Traco* tracoEmpilhado,
          Traco* tracoSemblance, Traco* tracoA, Traco* tracoB, Traco* tracoV);

/*
 * Seta os campos do cabecalho para o empilhamento
 */
void SetCabecalhoCRS(Traco *traco);


/*
 * Libera memoria alocada para o programa.
 */
void LiberarMemoria(ListaTracos ***lista, int *tamanho);


int main (int argc, char **argv)
{
    ListaTracos **listaTracos = NULL;
    int tamanhoLista = 0;
    float Aang, Av0;
    int Aint;
    float Bpctg;
    int Bint;
    float Vini, Vfin, Vinc;
    int Vint;
    float wind, md, aph;
    int tracos;
    int i, zero;
    float *Avector, *Angvector, *Vvector, *Cvector;
    char saida[101], saidaEmpilhado[104], saidaSemblance[104], saidaA[104], saidaB[104], saidaV[104];
    FILE *arquivoEmpilhado, *arquivoSemblance, *arquivoA, *arquivoB, *arquivoV;
    Traco tracoSemblance, tracoA, tracoB, tracoEmpilhado, tracoV;
    float azimuth;

    if(argc < 14){
        printf("ERRO: ./main <dado sismico> A_ANG A_V0 A_INT B_PCTG B_INT V_INI V_FIN V_INT MD WIND APH AZIMUTH\n");
        printf("\tARQUIVO: arquivo dos tracos sismicos\n");
        printf("\tA_ANG:  angulo de A\n");
        printf("\tA_V0:  v0 de A\n");
        printf("\tA_INT:    quantidade de A avaliados\n");
        printf("\tB_PCTG:  porcentagem de C para B\n");
        printf("\tB_INT:    quantidade de B avaliados\n");
        printf("\tV_INI:  velocidade inicial\n");
        printf("\tV_FIN:  velocidade final\n");
        printf("\tV_INT:    quantidade de velocidades avaliadas\n");
        printf("\tMD:    distancia dos vizinhos\n");
        printf("\tWIND:    janela do semblance (em amostras)\n");
        printf("\tAPH:    aperture\n");
        printf("\tAZIMUTH:    azimuth\n");
        exit(1);
    }

    //Leitura dos parametros
    Aang = atof(argv[2]);
    Av0 = atof(argv[3]);
    Aint = atoi(argv[4]);
    Bpctg = atof(argv[5]);
    Bint = atoi(argv[6]);
    Vini = atof(argv[7]);
    Vfin = atof(argv[8]);
    Vint = atoi(argv[9]);
    md = atof(argv[10]);
    wind = atof(argv[11]);
    aph = atof(argv[12]);
    azimuth = atof(argv[13]);

    //printf("A=%.20lf %.20lf %.20lf B=%.20lf %.20lf %.20lf C=%.20lf %.20lf %.20lf %.20lf %.20lf %.20lf\n", Aini, Afin, Ainc, Bini, Bfin, Binc, Cini, Cfin, Cinc, md, wind, aph);

    //Leitura do arquivo
    if(!LeitorArquivoSU(argv[1], &listaTracos, &tamanhoLista, aph,azimuth)){
        printf("ERRO NA LEITURA\n");
        exit(1);
    }

ListaTracos **listaV, **listaSemblance;
int tamV, tamSemblance;
    if(!LeitorArquivoSU(argv[14], &listaV, &tamV, aph, azimuth)){
        printf("ERRO NA LEITURA V\n");
        exit(1);
    }

    if(!LeitorArquivoSU(argv[15], &listaSemblance, &tamSemblance, aph, azimuth)){
        printf("ERRO NA LEITURA Semblance\n");
        exit(1);
    }


  /*
    for(int i=0; i<tamanhoLista; i++){
      if(listaTracos[i]->cdp == 250){
        for(int j=0; j<listaTracos[i]->tamanho; j++)
          PrintTracoSU(listaTracos[i]->tracos[j]);
      }
    }

    exit(1);
  */
    //Criacao dos arquivos de saida
    argv[1][strlen(argv[1])-3] = '\0';
    strcpy(saida,argv[1]);
    strcpy(saidaEmpilhado,saida);
    strcat(saidaEmpilhado,"-empilhado.out.su");
    arquivoEmpilhado = fopen(saidaEmpilhado,"w");
    strcpy(saidaSemblance,saida);
    strcat(saidaSemblance,"-semblance.out.su");
    arquivoSemblance = fopen(saidaSemblance,"w");
    strcpy(saidaA,saida);
    strcat(saidaA,"-A.out.su");
    arquivoA = fopen(saidaA,"w");
    strcpy(saidaB,saida);
    strcat(saidaB,"-B.out.su");
    arquivoB = fopen(saidaB,"w");
    strcpy(saidaV,saida);
    strcat(saidaV,"-V.out.su");
    arquivoV = fopen(saidaV,"w");


#ifdef OMP_H
    omp_set_num_threads(NUM_THREADS);
#endif

    //Calcular os valores de busca para A, V e C
    float Angini, Angfin, Anginc;
    Angini = -Aang;
    Angfin = Aang;
    Anginc = 2.0 * Angfin / (Aint);
    float Aini, Afin, Ainc;
    Aini = 2.0 * sin(Angini * PI / 180) / Av0;
    Afin = 2.0 * sin(Angfin * PI / 180) / Av0;
    Ainc = 2.0 * Afin / (Aint);

    /*Aini = 2.0 * sin(Aang) / Av0;
    if(Aini > 0) Aini *= -1;
    Afin = -Aini;
    Ainc = 2.0 * Afin / (Aint-1);*/
    Avector = malloc(sizeof(float)*(Aint+1));
    Angvector = malloc(sizeof(float)*(Aint+1));
    Angvector[0] = 0.0;
    Avector[0] = 0.0;
    Aint++;
    for(i=1; i<Aint; i++){
      //Avector[i] = Ainc*i+Aini;
      //Angvector[i] = Av0 * Avector[i] / 2.0;
      Angvector[i] = Anginc*i+Angini;
      Avector[i] = 2.0 * sin(Angvector[i] * PI /180) / Av0;
      //Avector[i] = Ainc*i+Aini;
    }

    Vinc = (Vfin-Vini)/(Vint);
    Vvector = malloc(sizeof(float)*(Vint));
    Cvector = malloc(sizeof(float)*(Vint));
    for(i=0; i<Vint; i++){
      Vvector[i] = Vinc*i+Vini;
      Cvector[i] = 4/Vvector[i]*1/Vvector[i];
    }


    //Rodar o CRS para cada conjunto de tracos de mesmo cdp
    for(tracos=0; tracos<tamanhoLista; tracos++){
        if(listaTracos[tracos]->cdp == 230) break;
        //PrintTracoSU(listaTracos[tracos]->tracos[0]);

        //Copiar cabecalho do conjunto dos tracos para os tracos de saida
        memcpy(&tracoEmpilhado,listaTracos[tracos]->tracos[0],SEISMIC_UNIX_HEADER);
        //E necessario setar os conteudos de offset e coordenadas de fonte e receptores
        SetCabecalhoCRS(&tracoEmpilhado);
        memcpy(&tracoSemblance,&tracoEmpilhado,SEISMIC_UNIX_HEADER);
        memcpy(&tracoA,&tracoEmpilhado,SEISMIC_UNIX_HEADER);
        memcpy(&tracoB,&tracoEmpilhado,SEISMIC_UNIX_HEADER);
        memcpy(&tracoV,&tracoEmpilhado,SEISMIC_UNIX_HEADER);

        //Computar os vizinhos do cdp
        ComputarVizinhos(listaTracos,tamanhoLista,tracos,md,azimuth);
        //PrintVizinhosSU(listaTracos[tracos]);
        //getchar();
        printf("\t%d[%d|%d] (cdp= %d) de %d\n", tracos, listaTracos[tracos]->tamanho,listaTracos[tracos]->numeroVizinhos,
                            listaTracos[tracos]->cdp, tamanhoLista);
        //Execucao do CRS
        CRS(listaTracos[tracos],Avector,Aint,Angvector,Bpctg,Bint,Vvector,Vint,Cvector,wind,azimuth,&tracoEmpilhado,&tracoSemblance,&tracoA,&tracoB,&tracoV);

        float seg = ((float) listaTracos[tracos]->tracos[0]->dt)/1000000;
        int amostras = listaTracos[tracos]->tracos[0]->ns;
        float pilhaTemp;
        for(i=0; i<amostras ; i++){
            float t0 = i*seg;
            printf("\nCDP: %d amostra:%d\n", listaTracos[tracos]->cdp, i);
            float nA, nAng, nB, nC, nV, nS;
            nA = 0.0;//2.0*sin(listaA[tracos]->tracos[0]->dados[i]*PI/180) /Av0;
            nAng = 0.0;//listaA[tracos]->tracos[0]->dados[i];
            nB = 0.0;//listaB[tracos]->tracos[0]->dados[i];
            nC = 4/listaV[tracos]->tracos[0]->dados[i]/listaV[tracos]->tracos[0]->dados[i];
            nV = listaV[tracos]->tracos[0]->dados[i];
            nS = listaSemblance[tracos]->tracos[0]->dados[i];
            float mA, mAng, mB, mC, mV, mS;
            mA = 0.0;//2.0*sin(tracoA.dados[i]*PI/180)/Av0;
            mAng = 0.0;//tracoA.dados[i];
            mB = 0.0;//tracoB.dados[i];
            mC = 4/tracoV.dados[i]/tracoV.dados[i];
            mV = tracoV.dados[i];
            mS = tracoSemblance.dados[i];
            printf("\nA:%.20lf Angulo:%.20lf B:%.20lf C:%.20lf V:%.20lf\n", nA, nAng, nB, nC, nV);
            printf("A:%.20lf Angulo:%.20lf B:%.20lf C:%.20lf V:%.20lf\n\n", mA, mAng, mB, mC, mV);
            float s = Semblance(*listaTracos,nA,nB,nC,t0,wind,seg,&pilhaTemp,azimuth);
            float ms = Semblance(*listaTracos,mA,mB,mC,t0,wind,seg,&pilhaTemp,azimuth);
            float delta = nS - s;
            printf("%.20lf\n", delta);
            printf("%.20lf == %.20lf (%.20lf == %.20lf)\n", s, nS, mS, ms);
            if(listaSemblance[tracos]->tracos[0]->dados[i] > 0.5) getchar();
            //if(s == -1) getchar();
            //if( i == 17) getchar();
            getchar();
        }

        getchar();

        //Copiar os tracos resultantes nos arquivos de saida
        fwrite(&tracoEmpilhado,SEISMIC_UNIX_HEADER,1,arquivoEmpilhado);
        fwrite(&(tracoEmpilhado.dados[0]),sizeof(float),tracoEmpilhado.ns,arquivoEmpilhado);
        fwrite(&tracoSemblance,SEISMIC_UNIX_HEADER,1,arquivoSemblance);
        fwrite(&(tracoSemblance.dados[0]),sizeof(float),tracoSemblance.ns,arquivoSemblance);
        fwrite(&tracoA,SEISMIC_UNIX_HEADER,1,arquivoA);
        fwrite(&(tracoA.dados[0]),sizeof(float),tracoA.ns,arquivoA);
        fwrite(&tracoB,SEISMIC_UNIX_HEADER,1,arquivoB);
        fwrite(&(tracoB.dados[0]),sizeof(float),tracoB.ns,arquivoB);
        fwrite(&tracoV,SEISMIC_UNIX_HEADER,1,arquivoV);
        fwrite(&(tracoV.dados[0]),sizeof(float),tracoV.ns,arquivoV);
        //fwrite(&tracoC,SEISMIC_UNIX_HEADER,1,arquivoC);
        //fwrite(&(tracoC.dados[0]),sizeof(float),tracoC.ns,arquivoC);

        //Liberar memoria alocada nos dados do traco resultante
        free(tracoEmpilhado.dados);
        free(tracoSemblance.dados);
        free(tracoA.dados);
        free(tracoB.dados);
        free(tracoV.dados);
        //free(tracoC.dados);
        //printf("\t%d[%d] (cdp= %d) de %d\n", tracos, listaTracos[tracos]->tamanho, listaTracos[tracos]->cdp, tamanhoLista);
        //break;
    }

    fclose(arquivoEmpilhado);
    fclose(arquivoSemblance);
    fclose(arquivoA);
    fclose(arquivoB);
    fclose(arquivoV);
    //fclose(arquivoC);
    free(Vvector);
    free(Avector);
    free(Angvector);
    free(Cvector);

    LiberarMemoria(&listaTracos, &tamanhoLista);

    return 1;
}


void CRS(ListaTracos *lista, float *Avector, int Aint, float *Angvector,
          float Bpctg, int Bint,
          float *Vvector, int Vint,
          float *Cvector,
          float wind, float azimuth, Traco* tracoEmpilhado,
          Traco* tracoSemblance, Traco* tracoA, Traco* tracoB, Traco* tracoV)
{
    int amostra, amostras;
    float seg, t0;
    float bestAng[3], bestA[3];
    float bestB[3], iB, Bini, Bfin, Binc;
    float bestC[3];
    float bestV[3];
    float s, bestS[3];
    float pilha[3], pilhaTemp;
    int i;

    //Tempo entre amostras, convertido para segundos
    seg = ((float) lista->tracos[0]->dt)/1000000;

    //Numero de amostras
    amostras = lista->tracos[0]->ns;
    //Alocar memoria para os dados dos tracos resultantes
    tracoEmpilhado->dados = malloc(sizeof(float)*amostras);
    tracoSemblance->dados = malloc(sizeof(float)*amostras);
    tracoA->dados = malloc(sizeof(float)*amostras);
    tracoB->dados = malloc(sizeof(float)*amostras);
    tracoV->dados = malloc(sizeof(float)*amostras);
    //tracoC->dados = malloc(sizeof(float)*amostras);

    for(i=0; i<3; i++){
      bestAng[i] = 0.0;
      bestA[i] = 0.0;
      bestB[i] = 0.0;
      bestV[i] = Vvector[0];
      bestC[i] = Cvector[0];
      bestS[i] = 0.0;
    }

    //Para cada amostra do primeiro traco
#ifdef OMP_H
//#pragma omp parallel for private(bestA,bestB,bestC,A,B,C,bestS,pilha,pilhaTemp,t0,amostra,s) shared(tracoEmpilhado,tracoSemblance,tracoA,tracoB,tracoC,seg,wind,lista,amostras,Aini,Bini,Cini,Afin,Bfin,Cfin,Ainc,Binc,Cinc)
#pragma omp parallel for private(i,iB,pilhaTemp,t0,amostra,s,Bini) firstprivate(bestA,bestB,bestV,bestAng,bestC,bestS,pilha,seg,wind,amostras,Vint,Vvector,Cvector,Avector,Angvector,Aint,Bint,Bpctg,azimuth) shared(tracoEmpilhado,tracoSemblance,tracoA,tracoB,tracoV,lista)
#endif
    for(amostra=0; amostra<amostras; amostra++){
        //Calcula o segundo inicial
        t0 = amostra*seg;

        //Inicializar variaveis antes da busca
        pilha[0] = lista->tracos[0]->dados[amostra];
        pilha[1] = lista->tracos[0]->dados[amostra];
        pilha[2] = lista->tracos[0]->dados[amostra];

        //Para cada constante A, B e C
        for(i=0; i<Vint; i++){
            //Calcular semblance
            pilhaTemp = 0.0;
            s = Semblance(lista,0.0,0.0,Cvector[i],t0,wind,seg,&pilhaTemp,azimuth);
            if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
            if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
            else if(s > bestS[0]){
                bestS[0] = s;
                bestC[0] = Cvector[i];
                bestV[0] = Vvector[i];
                pilha[0] = pilhaTemp;
            }
        }
        bestC[1] = bestC[0];
        bestC[2] = bestC[0];
        bestV[1] = bestV[0];
        bestV[2] = bestV[0];

        for(i=0; i<Aint; i++){
            //Calcular semblance
            pilhaTemp = 0.0;
            s = Semblance(lista,Avector[i],0.0,bestC[1],t0,wind,seg,&pilhaTemp,azimuth);
            if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
            if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
            else if(s > bestS[1]){
                bestS[1] = s;
                bestAng[1] = Angvector[i];
                bestA[1] = Avector[i];
                pilha[1] = pilhaTemp;
            }
        }
        bestAng[2] = bestAng[1];
        bestA[2] = bestA[1];

        Bfin = Bpctg * bestC[2];
        Bini = Bfin / (Bint*Bint/2);
        pilhaTemp = 0.0;
        s = Semblance(lista,bestA[2],0.0,bestC[2],t0,wind,seg,&pilhaTemp,azimuth);
        if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
        if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
        else if(s > bestS[2]){
            bestS[2] = s;
            bestB[2] = 0.0;
            pilha[2] = pilhaTemp;
        }

        for(i=0, iB=Bfin; i<=Bint/2; i++, iB=iB*0.8){
            //Calcular semblance
            pilhaTemp = 0.0;
            s = Semblance(lista,bestA[2],iB,bestC[2],t0,wind,seg,&pilhaTemp,azimuth);
            if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
            if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
            else if(s > bestS[2]){
                bestS[2] = s;
                bestB[2] = iB;
                pilha[2] = pilhaTemp;
            }
            pilhaTemp = 0.0;

            s = Semblance(lista,bestA[2],-iB,bestC[2],t0,wind,seg,&pilhaTemp,azimuth);
            if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
            if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
            else if(s > bestS[2]){
                bestS[2] = s;
                bestB[2] = -iB;
                pilha[2] = pilhaTemp;
            }
        }

        tracoEmpilhado->dados[amostra] = pilha[2];
        if(bestS[2] == -1) bestS[2] = 0;
        tracoSemblance->dados[amostra] = bestS[2];
        tracoA->dados[amostra] = bestAng[2];
        tracoB->dados[amostra] = bestB[2];
        tracoV->dados[amostra] = bestV[2];
    }
    printf("\n");
}

void SetCabecalhoCRS(Traco *traco)
{
    float mx, my;
    MidpointSU(traco,&mx,&my);
    //Offset zerado pois receptores e fonte estao na mesma coordenada
    traco->offset = 0;
    //Fonte e receptores na mesma coordenada
    traco->sx = (int) mx;
    traco->sy = (int) my;
    traco->gx = (int) mx;
    traco->gy = (int) my;
}

void LiberarMemoria(ListaTracos ***lista, int *tamanho)
{
    LiberarMemoriaSU(lista,tamanho);
}
