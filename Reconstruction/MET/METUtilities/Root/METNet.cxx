///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// METNet.cxx
// Implementation file for class METNet
// Author: M.Leigh<mleigh@cern.ch>
///////////////////////////////////////////////////////////////////

// STL Includes
#include <math.h>

// Tool headers
#include "METUtilities/METNet.h"
#include "METUtilities/METHelpers.h"

// Event info and extra EDMs
#include "xAODEventInfo/EventInfo.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODTracking/VertexContainer.h"

// ETMiss EDMs
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"

// Physics object EDMs
#include "xAODJet/JetContainer.h"

namespace met {

  using std::vector;
  using std::string;

  using xAOD::MissingET;
  using xAOD::MissingETContainer;
  using xAOD::MissingETAssociation;
  using xAOD::MissingETAssociationMap;
  using xAOD::MissingETAuxContainer;
  using xAOD::MissingETComposition;

  using xAOD::EventInfo;
  using xAOD::IParticle;
  using xAOD::IParticleContainer;
  using xAOD::JetContainer;
  using xAOD::VertexContainer;

  static const SG::AuxElement::ConstAccessor<std::string> acc_name("name");
  static const SG::AuxElement::ConstAccessor<MissingETBase::Types::bitmask_t> acc_source("source");

  typedef ElementLink<xAOD::IParticleContainer> iplink_t;
  static const SG::AuxElement::ConstAccessor< std::vector<iplink_t > > acc_constitObjLinks("ConstitObjectLinks");
  static const SG::AuxElement::Decorator< std::vector<iplink_t> > dec_constitObjLinks("ConstitObjectLinks");

  // Decorator for passing the vector of inputs to the met object
  static const SG::AuxElement::ConstAccessor< std::vector<float> > acc_inputvalues("input_values");
  static const SG::AuxElement::ConstAccessor< std::vector<std::string> > acc_inputnames("input_names");
  static const SG::AuxElement::Decorator< std::vector<float> > dec_inputvalues("input_values");
  static const SG::AuxElement::Decorator< std::vector<std::string> > dec_inputnames("input_names");

  //////////////////////////////////////////////////////////////////////////////

  METNet::METNet(const std::string& name)
  : AsgTool(name),
  m_acc_jvt(nullptr),
  m_acc_fjvt(nullptr),
  m_acc_fjvtCut(nullptr)
  {
    declareProperty( "NetworkLocation", m_NetLocation = "METUtilities/run2_13TeV/METNet/dummy_network.onnx" );
    declareProperty( "JetCollection", m_JetCollection = "AntiKt4EMPFlow" );
    declareProperty( "PVContainer", m_PVContainer = "PrimaryVertices" );
    declareProperty( "JetJvtMomentName", m_jetJvtMomentName = "Jvt" );
    declareProperty( "JetFwdJvtMomentName", m_jetFwdJvtMomentName = "fJvt" );
    declareProperty( "JetFwdJvtCutName", m_jetFwdJvtCutName = "passFJVT" );
  }

  //////////////////////////////////////////////////////////////////////////////

  METNet::~METNet() { }

  //////////////////////////////////////////////////////////////////////////////

  StatusCode METNet::initialize() {
    ATH_MSG_INFO( "Initializing " << name() << "..." );

    // Check the name of the passed network to see if it is a dummy (untrained) network
    if ( m_NetLocation.find("dummy") != std::string::npos ) {
      ATH_MSG_WARNING( "You are loading a dummy/untrained network! This should only be done when testing the code!\n"
                       " -- If you wish to load a trained network please specify a path to an onnx file using the 'NetworkLocation' argument.\n"
                       " -- A list of supported networks can be found at atlas-groupdata.web.cern.ch/atlas-groupdata/METUtilities/run2_13TeV/METNet/" );
    }

    bool DoPFlow = ( m_JetCollection == "AntiKt4EMPFlow" );

    // Configurable jet decoration accessors
    m_acc_jvt.reset(new SG::AuxElement::ConstAccessor<float>(m_jetJvtMomentName));
    m_acc_fjvt.reset(new SG::AuxElement::ConstAccessor<float>(m_jetFwdJvtMomentName));
    m_acc_fjvtCut.reset(new SG::AuxElement::ConstAccessor<char>(m_jetFwdJvtCutName));

    // Tool initialisations
    ATH_MSG_INFO("Initialising the METNet ONNX environment using the file " + m_NetLocation);
    m_metnethandler = std::make_unique<METNetHandler>( m_NetLocation );
    ATH_CHECK( m_metnethandler->initialize() );

    m_metmaker_tight.setTypeAndName( "met::METMaker/metmaker_tight" );
    ATH_CHECK( m_metmaker_tight.setProperty( "DoPFlow", DoPFlow ) );
    ATH_CHECK( m_metmaker_tight.setProperty( "JetSelection", "Tight" ) );
    ATH_CHECK( m_metmaker_tight.setProperty( "jetJvtMomentName", m_jetJvtMomentName ) );
    ATH_CHECK( m_metmaker_tight.retrieve() );

    m_metmaker_loose.setTypeAndName( "met::METMaker/metmaker_loose" );
    ATH_CHECK( m_metmaker_loose.setProperty( "DoPFlow", DoPFlow ) );
    ATH_CHECK( m_metmaker_loose.setProperty( "JetSelection", "Loose" ) );
    ATH_CHECK( m_metmaker_loose.setProperty( "jetJvtMomentName", m_jetJvtMomentName ) );
    ATH_CHECK( m_metmaker_loose.retrieve() );

    m_metmaker_tghtr.setTypeAndName( "met::METMaker/metmaker_tghtr" );
    ATH_CHECK( m_metmaker_tghtr.setProperty( "DoPFlow", DoPFlow ) );
    ATH_CHECK( m_metmaker_tghtr.setProperty( "JetSelection", "Tighter" ) );
    ATH_CHECK( m_metmaker_tghtr.setProperty( "jetJvtMomentName", m_jetJvtMomentName ) );
    ATH_CHECK( m_metmaker_tghtr.retrieve() );

    m_metmaker_fjvt.setTypeAndName( "met::METMaker/metmaker_fjvt" );
    ATH_CHECK( m_metmaker_fjvt.setProperty( "DoPFlow", DoPFlow ) );
    ATH_CHECK( m_metmaker_fjvt.setProperty( "JetSelection", "Tight" ) );
    ATH_CHECK( m_metmaker_fjvt.setProperty( "jetJvtMomentName", m_jetJvtMomentName ) );
    ATH_CHECK( m_metmaker_fjvt.setProperty( "JetRejectionDec", m_jetFwdJvtCutName ) );
    ATH_CHECK( m_metmaker_fjvt.retrieve() );

    m_metsigtool.setTypeAndName( "met::METSignificance/metsigtool" );
    ATH_CHECK( m_metsigtool.setProperty( "JetCollection", m_JetCollection ) );
    ANA_CHECK( m_metsigtool.setProperty( "TreatPUJets", true ) );
    ANA_CHECK( m_metsigtool.setProperty( "DoPhiReso", true ) );
    ATH_CHECK( m_metsigtool.setProperty( "jetJvtMomentName", m_jetJvtMomentName ) );
    ATH_CHECK( m_metsigtool.setProperty( "JetFwdJvtMomentName", m_jetFwdJvtMomentName ) );
    ATH_CHECK( m_metsigtool.retrieve() );

    return StatusCode::SUCCESS;
  }

