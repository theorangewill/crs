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




/*
 * Algoritmo CRS.
 */
void CRS(ListaTracos *lista, float Aini, float Afin, int Aint,
          float Bini, float Bfin, int Bint,
          float Vini, float Vfin, int Vint, 
          //float Cini, float Cfin, float Cint, 
          float wind, Traco* tracoEmpilhado,
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
    float Aini, Afin;
    int Aint;
    float Bini, Bfin;
    int Bint;
    float Vini, Vfin;
    int Vint;
    float Cini, Cfin;
    int Cint;
    float wind, md, aph;
    int tracos;
    char saida[101], saidaEmpilhado[104], saidaSemblance[104], saidaA[104], saidaB[104], saidaC[104], saidaV[104];
    FILE *arquivoEmpilhado, *arquivoSemblance, *arquivoA, *arquivoB, *arquivoC, *arquivoV;
    Traco tracoSemblance, tracoA, tracoB, tracoC, tracoEmpilhado, tracoV;

    if(argc < 14){
        printf("ERRO: ./main <dado sismico> A_INI A_FIN A_INT B_INI B_FIN B_INT C_INI C_FIN C_INT MD WIND APH\n");
        printf("\tARQUIVO: arquivo dos tracos sismicos\n");
        printf("\tA_INI:  constante A inicial\n");
        printf("\tA_FIN:  constante A final\n");
        printf("\tA_INT:    quantidade de A avaliados\n");
        printf("\tB_INI:  constante B inicial\n");
        printf("\tB_FIN:  constante B final\n");
        printf("\tB_INT:    quantidade de B avaliados\n");
        printf("\tVEL_INI:  constante C inicial\n");
        printf("\tVEL_FIN:  constante C final\n");
        printf("\tVEL_INT:    quantidade de C avaliados\n");
        printf("\tMD:    distancia dos vizinhos\n");
        printf("\tWIND:    janela do semblance (em amostras)\n");
        printf("\tAPH:    aperture\n");
        exit(1);
    }

    //Leitura dos parametros
    Aini = atof(argv[2]);
    Afin = atof(argv[3]);
    Aint = atoi(argv[4]);
    Bini = atof(argv[5]);
    Bfin = atof(argv[6]);
    Bint = atoi(argv[7]);
    Vini = atof(argv[8]);
    Vfin = atof(argv[9]);
    Vint = atoi(argv[10]);
    md = atof(argv[11]);
    wind = atof(argv[12]);
    aph = atof(argv[13]);

    //printf("A=%.20lf %.20lf %.20lf B=%.20lf %.20lf %.20lf C=%.20lf %.20lf %.20lf %.20lf %.20lf %.20lf\n", Aini, Afin, Ainc, Bini, Bfin, Binc, Cini, Cfin, Cinc, md, wind, aph);

    //Leitura do arquivo
    if(!LeitorArquivoSU(argv[1], &listaTracos, &tamanhoLista, aph)){
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
    strcpy(saida,argv[1]);
    strcpy(saidaEmpilhado,saida);
    strcat(saidaEmpilhado,"-empilhado.out4.su");
    arquivoEmpilhado = fopen(saidaEmpilhado,"w");
    strcpy(saidaSemblance,saida);
    strcat(saidaSemblance,"-semblance.out4.su");
    arquivoSemblance = fopen(saidaSemblance,"w");
    strcpy(saidaA,saida);
    strcat(saidaA,"-A.out4.su");
    arquivoA = fopen(saidaA,"w");
    strcpy(saidaB,saida);
    strcat(saidaB,"-B.out4.su");
    arquivoB = fopen(saidaB,"w");
    /*
    strcpy(saidaC,saida);
    strcat(saidaC,"-C.out4.su");
    arquivoC = fopen(saidaC,"w");
    */
    strcpy(saidaV,saida);
    strcat(saidaV,"-V.out4.su");
    arquivoV = fopen(saidaV,"w");

#ifdef OMP_H
    omp_set_num_threads(NUM_THREADS);
#endif
    //Rodar o CRS para cada conjunto de tracos de mesmo cdp
    for(tracos=0; tracos<tamanhoLista; tracos++){
      if(listaTracos[tracos]->cdp != 250) continue;
        //PrintTracoSU(listaTracos[tracos]->tracos[0]);

        //Copiar cabecalho do conjunto dos tracos para os tracos de saida
        memcpy(&tracoEmpilhado,listaTracos[tracos]->tracos[0],SEISMIC_UNIX_HEADER);
        //E necessario setar os conteudos de offset e coordenadas de fonte e receptores
        SetCabecalhoCRS(&tracoEmpilhado);
        memcpy(&tracoSemblance,&tracoEmpilhado,SEISMIC_UNIX_HEADER);
        memcpy(&tracoA,&tracoEmpilhado,SEISMIC_UNIX_HEADER);
        memcpy(&tracoB,&tracoEmpilhado,SEISMIC_UNIX_HEADER);
        memcpy(&tracoV,&tracoEmpilhado,SEISMIC_UNIX_HEADER);
        //memcpy(&tracoC,&tracoEmpilhado,SEISMIC_UNIX_HEADER);

        //Computar os vizinhos do cdp
        ComputarVizinhos(listaTracos,tamanhoLista,tracos,md);
        //PrintVizinhosSU(listaTracos[tracos]);
        //getchar();
        printf("\t%d[%d|%d] (cdp= %d) de %d\n", tracos, listaTracos[tracos]->tamanho,listaTracos[tracos]->numeroVizinhos, 
                            listaTracos[tracos]->cdp, tamanhoLista);
        //Execucao do CRS
        CRS(listaTracos[tracos],Aini,Afin,Aint,Bini,Bfin,Bint,Vini,Vfin,Vint,wind,&tracoEmpilhado,&tracoSemblance,&tracoA,&tracoB,&tracoV);

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

    LiberarMemoria(&listaTracos, &tamanhoLista);

    return 1;
}


void CRS(ListaTracos *lista, float Aini, float Afin, int Aint,
            float Bini, float Bfin, int Bint,
            float Vini, float Vfin, int Vint, 
            // float Cini, float Cfin, int Cint, 
             float wind, Traco* tracoEmpilhado, Traco* tracoSemblance, Traco* tracoA, Traco* tracoB, Traco* tracoV)
{
    int amostra, amostras;
    float seg, t0;
    float A, bestA, Ainc;
    float B, bestB, Binc;
    double C, bestC, Cinc;
    float V, bestV, Vinc;
    float s, bestS;
    float pilha, pilhaTemp;
    float *Avector, *Bvector;
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

    Ainc = (Afin-Aini)/(Aint-1);
    Avector = malloc(sizeof(float)*(Aint));
    for(i=0; i<Aint; i++) Avector[i] = Ainc*i+Aini;
    //for(i=0; i<Aint; i++) printf(" %.10lf ", Avector[i]);
    //printf("\n\tAini=%.10lf Afin=%.10lf Aint=%d Ainc=%.10lf\n", Aini, Afin, Aint, Ainc);
 
    Binc = (Bfin-Bini)/Bint;
    Vinc = (Vfin-Vini)/Vint;
    //Cinc = (Cfin-Cini)/Cinc;

    //Para cada amostra do primeiro traco
#ifdef OMP_H
//#pragma omp parallel for private(bestA,bestB,bestC,A,B,C,bestS,pilha,pilhaTemp,t0,amostra,s) shared(tracoEmpilhado,tracoSemblance,tracoA,tracoB,tracoC,seg,wind,lista,amostras,Aini,Bini,Cini,Afin,Bfin,Cfin,Ainc,Binc,Cinc)
//#pragma omp parallel for private(bestA,bestB,bestV,A,B,V,bestS,pilha,pilhaTemp,t0,amostra,s) shared(tracoEmpilhado,tracoSemblance,tracoA,tracoB,tracoV,seg,wind,lista,amostras,Aini,Bini,Vini,Afin,Bfin,Vfin,Ainc,Binc,Vinc)
#endif
    //for(amostra=0; amostra<amostras; amostra++){
    for(amostra=491; amostra<525; amostra++){
        //printf("%d\n", amostra);
        //Calcula o segundo inicial
        t0 = amostra*seg;

        //Inicializar variaveis antes da busca
        pilha = lista->tracos[0]->dados[amostra];

        bestA = Aini;
        bestB = Bini;
        bestV = Vini;
        bestC = 4/(Vini)*4/Vini;
        bestS = 0;

        //Para cada constante A, B e C
        for(V=Vini; V<=Vfin; V+=Vinc){
            //Calcular semblance
            pilhaTemp = 0;
            C = 4/(V*V);
            s = Semblance(lista,0,0,C,t0,wind,seg,&pilhaTemp);
            //if(s == -1){ printf("%d %d %d    ", na, nb, nc); nd++;}
            if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
            if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
            else if(s > bestS){
                bestS = s;
                bestC = C;
                bestV = V;
                pilha = pilhaTemp;
            }
        }

        /*
        //if(bestS >= 0.9) printf("%d depois do C\n", amostra);
        bestS = 0;
        //for(A=Aini; A<=Afin; A+=Ainc){
        for(i=0; i<Aint+1; i++){
            //Calcular semblance
            pilhaTemp = 0;
            s = Semblance(lista,Avector[i],0,bestC,t0,wind,seg,&pilhaTemp);
            //printf("s=%.10lf A=%.10lf\n", s, Avector[i]);
            //if(s == -1){ printf("%d %d %d    ", na, nb, nc); nd++;}
            if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
            if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
            else if(s > bestS){
                bestS = s;
                bestA = Avector[i];
                pilha = pilhaTemp;
            }
        }
        //printf(">> bestS %.10f bestA %.10lf\n", bestS, bestA);
        //getchar();
        //printf("A %.20f\n", bestS);
        //if(bestS >= 0.9) printf("%d depois do A\n", amostra);

        bestS = 0;
        for(B=Bini; B<=Bfin; B+=Binc){
            //Calcular semblance
            pilhaTemp = 0;
            s = Semblance(lista,bestA,B,bestC,t0,wind,seg,&pilhaTemp);
            //if(s == -1){ printf("%d %d %d    ", na, nb, nc); nd++;}
            if(s<0 && s!=-1) {printf("S NEGATIVO\n"); exit(1);}
            if(s>1) {printf("S MAIOR Q UM %.20f\n", s); exit(1);}
            else if(s > bestS){
                bestS = s;
                bestB = B;
                pilha = pilhaTemp;
            }
        }
        //printf("B %.20f\n", bestS);
        //if(bestS >= 0.9) printf("%d depois do B\n", amostra);

        */

        //if(nd > 0)   getchar();
        tracoEmpilhado->dados[amostra] = pilha;
        tracoSemblance->dados[amostra] = bestS;
        tracoA->dados[amostra] = bestA;
        tracoB->dados[amostra] = bestB;
        tracoV->dados[amostra] = bestV;
        //tracoC->dados[amostra] = bestC;
        printf("%.10lf %.10lf %.10lf\n", bestV, bestS, pilha);
        //printf("%d S=%.10f A=%.20f B=%.20f V=%.20f Pilha=%.10f\n", amostra, bestS, bestA, bestB, bestV, pilha);
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
