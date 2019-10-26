#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cmath>

using namespace std;

struct Elemento
{
    char *chave;
    char *valor;
};

// Saber qual coluna está a chave e o valor
void saberQualColunaEstaChaveEValor(
    ifstream &file, char *chave, char* valor,int &posChave, int &posValor
)
{
    string buffer;
    getline(file,buffer);
    char *linha = (char*)buffer.c_str();
    char *coluna = strtok(linha,",");
    int cont = 0;
    while (coluna!=NULL)
    {
        if(strcmp(coluna,chave)==0)  posChave = cont;
        if(strcmp(coluna,valor)==0)  posValor = cont;
        coluna = strtok(NULL,",");
        cont++;
    }
}
int particiona(Elemento *&v, int beg, int end, int pivo){
    char *valorPivo = v[pivo].chave;
    //colocamos o pivo temporariamente na ultima posição
    swap(v[pivo], v[end - 1]);
    // ao acharmos um elemento menor do que o pivo, vamos coloca-lo
    // na posicao "pos"
    int pos = beg;
    for (int i = beg; i < end - 1; i++){
        if (strcmp(v[i].chave, valorPivo) < 0){
            swap(v[pos], v[i]);
            pos++;
        }
    }
    //coloque o pivo depois do ultimo elemento menor que ele
    swap(v[pos], v[end - 1]);
    return pos;
}

void quickSort2(Elemento *&v, int beg, int end){
    if (beg == end)
        return;
    int pos = particiona(v, beg, end, beg);
    quickSort2(v, beg, pos);
    quickSort2(v, pos + 1, end);
}

void quickSort(Elemento *&v, int n){
    quickSort2(v, 0, n);
}

void obterElemento2(char*linha,int posChave, int posValor, Elemento &elemento){
    char *coluna = strtok(linha,",");
    int maior = posChave>posValor?posChave:posValor;
    for(int i=0;i<maior+1;i++){
        if(i==posChave){
            elemento.chave = strdup(coluna);
        }
        if(i==posValor){
            elemento.valor = strdup(coluna);
        }
        coluna = strtok(NULL,",");
    }
    free(coluna);
    free(linha);   
}

void obterElemento(char*&linha, int posChave, int posValor, Elemento &elemento){
    int maior = posChave>posValor?posChave:posValor;
    char* s = strdup(linha);
    char* coluna = strtok( s, ",");
    int i=0;
    while (coluna)
    {
        if(i==posChave){
            elemento.chave = strdup(coluna);
        }
        if(i==posValor){
            elemento.valor = strdup(coluna);
        }
        coluna = strtok( NULL,",");
        i++;
    }
    free(s);

}

int ordenarESepararEmArquivos(ifstream &file,int n, int posChave, int posValor){
    string buffer;
    char*linha;
    char*chave;
    char*valor;
    int nLinhas=0;
    Elemento *elementos = new Elemento[n];
    int cont=0;
    
    ofstream fout;
    while(getline(file,buffer)){
        linha = strdup(buffer.c_str());
        int aux = nLinhas%n;
        
        obterElemento(linha,posChave,posValor,elementos[aux]);
        
        if(aux==n-1|| file.peek()==-1){
            // Ordena vetor
            quickSort(elementos,aux);
            
            //
            char *nomeArquivo = new char[20];
            sprintf(nomeArquivo,"%d.txt",nLinhas/n);
            fout.open(nomeArquivo);
            for(int i=0;i<aux+1;i++){
                fout<<elementos[i].chave<<",";
                fout<<elementos[i].valor<<endl;
                free(elementos[i].chave);
                free(elementos[i].valor);
            }
            fout.close();
            delete[] elementos;
            delete[] nomeArquivo;
            elementos = new Elemento[n];
        }
        
        
        nLinhas++;
        free(linha);
    }
    
    delete[]elementos;
    return nLinhas;
}

void copiaElemento(Elemento &dest, Elemento &source){
    dest.chave = source.chave;
    dest.valor = source.valor;
}

void acharMenor(Elemento* &elementos,int n, int &posMenor, Elemento* &menor){
    menor = &elementos[0];
    for(int i=1;i<n;i++){
        // cout<<elementos[i].chave<<" "<<menor->chave<<endl;
        if(strcmp(elementos[i].chave,menor->chave)<0){
            menor = &elementos[i];
            posMenor = i;
        }
    }
}

void ordenacaoExterna(int nLinhasArquivos, int n){
    
    int nDispositivos = nLinhasArquivos/n+1;
    string buffer;
    
    ifstream* dispositivos = new ifstream[nDispositivos];
    Elemento* elementos = new Elemento[nDispositivos];
    char*linha;
    Elemento* menor;
    int posmenor;
    char* maiorValor = new char[2];
    maiorValor[0]=(char)255;
    maiorValor[1]='\0';
    // Abre os arquivos e copia para um vetor auxiliar a primeira linha de cada arquivo
    for(int i=0;i<nDispositivos;i++){
        // Abre os arquivos
        char *nome = new char[20];
        sprintf(nome, "%d.txt", i);
        dispositivos[i].open(nome);
        delete[]nome;

        // 
        getline(dispositivos[i],buffer);
        linha = (char *)buffer.c_str();
        obterElemento(linha,0,1,elementos[i]);
    }
    
    for(int i=0;i<nLinhasArquivos;i++){
        acharMenor(elementos,nDispositivos,posmenor,menor);
        cout<<menor->chave<<","<<menor->valor<<endl;
        free(menor->chave);
        free(menor->valor);
        getline(dispositivos[posmenor],buffer);
        linha = strdup(buffer.c_str());
        if(strlen(linha)==0){
            menor->chave = maiorValor;
            menor->valor = maiorValor;
        }else{            
            obterElemento(linha,0,1,*menor);
        }
        free(linha);
    }
    delete[]maiorValor;
    
    delete[]dispositivos;
    delete[]elementos;
   
}

// Salvar arquivos ordenados

int main(int argc, char **argv){
    char *fileIn = argv[1];
    int n = atoi(argv[2]);
    const char *chave = argv[3];
    const char *valor = argv[4];
    ifstream file(fileIn);
    int posChave,posValor;
    saberQualColunaEstaChaveEValor(file,(char*)chave,(char*)valor,posChave,posValor);
    int nLinhas = ordenarESepararEmArquivos(file,n,posChave,posValor);
    ordenacaoExterna(nLinhas,n);
    file.close();
    return 0;
}