/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauAnalysisTools/TauEfficiencyContJetIDTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"

using namespace TauAnalysisTools;

//==================================PUBLIC-PART=================================
//______________________________________________________________________________
TauEfficiencyContJetIDTool::TauEfficiencyContJetIDTool(std::string sName) :
  CommonEfficiencyTool(sName)
{
  m_sSystematicSet = new CP::SystematicSet();
  m_iSysDirection = 0;
}

//______________________________________________________________________________
TauEfficiencyContJetIDTool::~TauEfficiencyContJetIDTool()
{
}

//______________________________________________________________________________
StatusCode TauEfficiencyContJetIDTool::initialize()
{
  ATH_MSG_INFO("Initializing TauEfficiencyContJetIDTool");

  m_mSystematics =
  {
    {"TAUS_EFF_CONTJETID_STAT", STAT}
    , {"TAUS_EFF_CONTJETID_SYST", SYST}
  };

  return CommonEfficiencyTool::initialize();
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyContJetIDTool::getEfficiencyScaleFactor(const xAOD::TauJet& xTau,
    double& dEfficiencyScaleFactor)
{
  // obtain ID SF value
  if (!m_tTECT->m_bUseInclusiveEta)
    return getTotalSF(xTau, dEfficiencyScaleFactor);
  // else
  return getTotalSFInclusiveEta(xTau, dEfficiencyScaleFactor);
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyContJetIDTool::applyEfficiencyScaleFactor(const xAOD::TauJet& xTau)
{
  double sf = 0.;
  // retreive scale factor
  CP::CorrectionCode tmpCorrectionCode = getEfficiencyScaleFactor(xTau, sf);
  // adding scale factor
  xTau.auxdecor<double>(m_sVarName) = sf;
  ATH_MSG_VERBOSE("Stored value " << sf << " as " << m_tTECT->m_sVarNameBase << " in auxdecor");

  return tmpCorrectionCode;
}

bool TauEfficiencyContJetIDTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const
{
  CP::SystematicSet sys = affectingSystematics();
  return sys.find (systematic) != sys.end ();
}

CP::SystematicSet TauEfficiencyContJetIDTool::affectingSystematics() const
{
  CP::SystematicSet sSystematicSet = recommendedSystematics();
  return sSystematicSet;
}

CP::SystematicSet TauEfficiencyContJetIDTool::recommendedSystematics() const
{
  CP::SystematicSet sSystematicSet;
  sSystematicSet.insert (CP::SystematicVariation ("TAUS_EFF_CONTJETID_STAT", 1));
  sSystematicSet.insert (CP::SystematicVariation ("TAUS_EFF_CONTJETID_STAT", -1));
  sSystematicSet.insert (CP::SystematicVariation ("TAUS_EFF_CONTJETID_SYST", 1));
  sSystematicSet.insert (CP::SystematicVariation ("TAUS_EFF_CONTJETID_SYST", -1));
  return sSystematicSet;
}

CP::SystematicCode TauEfficiencyContJetIDTool::applySystematicVariation ( const CP::SystematicSet& sSystematicSet)
{
  // first check if we already know this systematic configuration
  auto itSystematicSet = m_mSystematicSets.find(sSystematicSet);
  if (itSystematicSet != m_mSystematicSets.end())
  {
    m_sSystematicSet = &itSystematicSet->first;
    return CP::SystematicCode::Ok;
  }
  // store this calibration for future use, and make it current
  m_sSystematicSet = &m_mSystematicSets.insert(std::pair<CP::SystematicSet,std::string>(sSystematicSet, sSystematicSet.name())).first->first;
  return CP::SystematicCode::Ok;
}

//=================================PRIVATE-PART=================================
//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyContJetIDTool::getTotalSF(const xAOD::TauJet& xTau,
    double& dEfficiencyScaleFactor)
{
  if (m_sSystematicSet->size() != 1)  // return nominal scale factor
  {
    if (m_sSystematicSet->size() > 1)
      ATH_MSG_ERROR("It is currently not supported to combine several systematic variations!");
    return getBDTbinnedSF(dEfficiencyScaleFactor, xTau.nTracks(), xTau.eta(), xTau.discriminant(xAOD::TauJetParameters::TauID(15)));
  }
  else
  {
    for (auto syst : *m_sSystematicSet)
    {
      auto it = m_mSystematics.find(syst.basename());
      if (it == m_mSystematics.end())
      {
        ATH_MSG_VERBOSE("unsupported systematic variation: skipping this one");
        continue;
      }

      if (fabs(syst.parameter()) != 1)
      {
        ATH_MSG_WARNING("systematic variation other than 1 sigma is not supported, skipping this one");
        continue;
      }
      m_iSysDirection = syst.parameter();
      if (it->second == STAT)
        return getBDTbinnedSFStatUnc(dEfficiencyScaleFactor, xTau.nTracks(), xTau.eta(), xTau.discriminant(xAOD::TauJetParameters::TauID(15)));
      else if (it->second == SYST)
        return getBDTbinnedSFSysUnc(dEfficiencyScaleFactor, xTau.nTracks(), xTau.eta(), xTau.discriminant(xAOD::TauJetParameters::TauID(15)));
      else
      {
        ATH_MSG_WARNING("unsupported systematic variation " << syst.name() << ": skipping this one");
        continue;
      }
    }
  }
  return CP::CorrectionCode::OutOfValidityRange;
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyContJetIDTool::getTotalSFInclusiveEta(const xAOD::TauJet& xTau,
    double& dEfficiencyScaleFactor)
{
  if (m_sSystematicSet->size() != 1)  // return nominal scale factor
  {
    if (m_sSystematicSet->size() > 1)
      ATH_MSG_ERROR("It is currently not supported to combine several systematic variations!");
    return getBDTbinnedSF(dEfficiencyScaleFactor, xTau.nTracks(), xTau.discriminant(xAOD::TauJetParameters::TauID(15)));
  }
  else
  {
    for (auto syst : *m_sSystematicSet)
    {
      auto it = m_mSystematics.find(syst.basename());
      if (it == m_mSystematics.end())
      {
        ATH_MSG_VERBOSE("unsupported systematic variation: skipping this one");
        continue;
      }

      if (fabs(syst.parameter()) != 1)
      {
        ATH_MSG_WARNING("systematic variation other than 1 sigma is not supported, skipping this one");
        continue;
      }
      m_iSysDirection = syst.parameter();
      if (it->second == STAT)
        return getBDTbinnedSFStatUnc(dEfficiencyScaleFactor, xTau.nTracks(), xTau.discriminant(xAOD::TauJetParameters::TauID(15)));
      else if (it->second == SYST)
        return getBDTbinnedSFSysUnc(dEfficiencyScaleFactor, xTau.nTracks(), xTau.discriminant(xAOD::TauJetParameters::TauID(15)));
      else
      {
        ATH_MSG_WARNING("unsupported systematic variation " << syst.name() << ": skipping this one");
        continue;
      }
    }
  }
  return CP::CorrectionCode::OutOfValidityRange;
}

// Central Value
//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyContJetIDTool::getBDTbinnedSF(double& val,
    int prongness,
    const double& BDT)
{
  std::string workingPoint = "ID_SF" + ConvertProngToString(prongness);
  return this->GetIDValue(val, workingPoint, BDT);
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyContJetIDTool::getBDTbinnedSF(double& val,
    int prongness,
    const double& eta,
    const double& BDT)
{
  std::string workingPoint = "ID_SF" + ConvertProngToString(prongness) + ConvertEtaToString(eta);
  return this->GetIDValue(val, workingPoint, BDT);
}

// Systematic Uncertainty
//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyContJetIDTool::getBDTbinnedSFSysUnc(double& val,
    int prongness,
    const double& BDT)
{
  std::string workingPoint = "ID_SF_sys" + ConvertSystematicToString(m_tTECT->m_iContSysType) + ConvertDirectionToString(m_iSysDirection) + ConvertProngToString(prongness);
  return this->GetIDValue(val, workingPoint, BDT);
}


//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyContJetIDTool::getBDTbinnedSFSysUnc(double& val,
    int prongness,
    const double& eta,
    const double& BDT)
{
  std::string workingPoint = "ID_SF_sys" + ConvertSystematicToString(m_tTECT->m_iContSysType) + ConvertDirectionToString(m_iSysDirection) + ConvertProngToString(prongness) + ConvertEtaToString(eta);
  return this->GetIDValue(val, workingPoint, BDT);
}

