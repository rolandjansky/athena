/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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


using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauSelectionTool::TauSelectionTool( const std::string& name )
  : asg::AsgMetadataTool( name )
  , m_sJetIDWP("JETIDNONE")
  , m_sEleRNNWP("ELEIDNONE")
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
  declareProperty( "JetRNNSigTransRegion", m_vJetRNNSigTransRegion  = {});
  declareProperty( "JetRNNSigTransMin", m_dJetRNNSigTransMin = NAN);
  declareProperty( "JetRNNSigTransMax", m_dJetRNNSigTransMax = NAN);
  declareProperty( "JetIDWP",       m_iJetIDWP       = 0);
  declareProperty( "EleRNNRegion",  m_vEleRNNRegion  = {});
  declareProperty( "EleRNNMin",     m_dEleRNNMin     = NAN);
  declareProperty( "EleRNNMax",     m_dEleRNNMax     = NAN);
  declareProperty( "EleRNNWP",      m_iEleRNNWP      = 0);
  declareProperty( "MuonOLR",       m_bMuonOLR       = false);

  m_sConfigPath = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/Selection/recommended_selection_mc15.conf";

  declareProperty( "ConfigPath",            m_sConfigPath);
  declareProperty( "ElectronContainerName", m_sElectronContainerName);
  declareProperty( "MuonContainerName",     m_sMuonContainerName);

}

