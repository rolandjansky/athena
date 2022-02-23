/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/* ***********************************************************************************\
 *                                                                                   *
 *      Name: GlobalNNCalibration                                           *
 *      Purpose: Perform the NN GSC step of the calibration                          *
 *                                                                                   *
 *  #   Date    Comments                   By                                        *
 * -- -------- -------------------------- ------------------------------------------ *
 *  1 25/06/20  First Version              I. Aizenberg                              * 
 *  1 25/06/20  Updates to work with JetCalibTools   J. Roloff     * 
\*************************************************************************************/

// System includes
#include <fstream> 
#include "PathResolver/PathResolver.h"
#include "JetCalibTools/CalibrationMethods/GlobalNNCalibration.h"
#include "JetCalibTools/JetCalibrationToolBase.h"

GlobalNNCalibration::GlobalNNCalibration()
    : JetCalibrationToolBase::JetCalibrationToolBase("GlobalNNCalibration/GlobalNNCalibration"),
m_config(nullptr), m_jetAlgo(""), m_calibAreaTag(""), m_dev(false), m_doSplineCorr(true)
{
}


// Constructor
GlobalNNCalibration::GlobalNNCalibration(const std::string& name)
    : JetCalibrationToolBase::JetCalibrationToolBase(name),
m_config(nullptr), m_jetAlgo(""), m_calibAreaTag(""), m_dev(false), m_doSplineCorr(true)
{
}

GlobalNNCalibration::GlobalNNCalibration(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool dev)
  : JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(config), m_jetAlgo(jetAlgo), m_calibAreaTag(calibAreaTag), m_dev(dev), m_doSplineCorr(true)
{

}

// Destructor
GlobalNNCalibration::~GlobalNNCalibration()
{ }

// Initialize
StatusCode GlobalNNCalibration::initializeTool(const std::string&){

  ATH_MSG_INFO("Initializing tool");
  m_nnEtaBins = JetCalibUtils::VectorizeD( m_config->GetValue("GNNC.EtaBinsForNN","") );
  m_closureEtaBins = JetCalibUtils::VectorizeD( m_config->GetValue("GNNC.EtaBinsForClosure","") );
  m_NNInputs = JetCalibUtils::Vectorize( m_config->GetValue("GNNC.Inputs","") );
  m_doSplineCorr = m_config->GetValue("GNNC.UseSplineCorr", true);

  // This code is copied from the GlobalSequentialCorrection code,
  // so while it is hard-coded, it is consistent with how these filenames are parsed.
  if ( !m_config ) { ATH_MSG_FATAL("Config file not specified. Aborting."); return StatusCode::FAILURE; }
  if ( m_jetAlgo.EqualTo("") ) { ATH_MSG_FATAL("No jet algorithm specified. Aborting."); return StatusCode::FAILURE; }

  //find the ROOT file containing response histograms, path comes from the config file.
  TString MLGSCFile = m_config->GetValue("GNNC.MLFactorsFile","empty");
  if ( MLGSCFile.EqualTo("empty") ) {
    ATH_MSG_FATAL("NO MLFactorsFile specified. Aborting.");
    return StatusCode::FAILURE;
  }
  if(m_dev){
    MLGSCFile.Remove(0,33);
    MLGSCFile.Insert(0,"JetCalibTools/");
  }
  else{MLGSCFile.Insert(14,m_calibAreaTag);}


  // Get all of the different NN json files for each eta bin
  for(unsigned int i=0; i<m_nnEtaBins.size()-1; i++){
    TString fileName = PathResolverFindCalibFile(Form("%s_etabin_%d.json", MLGSCFile.Data(), i));
    std::ifstream input(fileName);
    std::unique_ptr<lwt::LightweightGraph> lwnn = std::unique_ptr<lwt::LightweightGraph> (new lwt::LightweightGraph( lwt::parse_json_graph(input) ) );
    m_lwnns.push_back(std::move(lwnn));
  }



  // An additional correction on top of the neural network to smooth out fluctuations in the pt response
  if(m_doSplineCorr){
    TString ptCalibHists = m_config->GetValue("GNNC.JPtS_CalibHists","");
    if(m_dev){
      ptCalibHists.Remove(0,33);
      ptCalibHists.Insert(0,"JetCalibTools/");
    }
    else{
      ptCalibHists.Insert(14,m_calibAreaTag);
    }
    TString calibHistFile = PathResolverFindCalibFile(ptCalibHists.Data());
    loadSplineHists(calibHistFile, "etaJes");
    m_JPtS_MinPt_Pt = JetCalibUtils::VectorizeD( m_config->GetValue("GNNC.ptCutoff","") );
    if(m_JPtS_MinPt_Pt.size() != m_closureEtaBins.size()-1){
      ATH_MSG_FATAL("Pt cutoff vector has wrong length. There should be one value per eta bin."); return StatusCode::FAILURE; 
      return StatusCode::FAILURE;
    }

    for (uint ieta=0;ieta<m_closureEtaBins.size()-1;++ieta) {
      //Calculate the slope of the response curve at the minPt for each eta bin
      //Used in the GetLowPtJPtS method when Pt < minPt
      const double Rcutoff = getSplineCorr(ieta, m_JPtS_MinPt_Pt[ieta]);
      const double Slope = getSplineSlope(ieta, m_JPtS_MinPt_Pt[ieta]);
      if(Slope > Rcutoff/m_JPtS_MinPt_Pt[ieta]) ATH_MSG_WARNING("Slope of calibration curve at minimum ET is too steep for the JPtS factors of etabin " << ieta << ", eta = " << m_closureEtaBins[ieta] );

      m_JPtS_MinPt_R.push_back(Rcutoff);
      m_JPtS_MinPt_Slopes.push_back(Slope);
    }
  }


  return StatusCode::SUCCESS;
}



