void RecExCommon_dump()
{
dumpFile("ntuple.root");

}


void dumpFile (TString theFile)
{

  // 1 if CBNTAA
 int CBNTAA=1;
  
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

 TString thePSFile=theFile+".ps";
 
 std::cout << "thePSfile: " << thePSFile << std::endl;
 

// only 3 digits
TGaxis::SetMaxDigits(3);

//gStyle->SetNoExponent(false);
//myHisto->GetYaxis()


  //open file
TFile f(theFile);

 TTree *t3333=0;

 /// handle AA vs non AA
 if (CBNTAA==1) {
// this is needed only if pure numeric ttree identifier is used
   t3333=(TTree*) gDirectory->Get("CollectionTree");
 } else {
   f.cd("CBNT") ;
   t3333=(TTree*) gDirectory->Get("t3333");
 }
//use default style
t3333->UseCurrentStyle();



 

TCanvas *MyC1 = new TCanvas("MyC1","CBNT Dump ",1);

 
MyC1->Divide(2,2);

MyC1->cd(1);
t3333->Draw("PtGen");
MyC1->cd(2);
t3333->Draw("Trk_pt");
MyC1->cd(3);
t3333->Draw("Trk_pt:PtGen[Trk_truthNt]","Trk_truthBarcode!=0");
MyC1->cd(4);
t3333->Draw("vxp_vtx_z:ZV[IVPrimary]","NVer>0");

thePSFileOpen=thePSFile+"(";
MyC1->Print(thePSFileOpen); 
 
#

TCanvas *MyC2 = new TCanvas("MyC2","CBNT Dump mu",1);

MyC2->Divide(2,2);
MyC2->cd(1);
t3333->Draw("1./VT_PTInv");
MyC2->cd(2);
t3333->Draw("1./Moore_ptir");
MyC2->cd(3);
t3333->Draw("1./staco_qOverP");
MyC2->cd(4);
t3333->Draw("1./Muid_pircb");


//MyC2->Print("CBNTdump.ps");
MyC2->Print(thePSFile); 


TCanvas *MyC3 = new TCanvas("MyC3","CBNT Dump conv ele tau bt",1);

 


MyC3->Divide(2,2);
MyC3->cd(1);
t3333->Draw("sqrt(conv_vtx_x**2+conv_vtx_y**2)");
MyC3->cd(2);
t3333->Draw("Ele_eta");
MyC3->cd(3);
t3333->Draw("tau_eta");
MyC3->cd(4);
t3333->Draw("pjWSV2");

//MyC3->Print("CBNTdump.ps");
MyC3->Print(thePSFile); 



TCanvas *MyC4 = new TCanvas("MyC4","CBNT Dump MET",1);




MyC4->Divide(2,2);
MyC4->cd(1);
t3333->Draw("MET_EtSumFinal");
MyC4->cd(2);
t3333->Draw("MET_EtSumTruthInt");
MyC4->cd(3);
t3333->Draw("METSig_EtMissFinal");


//MyC4->Print("CBNTdump.ps");
MyC4->Print(thePSFile); 




TCanvas *MyC5 = new TCanvas("MyC5","CBNT Dump jet",1);


MyC5->Divide(2,2);


MyC5->cd(1);
t3333->Draw("jetEtaC7H1TopoJets");
MyC5->cd(2);
t3333->Draw("jetEtaCone7H1TowerJets");
MyC5->cd(3);
t3333->Draw("jetEtaKt6H1TowerJets");
MyC5->cd(4);
t3333->Draw("jetEtaC7McJets");


MyC5->Print(thePSFile); 




TCanvas *MyC6 = new TCanvas("MyC6","CBNT Dump soft",1);


MyC6->Divide(2,2);
MyC6->cd(1);
t3333->Draw("MemVSize:IEvent","MemVSize>0");
MyC6->cd(2);
t3333->Draw("MemRSS:IEvent","MemVSize>0");
MyC6->cd(3);
t3333->Draw("TimeTotal/1000");
MyC6->cd(4);
t3333->Draw("(TimeTotal-TimeSum)/1000");


thePSFileClose=thePSFile+")";
MyC6->Print(thePSFileClose); 

//t3333->Browse()
}

