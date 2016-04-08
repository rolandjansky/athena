
class ArtHists{
public:
  ArtHists(int base, char* fn):{
    cout<<"ArtHists base "<<base<<endl;
    TFile* f1 =new TFile(fn);
    if(base == 100){   
    m_h1 = (TH1F*)f1.Get("100");
    m_h2 = (TH1F*)f1.Get("101");
    m_h3 = (TH1F*)f1.Get("102");
    m_h4 = (TH1F*)f1.Get("200");
    m_h5 = (TH1F*)f1.Get("201");
    m_h6 = (TH1F*)f1.Get("202");
    }elseif(base==110){
    m_h1 = (TH1F*)f1.Get("110");
    m_h2 = (TH1F*)f1.Get("111");
    m_h3 = (TH1F*)f1.Get("112");
    m_h4 = (TH1F*)f1.Get("210");
    m_h5 = (TH1F*)f1.Get("211");
    m_h6 = (TH1F*)f1.Get("212");
    }elseif(base==120){
    m_h1 = (TH1F*)f1.Get("120");
    m_h2 = (TH1F*)f1.Get("121");
    m_h3 = (TH1F*)f1.Get("122");
    m_h4 = (TH1F*)f1.Get("220");
    m_h5 = (TH1F*)f1.Get("221");
    m_h6 = (TH1F*)f1.Get("222");
    }elseif(base==130){
    m_h1 = (TH1F*)f1.Get("130");
    m_h2 = (TH1F*)f1.Get("131");
    m_h3 = (TH1F*)f1.Get("132");
    m_h4 = (TH1F*)f1.Get("230");
    m_h5 = (TH1F*)f1.Get("231");
    m_h6 = (TH1F*)f1.Get("232");
    }
    
  }
  void Draw(TCanvas* can){
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
  
};

int artemis(){
   
  TCanvas* can = new TCanvas;
  
  ArtHists a1(100, "histo.hbook");
  a1.Draw(can);
  can->Print("plot_0", "gif");
  
  ArtHists a2(110, "histo.hbook");
  a2.Draw(can);
  can->Print("plot_1.ps");
  
  ArtHists a3(120, "histo.hbook");
  a3.Draw(can);
  can->Print("plot_2.ps");
  
  ArtHists a4(130, "histo.hbook");
  a4.Draw(can);
  can->Print("plot_3.ps");

  return 0;
}
