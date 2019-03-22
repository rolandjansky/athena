/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/CalibrationMethods/ResidualOffsetCorrection.h"
#include "PathResolver/PathResolver.h"

ResidualOffsetCorrection::ResidualOffsetCorrection()
  : JetCalibrationToolBase( "ResidualOffsetCorrection::ResidualOffsetCorrection" ),
    m_config(NULL), m_jetAlgo(""), m_calibAreaTag(""), m_dev(false), m_isData(false),
    m_npvBeamspotCorr(NULL), m_resOffsetBins(NULL)
{ }

ResidualOffsetCorrection::ResidualOffsetCorrection(const std::string& name)
  : JetCalibrationToolBase( name ),
    m_config(NULL), m_jetAlgo(""), m_calibAreaTag(""), m_dev(false), m_isData(false),
    m_npvBeamspotCorr(NULL), m_resOffsetBins(NULL)
{ }

ResidualOffsetCorrection::ResidualOffsetCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool isData, bool dev)
  : JetCalibrationToolBase( name ),
    m_config(config), m_jetAlgo(jetAlgo), m_calibAreaTag(calibAreaTag), m_dev(dev), m_isData(isData),
    m_npvBeamspotCorr(NULL), m_resOffsetBins(NULL)
{ }

ResidualOffsetCorrection::~ResidualOffsetCorrection() {

  if(m_npvBeamspotCorr) delete m_npvBeamspotCorr;
  if(m_resOffsetBins) delete m_resOffsetBins;

}

//bool ResidualOffsetCorrection::initializeTool(TEnv * config, TString jetAlgo, bool isData) {
StatusCode ResidualOffsetCorrection::initializeTool(const std::string&) {

  m_GeV=1000; //This will have to be fixed eventually

  //Read mu scale factor from global config
  m_muSF = m_config->GetValue("MuScaleFactor",1.0);

  // Read useNjet from global config
  m_useNjet = m_config->GetValue("OffsetCorrection.UseNjet",false);

  bool doMuOnly   = m_config->GetValue("ApplyOnlyMuResidual",false);
  bool doNPVOnly  = m_config->GetValue("ApplyOnlyNPVResidual",false);
  bool doNJetOnly = m_config->GetValue("ApplyOnlyNJetResidual",false);

  //Read reference mu and NPV (and nJet) from global config
  m_mu_ref = m_config->GetValue("OffsetCorrection.DefaultMuRef",-99.0);
  if (m_mu_ref==-99 && !doNPVOnly && !doNJetOnly) { ATH_MSG_FATAL("OffsetCorrection.DefaultMuRef not specified."); return StatusCode::FAILURE; }
  m_NPV_ref = m_config->GetValue("OffsetCorrection.DefaultNPVRef",-99.0);
  if (m_NPV_ref==-99 && !doMuOnly && !m_useNjet) { ATH_MSG_FATAL("OffsetCorrection.DefaultNPVRef not specified."); return StatusCode::FAILURE; }
  m_nJet_ref = m_config->GetValue("OffsetCorrection.DefaultNjetRef",-99.0);
  if (m_nJet_ref==-99 && m_useNjet) { ATH_MSG_FATAL("OffsetCorrection.DefaultNjetRef not specified."); return StatusCode::FAILURE; }

  //Add the residual offset correction factors to the config TEnv
  TString ResidualOffsetCalibFile = m_config->GetValue("ResidualOffset.CalibFile","");
  if(m_dev){
    ResidualOffsetCalibFile.Remove(0,33);
    ResidualOffsetCalibFile.Insert(0,"JetCalibTools/");
  }
  else{ResidualOffsetCalibFile.Insert(14,m_calibAreaTag);}
  TString calibFile = PathResolverFindCalibFile(ResidualOffsetCalibFile.Data());
  m_config->ReadFile(calibFile, kEnvLocal);
  //Retrieve information specific to the residual offset correction from the TEnv
  TString offsetName = m_config->GetValue("ResidualOffsetCorrection.Name","");
  m_resOffsetDesc = m_config->GetValue(offsetName+".Description","");
  ATH_MSG_INFO("Reading residual jet-area pile-up correction factors from: " << calibFile);
  ATH_MSG_INFO("Description: " << m_resOffsetDesc);

  //Check if the config is set to apply the beamspot correction to NPV
  m_applyNPVBeamspotCorrection = m_config->GetValue("ApplyNPVBeamspotCorrection",false);

  std::vector<double> offsetEtaBins = JetCalibUtils::VectorizeD( m_config->GetValue(offsetName+".AbsEtaBins","") );
  if (offsetEtaBins.size()<3) { ATH_MSG_FATAL(offsetName << ".AbsEtaBins not specified"); return StatusCode::FAILURE; }
  m_resOffsetBins = new TAxis(offsetEtaBins.size()-1,&offsetEtaBins[0]);

  if(!doNPVOnly && !doNJetOnly){
    m_resOffsetMu = JetCalibUtils::VectorizeD(m_config->GetValue(offsetName+".MuTerm."+m_jetAlgo,""));
    if ( (int)m_resOffsetMu.size()!=m_resOffsetBins->GetNbins()+1 ) {
      ATH_MSG_FATAL( "Incorrect specification of " << offsetName << ".MuTerm." << m_jetAlgo );
      return StatusCode::FAILURE;
    }
  }
  if(m_useNjet) {
    m_resOffsetNjet = JetCalibUtils::VectorizeD(m_config->GetValue(offsetName+".nJetTerm."+m_jetAlgo,""));
    if ((int)m_resOffsetNjet.size()!=m_resOffsetBins->GetNbins()+1) {
      ATH_MSG_FATAL( "Incorrect specification of " << offsetName << ".nJetTerm." << m_jetAlgo );
      return StatusCode::FAILURE;
    }
  }
  else if(!doMuOnly){
    m_resOffsetNPV = JetCalibUtils::VectorizeD(m_config->GetValue(offsetName+".NPVTerm."+m_jetAlgo,""));
    if ((int)m_resOffsetNPV.size()!=m_resOffsetBins->GetNbins()+1) {
      ATH_MSG_FATAL( "Incorrect specification of " << offsetName << ".NPVTerm." << m_jetAlgo );
      return StatusCode::FAILURE;
    }
    if( m_applyNPVBeamspotCorrection) {
      m_npvBeamspotCorr = new NPVBeamspotCorrection();
      ATH_MSG_INFO("\n  NPV beamspot correction will be applied.");
    }
  }

  return StatusCode::SUCCESS;
}

