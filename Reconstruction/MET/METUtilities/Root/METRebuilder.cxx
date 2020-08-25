///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METRebuilder.cxx
// Implementation file for class METRebuilder
// Author: T.J.Khoo<khoo@cern.ch>
///////////////////////////////////////////////////////////////////

// METUtilities includes
#include "METUtilities/METRebuilder.h"

// MET EDM
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETComponentMap.h"

// EDM includes
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauxAODHelpers.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetAttributes.h"

// Egamma EDM
#include "xAODEgamma/ElectronxAODHelpers.h"

// Tracking EDM
#include "xAODTracking/TrackParticle.h"
#ifdef ROOTCORE
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
#endif

// Track errors
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/WriteHandle.h"

#ifndef ROOTCORE
#include "GaudiKernel/IJobOptionsSvc.h"
#endif

namespace met {

  // Set up accessors to original object links in case of corrected copy containers
  const static SG::AuxElement::Accessor<obj_link_t> objLinkAcc("originalObjectLink");

  using std::vector;

  using namespace xAOD;

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

  // Constructors
  ////////////////
  METRebuilder::METRebuilder(const std::string& name) :
  AsgTool(name),
  m_METMapKey(""),
  m_ElectronContainerKey(""),
  m_PhotonContainerKey(""),
  m_TauJetContainerKey(""),
  m_MuonContainerKey(""),
  m_JetContainerKey(""),
  m_PVKey(""),
  m_doEle(false),
  m_doGamma(false),
  m_doTau(false),
  m_doMuon(false),
  m_rebuildEle(false),
  m_rebuildGamma(false),
  m_rebuildTau(false),
  m_rebuildMuon(false),
  m_doTracks(true),
  m_pureTrkSoft(true),
  m_trkUsedDec("usedByMET"),
  m_trkseltool("")
  {
    //
    // Property declaration
    //
    declareProperty( "EleColl",         m_eleColl    = "ElectronCollection" );
    declareProperty( "GammaColl",       m_gammaColl  = "PhotonCollection"   );
    declareProperty( "TauColl",         m_tauColl    = "TauRecContainer"    );
    declareProperty( "JetColl",         m_jetColl    = "AntiKt4LCTopoJets"  );
    declareProperty( "MuonColl",        m_muonColl   = "Muons"              );
    //
    declareProperty( "EleTerm",         m_eleTerm    = "RefEle"             );
    declareProperty( "GammaTerm",       m_gammaTerm  = "RefGamma"           );
    declareProperty( "TauTerm",         m_tauTerm    = "RefTau"             );
    declareProperty( "JetTerm",         m_jetTerm    = "RefJet"             );
    declareProperty( "MuonTerm",        m_muonTerm   = "Muons"              );
    declareProperty( "SoftTerm",        m_softTerm   = ""                   );
    //
    declareProperty( "InputMap",        m_inputMap   = "METMap_RefFinal"    );
    declareProperty( "OutputContainer", m_outMETCont = "MET_MyRefFinal"     );
    declareProperty( "OutputTotal",     m_outMETTerm = "Final"              );
    declareProperty( "WarnIfDuplicate", m_warnOfDupes = true                );
    //
    // for partial autoconfiguration
    declareProperty( "SoftTermType",    m_softTermType = "TrackSoftTerm"    );
    // or "ClusterSoftTerm" or "Reference"
    //
    declareProperty( "CalibJetPtCut",   m_jetPtCut   = 20e3                 );
    declareProperty( "DoJetJVFCut",     m_jetDoJvf   = true                 );
    declareProperty( "CalibJetJvfCut",  m_jetJvfCut  = 0.25                 );
    declareProperty( "SoftJetScale",    m_softJetScale = ""                 );
    //
    declareProperty( "DoTrackPVSel",    m_trk_doPVsel = true                );
    declareProperty( "VertexColl",      m_vtxColl    = "PrimaryVertices"    );
    declareProperty( "ClusterColl",     m_clusColl   = "CaloCalTopoCluster" );
    declareProperty( "TrkSelTool",      m_trkseltool                        );
    //
    declareProperty( "ComputeSTVF",     m_doSTVF     = false                );

  }

