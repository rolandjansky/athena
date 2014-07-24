/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUSF_CXX
#define TAUSF_CXX
#include "TauCorrUncert/TauSF.h"

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

ClassImp(TauSF)

TauSF::TauSF(const std::string& sharepath,
	     bool debug) :
  CommonSF(sharepath,
	   "sf",
	   debug),
  m_bUseHighPtUncert(false),
  m_sIDpt("_ptinc"),
  m_bUseInclusiveEta(false)
{
  std::string abspath;
  if(sharepath.size() != 0 && sharepath.compare(0,1,"/") != 0)
    abspath = (std::string)(getenv("PWD")) +  "/" + sharepath;
  else
    abspath = sharepath;

  m_mEVeto = new SFMAP();
  TFile* fEVeto = TFile::Open((abspath + "/eveto/sf.root").c_str(), "READ");
  this->ReadInputs(fEVeto, m_mEVeto);
  fEVeto->Close();
  delete fEVeto;

  m_mEVeto_3P = new SFMAP();
  TFile* fEVeto_3P = TFile::Open((abspath + "/eveto/sf.3p.root").c_str(), "READ");
  this->ReadInputs(fEVeto_3P, m_mEVeto_3P);
  fEVeto_3P->Close();
  delete fEVeto_3P;
}

double TauSF::GetIDSF(ID level,
		      double eta,
		      int prongness,
		      double pT)
{
  std::string workingPoint = "ID_SF_" + ConvertIDToString(level) + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  return this->GetIDValue(workingPoint, pT);
}

double TauSF::GetIDSysUnc(ID level,
			  double eta,
			  int prongness,
			  double pT){
  std::string workingPoint = "ID_SF_SysUnc_" + ConvertIDToString(level) + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  float lowptunc = this->GetIDValue(workingPoint, pT);
  if(m_bUseHighPtUncert && pT > 100.){
    float highpt_rel = 0.;
    float sf = this->GetIDSF(level,
			     eta,
			     prongness,
			     pT);
    if(prongness == 1)
      highpt_rel = 0.000137 * (pT - 100.);
    else
      highpt_rel = 0.0000783 * (pT - 100.);
    float highptunc = highpt_rel * sf;
    return sqrt(lowptunc * lowptunc + highptunc * highptunc);
  }
  return lowptunc;
}

double TauSF::GetIDStatUnc(ID level,
			     double eta,
			     int prongness,
			     double pT)
{
  std::string workingPoint = "ID_SF_StatUnc_" + ConvertIDToString(level) + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  return this->GetIDValue(workingPoint, pT);
}

double TauSF::GetExclIDSF(ID level,
			  double eta,
			  int prongness,
			  double pT)
{
  if(level == BDTTIGHT or level == LLHTIGHT)
    return GetIDSF(level,
		   eta,
		   prongness,
		   pT);

  std::string workingPoint = "ID_SF_" + ConvertIDToString(level) + "_excl" + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  return this->GetIDValue(workingPoint, pT);
}

double TauSF::GetExclIDSysUnc(ID level,
			      double eta,
			      int prongness,
			      double pT){
  if(level == BDTTIGHT or level == LLHTIGHT)
    return GetIDSysUnc(level,
		       eta,
		       prongness,
		       pT);
  std::string workingPoint = "ID_SF_SysUnc_" + ConvertIDToString(level) + "_excl" + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  float lowptunc = this->GetIDValue(workingPoint, pT);
  if(pT > 100.){
    float highpt_rel = 0.;
    float sf = this->GetExclIDSF(level,
				 eta,
				 prongness,
				 pT);
    if(prongness == 1)
      highpt_rel = 0.000137 * (pT - 100.);
      // highpt_rel = std::min(0.00011 * (pT / 1000. - 100.), 0.027);
    else
      highpt_rel = 0.0000783 * (pT - 100.);
      // highpt_rel = std::min(0.000076 * (pT / 1000. - 100.), 0.017);
    float highptunc = highpt_rel * sf;
    return sqrt(lowptunc * lowptunc + highptunc * highptunc);
  }
  return lowptunc;
}

double TauSF::GetExclIDStatUnc(ID level,
			       double eta,
			       int prongness,
			       double pT)
{
  if(level == BDTTIGHT or level == LLHTIGHT)
    return GetIDStatUnc(level,
			eta,
			prongness,
			pT);

  std::string workingPoint = "ID_SF_StatUnc_" + ConvertIDToString(level) + "_excl" + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  return this->GetIDValue(workingPoint, pT);
}