StatusCode GlobalNNCalibration::calibrateImpl(xAOD::Jet& jet, JetEventInfo& jetEventInfo) const {
  xAOD::JetFourMom_t jetStartP4;
  jetStartP4 = jet.jetP4();

  // The NN learns the jet response, so the original jet pT is divided 
  // by the NN output to get the calibrated pT
  int nnEtaBin = getEtaBin(jet, m_nnEtaBins);
  int closureEtaBin = getEtaBin(jet, m_closureEtaBins);
  std::map<std::string,double> NN_inputValues = getJetFeatures(jet, jetEventInfo);
  std::map<std::string,std::map<std::string,double>> inputs;
  inputs["node_0"] = NN_inputValues;

  std::map<std::string, double> outputs = m_lwnns[nnEtaBin]->compute(inputs);
  double nnCalibFactor =  outputs["out_0"];
  
  double response = nnCalibFactor;
  if(m_doSplineCorr){
    double jetPt = getJESPt(jet);
    response *= getSplineCorr(closureEtaBin, jetPt/nnCalibFactor);
  }

  xAOD::JetFourMom_t calibP4 = jetStartP4 / response;

  //Transfer calibrated jet properties to the Jet object
  jet.setAttribute<xAOD::JetFourMom_t>("JetGNNCScaleMomentum",calibP4);
  jet.setJetP4( calibP4 );

  return StatusCode::SUCCESS;

}




/// Loads the calib constants from histograms in TFile named fileName. 
void GlobalNNCalibration::loadSplineHists(const TString & fileName, const std::string &ptCorr_name) 
{
  std::unique_ptr<TFile> tmpF(TFile::Open( fileName ));
  TList *ptCorr_l = dynamic_cast<TList*>( tmpF->Get(ptCorr_name.c_str()));

  m_ptCorrFactors.resize( ptCorr_l->GetSize() );
  int nBinsCorr = ptCorr_l->GetSize();
  int nEtaBins = m_closureEtaBins.size()-1;
  if(nBinsCorr != nEtaBins){
    ATH_MSG_WARNING("Do not have the correct number of eta bins for " << fileName << "\t" << ptCorr_name << "\t" << ptCorr_l->GetSize() );
  }

  for(unsigned int i=0 ; i<m_closureEtaBins.size()-1; i++){
    m_ptCorrFactors[i].reset(dynamic_cast<TH1*>(ptCorr_l->At(i)));
    m_ptCorrFactors[i]->SetDirectory(nullptr);
  }
  tmpF->Close();
}



