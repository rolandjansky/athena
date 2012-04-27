// Macro for to print plots from the Calibration ntuple
//
// Useage: to run in batch mode, type
//         root -q -b 'MCP_RTT_makeHistos("physics")'
//
// Jan 2011     B. Di Micco 
//////////////////////////////////////////////////////////////////////////////

void MCP_RTT_makeHistos(char* inputName="physics")
{
//Set Some default styles for the histogram plots
  gStyle->SetMarkerStyle(8);
  gStyle->SetStatW(0.2);
  gStyle->SetStatH(0.2);
  gStyle->SetStatFontSize(0.07);
  gStyle->SetLabelSize(0.06);
  gStyle->SetOptFit(1111);  
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetTitleColor(0);
  gStyle->SetStatColor(0);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetMarkerSize(0.3);  
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(111111);




  printf("Input name = %s\n", inputName);
  char inputNtupleFile[256]; char outputHistosFile[256];
  sprintf(inputNtupleFile,"%s.root",inputName);
  sprintf(outputHistosFile,"%s_histos.root",inputName);
  printf("Input ntuple file = %s\n", inputNtupleFile);
  printf("output histos file = %s\n", outputHistosFile);

// open ntuple file
// create object histos

  TFile *fin = new TFile(inputNtupleFile,"READ");
  TTree *tree = (TTree*)fin->Get("physics");
  gROOT->ProcessLine(".L physics_MCP_RTT.C+");
  gROOT->ProcessLine(".L histos_MCP_RTT.C+");

  TFile *fout = new TFile(outputHistosFile,"RECREATE");
  histos myHistos(tree);
  myHistos.run();
  cout << "after running...." << endl;
  //TFile *fout = new TFile("MuonCalibNtuple_fromESD_histos.root","RECREATE");
 
  cout << "fout = " << fout << endl;
  fout->Write();
  fout->Close();
}

