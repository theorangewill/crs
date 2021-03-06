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
    if(!LeitorArquivoSU(argv[1], &listaTracos, &tamanhoLista, aph, azimuth)){
        printf("ERRO NA LEITURA\n");
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
    strcpy(saida,"results");
    for(i=strlen(argv[1])-1; argv[1][i] != '/' && i > 0; i--);
    strcat(saida,&(argv[1][i]));
    
    strcpy(saidaEmpilhado,saida);
    strcat(saidaEmpilhado,"-empilhado.outnv.su");
    arquivoEmpilhado = fopen(saidaEmpilhado,"w");
	if(arquivoEmpilhado == NULL){
        printf("ERRO NA ABERTURA DO ARQUIVO EMPILHADO\n");
		return 0;
	}
	
    strcpy(saidaSemblance,saida);
    strcat(saidaSemblance,"-semblance.outnv.su");
    arquivoSemblance = fopen(saidaSemblance,"w");
	if(arquivoSemblance == NULL){
        printf("ERRO NA ABERTURA DO ARQUIVO SEMBLANCE\n");
		return 0;
	}
	
    strcpy(saidaA,saida);
    strcat(saidaA,"-A.outnv.su");
    arquivoA = fopen(saidaA,"w");
	if(arquivoA == NULL){
        printf("ERRO NA ABERTURA DO ARQUIVO A\n");
		return 0;
	}
	
    strcpy(saidaB,saida);
    strcat(saidaB,"-B.outnv.su");
    arquivoB = fopen(saidaB,"w");
	if(arquivoB == NULL){
        printf("ERRO NA ABERTURA DO ARQUIVO B\n");
		return 0;
	}
	
    strcpy(saidaV,saida);
    strcat(saidaV,"-V.outnv.su");
    arquivoV = fopen(saidaV,"w");
	if(arquivoV == NULL){
        printf("ERRO NA ABERTURA DO ARQUIVO V\n");
		return 0;
	}


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
    zero = 0;
    for(i=0; i<Aint; i++){
      //Avector[i] = Ainc*i+Aini;
      //Angvector[i] = Av0 * Avector[i] / 2.0;
      Angvector[i] = Anginc*i+Angini;
      //Avector[i] = 2.0 * sin(Angvector[i] * PI /180) / Av0;
      Avector[i] = Ainc*i+Aini;
      if(Avector[i] == 0.0) zero = 1;
    }

    if(zero == 0){
      Avector[Aint] = 0.0;
      Angvector[Aint] = 0.0;
      Aint++;
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
        //if(listaTracos[tracos]->cdp == 230) break;
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
    float bestAng;
    float bestB, B, Bini, Bfin, Binc, bestA;
    float bestC;
    float bestV;
    float s, bestS;
    float pilha, pilhaTemp;
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

    //Para cada amostra do primeiro traco
#ifdef OMP_H
#pragma omp parallel for schedule(static) firstprivate(lista,amostras,Aint,Vint,Bint,Bpctg,seg,wind,azimuth,Avector,Angvector,Cvector,Vvector) private(bestA,bestB,bestAng,bestC,bestV,bestS,Bini,Bfin,i,pilha,pilhaTemp,t0,amostra,s)  shared(tracoEmpilhado,tracoSemblance,tracoV,tracoB,tracoA)
#endif
    for(amostra=0; amostra<amostras; amostra++){
        //Calcula o segundo inicial
        t0 = amostra*seg;

        //Inicializar variaveis antes da busca
        pilha = lista->tracos[0]->dados[amostra];
        bestA = 0.0;
        bestB = 0.0;
        bestAng = 0.0;
        bestV = Vvector[0];
        bestC = Cvector[0];
        bestS = -1;

        //Para cada constante A, B e C
        for(i=0; i<Vint; i++){
            //Calcular semblance
            pilhaTemp = 0;
            s = Semblance(lista,0.0,0.0,Cvector[i],t0,wind,seg,&pilhaTemp,azimuth);
            if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
            if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
            else if(s > bestS){
                bestS = s;
                bestC = Cvector[i];
                bestV = Vvector[i];
                pilha = pilhaTemp;
            }
        }
        
        bestS = -1;
        for(i=0; i<Aint; i++){
            //Calcular semblance
            pilhaTemp = 0;
            s = Semblance(lista,Avector[i],0.0,bestC,t0,wind,seg,&pilhaTemp,azimuth);
            if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
            if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
            else if(s > bestS){
                bestS = s;
                bestAng = Angvector[i];
                bestA = Avector[i];
                pilha = pilhaTemp;
            }
        }
        
        bestS = -1;
        Bfin = Bpctg * bestC;
        Bini = Bfin / (Bint*Bint/2);
        pilhaTemp = 0.0;
        s = Semblance(lista,bestA,0.0,bestC,t0,wind,seg,&pilhaTemp,azimuth);
        if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
        if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
        else if(s > bestS){
            bestS = s;
            bestB = 0.0;
            pilha = pilhaTemp;
        }
        
        for(i=0, B=Bfin; i<=Bint/2; i++, B=B*0.8){
            //Calcular semblance
            pilhaTemp = 0;
            s = Semblance(lista,bestA,B,bestC,t0,wind,seg,&pilhaTemp,azimuth);
            if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
            if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
            else if(s > bestS){
                bestS = s;
                bestB = B;
                pilha = pilhaTemp;
            }
            pilhaTemp = 0;

            s = Semblance(lista,bestA,-B,bestC,t0,wind,seg,&pilhaTemp,azimuth);
            if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
            if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
            else if(s > bestS){
                bestS = s;
                bestB = -B;
                pilha = pilhaTemp;
            }
        }

        tracoEmpilhado->dados[amostra] = pilha;
        if(bestS == -1) bestS = 0;
        tracoSemblance->dados[amostra] = bestS;
        tracoA->dados[amostra] = bestAng;
        tracoB->dados[amostra] = bestB;
        tracoV->dados[amostra] = bestV;
        //printf("%d(%d) S:%lf Ang:%lf B:%lf V:%lf\n", amostra, lista->tracos[0]->cdp, bestS, bestAng, bestB, bestV);
        //getchar();
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
