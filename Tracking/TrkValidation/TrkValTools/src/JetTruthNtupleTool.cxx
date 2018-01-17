/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// JetTruthNtupleTool.cxx
//   Source file for class JetTruthNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//Gaudi
// #include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
// #include "GaudiKernel/PropertyMgr.h"

// Trk
#include "TrkValTools/JetTruthNtupleTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkValEvent/GenParticleJet.h"
// Truth
#include "TrkTruthData/TrackTruth.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "HepMC/GenParticle.h"

#include <TGraphAsymmErrors.h>
#include "TTree.h"

// constructor
Trk::JetTruthNtupleTool::JetTruthNtupleTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
        AthAlgTool(t,n,p),
        m_nt(nullptr),
        m_numberOfTreeEntries(0),
        m_runNumber{},
        m_eventNumber{},
        m_genParticleLinkIndex(nullptr),
        m_mc_trackToJetAngle(nullptr),
        m_mc_jetMultiplicity{},
        m_mc_jetSphericity{},
        m_mc_jetThrust{},
        m_mc_jetEnergy{},
        m_mc_jetMomentum{}
{
    declareInterface<IJetTruthNtupleTool>(this);

    // Declare the properties
    declareProperty("NtupleFileName",               m_ntupleFileName = "TRKVAL/Validation","Ntuple file handle");
    declareProperty("NtupleTreeName",               m_ntupleTreeName = "TruthJets",        "Name of the ntuple tree");
}

// destructor
Trk::JetTruthNtupleTool::~JetTruthNtupleTool() {}

// initialize
StatusCode Trk::JetTruthNtupleTool::initialize() {

    // ---------------------------
    // retrieve pointer to THistSvc
    ITHistSvc *tHistSvc;
    StatusCode sc =  service("THistSvc", tHistSvc);
    if (sc.isFailure()) {
      ATH_MSG_ERROR ( "Unable to retrieve pointer to THistSvc" );
      return sc;
    }
    // ---------------------------
    // create tree and register it to THistSvc
    m_nt = new TTree(TString(m_ntupleTreeName), "Track Validation JetTruth");
    // NB: we must not delete the tree, this is done by THistSvc
    std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleTreeName;
    sc = tHistSvc->regTree(fullNtupleName, m_nt);
    if (sc.isFailure()) {
      ATH_MSG_ERROR ( "Unable to register TTree : " << m_ntupleTreeName );
        return sc;
    }

    //-----------------
    // add items
    // event info:
    m_nt->Branch("RunNumber",           &m_runNumber                );            // Run number
    m_nt->Branch("EventNumber",         &m_eventNumber              );          // Event number

    m_nt->Branch( "truth_jetMultiplicity",    &m_mc_jetMultiplicity );
    m_nt->Branch( "truth_jetSphericity",      &m_mc_jetSphericity   );
    m_nt->Branch( "truth_jetThrust",          &m_mc_jetThrust       );
    m_nt->Branch( "truth_jetEnergy",          &m_mc_jetEnergy       );
    m_nt->Branch( "truth_jetMomentum",        &m_mc_jetMomentum     );

    m_genParticleLinkIndex    = new std::vector<unsigned int>();
    m_nt->Branch( "GenParticleLinkIndex",     &m_genParticleLinkIndex);// link to particles
    m_mc_trackToJetAngle      = new std::vector<float>();
    m_nt->Branch( "truth_trackToJetAngle",    &m_mc_trackToJetAngle); // truth_trackToJetAngle

    ATH_MSG_VERBOSE ( "added items to ntuple" );

    return StatusCode::SUCCESS;

}


///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode Trk::JetTruthNtupleTool::finalize() {

  ATH_MSG_INFO ( "finalize() in " << name() );
  delete m_genParticleLinkIndex;
  delete m_mc_trackToJetAngle;

  return StatusCode::SUCCESS;
}

//////////////////////////////////////
/// write track data to ntuple
//////////////////////////////////////
StatusCode Trk::JetTruthNtupleTool::writeJetTruthData (
                const std::vector< Trk::GenParticleJet >& jets,
                const int& nTruthRecordsAtCurrentEvent
                //                const std::vector<unsigned int>& /*particleToJetIndex*/ 
                ) const {

  StatusCode sc;

  ATH_MSG_VERBOSE ( "in writeJetTruthData(...)" );

  // ---------------------------------------
  // fill event data
  const EventInfo* eventInfo = nullptr;
  sc = evtStore()->retrieve(eventInfo);
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "Could not retrieve event info" );
  }
  const EventID* myEventID=eventInfo->event_ID();

  m_runNumber=myEventID->run_number();
  m_eventNumber=myEventID->event_number();

  for (std::vector<Trk::GenParticleJet>::const_iterator itJet = jets.begin();
       itJet < jets.end(); ++itJet ) {
    
    m_mc_jetMultiplicity = itJet->getNumParticles();
    m_mc_jetMomentum     = itJet->getMomentum().mag();
    m_mc_jetEnergy       = itJet->getEnergy();
    m_mc_jetSphericity   = itJet->getSphericity();
    m_mc_jetThrust       = itJet->getThrust();

    std::vector<const HepMC::GenParticle*> particles = itJet->getParticles();
    m_mc_trackToJetAngle->reserve(particles.size());
    m_genParticleLinkIndex->reserve(particles.size());
    std::vector<int> indices         = itJet->getIndicesInEvent();
    std::vector<int>::iterator itIdx = indices.begin();
    m_genParticleLinkIndex->clear();
    m_mc_trackToJetAngle->clear();
    for (std::vector<const HepMC::GenParticle*>::iterator itPrt = particles.begin();
         itPrt < particles.end(); ++itPrt, ++itIdx) {
      
      HepGeom::Vector3D<double> tempMomentum((*itPrt)->momentum().px(),
                               (*itPrt)->momentum().py(),
                               (*itPrt)->momentum().pz());
      // attention: push_back calls need a clear() to avoid growth with each entry
      m_mc_trackToJetAngle->push_back((float)tempMomentum.angle(itJet->getMomentum()));
      m_genParticleLinkIndex->push_back(nTruthRecordsAtCurrentEvent+1+(unsigned int)(*itIdx));
    }
    m_nt->Fill();
    m_numberOfTreeEntries++;
  }
  return StatusCode::SUCCESS;
}


unsigned int Trk::JetTruthNtupleTool::getNumberOfTreeRecords() const {
    return m_numberOfTreeEntries;
}
