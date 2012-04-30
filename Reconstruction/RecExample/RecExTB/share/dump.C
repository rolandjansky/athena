{


// change default style 
// round filled marker

gStyle->SetMarkerStyle(8);
gStyle->SetStatW(0.4);
//gStyle->SetStatFontSize(60);
gStyle->SetStatFontSize(0.12);
gStyle->SetLabelSize(0.06);
gStyle->SetOptStat(111110);
gStyle->SetCanvasBorderMode(0);
gStyle->SetPadBorderMode(0);
gStyle->SetPadColor(0);
gStyle->SetCanvasColor(0);
gStyle->SetTitleColor(0);
gStyle->SetStatColor(0);



  //open file
TFile f("ntuple.root");

f.cd("CBNT") ;
// this is needed only if pure numeric ttree identifier is used
// TTree *t3333=(TTree*) gDirectory->Get("t3333");

//use default style
t3333->UseCurrentStyle();

t3333->Show(0);

TCanvas *myC1 = new TCanvas("myC1","TBeam Dump",1);

myC1->Divide(1,2);
myC1->cd(1);

t3333->Draw("ECells");
myC1->cd(2);
t3333->Draw("ecluster");


}
