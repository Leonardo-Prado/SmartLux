package core.adapters;

import android.app.DatePickerDialog;
import android.app.TimePickerDialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.graphics.drawable.Drawable;
import android.support.annotation.NonNull;
import android.support.v7.widget.CardView;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.DatePicker;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.TimePicker;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;
import com.lspsoftware.automacaoresidencial.R;

import java.util.Calendar;
import java.util.List;

import core.auxiliares.DialogConstrutor;
import core.nucleo.Dispositivo;

public class RvListaDeDispositivosAdapter extends RecyclerView.Adapter {
    Context context;
    List<Dispositivo> dispositivos;
    DialogConstrutor dialogProg;
    String data;
    String horaProg;


    public RvListaDeDispositivosAdapter(Context context, List<Dispositivo> dispositivos) {
        this.context = context;
        this.dispositivos = dispositivos;
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View itemView = inflater.inflate(R.layout.dispositivo,parent,false);
        RvListaDeDispositivosHolder rvListaDeDispositivosHolder = new RvListaDeDispositivosHolder(itemView);
        return rvListaDeDispositivosHolder;
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
        try {
            RvListaDeDispositivosHolder dispositivosHolder = (RvListaDeDispositivosHolder) holder;
            final Dispositivo dispositivo = dispositivos.get(position);
            dispositivosHolder.tvStatusDispositivo.setText(dispositivo.getStatus());
            dispositivosHolder.tvNomeDispositivo.setText(dispositivo.getNome());
            dispositivosHolder.tvDescricaoDispositivo.setText(dispositivo.getDescricao());
            Resources resources = context.getResources();
            dispositivosHolder.imvDispositivo.setImageDrawable(getImagem(dispositivo.getTipo(),dispositivo.getStatus(),resources));
            dispositivosHolder.imbProgTime.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                    View progTimeDialog = inflater.inflate(R.layout.dialog_prog_time,null);
                    final EditText edData = progTimeDialog.findViewById(R.id.edData);
                    final EditText edTime = progTimeDialog.findViewById(R.id.edTime);
                    final CheckBox cbDom = progTimeDialog.findViewById(R.id.cbDom);
                    final CheckBox cbSeg = progTimeDialog.findViewById(R.id.cbSeg);
                    final CheckBox cbTer = progTimeDialog.findViewById(R.id.cbTer);
                    final CheckBox cbQua = progTimeDialog.findViewById(R.id.cbQua);
                    final CheckBox cbQui = progTimeDialog.findViewById(R.id.cbQui);
                    final CheckBox cbSex = progTimeDialog.findViewById(R.id.cbSex);
                    final CheckBox cbSab = progTimeDialog.findViewById(R.id.cbSab);
                    final RadioGroup rgAcao = progTimeDialog.findViewById(R.id.rgAcao);
                    final RadioButton rbLigar = progTimeDialog.findViewById(R.id.rbLigar);
                    EditText rbDesligar = progTimeDialog.findViewById(R.id.rbDesligar);
                    Button btnProgramar = progTimeDialog.findViewById(R.id.btnProgramar);
                    final View llDataTime = progTimeDialog.findViewById(R.id.llDataTime);
                    final View llDiasDaSemana = progTimeDialog.findViewById(R.id.llDiasDaSemana);
                    final CheckBox cbRepetir = progTimeDialog.findViewById(R.id.cbRepetir);

                    cbRepetir.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                        @Override
                        public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                            if(b){
                                edData.setVisibility(View.GONE);
                                llDiasDaSemana.setVisibility(View.VISIBLE);
                                edData.setText(null);
                            }else {
                                edData.setVisibility(View.VISIBLE);
                                llDiasDaSemana.setVisibility(View.GONE);
                                cbDom.setChecked(false);
                                cbSeg.setChecked(false);
                                cbTer.setChecked(false);
                                cbQua.setChecked(false);
                                cbQui.setChecked(false);
                                cbSex.setChecked(false);
                                cbSab.setChecked(false);
                            }
                        }
                    });


                    edData.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(final View view) {
                            Calendar calendario = Calendar.getInstance();
                            int dia = calendario.get(Calendar.DAY_OF_MONTH);
                            int mes = calendario.get(Calendar.MONTH);
                            int ano = calendario.get(Calendar.YEAR);
                            DatePickerDialog datePickerDialog;
                            datePickerDialog = new DatePickerDialog(view.getContext(), new DatePickerDialog.OnDateSetListener() {
                                @Override
                                public void onDateSet(DatePicker datePicker, int selectedAno, int selectedMes, int selectedDia) {
                                    try {
                                        selectedMes = selectedMes + 1;
                                        if(selectedDia<=9&&selectedMes>9) {
                                            data = "0" + selectedDia + selectedMes + selectedAno;
                                            edData.setText("0" + selectedDia+"-"  + selectedMes+"-"  + selectedAno);
                                        }else if(selectedDia<=9&&selectedMes<=9) {
                                            data = "0" + selectedDia + "0" + selectedMes + selectedAno;
                                            edData.setText("0" + selectedDia+"-"  + "0" + selectedMes+"-"  + selectedAno);
                                        }else if(selectedDia>9&&selectedMes<=9) {
                                            data = selectedDia + "0" + selectedMes + selectedAno;
                                            edData.setText(selectedDia+"-"  + "0" + selectedMes+"-"  + selectedAno);
                                        }else if(selectedDia>9&&selectedMes>9) {
                                            data = "" + selectedDia + selectedMes + selectedAno;
                                            edData.setText("" + selectedDia+"-"  + selectedMes+"-"  + selectedAno);
                                        }else {
                                            data = "" + selectedDia + selectedMes + selectedAno;
                                            edData.setText(selectedDia+"-" + selectedMes +"-" + selectedAno);
                                        }
                                    } catch (Exception e) {
                                        Log.e("erro ao pegar data", e.getMessage());
                                    }
                                }
                            }, ano, mes, dia);
                            datePickerDialog.setTitle("Data programada");
                            datePickerDialog.show();
                        }
                    });

                    edTime.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            Calendar calendario = Calendar.getInstance();
                            final int hora = calendario.get(Calendar.HOUR_OF_DAY);
                            int minuto = calendario.get(Calendar.MINUTE);
                            TimePickerDialog timePickerDialog;
                            timePickerDialog = new TimePickerDialog(view.getContext(), new TimePickerDialog.OnTimeSetListener() {
                                @Override
                                public void onTimeSet(TimePicker timePicker, int selectedHour, int selectedMinute) {
                                    String horaString;
                                    String minutoString;
                                    if (selectedHour <= 9)
                                        horaString = "0" + Integer.toString(selectedHour);
                                    else
                                        horaString = Integer.toString(selectedHour);
                                    if (selectedMinute <= 9)
                                        minutoString = "0" + Integer.toString(selectedMinute);
                                    else
                                        minutoString = Integer.toString(selectedMinute);
                                    horaProg = horaString + minutoString;
                                    edTime.setText(horaString +":" + minutoString);
                                }
                            }, hora, minuto, true);
                            timePickerDialog.setTitle("Hora do acionamento");
                            timePickerDialog.show();
                        }
                    });

                    btnProgramar.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            if(validate(edTime)&&((!cbRepetir.isChecked()&&validate(edData))||(cbRepetir.isChecked()&&(cbDom.isChecked()||cbQua.isChecked()||cbQui.isChecked()||cbSab.isChecked()||cbSeg.isChecked()||cbSex.isChecked()||cbTer.isChecked())))){

                                String dia = data;
                                String hora ="-"+ horaProg;
                                String repetir = "-";
                                String boolRepetir = "-";
                                String ligado = "-";
                                String elemento = "-"+dispositivo.getPosicao();
                                String nomeElemento = "-"+dispositivo.getNome();
                                String s = "";
                                for (char c:nomeElemento.toCharArray()
                                     ) {
                                    if(c==' ')
                                        c = '_' ;
                                    s+=c;
                                }
                                nomeElemento = s;
                                if(cbDom.isChecked())
                                    repetir+= "Dom";
                                if(cbSeg.isChecked())
                                    repetir+="Seg";
                                if(cbTer.isChecked())
                                    repetir+="Ter";
                                if(cbQua.isChecked())
                                    repetir+="Qua";
                                if(cbQui.isChecked())
                                    repetir+="Qui";
                                if(cbSex.isChecked())
                                    repetir+="Sex";
                                if(cbSab.isChecked())
                                    repetir+="Sab";
                                if(rbLigar.isChecked())
                                    ligado += "true";
                                else
                                    ligado += "false";
                                if(cbRepetir.isChecked())
                                    boolRepetir+="true";
                                else
                                    boolRepetir+="false";
                                SharedPreferences preferences = context.getSharedPreferences("Preferencias", Context.MODE_PRIVATE);
                                String host = preferences.getString("ip",null);
                                RequestQueue queue = Volley.newRequestQueue(context);
                                String url = "http://" + host + "/novaProgramacao?prog="+dia+hora+repetir+boolRepetir+ligado+elemento+nomeElemento;
                                StringRequest request = new StringRequest(Request.Method.GET, url, new Response.Listener<String>() {
                                    @Override
                                    public void onResponse(String response) {
                                        if(response.contains("ok"))
                                            dialogProg.fechar();

                                    }
                                }, new Response.ErrorListener() {
                                    @Override
                                    public void onErrorResponse(VolleyError error) {
                                        new DialogConstrutor(context,"Erro ao acionar servidor",error.getMessage(),"OK");
                                    }
                                });
                                queue.add(request);
                            }
                        }
                    });
                    dialogProg = new DialogConstrutor(context,progTimeDialog);


                }
            });
            dispositivosHolder.cvDispositivos.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    SharedPreferences preferences = context.getSharedPreferences("Preferencias", Context.MODE_PRIVATE);
                    String host = preferences.getString("ip",null);
                    RequestQueue queue = Volley.newRequestQueue(context);
                    String url = "http://" + host + "/setStatusSaida="+dispositivo.getPosicao();
                    StringRequest request = new StringRequest(Request.Method.GET, url, new Response.Listener<String>() {
                        @Override
                        public void onResponse(String response) {

                        }
                    }, new Response.ErrorListener() {
                        @Override
                        public void onErrorResponse(VolleyError error) {
                            new DialogConstrutor(context,"Erro ao acionar servidor",error.getMessage(),"OK");
                        }
                    });
                    queue.add(request);
                }
            });

        }catch(Exception e){
            Log.e("erro:",e.getMessage());

        }
    }

    private boolean validate(EditText edValidar) {
        return (edValidar.getText().toString().isEmpty())?false:true;
    }

    private Drawable getImagem(String tipo,String status,Resources resources) {
        if(tipo.contains("Lampada")||tipo.contains("LED")||tipo.contains("FitaLED")) {
            if(!(status == Dispositivo.LIGADO))
                return resources.getDrawable(R.drawable.led_off);
            else
                return resources.getDrawable(R.drawable.led_on);
        }
        else if(tipo.contains("Cortina")) {
            if (status==Dispositivo.LIGADO)
                return resources.getDrawable(R.drawable.cortina_aberta);
            else
                return resources.getDrawable(R.drawable.cortina_fechada);
        }
        else {
            if (!(status == Dispositivo.LIGADO))
                return resources.getDrawable(R.drawable.led_off);
            else
                return resources.getDrawable(R.drawable.led_on);
        }
    }

    @Override
    public int getItemCount() {
        return dispositivos.size();
    }
}

class RvListaDeDispositivosHolder extends RecyclerView.ViewHolder{

    CardView cvDispositivos;
    ImageView imvDispositivo;
    ImageButton imbProgTime;
    TextView tvNomeDispositivo;
    TextView tvDescricaoDispositivo;
    TextView tvStatusDispositivo;
    public RvListaDeDispositivosHolder(View itemView) {
        super(itemView);
        cvDispositivos = itemView.findViewById(R.id.cvDispositivos);
        imvDispositivo = itemView.findViewById(R.id.imvDispositivo);
        tvNomeDispositivo = itemView.findViewById(R.id.tvNomeDoDispositivo);
        tvStatusDispositivo = itemView.findViewById(R.id.tvStatusDispositivo);
        tvDescricaoDispositivo = itemView.findViewById(R.id.tvDescricaoDispositivo);
        imbProgTime = itemView.findViewById(R.id.imbProgTime);
    }
}