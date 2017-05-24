/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>

#include <set>
// EDM include(s):

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODEventInfo/EventInfo.h"

#include <xAODTruth/TruthParticle.h>  // TODO: this introduces an additional dependecy which is not needed by the tool...
#include <xAODTruth/xAODTruthHelpers.h>
#include <xAODTruth/TruthVertex.h>
#include <xAODEgamma/PhotonxAODHelpers.h>
#include <xAODEgamma/EgammaTruthxAODHelpers.h>

#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicCode.h"
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"

#include "GaudiKernel/ITHistSvc.h"


// local include
#include "dumpAllSystematics.h"

DumpAllSystematics::DumpAllSystematics(const std::string& name, ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc)
{
  declareProperty("EgammaCalibrationAndSmearingTools", m_EgammaCalibrationAndSmearingTools);
  declareProperty("particle", m_particle_name="", "electron/photon");
  declareProperty("recoContainer", m_reco_container_name="", "reco container name");
  declareProperty("keepOnlyOne", m_keep_one=false, "keep only the most energetic particle");
}


StatusCode DumpAllSystematics::initialize()
{
  // Greet the user:
  ATH_MSG_INFO ("Initializing " << name() << "...");

  if (m_particle_name.empty()) {
    ATH_MSG_WARNING("particle property is empty, choose between \"photon\" or \"electron\". Assuming electron.");
    m_particle_name = "electron";
  }
  if (m_particle_name != "electron" and m_particle_name != "photon")
  {
    ATH_MSG_FATAL("cannot understand particle property: must be \"electron\" or \"photon\"");
  }

  m_particle_type = m_particle_name == "electron" ? ParticleType::ELECTRON : ParticleType::PHOTON;

  // Retrieve the tools:
  ATH_MSG_INFO("retrieving tools");
  if (!m_EgammaCalibrationAndSmearingTools.retrieve().isSuccess())
   { ATH_MSG_ERROR("Cannot retrieve calibration tools"); }
  ATH_MSG_INFO("retrieving tools, done");

  if (m_EgammaCalibrationAndSmearingTools.empty()) {
    ATH_MSG_FATAL("no tool specified in the job options");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("number of tools: " << m_EgammaCalibrationAndSmearingTools.size());
  if (m_reco_container_name.empty()) {
    if (m_particle_type == ParticleType::ELECTRON) m_reco_container_name = "Electrons";
    else m_reco_container_name = "Photons";
  }

  ATH_MSG_INFO("================================================================================");
  ATH_MSG_INFO("particle: " << m_particle_name);
  ATH_MSG_INFO("keep only one: " << m_keep_one);
  ATH_MSG_INFO("reco container: " << m_reco_container_name);
  ATH_MSG_INFO("================================================================================");



  ATH_MSG_DEBUG("creating tree");

  const std::string prefix = m_particle_type == ParticleType::ELECTRON ? "el_" : "ph_";
  m_tree = new TTree("all_sys", "all_sys");
  m_tree->Branch("index", &m_instance_index, "index/I")->SetTitle("index of the particle as in the original dataset");
  m_tree->Branch("actualIntPerXing", &m_actualIntPerXing, "actualIntPerXing/F");
  m_tree->Branch("eventNumber", &m_EventNumber, "eventNumber/I");
  m_tree->Branch("runNumber", &m_RunNumber, "runNumber/I");
  m_tree->Branch((prefix + "truth_E").c_str(), &m_truth_E, (prefix + "truth_E/F").c_str());
  m_tree->Branch((prefix + "truth_eta").c_str(), &m_truth_eta, (prefix + "truth_eta/F").c_str());
  m_tree->Branch((prefix + "truth_pt").c_str(), &m_truth_pt, (prefix + "truth_pt/F").c_str());
  m_tree->Branch((prefix + "truth_phi").c_str(), &m_truth_phi, (prefix + "truth_phi/F").c_str());
  m_tree->Branch((prefix + "truth_pdgId").c_str(), &m_truth_pdgId, (prefix + "truth_pdgId/I").c_str());
  m_tree->Branch((prefix + "truth_parent_pdgId").c_str(), &m_truth_parent_pdgId, (prefix + "truth_parent_pdgId/I").c_str());
  m_tree->Branch((prefix + "truth_matched").c_str(), &m_truth_matched, (prefix + "truth_matched/O").c_str());
  m_tree->Branch((prefix + "cl_eta").c_str(), &m_cl_eta, (prefix + "cl_eta/F").c_str());
  m_tree->Branch((prefix + "cl_etaCalo").c_str(), &m_cl_etaCalo, (prefix + "cl_etaCalo/F").c_str());
  m_tree->Branch((prefix + "cl_phi").c_str(), &m_cl_phi, (prefix + "cl_phi/F").c_str());
  m_tree->Branch((prefix + "cl_rawcl_Es0").c_str(), &m_cl_rawcl_Es0, (prefix + "cl_rawcl_Es0/F").c_str());
  m_tree->Branch((prefix + "cl_rawcl_Es1").c_str(), &m_cl_rawcl_Es1, (prefix + "cl_rawcl_Es1/F").c_str());
  m_tree->Branch((prefix + "cl_rawcl_Es2").c_str(), &m_cl_rawcl_Es2, (prefix + "cl_rawcl_Es2/F").c_str());
  m_tree->Branch((prefix + "cl_rawcl_Es3").c_str(), &m_cl_rawcl_Es3, (prefix + "cl_rawcl_Es3/F").c_str());
  m_tree->Branch((prefix + "cl_E").c_str(), &m_cl_E, (prefix + "cl_E/F").c_str())->SetTitle("original xAOD caloCluster energy");
  m_tree->Branch((prefix + "wstot").c_str(), &m_wstot, (prefix + "wstot/F").c_str());
  if (m_particle_type == ParticleType::PHOTON) {
    m_tree->Branch("ph_truth_isConv", &m_truth_isConv, "ph_truth_isConv/O");
    m_tree->Branch("ph_truth_Rconv", &m_truth_Rconv, "ph_truth_Rconv/F");
    m_tree->Branch("ph_Rconv", &m_ph_Rconv, "ph_Rconv/F");
    m_tree->Branch("ph_convFlag", &m_ph_convFlag, "ph_convFlag/I");
  }
  m_tree->Branch("npv", &m_npv, "npv/I");

  m_nominal_E.resize(m_EgammaCalibrationAndSmearingTools.size());
  m_energy_variations.resize(m_EgammaCalibrationAndSmearingTools.size());
  m_energy_variations_sum_up.resize(m_EgammaCalibrationAndSmearingTools.size());
  m_energy_variations_sum_down.resize(m_EgammaCalibrationAndSmearingTools.size());

  std::vector<std::vector<std::string> > all_sys_names_per_tool;
  std::set<std::string> all_sys_names;

  for (unsigned int itool = 0; itool != m_EgammaCalibrationAndSmearingTools.size(); ++itool) {
    const std::string tool_name = m_EgammaCalibrationAndSmearingTools[itool].name();
    {
      const std::string branch_name = prefix + tool_name + "_nominal_E";
      m_tree->Branch(branch_name.c_str(), &m_nominal_E[itool], (branch_name + "/F").c_str());
    }

    const CP::SystematicSet& sys_set = m_EgammaCalibrationAndSmearingTools[itool]->recommendedSystematics();
    ATH_MSG_INFO("size of the systematics set for tool [" << itool << "] " + m_EgammaCalibrationAndSmearingTools[itool].name() << ": " << sys_set.size());
    std::vector<std::string> sys_names; sys_names.reserve(sys_set.size());
    for (const auto sys : sys_set) { if (sys.parameter() == 1) { sys_names.push_back(sys.name()); all_sys_names.insert(sys.name()); } }
    std::sort(sys_names.begin(), sys_names.end());
    all_sys_names_per_tool.push_back(sys_names);


    m_energy_variations[itool].resize(sys_set.size());

    int isys = 0;
    for (const auto sys : sys_set) {
      const std::string branch_name = prefix + tool_name + "_ratio_" + sys.name();
      m_tree->Branch(branch_name.c_str(), &m_energy_variations[itool][isys],
                     (branch_name + "/F").c_str());
      ++isys;
    }

    m_tree->Branch((prefix + tool_name + "_ratio_SCALE_sum__1up").c_str(), &m_energy_variations_sum_up[itool], (prefix + tool_name +  "_ratio_SCALE_sum__1up/F").c_str());
    m_tree->Branch((prefix + tool_name + "_ratio_SCALE_sum__1down").c_str(), &m_energy_variations_sum_down[itool], (prefix + tool_name +  "_ratio_SCALE_sum__1down/F").c_str());
  }

  {
    std::string header = std::string(45, ' ');
    for (unsigned int i = 0; i != m_EgammaCalibrationAndSmearingTools.size(); ++i)  {
      header += (boost::format(" [%d] ") % i).str();
    }
    ATH_MSG_INFO(header);
  }
  for (auto sysname : all_sys_names) {
    std::string line = (boost::format("%45s") % sysname).str();
    for (const auto& sys_per_tool : all_sys_names_per_tool) {
      if (std::find(sys_per_tool.begin(), sys_per_tool.end(), sysname) != sys_per_tool.end()) line += "  X  ";
      else line += "     ";
    }
    ATH_MSG_INFO(line);
  }

  if (m_particle_type == ParticleType::ELECTRON) ATH_MSG_INFO("dumping electrons");
  else if (m_particle_type == ParticleType::PHOTON) ATH_MSG_INFO("dumping photons");
  
  // output configuration
  ServiceHandle<ITHistSvc> histSvc("THistSvc", name());
  CHECK(histSvc.retrieve());
  const std::string tree_stream = "/DATASTREAM";
  CHECK(histSvc->regTree(tree_stream + "/myTree", m_tree));

  return StatusCode::SUCCESS;
}


StatusCode DumpAllSystematics::finalize() {
  ATH_MSG_INFO("Particle saved: " << m_tree->GetEntries());
  if (m_tree->GetEntries() == 0) ATH_MSG_WARNING("no particle saved");
  ATH_MSG_INFO("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DumpAllSystematics::execute()
{
  const xAOD::EventInfo* eventInfo = 0;
  ATH_CHECK(evtStore()->retrieve(eventInfo));

  m_actualIntPerXing = eventInfo->actualInteractionsPerCrossing();
  m_averageIntPerXing = eventInfo->averageInteractionsPerCrossing();
  m_EventNumber = eventInfo->eventNumber();
  m_RunNumber = eventInfo->runNumber();

  // vertex information
  const xAOD::VertexContainer* vtx_container = nullptr;
  if (!evtStore()->retrieve(vtx_container, "PrimaryVertices")) { m_npv = -999; }
  else {
    m_npv = 0;
    for (const auto* vtx : *vtx_container) { if (vtx->vertexType() == xAOD::VxType::PriVtx or vtx->vertexType() ==  xAOD::VxType::PileUp) ++m_npv; }
  }


  if (m_particle_type == ParticleType::ELECTRON) {  // TODO: find a better way to generalize to electron / photons

    const xAOD::ElectronContainer* electrons = nullptr;
    CHECK(evtStore()->retrieve(electrons, m_reco_container_name));

    std::pair<xAOD::ElectronContainer*, xAOD::ShallowAuxContainer*> electrons_shallowCopy = xAOD::shallowCopyContainer(*electrons);
    for (const auto& el : *electrons_shallowCopy.first) {
      ATH_MSG_DEBUG("new electron eta: " << el->eta());


      CHECK(do_truth(*el));
      CHECK(do_egamma(*el));
      for (unsigned int itool = 0; itool != m_EgammaCalibrationAndSmearingTools.size(); ++itool) {
        CHECK(do_energy(*el, itool));
      }

      m_tree->Fill();

      if (m_keep_one) break;
    }
  }
  else if (m_particle_type == ParticleType::PHOTON) {

    const xAOD::PhotonContainer* photons = nullptr;
    CHECK(evtStore()->retrieve(photons, m_reco_container_name));

    std::pair<xAOD::PhotonContainer*, xAOD::ShallowAuxContainer*> photons_shallowCopy = xAOD::shallowCopyContainer(*photons);
    for (const auto& ph : *photons_shallowCopy.first) {
      ATH_MSG_DEBUG("new photon eta: " << ph->eta());

      CHECK(do_truth(*ph));
      CHECK(do_egamma(*ph));

      if (const auto* vertex = ph->vertex()) {
	  m_ph_Rconv = hypot(vertex->position().x(), vertex->position().y());
      }
      else { m_ph_Rconv = 0; }

      const auto original = xAOD::EgammaHelpers::conversionType(ph);
      if (original == 3) m_ph_convFlag = 2;
      else if (original != 0) m_ph_convFlag = 1;
      else m_ph_convFlag = original;

      for (unsigned int itool = 0; itool != m_EgammaCalibrationAndSmearingTools.size(); ++itool) {
        CHECK(do_energy(*ph, itool));
      }

      m_tree->Fill();

      if (m_keep_one) break;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode DumpAllSystematics::do_truth(const xAOD::Egamma& particle)
{
  // truth
  const xAOD::TruthParticle* true_particle = nullptr;
  true_particle = xAOD::TruthHelpers::getTruthParticle(particle);

  if (true_particle) {
    m_truth_matched = true;
    m_truth_pt = true_particle->pt();
    m_truth_phi = true_particle->phi();
    m_truth_eta = true_particle->eta();
    m_truth_E = true_particle->e();
    m_truth_pdgId = true_particle->pdgId();

    const xAOD::TruthParticle* true_parent = true_particle->parent();
    if (true_parent) { m_truth_parent_pdgId= true_parent->pdgId(); }
    else { m_truth_parent_pdgId = -999; }

    if (m_particle_type == ParticleType::PHOTON) {
      m_truth_isConv = xAOD::EgammaHelpers::isTrueConvertedPhoton(true_particle);
      m_truth_Rconv = (true_particle->pdgId() == 22) ? (true_particle->hasDecayVtx() ? true_particle->decayVtx()->perp() : 0) : -999;
    }

  }
  else {
    m_truth_pt = m_truth_phi = m_truth_eta = m_truth_E = m_truth_pdgId = m_truth_parent_pdgId = m_truth_Rconv = -999;
    m_truth_matched = false;
    m_truth_isConv = false;  // careful here (always check truth_matched before)
  }
  return StatusCode::SUCCESS;
}

StatusCode DumpAllSystematics::do_egamma(const xAOD::Egamma& particle)
{
  m_cl_phi = particle.phi();
  m_cl_eta = particle.caloCluster()->eta();
  m_cl_etaCalo = xAOD::get_eta_calo(*particle.caloCluster(),particle.author());
  m_cl_rawcl_Es0 = particle.caloCluster()->energyBE(0);
  m_cl_rawcl_Es1 = particle.caloCluster()->energyBE(1);
  m_cl_rawcl_Es2 = particle.caloCluster()->energyBE(2);
  m_cl_rawcl_Es3 = particle.caloCluster()->energyBE(3);
  static const SG::AuxElement::Accessor<float> wstot_accessor("wtots1");
  m_wstot = wstot_accessor.isAvailable(particle) ? wstot_accessor(particle) : -999;
  m_cl_E = particle.caloCluster()->e();

  return StatusCode::SUCCESS;
}

StatusCode DumpAllSystematics::do_energy(xAOD::Egamma& particle, int itool)
{
  m_nominal_E[itool] = m_energy_variations_sum_up[itool] = m_energy_variations_sum_down[itool] = -999.;
  std::fill(m_energy_variations[itool].begin(), m_energy_variations[itool].end(), -999.);

  if (m_EgammaCalibrationAndSmearingTools[itool]->applySystematicVariation(CP::SystematicSet()) != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("cannot apply nominal energy");
    return StatusCode::FAILURE;
  }
  if (m_EgammaCalibrationAndSmearingTools[itool]->applyCorrection(particle) == CP::CorrectionCode::Ok) {
    m_nominal_E[itool] = particle.e();
    ATH_MSG_DEBUG("nominal energy: " << particle.e());
  }
  else {
    ATH_MSG_WARNING("Cannot calibrate electron, eta: "  << particle.eta() << " phi: " << particle.phi());
    return StatusCode::FAILURE;
  }

  // systematics
  m_energy_variations_sum_up[itool] = 0.;
  m_energy_variations_sum_down[itool] = 0.;

  const CP::SystematicSet& sys_set = m_EgammaCalibrationAndSmearingTools[itool]->recommendedSystematics();
  int isys = -1;  // ugly
  for (const auto sys : sys_set) {
    ++isys;
    CP::SystematicSet ss;
    ss.insert(sys);

    if (m_EgammaCalibrationAndSmearingTools[itool]->applySystematicVariation(ss) != CP::SystematicCode::Ok)  {
      ATH_MSG_ERROR("Cannot configure calibration tool for systematic");
      continue;
    }

    if (m_EgammaCalibrationAndSmearingTools[itool]->applyCorrection(particle) == CP::CorrectionCode::Ok) {
      m_energy_variations[itool][isys] = particle.e() / m_nominal_E[itool] - 1;

      if (sys.basename().find("SCALE") != std::string::npos) { // sum only the scale variations
        if (sys.parameter() == 1) m_energy_variations_sum_up[itool] += m_energy_variations[itool][isys] * m_energy_variations[itool][isys];
        else if (sys.parameter() == -1) m_energy_variations_sum_down[itool] += m_energy_variations[itool][isys] * m_energy_variations[itool][isys];
      }
      ATH_MSG_DEBUG("calibrated with systematic " << sys.name() << ": " << particle.e());
    }
    else {
      ATH_MSG_WARNING("Cannot calibrate electron with systematic");
    }
  }

  m_energy_variations_sum_up[itool] = sqrt(m_energy_variations_sum_up[itool]);
  m_energy_variations_sum_down[itool] = sqrt(m_energy_variations_sum_down[itool]);

  return StatusCode::SUCCESS;
}
 
