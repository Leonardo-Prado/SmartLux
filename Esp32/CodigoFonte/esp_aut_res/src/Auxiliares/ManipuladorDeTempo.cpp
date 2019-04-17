    #include "ManipuladorDeTempo.h"
    #include "string.h"
    ManipuladorDeTempo::ManipuladorDeTempo(){
        
    }
    ManipuladorDeTempo::~ManipuladorDeTempo(){

    }
    int ManipuladorDeTempo::getHora(){
        return hora;
    }
    int ManipuladorDeTempo::getMinuto(){
        return minuto;
    }
    int ManipuladorDeTempo::getSegundo(){
        return segundo;
    }
    int ManipuladorDeTempo::getMilli(){
        return milli;
    }
    int ManipuladorDeTempo::getDia(){
        return dia;
    }
    int ManipuladorDeTempo::getMes(){
        return mes;
    }
    int ManipuladorDeTempo::getAno(){
        return ano;
    }
    int ManipuladorDeTempo::getDiaDaSemana(){
        return diaDaSemana;
    }
    long ManipuladorDeTempo::getTempoInt(){
        return hora*60*60*1000+minuto*60*1000+segundo*1000+milli;
    }
    std::string ManipuladorDeTempo::getTempoString(){
        return tempoString;
    }
    std::string ManipuladorDeTempo::getDiaDaSemanaString(){
        return diaDaSemanaString;
    }
    std::string ManipuladorDeTempo::getDiaDaSemanaStringLow(){
        return diaDaSemanaStringLow;
    }
    void ManipuladorDeTempo::setTempo(std::string tempo){
        if(tempo.length()<=6){
            hora = toInt(tempo.substr(0,2));
            minuto = toInt(tempo.substr(3,5));
        }else if(tempo.length()<=9){
            hora = toInt(tempo.substr(0,2));
            minuto = toInt(tempo.substr(3,5));
            segundo = toInt(tempo.substr(6,8));
        }else if(tempo.length()<=12){
            hora = toInt(tempo.substr(0,2));
            minuto = toInt(tempo.substr(3,5));
            segundo = toInt(tempo.substr(6,8));
            milli = toInt(tempo.substr(9,12));
        } 
        tempoString = tempo;
    }

    void ManipuladorDeTempo::setTempo(long tempo){
        int diaDoAno;
        int diasTotais;
        ano = (tempo/ANO)+1970;
        int diasBissextos = ((tempo/ANO)-2)/4;
        diaDoAno = (tempo%ANO)/(DIA) - diasBissextos;
        bisexto = ((tempo/ANO)+1972)%4==0?true:false;
        if(diaDoAno<=JAN){
            mes = 0;
            dia = diaDoAno;
        }
        else if(diaDoAno>JAN&&diaDoAno<=FEV){
            mes = 1; 
            dia = diaDoAno - JAN;
        }else if(diaDoAno>FEV&&diaDoAno<=MAR){
            mes = 2;
            dia = diaDoAno - FEV;
        }else if(diaDoAno>MAR&&diaDoAno<=ABR){
            mes = 3;     
            dia = diaDoAno - MAR;   
        }else if(diaDoAno>ABR&&diaDoAno<=MAI){
            mes = 4; 
            dia = diaDoAno - ABR; 
        }else if(diaDoAno>MAI&&diaDoAno<=JUN){
            mes = 5;
            dia = diaDoAno - MAI;
        }else if(diaDoAno>JUN&&diaDoAno<=JUL){
            mes = 6;
            dia = diaDoAno - JUN;
        }else if(diaDoAno>JUL&&diaDoAno<=AGO){
            mes = 7;
            dia = diaDoAno - JUL;
        }else if(diaDoAno>AGO&&diaDoAno<=SET){
            mes = 8;
            dia = diaDoAno - AGO;
        }else if(diaDoAno>SET&&diaDoAno<=OUT){
            mes = 9;
            dia = diaDoAno - SET;
        }else if(diaDoAno>OUT&&diaDoAno<=NOV){
            mes = 10;
            dia = diaDoAno - OUT;
        }else{
            mes = 11;
            dia = diaDoAno - NOV;
        }
        diasTotais = tempo/DIA;
        switch (diasTotais%7)
        {
            case 0:
                diaDaSemana = 0;
                diaDaSemanaString = "QUI";
                diaDaSemanaStringLow = "qui";
                break;
            case 1:
                diaDaSemana = 1;
                diaDaSemanaString = "SEX";
                diaDaSemanaStringLow = "sex";
                break;
            case 2:
                diaDaSemana = 2;
                diaDaSemanaString = "SAB";
                diaDaSemanaStringLow = "sab";
                break;
            case 3:
                diaDaSemana = 3;
                diaDaSemanaString = "DOM";
                diaDaSemanaStringLow = "dom";
                break;
            case 4:
                diaDaSemana = 4;
                diaDaSemanaString = "SEG";
                diaDaSemanaStringLow = "seg";
                break;
            case 5:
                diaDaSemana = 5;
                diaDaSemanaString = "TER";
                diaDaSemanaStringLow = "ter";
                break;
            case 6:
                diaDaSemana = 6;
                diaDaSemanaString = "QUA";
                diaDaSemanaStringLow = "qua";
                break;
            default:
                diaDaSemana = 0;
                diaDaSemanaString = "QUI";
                diaDaSemanaStringLow = "qui";
                break;
        }
        hora = (tempo%DIA)/HORA;
        minuto = (tempo%HORA)/MINUTO;
        segundo = tempo%MINUTO;
    }

    int ManipuladorDeTempo::toInt(std::string s){
        int a = ((int)s[0]) - 48;
        int b = ((int)s[1]) - 48;
        return a*10+b;
    }