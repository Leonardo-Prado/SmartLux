#ifndef String_H
#define String_H
#endif
#include "string.h"

struct TempoProgramado
{
    char dataProg[10];
    char horaProg[10];
    bool repetir = false;
    char diasRepetir[25] = "";
    bool ligar;
    int elemento;
    char nomeElemento[50];
    TempoProgramado *proximo;
};
class ListaTempoProgramado{  
    private:
    TempoProgramado *inicio,*fim;
    public:
    ListaTempoProgramado();
    void addItem(char *dataProg,char *horaProg,bool repetir,bool ligar,int elemento,char *nomeElemento);
    void addItem(char *dataProg,char *horaProg,bool repetir,bool ligar,int elemento,char *nomeElemento,char *diasRepetir);
    void addItem(TempoProgramado *prog);
    TempoProgramado* buscar(char *horaProg);
    TempoProgramado* buscar(short elemento);
    TempoProgramado* buscar(char *dataProg,char *horaProg);
    TempoProgramado* buscar(bool repetir);
    TempoProgramado* buscar(int index);
    TempoProgramado* getInicio();
    ListaTempoProgramado buscarTodos(char *dataProg,char *diaSemana);
    ListaTempoProgramado buscarTodos(char *dataProg);
    void apagarProg();
    bool stringContains(char *s,char *c);
};