  //////////////////////////////////////////////////////////////////////////////

  StatusCode METNet::rebuildMET( const std::string& metKey,
                                 xAOD::Type::ObjectType metType,
                                 xAOD::MissingETContainer* metCont,
                                 const xAOD::IParticleContainer* collection,
                                 const xAOD::MissingETAssociationMap* map,
                                 MissingETBase::UsageHandler::Policy objScale )
  {
    return m_metmaker_tight->rebuildMET( metKey, metType, metCont, collection, map, objScale );
  }

  StatusCode METNet::rebuildJetMET( const std::string& metJetKey,
                                    const std::string& softClusKey,
                                    const std::string& softTrkKey,
                                    xAOD::MissingETContainer* metCont,
                                    const xAOD::JetContainer* jets,
                                    const xAOD::MissingETContainer* metCoreCont,
                                    const xAOD::MissingETAssociationMap* map,
                                    bool doJetJVT )
  {

    // First we need to check that the jet container has the appropriate decorations
    for ( const auto& jet : *jets ) {
      ATH_MSG_VERBOSE( "Checking that the jets are decorated with their JVT values" );
      if ( !m_acc_jvt->isAvailable(*jet) ){
        ATH_MSG_ERROR( "Could not find JVT values on the jets using given flag: " + m_jetJvtMomentName );
        return StatusCode::FAILURE;
      }
      ATH_MSG_VERBOSE( "Checking that the jets are decorated with their FJVT values" );
      if ( !m_acc_fjvt->isAvailable(*jet) ){
        ATH_MSG_ERROR( "Could not find FJVT values on the jets using given flag: " + m_jetFwdJvtMomentName );
        return StatusCode::FAILURE;
      }
      ATH_MSG_VERBOSE( "Checking that the jets are decorated with their passFJVT values" );
      if ( !m_acc_fjvtCut->isAvailable(*jet) ){
        ATH_MSG_ERROR( "Could not find cut on FJVT values on the jets using given flag: " + m_jetFwdJvtCutName );
        return StatusCode::FAILURE;
      }
    }

    // Retrieving the event information (Needed for METSig, so we do it early)
    const xAOD::EventInfo* ei = 0;
    ATH_CHECK( evtStore()->retrieve( ei, "EventInfo" ) );
    float avgmu = ei->averageInteractionsPerCrossing();
    float actmu = ei->actualInteractionsPerCrossing();

    //////////////////// BUILDING MET WPS /////////////////////

    // Creating the new containers for all the WPs
    auto metCont_tight = std::make_shared<MissingETContainer>();
    auto metCont_tightAux = std::make_shared<MissingETAuxContainer>();
    metCont_tight->setStore( metCont_tightAux.get() );

    auto metCont_loose = std::make_shared<MissingETContainer>();
    auto metCont_looseAux = std::make_shared<MissingETAuxContainer>();
    metCont_loose->setStore( metCont_looseAux.get() );

    auto metCont_tghtr = std::make_shared<MissingETContainer>();
    auto metCont_tghtrAux = std::make_shared<MissingETAuxContainer>();
    metCont_tghtr->setStore( metCont_tghtrAux.get() );

    auto metCont_fjvt = std::make_shared<MissingETContainer>();
    auto metCont_fjvtAux = std::make_shared<MissingETAuxContainer>();
    metCont_fjvt->setStore( metCont_fjvtAux.get() );

    auto metCont_calo = std::make_shared<MissingETContainer>();
    auto metCont_caloAux = std::make_shared<MissingETAuxContainer>();
    metCont_calo->setStore( metCont_caloAux.get() );

    auto metCont_track = std::make_shared<MissingETContainer>();
    auto metCont_trackAux = std::make_shared<MissingETAuxContainer>();
    metCont_track->setStore( metCont_trackAux.get() );

    // Copying the contents of the original container into each WP container
    ATH_MSG_VERBOSE( "Making copies of base MET container" );
    ATH_CHECK( copyMETContainer( metCont_tight.get(), metCont ) );
    ATH_CHECK( copyMETContainer( metCont_loose.get(), metCont ) );
    ATH_CHECK( copyMETContainer( metCont_tghtr.get(), metCont ) );
    ATH_CHECK( copyMETContainer( metCont_fjvt.get(),  metCont ) );
    ATH_CHECK( copyMETContainer( metCont_calo.get(),  metCont ) );
    ATH_CHECK( copyMETContainer( metCont_track.get(), metCont, true ) );

    // Building the jet and soft terms for each WP (Calo and Track share the same METMaker as Tight, just different settings, jvt for all!)
    ATH_MSG_VERBOSE( "Building jet and soft terms for each MET WP" );
    ATH_CHECK( m_metmaker_tight->rebuildJetMET(   "RefJet", "PVSoftTrk", metCont_tight.get(), jets, metCoreCont, map, true ) );
    ATH_CHECK( m_metmaker_loose->rebuildJetMET(   "RefJet", "PVSoftTrk", metCont_loose.get(), jets, metCoreCont, map, true ) );
    ATH_CHECK( m_metmaker_tghtr->rebuildJetMET(   "RefJet", "PVSoftTrk", metCont_tghtr.get(), jets, metCoreCont, map, true ) );
    ATH_CHECK( m_metmaker_fjvt->rebuildJetMET(    "RefJet", "PVSoftTrk", metCont_fjvt.get(),  jets, metCoreCont, map, true ) );
    ATH_CHECK( m_metmaker_tight->rebuildJetMET(   "RefJet", "SoftClus",  metCont_calo.get(),  jets, metCoreCont, map, true ) );
    ATH_CHECK( m_metmaker_tight->rebuildTrackMET( "RefJet", "PVSoftTrk", metCont_track.get(), jets, metCoreCont, map, true ) );

    // Building and adding the "Final" MET object to each WP container
    ATH_MSG_VERBOSE( "Building the Final MET object for each MET WP" );
    ATH_CHECK( m_metmaker_tight->buildMETSum( "Final", metCont_tight.get(), MissingETBase::Source::Track  ) );
    ATH_CHECK( m_metmaker_loose->buildMETSum( "Final", metCont_loose.get(), MissingETBase::Source::Track  ) );
    ATH_CHECK( m_metmaker_tghtr->buildMETSum( "Final", metCont_tghtr.get(), MissingETBase::Source::Track  ) );
    ATH_CHECK( m_metmaker_fjvt->buildMETSum(  "Final", metCont_fjvt.get(),  MissingETBase::Source::Track  ) );
    ATH_CHECK( m_metmaker_tight->buildMETSum( "Final", metCont_calo.get(),  MissingETBase::Source::LCTopo ) );
    ATH_CHECK( m_metmaker_tight->buildMETSum( "Final", metCont_track.get(), MissingETBase::Source::Track  ) );

    //////////////////// SETTING UP OUTPUT ////////////////////

    // Initialise two vectors: One for holding the network input values and another for the input names
    std::vector<std::string> input_names;
    std::vector<float> input_values;

    //////////////////// CLASS 1 VARIABLES ////////////////////

    // Add the "Final" MET object and MET Sig to the vectors for each WP
    ATH_CHECK( addMETFinal( "Tight", metCont_tight.get(), avgmu, input_names, input_values ) );
    ATH_CHECK( addMETFinal( "Loose", metCont_loose.get(), avgmu, input_names, input_values ) );
    ATH_CHECK( addMETFinal( "Tghtr", metCont_tghtr.get(), avgmu, input_names, input_values ) );
    ATH_CHECK( addMETFinal( "FJVT",  metCont_fjvt.get(),  avgmu, input_names, input_values ) );
    ATH_CHECK( addMETFinal( "Calo",  metCont_calo.get(),  avgmu, input_names, input_values ) );
    ATH_CHECK( addMETFinal( "Track", metCont_track.get(), avgmu, input_names, input_values ) );

    //////////////////// CLASS 2 VARIABLES ////////////////////

    // Add the unique Jet terms from the WP containers (not from Calo or Track)
    ATH_CHECK( addMETTerm( "Tight", (*metCont_tight)["RefJet"], input_names, input_values ) );
    ATH_CHECK( addMETTerm( "Loose", (*metCont_loose)["RefJet"], input_names, input_values ) );
    ATH_CHECK( addMETTerm( "Tghtr", (*metCont_tghtr)["RefJet"], input_names, input_values ) );
    ATH_CHECK( addMETTerm( "FJVT",  (*metCont_fjvt)["RefJet"],  input_names, input_values ) );

    // Add the unique hard terms from the Tight MET container (retrieved by type not key)
    ATH_CHECK( addMETTerm( "Tight", *(metCont_tight->find(MissingETBase::Source::muon())),     input_names, input_values ) );
    ATH_CHECK( addMETTerm( "Tight", *(metCont_tight->find(MissingETBase::Source::electron())), input_names, input_values ) );
    ATH_CHECK( addMETTerm( "Tight", *(metCont_tight->find(MissingETBase::Source::photon())),   input_names, input_values ) );

    // Add the CST and the TST from the Loose and Calo containers
    ATH_CHECK( addMETTerm( "Loose", (*metCont_loose)["PVSoftTrk"], input_names, input_values ) );
    ATH_CHECK( addMETTerm( "Calo",  (*metCont_calo)["SoftClus"],   input_names, input_values ) );

    //////////////////// CLASS 3 VARIABLES ////////////////////

    // Pileup information and tracking information (using the user provided primary vertex container name)
    const xAOD::VertexContainer* pvtxs = 0;
    ATH_CHECK( evtStore()->retrieve( pvtxs, m_PVContainer ) );
    int NVx_2Tracks = 0;
    int NVx_4Tracks = 0;
    int PV_NTracks = 0;
    int n_tracks = 0;
    for ( const auto& vx : *pvtxs ) {
      n_tracks = vx->nTrackParticles();
      if ( n_tracks>=2 ) NVx_2Tracks++;
      if ( n_tracks>=4 ) NVx_4Tracks++;
      if ( vx->vertexType() == xAOD::VxType::PriVtx ) PV_NTracks = n_tracks;
    }
    ATH_CHECK( addInputValue( "ActMu",       actmu,       input_names, input_values ) );
    ATH_CHECK( addInputValue( "NVx_2Tracks", NVx_2Tracks, input_names, input_values ) );
    ATH_CHECK( addInputValue( "NVx_4Tracks", NVx_4Tracks, input_names, input_values ) );
    ATH_CHECK( addInputValue( "PV_NTracks",  PV_NTracks,  input_names, input_values ) );

    // Information about object multiplicities located in the Tight MET container
    ATH_CHECK( addInputValue( "N_Muons", acc_constitObjLinks( *(*(metCont_tight->find(MissingETBase::Source::muon()))) ).size(),     input_names, input_values ) );
    ATH_CHECK( addInputValue( "N_Ele",   acc_constitObjLinks( *(*(metCont_tight->find(MissingETBase::Source::electron()))) ).size(), input_names, input_values ) );
    ATH_CHECK( addInputValue( "N_Gamma", acc_constitObjLinks( *(*(metCont_tight->find(MissingETBase::Source::photon()))) ).size(),   input_names, input_values ) );
    ATH_CHECK( addInputValue( "N_Jets",  acc_constitObjLinks( *(*metCont_tight)["RefJet"] ).size(), input_names, input_values ) );

    // Extra Jet and Forward Jet information from the Loose WP
    int n_fwd_jets = 0;
    float sumet_fwd_jets = 0;
    float sum_jet_PU_prob = 0;
    float current_PU_val = 0;
    for( const auto& jet_link : acc_constitObjLinks(*(*metCont_loose)["RefJet"]) ) {

      // Calculating the weighted jet PU probability of all jets
      current_PU_val = (*jet_link)->pt() * GetPUProb( (*jet_link)->eta(),
                                                      (*jet_link)->pt() * 1e-3,
                                                      (*m_acc_jvt)(**jet_link),
                                                      (*m_acc_fjvt)(**jet_link),
                                                      avgmu );
      sum_jet_PU_prob += current_PU_val * current_PU_val;

      // Variables for forward jets
      if ( fabs( (*jet_link)->eta() ) < 2.5 ) continue;
      n_fwd_jets++;
      sumet_fwd_jets += (*jet_link)->pt();

    }
    sum_jet_PU_prob = sqrt(sum_jet_PU_prob);

    ATH_CHECK( addInputValue( "N_FWD_Jets",     n_fwd_jets,      input_names, input_values ) );
    ATH_CHECK( addInputValue( "SumET_FWD_Jets", sumet_fwd_jets,  input_names, input_values ) );
    ATH_CHECK( addInputValue( "Sum_JetPU",      sum_jet_PU_prob, input_names, input_values ) );

    // Finally we add the angle of the MET-tight method (not to be used as input but might be needed for processing)
    float tight_phi = (*metCont_tight)["Final"]->phi();
    ATH_CHECK( addInputValue( "Tight_Phi", tight_phi, input_names, input_values ) );

    //////////////////////// FINISHING ////////////////////////

    // Creating a new MET object on the output container and decorating it with the two vectors
    ATH_MSG_VERBOSE( "Saving network inputs to base MET container" );
    MissingET* net_inpts = nullptr;
    ATH_CHECK( fillMET( net_inpts, metCont, "Net_Inputs", MissingETBase::Source::total() ) );
    dec_inputnames( *(*metCont)["Net_Inputs"] ) = input_names;
    dec_inputvalues( *(*metCont)["Net_Inputs"] ) = input_values;

    // We have to run the base container through METMaker to ensure that it has jet+soft terms (wont be used)
    // Frameworks (like SUSYTools) may try to dereference it either as a test, or to calibrate it
    // This will cause a segmentation fault if it isnt there
    ATH_CHECK(m_metmaker_tight->rebuildJetMET(metJetKey, softClusKey, softTrkKey, metCont, jets, metCoreCont, map, doJetJVT ) );

    return StatusCode::SUCCESS;
  }

