/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaMVACalib/egammaMVATool.h"
#include "egammaMVACalib/egammaMVACalib.h"
#include "egammaMVACalib/egammaMVATree.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "PathResolver/PathResolver.h"
#include "CxxUtils/make_unique.h"

egammaMVATool::egammaMVATool( const std::string &name )
  : asg::AsgTool(name),
    m_mvaElectron(),
    m_mvaPhoton(),
    m_MVATreeElectron(),
    m_MVATreePhoton(),
    m_folder("")
{
  declareProperty("folder", m_folder="egammaMVACalib/offline/v3");//, "folder with weight files");
  declareProperty("use_layer_corrected", m_use_layer_corrected=true);
}

egammaMVATool::~egammaMVATool(){
}

StatusCode egammaMVATool::initialize() {
  ATH_MSG_DEBUG("In initialize of " << name() << "..." );

  ATH_MSG_DEBUG("initializing egammaMVACalib for electrons");
  m_mvaElectron = CxxUtils::make_unique<egammaMVACalib>(egammaMVACalib::egELECTRON, // particle type
							true, // use new BDT (not TMVA)
							m_folder, // folder with weight files
							"BDTG", // method
							1, // Full Calib
							false, // not debugging
							"", // that
							"",
							"",
							"", // file pattern of xml files
							true // ignore spectators
							);
  m_mvaElectron->msg().setLevel(this->msg().level());
  ATH_MSG_INFO(std::string(m_use_layer_corrected ? "U" : "Not u") + "sing layer correction");  // j4f
  {
    const std::string filename = PathResolverFindCalibFile(m_folder + "/MVACalib_electron.weights.root");
    ATH_MSG_DEBUG("configuration does not contain list of variables, try to guess:"); // TODO: because it is not implemented
    std::set<std::string> el_variables = guess_variables(filename);
    el_variables.insert({"el_cl_E", "el_cl_eta", "el_rawcl_Es0", "el_rawcl_Es1", "el_rawcl_Es2", "el_rawcl_Es3"}); // used for binning
    for (const auto var : el_variables) { ATH_MSG_DEBUG("  " << var); }
    ATH_MSG_INFO(el_variables.size() << " variables for electrons");

    m_MVATreeElectron = CxxUtils::make_unique<egammaMVATreeElectron>("MVATreeElectron", el_variables, m_use_layer_corrected);
    m_MVATreeElectron->msg().setLevel(this->msg().level());
    m_mvaElectron->InitTree(m_MVATreeElectron.get());
  }

  ATH_MSG_DEBUG("initializing egammaMVACalib for photons");
  m_mvaPhoton = CxxUtils::make_unique<egammaMVACalib>(egammaMVACalib::egPHOTON, // particle type
						      true, // use new BDT (not TMVA)
						      m_folder, // folder with weight files
						      "BDTG", // method
						      1 , // Full Calib
						      false, // not debugging
						      "", // that
						      "",
						      "",
						      "", // file pattern of xml files
						      true // ignore spectators
						      );
  m_mvaPhoton->msg().setLevel(this->msg().level());
  {
    const std::string filename_unconv = PathResolverFindCalibFile(m_folder + "/MVACalib_unconvertedPhoton.weights.root");
    const std::string filename_conv = PathResolverFindCalibFile(m_folder + "/MVACalib_convertedPhoton.weights.root");
    ATH_MSG_INFO("configuration does not contain list of variables, try to guess:"); // TODO: because it is not implemented
    const std::set<std::string> ph_unconv_variables = guess_variables(filename_unconv);
    const std::set<std::string> ph_conv_variables = guess_variables(filename_conv);
    std::set<std::string> ph_variables = ph_unconv_variables;
    ph_variables.insert(ph_conv_variables.begin(), ph_conv_variables.end());
    ph_variables.insert({"ph_cl_E", "ph_cl_eta", "ph_rawcl_Es0", "ph_rawcl_Es1", "ph_rawcl_Es2", "ph_rawcl_Es3"}); // used for binning
    ATH_MSG_INFO(ph_variables.size() << " variables for photons");
    for (const auto var : ph_variables) { ATH_MSG_INFO("  " << var); }

    m_MVATreePhoton = CxxUtils::make_unique<egammaMVATreePhoton>("MVATreePhoton", ph_variables, m_use_layer_corrected, true);
    m_MVATreePhoton->msg().setLevel(this->msg().level());
    m_mvaPhoton->InitTree(m_MVATreePhoton.get());
  }

  return StatusCode::SUCCESS;
}

