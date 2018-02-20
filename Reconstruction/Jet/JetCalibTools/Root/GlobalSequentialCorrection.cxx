/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  Calculates global sequential jet calibration factors
 *   -Requires jet branches trackWIDTH, nTrk, Tile0, EM3
 *   -Apply using
 *    1. No jet area correction: JetCalibrationTool::ApplyOffsetEtaJESGSC
 *    2. With jet area correction (not yet supported): JetCalibrationTool::ApplyJetAreaOffsetEtaJESGSC
 *   -GSC correction factor is returned by JetCalibrationTool::GetGSC
 *    TFile* inputFile = NULL;
    inputFile = openInputFile(m_fileName);
 *
 *  Extension of the ApplyJetCalibrationTool
 *
 *  Authors: Joe Taenzer (joseph.taenzer@cern.ch), Reina Camacho, Jonathan Bossio (jbossios@cern.ch)
 *
 */

#include <TKey.h>

#include "xAODMuon/MuonSegmentContainer.h"

#include "JetCalibTools/CalibrationMethods/GlobalSequentialCorrection.h"
#include "PathResolver/PathResolver.h"


GlobalSequentialCorrection::GlobalSequentialCorrection()
  : JetCalibrationToolBase::JetCalibrationToolBase("GlobalSequentialCorrection::GlobalSequentialCorrection"),
    m_config(NULL), m_jetAlgo(""), m_calibAreaTag(""), m_dev(false),
    m_binSize(0.1), m_depth(0), 
    m_trackWIDTHMaxEtaBin(25), m_nTrkMaxEtaBin(25), m_Tile0MaxEtaBin(17), m_EM3MaxEtaBin(35), m_chargedFractionMaxEtaBin(27),
    m_punchThroughMinPt(50)
   
{ }

GlobalSequentialCorrection::GlobalSequentialCorrection(const std::string& name)
  : JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(NULL), m_jetAlgo(""), m_calibAreaTag(""), m_dev(false),
    m_binSize(0.1), m_depth(0), 
    m_trackWIDTHMaxEtaBin(25), m_nTrkMaxEtaBin(25), m_Tile0MaxEtaBin(17), m_EM3MaxEtaBin(35), m_chargedFractionMaxEtaBin(27),
    m_punchThroughMinPt(50)
   
{ }

GlobalSequentialCorrection::GlobalSequentialCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool dev)
  : JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(config), m_jetAlgo(jetAlgo), m_calibAreaTag(calibAreaTag), m_dev(dev),
    m_binSize(0.1), m_depth(0),
    m_trackWIDTHMaxEtaBin(25), m_nTrkMaxEtaBin(25), m_Tile0MaxEtaBin(17), m_EM3MaxEtaBin(35), m_chargedFractionMaxEtaBin(27),
    m_punchThroughMinPt(50)
{ }

GlobalSequentialCorrection::~GlobalSequentialCorrection() {

}

