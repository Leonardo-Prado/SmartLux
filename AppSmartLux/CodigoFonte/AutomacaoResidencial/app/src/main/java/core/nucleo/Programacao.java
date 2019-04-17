package core.nucleo;

import java.text.ParseException;

import core.auxiliares.ManipuladorDataTempo;

public class Programacao {
    public static boolean DESLIGAR = false;
    public static boolean LIGAR = true;
    private long hora;
    private long data;
    private DiaDaSemana diaDaSemana;
    private int elemento;
    private boolean acao = LIGAR;
    private String nomeElemento;

    public long getHora() {
        return hora;
    }

    public void setHora(long hora) {
        this.hora = hora;
    }

    public void setHora(String horaString) {
        long horaInt = 0;
        try {
            horaInt = ManipuladorDataTempo.tempoStringToTempoInt(horaString);
        } catch (ParseException e) {
            e.printStackTrace();
        }
        this.hora = horaInt;
    }

    public long getData() {
        return data;
    }

    public void setData(long data) {
        this.data = data;
    }

    public void setData(String dataString) {
        long dataInt = 0;
        try {
            dataInt = ManipuladorDataTempo.dataStringToDataInt(dataString);
        } catch (ParseException e) {
            e.printStackTrace();
        }
        this.data = dataInt;
    }

    public DiaDaSemana getDiaDaSemana() {
        return diaDaSemana;
    }

    public void setDiaDaSemana(DiaDaSemana diaDaSemana) {
        this.diaDaSemana = diaDaSemana;
    }

    public int getElemento() {
        return elemento;
    }

    public void setElemento(int elemento) {
        this.elemento = elemento;
    }

    public boolean isAcao() {
        return acao;
    }

    public void setAcao(boolean acao) {
        this.acao = acao;
    }

    public String getNomeElemento() {
        return nomeElemento;
    }

    public void setNomeElemento(String nomeElemento) {
        this.nomeElemento = nomeElemento;
    }
}
enum DiaDaSemana{
    DOM,
    SEG,
    TER,
    QUA,
    QUI,
    SEX,
    SAB
}