  // Destructor
  ///////////////
  METRebuilder::~METRebuilder()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METRebuilder::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    if( m_inputMap.size()==0 ) {
      ATH_MSG_FATAL("Input MissingETComponentMap name must be provided.");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ("Input MET Map: " << m_inputMap);

    if( m_outMETCont.size()==0 ) {
      ATH_MSG_FATAL("Output MissingETContainer name must be provided.");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ("Output MET Container: " << m_outMETCont);

    ATH_MSG_INFO ("Configured to rebuild following MET Terms:");
    if( m_eleTerm!="" ) {
      m_doEle = true;
      if( m_eleColl!="" ) {
        m_rebuildEle = true;
        ATH_MSG_INFO("  Electrons: " << m_eleColl
                     << " > " << m_eleTerm );
      }
    }
    if( m_gammaTerm!="" ) {
      m_doGamma = true;
      if( m_gammaColl!="" ) {
        m_rebuildGamma = true;
        ATH_MSG_INFO("  Photons:   " << m_gammaColl
                     << " > " << m_gammaTerm );
      }
    }
    if( m_tauTerm!="" ) {
      m_doTau = true;
      if( m_tauColl!="" ) {
        m_rebuildTau = true;
        ATH_MSG_INFO("  Taus:      " << m_tauColl
                     << " > " << m_tauTerm );
      }
    }
    if( m_muonTerm!="" ) {
      m_doMuon = true;
      if( m_muonColl!="" ) {
        m_rebuildMuon = true;
        ATH_MSG_INFO("  Muons:     " << m_muonColl
                     << " > " << m_muonTerm );
      }
    }
    if( m_jetColl!="" && m_jetTerm!="") {
      ATH_MSG_INFO("  Jets:      " << m_jetColl
                   << " > " << m_jetTerm );
    } else {
      ATH_MSG_FATAL("Error in configuration -- jet input and term keys must both be specified.");
      return StatusCode::FAILURE;
    }
    if(m_softTerm.empty()) {
      if(m_softTermType=="TrackSoftTerm") {
        m_softTerm = "PVSoftTrk";
        m_jetDoJvf = true;
        m_softJetScale = "JetTrackScale";
      } else if(m_softTermType=="ClusterSoftTerm") {
        m_softTerm = "SoftClus";
        m_jetDoJvf = false;
        m_softJetScale = "JetConstitScaleMomentum";
      }
      if(m_softTermType=="Reference") {
        m_softTerm = "SoftClus";
        m_jetDoJvf = false;
        m_softJetScale = "JetConstitScaleMomentum";
        m_jetPtCut = 0.;
      }
    }
    ATH_MSG_INFO ("  Soft:      " << m_softTerm);

    m_trkUsedDec = SG::AuxElement::Decorator<char>("usedBy"+m_outMETCont);
    m_pureTrkSoft = (m_softJetScale == "JetTrackScale");

    if(m_doTracks) {
      if(m_trkseltool.empty()) {
#ifdef ROOTCORE
	InDet::InDetTrackSelectionTool* trkSelTool = new InDet::InDetTrackSelectionTool("IDTrkSel_MET");
	ATH_CHECK( trkSelTool->setProperty("maxZ0SinTheta",1.5) );
	ATH_CHECK( trkSelTool->setProperty("maxD0overSigmaD0",3.) );
	ATH_CHECK( trkSelTool->setProperty("CutLevel", "TightPrimary") );
	ATH_CHECK( trkSelTool->initialize() );
	m_trkseltool = ToolHandle<InDet::IInDetTrackSelectionTool>(trkSelTool);
#else
	ServiceHandle<IJobOptionsSvc> josvc("JobOptionsSvc",name());
	std::string toolName = "IDTrkSel_METUtil";
	ATH_MSG_INFO("METRebuilder: Autoconfiguring " << toolName);
	m_trkseltool.setTypeAndName("InDet::InDetTrackSelectionTool/"+toolName);
	std::string fullToolName = "ToolSvc."+toolName;
	ATH_CHECK( josvc->addPropertyToCatalogue(fullToolName,FloatProperty("maxZ0SinTheta",1.5)) );
	ATH_CHECK( josvc->addPropertyToCatalogue(fullToolName,FloatProperty("maxD0overSigmaD0",1.5)) );
	ATH_CHECK( josvc->addPropertyToCatalogue(fullToolName,StringProperty("CutLevel","TightPrimary")) );
#endif
      }
      ATH_CHECK( m_trkseltool.retrieve() );
    }

    // ReadHandleKey(s)
    ATH_CHECK( m_METMapKey.assign(m_inputMap) );
    ATH_CHECK( m_METMapKey.initialize() );
    ATH_CHECK( m_ElectronContainerKey.assign(m_eleColl) );
    ATH_CHECK( m_ElectronContainerKey.initialize() );
    ATH_CHECK( m_PhotonContainerKey.assign(m_gammaColl) );
    ATH_CHECK( m_PhotonContainerKey.initialize() );
    ATH_CHECK( m_TauJetContainerKey.assign(m_tauColl) );
    ATH_CHECK( m_TauJetContainerKey.initialize() );
    ATH_CHECK( m_MuonContainerKey.assign(m_muonColl) );
    ATH_CHECK( m_MuonContainerKey.initialize() );
    ATH_CHECK( m_JetContainerKey.assign(m_jetColl) );
    ATH_CHECK( m_JetContainerKey.initialize() );
    ATH_CHECK( m_PVKey.assign(m_vtxColl) );
    ATH_CHECK( m_PVKey.initialize() );
    // WriteHandleKey(s)
    ATH_CHECK( m_OutMETKey.assign(m_outMETCont) );
    ATH_CHECK( m_OutMETKey.initialize() );

    return StatusCode::SUCCESS;
  }

  StatusCode METRebuilder::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  StatusCode METRebuilder::execute()
  {
    ATH_MSG_DEBUG ( name() << " in execute...");

    SG::ReadHandle<xAOD::MissingETComponentMap> METMap(m_METMapKey);
    if (!METMap.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve MissingETComponentMap: " << METMap.key());
      return StatusCode::SUCCESS;
    }

    SG::ReadHandle<xAOD::MissingETContainer> MET(m_METContainerKey);
    if (!MET.isPresent()) {
      if(m_warnOfDupes){
        ATH_MSG_WARNING("MET container " << MET.key() << " already in StoreGate"); }      
      return StatusCode::SUCCESS;
    }

    // Create a MissingETContainer with its aux store
    SG::WriteHandle<xAOD::MissingETContainer> OutMET(m_OutMETKey);
    std::unique_ptr<MissingETContainer> outCont = std::make_unique<xAOD::MissingETContainer>();
    std::unique_ptr<MissingETAuxContainer> metAuxCont = std::make_unique<xAOD::MissingETAuxContainer>();
    ATH_CHECK( OutMET.record(std::move(outCont),std::move(metAuxCont) ) );

    if(m_doEle) {
      if(m_rebuildEle) {
        SG::ReadHandle<xAOD::ElectronContainer> Electrons(m_ElectronContainerKey);
        if (!Electrons.isValid()) {
          ATH_MSG_WARNING("Unable to retrieve ElectronContainer: " << Electrons.key());
          return StatusCode::SUCCESS;
        }
        ATH_CHECK( rebuildMET(m_eleTerm, outCont.get(), Electrons.get(), METMap.get(), m_doTracks) );
      } else {
        ATH_CHECK( copyMET(m_eleTerm,outCont.get(),METMap.get()) );
      }
    }

    if(m_doGamma) {
      if(m_rebuildGamma) {
        SG::ReadHandle<xAOD::PhotonContainer> Gamma(m_PhotonContainerKey);
        if (!Gamma.isValid()) {
          ATH_MSG_WARNING("Unable to retrieve GammaContainer: " << Gamma.key());
          return StatusCode::FAILURE;
        }
        ATH_CHECK( rebuildMET(m_gammaTerm, outCont.get(), Gamma.get(), METMap.get(), m_doTracks) );
      } else {
        ATH_CHECK( copyMET(m_gammaTerm,outCont.get(),METMap.get()) );
      }
    }

    if(m_doTau) {
      if(m_rebuildTau) {
        SG::ReadHandle<xAOD::TauJetContainer> TauJets(m_TauJetContainerKey);
        if (!TauJets.isValid()) {
          ATH_MSG_WARNING("Unable to retrieve TauJetContainer: " << TauJets.key());
          return StatusCode::FAILURE;
        }
        ATH_CHECK( rebuildMET(m_tauTerm, outCont.get(), TauJets.get(), METMap.get(), m_doTracks) );
      } else {
        ATH_CHECK( copyMET(m_tauTerm,outCont.get(),METMap.get()) );
      }
    }


    if(m_doMuon) {
      if(m_rebuildMuon) {
        // May need implementation of Eloss correction
        // Place in separate tool (?)
        SG::ReadHandle<xAOD::MuonContainer> Muons(m_MuonContainerKey);
        if (!Muons.isValid()) {
          ATH_MSG_WARNING("Unable to retrieve MuonContainer: "  << Muons.key());
          return StatusCode::FAILURE;
        }
        ATH_CHECK( rebuildMET(m_muonTerm, outCont.get(), Muons.get(), METMap.get(), m_doTracks) );
      } else {
        ATH_CHECK( copyMET(m_muonTerm,outCont.get(),METMap.get()) );
      }
    }


    // Implementation of the jet/soft term rebuilding
    // Place in separate tool (?)
    SG::ReadHandle<xAOD::JetContainer> Jets(m_JetContainerKey);
    if (!Jets.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve JetContainer: " << Jets.key());
      return StatusCode::FAILURE;
    }
    ATH_CHECK( rebuildJetMET(m_jetTerm, m_softTerm, outCont.get(), Jets.get(), METMap.get(), m_doTracks) );
    ATH_CHECK( buildMETSum(m_outMETTerm, outCont.get()) );

    return StatusCode::SUCCESS;
  }

  StatusCode METRebuilder::copyMET(const std::string& metKey,
                                   xAOD::MissingETContainer* metCont,
                                   const xAOD::MissingETComponentMap* metMap) {

    MissingET* metterm = new MissingET();
    const MissingET* metterm_ref = MissingETComposition::getMissingET(metMap,metKey);
    metCont->push_back(metterm);
    *metterm = *metterm_ref;

    return StatusCode::SUCCESS;
  }

  // **** Rebuild generic MET term ****

  StatusCode METRebuilder::rebuildMET(const std::string& metKey,
                                      xAOD::MissingETContainer* metCont,
                                      const xAOD::IParticleContainer* collection,
                                      const xAOD::MissingETComponentMap* metMap,
                                      bool doTracks) {
    ATH_MSG_DEBUG("Rebuild MET term: " << metKey);
    const MissingETComponent* component = MissingETComposition::getComponent(metMap,metKey);
    if(!component) {
      ATH_MSG_WARNING("Could not find METComponent for " << metKey << " in MET Map!");
      return StatusCode::FAILURE;
    }
    MissingET* met = nullptr; //= new MissingET(0.,0.,0.,metKey,component->metObject()->source());
    if(fillMET(met, metCont, metKey, component->metObject()->source()) != StatusCode::SUCCESS){
      ATH_MSG_ERROR("failed to fill MET term");
      return StatusCode::FAILURE;
    }
    //    metCont->push_back(met);
    return rebuildMET(met,collection,component,doTracks);
  }

  StatusCode METRebuilder::rebuildMET(xAOD::MissingET* met,
                                      const xAOD::IParticleContainer* collection,
                                      const xAOD::MissingETComponent* component,
                                      bool doTracks) {

    if(component->size()==0) return StatusCode::SUCCESS;

    ATH_MSG_VERBOSE("Rebuilding MET term " << component->metObject()->name());

    const IParticleContainer* testCollection = dynamic_cast<const IParticleContainer*>(component->objects().front()->container());
    bool originalInputs = (testCollection == collection);
    bool matchCollection = true;
    if(collection->size()>0) {
      // Consistency test: check that the collection supplied is the original one
      // used for MET reconstruction, or is derived from this collection
      if(!originalInputs) {
        const IParticle* pObj = collection->front();
        if(!objLinkAcc.isAvailable(*pObj)) {
          ATH_MSG_WARNING("Modified container provided without originalObjectLink -- cannot proceed.");
          matchCollection = false;
        } else {
          const IParticleContainer* sourceCollection = dynamic_cast<const IParticleContainer*>((*objLinkAcc(*pObj))->container());
          matchCollection = (sourceCollection == testCollection);
        }
      }
      if(!matchCollection) {
        ATH_MSG_WARNING("Problem with input object container -- skipping this term.");
        return StatusCode::SUCCESS;
      }
    }

    // Method flow:
    // 1. Loop over the objects in the collection
    // 2. Find them or their originals in the METComponent
    // 3. Add to the MET term with appropriate weights

    for( IParticleContainer::const_iterator iObj=collection->begin();
        iObj!=collection->end(); ++iObj ) {

      const IParticle* pObj = *iObj;
      // check if this is a copy - if so, get the original object pointer
      if(!originalInputs) pObj = *objLinkAcc(*pObj);

      if(component->findIndex(pObj) != MissingETBase::Numerical::invalidIndex()) {
        MissingETBase::Types::weight_t objWeight = component->weight(pObj);
        ATH_MSG_VERBOSE( "Object with pt " << (*iObj)->pt() << " has weight " << objWeight.wet() );

        if(doTracks) {
          associateTracks(*iObj);
        }

        met->add((*iObj)->pt()*cos((*iObj)->phi())*objWeight.wpx(),
                 (*iObj)->pt()*sin((*iObj)->phi())*objWeight.wpy(),
                 (*iObj)->pt()*objWeight.wet());
      } // used object in MET
      else {
        ATH_MSG_VERBOSE( "Object with pt " << (*iObj)->pt() << " not found." );
      }
    }

    ATH_MSG_DEBUG( "Original " << component->metObject()->name() << " MET --"
                  << " mpx: " << component->metObject()->mpx()
                  << " mpy: " << component->metObject()->mpy()
                  );
    ATH_MSG_DEBUG( "Rebuilt " << component->metObject()->name() << " MET  --"
                  << " mpx: " << met->mpx()
                  << " mpy: " << met->mpy()
                  );

    return StatusCode::SUCCESS;
  }

  // **** Rebuild jet & soft MET terms ****

  StatusCode METRebuilder::rebuildJetMET(const std::string& jetKey,
                                         const std::string& softKey,
                                         xAOD::MissingETContainer* metCont,
                                         const xAOD::JetContainer* jets,
                                         const xAOD::MissingETComponentMap* metMap,
                                         bool doTracks,
                                         bool doJvfCut,
                                         bool pureTrkSoft,
                                         const std::string& softJetScale) {
    float dummy(0);
    return rebuildJetMET(jetKey,softKey,metCont,jets,metMap,doTracks,
                         doJvfCut,pureTrkSoft,softJetScale,dummy);
  }

  StatusCode METRebuilder::rebuildJetMET(const std::string& jetKey,
                                         const std::string& softKey,
                                         xAOD::MissingETContainer* metCont,
                                         const xAOD::JetContainer* jets,
                                         const xAOD::MissingETComponentMap* metMap,
                                         bool doTracks,
                                         bool doJvfCut,
                                         bool pureTrkSoft,
                                         const std::string& softJetScale,
					 float& stvf) {
    const MissingETComponent* component = MissingETComposition::getComponent(metMap,jetKey);
    if(!component) {
      ATH_MSG_WARNING("Could not find METComponent for " << jetKey << " in MET Map!");
      return StatusCode::FAILURE;
    }
    MissingET* metJet = nullptr; // new MissingET(0.,0.,0.,jetKey,component->metObject()->source());
    if(fillMET(metJet, metCont, jetKey, component->metObject()->source()) != StatusCode::SUCCESS){
      ATH_MSG_ERROR("failed to fill MET term");
      return StatusCode::FAILURE;
    }
    //    metCont->push_back(metJet);
    ATH_CHECK( copyMET(softKey,metCont,metMap) );
    // copy constructor needs correcting.
    MissingET* metSoft = (*metCont)[softKey];
    const MissingETComponent* comp_softtrk(0);
    if(m_doSTVF) {
      comp_softtrk = MissingETComposition::getComponent(metMap,MissingETBase::Source::SoftEvent|MissingETBase::Source::idTrack());
      if(!comp_softtrk) {
	ATH_MSG_WARNING("Could not retrieve soft track component -- STVF calculation failed!");
	return StatusCode::FAILURE;
      }
    }
    return rebuildJetMET(metJet,metSoft,jets,component,doTracks,
                         doJvfCut,pureTrkSoft,softJetScale,
			 stvf,comp_softtrk);
  }

  StatusCode METRebuilder::rebuildJetMET(xAOD::MissingET* metJet,
                                         xAOD::MissingET* metSoft,
                                         const xAOD::JetContainer* jets,
                                         const xAOD::MissingETComponent* component,
                                         bool doTracks,
                                         bool doJvfCut,
                                         bool pureTrkSoft,
                                         const std::string& softJetScale,
					 float& stvf,
					 const xAOD::MissingETComponent* comp_softtrk) {

    if(component->size()==0) return StatusCode::SUCCESS;

    // const VertexContainer* vtxCont = 0;
    SG::ReadHandle<xAOD::VertexContainer> PV(m_PVKey);

    const Vertex* pv = 0;


    if(doJvfCut || (m_trk_doPVsel && doTracks)) {
      if (!PV.isValid()) {
        ATH_MSG_WARNING("Unable to retrieve input primary vertex container");

        return StatusCode::FAILURE;
      }

      for(const auto& vx : *PV) {
	if(vx->vertexType()==VxType::PriVtx)

          {pv = vx; break;}

      }
      if(!pv) {
        ATH_MSG_WARNING("Event has no primary vertex");
        return StatusCode::SUCCESS;
      } else{
        ATH_MSG_DEBUG("Main primary vertex has z = " << pv->z());
      }
    }


    stvf = 0.;
    float trksumpt_allsoft(0.);
    if(m_doSTVF) {
      if(!comp_softtrk) {
	ATH_MSG_WARNING("Could not retrieve soft track component -- STVF calculation failed!");
	return StatusCode::FAILURE;
      }
      trksumpt_allsoft += comp_softtrk->metObject()->sumet();
    }

    const IParticleContainer* testCollection = static_cast<const IParticleContainer*>(component->objects().front()->container());
    const IParticleContainer* collcast = static_cast<const IParticleContainer*>(jets);
    bool originalInputs = (testCollection == collcast);
    bool matchCollection = true;
    if(jets->size()>0) {
      // Consistency test: check that the collection supplied is the original one
      // used for MET reconstruction, or is derived from this collection
      if(!originalInputs) {
        const IParticle* pJet = jets->front();
        if(!objLinkAcc.isAvailable(*pJet)) {
          ATH_MSG_WARNING("Modified container provided without originalObjectLink -- cannot proceed.");
          matchCollection = false;
        } else {
          const IParticleContainer* sourceCollection = static_cast<const IParticleContainer*>((*objLinkAcc(*pJet))->container());
          matchCollection = (sourceCollection == testCollection);
        }
      }
    }
    if(!matchCollection) {
      ATH_MSG_WARNING("Problem with input object container -- skipping these terms.");
      return StatusCode::SUCCESS;
    }
    // 1. Loop over the jets in the collection
    // 2. Find them or their originals in the METComponent
    // 3. Add to the MET term with appropriate weights
    for( JetContainer::const_iterator iJet=jets->begin();
        iJet!=jets->end(); ++iJet ) {

      const xAOD::IParticle* pJet = *iJet;
      if(!originalInputs) pJet = *objLinkAcc(*pJet);

      if(component->findIndex(pJet) != MissingETBase::Numerical::invalidIndex()) {

        MissingETBase::Types::weight_t jetWeight = component->weight(pJet);
        bool passJVF = true;
        if(doJvfCut) {
          vector<float> jvf;
          (*iJet)->getAttribute<vector<float> >(xAOD::JetAttribute::JVF,jvf);
          passJVF = (*iJet)->pt()>50e3 || fabs((*iJet)->eta())>2.4 || fabs(jvf[pv->index()])>m_jetJvfCut;
          ATH_MSG_VERBOSE("Jet with pt " << (*iJet)->pt() << " has jvf = " << jvf[pv->index()]);
        }

        if((*iJet)->pt()>m_jetPtCut && passJVF) {

          ATH_MSG_VERBOSE("Retain jet with pt " << (*iJet)->pt() << " at full scale.");

          metJet->add((*iJet)->px()*jetWeight.wpx(),
                      (*iJet)->py()*jetWeight.wpy(),
                      (*iJet)->pt()*jetWeight.wet());
        } // minimum pt cut for jet calibration
        else {
          double trkjetpx(0), trkjetpy(0), trkjetpt(0);
          if(doTracks) {
            ATH_MSG_VERBOSE("Add tracks from jet with pt " << (*iJet)->pt());
            vector<const TrackParticle*> jettracks = (*iJet)->getAssociatedObjects<TrackParticle>(xAOD::JetAttribute::GhostTrack);
            ATH_MSG_VERBOSE("Got jet tracks");
            for(const auto& trk : jettracks) {
              if(!trk) continue;
              bool badTrack = false;
              if( (fabs((trk)->eta())<1.5 && (trk)->pt()>200e3) ||
                 (fabs((trk)->eta())>=1.5 && (trk)->pt()>120e3) ) {
                // Get relative error on qoverp
                float Rerr = Amg::error(trk->definingParametersCovMatrix(),4)/fabs(trk->qOverP());
                // Simplified cut -- remove tracks that are more energetic than the jet
                if(Rerr>0.4 || trk->pt()>2*(*iJet)->pt()) badTrack = true;
              } // additional cuts against high pt mismeasured tracks
              bool uniqueTrack = true;
              uniqueTrack = !m_trkUsedDec(*trk);
              if(!badTrack && uniqueTrack) {
		if(acceptTrack(trk,pv)) {
		  ATH_MSG_VERBOSE("  + track with pt " << trk->pt());
		  trkjetpx += trk->pt()*cos(trk->phi());
		  trkjetpy += trk->pt()*sin(trk->phi());
		  trkjetpt += trk->pt();
		} else {
		  ATH_MSG_VERBOSE("  - track failed badtrack/uniqueness/PV");
		  if(m_doSTVF && acceptTrack(trk,0)) {
		    trksumpt_allsoft += trk->pt();
		  } // STVF
		} // track selection
	      } // reject bad/duplicate tracks
            } // track loop
          } // track-based soft term
          if(pureTrkSoft){
            metSoft->add(trkjetpx,trkjetpy,trkjetpt);
          } else {
            // just add the weighted constituent-scale jet
            xAOD::JetFourMom_t jetP = (*iJet)->jetP4(softJetScale);
            ATH_MSG_VERBOSE("Soft jet pt = " << jetP.Pt() << ", track pt = " << trkjetpt);
            if(trkjetpt>jetP.Pt() || !passJVF) { // use tracks if higher scale than calo jet or fail JVF cut in central region
              ATH_MSG_VERBOSE("Add jet with pt " << (*iJet)->pt()
                              << " at track scale (pt = " << trkjetpt << ").");
              metSoft->add(trkjetpx,trkjetpy,trkjetpt);
            } else {
              ATH_MSG_VERBOSE("Add jet with pt " << (*iJet)->pt()
                              << " at constituent scale (pt = " << jetP.Pt() << ").");
              metSoft->add(jetP.Px()*jetWeight.wpx(),
                           jetP.Py()*jetWeight.wpy(),
                           jetP.Pt()*jetWeight.wet());
            } // otherwise, use calo jet at chosen scale
          } // cluster-based soft term
        } // jets below threshold should be added to the soft terms
      } // used jet in MET
    }

    ATH_MSG_DEBUG( "Original jet MET --"
                  << " mpx: " << component->metObject()->mpx()
                  << " mpy: " << component->metObject()->mpy()
                  );
    ATH_MSG_DEBUG( "Rebuilt jet MET  --"
                  << " mpx: " << metJet->mpx()
                  << " mpy: " << metJet->mpy()
                  );

    ATH_MSG_DEBUG( "Rebuilt MET soft --"
                  << " mpx: " << metSoft->mpx()
                  << " mpy: " << metSoft->mpy()
                  );

    if(m_doSTVF) stvf = metSoft->sumet()/trksumpt_allsoft;

    return StatusCode::SUCCESS;
  }

  // **** Sum up MET terms ****

  StatusCode METRebuilder::buildMETSum(const std::string& totalName,
                                       xAOD::MissingETContainer* metCont)
  {
    ATH_MSG_DEBUG("Build MET total: " << totalName);

    MissingET* metFinal = nullptr; // new MissingET(0.,0.,0.,"Final",MissingETBase::Source::total());
    if(fillMET(metFinal, metCont, totalName, MissingETBase::Source::total()) != StatusCode::SUCCESS){
      ATH_MSG_ERROR("failed to fill MET term");
      return StatusCode::FAILURE;
    }
    //    metCont->push_back(metFinal);

    for(MissingETContainer::const_iterator iMET=metCont->begin();
        iMET!=metCont->end(); ++iMET) {
      if(*iMET==metFinal) continue;
      *metFinal += **iMET;
    }

    ATH_MSG_DEBUG( "Rebuilt MET Final --"
                  << " mpx: " << metFinal->mpx()
                  << " mpy: " << metFinal->mpy()
                  );

    return StatusCode::SUCCESS;
  }

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Protected methods:
  ///////////////////////////////////////////////////////////////////

  // Implement for now, but should move to common tools when possible
  bool METRebuilder::acceptTrack(const xAOD::TrackParticle* trk,
                                 const xAOD::Vertex* pv) const
  {

    // if(trk->d0()>m_trk_d0Max) return false;
    // if(fabs(trk->z0()+trk->vz() - pv->z()) > m_trk_z0Max) return false;
    if(m_trk_doPVsel && pv) {
      return static_cast<bool>(m_trkseltool->accept( *trk, pv ));
    }
    else  {
      return static_cast<bool>(m_trkseltool->accept( trk ));
    }
  }

  void METRebuilder::associateTracks(const xAOD::IParticle* obj) {

    if(obj->type()==xAOD::Type::Electron) {
      const xAOD::Electron* el = static_cast<const xAOD::Electron*>(obj);
      for(size_t iTrk=0; iTrk<el->nTrackParticles(); ++iTrk) {
        const TrackParticle* eltrk = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(el->trackParticle(iTrk));
        if(eltrk) m_trkUsedDec(*eltrk) = true;
      }
    }
    if(obj->type()==xAOD::Type::Photon) {
      const xAOD::Photon* ph = static_cast<const xAOD::Photon*>(obj);
      for(size_t iVtx=0; iVtx<ph->nVertices(); ++iVtx) {
        const xAOD::Vertex* phvx = ph->vertex(iVtx);

        if(phvx) {
          for(size_t iTrk=0; iTrk<phvx->nTrackParticles(); ++iTrk) {
            const TrackParticle* phtrk = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(phvx->trackParticle(iTrk));
            if(phtrk) m_trkUsedDec(*phtrk) = true;
          }
        }
      }
    }
		if(obj->type()==xAOD::Type::Tau) {
			const xAOD::TauJet* tau = static_cast<const xAOD::TauJet*>(obj);
			//// now find associated tracks
			//const std::vector< ElementLink< xAOD::TrackParticleContainer > >& trackLinks = tau->trackLinks();
			//for(const auto& trklink : trackLinks) {
			//	if(trklink.isValid()) {m_trkUsedDec(**trklink) = true;}
			//	else { ATH_MSG_WARNING("Invalid tau track link"); }
			//}
			//const std::vector< ElementLink< xAOD::TrackParticleContainer > >& otherTrackLinks = tau->otherTrackLinks();
			//for(const auto& trklink : otherTrackLinks) {
			//	if(trklink.isValid()) {
			//		double dR = (*tau->jetLink())->p4().DeltaR((*trklink)->p4());
			//		if(dR<0.2) {
			//			m_trkUsedDec(**trklink) = true;
			//		}
			//	}	else { ATH_MSG_WARNING("Invalid tau track link"); }
			//}

            // TauJet_v3
            //all track links dR<0.2 no track quality
            std::vector<ElementLink< xAOD::TrackParticleContainer> > trackLinks = xAOD::TauHelpers::trackParticleLinks(tau, xAOD::TauJetParameters::coreTrack);
                for(const auto& trklink : trackLinks) {                                                                                                                   
                if(trklink.isValid()) {m_trkUsedDec(**trklink) = true;}
                else { ATH_MSG_WARNING("Invalid tau track link"); }
            }
		}
    if(obj->type()==xAOD::Type::Muon) {
      const xAOD::Muon* mu = static_cast<const xAOD::Muon*>(obj);
      if(mu->inDetTrackParticleLink().isValid()) {
        m_trkUsedDec(**mu->inDetTrackParticleLink()) = true;
      }
    }
  }

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////
  StatusCode METRebuilder::fillMET(xAOD::MissingET *& met,
				   xAOD::MissingETContainer * metCont,
				   const std::string& metKey,
				   const MissingETBase::Types::bitmask_t metSource){
    if(met){
      ATH_MSG_ERROR("You can't fill a filled MET value");
      return StatusCode::FAILURE;
    }

    met = new xAOD::MissingET;
    metCont->push_back(met);

    met->setName  (metKey);
    met->setSource(metSource);

    return StatusCode::SUCCESS;
}


} //> end namespace met