//______________________________________________________________________________
TauSelectionTool::~TauSelectionTool()
{
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
  if (!bConfigViaProperties and !m_vJetRNNSigTransRegion.empty())         bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dJetRNNSigTransMin == m_dJetRNNSigTransMin) bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dJetRNNSigTransMax == m_dJetRNNSigTransMax) bConfigViaProperties = true;
  if (!bConfigViaProperties and m_iJetIDWP != 0)              bConfigViaProperties = true;
  if (!bConfigViaProperties and !m_vEleRNNRegion.empty())     bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dEleRNNMin == m_dEleRNNMin) bConfigViaProperties = true;
  if (!bConfigViaProperties and m_dEleRNNMax == m_dEleRNNMax) bConfigViaProperties = true;
  if (!bConfigViaProperties and m_iEleRNNWP != 0)             bConfigViaProperties = true;
  if (!bConfigViaProperties and m_bMuonOLR == true)           bConfigViaProperties = true;

  if (bConfigViaConfigFile and bConfigViaProperties)
  {
    ATH_MSG_WARNING("Configured tool via setProperty and configuration file, which may lead to unexpected configuration.");
    ATH_MSG_WARNING("In doubt check the configuration that is printed when the tool is initialized and the message level is set to debug");
    ATH_MSG_WARNING("For further details please refer to the documentation:");
    ATH_MSG_WARNING("https://gitlab.cern.ch/atlas/athena/blob/master/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauSelectionTool.rst");
  }
  if (!bConfigViaConfigFile and !bConfigViaProperties)
  {
    ATH_MSG_WARNING("No cut configuration provided, the tool will not do anything. For further details please refer to the documentation:");
    ATH_MSG_WARNING("https://gitlab.cern.ch/atlas/athena/blob/master/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauSelectionTool.rst");
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
      else if (sCut == "EleRNNRegion")
      {
        iSelectionCuts = iSelectionCuts | CutEleRNNScore;
        if (m_vEleRNNRegion.size() == 0)
          TauAnalysisTools::split(rEnv,"EleRNNRegion", ';', m_vEleRNNRegion);
      }
      else if (sCut == "EleRNNMin")
      {
        iSelectionCuts = iSelectionCuts | CutEleRNNScore;
        if (m_dEleRNNMin != m_dEleRNNMin)
          m_dEleRNNMin = rEnv.GetValue("EleRNNMin",NAN);
      }
      else if (sCut == "EleRNNMax")
      {
        iSelectionCuts = iSelectionCuts | CutEleRNNScore;
        if (m_dEleRNNMax != m_dEleRNNMax)
          m_dEleRNNMax = rEnv.GetValue("EleRNNMax",NAN);
      }
      else if (sCut == "JetIDWP")
      {
        iSelectionCuts = iSelectionCuts | CutJetIDWP;
        if (m_iJetIDWP == JETIDNONEUNCONFIGURED)
          m_iJetIDWP = convertStrToJetIDWP(rEnv.GetValue("JetIDWP","JETIDNONE"));
      }
      else if (sCut == "EleRNNWP")
      {
        iSelectionCuts = iSelectionCuts | CutEleRNNWP;
        if (m_iEleRNNWP == ELEIDNONEUNCONFIGURED)
          m_iEleRNNWP = convertStrToEleRNNWP(rEnv.GetValue("EleRNNWP","ELEIDNONE"));
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

  m_sJetIDWP = convertJetIDWPToStr(m_iJetIDWP);
  m_sEleRNNWP = convertEleRNNWPToStr(m_iEleRNNWP);

  // specify all available cut descriptions
  using map_type  = std::map<SelectionCuts, std::unique_ptr<TauAnalysisTools::SelectionCut>>;
  using pair_type = map_type::value_type;

  pair_type elements[] =
  {
   {CutPt, std::make_unique<TauAnalysisTools::SelectionCutPt>(this)},
   {CutAbsEta, std::make_unique<TauAnalysisTools::SelectionCutAbsEta>(this)},
   {CutAbsCharge, std::make_unique<TauAnalysisTools::SelectionCutAbsCharge>(this)},
   {CutNTrack, std::make_unique<TauAnalysisTools::SelectionCutNTracks>(this)},
   {CutJetRNNScoreSigTrans, std::make_unique<TauAnalysisTools::SelectionCutRNNJetScoreSigTrans>(this)},
   {CutJetIDWP, std::make_unique<TauAnalysisTools::SelectionCutJetIDWP>(this)},
   {CutEleRNNScore, std::make_unique<TauAnalysisTools::SelectionCutRNNEleScore>(this)},
   {CutEleRNNWP, std::make_unique<TauAnalysisTools::SelectionCutEleRNNWP>(this)},
   {CutMuonOLR, std::make_unique<TauAnalysisTools::SelectionCutMuonOLR>(this)}
  };
  
  m_cMap = { std::make_move_iterator( begin(elements) ), std::make_move_iterator( end(elements) ) };
  
  ATH_MSG_INFO( "Initializing TauSelectionTool" );
  FillRegionVector(m_vPtRegion, m_dPtMin, m_dPtMax);
  FillRegionVector(m_vAbsEtaRegion, m_dAbsEtaMin, m_dAbsEtaMax);
  FillRegionVector(m_vJetRNNSigTransRegion, m_dJetRNNSigTransMin, m_dJetRNNSigTransMax );
  FillRegionVector(m_vEleRNNRegion, m_dEleRNNMin, m_dEleRNNMax );
  FillValueVector(m_vAbsCharges, m_iAbsCharge );
  FillValueVector(m_vNTracks, m_iNTrack );


  PrintConfigRegion ("Pt",          m_vPtRegion);
  PrintConfigRegion ("AbsEta",      m_vAbsEtaRegion);
  PrintConfigValue  ("AbsCharge",   m_vAbsCharges);
  PrintConfigValue  ("NTrack",      m_vNTracks);
  PrintConfigRegion ("RNNJetScoreSigTrans", m_vJetRNNSigTransRegion);
  PrintConfigRegion ("RNNEleScore", m_vEleRNNRegion);
  PrintConfigValue  ("JetIDWP",     m_sJetIDWP);
  PrintConfigValue  ("JetIDWP ENUM",m_iJetIDWP);
  PrintConfigValue  ("EleRNNDWP",   m_sEleRNNWP);
  PrintConfigValue  ("EleRNNDWP ENUM",m_iEleRNNWP);
  PrintConfigValue  ("MuonOLR",    m_bMuonOLR);

  std::string sCuts = "";
  if (m_iSelectionCuts & CutPt) sCuts+= "Pt ";
  if (m_iSelectionCuts & CutAbsEta) sCuts+= "AbsEta ";
  if (m_iSelectionCuts & CutAbsCharge) sCuts+= "AbsCharge ";
  if (m_iSelectionCuts & CutNTrack) sCuts+= "NTrack ";
  if (m_iSelectionCuts & CutJetRNNScoreSigTrans) sCuts+= "JetRNNScoreSigTrans ";
  if (m_iSelectionCuts & CutJetIDWP) sCuts+= "JetIDWP ";
  if (m_iSelectionCuts & CutEleRNNScore) sCuts+= "EleRNNScore ";
  if (m_iSelectionCuts & CutEleRNNWP) sCuts+= "EleRNNWP ";
  if (m_iSelectionCuts & CutMuonOLR) sCuts+= "MuonOLR ";

  ATH_MSG_DEBUG( "cuts: " << sCuts);

  if (m_bCreateControlPlots)
    setupCutFlowHistogram();

  for ( const auto& entry : m_cMap ) {
     if ( m_iSelectionCuts &entry.first ) {
	entry.second->setAcceptInfo(m_aAccept);
     }
  }
  
  return StatusCode::SUCCESS;
}


//______________________________________________________________________________
StatusCode TauSelectionTool::beginEvent()
{
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
const asg::AcceptInfo& TauSelectionTool::getAcceptInfo() const
{
  return m_aAccept;
}

//______________________________________________________________________________
asg::AcceptData
TauSelectionTool::accept( const xAOD::IParticle* xP ) const
{
  // Check if this is a tau:
  if( xP->type() != xAOD::Type::Tau )
  {
    ATH_MSG_ERROR( "accept(...) Function received a non-tau" );
    return asg::AcceptData (&m_aAccept);
  }

  // Cast it to a tau:
  const xAOD::TauJet* xTau = dynamic_cast< const xAOD::TauJet* >( xP );
  if( ! xTau )
  {
    ATH_MSG_FATAL( "accept(...) Failed to cast particle to tau" );
    return asg::AcceptData (&m_aAccept);
  }

  // Let the specific function do the work:
  return accept( *xTau );
}

//______________________________________________________________________________
asg::AcceptData
TauSelectionTool::accept( const xAOD::TauJet& xTau ) const
{
  asg::AcceptData acceptData (&m_aAccept);

  int iNBin = 0;

  if (m_bCreateControlPlots)
  {
    // fill cutflow 'All' bin
    m_hCutFlow->Fill(iNBin);
    // fill main distributions before all cuts
    for (const auto& entry : m_cMap)
      entry.second->fillHistogramCutPre(xTau);
  }
  try
  {
    for (const auto& entry : m_cMap)
    {
      if (m_iSelectionCuts & entry.first)
      {
        if (!entry.second->accept(xTau, acceptData))
          return acceptData;
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
    for (const auto& entry : m_cMap)
      entry.second->fillHistogramCut(xTau);
  }

  // // Return the result:
  return acceptData;
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

    for (const auto& entry : m_cMap)
      entry.second->writeControlHistograms();
  }
  /// delete cut pointer, which in case m_bCreateControlPlots==true also writes histograms
}


//=================================PRIVATE-PART=================================
void TauSelectionTool::setupCutFlowHistogram()
{
  // count number of cuts
  int iNBins = 0;
  for (const auto& entry : m_cMap)
    if (m_iSelectionCuts & entry.first)
      iNBins++;
  // creat cutflow histogram with iNBins+1 bins, where first bin is 'All' bin
  m_hCutFlow = std::make_shared<TH1F>("hCutFlow","CutFlow;; events",iNBins+1,0,iNBins+1);
  m_hCutFlow->GetXaxis()->SetBinLabel(1,"All");

  // reusing this variable to reduce overhead
  iNBins = 2;
  // set bin labels
  for (const auto& entry : m_cMap)
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
void TauSelectionTool::PrintConfigRegion(const std::string& sCutName, std::vector<T>& vRegion)
{
  unsigned int iNumRegion = vRegion.size()/2;
  for( unsigned int iRegion = 0; iRegion < iNumRegion; iRegion++ )
  {
    ATH_MSG_DEBUG( sCutName<<": " << vRegion.at(iRegion*2) << " to " << vRegion.at(iRegion*2+1) );
  }
}

//______________________________________________________________________________
template<typename T>
void TauSelectionTool::PrintConfigValue(const std::string& sCutName, std::vector<T>& vRegion)
{
  for (auto tVal : vRegion)
    ATH_MSG_DEBUG( sCutName<<": " << tVal );
}

//______________________________________________________________________________
template<typename T>
void TauSelectionTool::PrintConfigValue(const std::string& sCutName, T& tVal)
{
  ATH_MSG_DEBUG( sCutName<<": " << tVal );
}

//______________________________________________________________________________
int TauSelectionTool::convertStrToJetIDWP(const std::string& sJetIDWP)
{
  if (sJetIDWP == "JETIDNONE") return int(JETIDNONE);
  else if (sJetIDWP == "JETIDRNNVERYLOOSE") return int(JETIDRNNVERYLOOSE);
  else if (sJetIDWP == "JETIDRNNLOOSE") return int(JETIDRNNLOOSE);
  else if (sJetIDWP == "JETIDRNNMEDIUM") return int(JETIDRNNMEDIUM);
  else if (sJetIDWP == "JETIDRNNTIGHT") return int(JETIDRNNTIGHT);

  ATH_MSG_ERROR( "jet ID working point "<<sJetIDWP<<" is unknown, the cut JETIDWP will not accept any tau!" );
  return -1;
}

//______________________________________________________________________________
int TauSelectionTool::convertStrToEleRNNWP(const std::string& sEleRNNWP)
{
  if (sEleRNNWP == "ELEIDNONE") return int(ELEIDNONE);
  else if (sEleRNNWP == "ELEIDRNNLOOSE") return int(ELEIDRNNLOOSE);
  else if (sEleRNNWP == "ELEIDRNNMEDIUM") return int(ELEIDRNNMEDIUM);
  else if (sEleRNNWP == "ELEIDRNNTIGHT") return int(ELEIDRNNTIGHT);

  ATH_MSG_ERROR( "electron ID working point "<<sEleRNNWP<<" is unknown, the cut EleRNNWP will not accept any tau!" );
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
std::string TauSelectionTool::convertEleRNNWPToStr(int iEleRNNWP)
{
  switch (iEleRNNWP)
  {
  case ELEIDNONEUNCONFIGURED:
    return "ELEIDNONE";
    break;
  case ELEIDNONE:
    return "ELEIDNONE";
    break;
  case ELEIDRNNLOOSE:
    return "ELEIDRNNLOOSE";
    break;
  case ELEIDRNNMEDIUM:
    return "ELEIDRNNMEDIUM";
    break;
  case ELEIDRNNTIGHT:
    return "ELEIDRNNTIGHT";
    break;
  default:
    ATH_MSG_ERROR( "ID working point with enum "<<iEleRNNWP<<" is unknown, the cut EleRNNWP will not accept any tau!" );
    break;
  }

  return "";
}

