/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COMMONSF_CXX
#define COMMONSF_CXX
#include "TauCorrUncert/CommonSF.h"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "TDirectory.h"
#include "TParameter.h"
#include "TKey.h"

using namespace TauCorrUncert;

ClassImp(CommonSF)

CommonSF::CommonSF(const std::string& sharepath,
		   const std::string& sSFFileName,
		   bool debug) :
  m_debug(debug)
{
  std::string abspath;
  if(sharepath.size() != 0 && sharepath.compare(0,1,"/") != 0)
    abspath = (std::string)(getenv("PWD")) +  "/" + sharepath;
  else
    abspath = sharepath;

  m_mSF = new SFMAP();
  TFile* fSF = TFile::Open((abspath + "/ID/" + sSFFileName + ".root").c_str(), "READ");
  this->ReadInputs(fSF, m_mSF);
  fSF->Close();
  delete fSF;
}

CommonSF::~CommonSF()
{
  delete m_mSF;
}

std::string CommonSF::ConvertProngToString(int prongness)
{
  std::string prong = "";
  if(prongness == -1)
    return "_inc";
  prongness == 1 ? prong = "_1p" : prong = "_3p";
  return prong;
}
#endif