StatusCode GlobalSequentialCorrection::initializeTool(const std::string&) {

  ATH_MSG_INFO("Initializing the Global Sequential Calibration tool");

  // Set m_PFlow
  if( m_jetAlgo == "AntiKt4EMPFlow" ) m_PFlow = true;
  else{m_PFlow=false;}

  m_jetStartScale = m_config->GetValue("GSCStartingScale","JetEtaJESScaleMomentum");
  m_turnOffTrackCorrections = m_config->GetValue("TurnOffTrackCorrections", false);
  m_turnOffStartingpT = m_config->GetValue("TurnOffStartingpT", 1200);
  m_turnOffEndpT = m_config->GetValue("TurnOffEndpT", 2000);
  m_pTResponseRequirementOff = m_config->GetValue("PTResponseRequirementOff", false);

  // In release 21, the nTrk and trackWIDTH corrections are also included for PFlow jets
  // The default is set to false to maintain the backwards compatibility
  m_nTrkwTrk_4PFlow = m_config->GetValue("nTrkwTrk4PFlow", false);

  if ( !m_config ) { ATH_MSG_FATAL("Config file not specified. Aborting."); return StatusCode::FAILURE; }
  if ( m_jetAlgo.EqualTo("") ) { ATH_MSG_FATAL("No jet algorithm specified. Aborting."); return StatusCode::FAILURE; }

  //find the ROOT file containing response histograms, path comes from the config file.
  TString GSCFile = m_config->GetValue("GSCFactorsFile","empty");
  if ( GSCFile.EqualTo("empty") ) { 
    ATH_MSG_FATAL("NO GSCFactorsFile specified. Aborting.");
    return StatusCode::FAILURE;
  }
  if(m_dev){
    GSCFile.Remove(0,33);
    GSCFile.Insert(0,"JetCalibTools/");
  }
  else{GSCFile.Insert(14,m_calibAreaTag);}
  TString fileName = PathResolverFindCalibFile(GSCFile.Data());
  TFile *inputFile = TFile::Open(fileName);
  if (!inputFile){
    ATH_MSG_FATAL("Cannot open GSC factors file" << fileName);
    return StatusCode::FAILURE;
  }

  TString depthString = m_config->GetValue("GSCDepth","Full");
  if ( !depthString.Contains("ChargedFraction") && !depthString.Contains("Tile0") && !depthString.Contains("EM3") && !depthString.Contains("nTrk") && !depthString.Contains("trackWIDTH") && !depthString.Contains("PunchThrough") && !depthString.Contains("Full") ) {
    ATH_MSG_FATAL("depthString flag not properly set, please check your config file.");
    return StatusCode::FAILURE;
  }

  // Protection against requesting nTrk or trackWIDTH corrections for PFlow jets when m_nTrkwTrk_4PFlow is false
  if ( !m_nTrkwTrk_4PFlow && (depthString.Contains("nTrk")||depthString.Contains("trackWIDTH")) && m_PFlow ){
    ATH_MSG_FATAL("depthString flag not properly set, please check your config file. nTrkwTrk4PFlow should be set to true to apply nTrk or trackWIDTH corrections to PFlow jets");
    return StatusCode::FAILURE;
  }

  //ATH_MSG_INFO("  for " << m_jetAlgo << " jets\n\n");

  if ( depthString.Contains("PunchThrough") || depthString.Contains("Full") ) {
    setPunchThroughEtaBins( JetCalibUtils::VectorizeD( m_config->GetValue("PunchThroughEtaBins","") ) );
    setPunchThroughMinPt( m_config->GetValue("PunchThroughMinPt",50) );
  }

  //set the depth private variable, used to determine which parts of the GS calibration are applied
  if( !m_PFlow ){
    if ( depthString.Contains("PunchThrough") || depthString.Contains("Full") ) m_depth = ApplyTile0 | ApplyEM3 | ApplynTrk | ApplytrackWIDTH | ApplyPunchThrough;
    else if ( depthString.Contains("trackWIDTH") ) m_depth = ApplyTile0 | ApplyEM3 | ApplynTrk | ApplytrackWIDTH;
    else if ( depthString.Contains("nTrk") ) m_depth = ApplyTile0 | ApplyEM3 | ApplynTrk;
    else if ( depthString.Contains("EM3") ) m_depth = ApplyTile0 | ApplyEM3;
    else if ( depthString.Contains("Tile0") ) m_depth = ApplyTile0;
    else { ATH_MSG_FATAL("depthString flag not properly set, please check your config file."); return StatusCode::FAILURE; }
  } 
  else{
    if(!m_nTrkwTrk_4PFlow){
      if ( depthString.Contains("PunchThrough") || depthString.Contains("Full") ) m_depth = ApplyChargedFraction | ApplyTile0 | ApplyEM3 | ApplyPunchThrough;
      else if ( depthString.Contains("EM3") ) m_depth = ApplyChargedFraction | ApplyTile0 | ApplyEM3;
      else if ( depthString.Contains("Tile0") ) m_depth = ApplyChargedFraction | ApplyTile0;
      else if ( depthString.Contains("ChargedFraction") ) m_depth = ApplyChargedFraction;
      else { ATH_MSG_FATAL("depthString flag not properly set, please check your config file."); return StatusCode::FAILURE; }
    } else {
      if ( depthString.Contains("PunchThrough") || depthString.Contains("Full") ) m_depth = ApplyChargedFraction | ApplyTile0 | ApplyEM3 | ApplynTrk | ApplytrackWIDTH | ApplyPunchThrough;
      else if ( depthString.Contains("trackWIDTH") ) m_depth = ApplyChargedFraction | ApplyTile0 | ApplyEM3 | ApplynTrk | ApplytrackWIDTH;
      else if ( depthString.Contains("nTrk") ) m_depth = ApplyChargedFraction | ApplyTile0 | ApplyEM3 | ApplynTrk;
      else if ( depthString.Contains("EM3") ) m_depth = ApplyChargedFraction | ApplyTile0 | ApplyEM3;
      else if ( depthString.Contains("Tile0") ) m_depth = ApplyChargedFraction | ApplyTile0;
      else if ( depthString.Contains("ChargedFraction") ) m_depth = ApplyChargedFraction;
      else { ATH_MSG_FATAL("depthString flag not properly set, please check your config file."); return StatusCode::FAILURE; }
    }
  }

  //Get a TList of TKeys pointing to the histograms contained in the ROOT file
  inputFile->cd();
  TList *keys = inputFile->GetListOfKeys();
  std::vector<TString> histoNames;
  //fill the names of the TKeys into a vector of TStrings
  TIter ikeys(keys);
  while ( TKey *iterobj = (TKey*)ikeys() ) { histoNames.push_back( iterobj->GetName() ); }

  //Grab the TH2Fs from the ROOT file and put them into a vectors of TH2Fs
  for (uint ihisto=0; ihisto<histoNames.size(); ++ihisto) {
    if ( !histoNames[ihisto].Contains( m_jetAlgo.Data() ) ) continue;
    else if ( ihisto>0 && histoNames[ihisto].Contains( histoNames[ihisto-1].Data() ) ) continue;
    else if ( histoNames[ihisto].Contains("EM3") && m_respFactorsEM3.size() < m_EM3MaxEtaBin) 
      m_respFactorsEM3.push_back( (TH2F*)JetCalibUtils::GetHisto2(inputFile,histoNames[ihisto]) );
    else if ( histoNames[ihisto].Contains("nTrk") && m_respFactorsnTrk.size() < m_nTrkMaxEtaBin) 
      m_respFactorsnTrk.push_back( (TH2F*)JetCalibUtils::GetHisto2(inputFile,histoNames[ihisto]) );
    else if ( histoNames[ihisto].Contains("Tile0") && m_respFactorsTile0.size() < m_Tile0MaxEtaBin) 
      m_respFactorsTile0.push_back( (TH2F*)JetCalibUtils::GetHisto2(inputFile,histoNames[ihisto]) );
    else if ( histoNames[ihisto].Contains("chargedFraction") && m_respFactorsChargedFraction.size() < m_chargedFractionMaxEtaBin)
      m_respFactorsChargedFraction.push_back( (TH2F*)JetCalibUtils::GetHisto2(inputFile,histoNames[ihisto]) );
    else if ( histoNames[ihisto].Contains("trackWIDTH") && m_respFactorstrackWIDTH.size() < m_trackWIDTHMaxEtaBin) 
      m_respFactorstrackWIDTH.push_back( (TH2F*)JetCalibUtils::GetHisto2(inputFile,histoNames[ihisto]) );
    else if ( histoNames[ihisto].Contains("PunchThrough") ) 
      m_respFactorsPunchThrough.push_back( (TH2F*)JetCalibUtils::GetHisto2(inputFile,histoNames[ihisto]) );
  }

  //Make sure we put something in the vectors of TH2Fs
  if( !m_PFlow ){
    if ( (m_depth & ApplyEM3) && m_respFactorsEM3.size() < 3 ) {
      ATH_MSG_FATAL("Vector of EM3 histograms may be empty. Please check your GSCFactors file: " << GSCFile);
      return StatusCode::FAILURE;
    }
    else if ( (m_depth & ApplynTrk) &&m_respFactorsnTrk.size() < 3 ) {
      ATH_MSG_FATAL("Vector of nTrk histograms may be empty. Please check your GSCFactors file: " << GSCFile);
      return StatusCode::FAILURE;
    }
    else if ( (m_depth & ApplyTile0) && m_respFactorsTile0.size() < 3 ) {
      ATH_MSG_FATAL("Vector of Tile0 histograms may be empty. Please check your GSCFactors file: " << GSCFile);
      return StatusCode::FAILURE;
    }
    else if ( (m_depth & ApplytrackWIDTH) && m_respFactorstrackWIDTH.size() < 3 ) {
      ATH_MSG_FATAL("Vector of trackWIDTH histograms may be empty. Please check your GSCFactors file: " << GSCFile);
      return StatusCode::FAILURE;
    }
    else if ( (m_depth & ApplyPunchThrough) && m_respFactorsPunchThrough.size() < 2 ) {
      ATH_MSG_FATAL("Vector of PunchThrough histograms may be empty. Please check your GSCFactors file: " << GSCFile);
      return StatusCode::FAILURE;
    }
    else ATH_MSG_INFO("GSC Tool has been initialized with binning and eta fit factors from: " << fileName);
  }
  else{
    if ( (m_depth & ApplyChargedFraction) && m_respFactorsChargedFraction.size() < 3 ) {
      ATH_MSG_FATAL("Vector of ChargedFraction histograms may be empty. Please check your GSCFactors file: " << GSCFile);
      return StatusCode::FAILURE;
    }
    else if ( (m_depth & ApplyEM3) && m_respFactorsEM3.size() < 3 ) {
      ATH_MSG_FATAL("Vector of EM3 histograms may be empty. Please check your GSCFactors file: " << GSCFile);
      return StatusCode::FAILURE;
    }
    else if ( (m_depth & ApplyTile0) && m_respFactorsTile0.size() < 3 ) {
      ATH_MSG_FATAL("Vector of Tile0 histograms may be empty. Please check your GSCFactors file: " << GSCFile);
      return StatusCode::FAILURE;
    }
    else if ( m_nTrkwTrk_4PFlow && (m_depth & ApplynTrk) && m_respFactorsnTrk.size() < 3 ) {
      ATH_MSG_FATAL("Vector of nTrk histograms may be empty. Please check your GSCFactors file: " << GSCFile);
      return StatusCode::FAILURE;
    }
    else if ( m_nTrkwTrk_4PFlow && (m_depth & ApplytrackWIDTH) && m_respFactorstrackWIDTH.size() < 3 ) {
      ATH_MSG_FATAL("Vector of trackWIDTH histograms may be empty. Please check your GSCFactors file: " << GSCFile);
      return StatusCode::FAILURE;
    }
    else if ( (m_depth & ApplyPunchThrough) && m_respFactorsPunchThrough.size() < 2 ) {
      ATH_MSG_FATAL("Vector of PunchThrough histograms may be empty. Please check your GSCFactors file: " << GSCFile);
      return StatusCode::FAILURE;
    }
    else ATH_MSG_INFO("GSC Tool has been initialized with binning and eta fit factors from: " << fileName);
  }
  return StatusCode::SUCCESS;

}

