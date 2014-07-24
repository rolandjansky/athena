/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BDTSF_CXX
#define BDTSF_CXX
#include "TauCorrUncert/BDTSF.h"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <string>

#include "TDirectory.h"
#include "TParameter.h"
#include "TKey.h"

using namespace TauCorrUncert;

ClassImp(BDTSF)

BDTSF::BDTSF(const std::string& sharepath,
	     bool debug) :
CommonSF(sharepath,
	 "bdtsf",
	 debug)
{}

// Central Value
double BDTSF::getBDTbinnedSF(int prongness,
			     const double& BDT)
{
  std::string workingPoint = "ID_SF" + ConvertProngToString(prongness);
  return this->GetBDTbinnedSFValue(workingPoint, BDT);
}

double BDTSF::getBDTbinnedSF(int prongness,
			     const double& eta,
			     const double& BDT)
{
  std::string workingPoint = "ID_SF" + ConvertProngToString(prongness) + ConvertEtaToString(eta);
  return this->GetBDTbinnedSFValue(workingPoint, BDT);
}

// Total Systematic Uncertainty
double BDTSF::getBDTbinnedSFSysUnc(int prongness,
				   const double& BDT,
				   const std::string& direction,
				   SYS sys)
{
  std::string workingPoint = "ID_SF_sys" + ConvertSystematicToString(sys) + ConvertDirectionToString(direction) + ConvertProngToString(prongness);
  return this->GetBDTbinnedSFValue(workingPoint, BDT);
}


double BDTSF::getBDTbinnedSFSysUnc(int prongness,
				   const double& eta,
				   const double& BDT,
				   const std::string& direction,
				   SYS sys)
{
  std::string workingPoint = "ID_SF_sys" + ConvertSystematicToString(sys) + ConvertDirectionToString(direction) + ConvertProngToString(prongness) + ConvertEtaToString(eta);
  return this->GetBDTbinnedSFValue(workingPoint, BDT);
}
// Statistical Error
double BDTSF::getBDTbinnedSFStatUnc(int prongness,
				    const double& BDT,
				    const std::string& direction)
{
  std::string workingPoint = "ID_SF_stat" + ConvertDirectionToString(direction) + ConvertProngToString(prongness);
  return this->GetBDTbinnedSFValue(workingPoint, BDT);
}

double BDTSF::getBDTbinnedSFStatUnc(int prongness,
				    const double& eta,
				    const double& BDT,
				    const std::string& direction)
{

  std::string workingPoint = "ID_SF_stat" + ConvertDirectionToString(direction) + ConvertProngToString(prongness) + ConvertEtaToString(eta);
  return this->GetBDTbinnedSFValue(workingPoint, BDT);
}

// Return Histogram Names

std::string BDTSF::ConvertDirectionToString(const std::string& direction){
  if(direction == "UP")
    return "_UP";
  else if (direction == "DOWN")
    return "_DOWN";
  else
    assert(false && "Statistical/Systematic direction must be UP or DOWN. Breaking up ...");
  return "";
}

float BDTSF::GetBDTbinnedSFValue(const std::string& sWorkingPoint, const float& fBDT){
  if(m_debug)
    std::cout << "Try to access histogram: " << sWorkingPoint << std::endl;
  if (fBDT < 0.4 || fBDT > 1.0)
    {
      Warning("BDTSF::GetBDTbinnedSFValue",
	      "BDT Value out of bounds: %.2f (SFs only valid between 0.4 and 1.0). Returning a SF of 1.0",
	      fBDT);
      return 1.0;
    }
  TH1F* tmp = (*m_mSF)[sWorkingPoint];
  int iBin = tmp->FindBin(fBDT);
  //check overflow:
  if(iBin == tmp->GetNbinsX() + 1)
    iBin -= 1;
  return tmp->GetBinContent(iBin);
}

std::string BDTSF::ConvertEtaToString(const float& fEta){
  if(fabs(fEta) <= 1.37)
    return "_barrel";
  else if(fabs(fEta) > 1.37)
    return "_endcap";
  else
    assert(false && "Invalid eta provided. Breaking up ...");
  return "";
}


std::string BDTSF::ConvertSystematicToString(SYS sys)
{
  switch(sys)
    {
    case TOTAL:
      return "";
      break;
    case QCD:
      return "_QCD";
      break;
    case WJETS:
      return "_Wjets";
      break;
    case TES:
      return "_TES";
      break;
    case REMAIN:
      return "_Other";
      break;
    default:
      std::cerr << "No valid systematic uncertainty component passed. Breaking up";
      assert(false);
      break;
    }
  return "";
}

#endif