double GlobalNNCalibration::getSplineCorr(const int etaBin, double pT) const {
  if(pT >=  m_ptCorrFactors[ etaBin ]->GetBinLowEdge( m_ptCorrFactors[ etaBin ]->GetNbinsX()+1)){
     pT =  m_ptCorrFactors[ etaBin ]->GetBinLowEdge( m_ptCorrFactors[ etaBin ]->GetNbinsX());
  }
  if(pT < m_JPtS_MinPt_Pt[etaBin]){
    double ptCutoff = m_JPtS_MinPt_Pt[etaBin];
    double Rcutoff = m_JPtS_MinPt_R[etaBin];
    double slope = m_JPtS_MinPt_Slopes[etaBin];
    double R = slope*(pT-ptCutoff)+Rcutoff;
    return R;
  }
  

  double R = m_ptCorrFactors[ etaBin ]->Interpolate(pT);
  return R;
}


double GlobalNNCalibration::getSplineSlope(const int ieta, const double minPt) const {
  // Don't want to use interpolation here, so instead just use the values at the bin centers near the cutoff
  int minBin = m_ptCorrFactors[ieta]->FindBin(minPt);

  double rFirst = m_ptCorrFactors[ ieta ]->GetBinContent(minBin);
  double rSecond = m_ptCorrFactors[ ieta ]->GetBinContent(minBin+1);
  double binWidth = m_ptCorrFactors[ ieta ]->GetBinCenter(minBin+1) - m_ptCorrFactors[ ieta ]->GetBinCenter(minBin);
  double slope = (rSecond - rFirst) / binWidth;

  return slope;
}




std::map<std::string,double> GlobalNNCalibration::getJetFeatures(const xAOD::Jet& jet_reco, JetEventInfo& jetEventInfo) const {
  std::vector<float> samplingFrac = jet_reco.getAttribute<std::vector<float> >("EnergyPerSampling");
  xAOD::JetFourMom_t jetconstitP4 = jet_reco.getAttribute<xAOD::JetFourMom_t>("JetConstitScaleMomentum");

  xAOD::JetFourMom_t jetStartP4;
  jetStartP4 = jet_reco.jetP4();

  float jetE_constitscale = jetconstitP4.e();

  //EM3 and Tile0 fraction calculations
  //EM3 = (EMB3+EME3)/energy, Tile0 = (TileBar0+TileExt0)/energy
  //Check the map above to make sure the correct entries of samplingFrac are being used
  float EM0 = (samplingFrac[0]+samplingFrac[4])/jetE_constitscale;
  float EM1 = (samplingFrac[1]+samplingFrac[5])/jetE_constitscale;
  float EM2 = (samplingFrac[2]+samplingFrac[6])/jetE_constitscale;
  float EM3 = (samplingFrac[3]+samplingFrac[7])/jetE_constitscale;
  float Tile0 = (samplingFrac[12]+samplingFrac[18])/jetE_constitscale;
  float Tile1 = (samplingFrac[13]+samplingFrac[19])/jetE_constitscale;
  float Tile2 = (samplingFrac[14]+samplingFrac[20])/jetE_constitscale;
  float HEC0 = (samplingFrac[8])/jetE_constitscale;
  float HEC1 = (samplingFrac[9])/jetE_constitscale;
  float HEC2 = (samplingFrac[10])/jetE_constitscale;
  float HEC3 = (samplingFrac[11])/jetE_constitscale;

  float FCAL0 = (samplingFrac[21])/jetE_constitscale;
  float FCAL1 = (samplingFrac[22])/jetE_constitscale;
  float FCAL2 = (samplingFrac[23])/jetE_constitscale;


  // A map of possible NN inputs with their values for this jet. 
  // These may not all be included in the NN.
  std::map<std::string,double> inputValues;

  // This is a list of variables that could be included in the NN.
  // This makes it simple to change the variables in the NN without complicated changes
  // to the code.
  inputValues["jet_JESPt"] = getJESPt(jet_reco);
  inputValues["EM0"] = EM0;
  inputValues["EM1"] = EM1;
  inputValues["EM2"] = EM2;
  inputValues["EM3"] = EM3;
  inputValues["TILE0"] = Tile0;
  inputValues["TILE1"] = Tile1;
  inputValues["TILE2"] = Tile2;
  inputValues["HEC0"] = HEC0;
  inputValues["HEC1"] = HEC1;
  inputValues["HEC2"] = HEC2;
  inputValues["HEC3"] = HEC3;
  inputValues["FCAL0"] = FCAL0;
  inputValues["FCAL1"] = FCAL1;
  inputValues["FCAL2"] = FCAL2;
  inputValues["jet_Ntrk1000"] = getJetNtrk1000(jet_reco);
  inputValues["jet_ChargedFraction"] = getJetChargedFraction(jet_reco);
  inputValues["jet_Wtrk1000"] = getJetWtrk1000(jet_reco);
  inputValues["jet_DetEta"] = getJetDetEta(jet_reco);
  inputValues["jet_n90Constituents"] = jet_reco.getAttribute<float>("N90Constituents");
  inputValues["jet_nMuSeg"] = jet_reco.getAttribute<int>("GhostMuonSegmentCount");
  inputValues["NPV"] = jetEventInfo.NPV();
  inputValues["averageInteractionsPerCrossing"] = jetEventInfo.mu();

  // The actual NN inputs and values
	std::map<std::string,double> NNInputValues;
  for(unsigned int i=0; i<m_NNInputs.size(); i++){
    NNInputValues[m_NNInputs[i].Data()] = inputValues[m_NNInputs[i].Data()];
  }

	return NNInputValues;
}