//wrapper for 1-prong eveto to not break backwards compatibility
double TauSF::GetEVetoSF(float trackEta,
			 ID level,
			 EVETO evLevel,
			 OLR olr)
{
  return this->GetEVetoSF(trackEta,
			  1,
			  level,
			  evLevel,
			  olr);
}

double TauSF::GetEVetoSFUnc(float trackEta,
			    ID level,
			    EVETO evLevel,
			    OLR olr,
			    int direction)
{
  return this->GetEVetoSFUnc(trackEta,
			     1,
			     level,
			     evLevel,
			     olr,
			     direction);
}

double TauSF::GetEVetoSF(float trackEta,
			 int nTrack,
			 ID level,
			 EVETO evLevel,
			 OLR olr)
{
  this->checkTrackEtaValidity(trackEta);
  SFMAP* mtmp = 0;
  if(nTrack == 1)
    mtmp = m_mEVeto;
  else
    mtmp = m_mEVeto_3P;
  if(nTrack == 3) {// do the conversion
    if(level != BDTMEDIUM || evLevel != LOOSE || olr != TIGHTPP){
      level = BDTOTHER;
      evLevel = OTHER;
      olr = OLROTHER;
    }
  }
  std::string workingPoint = "h_" + ConvertIDToString(level) + "_" + ConvertEVetoToString(evLevel) + "_" + ConvertOLRToString(olr) + "_SF";
  TH1F* h = (*mtmp)[workingPoint];
  int bin = h->FindFixBin(fabs(trackEta));
  return h->GetBinContent(bin);
}

double TauSF::GetEVetoSFUnc(float trackEta,
			    int nTrack,
			    ID level,
			    EVETO evLevel,
			    OLR olr,
			    int direction)
{
  this->checkTrackEtaValidity(trackEta);
  SFMAP* mtmp = 0;
  if(nTrack == 1)
     mtmp = m_mEVeto;
  else
    mtmp = m_mEVeto_3P;
  if(nTrack == 3) {// do the conversion
    if(level != BDTMEDIUM || evLevel != LOOSE || olr != TIGHTPP){
      level = BDTOTHER;
      evLevel = OTHER;
      olr = OLROTHER;
    }
  }
  std::string workingPoint = "h_" + ConvertIDToString(level) + "_" + ConvertEVetoToString(evLevel) + "_" + ConvertOLRToString(olr);
  direction == 1 ? workingPoint+="_EU" : workingPoint += "_ED";
  TH1F* h = (*mtmp)[workingPoint];
  int bin = h->FindFixBin(TMath::Abs(trackEta));
  return h->GetBinContent(bin);
}

void TauSF::checkTrackEtaValidity(float& trackEta)
{
  if(fabs(trackEta) > 2.47)
    {
      Warning("TauSF::checkTrackEtaValidity",
	      "Track eta out of bounds: %.2f (SFs only valid up to 2.47). Will use 2.46",
	      trackEta);

      trackEta = 2.46;
    }
}

std::string TauSF::ConvertEVetoToString(EVETO level)
{
  switch(level)
    {
    case NONE:
      return "None";
      break;
    case LOOSE:
      return "EleBDTLoose";
      break;
    case MEDIUM:
      return "EleBDTMedium";
      break;
    case TIGHT:
      return "EleBDTTight";
      break;
    case OTHER:
      return "EleBDTOther";
      break;
    default:
      std::cerr << "No valid eveto level passed. Breaking up";
      assert(false);
      break;
    }
  return "";
}

std::string TauSF::ConvertOLRToString(OLR level)
{
  switch(level)
    {
    case OLRNONE:
      return "None";
      break;
    case LOOSEPP:
      return "loosePP";
      break;
    case MEDIUMPP:
      return "mediumPP";
      break;
    case TIGHTPP:
      return "tightPP";
      break;
    case OLROTHER:
      return "other";
      break;
    default:
      std::cerr << "No valid olr level passed. Breaking up";
      assert(false);
      break;
    }
  return "";
}

