#include "ListaTempoProgramado.h"
#include "string.h"
ListaTempoProgramado::ListaTempoProgramado(){
    inicio = nullptr;
    fim = nullptr;
}
void ListaTempoProgramado::addItem(char *dataProg,char *horaProg,bool repetir,bool ligar,int elemento,char *nomeElemento){
    TempoProgramado *temp = new TempoProgramado;
    strcpy(temp->dataProg,dataProg);
    strcpy(temp->horaProg,horaProg);
    strcpy(temp->nomeElemento,nomeElemento);
    temp->repetir = repetir;
    temp->ligar = ligar;
    temp->elemento = elemento;
    temp->proximo = nullptr;
    if(inicio==nullptr){
        inicio = temp;
        fim = temp;
    }else{
        fim->proximo = temp;
        fim = temp;
    }
}

void ListaTempoProgramado::addItem(char *dataProg,char *horaProg,bool repetir,bool ligar,int elemento,char *nomeElemento,char *diasRepetir){
    TempoProgramado *temp = new TempoProgramado;
    strcpy(temp->dataProg,dataProg);
    strcpy(temp->horaProg,horaProg);
    strcpy(temp->nomeElemento,nomeElemento);
    strcpy(temp->diasRepetir,diasRepetir);
    temp->repetir = repetir;
    temp->ligar = ligar;
    temp->elemento = elemento;
    temp->proximo = nullptr;
    if(inicio==nullptr){
        inicio = temp;
        fim = temp;
    }else{
        fim->proximo = temp;
        fim = temp;
    }
}

void ListaTempoProgramado::addItem(TempoProgramado *prog){
    if(inicio==nullptr){
        inicio = prog;
        fim = prog;
    }else{
        fim->proximo = prog;
        fim = prog;
    }
}

TempoProgramado* ListaTempoProgramado::buscar(char *horaProg){
        TempoProgramado *prog = nullptr;
        if(inicio!=nullptr){
            prog = inicio;
            do{
                if(strcmp(prog->horaProg,horaProg))
                    return prog;
                else
                    prog = prog->proximo;
            }while(prog!=nullptr);
        }
        return prog;
    }
TempoProgramado* ListaTempoProgramado::buscar(short elemento){
        TempoProgramado *prog = nullptr;
        if(inicio!=nullptr){
            prog = inicio;
            do{
                if(prog->elemento==elemento)
                    return prog;
                else
                    prog = prog->proximo;
            }while(prog!=nullptr);
        }
        return prog;
    }
TempoProgramado* ListaTempoProgramado:: buscar(char *dataProg,char *horaProg){
        TempoProgramado *prog = nullptr;
        if(inicio!=nullptr){
            prog = inicio;
            do{
                if(prog->dataProg==dataProg&&prog->horaProg==horaProg)
                    return prog;
                else
                    prog = prog->proximo;
            }while(prog!=nullptr);
        }
        return prog;
    }
TempoProgramado* ListaTempoProgramado:: buscar(bool repetir){
        TempoProgramado *prog = nullptr;
        if(inicio!=nullptr){
            prog = inicio;
            do{
                if(prog->repetir==repetir)
                    return prog;
                else
                    prog = prog->proximo;
            }while(prog!=nullptr);
        }
        return prog;
    }
TempoProgramado* ListaTempoProgramado:: buscar(int index){
        TempoProgramado *prog = nullptr;
        if(inicio!=nullptr){
            prog = inicio;
            int i = 0;
            while(i<=index&&prog->proximo!=nullptr){
                if(i==index)
                    return prog;
                prog = prog->proximo;
                i++;
            }
            prog = prog->proximo;
        }
        return prog;
}

TempoProgramado* ListaTempoProgramado::getInicio(){
    return inicio;
}

void ListaTempoProgramado::apagarProg(){
    if(inicio!=nullptr){
        TempoProgramado *atual = inicio;
        TempoProgramado *prox = inicio->proximo;
        while(prox!=nullptr){
            if(atual!=nullptr){
                delete atual;
                atual = prox;
                prox = atual->proximo;
            }
        }
        if(atual!=nullptr)
            delete atual;
        inicio = nullptr;
        fim = nullptr;
    }
}

ListaTempoProgramado ListaTempoProgramado::buscarTodos(char *data,char *diaSemana){
    ListaTempoProgramado prog;
    TempoProgramado *temp = nullptr;
    if(inicio!=nullptr){
        bool terminou = false;
        temp = inicio;
        while(!terminou){
            if(strcmp(temp->dataProg,data)==0||stringContains(temp->diasRepetir,diaSemana)){
                prog.addItem(temp);
            }
            if(temp->proximo!=nullptr)
                temp = temp->proximo;
            else
                terminou = true;
        }
    }
    return prog;
}

ListaTempoProgramado ListaTempoProgramado::buscarTodos(char *data){
    ListaTempoProgramado prog;
    TempoProgramado *temp = nullptr;
    if(inicio!=nullptr){
        bool terminou = false;
        temp = inicio;
        while(!terminou){
            if(strcmp(temp->dataProg,data)==0){
                prog.addItem(temp);
            }
            if(temp->proximo!=nullptr)
                temp = temp->proximo;
            else
                terminou = true;
        }
    }
    return prog;
}

bool ListaTempoProgramado::stringContains(char *s,char *c){
    int a = strlen(s);
    int b = strlen(s);
    int j = 0;
    for(int i = 0;i<a;i++){
        if(s[i]==c[j])
            j++ ;
        else
            j = 0;
        if(j == b-1)
            return true;        
    }
    return false;
}