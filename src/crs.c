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
    float Angini, Angfin, Anginc;
    float Aini, Afin, Ainc;
    int Vint;
    float wind, md, aph;
    int tracos;
    int i, zero;
    float *Avector, *Angvector, *Vvector, *Cvector;
    char saida[101], saidaEmpilhado[104], saidaSemblance[104], saidaA[104], saidaB[104], saidaV[104];
    FILE *arquivoEmpilhado, *arquivoSemblance, *arquivoA, *arquivoB, *arquivoV;
    Traco tracoSemblance, tracoA, tracoB, tracoEmpilhado, tracoV;
    float azimuth;

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

    ListaTracos **listaA, **listaB, **listaV, **listaSemblance;
    int tamA, tamB, tamV, tamSemblance;

    if(!LeitorArquivoSU(argv[1], &listaTracos, &tamanhoLista, aph)){
        printf("ERRO NA LEITURA\n");
        exit(1);
    }

    if(!LeitorArquivoSU(argv[14], &listaA, &tamA, aph)){
        printf("ERRO NA LEITURA A\n");
        exit(1);
    }

    if(!LeitorArquivoSU(argv[15], &listaB, &tamB, aph)){
        printf("ERRO NA LEITURA B\n");
        exit(1);
    }

    if(!LeitorArquivoSU(argv[16], &listaV, &tamV, aph)){
        printf("ERRO NA LEITURA V\n");
        exit(1);
    }

    if(!LeitorArquivoSU(argv[17], &listaSemblance, &tamSemblance, aph)){
        printf("ERRO NA LEITURA Semblance\n");
        exit(1);
    }

    float pilhaTemp;
/*
    for(int traco=0; traco<tamanhoLista; traco++){
        float seg = ((float) listaTracos[traco]->tracos[0]->dt)/1000000;
        int amostras = listaTracos[traco]->tracos[0]->ns;
        for(i=0; i<amostras ; i++){
            float t0 = i*seg;
            printf("%.20lf %.20lf %.20lf %.20lf %.20lf: ", 2.0*sin(listaA[traco]->tracos[0]->dados[i]*PI/180) /Av0, 
             listaA[traco]->tracos[0]->dados[i], listaB[traco]->tracos[0]->dados[i],
             4/listaV[traco]->tracos[0]->dados[i]/listaV[traco]->tracos[0]->dados[i], listaV[traco]->tracos[0]->dados[i]);
            float s = Semblance(*listaTracos,2.0*sin(listaA[traco]->tracos[0]->dados[i]*PI/180) /Av0,listaB[traco]->tracos[0]->dados[i],4/listaV[traco]->tracos[0]->dados[i]/listaV[traco]->tracos[0]->dados[i],t0,wind,seg,&pilhaTemp,azimuth);
            printf("%.20lf == %.20lf\n", s, listaSemblance[traco]->tracos[0]->dados[i]);
            getchar();
        }
    }

    exit(1);*/

/*
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
        printf("\tWIND:    janela do semblance\n");
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

    //Leitura do arquivo
    if(!LeitorArquivoSU(argv[1], &listaTracos, &tamanhoLista, aph)){
        printf("ERRO NA LEITURA\n");
        exit(1);
    }*/

    //Criacao dos arquivos de saida
    argv[1][strlen(argv[1])-3] = '\0';
    strcpy(saida,argv[1]);
    strcpy(saidaEmpilhado,saida);
    strcat(saidaEmpilhado,"-empilhado.out3.su");
    arquivoEmpilhado = fopen(saidaEmpilhado,"w");
    strcpy(saidaSemblance,saida);
    strcat(saidaSemblance,"-semblance.out3.su");
    arquivoSemblance = fopen(saidaSemblance,"w");
    strcpy(saidaA,saida);
    strcat(saidaA,"-A.out3.su");
    arquivoA = fopen(saidaA,"w");
    strcpy(saidaB,saida);
    strcat(saidaB,"-B.out3.su");
    arquivoB = fopen(saidaB,"w");
    strcpy(saidaV,saida);
    strcat(saidaV,"-V.out3.su");
    arquivoV = fopen(saidaV,"w");


#ifdef OMP_H
    omp_set_num_threads(NUM_THREADS);
