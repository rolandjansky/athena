/**
 *  @autor Tomasz.Bold@cern.ch
 *  @brief NetLogger output generation script 
 *  This is a ROOT script (which can be compiled).
 *  It produces a text file which can be read in by the "nlv"
 *  NetLogger visyalisation tool. 
 *  It should be tun as in example below:
 *  root -l -b  to_netlog.C $rootfile "TIMER_NTUPLES/Lvl1Conversion_L2" Lvl1Conversion_L2.netlog
 *  where:
 *  root -l -b    -- is standard ROOT execution without splash screen (-l) in batch (-b)
 *  "TIMER_NTUPLES/Lvl1Conversion_L2"  -- is full name of the timing tree in the file
 *  Lvl1Conversion_L2.netlog           -- is a output file name
 */

#include <TTree.h>
#include <TApplication.h>
#include <iostream>
#include <fstream>



// format date
TString datestring ( UInt_t* t) {
  char buf[50];
  strftime(buf, 50, "%Y%m%d%H%M%S", localtime((time_t*)t));
  TString s;
  s.Form("DATE=%s.%06d",buf, t[1]);
  return s;
}


// process given tree
bool process(TTree *tt, const char* fname) 
{
  TObjArray brAbs;
  
  TIter next(tt->GetListOfBranches());
  TBranch::ResetCount();
  TBranch *branch;
  while ((branch = (TBranch*)next())) {
    // TString contains maybe?
    TString brname = branch->GetName();
    if ( brname.Index("absolute", 8) != kNPOS ) {
      brAbs.Add(branch);
    }
  }
  
  if ( brAbs.GetSize() == 0 ) {
    cerr << "No absolute time stamps available in this tree" << endl; 
    return false;
  }

  // allocate an array of timval
  UInt_t **tvArray = new UInt_t*[brAbs.GetEntries()];
  
  // fix addressing
  int i;
  for ( i = 0 ; i < brAbs.GetEntries(); i++ ) {
    tvArray[i] = new UInt_t [4];
    tt->SetBranchAddress( brAbs.At(i)->GetName(), tvArray[i] );
    //   cout <<  brAbs.At(i)->GetName() << endl;
  }

  // handle presence of Lvl1Id
  //
  int isPassInfo = 0;
  
  Int_t passInfo[2];
  Int_t &Lvl1Id = passInfo[0];
  Int_t &RoIId  = passInfo[1];
  if ( tt->GetBranch("PassInfo") ) {
    tt->SetBranchAddress("PassInfo", &passInfo );
    isPassInfo = 1;
    cerr << "PassInfo (Lvl1Id & RoIId) available" << endl;
  }
  
  // ouput file
  ofstream fout;
  fout.open(fname);
  
  Long64_t nEntries = tt->GetEntries();
  TString s;
  for (  Long64_t event = 0; event < nEntries; event++ ) {
    tt->GetEntry(event);
    for ( i = 0 ; i < brAbs.GetEntries(); i++ ) {

      s.Resize(0);
      s += datestring(tvArray[i]);
      s += " NL.EVNT=START_";
      s += brAbs.At(i)->GetName();
      if ( isPassInfo ) {
	s += " NL.Lvl1Id=";
	s += Lvl1Id;
	s += " NL.RoIId=";
	s += RoIId;
      }
	
      fout << s << endl;

      s.Resize(0);
      s += datestring(&tvArray[i][2]);
      s += " NL.EVNT=STOP_";
      s += brAbs.At(i)->GetName();
      if ( isPassInfo ) {
	s += " NL.Lvl1Id=";
	s += Lvl1Id;
	s += " NL.RoIId=";
	s += RoIId;
      }
      fout << s << endl;
    }
  } 
  return true;
}

// main of this script
// preparation, file opening, tree access etc.
void to_netlog() {
  using namespace std;
  cerr << "Opening file: " << gApplication->Argv()[gApplication->Argc()-3] << " ";
  TFile f(gApplication->Argv()[gApplication->Argc()-3]);
  if ( f.IsOpen() )
    cerr << "Success" << endl;
  else {
    cerr << "Failed, check file name and path, exitting now ..." << endl;
  gROOT->ProcessLine(".q");
  }
  
  cerr << "Getting TTree: " <<  gApplication->Argv()[gApplication->Argc()-2] << " ";
  TTree *t = (TTree*)f.Get(gApplication->Argv()[gApplication->Argc()-2]);
  if ( t )
    if ( t->Class() == TTree::Class() )
      cerr << "Success" << endl;
    else {
      cerr << "Failed, " << gApplication->Argv()[gApplication->Argc()-2] <<" not a TTree, exitting now ..." << endl;
      gROOT->ProcessLine(".q");
    }
      
  else {
    cerr << "Failed, check the name and path, exitting now ..." << endl;
    gROOT->ProcessLine(".q");
  }
  
  //  gROOT->ProcessLine(".L to_netlog.C+");
  
  cerr << "Producing output to file: " << gApplication->Argv()[gApplication->Argc()-1] << endl;
  process(t, gApplication->Argv()[gApplication->Argc()-1]);
  gROOT->ProcessLine(".q");
}