std::string TauSF::ConvertIDToString(ID level)
{
  switch(level)
    {
    case IDNONE:
      return "None";
      break;
    case BDTLOOSE:
      return "JetBDTSigLoose";
      break;
    case BDTMEDIUM:
      return "JetBDTSigMedium";
      break;
    case BDTTIGHT:
      return "JetBDTSigTight";
      break;
    case BDTOTHER:
      return "JetBDTSigOther";
      break;
    case LLHLOOSE:
      return "tauJLLHLoose";
      break;
    case LLHMEDIUM:
      return "tauJLLHMedium";
      break;
    case LLHTIGHT:
      return "tauJLLHTight";
      break;
    case LLHFAIL:
      return "tauJLLH";
      break;
    case BDTFAIL:
      return "JetBDTSig";
      break;
    default:
      std::cerr << "No valid olr level passed. Breaking up";
      assert(false);
      break;
    }
  return "";
}

float TauSF::GetIDValue(const std::string& sWorkingPoint, const float& fPt){
  if(m_debug)
    std::cout << "Try to access histogram: " << sWorkingPoint << std::endl;
  TH1F* tmp = (*m_mSF)[sWorkingPoint];
  int iBin = tmp->FindBin(fPt);
  //check overflow:
  if(iBin == tmp->GetNbinsX() + 1)
    iBin -= 1;
  return tmp->GetBinContent(iBin);
}

void TauSF::SwitchOnPtBinnedIDSF(bool bSwitch){
  if(bSwitch)
    m_sIDpt = "_ptbin";
  else
    m_sIDpt = "_ptinc";
}

void TauSF::SwitchOnEtaBinnedIDSF(bool bSwitch){
  m_bUseInclusiveEta = bSwitch;
}

void TauSF::SwitchOnHighPtUncert(bool bSwitch){
  m_bUseHighPtUncert = bSwitch;
}

std::string TauSF::ConvertEtaToString(const float& fEta){
  if(m_bUseInclusiveEta)
    return "_etaAll";
  if(fabs(fEta) <= 1.5)
    return "_barrel";
  else if(fabs(fEta) > 1.5)
    return "_endcap";
  else
    assert(false && "Invalid eta provided. Breaking up ...");
  return "";
}

#endif

