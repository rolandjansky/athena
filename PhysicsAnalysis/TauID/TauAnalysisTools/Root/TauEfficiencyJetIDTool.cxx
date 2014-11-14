/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUEFFICIENCYTOOL_CXX
#define TAUEFFICIENCYTOOL_CXX

// #include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/TauEfficiencyJetIDTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauEfficiencyJetIDTool::TauEfficiencyJetIDTool(std::string sName) :
  CommonEfficiencyTool(sName)
{
  m_sSystematicSet = new CP::SystematicSet();
}

TauEfficiencyJetIDTool::~TauEfficiencyJetIDTool()
{
}

//______________________________________________________________________________
StatusCode TauEfficiencyJetIDTool::initialize()
{
  ATH_MSG_INFO("Initializing TauEfficiencyJetIDTool");

  m_mSystematics =
  {
    {"TAUS_EFF_JETID_STAT", STAT}
    , {"TAUS_EFF_JETID_SYST", SYST}
  };

  return CommonEfficiencyTool::initialize();
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::getEfficiencyScaleFactor(const xAOD::TauJet& xTau,
    double& dEfficiencyScaleFactor)
{
  m_sIDLevel = ConvertIDToString(m_tTECT->m_iIDLevel);
  SwitchOnPtBinnedIDSF(m_tTECT->m_bUsePtBinnedSF);

  // retreive ID SF value
  if (m_tTECT->m_bUseIDExclusiveSF)
    return getExclTotalSF(xTau, dEfficiencyScaleFactor);
  // else
  return getTotalSF(xTau, dEfficiencyScaleFactor);
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::applyEfficiencyScaleFactor(const xAOD::TauJet& xTau)
{
  double dSf = 0.;
  // retreive scale factor
  CP::CorrectionCode tmpCorrectionCode = getEfficiencyScaleFactor(xTau, dSf);
  // adding scale factor to auxdecor
  xTau.auxdecor< double >( m_tTECT->m_sVarNameBase ) = dSf;

  return tmpCorrectionCode;
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::getEfficiencyScaleFactorStatUnc(const xAOD::TauJet& xTau, double& dStatUnc)
{
  m_sIDLevel = ConvertIDToString(m_tTECT->m_iIDLevel);
  SwitchOnPtBinnedIDSF(m_tTECT->m_bUsePtBinnedSF);
  if (m_tTECT->m_bUseIDExclusiveSF)
    return GetExclIDStatUnc(dStatUnc, xTau.p4().Eta(), xTau.nTracks(), xTau.p4().Pt());
  else
    return GetIDStatUnc(dStatUnc, xTau.p4().Eta(), xTau.nTracks(), xTau.p4().Pt());
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::applyEfficiencyScaleFactorStatUnc(const xAOD::TauJet& xTau)
{
  double dStatUnc = 0;
  // retreive ID SF value
  CP::CorrectionCode tmpCorrectionCode = getEfficiencyScaleFactorStatUnc(xTau, dStatUnc);

  std::string sVarName = m_tTECT->m_sVarNameBase + "StatUnc";
  if (m_tTECT->m_iSysDirection == 1)
    sVarName += "_Up";
  else if (m_tTECT->m_iSysDirection == -1)
    sVarName += "_Down";
  else
  {
    ATH_MSG_WARNING("SysDirection "<<m_tTECT->m_iSysDirection<<" is not supported, need to be 1 or -1");
    return CP::CorrectionCode::Error;
  }

  // adding stat unc to auxdecor
  xTau.auxdecor< double >( sVarName ) = dStatUnc;

  return tmpCorrectionCode;
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::getEfficiencyScaleFactorSysUnc(const xAOD::TauJet& xTau, double& dSysUnc)
{
  m_sIDLevel = ConvertIDToString(m_tTECT->m_iIDLevel);
  SwitchOnPtBinnedIDSF(m_tTECT->m_bUsePtBinnedSF);
  CP::CorrectionCode tmpCorrectionCode;
  if (m_tTECT->m_bUseIDExclusiveSF)
    return GetExclIDSysUnc(dSysUnc, xTau.p4().Eta(), xTau.nTracks(), xTau.p4().Pt());
  // else
  return GetIDSysUnc(dSysUnc, xTau.p4().Eta(), xTau.nTracks(), xTau.p4().Pt());
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::applyEfficiencyScaleFactorSysUnc(const xAOD::TauJet& xTau)
{
  double dSysUnc = 0.;
  // retreive ID SF value
  CP::CorrectionCode tmpCorrectionCode = getEfficiencyScaleFactorSysUnc(xTau, dSysUnc);

  std::string sVarName = m_tTECT->m_sVarNameBase + "SysUnc";
  if (m_tTECT->m_iSysDirection == 1)
    sVarName += "_Up";
  else if (m_tTECT->m_iSysDirection == -1)
    sVarName += "_Down";
  else
  {
    ATH_MSG_WARNING("SysDirection "<<m_tTECT->m_iSysDirection<<" is not supported, need to be 1 or -1");
    return CP::CorrectionCode::Error;
  }

  // adding sys unc to auxdecor
  xTau.auxdecor< double >( sVarName ) = dSysUnc;

  return tmpCorrectionCode;
}

bool TauEfficiencyJetIDTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const
{
  CP::SystematicSet sys = affectingSystematics();
  return sys.find (systematic) != sys.end ();
}

CP::SystematicSet TauEfficiencyJetIDTool::affectingSystematics() const
{
  CP::SystematicSet result = recommendedSystematics();
  return result;
}

CP::SystematicSet TauEfficiencyJetIDTool::recommendedSystematics() const
{
  CP::SystematicSet result;
  result.insert (CP::SystematicVariation ("TAUS_EFF_JETID_STAT", 1));
  result.insert (CP::SystematicVariation ("TAUS_EFF_JETID_STAT", -1));
  result.insert (CP::SystematicVariation ("TAUS_EFF_JETID_SYST", 1));
  result.insert (CP::SystematicVariation ("TAUS_EFF_JETID_SYST", -1));
  return result;
}

CP::SystematicCode TauEfficiencyJetIDTool::applySystematicVariation ( const CP::SystematicSet& sSystematicSet)
{
  m_sSystematicSet = &sSystematicSet;
  return CP::SystematicCode::Ok;
}



//=================================PRIVATE-PART=================================
//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::getTotalSF(const xAOD::TauJet& xTau,
    double& dEfficiencyScaleFactor)
{
  CP::CorrectionCode tmpCorrectionCode = GetIDSF(dEfficiencyScaleFactor, xTau.p4().Eta(), xTau.nTracks(), xTau.p4().Pt());
  if(tmpCorrectionCode != CP::CorrectionCode::Ok)
    return tmpCorrectionCode;
  // sqaure of total systematic

  if (m_sSystematicSet->size() == 1)
  {
    for (auto syst : *m_sSystematicSet)
    {
      auto it = m_mSystematics.find(syst.basename());
      if (it == m_mSystematics.end())
      {
        ATH_MSG_VERBOSE("unsupported systematic variation: skipping this one");
        continue;
      }

      CP::CorrectionCode tmpCorrectionCodeSys = CP::CorrectionCode::Error;

      double dSystematic = 0;
      if (it->second == STAT)
        tmpCorrectionCodeSys = GetIDStatUnc(dSystematic, xTau.p4().Eta(), xTau.nTracks(), xTau.p4().Pt());
      if (it->second == SYST)
        tmpCorrectionCodeSys = GetIDSysUnc(dSystematic, xTau.p4().Eta(), xTau.nTracks(), xTau.p4().Pt());

      if (tmpCorrectionCodeSys != CP::CorrectionCode::Ok)
        return tmpCorrectionCodeSys;

      dEfficiencyScaleFactor += syst.parameter() * dSystematic;

    }
  }

  if (m_sSystematicSet->size() == 2)
  {
    double dDirection = 0;
    double dTotalSystematic2 = 0;
    for (auto syst : *m_sSystematicSet)
    {
      auto it = m_mSystematics.find(syst.basename());
      if (it == m_mSystematics.end())
      {
        ATH_MSG_VERBOSE("unsupported systematic variation: skipping this one");
        continue;
      }

      // test for change in systematics direction
      if (dDirection*syst.parameter() < 0)
      {
        ATH_MSG_ERROR("unsupported set of systematic variations, you should either use only \"UP\" or only \"DOWN\" systematics in a set!");
        ATH_MSG_ERROR("systematic variation will not be applied");
        return CP::CorrectionCode::OutOfValidityRange;
      }
      dDirection = syst.parameter();

      // get scale factor uncertainties and add them in quadrature

      CP::CorrectionCode tmpCorrectionCodeSys = CP::CorrectionCode::Error;

      double dSystematic = 0;
      if (it->second == STAT)
        tmpCorrectionCodeSys = GetIDStatUnc(dSystematic, xTau.p4().Eta(), xTau.nTracks(), xTau.p4().Pt());
      else if (it->second == SYST)
        tmpCorrectionCodeSys = GetIDSysUnc(dSystematic, xTau.p4().Eta(), xTau.nTracks(), xTau.p4().Pt());
      else
      {
        ATH_MSG_VERBOSE("unsupported systematic variation: skipping this one");
        continue;
      }

      if (tmpCorrectionCodeSys != CP::CorrectionCode::Ok)
        return tmpCorrectionCodeSys;

      dSystematic *= syst.parameter();
      dTotalSystematic2 += dSystematic*dSystematic;
    }
    // now use dDirection to use up/down uncertainty
    dDirection = (dDirection > 0) ? +1 : -1;
    dEfficiencyScaleFactor += dDirection * sqrt(dTotalSystematic2);
  }

  return tmpCorrectionCode;
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::getExclTotalSF(const xAOD::TauJet& xTau,
    double& dEfficiencyScaleFactor)
{
  CP::CorrectionCode tmpCorrectionCode = GetExclIDSF(dEfficiencyScaleFactor, xTau.p4().Eta(), xTau.nTracks(), xTau.p4().Pt());
  if(tmpCorrectionCode != CP::CorrectionCode::Ok) return tmpCorrectionCode;
  // sqaure of total systematic

  if (m_sSystematicSet->size() == 1)
  {
    for (auto syst : *m_sSystematicSet)
    {
      auto it = m_mSystematics.find(syst.basename());
      if (it == m_mSystematics.end())
      {
        ATH_MSG_VERBOSE("unsupported systematic variation: skipping this one");
        continue;
      }

      CP::CorrectionCode tmpCorrectionCodeSys = CP::CorrectionCode::Error;

      double dSystematic = 0;
      if (it->second == STAT)
        tmpCorrectionCodeSys = GetExclIDStatUnc(dSystematic, xTau.p4().Eta(), xTau.nTracks(), xTau.p4().Pt());
      else if (it->second == SYST)
        tmpCorrectionCodeSys = GetExclIDSysUnc(dSystematic, xTau.p4().Eta(), xTau.nTracks(), xTau.p4().Pt());
      else
      {
        ATH_MSG_VERBOSE("unsupported systematic variation: skipping this one");
        continue;
      }

      if (tmpCorrectionCodeSys != CP::CorrectionCode::Ok)
        return tmpCorrectionCodeSys;

      dEfficiencyScaleFactor += syst.parameter() * dSystematic;
    }
  }

  if (m_sSystematicSet->size() == 2)
  {
    double dDirection = 0;
    double dTotalSystematic2 = 0;
    for (auto syst : *m_sSystematicSet)
    {
      auto it = m_mSystematics.find(syst.basename());
      if (it == m_mSystematics.end())
      {
        ATH_MSG_VERBOSE("unsupported systematic variation: skipping this one");
        continue;
      }

      // test for change in systematics direction
      if (dDirection*syst.parameter() < 0)
      {
        ATH_MSG_ERROR("unsupported set of systematic variations, you should either use only \"UP\" or only \"DOWN\" systematics in a set!");
        ATH_MSG_ERROR("systematic variation will not be applied");
        return CP::CorrectionCode::OutOfValidityRange;
      }
      dDirection = syst.parameter();

      // get scale factor uncertainties and add them in quadrature
      CP::CorrectionCode tmpCorrectionCodeSys = CP::CorrectionCode::Error;

      double dSystematic = 0;
      if (it->second == STAT)
        tmpCorrectionCodeSys = GetExclIDStatUnc(dSystematic, xTau.p4().Eta(), xTau.nTracks(), xTau.p4().Pt());
      else if (it->second == SYST)
        tmpCorrectionCodeSys = GetExclIDSysUnc(dSystematic, xTau.p4().Eta(), xTau.nTracks(), xTau.p4().Pt());
      else
      {
        ATH_MSG_VERBOSE("unsupported systematic variation: skipping this one");
        continue;
      }

      if (tmpCorrectionCodeSys != CP::CorrectionCode::Ok)
        return tmpCorrectionCodeSys;

      dSystematic *= syst.parameter();
      dTotalSystematic2 += dSystematic*dSystematic;
    }
    // now use dDirection to use up/down uncertainty
    dDirection = (dDirection > 0) ? +1 : -1;
    dEfficiencyScaleFactor += dDirection * sqrt(dTotalSystematic2);
  }

  return tmpCorrectionCode;
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::GetIDSF(double& val,
    double eta,
    int prongness,
    double pT)
{
  m_sWorkingPoint = "ID_SF_" + m_sIDLevel + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  ATH_MSG_DEBUG("using working point: "+m_sWorkingPoint);

  return this->GetIDValue(val, m_sWorkingPoint, pT);
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::GetIDSysUnc(double& val,
    double eta,
    int prongness,
    double pT)
{
  m_sWorkingPoint = "ID_SF_SysUnc_" + m_sIDLevel + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  double lowptunc = 0;
  CP::CorrectionCode tmp_correctioncode_unc;
  tmp_correctioncode_unc = this->GetIDValue(lowptunc, m_sWorkingPoint, pT);
  if (tmp_correctioncode_unc != CP::CorrectionCode::Ok)
    return tmp_correctioncode_unc;

  if(m_tTECT->m_bUseHighPtUncert && pT/1000. > 100.)
  {
    double highpt_rel = 0.;
    double sf = 0;
    CP::CorrectionCode tmp_correctioncode_sf;
    tmp_correctioncode_sf = this->GetIDSF(sf,
                                          eta,
                                          prongness,
                                          pT);
    if (tmp_correctioncode_sf != CP::CorrectionCode::Ok)
      return tmp_correctioncode_sf;

    if(prongness == 1)
      highpt_rel = 0.000137 * (pT/1000. - 100.);
    else
      highpt_rel = 0.0000783 * (pT/1000. - 100.);
    double highptunc = highpt_rel * sf;
    val = sqrt(lowptunc * lowptunc + highptunc * highptunc);
    return tmp_correctioncode_sf; // one correction code return should be enough
  }
  val = lowptunc;
  return tmp_correctioncode_unc;
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::GetIDStatUnc(double& val,
    double eta,
    int prongness,
    double pT)
{
  m_sWorkingPoint = "ID_SF_StatUnc_" + m_sIDLevel + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  return this->GetIDValue(val, m_sWorkingPoint, pT);
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::GetExclIDSF(double& val,
    double eta,
    int prongness,
    double pT)
{
  if(m_tTECT->m_iIDLevel == JETIDBDTTIGHT or m_tTECT->m_iIDLevel == JETIDLLHTIGHT)
    return GetIDSF(eta,
                   prongness,
                   pT);

  m_sWorkingPoint = "ID_SF_" + m_sIDLevel + "_excl" + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  return this->GetIDValue(val, m_sWorkingPoint, pT);
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::GetExclIDSysUnc(double& val,
    double eta,
    int prongness,
    double pT)
{
  if(m_tTECT->m_iIDLevel == JETIDBDTTIGHT or m_tTECT->m_iIDLevel == JETIDLLHTIGHT)
    return GetIDSysUnc(eta,
                       prongness,
                       pT);
  m_sWorkingPoint = "ID_SF_SysUnc_" + m_sIDLevel + "_excl" + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  double lowptunc = 0;
  CP::CorrectionCode tmp_correctioncode_unc;
  tmp_correctioncode_unc = this->GetIDValue(lowptunc, m_sWorkingPoint, pT);
  if (tmp_correctioncode_unc != CP::CorrectionCode::Ok)
    return tmp_correctioncode_unc;

  if(m_tTECT->m_bUseHighPtUncert && pT/1000. > 100.)
  {
    double highpt_rel = 0.;
    double sf = 0;
    CP::CorrectionCode tmp_correctioncode_sf;
    tmp_correctioncode_sf = this->GetExclIDSF(sf,
                            eta,
                            prongness,
                            pT);
    if (tmp_correctioncode_sf != CP::CorrectionCode::Ok)
      return tmp_correctioncode_sf;

    if(prongness == 1)
      highpt_rel = 0.000137 * (pT/1000. - 100.);
    else
      highpt_rel = 0.0000783 * (pT/1000. - 100.);
    double highptunc = highpt_rel / sf;
    val = sqrt(lowptunc * lowptunc + highptunc * highptunc);
    return tmp_correctioncode_sf;
  }
  val = lowptunc;
  return tmp_correctioncode_unc;
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::GetExclIDStatUnc(double& val,
    double eta,
    int prongness,
    double pT)
{
  if(m_tTECT->m_iIDLevel == JETIDBDTTIGHT or m_tTECT->m_iIDLevel == JETIDLLHTIGHT)
    return GetIDStatUnc(val,
                        eta,
                        prongness,
                        pT);

  m_sWorkingPoint = "ID_SF_StatUnc_" + m_sIDLevel + "_excl" + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  return this->GetIDValue(val, m_sWorkingPoint, pT);
}

//______________________________________________________________________________
std::string TauEfficiencyJetIDTool::ConvertIDToString(const int& iLevel)
{
  switch(iLevel)
  {
  case JETIDNONE:
    return "None";
    break;
  case JETIDBDTLOOSE:
    return "JetBDTSigLoose";
    break;
  case JETIDBDTMEDIUM:
    return "JetBDTSigMedium";
    break;
  case JETIDBDTTIGHT:
    return "JetBDTSigTight";
    break;
  case JETIDBDTOTHER:
    return "JetBDTSigOther";
    break;
  case JETIDLLHLOOSE:
    return "tauJLLHLoose";
    break;
  case JETIDLLHMEDIUM:
    return "tauJLLHMedium";
    break;
  case JETIDLLHTIGHT:
    return "tauJLLHTight";
    break;
  case JETIDLLHFAIL:
    return "tauJLLH";
    break;
  case JETIDBDTFAIL:
    return "JetBDTSig";
    break;
  default:
    assert(false && "No valid ID level passed. Breaking up ...");
    break;
  }
  return "";
}

//______________________________________________________________________________
std::string TauEfficiencyJetIDTool::ConvertEtaToString(const float& fEta)
{
  if(m_tTECT->m_bUseInclusiveEta)
    return "_etaAll";
  if(fabs(fEta) <= 1.5)
    return "_barrel";
  else if(fabs(fEta) > 1.5)
    return "_endcap";
  else
    assert(false && "Invalid eta provided. Breaking up ...");
  return "";
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::GetIDValue(double& val,
    const std::string& sWorkingPoint,
    const float& fPt)
{
  ATH_MSG_DEBUG("Try to access histogram: " << sWorkingPoint);
  TH1F* tmp = (*m_mSF)[sWorkingPoint];
  if (!tmp)
  {
    ATH_MSG_FATAL("could not find histogram " << sWorkingPoint << ", breaking up");
    return CP::CorrectionCode::Error;
  }
  int iBin = tmp->FindBin(fPt / 1000.); // histograms are binned in GeV
  //check underflow:
  if (iBin==0)
  {
    ATH_MSG_WARNING("tau pt out of validity range, set scale factor to 0");
    val = 0;
    return CP::CorrectionCode::OutOfValidityRange;
  }
  //check overflow:
  if(iBin == tmp->GetNbinsX() + 1)
    iBin -= 1;
  val = tmp->GetBinContent(iBin);
  ATH_MSG_VERBOSE("Got value " << val << " in bin " << iBin << "(pt=" << fPt <<")");
  return CP::CorrectionCode::Ok;
}


//______________________________________________________________________________
void TauEfficiencyJetIDTool::SwitchOnPtBinnedIDSF(bool bSwitch)
{
  if(bSwitch)
  {
    ATH_MSG_VERBOSE("switching PtBinnedIDSF on");
    m_sIDpt = "_ptbin";
  }
  else
  {
    ATH_MSG_VERBOSE("switching PtBinnedIDSF off");
    m_sIDpt = "_ptinc";
  }
}

#endif // TAUEFFICIENCYTOOL_CXX


//  LocalWords:  athena
