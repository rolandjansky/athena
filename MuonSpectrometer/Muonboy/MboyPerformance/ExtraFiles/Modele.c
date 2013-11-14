/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Modele.h"
#include "TH2.h"
#include "TProfile.h"
#include "TF1.h"
#include "TDatime.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "math.h"
#include "TMath.h"
#include "iostream.h"
#include "TPostScript.h"
#include "TPaveLabel.h"
#include "TPaveText.h"
#include "TGraph.h"
#include "TGaxis.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TGraphErrors.h"


Modele::Modele():Analysis() {

  m_Debug = kFALSE;

  m_nevt = 0;

}


Modele::~Modele() {}

void Modele::initialize(char* option) {
//
  printf("Starting Modele\n");
  if( strstr(option, "debug") != NULL )  m_Debug = kTRUE;

}


void Modele::execute() {
  m_nevt = m_nevt + 1;
}

void Modele::finalize() {
}
