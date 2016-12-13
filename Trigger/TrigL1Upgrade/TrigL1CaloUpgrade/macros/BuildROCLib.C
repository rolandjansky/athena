/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <unistd.h>

void BuildROC( const TH1F* h1, const TH1F* h2, TH2F*& ROC,char* hn,bool inverse = false){

//hc1 = (TH1F*)h1->Clone();
//hc2 = (TH1F*)h2->Clone();

// now ROC
TH2F* ROC1 = new TH2F(hn,hn,400,0,100,400,0,100.);
ROC1->SetXTitle("jet fake rate");
ROC1->SetYTitle("electron efficiency");
for(Int_t i=0;i<h1->GetNbinsX();i++){
float X,Y;
if (!inverse) {
X = h1->Integral(i,h1->GetNbinsX()+1); // / hc1->Integral(0,hc1->GetNbinsX());
Y = h2->Integral(i,h2->GetNbinsX()+1); // / hc2->Integral(0,hc1->GetNbinsX());
} else {
X = h1->Integral(0,i); // / hc1->Integral(0,hc1->GetNbinsX());
Y = h2->Integral(0,i); // / hc2->Integral(0,hc1->GetNbinsX());
}
cout << "For parameter : " << h1->GetBinCenter(i);
cout << "; efficiency : " << X << "; fake rate : " << Y << endl;
ROC1->Fill(Y,X);
}
ROC = ROC1;
return ;

}


