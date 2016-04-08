class AtlHists{
public:
  AtlHists(char* fn){
    m_file =new TFile(fn);

    this->nonIsolatedElectronHists();    
    this->isolatedElectronHists();
    this->nonIsolatedPhotonHists();
    this->isolatedPhotonHists();
    this->nonIsolatedMuonHists();
    this->isolatedMuonHists();
    this->nonIsolatedMuonHists();
    this->isolatedMuonHists();
    this->cellHists();
    this->clusterHists();
    this->jetHists();
    this->jetBHists();
    this->eventHeaderHists1();
    this->eventHeaderHists2();
  }

  void nonIsolatedElectronHists(){
    m_h1 = (TH1F*)m_file->Get("simple/1");
    m_h2 = (TH1F*)m_file->Get("simple/2");
    m_h3 = (TH1F*)m_file->Get("simple/3");
    m_h4 = (TH1F*)m_file->Get("simple/4");
    m_h5 = (TH1F*)m_file->Get("simple/5");
    m_h6 = (TH1F*)m_file->Get("simple/6");
    
    TPostScript ps("nonIsolatedElectron.ps", 112);
    this->Draw();
    ps.Close();

    return;
  }

  void isolatedElectronHists(){
    m_h1 = (TH1F*)m_file->Get("simple/7");
    m_h2 = (TH1F*)m_file->Get("simple/8");
    m_h3 = (TH1F*)m_file->Get("simple/9");
    m_h4 = (TH1F*)m_file->Get("simple/10");
    m_h5 = (TH1F*)m_file->Get("simple/11");
    m_h6 = (TH1F*)m_file->Get("simple/12");

    TPostScript ps("isolatedElectron.ps", 112);
    this->Draw();
    ps.Close();

  }
    

  void nonIsolatedPhotonHists(){
    m_h1 = (TH1F*)m_file->Get("simple/13");
    m_h2 = (TH1F*)m_file->Get("simple/14");
    m_h3 = (TH1F*)m_file->Get("simple/15");
    m_h4 = (TH1F*)m_file->Get("simple/16");
    m_h5 = (TH1F*)m_file->Get("simple/17");
    m_h6 = (TH1F*)m_file->Get("simple/18");

    TPostScript ps("nonIsolatedPhoton.ps", 112);
    this->Draw();
    ps.Close();



    return;
  }

  void isolatedPhotonHists(){
    m_h1 = (TH1F*)m_file->Get("simple/19");
    m_h2 = (TH1F*)m_file->Get("simple/20");
    m_h3 = (TH1F*)m_file->Get("simple/21");
    m_h4 = (TH1F*)m_file->Get("simple/22");
    m_h5 = (TH1F*)m_file->Get("simple/23");
    m_h6 = (TH1F*)m_file->Get("simple/24");

    TPostScript ps("isolatedPhoton.ps", 112);
    this->Draw();
    ps.Close();


  }
    

  void nonIsolatedMuonHists(){
    m_h1 = (TH1F*)m_file->Get("simple/25");
    m_h2 = (TH1F*)m_file->Get("simple/26");
    m_h3 = (TH1F*)m_file->Get("simple/27");
    m_h4 = (TH1F*)m_file->Get("simple/28");
    m_h5 = (TH1F*)m_file->Get("simple/29");
    m_h6 = (TH1F*)m_file->Get("simple/30");


    TPostScript ps("nonIsolatedMuon.ps", 112);
    this->Draw();
    ps.Close();



    return;
  }

  void isolatedMuonHists(){
    m_h1 = (TH1F*)m_file->Get("simple/31");
    m_h2 = (TH1F*)m_file->Get("simple/32");
    m_h3 = (TH1F*)m_file->Get("simple/33");
    m_h4 = (TH1F*)m_file->Get("simple/34");
    m_h5 = (TH1F*)m_file->Get("simple/35");
    m_h6 = (TH1F*)m_file->Get("simple/36");


    TPostScript ps("isolatedMuon.ps", 112);
    this->Draw();
    ps.Close();



  }
    

  void cellHists(){
    m_h1 = (TH1F*)m_file->Get("simple/37");
    m_h2 = (TH1F*)m_file->Get("simple/38");
    m_h3 = (TH1F*)m_file->Get("simple/39");
    m_h4 = (TH1F*)m_file->Get("simple/40");
    m_h5 = (TH1F*)m_file->Get("simple/41");
    m_h6 = (TH1F*)m_file->Get("simple/42");

    TPostScript ps("cells.ps", 112);
    this->Draw();
    ps.Close();


  }

  void clusterHists(){
    m_h1 = (TH1F*)m_file->Get("simple/43");
    m_h2 = (TH1F*)m_file->Get("simple/44");
    m_h3 = (TH1F*)m_file->Get("simple/45");
    m_h4 = (TH1F*)m_file->Get("simple/46");
    m_h5 = (TH1F*)m_file->Get("simple/47");
    m_h6 = (TH1F*)m_file->Get("simple/48");

    TPostScript ps("clusters.ps", 112);
    this->Draw();
    ps.Close();



  }

  void jetHists(){
    m_h1 = (TH1F*)m_file->Get("simple/49");
    m_h2 = (TH1F*)m_file->Get("simple/50");
    m_h3 = (TH1F*)m_file->Get("simple/51");
    m_h4 = (TH1F*)m_file->Get("simple/52");
    m_h5 = (TH1F*)m_file->Get("simple/53");
    m_h6 = (TH1F*)m_file->Get("simple/54");


    TPostScript ps("jets.ps", 112);
    this->Draw();
    ps.Close();



  }
  void jetBHists(){
    m_h1 = (TH1F*)m_file->Get("simple/55");
    m_h2 = (TH1F*)m_file->Get("simple/56");
    m_h3 = (TH1F*)m_file->Get("simple/57");
    m_h4 = (TH1F*)m_file->Get("simple/58");
    m_h5 = (TH1F*)m_file->Get("simple/59");
    m_h6 = (TH1F*)m_file->Get("simple/60");

    TPostScript ps("bjets.ps", 112);
    this->Draw();
    ps.Close();


  }
    

  void eventHeaderHists1(){
    m_h1 = (TH1F*)m_file->Get("simple/55");
    m_h2 = (TH1F*)m_file->Get("simple/56");
    m_h3 = (TH1F*)m_file->Get("simple/57");
    m_h4 = (TH1F*)m_file->Get("simple/58");
    m_h5 = (TH1F*)m_file->Get("simple/59");
    m_h6 = (TH1F*)m_file->Get("simple/60");


    TPostScript ps("eventHeader1.ps", 112);
    this->Draw();
    ps.Close();



  }

  void eventHeaderHists2(){
    m_h1 = (TH1F*)m_file->Get("simple/61");
    m_h2 = (TH1F*)m_file->Get("simple/62");
    m_h3 = (TH1F*)m_file->Get("simple/63");
    m_h4 = new TH1F;
    m_h5 = new TH1F;
    m_h6 = new TH1F;

    TPostScript ps("eventHeader2.ps", 112);
    this->Draw();
    ps.Close();



  }

  void Draw(){
    TCanvas* can = new TCanvas;
    can->Clear();
    can->Divide(3,2);
    can->cd(1);
    m_h1->Draw();
    can->cd(2);
    m_h2->Draw();
    can->cd(3);
    m_h3->Draw();
    can->cd(4);
    m_h4->Draw();
    can->cd(5);
    m_h5->Draw();
    can->cd(6);
    m_h6->Draw();
  }
  void Print(TCanvas* c){
    c->Print(fn);
  }
 
private:
  TH1F* m_h1;
  TH1F* m_h2;
  TH1F* m_h3;
  TH1F* m_h4;
  TH1F* m_h5;
  TH1F* m_h6;
  TFile* m_file;
  
};