std::set<std::string> egammaMVATool::guess_variables(const std::string& filename)
{
  TFile f(filename.c_str());
  std::unique_ptr<TObjArray> formulae(dynamic_cast<TObjArray*>(f.Get("formulae")));
  formulae->SetOwner(true);  // by default TObjArray doesn't own elements
  if (not formulae) { ATH_MSG_FATAL("cannot find formulae in " << filename); }

  // TODO: use regex parsing (regex supported only in gcc 4.9, TPRegexp sucks)
  const std::vector<std::string> all_possible_variables = {
    "el_cl_E", "el_cl_eta", "el_cl_phi", "el_charge", "el_author",
    "el_rawcl_Es0", "el_rawcl_Es1", "el_rawcl_Es2", "el_rawcl_Es3", "el_cl_E_TileGap3",
    "el_cl_etaCalo", "el_cl_phiCalo", "el_rawcl_calibHitsShowerDepth", "el_wtots1",
    "ph_cl_E", "ph_cl_eta", "ph_cl_phi", "ph_author",
    "ph_rawcl_Es0", "ph_rawcl_Es1", "ph_rawcl_Es2", "ph_rawcl_Es3", "ph_cl_E_TileGap3", "ph_wtots1",
    "ph_cl_etaCalo", "ph_cl_phiCalo", "ph_rawcl_calibHitsShowerDepth",
    "el_charge", "el_tracketa", "el_trackpt", "el_trackz0", "el_refittedTrack_qoverp", "el_author",
    "ph_Rconv", "ph_zconv", "ph_pt1conv", "ph_pt2conv", "ph_ptconv", "ph_convtrk1nPixHits", "ph_convtrk2nPixHits", "ph_convtrk1nSCTHits", "ph_convtrk2nSCTHits",
  };

  std::set<std::string> variables_found;
  TIter iter(formulae.get());
  while (TNamed* obj = dynamic_cast<TNamed*>(iter())) {
    const std::string expression = obj->GetTitle();
    ATH_MSG_DEBUG("searching variables in " << expression);
    for (const auto var : all_possible_variables) {
      const auto pos = expression.find(var);
      if (pos != std::string::npos) {
        if (pos + var.size() < expression.size()) {
          const char next_char = expression[pos + var.size()];
          if (isalnum(next_char) or next_char == '_') continue;
        }
        ATH_MSG_DEBUG("found variable '" << var << "' in '" << expression << "'");
        variables_found.insert(var);
      }
    }
  }
  return variables_found;
}

StatusCode egammaMVATool::finalize(){
  ATH_MSG_DEBUG( "in finalize" );
  return StatusCode::SUCCESS;
}

StatusCode egammaMVATool::execute(xAOD::CaloCluster* cluster,const xAOD::Egamma* eg){
  if (!eg || !cluster) {
    ATH_MSG_ERROR("Invalid Pointer to egamma or cluster object");
    return StatusCode::FAILURE;
  }
  double mvaE = getEnergy(cluster, eg);
  ATH_MSG_DEBUG( "Calculated MVA calibrated energy = " << mvaE );
  if (mvaE > eg->m()) {
    cluster->setCalE(mvaE);
  }
  else {
    ATH_MSG_DEBUG("MVA energy (" << mvaE << ") < particle mass ("
                  << eg->m() << "), setting e = cluster energy (" << cluster->e() << ")");
    cluster->setCalE(cluster->e());
  }
  return StatusCode::SUCCESS;
}

StatusCode egammaMVATool::execute(xAOD::CaloCluster* cluster, const xAOD::EgammaParameters::EgammaType egType){
  if (!cluster) {
    ATH_MSG_ERROR("Invalid Pointer to egamma or cluster object");
    return StatusCode::FAILURE;
  }
  double mvaE = getEnergy(cluster, egType);
  ATH_MSG_DEBUG( "Calculated MVA calibrated energy = " << mvaE );
  if (mvaE > 0) {
    cluster->setCalE(mvaE);
  }
  else {
    cluster->setCalE(cluster->e());
  }
  return StatusCode::SUCCESS;
}