double GlobalSequentialCorrection::readPtJetPropertyHisto(double pT, double jetProperty, TH2F *respFactors) const {
 int pTbin = respFactors->GetXaxis()->FindBin(pT);
 int pTMinbin = respFactors->GetXaxis()->GetFirst();
 int pTMaxbin = respFactors->GetXaxis()->GetLast();
 int jetPropbin = respFactors->GetYaxis()->FindBin(jetProperty);
 int jetPropMinbin = respFactors->GetYaxis()->GetFirst();
 int jetPropMaxbin = respFactors->GetYaxis()->GetLast();
 //Protection against input values that are outside the histogram range, which would cause TH2::Interpolate to throw an error
 if (pTbin < pTMinbin) pT = respFactors->GetXaxis()->GetBinLowEdge(pTMinbin)+1e-6;
 else if (pTbin > pTMaxbin) pT = respFactors->GetXaxis()->GetBinUpEdge(pTMaxbin)-1e-6;
 if (jetPropbin < jetPropMinbin) jetProperty = respFactors->GetYaxis()->GetBinLowEdge(jetPropMinbin)+1e-6;
 else if (jetPropbin > jetPropMaxbin) jetProperty = respFactors->GetYaxis()->GetBinUpEdge(jetPropMaxbin)-1e-6;
 //TH2::Interpolate is a bilinear interpolation from the bin centers.
 return respFactors->Interpolate(pT, jetProperty);
}

