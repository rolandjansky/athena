/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/SharedFilesVersion.h"
#include "TauAnalysisTools/SelectionCuts.h"

// Framework include(s):
#include "PathResolver/PathResolver.h"

// ROOT include(s)
#include "TEnv.h"
#include "THashList.h"

// EDM include(s)
#include "xAODMetaData/FileMetaData.h"


using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauSelectionTool::TauSelectionTool( const std::string& name )
  : asg::AsgMetadataTool( name )
  , m_sJetIDWP("JETIDNONE")
  , m_sEleBDTWP("ELEIDNONE")
  , m_fOutFile(0)
  , m_sElectronContainerName("Electrons")
  , m_sMuonContainerName("Muons")
  , m_aAccept( "TauSelection" )
{
  declareProperty( "CreateControlPlots", m_bCreateControlPlots = false);
  /*
    Baseline properties declaration:
    properties containing 'Region' are a vector of lower and upper bounds
    other properties named in plural are a list of exact values to cut on
    other properties are single cuts
  */
  declareProperty( "SelectionCuts", m_iSelectionCuts = NoCut); // initialize with 'no' cuts
  declareProperty( "PtRegion",      m_vPtRegion      = {}); // in GeV
  declareProperty( "PtMin",         m_dPtMin         = NAN); // in GeV
  declareProperty( "PtMax",         m_dPtMax         = NAN); // in GeV
  declareProperty( "AbsEtaRegion",  m_vAbsEtaRegion  = {});
  declareProperty( "AbsEtaMin",     m_dAbsEtaMin     = NAN);
  declareProperty( "AbsEtaMax",     m_dAbsEtaMax     = NAN);
  declareProperty( "AbsCharges",    m_vAbsCharges    = {});
  declareProperty( "AbsCharge",     m_iAbsCharge     = NAN);
  declareProperty( "NTracks",       m_vNTracks       = {});
  declareProperty( "NTrack",        m_iNTrack        = NAN);
  declareProperty( "JetBDTRegion",  m_vJetBDTRegion  = {});
  declareProperty( "JetBDTMin",     m_dJetBDTMin     = NAN);
  declareProperty( "JetBDTMax",     m_dJetBDTMax     = NAN);
  declareProperty( "JetBDTSigTransRegion", m_vJetBDTSigTransRegion  = {});
  declareProperty( "JetBDTSigTransMin", m_dJetBDTSigTransMin = NAN);
  declareProperty( "JetBDTSigTransMax", m_dJetBDTSigTransMax = NAN);
  declareProperty( "JetRNNSigTransRegion", m_vJetRNNSigTransRegion  = {});
  declareProperty( "JetRNNSigTransMin", m_dJetRNNSigTransMin = NAN);
  declareProperty( "JetRNNSigTransMax", m_dJetRNNSigTransMax = NAN);
  declareProperty( "JetIDWP",       m_iJetIDWP       = 0);
  declareProperty( "EleBDTRegion",  m_vEleBDTRegion  = {});
  declareProperty( "EleBDTMin",     m_dEleBDTMin     = NAN);
  declareProperty( "EleBDTMax",     m_dEleBDTMax     = NAN);
  declareProperty( "EleBDTWP",      m_iEleBDTWP      = 0);
  declareProperty( "EleOLR",        m_bEleOLR        = false);
  declareProperty( "MuonVeto",      m_bMuonVeto      = false);
  declareProperty( "MuonOLR",       m_bMuonOLR       = false);

  m_sConfigPath = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/Selection/recommended_selection_mc15.conf";
  m_sEleOLRFilePath = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/Selection/eveto_cutvals.root";

  declareProperty( "ConfigPath",            m_sConfigPath);
  declareProperty( "EleOLRFilePath",        m_sEleOLRFilePath);
  declareProperty( "ElectronContainerName", m_sElectronContainerName);
  declareProperty( "MuonContainerName",     m_sMuonContainerName);

  declareProperty( "IgnoreAODFixCheck", m_bIgnoreAODFixCheck = false);
  declareProperty( "RecalcEleOLR",      m_bRecalcEleOLR = false);
}

//______________________________________________________________________________
TauSelectionTool::~TauSelectionTool()
{
  for (auto entry : m_cMap)
    delete entry.second;
  m_cMap.clear();
}

