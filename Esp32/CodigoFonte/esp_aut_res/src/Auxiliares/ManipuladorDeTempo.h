#include<string>
class ManipuladorDeTempo
{
  private:
    const int SEGUNDO = 1;
    const int  MINUTO = SEGUNDO * 60;
    const int HORA = MINUTO * 60;
    const int DIA = HORA * 24;
    const int SEMANA = DIA * 7;
    const int ANO = DIA * 365;
    const int JAN = 31;
    const int FEV = JAN + 28;
    const int MAR = FEV + 31;
    const int ABR = MAR + 30;
    const int MAI = ABR + 31;
    const int JUN = MAI + 30;
    const int JUL = JUN + 31;
    const int AGO = JUL + 31;
    const int SET = AGO + 30;
    const int OUT = SET + 31;
    const int NOV = OUT + 30;
    const int DEZ = NOV + 31;
    int hora, minuto, segundo, milli, dia, mes, ano,diaDaSemana;
    bool bisexto;
    long tempoInt;
    std::string tempoString,diaDaSemanaString,diaDaSemanaStringLow;
    
  public:
    ManipuladorDeTempo(/* args */);
    ~ManipuladorDeTempo();
    int getHora();
    int getMinuto();
    int getSegundo();
    int getMilli();
    int getDia();
    int getMes();
    int getAno();
    int getDiaDaSemana();
    long getTempoInt();
    std::string getTempoString();
    std::string getDiaDaSemanaString();
    std::string getDiaDaSemanaStringLow();
    void setTempo(std::string tempo);
    void setTempo(long tempo);
    int toInt(std::string s);
};