  StatusCode METNet::buildMETSum( const std::string& totalName,
                                  xAOD::MissingETContainer* metCont,
                                  MissingETBase::Types::bitmask_t )
    {

      // First we check that the container has the correct decorations
      ATH_MSG_VERBOSE( "Checking MET container for network inputs" );
      if ( (*metCont)["Net_Inputs"] == NULL ) {
        ATH_MSG_ERROR( "Could not find the Net_Inputs MET object in the container. Did you run rebuildJetMET?" );
        return StatusCode::FAILURE;
      }

      // Pulling out the vectors from the met containter
      std::vector<std::string> tmp_names = acc_inputnames(*(*metCont)["Net_Inputs"]);
      std::vector<float> tmp_inputs = acc_inputvalues(*(*metCont)["Net_Inputs"]);
      ATH_MSG_VERBOSE( "Loaded " << tmp_names.size() << " network inputs" );

      // Check that the list of inputs matches the network size
      if ( (int)tmp_inputs.size() != m_metnethandler->getReqSize() ) {
        ATH_MSG_ERROR( "The MET container provided " << tmp_inputs.size()
                        << " elements, but the ONNX network needs exctly " << m_metnethandler->getReqSize() << "!" );
        return StatusCode::FAILURE;
      }

      // Passing the inputs through the network
      std::vector<float> net_met = m_metnethandler->predict( tmp_inputs );

      // Adding the network output to the Final Met container
      MissingET* metFinal = nullptr;
      ATH_CHECK( fillMET( metFinal, metCont, totalName, MissingETBase::Source::total() ) );
      metFinal->setMpx( net_met[0] );
      metFinal->setMpy( net_met[1] );

      ATH_MSG_VERBOSE( "Writing the Final Network MET: (" << net_met[0] << ", " << net_met[1] << ")" );

      return StatusCode::SUCCESS;
    }

