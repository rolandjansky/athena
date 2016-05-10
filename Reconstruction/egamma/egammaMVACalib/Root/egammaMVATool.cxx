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

egammaMVATool::egammaMVATool( const std::string &name )
  : asg::AsgTool(name),
    m_mvaElectron(0),
    m_mvaPhoton(0),
    m_MVATreeElectron(nullptr),
    m_MVATreePhoton(nullptr),
    m_folder(""),
    m_rawcl_Es0(0),
    m_rawcl_Es1(0),
    m_rawcl_Es2(0),
    m_rawcl_Es3(0),
    m_cl_eta(0),
    m_cl_E(0),
    m_ptconv(0),
    m_pt1conv(0),
    m_pt2conv(0),
    m_cl_etaCalo(0),
    m_cl_phiCalo(0),
    m_convtrk1nPixHits(0),
    m_convtrk1nSCTHits(0),
    m_convtrk2nPixHits(0),
    m_convtrk2nSCTHits(0),
    m_Rconv(0)
{
  declareProperty("folder", m_folder="egammaMVACalib/offline/v3");//, "folder with weight files");
  declareProperty("new_version", m_new_version=true);      // this will become the default
  declareProperty("use_layer_corrected", m_use_layer_corrected=true);
}

StatusCode egammaMVATool::initialize() {
  ATH_MSG_DEBUG("In initialize of " << name() << "..." );

  ATH_MSG_DEBUG("initializing egammaMVACalib for electrons");
  m_mvaElectron = new egammaMVACalib(egammaMVACalib::egELECTRON, // particle type
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
  if (m_new_version) {
    const std::string filename = PathResolverFindCalibFile(m_folder + "/MVACalib_electron.weights.root");
    ATH_MSG_INFO("configuration does not contain list of variables, try to guess:"); // TODO: because it is not implemented
    std::set<std::string> el_variables = guess_variables(filename);
    el_variables.insert({"el_cl_E", "el_cl_eta", "el_rawcl_Es0", "el_rawcl_Es1", "el_rawcl_Es2", "el_rawcl_Es3"}); // used for binning
    for (const auto var : el_variables) { ATH_MSG_INFO("  " << var); }
    m_MVATreeElectron = new egammaMVATreeElectron("MVATreeElectron", el_variables, m_use_layer_corrected);
    m_MVATreeElectron->msg().setLevel(this->msg().level());
    m_mvaElectron->InitTree(m_MVATreeElectron);
  }
  else {
    ATH_MSG_INFO("not new");
    m_mvaElectron->InitTree(0);
  }

  ATH_MSG_DEBUG("initializing egammaMVACalib for photons");
  m_mvaPhoton = new egammaMVACalib(egammaMVACalib::egPHOTON, // particle type
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
  if (m_new_version) {
    const std::string filename_unconv = PathResolverFindCalibFile(m_folder + "/MVACalib_unconvertedPhoton.weights.root");
    const std::string filename_conv = PathResolverFindCalibFile(m_folder + "/MVACalib_convertedPhoton.weights.root");
    ATH_MSG_INFO("configuration does not contain list of variables, try to guess:"); // TODO: because it is not implemented
    const std::set<std::string> ph_unconv_variables = guess_variables(filename_unconv);
    const std::set<std::string> ph_conv_variables = guess_variables(filename_conv);
    std::set<std::string> ph_variables = ph_unconv_variables;
    ph_variables.insert(ph_conv_variables.begin(), ph_conv_variables.end());
    ph_variables.insert({"ph_cl_E", "ph_cl_eta", "ph_rawcl_Es0", "ph_rawcl_Es1", "ph_rawcl_Es2", "ph_rawcl_Es3"}); // used for binning
    for (const auto var : ph_variables) { ATH_MSG_INFO("  " << var); }
    m_MVATreePhoton = new egammaMVATreePhoton("MVATreePhoton", ph_variables, m_use_layer_corrected, true);
    m_MVATreePhoton->msg().setLevel(this->msg().level());
    m_mvaPhoton->InitTree(m_MVATreePhoton);
  }
  else {
    m_mvaPhoton->InitTree(0);
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
    "el_cl_etaCalo", "el_cl_phiCalo", "el_rawcl_calibHitsShowerDepth",
    "ph_cl_E", "ph_cl_eta", "ph_cl_phi", "ph_author",
    "ph_rawcl_Es0", "ph_rawcl_Es1", "ph_rawcl_Es2", "ph_rawcl_Es3",
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
  delete m_mvaElectron;
  delete m_mvaPhoton;
  delete m_MVATreePhoton;
  delete m_MVATreeElectron;
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

float egammaMVATool::getEnergy(const xAOD::CaloCluster* cluster, const std::string& egType){

  // Check for errors...
  if ( !cluster ){
    ATH_MSG_WARNING("no xAOD::CaloCluster object provided");
    return 0;
  }

  if (m_new_version) {
    if (egType == "Electron") {
      ATH_MSG_DEBUG("Processing for HLT electron");
      m_MVATreeElectron->update(nullptr, cluster);
      return m_mvaElectron->getMVAEnergy();
    }
    else if(egType == "Photon"){
      ATH_MSG_DEBUG("Processing for HLT photon");
      m_MVATreePhoton->update(nullptr, cluster);
      return m_mvaPhoton->getMVAEnergy();
    }
    else {
      ATH_MSG_INFO("Unknown Type");
    }
    return 0;
  }


  getClusterVariables(cluster);

  if(egType == "Electron"){
      ATH_MSG_DEBUG("Processing for electron...");
      return m_mvaElectron->getMVAEnergyElectron(m_rawcl_Es0,
                                                 m_rawcl_Es1,
                                                 m_rawcl_Es2,
                                                 m_rawcl_Es3,
                                                 m_cl_eta,
                                                 m_cl_E,
                                                 m_cl_etaCalo,
                                                 m_cl_phiCalo);
  }
  else if(egType == "Photon"){
    ATH_MSG_DEBUG("Processing for photon...");
    m_ptconv = 0;
    m_pt1conv = 0;
    m_pt2conv = 0;
    m_convtrk1nPixHits = 0;
    m_convtrk1nSCTHits = 0;
    m_convtrk2nPixHits = 0;
    m_convtrk2nSCTHits = 0;
    m_Rconv = 0;
    return m_mvaPhoton->getMVAEnergyPhoton(m_rawcl_Es0,
                                           m_rawcl_Es1,
                                           m_rawcl_Es2,
                                           m_rawcl_Es3,
                                           m_cl_eta,
                                           m_cl_E,
                                           m_cl_etaCalo,
                                           m_cl_phiCalo,
                                           m_ptconv,
                                           m_pt1conv,
                                           m_pt2conv,
                                           m_convtrk1nPixHits,
                                           m_convtrk1nSCTHits,
                                           m_convtrk2nPixHits,
                                           m_convtrk2nSCTHits,
                                           m_Rconv
                                         );
  }
  else{
    ATH_MSG_INFO("Unknown Type");
  }

  return 0;
}

float egammaMVATool::getEnergy(const xAOD::CaloCluster* cluster, const xAOD::Egamma* eg){
  ATH_MSG_DEBUG("In execute...");

  // Check for errors...
  if ( !eg ){
    ATH_MSG_WARNING("no xAOD::Egamma object provided");
    return 0;
  }

  if( eg->type() == xAOD::Type::Electron  ){
    ATH_MSG_DEBUG("Processing for electron...");
    return getEnergy(cluster, static_cast<const xAOD::Electron*>(eg));
  }
  else if (eg->type() == xAOD::Type::Photon ){
    ATH_MSG_DEBUG("Processing for photon...");

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

float egammaMVATool::getEnergy(const xAOD::CaloCluster* cluster, const xAOD::Electron* el){
  if(!el){
    ATH_MSG_ERROR("No electron passed");
    return 0;
  }

  if (m_new_version) {
    ATH_MSG_DEBUG("updating variables electron");
    m_MVATreeElectron->update(el, cluster);
    return m_mvaElectron->getMVAEnergy();
  }
  else {
    getClusterVariables(cluster);

    return m_mvaElectron->getMVAEnergyElectron(m_rawcl_Es0,
      m_rawcl_Es1,
      m_rawcl_Es2,
      m_rawcl_Es3,
      m_cl_eta,
      m_cl_E,
      m_cl_etaCalo,
      m_cl_phiCalo);
    }
}

float egammaMVATool::getEnergy(const xAOD::CaloCluster* cluster, const xAOD::Photon* ph){
  if(!ph){
    ATH_MSG_ERROR("No photon passed");
    return 0;
  }

  if (m_new_version) {
    ATH_MSG_DEBUG("updating variables photon");
    m_MVATreePhoton->update(ph, cluster);
    return m_mvaPhoton->getMVAEnergy();
  }
  else {
    if( !getClusterVariables(cluster)){
      ATH_MSG_ERROR("Could not retrieve cluster variables.");
      return 0;
    }

    const xAOD::Vertex* phVertex = ph->vertex();
    if(!phVertex){
      m_ptconv = 0;
      m_pt1conv = 0;
      m_pt2conv = 0;
      m_convtrk1nPixHits = 0;
      m_convtrk1nSCTHits = 0;
      m_convtrk2nPixHits = 0;
      m_convtrk2nSCTHits = 0;
      m_Rconv = 0;
    } else{
      getConversionVariables(phVertex);
    }

    return m_mvaPhoton->getMVAEnergyPhoton(m_rawcl_Es0,
      m_rawcl_Es1,
      m_rawcl_Es2,
      m_rawcl_Es3,
      m_cl_eta,
      m_cl_E,
      m_cl_etaCalo,
      m_cl_phiCalo,
      m_ptconv,
      m_pt1conv,
      m_pt2conv,
      m_convtrk1nPixHits,
      m_convtrk1nSCTHits,
      m_convtrk2nPixHits,
      m_convtrk2nSCTHits,
      m_Rconv
    );
  }
}

bool egammaMVATool::getClusterVariables(const xAOD::CaloCluster* cluster){

  // The cluster energy in the sampling layers
  m_rawcl_Es0 = cluster->energyBE(0);
  m_rawcl_Es1 = cluster->energyBE(1);
  m_rawcl_Es2 = cluster->energyBE(2);
  m_rawcl_Es3 = cluster->energyBE(3);

  m_cl_eta = cluster->eta();//el_cl_eta;
  m_cl_E = cluster->e(); //el_cl_E;

  // Correct this with the right variables
  m_cl_etaCalo = 0.;
  if (cluster->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME, m_cl_etaCalo)) { }
  else if (cluster->isAvailable<float>("etaCalo")) {
    m_cl_etaCalo = cluster->auxdata<float>("etaCalo");
  }
  else {
    ATH_MSG_ERROR("etaCalo not found in CaloCluster object");
    return false;
  }

  m_cl_phiCalo = 0.;
  if (cluster->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME, m_cl_phiCalo)) { }
  else if (cluster->isAvailable<float>("phiCalo")) {
    m_cl_phiCalo = cluster->auxdata<float>("phiCalo");
  }
  else {
    ATH_MSG_ERROR("phiCalo not found in CaloCluster object");
    return false;
  }
  return true;
}

bool egammaMVATool::getConversionVariables(const xAOD::Vertex *phVertex){
  const Amg::Vector3D pos = phVertex->position();
  m_Rconv = static_cast<float>(hypot (pos.x(), pos.y()));

  const xAOD::TrackParticle* tp0 = phVertex->trackParticle(0);
  const xAOD::TrackParticle* tp1 = phVertex->trackParticle(1);

  static SG::AuxElement::Accessor<float> accPt1("pt1");
  static SG::AuxElement::Accessor<float> accPt2("pt2");
  if (accPt1.isAvailable(*phVertex) && accPt2.isAvailable(*phVertex) )
  {
    m_pt1conv = accPt1(*phVertex);
    m_pt2conv = accPt2(*phVertex);
  }
  else
  {
    ATH_MSG_WARNING("pt1/pt2 not available, will approximate from first measurements");
    m_pt1conv = getPtAtFirstMeasurement( tp0 );
    m_pt2conv = getPtAtFirstMeasurement( tp1 );
  }

  if(tp0){
    uint8_t hits;
    tp0->summaryValue(hits,xAOD::numberOfPixelHits);
    m_convtrk1nPixHits = hits;
    tp0->summaryValue(hits,xAOD::numberOfSCTHits);
    m_convtrk1nSCTHits = hits;
  }
  else { m_convtrk1nPixHits = m_convtrk1nSCTHits = 0; }

  if(tp1){
    uint8_t hits;
    tp1->summaryValue(hits,xAOD::numberOfPixelHits);
    m_convtrk2nPixHits = hits;
    tp1->summaryValue(hits,xAOD::numberOfSCTHits);
    m_convtrk2nSCTHits = hits;
  }
  else { m_convtrk2nPixHits = m_convtrk2nSCTHits = 0; }

  m_ptconv = xAOD::EgammaHelpers::momentumAtVertex(*phVertex).perp();
  return true;
}

float egammaMVATool::getPtAtFirstMeasurement(const xAOD::TrackParticle* tp) const
{
  if (!tp) return 0;
  for (unsigned int i = 0; i < tp->numberOfParameters(); ++i)
    if (tp->parameterPosition(i) == xAOD::FirstMeasurement)
      return hypot(tp->parameterPX(i), tp->parameterPY(i));

  ATH_MSG_WARNING("Could not find first parameter, return pt at perigee");
  return tp->pt();
}