// Statistical Uncertainty
//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyContJetIDTool::getBDTbinnedSFStatUnc(double& val,
    int prongness,
    const double& BDT)
{
  std::string workingPoint = "ID_SF_stat" + ConvertDirectionToString(m_iSysDirection) + ConvertProngToString(prongness);
  return this->GetIDValue(val, workingPoint, BDT);
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyContJetIDTool::getBDTbinnedSFStatUnc(double& val,
    int prongness,
    const double& eta,
    const double& BDT)
{
  std::string workingPoint = "ID_SF_stat" + ConvertDirectionToString(m_iSysDirection) + ConvertProngToString(prongness) + ConvertEtaToString(eta);
  return this->GetIDValue(val, workingPoint, BDT);
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyContJetIDTool::GetIDValue(double& val,
    const std::string& sWorkingPoint,
    const float& fBDT)
{
  ATH_MSG_VERBOSE("Try to access histogram: " << sWorkingPoint);
  if (fBDT < 0.4 || fBDT > 1.0)
  {
    ATH_MSG_WARNING("BDT Value out of bounds: "<<fBDT<<" (SFs only valid between 0.4 and 1.0). Returning a SF of 1.0");
    val = 1.0;
    return CP::CorrectionCode::OutOfValidityRange;
  }
  TH1F* tmp = dynamic_cast<TH1F*>(std::get<0>((*m_mSF)[sWorkingPoint]));
  if (!tmp)
  {
    ATH_MSG_FATAL("could not find histogram " << sWorkingPoint << ", breaking up");
    return CP::CorrectionCode::Error;
  }
  int iBin = tmp->FindBin(fBDT);
  //check overflow:
  if(iBin == tmp->GetNbinsX() + 1)
    iBin -= 1;
  val = tmp->GetBinContent(iBin);
  ATH_MSG_VERBOSE("Got value " << val << " in bin " << iBin << "(pt=" << fBDT <<")");
  return CP::CorrectionCode::Ok;
}

// Return Histogram Names
//______________________________________________________________________________
std::string TauEfficiencyContJetIDTool::ConvertDirectionToString(const int iSysDirection)
{
  if(iSysDirection == 1)
    return "_UP";
  else if (iSysDirection == -1)
    return "_DOWN";
  else
  {
    ATH_MSG_FATAL("Statistical/Systematic direction must be UP or DOWN. Breaking up ...");
    assert(false);
  }
  return "";
}

//______________________________________________________________________________
std::string TauEfficiencyContJetIDTool::ConvertEtaToString(const float& fEta)
{
  if(m_tTECT->m_bUseInclusiveEta)
    return "";
  if(fabs(fEta) <= 1.5)
    return "_barrel";
  else if(fabs(fEta) > 1.5)
    return "_endcap";
  else
  {
    ATH_MSG_FATAL("Invalid eta provided. Breaking up ...");
    assert(false);
  }
  return "";
}


//______________________________________________________________________________
std::string TauEfficiencyContJetIDTool::ConvertSystematicToString(int iContSysType)
{
  switch(iContSysType)
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
    ATH_MSG_FATAL("No valid systematic uncertainty component passed. Breaking up");
    assert(false);
    break;
  }
  return "";
}