double ResidualOffsetCorrection::GetResidualOffset ( double abseta, double mu, double NPV, int nJet, bool MuOnly, bool NOnly) const {
  return GetResidualOffsetET(abseta, mu, NPV, nJet, MuOnly, NOnly, m_resOffsetMu, m_resOffsetNPV, m_resOffsetNjet, m_resOffsetBins);
}

double ResidualOffsetCorrection::GetResidualOffsetET( double abseta, double mu, double NPV, int nJet, bool MuOnly, bool NOnly, std::vector<double> OffsetMu, std::vector<double> OffsetNPV, std::vector<double> OffsetNjet, TAxis *OffsetBins ) const {

  //mu rescaling
  const double muCorr = m_isData ? mu : mu*m_muSF;

  if(m_useNjet) {
    // further correction to nJet if desired
    int nJetCorr = nJet;

    double alpha   = NOnly ? 0 : OffsetMu[0];
    double beta    = MuOnly ? 0 : OffsetNjet[0];
    double etaEdge = 0;
    int bin=1;
    for (;bin<=OffsetBins->GetNbins();++bin) {
      etaEdge = OffsetBins->GetBinLowEdge(bin);
      const double width=OffsetBins->GetBinWidth(bin);
      if (abseta<etaEdge+width) break;
      if(!NOnly)  alpha += width*OffsetMu[bin];
      if(!MuOnly) beta  += width*OffsetNjet[bin];
    }
    if(!NOnly)  alpha += OffsetMu[bin]*(abseta-etaEdge);
    if(!MuOnly) beta  += OffsetNjet[bin]*(abseta-etaEdge);
    return (alpha*(muCorr-m_mu_ref) + beta*(nJetCorr-m_nJet_ref))*m_GeV;
  } else {
    //NPV beamspot correction
    const double NPVCorr = GetNPVBeamspotCorrection(NPV);

    double alpha   = NOnly ? 0 : OffsetMu[0];
    double beta    = MuOnly ? 0 : OffsetNPV[0];
    double etaEdge = 0;
    int bin=1;
    for (;bin<=OffsetBins->GetNbins();++bin) {
      etaEdge = OffsetBins->GetBinLowEdge(bin);
      const double width=OffsetBins->GetBinWidth(bin);
      if (abseta<etaEdge+width) break;
      if(!NOnly)  alpha += width*OffsetMu[bin];
      if(!MuOnly) beta  += width*OffsetNPV[bin];
    }
    if(!NOnly)  alpha += OffsetMu[bin]*(abseta-etaEdge);
    if(!MuOnly) beta  += OffsetNPV[bin]*(abseta-etaEdge);
    return (alpha*(muCorr-m_mu_ref) + beta*(NPVCorr-m_NPV_ref))*m_GeV;
  }
}

double ResidualOffsetCorrection::GetNPVBeamspotCorrection(double NPV) const {
  if(!m_isData && m_applyNPVBeamspotCorrection) return m_npvBeamspotCorr->GetNVertexBsCorrection(NPV);
  return NPV;
}

