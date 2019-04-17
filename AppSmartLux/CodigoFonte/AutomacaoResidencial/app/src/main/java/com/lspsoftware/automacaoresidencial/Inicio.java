package com.lspsoftware.automacaoresidencial;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.LinearLayout;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import java.util.ArrayList;
import java.util.List;

import core.adapters.RvListaDeProgAdapter;
import core.auxiliares.DialogConstrutor;
import core.auxiliares.ManipuladorDataTempo;
import core.nucleo.Programacao;

public class Inicio extends AppCompatActivity {
    boolean back;
    Context context;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        context = this;
        setContentView(R.layout.activity_inicio);
        BuscarServicoLocal buscarServicoFragment = BuscarServicoLocal.newInstance();
        openFrag(buscarServicoFragment);
    }

    public void openFrag(Fragment fragment){
        FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
        transaction.replace(R.id.frmFragmentContainer, fragment);
        if(back)
            transaction.addToBackStack(null);
        back = true;
        transaction.commit();
    }

    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return true;
    }

    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.imProg: {
                try {
                    final DialogConstrutor dialogConectandoServer = new DialogConstrutor(this, "Buscando no Servidor", "Aguardando respsta do servidor");
                    SharedPreferences preferences = context.getSharedPreferences("Preferencias", Context.MODE_PRIVATE);
                    String host = preferences.getString("ip", null);
                    RequestQueue queue = Volley.newRequestQueue(context);
                    String url = "http://" + host + "/getTodaProgramacao";
                    StringRequest request = new StringRequest(Request.Method.GET, url, new Response.Listener<String>() {
                        @Override
                        public void onResponse(String response) {
                            List<Programacao> programacaos = new ArrayList<>();
                            String[] responseSplited = response.split("%");
                            for (String s : responseSplited) {
                                String[] prog = s.split("-");
                                Programacao p = new Programacao();
                                prog[0] = ManipuladorDataTempo.strinToDataFormat(prog[0]);
                                prog[1] = ManipuladorDataTempo.stringToHoraFormat(prog[1]);
                                p.setData(prog[0]);
                                p.setHora(prog[1]);
                                p.setAcao(prog[3].equalsIgnoreCase("true") ? true : false);
                                p.setElemento(Integer.parseInt(prog[4]));
                                p.setNomeElemento(prog[5]);
                                programacaos.add(p);
                            }
                            RvListaDeProgAdapter adapter = new RvListaDeProgAdapter(context, programacaos);
                            LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                            View view = inflater.inflate(R.layout.dialog_toda_prog, null);
                            RecyclerView rvProgamacoes = view.findViewById(R.id.rvProgramacao);
                            LinearLayoutManager layoutManager = new LinearLayoutManager(context, LinearLayout.VERTICAL, false);
                            layoutManager.generateLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT));
                            rvProgamacoes.setLayoutManager(layoutManager);
                            rvProgamacoes.setAdapter(adapter);
                            new DialogConstrutor(context, view);
                            dialogConectandoServer.fechar();


                        }
                    }, new Response.ErrorListener() {
                        @Override
                        public void onErrorResponse(VolleyError error) {
                            dialogConectandoServer.fechar();
                            new DialogConstrutor(context, "Erro ao acionar servidor", error.getMessage(), "OK");
                        }
                    });
                    queue.add(request);
                }catch (Exception e){
                    Log.e("erro: ",e.getMessage());
                }
            }
            case R.id.imContatos: {

            }
            case R.id.imSobre: {

            }
        }
        return super.onOptionsItemSelected(item);
    }
}