class HistoRegression{
public:
 HistoRegression(char* file, char* refFile){
   bool initOK = true;
   m_file =new TFile(file);
   if (m_file->IsZombie()) {
     initOK = false;
   }

   m_refFile = new TFile(refFile);
   if (m_refFile->IsZombie()) {
     initOK = false;
   }

   cout <<"initOK "<<initOK<<endl;
   if(initOK){
     cout<<"performing ...."<<endl;
     this->nonIsolatedElectronHists(); 
     this->isolatedElectronHists();
     this->nonIsolatedPhotonHists();
     this->isolatedPhotonHists();
   }
 }

  void nonIsolatedElectronHists(){
    m_h1 = (TH1F*)m_file->Get("simple/1");
    m_h2 = (TH1F*)m_file->Get("simple/2");
    m_h3 = (TH1F*)m_file->Get("simple/3");
    m_h4 = (TH1F*)m_file->Get("simple/4");
    m_h5 = (TH1F*)m_file->Get("simple/5");
    m_h6 = (TH1F*)m_file->Get("simple/6");

    m_refh1 = (TH1F*)m_refFile->Get("simple/1");
    m_refh2 = (TH1F*)m_refFile->Get("simple/2");
    m_refh3 = (TH1F*)m_refFile->Get("simple/3");
    m_refh4 = (TH1F*)m_refFile->Get("simple/4");
    m_refh5 = (TH1F*)m_refFile->Get("simple/5");
    m_refh6 = (TH1F*)m_refFile->Get("simple/6");
    
    TPostScript ps("nonIsolatedElectron_regression.ps", 112);
    this->Draw();
    ps.Close();

    return;
  }
  
