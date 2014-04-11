////////////////////////////////////////////////////////////
// OKButton.C
// 
// last modified 2004/06/23
//
// Message button that will be displayed when a monitoring
// menu button is clicked which refers to a TDirectory in
// the root file that does not exist.  
// It receives a error message (of type TString) that
// is displayed on the button.
////////////////////////////////////////////////////////////

#include <iostream>

#include "TControlBar.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

void OKButton(TString OKMessage="This is probably the best button to press") {

  TControlBar* OKBar = new TControlBar("vertical", "Read this button!", 300, 300);

  // fancy stringToExcute that will delete the TControlBar
  TString stringToExecute = "TControlBar* bar = (TControlBar*)";
  stringToExecute += (int)OKBar;
  stringToExecute += "; bar->Delete()";

  cout << OKMessage << endl;
  OKBar->AddButton(OKMessage + ": OK?", stringToExecute, "OK?");

  OKBar->Show();
  gROOT->SaveContext();
}

