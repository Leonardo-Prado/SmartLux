package core.adapters;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.TextView;

import com.lspsoftware.automacaoresidencial.R;

import java.util.List;

import core.auxiliares.ManipuladorDataTempo;
import core.nucleo.Programacao;

public class RvListaDeProgAdapter extends RecyclerView.Adapter {
    Context context;
    List<Programacao> programacoes;

    public RvListaDeProgAdapter(Context context, List<Programacao> programacoes) {
        this.context = context;
        this.programacoes = programacoes;
    }


    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View view = inflater.inflate(R.layout.prog_item,parent,false);
        ProgViewHolder viewHolder = new ProgViewHolder(view);
        return viewHolder;
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
        ProgViewHolder viewHolder = (ProgViewHolder) holder;
        Programacao programacao = programacoes.get(position);
        try {
            viewHolder.tvHora.setText(ManipuladorDataTempo.tempoIntToTempoString(programacao.getHora()));
            viewHolder.tvData.setText(ManipuladorDataTempo.dataIntToDataString(programacao.getData()));
            viewHolder.tvAcao.setText(programacao.isAcao()?"Ligar":"Desligar");
            viewHolder.tvNomeElemento.setText(programacao.getNomeElemento());
        }catch (Exception e){
            Log.e("error: ",e.getMessage());
        }
    }

    @Override
    public int getItemCount() {
        return programacoes.size();
    }
}
class ProgViewHolder extends RecyclerView.ViewHolder{
    TextView tvData;
    TextView tvHora;
    TextView tvNomeElemento;
    TextView tvAcao;
    ImageButton imbEditar;

    public ProgViewHolder(View itemView) {
        super(itemView);
        tvNomeElemento = itemView.findViewById(R.id.tvElementoDesc);
        tvData = itemView.findViewById(R.id.tvDataProg);
        tvHora = itemView.findViewById(R.id.tvHoraProg);
        tvAcao = itemView.findViewById(R.id.tvAcao);
        imbEditar = itemView.findViewById(R.id.imbEdit);
    }
}