// This is an example.
// BjetTree.C was generated using MakeClass

void main() {

  gSystem->Load("libCintex.so");
  Cintex::Enable();

  std::string incpath = gSystem->GetIncludePath();
  incpath += "-I${TestArea}/InstallArea/include/TriggerMenuNtuple";
  gSystem->SetIncludePath(incpath.c_str());
  gSystem->Load("${TestArea}/InstallArea/${CMTCONFIG}/lib/libTriggerMenuNtupleDict.so");

  gROOT->LoadMacro("BjetTree.C+");
  gROOT->LoadMacro("BjetAnalysis.C+");

  BjetAnalysis("bjet.root");
}