  /////////////////////////////// PRIVATE METHODS ///////////////////////////////

  StatusCode METNet::addMETFinal( std::string WP_name,
                                  xAOD::MissingETContainer* met_container,
                                  float avgmu,
                                  std::vector<std::string>& name_vec,
                                  std::vector<float>& val_vec )
  {
    // Add the Final term to the vector
    ATH_CHECK( addMETTerm( WP_name, (*met_container)["Final"], name_vec, val_vec ) );

    // Track does not get MET Significance
    if ( WP_name=="Track" ) return StatusCode::SUCCESS;

    // Get the correct soft term key and build METSig
    std::string softkey = (WP_name=="Calo") ? "SoftClus" : "PVSoftTrk";
    ATH_CHECK( m_metsigtool->varianceMET( met_container, avgmu, "RefJet", softkey, "Final" ) );
    ATH_CHECK( addInputValue( WP_name+"_Sig", m_metsigtool->GetSignificance(), name_vec, val_vec ) );

    return StatusCode::SUCCESS;
  }

  StatusCode METNet::addMETTerm( std::string WP_name,
                                 xAOD::MissingET* met,
                                 std::vector<std::string>& name_vec,
                                 std::vector<float>& val_vec )
  {
    // Build the name of the term from the WP and the Object type from the container
    std::string tname = WP_name + "_" + acc_name(*met);

    // Add the MET magnitude, components, and SumET
    ATH_CHECK( addInputValue( tname+"_ET",    met->met(),   name_vec, val_vec ) );
    ATH_CHECK( addInputValue( tname+"_EX",    met->mpx(),   name_vec, val_vec ) );
    ATH_CHECK( addInputValue( tname+"_EY",    met->mpy(),   name_vec, val_vec ) );
    ATH_CHECK( addInputValue( tname+"_SumET", met->sumet(), name_vec, val_vec ) );

    return StatusCode::SUCCESS;
  }

  StatusCode METNet::addInputValue( std::string var_name,
                                    float value,
                                    std::vector<std::string>& name_vec,
                                    std::vector<float>& val_vec )
  {
    // This is the only place we push_back to either vector
    // Ensuring that they are always consistant with each other!!
    ATH_MSG_VERBOSE( "Adding variable: " << var_name << " = " << value << " to the network input vector" );
    name_vec.push_back( var_name );
    val_vec.push_back( value );
    return StatusCode::SUCCESS;
  }