#endif

    //Calcular os valores de busca para A, V e C
    Angini = -Aang;
    Angfin = Aang;
    Anginc = 2.0 * Angfin / (Aint);
    Aini = 2.0 * sin(Angini * PI / 180) / Av0;
    Afin = 2.0 * sin(Angfin * PI / 180) / Av0;
    Ainc = 2.0 * Afin / (Aint);

    Avector = malloc(sizeof(float)*(Aint+1));
    Angvector = malloc(sizeof(float)*(Aint+1));
    zero = 0;
    for(i=0; i<Aint; i++){
      Angvector[i] = Anginc*i+Angini;
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
        ComputarVizinhos(listaTracos,tamanhoLista,tracos,md);
        //PrintVizinhosSU(listaTracos[tracos]);

        printf("\t%d[%d|%d] (cdp= %d) de %d\n", tracos, listaTracos[tracos]->tamanho,listaTracos[tracos]->numeroVizinhos,
                            listaTracos[tracos]->cdp, tamanhoLista);
        //Execucao do CRS
        CRS(listaTracos[tracos],Avector,Aint,Angvector,Bpctg,Bint,Vvector,Vint,Cvector,wind,azimuth,&tracoEmpilhado,&tracoSemblance,&tracoA,&tracoB,&tracoV);

        float seg = ((float) listaTracos[tracos]->tracos[0]->dt)/1000000;
        int amostras = listaTracos[tracos]->tracos[0]->ns;
        for(i=0; i<amostras ; i++){
            float t0 = i*seg;
            printf("\nA:%.20lf Angulo:%.20lf B:%.20lf C:%.20lf V:%.20lf\n", 2.0*sin(listaA[tracos]->tracos[0]->dados[i]*PI/180) /Av0, 
             listaA[tracos]->tracos[0]->dados[i], listaB[tracos]->tracos[0]->dados[i],
             4/listaV[tracos]->tracos[0]->dados[i]/listaV[tracos]->tracos[0]->dados[i], listaV[tracos]->tracos[0]->dados[i]);
            printf("A:%.20lf Angulo:%.20lf B:%.20lf C:%.20lf V:%.20lf\n", 2.0*sin(tracoA.dados[i]*PI/180)/Av0, tracoA.dados[i], tracoB.dados[i],
                        4/tracoV.dados[i]/tracoV.dados[i], tracoV.dados[i]);
            float s = Semblance(*listaTracos,2.0*sin(listaA[tracos]->tracos[0]->dados[i]*PI/180) /Av0,listaB[tracos]->tracos[0]->dados[i],4/listaV[tracos]->tracos[0]->dados[i]/listaV[tracos]->tracos[0]->dados[i],t0,wind,seg,&pilhaTemp,azimuth);
            printf("%.20lf == %.20lf (%.20lf)\n", s, listaSemblance[tracos]->tracos[0]->dados[i], tracoSemblance.dados[i]);
            getchar();
        }



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

    printf("SALVO NOS ARQUIVOS:\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n",saidaEmpilhado,saidaSemblance,saidaV,saidaA,saidaB);
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
#pragma omp parallel for firstprivate(lista,amostras,Vint,Aint,Bpctg,Bint,seg,wind,azimuth,Cvector,Vvector,Avector) private(bestA,bestB,bestAng,bestV,bestC,bestS,Bini,Bfin,B,i,pilha,pilhaTemp,t0,amostra,s)  shared(tracoEmpilhado,tracoSemblance,tracoA,tracoB,tracoV)
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

        //Para cada constante C
        for(i=0; i<Vint; i++){
            //Calcular semblance
            pilhaTemp = 0;
            s = SemblanceCMP(lista,0.0,0.0,Cvector[i],t0,wind,seg,&pilhaTemp,azimuth);
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
        //Para cada constante A
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
        //Para cada constante B
        for(i=0, B=Bfin; i<=Bint; i++, B=B*0.8){
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
                bestB = B;
                pilha = pilhaTemp;
            }
        }

        tracoEmpilhado->dados[amostra] = pilha;
        if(bestS == -1) bestS = 0;
        tracoSemblance->dados[amostra] = bestS;
        tracoA->dados[amostra] = bestAng;
        tracoB->dados[amostra] = bestB;
        tracoV->dados[amostra] = bestV;
        //tracoC->dados[amostra] = bestC;
    }
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