double GlobalSequentialCorrection::getTrackWIDTHResponse(double pT, uint etabin, double trackWIDTH) const {
  if (trackWIDTH<=0) return 1;
  if ( etabin >= m_respFactorstrackWIDTH.size() ) return 1.;
  //jets with no tracks are assigned a trackWIDTH of -1, we use the trackWIDTH=0 correction in those cases
  double trackWIDTHResponse;
  if(m_turnOffTrackCorrections){
    if(pT>=m_turnOffStartingpT && pT<=m_turnOffEndpT){
      double responseatStartingpT = readPtJetPropertyHisto(m_turnOffStartingpT, trackWIDTH, m_respFactorstrackWIDTH[etabin]);
      trackWIDTHResponse = (1-responseatStartingpT)/(m_turnOffEndpT-m_turnOffStartingpT);
      trackWIDTHResponse *= pT;
      trackWIDTHResponse += 1 - (m_turnOffEndpT*(1-responseatStartingpT)/(m_turnOffEndpT-m_turnOffStartingpT));
      return trackWIDTHResponse;
    }
    else if(pT>m_turnOffEndpT) return 1;
  }
  trackWIDTHResponse = readPtJetPropertyHisto(pT, trackWIDTH, m_respFactorstrackWIDTH[etabin]);
  return trackWIDTHResponse;
}