// This needs to be fixed, but just getting a placeholder
int GlobalNNCalibration::getEtaBin(const xAOD::Jet& jet_reco, const std::vector<double> etaBins) const{
  double detEta = jet_reco.getAttribute<float>("DetectorEta");
  for(unsigned int i=1; i<etaBins.size()-1; i++){
    if(abs(detEta) < etaBins[i]) return i-1;
  }
  // This should throw an error instead probably, since this is outside of the eta range we are calibrating
  return etaBins.size()-2;
}


double GlobalNNCalibration::getJetChargedFraction(const xAOD::Jet& jet_reco) const{
  if( jet_reco.isAvailable<std::vector<float> >("SumPtChargedPFOPt500") ){
    float thisChargedFraction =  jet_reco.getAttribute<std::vector<float> >("SumPtChargedPFOPt500").at(0);
    thisChargedFraction /= jet_reco.jetP4(xAOD::JetConstitScaleMomentum).Pt();
    return double(thisChargedFraction);
  }

  return -999.;
}

double GlobalNNCalibration::getJetDetEta(const xAOD::Jet& jet_reco) const {
  if(jet_reco.isAvailable<float>("DetectorEta")) return double(jet_reco.getAttribute<float>("DetectorEta"));
  return -999.;
}

int GlobalNNCalibration::getJetNtrk1000(const xAOD::Jet& jet_reco) const {
  if(jet_reco.isAvailable<std::vector<int> >("NumTrkPt1000")) return jet_reco.getAttribute<std::vector<int> >("NumTrkPt1000").at(0);
  return -999;
}

double GlobalNNCalibration::getJetWtrk1000(const xAOD::Jet& jet_reco) const {
  if(jet_reco.isAvailable<std::vector<float> >("TrackWidthPt1000")) return double(jet_reco.getAttribute<std::vector<float> >("TrackWidthPt1000").at(0));
  return -999.;
}


double GlobalNNCalibration::getJESPt(const xAOD::Jet& jet_reco) const {
  return jet_reco.jetP4("JetEtaJESScaleMomentum").pt() / 1.e3;
}