StatusCode egammaMVATool::hltexecute(xAOD::CaloCluster* cluster, const std::string& egType) {
  if(!cluster){
    ATH_MSG_ERROR("Invalid Pointer to cluster object");
    return StatusCode::FAILURE;
  }
  double mvaE = getEnergy(cluster, egType);
  ATH_MSG_DEBUG( "Calculated MVA calibrated energy = " << mvaE );
  if(mvaE > 0){
    cluster->setCalE(mvaE);
  }
  else{
    cluster->setCalE(cluster->e());
  }
  return StatusCode::SUCCESS;
}

float egammaMVATool::getEnergy(const xAOD::Egamma* eg){
  if (!eg){
    ATH_MSG_WARNING("no xAOD::Egamma object provided");
    return 0;
  }
  return getEnergy(eg->caloCluster(),eg);
}

float egammaMVATool::getEnergy(const xAOD::CaloCluster* cluster, 
			       const xAOD::Egamma* eg){
  ATH_MSG_DEBUG("In execute...");
  // Check for errors...
  if ( !eg ){
    ATH_MSG_WARNING("no xAOD::Egamma object provided");
    return 0;
  }
  if( eg->type() == xAOD::Type::Electron  ){
    ATH_MSG_DEBUG("Processing for electron");
    return getEnergy(cluster, static_cast<const xAOD::Electron*>(eg));
  }
  else if (eg->type() == xAOD::Type::Photon ){
    ATH_MSG_DEBUG("Processing for photon");
    // this is because topo seeded electron (author == 128) have cluster in
    // another collection, which is not decorated with etaCalo, m_cl_phiCalo
    // needed by the MVA calibration
    // TODO: make more general: no input -> no MVA
    if (eg->author() == 128) { return cluster->e(); }

    return getEnergy(cluster, static_cast<const xAOD::Photon*>(eg));
  }
  else{
    ATH_MSG_INFO("Unknown Type");
  }
  return 0;
}


float egammaMVATool::getEnergy(const xAOD::CaloCluster* cluster, 
			       const std::string& egType){
  // Check for errors...
  if ( !cluster ){
    ATH_MSG_WARNING("no xAOD::CaloCluster object provided");
    return 0;
  }
  if (egType == "Electron") {
    ATH_MSG_DEBUG("Processing  for type electron");
    m_MVATreeElectron->update(nullptr, cluster);
    return m_mvaElectron->getMVAEnergy();
  }
  else if(egType == "Photon"){
    ATH_MSG_DEBUG("Processing for type photon");
    m_MVATreePhoton->update(nullptr, cluster);
    return m_mvaPhoton->getMVAEnergy();
  }
  else {
    ATH_MSG_WARNING("Unknown particle type");
  }
  return 0;
}

float egammaMVATool::getEnergy(const xAOD::CaloCluster* cluster, 
			       const xAOD::EgammaParameters::EgammaType egType){
  return  ( (egType==xAOD::EgammaParameters::electron) ?  
	   getEnergy(cluster, "Electron") : 
	   getEnergy(cluster, "Photon"));
}


float egammaMVATool::getEnergy(const xAOD::CaloCluster* cluster, 
			       const xAOD::Electron* el){
  if(!el){
    ATH_MSG_ERROR("No electron passed");
    return 0;
  }
  ATH_MSG_DEBUG("updating variables electron");
  m_MVATreeElectron->update(el, cluster);
  return m_mvaElectron->getMVAEnergy();
}

float egammaMVATool::getEnergy(const xAOD::CaloCluster* cluster, 
			       const xAOD::Photon* ph){
  if(!ph){
    ATH_MSG_ERROR("No photon passed");
    return 0;
  }
  ATH_MSG_DEBUG("updating variables photon");
  m_MVATreePhoton->update(ph, cluster);
  return m_mvaPhoton->getMVAEnergy();
}