//______________________________________________________________________________
StatusCode TauSelectionTool::initialize()
{
  bool bConfigViaConfigFile = !m_sConfigPath.empty();
  bool bConfigViaProperties = false;
  if (!bConfigViaProperties and !m_vPtRegion.empty())         bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dPtMin == m_dPtMin)         bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dPtMax == m_dPtMax)         bConfigViaProperties = true;
  if (!bConfigViaProperties and !m_vAbsEtaRegion.empty())     bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dAbsEtaMin == m_dAbsEtaMin) bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dAbsEtaMax == m_dAbsEtaMax) bConfigViaProperties = true;
  if (!bConfigViaProperties and !m_vAbsCharges.empty())       bConfigViaProperties = true;
  if (!bConfigViaProperties and m_iAbsCharge == m_iAbsCharge) bConfigViaProperties = true;
  if (!bConfigViaProperties and !m_vNTracks.empty())          bConfigViaProperties = true;
  if (!bConfigViaProperties and m_iNTrack == m_iNTrack)       bConfigViaProperties = true;
  if (!bConfigViaProperties and !m_vJetBDTRegion.empty())     bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dJetBDTMin == m_dJetBDTMin) bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dJetBDTMax == m_dJetBDTMax) bConfigViaProperties = true;
  if (!bConfigViaProperties and !m_vJetBDTSigTransRegion.empty())         bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dJetBDTSigTransMin == m_dJetBDTSigTransMin) bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dJetBDTSigTransMax == m_dJetBDTSigTransMax) bConfigViaProperties = true;
  if (!bConfigViaProperties and !m_vJetRNNSigTransRegion.empty())         bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dJetRNNSigTransMin == m_dJetRNNSigTransMin) bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dJetRNNSigTransMax == m_dJetRNNSigTransMax) bConfigViaProperties = true;
  if (!bConfigViaProperties and m_iJetIDWP != 0)              bConfigViaProperties = true;
  if (!bConfigViaProperties and !m_vEleBDTRegion.empty())     bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dEleBDTMin == m_dEleBDTMin) bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dEleBDTMax == m_dEleBDTMax) bConfigViaProperties = true;
  if (!bConfigViaProperties and m_iEleBDTWP != 0)             bConfigViaProperties = true;
  if (!bConfigViaProperties and m_bEleOLR == true)            bConfigViaProperties = true;
  if (!bConfigViaProperties and m_bMuonVeto == true)          bConfigViaProperties = true;
  if (!bConfigViaProperties and m_bMuonOLR == true)           bConfigViaProperties = true;

  if (bConfigViaConfigFile and bConfigViaProperties)
  {
    ATH_MSG_WARNING("Configured tool via setProperty and configuration file, which may lead to unexpected configuration.");
    ATH_MSG_WARNING("In doubt check the configuration that is printed when the tool is initialized and the message level is set to debug");
    ATH_MSG_WARNING("For further details please refer to the documentation:");
    ATH_MSG_WARNING("https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauSelectionTool.rst");
  }
  if (!bConfigViaConfigFile and !bConfigViaProperties)
  {
    ATH_MSG_WARNING("No cut configuration provided, the tool will not do anything. For further details please refer to the documentation:");
    ATH_MSG_WARNING("https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauSelectionTool.rst");
  }

  if (bConfigViaConfigFile)
  {
    TEnv rEnv;
    std::string sInputFilePath = PathResolverFindCalibFile(m_sConfigPath);

    if (!testFileForEOFContainsCharacters(sInputFilePath))
      ATH_MSG_WARNING("Config file for TauSelectionTool with path "<<sInputFilePath<<" does not contain an empty last line. The tool might be not properly configured!");

    rEnv.ReadFile(sInputFilePath.c_str(),
                  kEnvAll);

    std::vector<std::string> vCuts;
    // if Cuts are specified in the config file take these ones, if not take all
    // specified in the config
    if (rEnv.Defined("SelectionCuts"))
      TauAnalysisTools::split(rEnv, "SelectionCuts", ' ', vCuts);
    else
    {
      auto lList = rEnv.GetTable();
      for( Int_t i = 0; i < lList->GetEntries(); ++i )
      {
        vCuts.push_back( lList->At( i )->GetName() );
      }
    }

    int iSelectionCuts = 0;

    for (auto sCut : vCuts)
    {
      if (sCut == "PtRegion")
      {
        iSelectionCuts = iSelectionCuts | CutPt;
        if (m_vPtRegion.size() == 0)
          TauAnalysisTools::split(rEnv,"PtRegion", ';', m_vPtRegion);
      }
      else if (sCut == "PtMin")
      {
        iSelectionCuts = iSelectionCuts | CutPt;
        if (m_dPtMin != m_dPtMin)
          m_dPtMin = rEnv.GetValue("PtMin",NAN);
      }
      else if (sCut == "PtMax")
      {
        iSelectionCuts = iSelectionCuts | CutPt;
        if (m_dPtMax != m_dPtMax)
          m_dPtMax = rEnv.GetValue("PtMax",NAN);
      }
      else if (sCut == "AbsEtaRegion")
      {
        iSelectionCuts = iSelectionCuts | CutAbsEta;
        if (m_vAbsEtaRegion.size() == 0)
          TauAnalysisTools::split(rEnv,"AbsEtaRegion", ';', m_vAbsEtaRegion);
      }
      else if (sCut == "AbsEtaMin")
      {
        iSelectionCuts = iSelectionCuts | CutAbsEta;
        if (m_dAbsEtaMin != m_dAbsEtaMin)
          m_dAbsEtaMin = rEnv.GetValue("AbsEtaMin",NAN);
      }
      else if (sCut == "AbsEtaMax")
      {
        iSelectionCuts = iSelectionCuts | CutAbsEta;
        if (m_dAbsEtaMax != m_dAbsEtaMax)
          m_dAbsEtaMax = rEnv.GetValue("AbsEtaMax",NAN);
      }
      else if (sCut == "AbsCharges")
      {
        iSelectionCuts = iSelectionCuts | CutAbsCharge;
        if (m_vAbsCharges.size() == 0)
          TauAnalysisTools::split(rEnv,"AbsCharges", ';', m_vAbsCharges);
      }
      else if (sCut == "AbsCharge")
      {
        iSelectionCuts = iSelectionCuts | CutAbsCharge;
        if (m_iAbsCharge != m_iAbsCharge)
          m_iAbsCharge = rEnv.GetValue("AbsCharge",NAN);
      }
      else if (sCut == "NTracks")
      {
        iSelectionCuts = iSelectionCuts | CutNTrack;
        if (m_vNTracks.size() == 0)
          TauAnalysisTools::split(rEnv,"NTracks", ';', m_vNTracks);
      }
      else if (sCut == "NTrack")
      {
        iSelectionCuts = iSelectionCuts | CutNTrack;
        if (m_iNTrack != m_iNTrack)
          m_iNTrack = rEnv.GetValue("NTrack",NAN);
      }
      else if (sCut == "JetBDTRegion")
      {
        iSelectionCuts = iSelectionCuts | CutJetBDTScore;
        if (m_vJetBDTRegion.size() == 0)
          TauAnalysisTools::split(rEnv,"JetBDTRegion", ';', m_vJetBDTRegion);
      }
      else if (sCut == "JetBDTMin")
      {
        iSelectionCuts = iSelectionCuts | CutJetBDTScore;
        if (m_dJetBDTMin != m_dJetBDTMin)
          m_dJetBDTMin = rEnv.GetValue("JetBDTMin",NAN);
      }
      else if (sCut == "JetBDTMax")
      {
        iSelectionCuts = iSelectionCuts | CutJetBDTScore;
        if (m_dJetBDTMax != m_dJetBDTMax)
          m_dJetBDTMax = rEnv.GetValue("JetBDTMax",NAN);
      }
      else if (sCut == "JetBDTSigTransRegion")
      {
        iSelectionCuts = iSelectionCuts | CutJetBDTScoreSigTrans;
        if (m_vJetBDTSigTransRegion.size() == 0)
          TauAnalysisTools::split(rEnv,"JetBDTSigTransRegion", ';', m_vJetBDTSigTransRegion);
      }
      else if (sCut == "JetBDTSigTransMin")
      {
        iSelectionCuts = iSelectionCuts | CutJetBDTScoreSigTrans;
        if (m_dJetBDTSigTransMin != m_dJetBDTSigTransMin)
          m_dJetBDTSigTransMin = rEnv.GetValue("JetBDTSigTransMin",NAN);
      }
      else if (sCut == "JetBDTSigTransMax")
      {
        iSelectionCuts = iSelectionCuts | CutJetBDTScoreSigTrans;
        if (m_dJetBDTSigTransMax != m_dJetBDTSigTransMax)
          m_dJetBDTSigTransMax = rEnv.GetValue("JetBDTSigTransMax",NAN);
      }
      else if (sCut == "JetRNNSigTransRegion")
      {
        iSelectionCuts = iSelectionCuts | CutJetRNNScoreSigTrans;
        if (m_vJetRNNSigTransRegion.size() == 0)
          TauAnalysisTools::split(rEnv,"JetRNNSigTransRegion", ';', m_vJetRNNSigTransRegion);
      }
      else if (sCut == "JetRNNSigTransMin")
      {
        iSelectionCuts = iSelectionCuts | CutJetRNNScoreSigTrans;
        if (m_dJetRNNSigTransMin != m_dJetRNNSigTransMin)
          m_dJetRNNSigTransMin = rEnv.GetValue("JetRNNSigTransMin",NAN);
      }
      else if (sCut == "JetRNNSigTransMax")
      {
        iSelectionCuts = iSelectionCuts | CutJetRNNScoreSigTrans;
        if (m_dJetRNNSigTransMax != m_dJetRNNSigTransMax)
          m_dJetRNNSigTransMax = rEnv.GetValue("JetRNNSigTransMax",NAN);
      }
      else if (sCut == "EleBDTRegion")
      {
        iSelectionCuts = iSelectionCuts | CutEleBDTScore;
        if (m_vEleBDTRegion.size() == 0)
          TauAnalysisTools::split(rEnv,"EleBDTRegion", ';', m_vEleBDTRegion);
      }
      else if (sCut == "EleBDTMin")
      {
        iSelectionCuts = iSelectionCuts | CutEleBDTScore;
        if (m_dEleBDTMin != m_dEleBDTMin)
          m_dEleBDTMin = rEnv.GetValue("EleBDTMin",NAN);
      }
      else if (sCut == "EleBDTMax")
      {
        iSelectionCuts = iSelectionCuts | CutEleBDTScore;
        if (m_dEleBDTMax != m_dEleBDTMax)
          m_dEleBDTMax = rEnv.GetValue("EleBDTMax",NAN);
      }
      else if (sCut == "JetIDWP")
      {
        iSelectionCuts = iSelectionCuts | CutJetIDWP;
        if (m_iJetIDWP == JETIDNONEUNCONFIGURED)
          m_iJetIDWP = convertStrToJetIDWP(rEnv.GetValue("JetIDWP","JETIDNONE"));
      }
      else if (sCut == "EleBDTWP")
      {
        iSelectionCuts = iSelectionCuts | CutEleBDTWP;
        if (m_iEleBDTWP == ELEIDNONEUNCONFIGURED)
          m_iEleBDTWP = convertStrToEleBDTWP(rEnv.GetValue("EleBDTWP","ELEIDNONE"));
      }
      else if (sCut == "EleOLR")
      {
        iSelectionCuts = iSelectionCuts | CutEleOLR;
        if (m_bEleOLR == false)
          m_bEleOLR = rEnv.GetValue("EleOLR",false);
      }
      else if (sCut == "MuonVeto")
      {
        iSelectionCuts = iSelectionCuts | CutMuonVeto;
        if (m_bMuonVeto == false)
          m_bMuonVeto = rEnv.GetValue("MuonVeto",false);
      }
      else if (sCut == "MuonOLR")
      {
        iSelectionCuts = iSelectionCuts | CutMuonOLR;
        if (m_bMuonOLR == false)
          m_bMuonOLR = rEnv.GetValue("MuonOLR",false);
      }
      else ATH_MSG_WARNING("Cut " << sCut << " is not available");
    }

    if (m_iSelectionCuts == NoCut)
      m_iSelectionCuts = iSelectionCuts;
  }

  // specify all available cut descriptions
  m_cMap =
  {
    {CutPt, new TauAnalysisTools::SelectionCutPt(this)},
    {CutAbsEta, new TauAnalysisTools::SelectionCutAbsEta(this)},
    {CutAbsCharge, new TauAnalysisTools::SelectionCutAbsCharge(this)},
    {CutNTrack, new TauAnalysisTools::SelectionCutNTracks(this)},
    {CutJetBDTScore, new TauAnalysisTools::SelectionCutBDTJetScore(this)},
    {CutJetBDTScoreSigTrans, new TauAnalysisTools::SelectionCutBDTJetScoreSigTrans(this)},
    {CutJetRNNScoreSigTrans, new TauAnalysisTools::SelectionCutRNNJetScoreSigTrans(this)},
    {CutJetIDWP, new TauAnalysisTools::SelectionCutJetIDWP(this)},
    {CutEleBDTScore, new TauAnalysisTools::SelectionCutBDTEleScore(this)},
    {CutEleBDTWP, new TauAnalysisTools::SelectionCutEleBDTWP(this)},
    {CutEleOLR, new TauAnalysisTools::SelectionCutEleOLR(this)},
    {CutMuonVeto, new TauAnalysisTools::SelectionCutMuonVeto(this)},
    {CutMuonOLR, new TauAnalysisTools::SelectionCutMuonOLR(this)}
  };

  ATH_MSG_INFO( "Initializing TauSelectionTool" );
  FillRegionVector(m_vPtRegion, m_dPtMin, m_dPtMax);
  FillRegionVector(m_vAbsEtaRegion, m_dAbsEtaMin, m_dAbsEtaMax);
  FillRegionVector(m_vJetBDTRegion, m_dJetBDTMin, m_dJetBDTMax );
  FillRegionVector(m_vJetBDTSigTransRegion, m_dJetBDTSigTransMin, m_dJetBDTSigTransMax );
  FillRegionVector(m_vJetRNNSigTransRegion, m_dJetRNNSigTransMin, m_dJetRNNSigTransMax );
  FillRegionVector(m_vEleBDTRegion, m_dEleBDTMin, m_dEleBDTMax );
  FillValueVector(m_vAbsCharges, m_iAbsCharge );
  FillValueVector(m_vNTracks, m_iNTrack );

  m_sJetIDWP = convertJetIDWPToStr(m_iJetIDWP);
  m_sEleBDTWP = convertEleBDTWPToStr(m_iEleBDTWP);

  PrintConfigRegion ("Pt",          m_vPtRegion);
  PrintConfigRegion ("AbsEta",      m_vAbsEtaRegion);
  PrintConfigValue  ("AbsCharge",   m_vAbsCharges);
  PrintConfigValue  ("NTrack",      m_vNTracks);
  PrintConfigRegion ("BDTJetScore", m_vJetBDTRegion);
  PrintConfigRegion ("BDTJetScoreSigTrans", m_vJetBDTSigTransRegion);
  PrintConfigRegion ("RNNJetScoreSigTrans", m_vJetRNNSigTransRegion);
  PrintConfigRegion ("BDTEleScore", m_vEleBDTRegion);
  PrintConfigValue  ("JetIDWP",     m_sJetIDWP);
  PrintConfigValue  ("JetIDWP ENUM",m_iJetIDWP);
  PrintConfigValue  ("EleBDTDWP",   m_sEleBDTWP);
  PrintConfigValue  ("EleBDTDWP ENUM",m_iEleBDTWP);
  PrintConfigValue  ("EleOLR",      m_bEleOLR);
  PrintConfigValue  ("MuonVeto",    m_bMuonVeto);
  PrintConfigValue  ("MuonOLR",    m_bMuonOLR);

  std::string sCuts = "";
  if (m_iSelectionCuts & CutPt) sCuts+= "Pt ";
  if (m_iSelectionCuts & CutAbsEta) sCuts+= "AbsEta ";
  if (m_iSelectionCuts & CutAbsCharge) sCuts+= "AbsCharge ";
  if (m_iSelectionCuts & CutNTrack) sCuts+= "NTrack ";
  if (m_iSelectionCuts & CutJetBDTScore) {
    sCuts+= "JetBDTScore "; 
    ATH_MSG_WARNING("Cutting on raw JetBDT score is deprecated. Please use properties JetBDTSigTransRegion, JetBDTSigTransMin or JetBDTSigTransMax instead");
    ATH_MSG_WARNING("For further details please refer to the documentation:");
    ATH_MSG_WARNING("https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauSelectionTool.rst");
  }
  if (m_iSelectionCuts & CutJetBDTScoreSigTrans) sCuts+= "JetBDTScoreSigTrans ";
  if (m_iSelectionCuts & CutJetRNNScoreSigTrans) sCuts+= "JetRNNScoreSigTrans ";
  if (m_iSelectionCuts & CutJetIDWP) sCuts+= "JetIDWP ";
  if (m_iSelectionCuts & CutEleBDTScore) sCuts+= "EleBDTScore ";
  if (m_iSelectionCuts & CutEleBDTWP) sCuts+= "EleBDTWP ";
  if (m_iSelectionCuts & CutEleOLR) sCuts+= "EleOLR ";
  if (m_iSelectionCuts & CutMuonVeto) sCuts+= "MuonVeto ";
  if (m_iSelectionCuts & CutMuonOLR) sCuts+= "MuonOLR ";

  ATH_MSG_DEBUG( "cuts: " << sCuts);

  if (m_bCreateControlPlots)
    setupCutFlowHistogram();

  return StatusCode::SUCCESS;
}


