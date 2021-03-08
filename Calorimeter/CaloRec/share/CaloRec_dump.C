void CaloRec_dump()
{
dumpFile("ntuple.root");

}


void dumpFile (TString theFile)
{
  
// simplistic root macro to dump variables from combined ntuple
// to allow simle integrity checks


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

// only 3 digits
TGaxis::SetMaxDigits(3);

//gStyle->SetNoExponent(false);
//myHisto->GetYaxis()


//open file
TFile f(theFile);

f.cd("CBNT") ;
// this is needed only if pure numeric ttree identifier is used
TTree *t3333=(TTree*) gDirectory->Get("t3333");

//use default style
t3333->UseCurrentStyle();

TFile *hfile = new TFile("CBNTDump.hist","RECREATE","CBNT dump");

TH1F *h1 = new TH1F("h1", "MemVSize histo",1,0,5);


TCanvas *MyC5 = new TCanvas("MyC5","CBNT Dump 5",1);




MyC5->Divide(2,2);
MyC5->cd(1);
t3333->Draw("MemVSize:IEvent","MemVSize>0");
t3333->Draw("MemVSize:IEvent>>h1","MemVSize>0");
hfile->Write();

MyC5->cd(2);
t3333->Draw("MemRSS:IEvent","MemVSize>0");
MyC5->cd(3);
t3333->Draw("TimeTotal/1000");
MyC5->cd(4);
t3333->Draw("(TimeTotal-TimeSum)/1000");

MyC5->Print("CBNTdump.ps)");
 
hfile->Close();

}

