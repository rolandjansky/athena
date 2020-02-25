/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

void TileSamplingFraction_analysis()
{
  std::vector< std::string > thetalist={"-00","-05","+05","-10","+10","-15","+15","-20","+20","-25","+25","-30","+30","-35","+35",
                                        "-40","+40","-45","+45","-50","+50","-55","+55","-60","+60","-90","+90"};

  TH1F* SF=new TH1F("TileSamplingFractions","Tile Sampling Fractions",25,-62.5,62.5);
  SF->GetXaxis()->SetTitle("theta");
  SF->GetYaxis()->SetTitle("Sampling Fractions");
  SF->SetMinimum(30);
  SF->SetStats(0);
  for(auto theta : thetalist) {
    TFile* file=TFile::Open(Form("hist%s.root",theta.c_str()));
    if(!file) continue;
    if(!file->IsOpen()) continue;
    //cout<<theta<<" : Open"<<endl;
    //file->ls();
    TH1* h_ptgen=(TH1*)file->Get("h_ptgen");
    if(!h_ptgen) continue;
    TF1* thefun =(TF1*)file->Get("thefun");
    if(!thefun) continue;
    
    double invSF=(h_ptgen->GetMean())/(thefun->GetParameter(0));
    if(isnan(invSF)) continue;
    double invSFerr=(h_ptgen->GetMean()/thefun->GetParameter(0))*(thefun->GetParError(0)/thefun->GetParameter(0));
    if(isnan(invSFerr)) continue;
    
    double th=atof(theta.c_str());

    cout << "1/SF value (theta="<<th<<"): " << (h_ptgen->GetMean())/(thefun->GetParameter(0)) << "+/-" << (h_ptgen->GetMean()/thefun->GetParameter(0))*(thefun->GetParError(0)/thefun->GetParameter(0)) << " dErr % " <<  (thefun->GetParError(0)/thefun->GetParameter(0))*100 << endl;   
    
    int bin=SF->FindBin(th);
    SF->SetBinContent(bin,invSF);
    SF->SetBinError(bin,invSFerr);
  }
  TCanvas* c=new TCanvas("TileSamplingFractions","Tile Sampling Fractions");
  SF->Draw();
  c->SaveAs("TileSamplingFractions.png");
  
  TFile fout("TileSamplingFractions.root","recreate");
  SF->Write();
  fout.Close();
  
}