double GlobalSequentialCorrection::getNTrkResponse(double pT, uint etabin, double nTrk) const {
  if (nTrk<=0) return 1; //nTrk < 0 is unphysical, nTrk = 0 is a special case, so return 1 for nTrk <= 0
  if ( etabin >= m_respFactorsnTrk.size() ) return 1.;
  double nTrkResponse;
  if(m_turnOffTrackCorrections){
    if(pT>=m_turnOffStartingpT && pT<=m_turnOffEndpT){
      double responseatStartingpT = readPtJetPropertyHisto(m_turnOffStartingpT, nTrk, m_respFactorsnTrk[etabin]);
      nTrkResponse = (1-responseatStartingpT)/(m_turnOffEndpT-m_turnOffStartingpT);
      nTrkResponse *= pT;
      nTrkResponse += 1 - (m_turnOffEndpT*(1-responseatStartingpT)/(m_turnOffEndpT-m_turnOffStartingpT));
      return nTrkResponse;
    }
    else if(pT>m_turnOffEndpT) return 1;
  }
  nTrkResponse = readPtJetPropertyHisto(pT, nTrk, m_respFactorsnTrk[etabin]);
  return nTrkResponse;
}

double GlobalSequentialCorrection::getTile0Response(double pT, uint etabin, double Tile0) const {
  if (Tile0<0) return 1; //Tile0 < 0 is unphysical, so we return 1
  if ( etabin >= m_respFactorsTile0.size() ) return 1.;
  double Tile0Response = readPtJetPropertyHisto(pT, Tile0, m_respFactorsTile0[etabin]);
  return Tile0Response;
}

double GlobalSequentialCorrection::getEM3Response(double pT, uint etabin, double EM3) const {
  if (EM3<=0) return 1; //EM3 < 0 is unphysical, EM3 = 0 is a special case, so we return 1 for EM3 <= 0
  if ( etabin >= m_respFactorsEM3.size() ) return 1.;
  double EM3Response = readPtJetPropertyHisto(pT, EM3, m_respFactorsEM3[etabin]);
  return EM3Response;
}

double GlobalSequentialCorrection::getChargedFractionResponse(double pT, uint etabin, double ChargedFraction) const {
  if (ChargedFraction<=0) return 1; //ChargedFraction < 0 is unphysical, ChargedFraction = 0 is a special case, so we return 1 for ChargedFraction <= 0
  if ( etabin >= m_respFactorsChargedFraction.size() ) return 1.;
  double ChargedFractionResponse = readPtJetPropertyHisto(pT, ChargedFraction, m_respFactorsChargedFraction[etabin]);
  return ChargedFractionResponse;
}

double GlobalSequentialCorrection::getPunchThroughResponse(double E, double eta_det, int Nsegments) const {
  int etabin=-99;
  //Check that the punch through eta binning defined in the config appears reasonable, otherwise throw an error.
  if (m_punchThroughEtaBins.size()==0 || m_respFactorsPunchThrough.size() != m_punchThroughEtaBins.size()-1) 
    ATH_MSG_WARNING("Please check that the punch through eta binning is properly set in your config file");
  if ( eta_det >= m_punchThroughEtaBins.back() || Nsegments < 20 ) return 1;
  for (uint i=0; i<m_punchThroughEtaBins.size()-1; ++i) {
    if(eta_det >= m_punchThroughEtaBins[i] && eta_det < m_punchThroughEtaBins[i+1]) etabin = i;
  }
  if(etabin<0) {
    ATH_MSG_WARNING("There was a problem determining the eta bin to use for the punch through correction.");
    //this could probably be improved, but to avoid a seg fault...
    return 1;
  }
  double PunchThroughResponse = readPtJetPropertyHisto(E,log(Nsegments),m_respFactorsPunchThrough[etabin]);
  if(!m_pTResponseRequirementOff && PunchThroughResponse>1) return 1;
  return PunchThroughResponse;
}

