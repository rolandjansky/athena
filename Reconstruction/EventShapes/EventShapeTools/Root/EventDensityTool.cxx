/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EventDensityTool.cxx 

#include "EventShapeTools/EventDensityTool.h"
#include "fastjet/JetDefinition.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "xAODEventShape/EventShape.h"
#include "xAODEventShape/EventShapeAuxInfo.h"

using fastjet::JetAlgorithm;
using fastjet::JetDefinition;
using fastjet::ClusterSequence;
using fastjet::ClusterSequenceArea;
using fastjet::AreaDefinition;
using fastjet::VoronoiAreaSpec;

//**********************************************************************

EventDensityTool::EventDensityTool(const std::string& name)
  : asg::AsgTool(name),
  m_useAreaFourMom(true),
  m_rhoDec(""), 
  m_sigmaDec(""), 
  m_areaDec("") {  
  declareProperty("JetAlgorithm",    m_jetalg  = "Kt");
  declareProperty("JetRadius",       m_jetrad  = 0.4);
  declareProperty("JetInput",        m_pjgetter);
  declareProperty("AbsRapidityMin",  m_rapmin  = 0.0);
  declareProperty("AbsRapidityMax",  m_rapmax  = 2.0);
  declareProperty("AreaDefinition",  m_areadef = "Voronoi");
  declareProperty("VoronoiRfact",    m_vrfact  = 1.0);
  declareProperty("OutputContainer", m_outcon  = "GenericEventDensity");
  declareProperty("UseFourMomArea", m_useAreaFourMom);
}

//**********************************************************************

EventDensityTool::~EventDensityTool() {}

//**********************************************************************

