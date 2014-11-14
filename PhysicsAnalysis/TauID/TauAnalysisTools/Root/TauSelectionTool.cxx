/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "TauAnalysisTools/TauSelectionTool.h"

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauSelectionTool::TauSelectionTool( const std::string& name )
  : asg::AsgTool( name )
  , m_iSelectionCuts(NoCut) // initialize with 'no' cuts
  , m_vPtRegion( {})
, m_vAbsEtaRegion( {})
, m_vAbsCharges( {})
, m_vNTracks( {})
, m_vJetBDTRegion( {})
, m_iJetIDWP(JETIDNONE)
, m_vEleBDTRegion( {})
, m_iEleBDTWP(ELEIDNONE)
, m_dPtMin(NAN) // in GeV
, m_dPtMax(NAN) // in GeV
, m_dAbsEtaMin(NAN)
, m_dAbsEtaMax(NAN)
, m_iAbsCharge(NAN)
, m_iNTrack(NAN)
, m_dJetBDTMin(NAN)
, m_dJetBDTMax(NAN)
, m_dEleBDTMin(NAN)
, m_dEleBDTMax(NAN)
, m_fOutFile(0)
, m_hCutFlow(0)
, m_aAccept( "TauSelection" )
{
  declarePropertiesBaseline();
  declareProperty( "CreateControlPlots", m_bCreateControlPlots = false);
}

//______________________________________________________________________________
TauSelectionTool::~TauSelectionTool()
{
  if (m_hCutFlow)
    delete m_hCutFlow;
  for (auto entry : m_cMap)
    delete entry.second;
  m_cMap.clear();
}

//______________________________________________________________________________
StatusCode TauSelectionTool::initialize()
{
  m_cMap =
  {
    {CutPt, new TauAnalysisTools::SelectionCutPt(this)},
    {CutAbsEta, new TauAnalysisTools::SelectionCutAbsEta(this)},
    {CutAbsCharge, new TauAnalysisTools::SelectionCutAbsCharge(this)},
    {CutNTrack, new TauAnalysisTools::SelectionCutNTracks(this)},
    {CutJetBDTScore, new TauAnalysisTools::SelectionCutBDTJetScore(this)},
    {CutJetIDWP, new TauAnalysisTools::SelectionCutJetIDWP(this)},
    {CutEleBDTScore, new TauAnalysisTools::SelectionCutBDTEleScore(this)},
    {CutEleBDTWP, new TauAnalysisTools::SelectionCutEleBDTWP(this)},
    {CutMuonVeto, new TauAnalysisTools::SelectionCutMuonVeto(this)}
  };

  ATH_MSG_INFO( "Initialising TauSelectionTool" );
  FillRegionVector(&m_vPtRegion, m_dPtMin, m_dPtMax);
  FillRegionVector(&m_vAbsEtaRegion, m_dAbsEtaMin, m_dAbsEtaMax);
  FillRegionVector(&m_vJetBDTRegion, m_dJetBDTMin, m_dJetBDTMax );
  FillRegionVector(&m_vEleBDTRegion, m_dEleBDTMin, m_dEleBDTMax );
  FillValueVector(&m_vAbsCharges, m_iAbsCharge );
  FillValueVector(&m_vNTracks, m_iNTrack );
  // Greet the user:
  PrintConfigRegion("Pt",&m_vPtRegion);
  PrintConfigRegion("AbsEta",&m_vAbsEtaRegion);
  PrintConfigValue("AbsCharge",&m_vAbsCharges);
  PrintConfigValue("NTrack",&m_vNTracks);
  PrintConfigRegion("BDTJetScore",&m_vJetBDTRegion);
  PrintConfigRegion("BDTEleScore",&m_vEleBDTRegion);
  std::string sCuts = "";
  if (m_iSelectionCuts & CutPt) sCuts+= "Pt ";
  if (m_iSelectionCuts & CutAbsEta) sCuts+= "AbsEta ";
  if (m_iSelectionCuts & CutAbsCharge) sCuts+= "AbsCharge ";
  if (m_iSelectionCuts & CutNTrack) sCuts+= "NTrack ";
  if (m_iSelectionCuts & CutJetBDTScore) sCuts+= "JetBDTScore ";
  if (m_iSelectionCuts & CutJetIDWP) sCuts+= "JetIDWP ";
  if (m_iSelectionCuts & CutEleBDTScore) sCuts+= "EleBDTScore ";
  if (m_iSelectionCuts & CutEleBDTWP) sCuts+= "EleBDTWP ";
  if (m_iSelectionCuts & CutMuonVeto) sCuts+= "CutMuonVeto ";

  ATH_MSG_DEBUG( "cuts: " << sCuts);

  if (m_bCreateControlPlots and m_fOutFile)
    setupCutFlowHistogram();

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
  if (m_bCreateControlPlots and m_fOutFile)
  {
    // fill cutflow 'All' bin
    m_hCutFlow->Fill(iNBin);
    // fill main distributions before all cuts
    for (auto entry : m_cMap)
      entry.second->fillHistogramCutPre(xTau);
  }
  for (auto entry : m_cMap)
    if (m_iSelectionCuts & entry.first)
    {
      if (!entry.second->accept(xTau))
        return m_aAccept;
      else
      {
        if (m_bCreateControlPlots and m_fOutFile)
        {
          // fill cutflow after each passed cut
          iNBin++;
          m_hCutFlow->Fill(iNBin);
        }
      }
    }
  // fill main distributions after all cuts
  if (m_bCreateControlPlots and m_fOutFile)
  {
    for (auto entry : m_cMap)
      entry.second->fillHistogramCut(xTau);
  }

  // // Return the result:
  return m_aAccept;
}