  StatusCode METNet::copyMETContainer( xAOD::MissingETContainer* new_container,
                                       xAOD::MissingETContainer* old_container,
                                       bool excl_gama )
  {

    // Initiase variables used in the loop
    string name;
    xAOD::MissingET* blank_met;
    MissingETBase::Types::bitmask_t source;
    auto gamma_src = MissingETBase::Source::photon();

    for ( const auto& old_met : *old_container ) {
      blank_met = nullptr;                                                  // Resetting the null pointer
      name = acc_name(*old_met);                                            // Getting the name from the original container
      source = acc_source(*old_met);                                        // Getting the particle type from the original container
      if ( (excl_gama) && (source==gamma_src) ) continue;                   // Skip the photon term if specified (for track met)
      ATH_CHECK( fillMET( blank_met, new_container, name, source ) );       // Initialising the blank met with the new name in the new container
      *(*new_container)[name] = *(*old_container)[name];                    // Copying over the contents from one container to another

      // Also need to copy the original object links
      dec_constitObjLinks(*(*new_container)[name]) = acc_constitObjLinks(*(*old_container)[name]);
    }

    return StatusCode::SUCCESS;
  }

  /////////////////////////////// UNUSED METHODS ///////////////////////////////

  StatusCode METNet::rebuildMET( xAOD::MissingET*,
                                 const xAOD::IParticleContainer*,
                                 const xAOD::MissingETAssociationMap*,
                                 MissingETBase::UsageHandler::Policy )
  {
    ATH_MSG_ERROR( "METNet has not overloaded this version of rebuildMET from IMETMaker!\n"
                   "Please only use this method:\n"
                   "StatusCode METMaker::rebuildMET("
                      "const std::string& metKey,\n"
                      "xAOD::Type::ObjectType metType,\n"
                      "xAOD::MissingETContainer* metCont,\n"
                      "const xAOD::IParticleContainer* collection,\n"
                      "const xAOD::MissingETAssociationMap* map,\n"
                      "MissingETBase::UsageHandler::Policy objScale\n"
                    ")" );
    return StatusCode::FAILURE;
  }

  StatusCode METNet::rebuildMET( xAOD::MissingET*,
                                 const xAOD::IParticleContainer*,
                                 const xAOD::MissingETAssociationMap*,
                                 MissingETBase::UsageHandler::Policy,
                                 bool,
                                 MissingETBase::UsageHandler::Policy )

  {
    ATH_MSG_ERROR( "METNet has not overloaded this version of rebuildMET from IMETMaker!\n"
                   "Please only use this method:\n"
                   "StatusCode METMaker::rebuildMET("
                      "const std::string& metKey,\n"
                      "xAOD::Type::ObjectType metType,\n"
                      "xAOD::MissingETContainer* metCont,\n"
                      "const xAOD::IParticleContainer* collection,\n"
                      "const xAOD::MissingETAssociationMap* map,\n"
                      "MissingETBase::UsageHandler::Policy objScale\n"
                    ")" );
    return StatusCode::FAILURE;
  }

  StatusCode METNet::rebuildJetMET(const std::string& metJetKey,
                                   const std::string& /*metSoftKey*/,
                                   xAOD::MissingETContainer* metCont,
                                   const xAOD::JetContainer* jets,
                                   const xAOD::MissingETContainer* metCoreCont,
                                   const xAOD::MissingETAssociationMap* map,
                                   bool doJetJVT)
  {
    return rebuildJetMET(metJetKey, "SoftClus", "PVSoftTrk", metCont, jets, metCoreCont, map, doJetJVT);
  }

  StatusCode METNet::rebuildJetMET( xAOD::MissingET*,
                                    const xAOD::JetContainer*,
                                    const xAOD::MissingETAssociationMap*,
                                    xAOD::MissingET*,
                                    const xAOD::MissingET*,
                                    xAOD::MissingET*,
                                    const xAOD::MissingET*,
                                    bool,
                                    bool,
                                    std::vector<const xAOD::IParticle*>* )
  {
    ATH_MSG_ERROR( "METNet has not overloaded this version of rebuildJetMET from IMETMaker!\n"
                   "Please use the method with the following arguments:\n"
                   "StatusCode METNet::rebuildJetMET(\n"
                      "const std::string& metJetKey,\n"
                      "const std::string& softKey1,\n"
                      "const std::string& softKey2 (optional),\n"
                      "xAOD::MissingETContainer* metCont,\n"
                      "const xAOD::JetContainer* jets,\n"
                      "const xAOD::MissingETContainer* metCoreCont,\n"
                      "const xAOD::MissingETAssociationMap* map,\n"
                      "bool doJetJVT\n"
                    ")" );
    return StatusCode::FAILURE;
  }

  StatusCode METNet::rebuildTrackMET( const std::string&,
                                      const std::string&,
                                      xAOD::MissingETContainer*,
                                      const xAOD::JetContainer*,
                                      const xAOD::MissingETContainer*,
                                      const xAOD::MissingETAssociationMap*,
                                      bool )
  {
    ATH_MSG_ERROR( "Please dont use the method METNet::rebuildTrackMET!\n"
                   "It has no use in METNet and is a holdover from it's inheritance from IMETMaker." );
    return StatusCode::FAILURE;
  }

  StatusCode METNet::rebuildTrackMET( xAOD::MissingET*,
                                      const xAOD::JetContainer*,
                                      const xAOD::MissingETAssociationMap*,
                                      xAOD::MissingET*,
                                      const xAOD::MissingET*,
                                      bool )
  {
    ATH_MSG_ERROR( "Please dont use the method METNet::rebuildTrackMET!\n"
                   "It has no use in METNet and is a holdover from it's inheritance from IMETMaker." );
    return StatusCode::FAILURE;
  }

  StatusCode METNet::markInvisible( const xAOD::IParticleContainer*,
                                    const xAOD::MissingETAssociationMap*,
                                    xAOD::MissingETContainer* )
  {
    ATH_MSG_ERROR( "Please dont use the method METNet::markInvisible!\n"
                   "It has no use in METNet and is a holdover from it's inheritance from IMETMaker.");
    return StatusCode::FAILURE;
  }

