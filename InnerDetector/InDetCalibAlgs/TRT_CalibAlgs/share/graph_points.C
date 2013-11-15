int graph_points(float t0shift, int it){
  
  string det[4]={"BA","BC","EA","EC"};
  int detnum[4]={-1,1,-2,2};

  TFile* test=new TFile(Form("gp_%02d_%f.root",it,t0shift),"RECREATE");

  TGraph* res_tge = new TGraph(1); 
  TGraph* res_tg = new TGraph(1); 
  TGraph* absres_tge = new TGraph(1); 
  TGraph* absres_tg = new TGraph(1); 
  TGraph* tres_tge = new TGraph(1); 
  TGraph* tres_tg = new TGraph(1); 
  TGraph* stres_tge = new TGraph(1); 
  TGraph* stres_tg = new TGraph(1); 

  TF1* fitfunc = new TF1("fitfunc","gaus",-1,1);

  for(idet=0; idet<4; idet++){

    _file0->cd();

    cout << det[idet] << endl;

    TH1F* res=new TH1F(Form("res_%s",det[idet]),Form("res_%s",det[idet]),100,-1,1);
    ntuple->Draw(Form("r-rtrackunbias>>res_%s",det[idet]),Form("det==%d && dr<1.0",detnum[idet]));
    res->Fit("fitfunc","","",res->GetMean()-0.3,res->GetMean()+0.3);
    //res->Fit("fitfunc","","",fitfunc->GetParameter(1)-0.3,fitfunc->GetParameter(1)+0.3)
    res->Fit("fitfunc","","",fitfunc->GetParameter(1)-1.5*fitfunc->GetParameter(2),fitfunc->GetParameter(1)+1.5*fitfunc->GetParameter(2));
    res->Fit("fitfunc","","",fitfunc->GetParameter(1)-1.5*fitfunc->GetParameter(2),fitfunc->GetParameter(1)+1.5*fitfunc->GetParameter(2));
    res_tg->SetPoint(0,t0shift,fitfunc->GetParameter(2));
    res_tge->SetPoint(0,t0shift,fitfunc->GetParError(2));
    res_tge->SetName(Form("res_tge_%s_%02d",det[idet],it));
    res_tg->SetName(Form("res_tg_%s_%02d",det[idet],it));

    TH1F* tres=new TH1F(Form("tres_%s",det[idet]),Form("tres_%s",det[idet]),100,-25,25);
    ntuple->Draw(Form("t-ttrack-t0+ephase>>tres_%s",det[idet]),Form("det==%d && dr<1.0",detnum[idet]));
    tres->Fit("fitfunc","","",tres->GetMean()-tres->GetRMS(),tres->GetMean()+tres->GetRMS());
    //tres->Fit("fitfunc","","",fitfunc->GetParameter(1)-3*fitfunc->GetParameter(2),fitfunc->GetParameter(1)+fitfunc->GetParameter(2));
    tres->Fit("fitfunc","","",fitfunc->GetParameter(1)-fitfunc->GetParameter(2),fitfunc->GetParameter(1)+fitfunc->GetParameter(2));
    tres->Fit("fitfunc","","",fitfunc->GetParameter(1)-fitfunc->GetParameter(2),fitfunc->GetParameter(1)+fitfunc->GetParameter(2));
    tres_tg->SetPoint(0,t0shift,fitfunc->GetParameter(1));
    tres_tge->SetPoint(0,t0shift,fitfunc->GetParError(1));
    tres_tge->SetName(Form("tres_tge_%s_%02d",det[idet],it));
    tres_tg->SetName(Form("tres_tg_%s_%02d",det[idet],it));

    TH1F* stres1=new TH1F(Form("stres1_%s",det[idet]),Form("stres1_%s",det[idet]),100,-25,25);
    TH1F* stres2=new TH1F(Form("stres2_%s",det[idet]),Form("stres2_%s",det[idet]),100,-25,25);
    TH1F* stres=new TH1F(Form("stres_%s",det[idet]),Form("stres_%s",det[idet]),100,-25,25);
    ntuple->Draw(Form("t-ttrack-t0+ephase>>stres1_%s",det[idet]),Form("det==%d && dr<1.0",detnum[idet]));
    ntuple->Draw(Form("-(t-ttrack-t0+ephase)>>stres2_%s",det[idet]),Form("det==%d && dr<1.0",detnum[idet]));
    stres->Add(stres1,stres2,0.5,0.5);
    stres->Fit("fitfunc","","",stres->GetMean()-stres->GetRMS(),stres->GetMean()+stres->GetRMS());
    //stres->Fit("fitfunc","","",fitfunc->GetParameter(1)-3*fitfunc->GetParameter(2),fitfunc->GetParameter(1)+fitfunc->GetParameter(2));
    stres->Fit("fitfunc","","",fitfunc->GetParameter(1)-fitfunc->GetParameter(2),fitfunc->GetParameter(1)+fitfunc->GetParameter(2));
    stres->Fit("fitfunc","","",fitfunc->GetParameter(1)-fitfunc->GetParameter(2),fitfunc->GetParameter(1)+fitfunc->GetParameter(2));
    stres_tg->SetPoint(0,t0shift,fitfunc->GetParameter(2));
    stres_tge->SetPoint(0,t0shift,fitfunc->GetParError(2));
    stres_tge->SetName(Form("stres_tge_%s_%02d",det[idet],it));
    stres_tg->SetName(Form("stres_tg_%s_%02d",det[idet],it));

    TH1F* absres=new TH1F(Form("absres_%s",det[idet]),Form("absres_%s",det[idet]),100,-0.4,0.4);
    ntuple->Draw(Form("abs(r)-abs(rtrackunbias)>>absres_%s",det[idet]),Form("det==%d && dr<1.0 && (t-t0)>10 && (t-t0)<40",detnum[idet]));
    //ntuple->Draw(Form("abs(r)-abs(rtrackunbias)>>absres_%s",det[idet]),Form("det==%d && dr<1.0",detnum[idet]));
    absres->Fit("fitfunc","","",absres->GetMean()-0.1,absres->GetMean()+0.1);
    absres->Fit("fitfunc","","",absres->GetMean()-0.1,absres->GetMean()+0.1);
    //absres->Fit("fitfunc","","",-0.3,0.1);
    //absres->Fit("fitfunc","","",fitfunc->GetParameter(1)-3*fitfunc->GetParameter(2),fitfunc->GetParameter(1)+fitfunc->GetParameter(2));
    absres->Fit("fitfunc","","",fitfunc->GetParameter(1)-fitfunc->GetParameter(2),fitfunc->GetParameter(1)+fitfunc->GetParameter(2));
    absres->Fit("fitfunc","","",fitfunc->GetParameter(1)-fitfunc->GetParameter(2),fitfunc->GetParameter(1)+fitfunc->GetParameter(2));
    absres_tg->SetPoint(0,t0shift,fitfunc->GetParameter(1));
    absres_tge->SetPoint(0,t0shift,fitfunc->GetParError(1));
    absres_tge->SetName(Form("absres_tge_%s_%02d",det[idet],it));
    absres_tg->SetName(Form("absres_tg_%s_%02d",det[idet],it));

    test->cd();

    res->Write(Form("res_%s_%02d_%.2f",det[idet],it,t0shift));
    res_tge->Write();
    res_tg->Write();

    tres->Write(Form("timeres_%s_%02d_%.2f",det[idet],it,t0shift));
    tres_tge->Write();
    tres_tg->Write();

    stres->Write(Form("sym_timeres_%s_%02d_%.2f",det[idet],it,t0shift));
    stres_tge->Write();
    stres_tg->Write();

    absres->Write(Form("absres_%s_%02d_%.2f",det[idet],it,t0shift));
    absres_tge->Write();
    absres_tg->Write();

  }
  
  return 0;

}