//______________________________________________________________________________
void TauSelectionTool::setRecommendedProperties()
{
  ATH_MSG_INFO( "setting recommended tau selection cuts" );
  m_iSelectionCuts = SelectionCuts(CutPt | CutAbsEta | CutAbsCharge | CutNTrack | CutJetIDWP | CutEleBDTWP );
  m_dPtMin = 20;
  m_vAbsEtaRegion = {0,1.37,1.52,2.5};
  m_iAbsCharge = 1;
  m_vNTracks = {1,3};
  m_iJetIDWP = JETIDBDTMEDIUM;
  m_iEleBDTWP = ELEIDBDTLOOSE;
}

//______________________________________________________________________________
void TauSelectionTool::setOutFile( TFile* fOutFile )
{
  m_fOutFile = fOutFile;
}

//______________________________________________________________________________
void TauSelectionTool::writeControlHistograms()
{
  if (m_bCreateControlPlots and m_fOutFile)
  {
    if (!m_fOutFile)
      ATH_MSG_FATAL("CreateControlPlots was set to true, but no valid file pointer was provided");
    /// create output directory
    m_fOutFile->mkdir((this->name()+"_control").c_str());
    m_fOutFile->cd((this->name()+"_control").c_str());
    /// write cutflow histogram
    m_hCutFlow->Write();
  }
  /// delete cut pointer, which in case m_bCreateControlPlots==true also writes histograms
  for (auto entry : m_cMap)
    entry.second->writeControlHistograms();
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
  m_hCutFlow = new TH1F("hCutFlow","CutFlow;; events",iNBins+1,0,iNBins+1);
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
void TauSelectionTool::declarePropertiesBaseline()
{
  /*
    properties containing 'Region' are a vector of lower and upper bounds
    other properties named in plural are a list of exact values to cut on
    other properties are single cuts
  */
  declareProperty( "SelectionCuts", m_iSelectionCuts); // initialize with 'no' cuts
  declareProperty( "PtRegion", m_vPtRegion ); // in GeV
  declareProperty( "PtMin", m_dPtMin); // in GeV
  declareProperty( "PtMax", m_dPtMax); // in GeV
  declareProperty( "AbsEtaRegion", m_vAbsEtaRegion);
  declareProperty( "AbsEtaMin", m_dAbsEtaMin);
  declareProperty( "AbsEtaMax", m_dAbsEtaMax);
  declareProperty( "AbsCharges", m_vAbsCharges);
  declareProperty( "AbsCharge", m_iAbsCharge);
  declareProperty( "NTracks", m_vNTracks);
  declareProperty( "NTrack", m_iNTrack);
  declareProperty( "JetBDTRegion", m_vJetBDTRegion);
  declareProperty( "JetBDTMin", m_dJetBDTMin);
  declareProperty( "JetBDTMax", m_dJetBDTMax);
  declareProperty( "JetIDWP", m_iJetIDWP);
  declareProperty( "EleBDTRegion", m_vEleBDTRegion);
  declareProperty( "EleBDTMin", m_dEleBDTMin);
  declareProperty( "EleBDTMax", m_dEleBDTMax);
  declareProperty( "EleBDTWP", m_iEleBDTWP);
}

//______________________________________________________________________________
template<typename T, typename U>
void TauSelectionTool::FillRegionVector(std::vector<T>* vRegion, U tMin, U tMax)
{
  if (vRegion->size()>0)
    return;
  if (tMin == tMin) 		// if tMin is NAN, then this assumption fails and -inf is added to the vector
    vRegion->push_back(tMin);
  else
    // vRegion->push_back(-1./0.);
    vRegion->push_back(-std::numeric_limits<T>::infinity());

  if (tMax == tMax)		// if tMax is NAN, then this assumption fails and inf is added to the vector
    vRegion->push_back(tMax);
  else
    vRegion->push_back(std::numeric_limits<T>::infinity());
}

//______________________________________________________________________________
template<typename T, typename U>
void TauSelectionTool::FillValueVector(std::vector<T>* vRegion, U tVal)
{
  if (vRegion->size()>0)
    return;
  if (tVal == tVal)		// if tMax is NAN, then this assumption fails and nothing is added to the vector
    vRegion->push_back(tVal);
}

//______________________________________________________________________________
template<typename T>
void TauSelectionTool::PrintConfigRegion(std::string sCutName, std::vector<T>* vRegion)
{
  unsigned int iNumRegion = vRegion->size()/2;
  for( unsigned int iRegion = 0; iRegion < iNumRegion; iRegion++ )
  {
    ATH_MSG_DEBUG( sCutName<<": " << vRegion->at(iRegion*2) << " to " << vRegion->at(iRegion*2+1) );
  }
}

//______________________________________________________________________________
template<typename T>
void TauSelectionTool::PrintConfigValue(std::string sCutName, std::vector<T>* vRegion)
{
  for( unsigned int iRegion = 0; iRegion < vRegion->size(); iRegion++ )
  {
    ATH_MSG_DEBUG( sCutName<<": " << vRegion->at(iRegion) );
  }
}