double GlobalSequentialCorrection::getGSCCorrection(xAOD::JetFourMom_t jetP4, double eta, 
				                    double trackWIDTH, double nTrk, double Tile0, double EM3, int Nsegments, double ChargedFraction) const {
  //eta bins have size m_binSize=0.1 and are numbered sequentially from 0, so |eta|=2.4 is in eta bin #24
  int etabin = eta/m_binSize;
  double Corr=1;
  //Using bit sequence check to determine which GS corrections to apply.
  if( !m_PFlow ){
    if (m_depth & ApplyTile0)      Corr*=1./getTile0Response(jetP4.pt()/m_GeV, etabin, Tile0);
    if (m_depth & ApplyEM3)        Corr*=1./getEM3Response(jetP4.pt()/m_GeV*Corr, etabin, EM3);
    if (m_depth & ApplynTrk)       Corr*=1./getNTrkResponse(jetP4.pt()/m_GeV*Corr, etabin, nTrk);
    if (m_depth & ApplytrackWIDTH) Corr*=1./getTrackWIDTHResponse(jetP4.pt()/m_GeV*Corr,etabin,trackWIDTH);
    if ( jetP4.pt() < m_punchThroughMinPt ) return Corr; //Applying punch through correction to low pT jets introduces a bias, default threshold is 50 GeV
    //eta binning for the punch through correction differs from the rest of the GSC, so the eta bin is determined in the GetPunchThroughResponse method
    else if (m_depth & ApplyPunchThrough) {
      jetP4*=Corr; //The punch through correction is binned in E instead of pT, so we determine E from the corrected jet here
      Corr*=1/getPunchThroughResponse(jetP4.e()/m_GeV,eta,Nsegments);
    }
  }
  else{
    if (m_depth & ApplyChargedFraction)                     Corr*=1./getChargedFractionResponse(jetP4.pt()/m_GeV, etabin, ChargedFraction);
    if (m_depth & ApplyTile0)                               Corr*=1./getTile0Response(jetP4.pt()/m_GeV*Corr, etabin, Tile0);
    if (m_depth & ApplyEM3)                                 Corr*=1./getEM3Response(jetP4.pt()/m_GeV*Corr, etabin, EM3);
    if ( m_nTrkwTrk_4PFlow && (m_depth & ApplynTrk) )       Corr*=1./getNTrkResponse(jetP4.pt()/m_GeV*Corr, etabin, nTrk);
    if ( m_nTrkwTrk_4PFlow && (m_depth & ApplytrackWIDTH) ) Corr*=1./getTrackWIDTHResponse(jetP4.pt()/m_GeV*Corr,etabin,trackWIDTH);
    if ( jetP4.pt() < m_punchThroughMinPt ) return Corr; //Applying punch through correction to low pT jets introduces a bias, default threshold is 50 GeV
    //eta binning for the punch through correction differs from the rest of the GSC, so the eta bin is determined in the GetPunchThroughResponse method
    else if (m_depth & ApplyPunchThrough) {
      jetP4*=Corr; //The punch through correction is binned in E instead of pT, so we determine E from the corrected jet here
      Corr*=1/getPunchThroughResponse(jetP4.e()/m_GeV,eta,Nsegments);
    }
  }
  return Corr;
}