  void isolatedElectronHists(){
    m_h1 = (TH1F*)m_file->Get("simple/7");
    m_h2 = (TH1F*)m_file->Get("simple/8");
    m_h3 = (TH1F*)m_file->Get("simple/9");
    m_h4 = (TH1F*)m_file->Get("simple/10");
    m_h5 = (TH1F*)m_file->Get("simple/11");
    m_h6 = (TH1F*)m_file->Get("simple/12");
    m_refh1 = (TH1F*)m_refFile->Get("simple/7");
    m_refh2 = (TH1F*)m_refFile->Get("simple/8");
    m_refh3 = (TH1F*)m_refFile->Get("simple/9");
    m_refh4 = (TH1F*)m_refFile->Get("simple/10");
    m_refh5 = (TH1F*)m_refFile->Get("simple/11");
    m_refh6 = (TH1F*)m_refFile->Get("simple/12");
    TPostScript ps("isolatedElectron_regression.ps", 112);
    this->Draw();
    ps.Close();

  }
    

  void nonIsolatedPhotonHists(){
    m_h1 = (TH1F*)m_file->Get("simple/13");
    m_h2 = (TH1F*)m_file->Get("simple/14");
    m_h3 = (TH1F*)m_file->Get("simple/15");
    m_h4 = (TH1F*)m_file->Get("simple/16");
    m_h5 = (TH1F*)m_file->Get("simple/17");
    m_h6 = (TH1F*)m_file->Get("simple/18");
    m_refh1 = (TH1F*)m_refFile->Get("simple/13");
    m_refh2 = (TH1F*)m_refFile->Get("simple/14");
    m_refh3 = (TH1F*)m_refFile->Get("simple/15");
    m_refh4 = (TH1F*)m_refFile->Get("simple/16");
    m_refh5 = (TH1F*)m_refFile->Get("simple/17");
    m_refh6 = (TH1F*)m_refFile->Get("simple/18");
    TPostScript ps("nonIsolatedPhoton_regression.ps", 112);
    this->Draw();
    ps.Close();

    return;
  }

  void isolatedPhotonHists(){
    m_h1 = (TH1F*)m_file->Get("simple/19");
    m_h2 = (TH1F*)m_file->Get("simple/20");
    m_h3 = (TH1F*)m_file->Get("simple/21");
    m_h4 = (TH1F*)m_file->Get("simple/22");
    m_h5 = (TH1F*)m_file->Get("simple/23");
    m_h6 = (TH1F*)m_file->Get("simple/24");
    m_refh1 = (TH1F*)m_refFile->Get("simple/19");
    m_refh2 = (TH1F*)m_refFile->Get("simple/20");
    m_refh3 = (TH1F*)m_refFile->Get("simple/21");
    m_refh4 = (TH1F*)m_refFile->Get("simple/22");
    m_refh5 = (TH1F*)m_refFile->Get("simple/23");
    m_refh6 = (TH1F*)m_refFile->Get("simple/24");
    TPostScript ps("isolatedPhoton_regression.ps", 112);
    this->Draw();
    ps.Close();


  }

  void setLineColor(int i){
    gStyle->SetHistLineColor(i)
  }
  
  void Draw(){
    TCanvas* can = new TCanvas;
    can->Clear();
    can->Divide(3,2);
    can->cd(1);
    setLineColor(1);
    m_h1->Draw();
    setLineColor(2);
    m_refh1->Draw("same");
    can->cd(2);
    setLineColor(1);
    m_h2->Draw();
    setLineColor(2);
    m_refh2->Draw("same");
    can->cd(3);
    setLineColor(1);
    m_h3->Draw();
    setLineColor(2);
    m_refh3->Draw("same");
    can->cd(4);
    setLineColor(1);
    m_h4->Draw();
    setLineColor(2);
    m_refh4->Draw("same");
    can->cd(5);
    setLineColor(1);
    m_h5->Draw();
    setLineColor(2);
    m_refh5->Draw("same");
    can->cd(6);
    setLineColor(1);
    m_h6->Draw();
    setLineColor(2);
    m_refh6->Draw("same");
  }

  void Print(TCanvas* c){
    c->Print(fn);
  }
 
private:
  TH1F* m_h1;
  TH1F* m_h2;
  TH1F* m_h3;
  TH1F* m_h4;
  TH1F* m_h5;
  TH1F* m_h6;
  TH1F* m_refh1;
  TH1F* m_refh2;
  TH1F* m_refh3;
  TH1F* m_refh4;
  TH1F* m_refh5;
  TH1F* m_refh6;
  TFile* m_file;
  TFile* m_refFile;

};

int RTT_atlfast(){
   
  //  cout<<"in RTT_atlfast.C"<<endl;
  AtlHists a1("histo.hbook");

  //  cout<<"doing regression histograms..."<<endl;
  HistoRegression hr("histo.hbook","refHisto.hbook");

  return 0;
}