  // The jet Pile Up probability table for both Topo and PFlow jets
  // Function is a copy of the private method in METSignificance.cxx (dropping phi dummy)
  // I am not editing any of this mess!
  double METNet::GetPUProb( double jet_eta, double jet_pt,
                            double jet_jvt, double jet_fjvt,
				                    float avgmu )
  {
    double unc = 0.0;

    if(m_JetCollection == "AntiKt4EMTopo"){
      if(jet_eta<2.4){
	if(jet_pt<30){
	  if(jet_jvt<0.11)      unc = 1;
	  else if(jet_jvt<0.25) unc = 0.0730 + 0.0024 * avgmu + 0.00001 * avgmu * avgmu;
	  else if(jet_jvt<0.85) unc = 0.0995 + 0.0031 * avgmu + 0.00005 * avgmu * avgmu;
	  else if(jet_jvt<0.95) unc = 0.0311 + 0.0025 * avgmu + 0.00005 * avgmu * avgmu;
	  else                  unc = 0.0308 -0.0010 * avgmu + 0.00006 * avgmu * avgmu ;
	}else if(jet_pt<40){
	  if(jet_jvt<0.11)      unc = 1.;
	  else if(jet_jvt<0.25) unc = 1.;
	  else if(jet_jvt<0.85) unc = -0.0188 + 0.0039 * avgmu + 0.00002 * avgmu * avgmu;
	  else if(jet_jvt<0.95) unc = 0.0252 -0.0009 * avgmu + 0.00006 * avgmu * avgmu  ;
	  else                  unc = 0.0085 -0.0003 * avgmu + 0.00002 * avgmu * avgmu  ;
	}else if(jet_pt<50){
	  if(jet_jvt<0.11)      unc = 1;
	  else if(jet_jvt<0.25) unc = 0.0345 -0.0006 * avgmu + 0.00004 * avgmu * avgmu  ;
	  else if(jet_jvt<0.85) unc = 0.1078 -0.0051 * avgmu + 0.00011 * avgmu * avgmu  ;
	  else if(jet_jvt<0.95) unc = -0.0026 + 0.0005 * avgmu + 0.00002 * avgmu * avgmu;
	  else                  unc = 0.0090 -0.0004 * avgmu + 0.00001 * avgmu * avgmu  ;
	}else if(jet_pt<60){
	  if(jet_jvt<0.11)      unc = 1;
	  else if(jet_jvt<0.25) unc = -0.0321 + 0.0030 * avgmu -0.00002 * avgmu * avgmu;
	  else if(jet_jvt<0.85) unc = 0.0260 -0.0007 * avgmu + 0.00003 * avgmu * avgmu ;
	  else                  unc = -0.0040 + 0.0003 * avgmu;
	}else if(jet_pt<100){
	  unc = 0.9492 -2.0757 * jet_jvt + 1.13328 * jet_jvt * jet_jvt;
	}else if(jet_pt<150){
	  unc = 0.7888 -1.8372 * jet_jvt + 1.05539 * jet_jvt * jet_jvt;
	}
      }else if(jet_eta<2.6){
	if(jet_pt<30){
	  if(jet_jvt<0.11)      unc = 0.2633 + 0.0091 * avgmu + -0.00009 * avgmu * avgmu;
	  else if(jet_jvt<0.25) unc = 0.1841 + 0.0144 * avgmu + -0.00008 * avgmu * avgmu;
	  else if(jet_jvt<0.85) unc = 0.1401 + 0.0048 * avgmu + 0.00006 * avgmu * avgmu ;
	  else if(jet_jvt<0.95) unc = -0.0118 + 0.0076 * avgmu + 0.00003 * avgmu * avgmu;
	  else                  unc = 0.0534 + -0.0011 * avgmu + 0.00010 * avgmu * avgmu;
	}else if(jet_pt<40){
	  if(jet_jvt<0.11)      unc = 0.1497 + 0.0133 * avgmu + -0.00015 * avgmu * avgmu  ;
	  else if(jet_jvt<0.25) unc = -0.2260 + 0.0276 * avgmu + -0.00021 * avgmu * avgmu ;
	  else if(jet_jvt<0.85) unc = 0.2743 + -0.0093 * avgmu + 0.00022 * avgmu * avgmu  ;
	  else if(jet_jvt<0.95) unc = 0.0604 + 0.0006 * avgmu + 0.00006 * avgmu * avgmu   ;
	  else                  unc = 0.0478 + -0.0009 * avgmu + 0.00004 * avgmu * avgmu  ;
	}else if(jet_pt<50){
	  if(jet_jvt<0.11)      unc = -0.2187 + 0.0317 * avgmu + -0.00037 * avgmu * avgmu ;
	  else if(jet_jvt<0.25) unc = 0.0964 + 0.0053 * avgmu + 0.00002 * avgmu * avgmu   ;
	  else if(jet_jvt<0.85) unc = 1.1730 + -0.0624 * avgmu + 0.00088 * avgmu * avgmu  ;
	  else if(jet_jvt<0.95) unc = -0.2011 + 0.0151 * avgmu + -0.00018 * avgmu * avgmu ;
	  else                  unc = 0.0145 + -0.0003 * avgmu + 0.00002 * avgmu * avgmu  ;
	}else if(jet_pt<60){
	  if(jet_jvt<0.11)      unc = 0.0051 + 0.0113 * avgmu + -0.00008 * avgmu * avgmu  ;
	  else if(jet_jvt<0.25) unc = -0.1024 + 0.0109 * avgmu + -0.00006 * avgmu * avgmu ;
	  else if(jet_jvt<0.85) unc = 1.2491 + -0.0501 * avgmu + 0.00052 * avgmu * avgmu  ;
	  else                  unc = 0.0267 + -0.0014 * avgmu + 0.00003 * avgmu * avgmu  ;
	}else if(jet_pt<100){
	  unc = 0.8951 -2.4995 * jet_jvt + 1.63229 * jet_jvt * jet_jvt;
	}else if(jet_pt<150){
	  unc = 0.9998 -1.7319 * jet_jvt + 0.72680 * jet_jvt * jet_jvt;
	}
      }else if(jet_eta<2.7){
	if(jet_pt<30){
	  if(jet_jvt<0.11)      unc = 0.3001 + 0.0054 * avgmu -0.00004 * avgmu * avgmu  ;
	  else if(jet_jvt<0.25) unc = 0.0663 + 0.0198 * avgmu -0.00013 * avgmu * avgmu  ;
	  else if(jet_jvt<0.85) unc = -0.0842 + 0.0163 * avgmu -0.00008 * avgmu * avgmu ;
	  else if(jet_jvt<0.95) unc = -0.0219 + 0.0080 * avgmu + 0.00003 * avgmu * avgmu;
	  else                  unc = 0.0461 -0.0003 * avgmu + 0.00012 * avgmu * avgmu  ;
	}else if(jet_pt<40){
	  if(jet_jvt<0.11)      unc = 0.1885 + 0.0083 * avgmu -0.00006 * avgmu * avgmu ;
	  else if(jet_jvt<0.25) unc = -0.0286 + 0.0150 * avgmu -0.00007 * avgmu * avgmu;
	  else if(jet_jvt<0.85) unc = 0.0152 + 0.0028 * avgmu + 0.00005 * avgmu * avgmu;
	  else if(jet_jvt<0.95) unc = 0.1815 -0.0076 * avgmu + 0.00018 * avgmu * avgmu ;
	  else                  unc = 0.0192 -0.0003 * avgmu + 0.00007 * avgmu * avgmu ;
	}else if(jet_pt<50){
	  if(jet_jvt<0.11)      unc = 0.1257 + 0.0074 * avgmu -0.00004 * avgmu * avgmu  ;
	  else if(jet_jvt<0.25) unc = -0.0276 + 0.0080 * avgmu + 0.00000 * avgmu * avgmu;
	  else if(jet_jvt<0.85) unc = 0.1403 -0.0051 * avgmu + 0.00009 * avgmu * avgmu  ;
	  else if(jet_jvt<0.95) unc = 0.2078 -0.0101 * avgmu + 0.00017 * avgmu * avgmu  ;
	  else                  unc = 0.2597 -0.0132 * avgmu + 0.00020 * avgmu * avgmu  ;
	}else if(jet_pt<60){
	  if(jet_jvt<0.11)      unc = 0.1111 + 0.0045 * avgmu -0.00000 * avgmu * avgmu ;
	  else if(jet_jvt<0.25) unc = 0.0975 -0.0011 * avgmu + 0.00008 * avgmu * avgmu ;
	  else if(jet_jvt<0.85) unc = 0.0920 -0.0053 * avgmu + 0.00013 * avgmu * avgmu ;
	  else                  unc = -0.0071 + 0.0016 * avgmu -0.00001 * avgmu * avgmu;
	}else if(jet_pt<100){
	  unc = 0.4660 -1.2116 * jet_jvt + 0.78807 * jet_jvt * jet_jvt;
	}else if(jet_pt<150){
	  unc = 0.2254 -0.5476 * jet_jvt + 0.32617 * jet_jvt * jet_jvt;
	}
      }// end eta 2.7
      else{//forward jets
	float fjvt = jet_fjvt>0.6 ? 0.6 : jet_fjvt; // the pileup more or less plateaus at 0.6
	if(jet_pt<30)       unc = 0.5106 + 1.2566 * fjvt -1.15060  * fjvt * fjvt;
	else if(jet_pt<40)  unc = 0.2972 + 1.9418 * fjvt -1.82694  * fjvt * fjvt;
	else if(jet_pt<50)  unc = 0.1543 + 1.9864 * fjvt -1.48429  * fjvt * fjvt;
	else if(jet_pt<60)  unc = 0.1050 + 1.3196 * fjvt + 0.03554 * fjvt * fjvt;
	else if(jet_pt<120) unc = 0.0400 + 0.5653 * fjvt + 1.96323 * fjvt * fjvt;
	// max of 0.9 seems reasonable
	if(jet_fjvt>0.6) unc = 0.9;
      }
      // end emtopo
    }else{//p-flow inputs
      if(jet_eta<2.4){
	if(jet_pt<30){
	  if(jet_jvt<0.11)      unc = 1;
	  else if(jet_jvt<0.25) unc = 0.2494 + 0.0076 * avgmu -0.00001 * avgmu * avgmu ;
	  else if(jet_jvt<0.85) unc = 0.0626 + 0.0037 * avgmu + 0.00004 * avgmu * avgmu;
	  else if(jet_jvt<0.95) unc = 0.0192 + 0.0017 * avgmu + 0.00005 * avgmu * avgmu;
	  else                  unc = 0.0147 -0.0003 * avgmu + 0.00004 * avgmu * avgmu ;
	}else if(jet_pt<40){
	  if(jet_jvt<0.11)      unc = 1;
	  else if(jet_jvt<0.25) unc = 0.1979 + 0.0034 * avgmu + 0.00003 * avgmu * avgmu;
	  else if(jet_jvt<0.85) unc = 0.0731 -0.0022 * avgmu + 0.00009 * avgmu * avgmu ;
	  else if(jet_jvt<0.95) unc = 0.0281 -0.0012 * avgmu + 0.00006 * avgmu * avgmu ;
	  else                  unc = 0.0086 -0.0003 * avgmu + 0.00002 * avgmu * avgmu ;
	}else if(jet_pt<50){
	  if(jet_jvt<0.11)      unc = 1;
	  else if(jet_jvt<0.25) unc = 0.2242 -0.0010 * avgmu + 0.00006 * avgmu * avgmu  ;
	  else if(jet_jvt<0.85) unc = 0.0568 -0.0019 * avgmu + 0.00006 * avgmu * avgmu  ;
	  else if(jet_jvt<0.95) unc = -0.0050 + 0.0008 * avgmu + 0.00001 * avgmu * avgmu;
	  else                  unc = 0.0037 -0.0000 * avgmu + 0.00000 * avgmu * avgmu  ;
	}else if(jet_pt<60){
	  if(jet_jvt<0.11)      unc = 1;
	  else if(jet_jvt<0.25) unc = 0.0027 + 0.0058 * avgmu -0.00001 * avgmu * avgmu  ;
	  else if(jet_jvt<0.85) unc = -0.0143 + 0.0008 * avgmu + 0.00001 * avgmu * avgmu;
	  else                  unc = -0.0012 + 0.0001 * avgmu + 0.00000 * avgmu * avgmu;
	}else if(jet_pt<100){
	  unc = 0.8558 -1.8519 * jet_jvt + 1.00208 * jet_jvt * jet_jvt;
	}else if(jet_pt<150){
	  unc = 0.6474 -1.4491 * jet_jvt + 0.80591 * jet_jvt * jet_jvt;
	}
      }else if(jet_eta<2.6){
	if(jet_pt<30){
	  if(jet_jvt<0.11)      unc = 0.2633 + 0.0091 * avgmu + -0.00009 * avgmu * avgmu;
	  else if(jet_jvt<0.25) unc = 0.1841 + 0.0144 * avgmu + -0.00008 * avgmu * avgmu;
	  else if(jet_jvt<0.85) unc = 0.1401 + 0.0048 * avgmu + 0.00006 * avgmu * avgmu ;
	  else if(jet_jvt<0.95) unc = -0.0118 + 0.0076 * avgmu + 0.00003 * avgmu * avgmu;
	  else                  unc = 0.0534 + -0.0011 * avgmu + 0.00010 * avgmu * avgmu;
	}else if(jet_pt<40){
	  if(jet_jvt<0.11)      unc = 0.1497 + 0.0133 * avgmu + -0.00015 * avgmu * avgmu  ;
	  else if(jet_jvt<0.25) unc = -0.2260 + 0.0276 * avgmu + -0.00021 * avgmu * avgmu ;
	  else if(jet_jvt<0.85) unc = 0.2743 + -0.0093 * avgmu + 0.00022 * avgmu * avgmu  ;
	  else if(jet_jvt<0.95) unc = 0.0604 + 0.0006 * avgmu + 0.00006 * avgmu * avgmu   ;
	  else                  unc = 0.0478 + -0.0009 * avgmu + 0.00004 * avgmu * avgmu  ;
	}else if(jet_pt<50){
	  if(jet_jvt<0.11)      unc = -0.2187 + 0.0317 * avgmu + -0.00037 * avgmu * avgmu ;
	  else if(jet_jvt<0.25) unc = 0.0964 + 0.0053 * avgmu + 0.00002 * avgmu * avgmu   ;
	  else if(jet_jvt<0.85) unc = 1.1730 + -0.0624 * avgmu + 0.00088 * avgmu * avgmu  ;
	  else if(jet_jvt<0.95) unc = -0.2011 + 0.0151 * avgmu + -0.00018 * avgmu * avgmu ;
	  else                  unc = 0.0145 + -0.0003 * avgmu + 0.00002 * avgmu * avgmu  ;
	}else if(jet_pt<60){
	  if(jet_jvt<0.11)      unc = 0.0051 + 0.0113 * avgmu + -0.00008 * avgmu * avgmu  ;
	  else if(jet_jvt<0.25) unc = -0.1024 + 0.0109 * avgmu + -0.00006 * avgmu * avgmu ;
	  else if(jet_jvt<0.85) unc = 1.2491 + -0.0501 * avgmu + 0.00052 * avgmu * avgmu  ;
	  else                  unc = 0.0267 + -0.0014 * avgmu + 0.00003 * avgmu * avgmu  ;
	}else if(jet_pt<100){
	  unc = 0.8802 -1.6233 * jet_jvt + 0.74604 * jet_jvt * jet_jvt;
	}else if(jet_pt<150){
	  unc = 0.9762 -2.4160 * jet_jvt + 1.45763 * jet_jvt * jet_jvt;
	}
      }else if(jet_eta<2.7){
	if(jet_pt<30){
	  if(jet_jvt<0.11)      unc = 0.2877 + 0.0056 * avgmu -0.00004 * avgmu * avgmu;
	  else if(jet_jvt<0.25) unc = 0.0353 + 0.0196 * avgmu -0.00012 * avgmu * avgmu;
	  else if(jet_jvt<0.85) unc = -0.1616 + 0.0188 * avgmu -0.00009 * avgmu * avgmu;
	  else if(jet_jvt<0.95) unc =  0.0373 + 0.0048 * avgmu + 0.00006 * avgmu * avgmu;
	  else                  unc = 0.0666 -0.0007 * avgmu + 0.00013 * avgmu * avgmu;
	}else if(jet_pt<40){
	  if(jet_jvt<0.11)      unc = 0.1331 + 0.0098 * avgmu -0.00007 * avgmu * avgmu;
	  else if(jet_jvt<0.25) unc = 0.0570 + 0.0096 * avgmu  -0.00000 * avgmu * avgmu;
	  else if(jet_jvt<0.85) unc = 0.2338  -0.0094 * avgmu + 0.00019 * avgmu * avgmu;
	  else if(jet_jvt<0.95) unc = 0.2930  -0.0127 * avgmu + 0.00023 * avgmu * avgmu;
	  else                  unc = 0.0152  -0.0003 * avgmu + 0.00007 * avgmu * avgmu;
	}else if(jet_pt<50){
	  if(jet_jvt<0.11)      unc = 0.1582 + 0.0060 * avgmu  -0.00003 * avgmu * avgmu;
	  else if(jet_jvt<0.25) unc = -0.0079 + 0.0057 * avgmu + 0.00003 * avgmu * avgmu;
	  else if(jet_jvt<0.85) unc = 0.1865  -0.0081 * avgmu + 0.00013 * avgmu * avgmu;
	  else if(jet_jvt<0.95) unc = 0.9103  -0.0405 * avgmu + 0.00049 * avgmu * avgmu;
	  else                  unc = 0.1183  -0.0048 * avgmu + 0.00009 * avgmu * avgmu;
	}else if(jet_pt<60){
	  if(jet_jvt<0.11)      unc = 0.0859 + 0.0047 * avgmu  -0.00000 * avgmu * avgmu;
	  else if(jet_jvt<0.25) unc = 0.0249 + 0.0027 * avgmu + 0.00004 * avgmu * avgmu;
	  else if(jet_jvt<0.85) unc = 0.1865  -0.0087 * avgmu + 0.00012 * avgmu * avgmu;
	  else                  unc = 0.2069  -0.0087 * avgmu + 0.00011 * avgmu * avgmu;
	}else if(jet_pt<100){
	  unc = 0.4281 -1.1109 * jet_jvt + 0.71551 * jet_jvt * jet_jvt;
	}else if(jet_pt<150){
	  unc = 0.2033 -0.5162 * jet_jvt + 0.33810 * jet_jvt * jet_jvt;
	}
      }// end eta 2.7
      else{//forward jets
	float fjvt = jet_fjvt>0.6 ? 0.6 : jet_fjvt; // the pileup more or less plateaus at 0.6
	if(jet_pt<30)       unc = 0.5295 + 1.2467 * fjvt -1.13946  * fjvt * fjvt;
	else if(jet_pt<40)  unc = 0.3118 + 1.9951 * fjvt -1.86882  * fjvt * fjvt;
	else if(jet_pt<50)  unc = 0.1347 + 2.3884 * fjvt -1.96891  * fjvt * fjvt;
	else if(jet_pt<60)  unc = 0.0872 + 1.5718 * fjvt + 0.02135 * fjvt * fjvt;
	else if(jet_pt<120) unc = 0.0303 + 0.8560 * fjvt + 1.89537 * fjvt * fjvt;
	// max of 0.9 seems reasonable
	if(jet_fjvt>0.6) unc = 0.9;
      }
    }// end pflow

    unc = std::min(unc, 1.0);
    unc = std::max(unc, 0.0);

    return unc;
  }

}