//______________________________________________________________________________
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
StatusCode TauSelectionTool::initializeEvent()
{
  return beginEvent();
}
#pragma GCC diagnostic pop

//______________________________________________________________________________
StatusCode TauSelectionTool::beginEvent()
{
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
const Root::TAccept& TauSelectionTool::getTAccept() const
{
  return m_aAccept;
}

//______________________________________________________________________________
const Root::TAccept&
TauSelectionTool::accept( const xAOD::IParticle* xP ) const
{
  // Reset the result:
  m_aAccept.clear();

  // Check if this is a tau:
  if( xP->type() != xAOD::Type::Tau )
  {
    ATH_MSG_ERROR( "accept(...) Function received a non-tau" );
    return m_aAccept;
  }

  // Cast it to a tau:
  const xAOD::TauJet* xTau = dynamic_cast< const xAOD::TauJet* >( xP );
  if( ! xTau )
  {
    ATH_MSG_FATAL( "accept(...) Failed to cast particle to tau" );
    return m_aAccept;
  }

  // Let the specific function do the work:
  return accept( *xTau );
}

//______________________________________________________________________________
const Root::TAccept& TauSelectionTool::accept( const xAOD::TauJet& xTau ) const
{
  // Reset the result:
  m_aAccept.clear();
  int iNBin = 0;

  if (m_bCreateControlPlots)
  {
    // fill cutflow 'All' bin
    m_hCutFlow->Fill(iNBin);
    // fill main distributions before all cuts
    for (auto entry : m_cMap)
      entry.second->fillHistogramCutPre(xTau);
  }
  try
  {
    for (auto entry : m_cMap)
    {
      if (m_iSelectionCuts & entry.first)
      {
        if (!entry.second->accept(xTau))
          return m_aAccept;
        else
        {
          if (m_bCreateControlPlots)
          {
            // fill cutflow after each passed cut
            iNBin++;
            m_hCutFlow->Fill(iNBin);
          }
        }
      }
    }
  }
  catch (const std::runtime_error& error)
  {
    ATH_MSG_ERROR(error.what());
  }

  // fill main distributions after all cuts
  if (m_bCreateControlPlots)
  {
    for (auto entry : m_cMap)
      entry.second->fillHistogramCut(xTau);
  }

  // // Return the result:
  return m_aAccept;
}

//______________________________________________________________________________
void TauSelectionTool::setOutFile( TFile* fOutFile )
{
  m_fOutFile = fOutFile;
}

//______________________________________________________________________________
void TauSelectionTool::writeControlHistograms()
{
  if (m_bCreateControlPlots and !m_fOutFile)
    ATH_MSG_WARNING("CreateControlPlots was set to true, but no valid file pointer was provided");
  if (m_bCreateControlPlots and m_fOutFile)
  {
    /// create output directory
    m_fOutFile->mkdir((this->name()+"_control").c_str());
    m_fOutFile->cd((this->name()+"_control").c_str());
    /// write cutflow histogram
    m_hCutFlow->Write();

    for (auto entry : m_cMap)
      entry.second->writeControlHistograms();
  }
  /// delete cut pointer, which in case m_bCreateControlPlots==true also writes histograms
}


//=================================PRIVATE-PART=================================
void TauSelectionTool::setupCutFlowHistogram()
{
  // count number of cuts
  int iNBins = 0;
  for (auto entry : m_cMap)
    if (m_iSelectionCuts & entry.first)
      iNBins++;
  // creat cutflow histogram with iNBins+1 bins, where first bin is 'All' bin
  m_hCutFlow = std::make_shared<TH1F>("hCutFlow","CutFlow;; events",iNBins+1,0,iNBins+1);
  m_hCutFlow->GetXaxis()->SetBinLabel(1,"All");

  // reusing this variable to reduce overhead
  iNBins = 2;
  // set bin labels
  for (auto entry : m_cMap)
    if (m_iSelectionCuts & entry.first)
    {
      m_hCutFlow->GetXaxis()->SetBinLabel(iNBins, entry.second->getName().c_str());
      iNBins++;
    }
}

//______________________________________________________________________________
template<typename T, typename U>
void TauSelectionTool::FillRegionVector(std::vector<T>& vRegion, U tMin, U tMax)
{
  if (vRegion.size()>0)
    return;
  if (tMin == tMin) 		// if tMin is NAN, then this assumption fails and -inf is added to the vector
    vRegion.push_back(tMin);
  else
    vRegion.push_back(-std::numeric_limits<T>::infinity());

  if (tMax == tMax)		// if tMax is NAN, then this assumption fails and inf is added to the vector
    vRegion.push_back(tMax);
  else
    vRegion.push_back(std::numeric_limits<T>::infinity());
}

//______________________________________________________________________________
template<typename T, typename U>
void TauSelectionTool::FillValueVector(std::vector<T>& vRegion, U tVal)
{
  if (vRegion.size()>0)
    return;
  if (tVal == tVal)		// if tMax is NAN, then this assumption fails and nothing is added to the vector
    vRegion.push_back(tVal);
}

//______________________________________________________________________________
template<typename T>
void TauSelectionTool::PrintConfigRegion(std::string sCutName, std::vector<T>& vRegion)
{
  unsigned int iNumRegion = vRegion.size()/2;
  for( unsigned int iRegion = 0; iRegion < iNumRegion; iRegion++ )
  {
    ATH_MSG_DEBUG( sCutName<<": " << vRegion.at(iRegion*2) << " to " << vRegion.at(iRegion*2+1) );
  }
}

//______________________________________________________________________________
template<typename T>
void TauSelectionTool::PrintConfigValue(std::string sCutName, std::vector<T>& vRegion)
{
  for (auto tVal : vRegion)
    ATH_MSG_DEBUG( sCutName<<": " << tVal );
}

//______________________________________________________________________________
template<typename T>
void TauSelectionTool::PrintConfigValue(std::string sCutName, T& tVal)
{
  ATH_MSG_DEBUG( sCutName<<": " << tVal );
}

//______________________________________________________________________________
int TauSelectionTool::convertStrToJetIDWP(std::string sJetIDWP)
{
  if (sJetIDWP == "JETIDNONE") return int(JETIDNONE);
  else if (sJetIDWP == "JETIDBDTVERYLOOSE") return int(JETIDBDTVERYLOOSE); // new in rel21 
  else if (sJetIDWP == "JETIDBDTLOOSE") return int(JETIDBDTLOOSE);
  else if (sJetIDWP == "JETIDBDTMEDIUM") return int(JETIDBDTMEDIUM);
  else if (sJetIDWP == "JETIDBDTTIGHT") return int(JETIDBDTTIGHT);
  else if (sJetIDWP == "JETIDBDTLOOSENOTMEDIUM") return int(JETIDBDTLOOSENOTMEDIUM);
  else if (sJetIDWP == "JETIDBDTLOOSENOTTIGHT") return int(JETIDBDTLOOSENOTTIGHT);
  else if (sJetIDWP == "JETIDBDTMEDIUMNOTTIGHT") return int(JETIDBDTMEDIUMNOTTIGHT);
  else if (sJetIDWP == "JETIDBDTNOTLOOSE") return int(JETIDBDTNOTLOOSE);
  else if (sJetIDWP == "JETBDTBKGLOOSE") return int(JETBDTBKGLOOSE); // new in rel21
  else if (sJetIDWP == "JETBDTBKGMEDIUM") return int(JETBDTBKGMEDIUM); // new in rel21
  else if (sJetIDWP == "JETBDTBKGTIGHT") return int(JETBDTBKGTIGHT); // new in rel21
  else if (sJetIDWP == "JETIDRNNVERYLOOSE") return int(JETIDRNNVERYLOOSE);
  else if (sJetIDWP == "JETIDRNNLOOSE") return int(JETIDRNNLOOSE);
  else if (sJetIDWP == "JETIDRNNMEDIUM") return int(JETIDRNNMEDIUM);
  else if (sJetIDWP == "JETIDRNNTIGHT") return int(JETIDRNNTIGHT);

  ATH_MSG_ERROR( "jet ID working point "<<sJetIDWP<<" is unknown, the cut JETIDWP will not accept any tau!" );
  return -1;
}

//______________________________________________________________________________
int TauSelectionTool::convertStrToEleBDTWP(std::string sEleBDTWP)
{
  if (sEleBDTWP == "ELEIDNONE") return int(ELEIDNONE);
  else if (sEleBDTWP == "ELEIDBDTLOOSE") return int(ELEIDBDTLOOSE);
  else if (sEleBDTWP == "ELEIDBDTMEDIUM") return int(ELEIDBDTMEDIUM);
  else if (sEleBDTWP == "ELEIDBDTTIGHT") return int(ELEIDBDTTIGHT);

  ATH_MSG_ERROR( "electron ID working point "<<sEleBDTWP<<" is unknown, the cut EleBDTWP will not accept any tau!" );
  return -1;
}

//______________________________________________________________________________
std::string TauSelectionTool::convertJetIDWPToStr(int iJetIDWP)
{
  switch (iJetIDWP)
  {
  case JETIDNONEUNCONFIGURED:
    return "JETIDNONE";
    break;
  case JETIDNONE:
    return "JETIDNONE";
    break;
  case JETIDBDTLOOSE:
    return "JETIDBDTLOOSE";
    break;
  case JETIDBDTVERYLOOSE:
    return "JETIDBDTVERYLOOSE";
    break;
  case JETIDBDTMEDIUM:
    return "JETIDBDTMEDIUM";
    break;
  case JETIDBDTTIGHT:
    return "JETIDBDTTIGHT";
    break;
  case JETIDBDTLOOSENOTMEDIUM:
    return "JETIDBDTLOOSENOTMEDIUM";
    break;
  case JETIDBDTLOOSENOTTIGHT:
    return "JETIDBDTLOOSENOTTIGHT";
    break;
  case JETIDBDTMEDIUMNOTTIGHT:
    return "JETIDBDTMEDIUMNOTTIGHT";
    break;
  case JETIDBDTNOTLOOSE:
    return "JETIDBDTNOTLOOSE";
    break;
  case JETBDTBKGLOOSE:
    return "JETBDTBKGLOOSE";
    break;
  case JETBDTBKGMEDIUM:
    return "JETBDTBKGMEDIUM";
    break;
  case JETBDTBKGTIGHT:
    return "JETBDTBKGTIGHT";
    break;
  case JETIDRNNVERYLOOSE:
    return "JETIDRNNVERYLOOSE";
    break;
  case JETIDRNNLOOSE:
    return "JETIDRNNLOOSE";
    break;
  case JETIDRNNMEDIUM:
    return "JETIDRNNMEDIUM";
    break;
  case JETIDRNNTIGHT:
    return "JETIDRNNTIGHT";
    break;
  default:
    ATH_MSG_ERROR( "jet ID working point with enum "<<iJetIDWP<<" is unknown, the cut JETIDWP will not accept any tau!" );
    break;
  }
  return "";
}

//______________________________________________________________________________
std::string TauSelectionTool::convertEleBDTWPToStr(int iEleBDTWP)
{
  switch (iEleBDTWP)
  {
  case ELEIDNONEUNCONFIGURED:
    return "ELEIDNONE";
    break;
  case ELEIDNONE:
    return "ELEIDNONE";
    break;
  case ELEIDBDTLOOSE:
    return "ELEIDBDTLOOSE";
    break;
  case ELEIDBDTMEDIUM:
    return "ELEIDBDTMEDIUM";
    break;
  case ELEIDBDTTIGHT:
    return "ELEIDBDTTIGHT";
    break;
  default:
    ATH_MSG_ERROR( "ID working point with enum "<<iEleBDTWP<<" is unknown, the cut EleBDTWP will not accept any tau!" );
    break;
  }

  return "";
}