/*
#ifndef TAUSF_CXX
#define TAUSF_CXX
#include "TauCorrUncert/TauSF.h"

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

ClassImp(TauSF)

TauSF::TauSF(const std::string& sharepath,
	     bool debug) :
  m_bUseHighPtUncert(true),
  m_sIDpt("_ptinc"),
  m_bUseInclusiveEta(false),
  m_debug(debug)
{
  std::string abspath;
  if(sharepath.size() != 0 && sharepath.compare(0,1,"/") != 0)
    abspath = (std::string)(getenv("PWD")) +  "/" + sharepath;
  else
    abspath = sharepath;

  m_mSF = new SFMAP();
  TFile* fSF = TFile::Open((abspath + "/ID/sf.root").c_str(), "READ");
  this->ReadInputs(fSF, m_mSF);
  fSF->Close();
  delete fSF;

  m_mEVeto = new SFMAP();
  TFile* fEVeto = TFile::Open((abspath + "/eveto/sf.root").c_str(), "READ");
  this->ReadInputs(fEVeto, m_mEVeto);
  fEVeto->Close();
  delete fEVeto;

  m_mEVeto_3P = new SFMAP();
  TFile* fEVeto_3P = TFile::Open((abspath + "/eveto/sf.3p.root").c_str(), "READ");
  this->ReadInputs(fEVeto_3P, m_mEVeto_3P);
  fEVeto_3P->Close();
  delete fEVeto_3P;
}

double TauSF::GetIDSF(ID level,
		      double eta,
		      int prongness,
		      double pT)
{
  std::string workingPoint = "ID_SF_" + ConvertIDToString(level) + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  return this->GetIDValue(workingPoint, pT);
}

double TauSF::GetIDSysUnc(ID level,
			  double eta,
			  int prongness,
			  double pT){
  std::string workingPoint = "ID_SF_SysUnc_" + ConvertIDToString(level) + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  float lowptunc = this->GetIDValue(workingPoint, pT);
  if(pT > 100.){
    float highpt_rel = 0.;
    float sf = this->GetIDSF(level,
			     eta,
			     prongness,
			     pT);
    if(prongness == 1)
      highpt_rel = std::min(0.00011 * (pT - 100.), 0.027);
    else
      highpt_rel = std::min(0.000076 * (pT - 100.), 0.017);
    float highptunc = highpt_rel / sf;
    return sqrt(lowptunc * lowptunc + highptunc * highptunc);
  }
  return lowptunc;
}

double TauSF::GetIDStatUnc(ID level,
			     double eta,
			     int prongness,
			     double pT)
{
  std::string workingPoint = "ID_SF_StatUnc_" + ConvertIDToString(level) + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  return this->GetIDValue(workingPoint, pT);
}

double TauSF::GetExclIDSF(ID level,
			  double eta,
			  int prongness,
			  double pT)
{
  if(level == BDTTIGHT or level == LLHTIGHT)
    return GetIDSF(level,
		   eta,
		   prongness,
		   pT);

  std::string workingPoint = "ID_SF_" + ConvertIDToString(level) + "_excl" + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  return this->GetIDValue(workingPoint, pT);
}

double TauSF::GetExclIDSysUnc(ID level,
			      double eta,
			      int prongness,
			      double pT){
  if(level == BDTTIGHT or level == LLHTIGHT)
    return GetIDSysUnc(level,
		       eta,
		       prongness,
		       pT);
  std::string workingPoint = "ID_SF_SysUnc_" + ConvertIDToString(level) + "_excl" + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  float lowptunc = this->GetIDValue(workingPoint, pT);
  if(pT > 100.){
    float highpt_rel = 0.;
    float sf = this->GetExclIDSF(level,
				 eta,
				 prongness,
				 pT);
    if(prongness == 1)
      highpt_rel = std::min(0.00011 * (pT / 1000. - 100.), 0.027);
    else
      highpt_rel = std::min(0.000076 * (pT / 1000. - 100.), 0.017);
    float highptunc = highpt_rel / sf;
    return sqrt(lowptunc * lowptunc + highptunc * highptunc);
  }
  return lowptunc;
}

double TauSF::GetExclIDStatUnc(ID level,
			       double eta,
			       int prongness,
			       double pT)
{
  if(level == BDTTIGHT or level == LLHTIGHT)
    return GetIDStatUnc(level,
			eta,
			prongness,
			pT);

  std::string workingPoint = "ID_SF_StatUnc_" + ConvertIDToString(level) + "_excl" + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  return this->GetIDValue(workingPoint, pT);
}

//wrapper for 1-prong eveto to not break backwards compatibility
double TauSF::GetEVetoSF(float trackEta,
			 ID level,
			 EVETO evLevel,
			 OLR olr)
{
  return this->GetEVetoSF(trackEta,
			  1,
			  level,
			  evLevel,
			  olr);
}

double TauSF::GetEVetoSFUnc(float trackEta,
			    ID level,
			    EVETO evLevel,
			    OLR olr,
			    int direction)
{
  return this->GetEVetoSFUnc(trackEta,
			     1,
			     level,
			     evLevel,
			     olr,
			     direction);
}

double TauSF::GetEVetoSF(float trackEta,
			 int nTrack,
			 ID level,
			 EVETO evLevel,
			 OLR olr)
{
  this->checkTrackEtaValidity(trackEta);
  SFMAP* mtmp = 0;
  if(nTrack == 1)
    mtmp = m_mEVeto;
  else
    mtmp = m_mEVeto_3P;
  if(nTrack == 3) {// do the conversion
    if(level != BDTMEDIUM || evLevel != LOOSE || olr != TIGHTPP){
      level = BDTOTHER;
      evLevel = OTHER;
      olr = OLROTHER;
    }
  }
  std::string workingPoint = "h_" + ConvertIDToString(level) + "_" + ConvertEVetoToString(evLevel) + "_" + ConvertOLRToString(olr) + "_SF";
  TH1F* h = (*mtmp)[workingPoint];
  int bin = h->FindFixBin(fabs(trackEta));
  return h->GetBinContent(bin);
}

double TauSF::GetEVetoSFUnc(float trackEta,
			    int nTrack,
			    ID level,
			    EVETO evLevel,
			    OLR olr,
			    int direction)
{
  this->checkTrackEtaValidity(trackEta);
  SFMAP* mtmp = 0;
  if(nTrack == 1)
     mtmp = m_mEVeto;
  else
    mtmp = m_mEVeto_3P;
  if(nTrack == 3) {// do the conversion
    if(level != BDTMEDIUM || evLevel != LOOSE || olr != TIGHTPP){
      level = BDTOTHER;
      evLevel = OTHER;
      olr = OLROTHER;
    }
  }
  std::string workingPoint = "h_" + ConvertIDToString(level) + "_" + ConvertEVetoToString(evLevel) + "_" + ConvertOLRToString(olr);
  direction == 1 ? workingPoint+="_EU" : workingPoint += "_ED";
  TH1F* h = (*mtmp)[workingPoint];
  int bin = h->FindFixBin(TMath::Abs(trackEta));
  return h->GetBinContent(bin);
}

void TauSF::checkTrackEtaValidity(float& trackEta)
{
  if(fabs(trackEta) > 2.47)
    {
      Warning("TauSF::checkTrackEtaValidity",
	      "Track eta out of bounds: %.2f (SFs only valid up to 2.47). Will use 2.46",
	      trackEta);

      trackEta = 2.46;
    }
}

std::string TauSF::ConvertProngToString(int prongness)
{
  std::string prong = "";
  if(prongness == -1)
    return "_inc";
  prongness == 1 ? prong = "_1p" : prong = "_3p";
  return prong;
}

std::string TauSF::ConvertEVetoToString(EVETO level)
{
  switch(level)
    {
    case NONE:
      return "None";
      break;
    case LOOSE:
      return "EleBDTLoose";
      break;
    case MEDIUM:
      return "EleBDTMedium";
      break;
    case TIGHT:
      return "EleBDTTight";
      break;
    case OTHER:
      return "EleBDTOther";
      break;
    default:
      std::cerr << "No valid eveto level passed. Breaking up";
      assert(false);
      break;
    }
}

std::string TauSF::ConvertOLRToString(OLR level)
{
  switch(level)
    {
    case OLRNONE:
      return "None";
      break;
    case LOOSEPP:
      return "loosePP";
      break;
    case MEDIUMPP:
      return "mediumPP";
      break;
    case TIGHTPP:
      return "tightPP";
      break;
    case OLROTHER:
      return "other";
      break;
    default:
      std::cerr << "No valid olr level passed. Breaking up";
      assert(false);
      break;
    }
}

std::string TauSF::ConvertIDToString(ID level)
{
  switch(level)
    {
    case IDNONE:
      return "None";
      break;
    case BDTLOOSE:
      return "JetBDTSigLoose";
      break;
    case BDTMEDIUM:
      return "JetBDTSigMedium";
      break;
    case BDTTIGHT:
      return "JetBDTSigTight";
      break;
    case BDTOTHER:
      return "JetBDTSigOther";
      break;
    case LLHLOOSE:
      return "tauJLLHLoose";
      break;
    case LLHMEDIUM:
      return "tauJLLHMedium";
      break;
    case LLHTIGHT:
      return "tauJLLHTight";
      break;
    case LLHFAIL:
      return "tauJLLH";
      break;
    case BDTFAIL:
      return "JetBDTSig";
      break;
    default:
      std::cerr << "No valid olr level passed. Breaking up";
      assert(false);
      break;
    }
}

std::string TauSF::ConvertEtaToString(const float& fEta){
  if(m_bUseInclusiveEta)
    return "_etaAll";
  if(fabs(fEta) <= 1.5)
    return "_barrel";
  else if(fabs(fEta) > 1.5)
    return "_endcap";
  else
    assert(false && "Invalid eta provided. Breaking up ...");
}

float TauSF::GetIDValue(const std::string& sWorkingPoint, const float& fPt){
  if(m_debug)
    std::cout << "Try to access histogram: " << sWorkingPoint << std::endl;
  TH1F* tmp = (*m_mSF)[sWorkingPoint];
  int iBin = tmp->FindBin(fPt);
  //check overflow:
  if(iBin == tmp->GetNbinsX() + 1)
    iBin -= 1;
  return tmp->GetBinContent(iBin);
}

void TauSF::SwitchOnPtBinnedIDSF(bool bSwitch){
  if(bSwitch)
    m_sIDpt = "_ptbin";
  else
    m_sIDpt = "_ptinc";
}

void TauSF::SwitchOnEtaBinnedIDSF(bool bSwitch){
  m_bUseInclusiveEta = bSwitch;
}

void TauSF::SwitchOnHighPtUncert(bool bSwitch){
  m_bUseHighPtUncert = bSwitch;
}
#endif
*/