StatusCode GlobalSequentialCorrection::calibrateImpl(xAOD::Jet& jet, JetEventInfo& jetEventInfo) const {

  //vector<float> that holds the fractional energy deposited by the jet in different layers of the calorimetery
  /* Map of the entries in the vector to different layers of the calorimeter
  Retrieved on July 14th 2014 from https://twiki.cern.ch/twiki/bin/view/AtlasProtected/Run2JetMoments
  If July 14th 2014 was awhile ago, it might be worth double checking this is still valid...

  Layer 	Index
  LAr barrel
  PreSamplerB	0
  EMB1 		1
  EMB2 		2
  EMB3 		3
  LAr EM endcap
  PreSamplerE 	4
  EME1 		5
  EME2 		6
  EME3 		7
  Hadronic endcap
  HEC0 		8
  HEC1 		9
  HEC2 		10
  HEC3 		11
  Tile barrel
  TileBar0 	12
  TileBar1 	13
  TileBar2 	14
  Tile gap (ITC & scint)
  TileGap1 	15
  TileGap2 	16
  TileGap3 	17
  Tile extended barrel
  TileExt0 	18
  TileExt1 	19
  TileExt2 	20
  Forward EM endcap
  FCAL0 	21
  FCAL1 	22
  FCAL2 	23
  Mini FCAL
  MINIFCAL0 	24
  MINIFCAL1 	25
  MINIFCAL2 	26
  MINIFCAL3 	27 
  */

  std::vector<float> samplingFrac = jet.getAttribute<std::vector<float> >("EnergyPerSampling");
  //vector<int> that holds the number of tracks with pT > 1 GeV for different primary vertices
  std::vector<int> nTrk;
  if(m_depth & ApplynTrk){
    if( !jet.getAttribute<std::vector<int> >("NumTrkPt1000",nTrk) ) {
      ATH_MSG_ERROR("Failed to retrieve NumTrkPt1000!");
      return StatusCode::FAILURE;
    }
  }
  //vector<float> that holds the trackWIDTH variable calculated with tracks of pT > 1 GeV for different primary vertices
  std::vector<float> trackWIDTH;
  if(m_depth & ApplytrackWIDTH){
    if( !jet.getAttribute<std::vector<float> >("TrackWidthPt1000",trackWIDTH) ) {
      ATH_MSG_ERROR("Failed to retrieve TrackWidthPt1000!");
      return StatusCode::FAILURE;
    }
  }
  //Nsegments number of ghost associated muon segments behind each jet
  int Nsegments = 0;
  if(m_depth & ApplyPunchThrough){
    if( jet.isAvailable< int >( "GhostMuonSegmentCount" ) ) {
      Nsegments = jet.getAttribute<int>("GhostMuonSegmentCount");
    } else {
      ATH_MSG_WARNING("GhostMuonSegmentCount is not available, Nsegments=0 will be used, so NO PunchThrough Correction will be applied!");
    }
  }

  xAOD::JetFourMom_t jetconstitP4 = jet.getAttribute<xAOD::JetFourMom_t>("JetConstitScaleMomentum");

  double ChargedFraction = 0;
  if( m_PFlow ) ChargedFraction = (jet.getAttribute<std::vector<float> >("SumPtChargedPFOPt500"))[0]/jetconstitP4.Pt();

  xAOD::JetFourMom_t jetStartP4;
  ATH_CHECK( setStartP4(jet) );
  jetStartP4 = jet.jetP4();

  float jetE_constitscale = jetconstitP4.e();
  float detectorEta = jet.getAttribute<float>("DetectorEta");
  //Entry 0 of the nTrk and trackWIDTH vectors should correspond to PV0
  //other entries are for other primary vertices in the event
  //Check what index the user wants just in case (99% of the time they want PV0)
  int PVindex = jetEventInfo.PVIndex();
  int nTrkPV0 = (m_depth & ApplynTrk) ? nTrk[PVindex] : 0;
  float trackWIDTHPV0 = (m_depth & ApplytrackWIDTH) ? trackWIDTH[PVindex] : 0;
  //EM3 and Tile0 fraction calculations
  //EM3 = (EMB3+EME3)/energy, Tile0 = (TileBar0+TileExt0)/energy
  //Check the map above to make sure the correct entries of samplingFrac are being used
  float EM3 = (samplingFrac[3]+samplingFrac[7])/jetE_constitscale;
  float Tile0 = (samplingFrac[12]+samplingFrac[18])/jetE_constitscale;

  xAOD::JetFourMom_t calibP4 = jetStartP4*getGSCCorrection( jetStartP4, fabs(detectorEta), trackWIDTHPV0, nTrkPV0, Tile0, EM3, Nsegments, ChargedFraction );

  //Transfer calibrated jet properties to the Jet object
  jet.setAttribute<xAOD::JetFourMom_t>("JetGSCScaleMomentum",calibP4);
  jet.setJetP4( calibP4 );

  return StatusCode::SUCCESS;

}