StatusCode EventDensityTool::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ATH_CHECK( m_pjgetter.retrieve() );

  // Fetch the fastjet algorithm enum
  JetAlgorithm fjalg;
  if      ( m_jetalg == "Kt"     ) fjalg = fastjet::kt_algorithm;
  else if ( m_jetalg == "AntiKt" ) fjalg = fastjet::antikt_algorithm;
  else if ( m_jetalg == "CamKt"  ) fjalg = fastjet::cambridge_algorithm;
  else {
    ATH_MSG_ERROR("Invalid jet algorithm name: " << m_jetalg);
    ATH_MSG_ERROR("Allowed values are Kt, CamKt, AntiKt, etc.");
    return StatusCode::FAILURE;
  }

  // Build jet definition.
  m_fjjetdef = JetDefinition(fjalg, m_jetrad);

  // Build area definition.
  if ( m_areadef == "Voronoi" ) {
    m_fjareadef = AreaDefinition(fastjet::voronoi_area, VoronoiAreaSpec(m_vrfact));
  } else if ( m_areadef == "Active" || m_useAreaFourMom ) {
    // Default is fine here for now.
    // Later might want to switch to seeds and binning used in jet reco.
    // See JetRec/JetFinder for that.
  } else {
    ATH_MSG_WARNING("Unsupported area option: " << m_areadef);
    return StatusCode::FAILURE;
  }

  // Build the jet selector.
  if ( m_rapmin >= 0.0  &&  m_rapmax > m_rapmin ) {
    m_fjselector = fastjet::SelectorAbsRapRange(m_rapmin, m_rapmax);
  } else {
    ATH_MSG_WARNING("Invalid absolute rapidity range: ("
                    << m_rapmin << ", " << m_rapmax << ")");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Configured properties:");
  ATH_MSG_INFO("     JetAlgorithm: " << m_jetalg);
  ATH_MSG_INFO("        JetRadius: " << m_jetrad);
  ATH_MSG_INFO("         JetInput: " << m_pjgetter);
  ATH_MSG_INFO("   AbsRapidityMin: " << m_rapmin);
  ATH_MSG_INFO("   AbsRapidityMax: " << m_rapmax);
  ATH_MSG_INFO("   AreaDefinition: " << m_areadef);
  ATH_MSG_INFO("     VoronoiRfact: " << m_vrfact);
  ATH_MSG_INFO("  OutputContainer: " << m_outcon.key());
  ATH_MSG_INFO("Derived properties:");
  ATH_MSG_INFO("        Fastjet jet defn: " << m_fjjetdef.description());
  ATH_MSG_INFO("       Fastjet area defn: " << m_fjareadef.description());
  ATH_MSG_INFO("    Fastjet jet selector: " << m_fjselector.description());
  ATH_MSG_INFO("  Use area four-momentum: " << m_useAreaFourMom);

  // Fill the EventShape object
  m_rhoDec   = SG::AuxElement::Accessor<float>("Density");
  m_sigmaDec = SG::AuxElement::Accessor<float>("DensitySigma");
  m_areaDec  = SG::AuxElement::Accessor<float>("DensityArea");

  // DataHandles
  ATH_CHECK( m_outcon.initialize() );
  m_outconIn = m_outcon.key();
  ATH_CHECK( m_outconIn.initialize() );

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode EventDensityTool::fillEventShape() const {
  
  ATH_MSG_DEBUG("Begin fillEventShape()");

  // check if not already existing
  auto handle_inOut = SG::makeHandle (m_outconIn);
  if ( handle_inOut.isValid() ) {
    ATH_MSG_WARNING( "EventShape with key "<< m_outconIn.key() << " already exists. Not overwriting it." );
    return StatusCode::SUCCESS;
  }

  xAOD::EventShape *pevs = new xAOD::EventShape();
  std::unique_ptr<const xAOD::EventShape> pevs_ptr(pevs);

  xAOD::EventShapeAuxInfo* pevsaux = new xAOD::EventShapeAuxInfo();
  std::unique_ptr<const xAOD::EventShapeAuxInfo> pevsaux_ptr(pevsaux);
  pevs->setStore( pevsaux );

  // Change the order: first fill the object and then record
  ATH_CHECK(fillEventShape(pevs));  

  SG::WriteHandle<xAOD::EventShape> h_out(m_outcon);
  if ( ! h_out.put(std::move(pevs_ptr), std::move(pevsaux_ptr) )) {
    ATH_MSG_WARNING("Unable to write new Jet collection and aux store to event store: " << m_outcon.key());
  } else {
    ATH_MSG_DEBUG("Created new EventShape container: " << m_outcon.key());
  }

  return StatusCode::SUCCESS;
}

//**********************************************************************
StatusCode EventDensityTool::fillEventShape(xAOD::EventShape *pevs) const {

  const PseudoJetVector* ppjv = m_pjgetter->get();
  if ( ppjv == 0 ) {
    ATH_MSG_ERROR( "Couldn't retrieve pseudojets from "<< m_pjgetter->name() );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved input pseudojet count: " <<  ppjv->size());
  }

  // call fillEventShape(PseudoJetVector, EventShape) :
  return fillEventShape(pevs, *ppjv);    
}


StatusCode EventDensityTool::fillEventShape(xAOD::EventShape *pevs, const xAOD::IParticleContainer*) const {
  // Ignore IParticleContainer argument.

  // Fetch inputs.
  const PseudoJetVector* ppjv = m_pjgetter->get();
 
  if ( ppjv == 0 ) {
    ATH_MSG_ERROR("Could not retrieve pseudojets from " << m_pjgetter->name());
    return StatusCode::FAILURE;
    /// TODO : instead build a PseudoJetVector from the input IParticleContainer
  }
  
  return fillEventShape( pevs, *ppjv);
}
  
//**********************************************************************

StatusCode EventDensityTool::
fillEventShape( xAOD::EventShape* pevs, const PseudoJetVector& pjv) const { 
  ATH_MSG_DEBUG("Input pseudojet count: " << pjv.size());
  ATH_MSG_DEBUG("Event shape container address: " << pevs);

  for(const auto & pj : pjv) {
    ATH_MSG_DEBUG(" pj input e="<<pj.e() << " pz="<<pj.pz() << " px="<<pj.px() );
  }
  // Find jets.
  const ClusterSequenceArea* pcsa = new ClusterSequenceArea(pjv, m_fjjetdef, m_fjareadef);
  if ( pcsa == 0 ) {
    ATH_MSG_WARNING("Jet finding failed.");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found jet count: " << pcsa->inclusive_jets().size());

  // Extract rho.
  double rho, sigma, area;
  pcsa->get_median_rho_and_sigma(m_fjselector, m_useAreaFourMom, rho, sigma, area);
  ATH_MSG_DEBUG(" calculated rho="<< rho);

  // Record rho.
  m_rhoDec(*pevs) = rho;
  m_sigmaDec(*pevs) = sigma;
  m_areaDec(*pevs) = area;

  ATH_MSG_DEBUG("Recorded event density:  = " << 0.001*rho << " GeV");

  delete pcsa;

  return StatusCode::SUCCESS;
}
