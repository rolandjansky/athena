/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PositionMomentumWriter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// InDet include
#include "TrkValTools/PositionMomentumWriter.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
// Gaudi
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"

// constructor
Trk::PositionMomentumWriter::PositionMomentumWriter(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_treeName("PositionMomentumWriter"),
  m_treeFolder("/val/"),
  m_treeDescription("Position Momentum Writer")    
{
   declareInterface<Trk::IPositionMomentumWriter>(this);
   // from outside
   declareProperty("TreeName",                 m_treeName);
   declareProperty("TreeFolder",               m_treeFolder);
   declareProperty("TreeDescription",          m_treeDescription);  
}

// Athena standard methods
// initialize
StatusCode Trk::PositionMomentumWriter::initialize()
{
    ATH_MSG_VERBOSE("Booking the Extrapolation test Tree.");
    
    // ------------------------------> OUTPUT NTUPLE (geometry validation)
    m_tree = new TTree(m_treeName.c_str(), m_treeDescription.c_str());

    // add the Branches
    m_tree->Branch("StartEta",      &m_pEta);
    m_tree->Branch("StartPhi",      &m_pPhi);
    m_tree->Branch("StartEnergy",   &m_pE);
    m_tree->Branch("EndEta",        &m_eEta);
    m_tree->Branch("EndPhi",        &m_ePhi);
    m_tree->Branch("EndEnergy",     &m_eE);  
    m_tree->Branch("Pdg",           &m_pPdg);
    m_tree->Branch("Mass",          &m_pM);
    // the branches
    m_pPositionX   = new std::vector<float>;
    m_pPositionY   = new std::vector<float>;
    m_pPositionZ   = new std::vector<float>; 
    m_pPositionR   = new std::vector<float>;
    m_pMomentumX   = new std::vector<float>;
    m_pMomentumY   = new std::vector<float>;
    m_pMomentumZ   = new std::vector<float>;
    m_pMomentumMag = new std::vector<float>;
    m_pMomentumEta = new std::vector<float>;
    m_pMomentumPhi = new std::vector<float>;
    // add the Position Branches
    m_tree->Branch("PositionX",   m_pPositionX);
    m_tree->Branch("PositionY",   m_pPositionY);
    m_tree->Branch("PositionZ",   m_pPositionZ);
    m_tree->Branch("PositionR",   m_pPositionR);
    m_tree->Branch("MomentumX",   m_pMomentumX);
    m_tree->Branch("MomentumY",   m_pMomentumY);
    m_tree->Branch("MomentumZ",   m_pMomentumZ);
    m_tree->Branch("MomentumMag", m_pMomentumMag);
    m_tree->Branch("MomentumEta", m_pMomentumEta);
    m_tree->Branch("MomentumPhi", m_pMomentumPhi);
    
    // now register the Tree
    ITHistSvc* tHistSvc = 0;
    if (service("THistSvc",tHistSvc).isFailure()) {
        ATH_MSG_ERROR( "initialize() Could not find Hist Service  -> Switching Tree output off !" );
        delete m_pPositionX; m_pPositionX = 0; 
        delete m_pPositionY; m_pPositionY = 0;
        delete m_pPositionZ; m_pPositionZ = 0;
        delete m_pPositionR; m_pPositionR = 0;
        delete m_pMomentumX; m_pMomentumX = 0;
        delete m_pMomentumY; m_pMomentumY = 0;
        delete m_pMomentumZ; m_pMomentumZ = 0;
        delete m_pMomentumMag; m_pMomentumMag = 0;
	delete m_pMomentumEta; m_pMomentumEta = 0;
	delete m_pMomentumPhi; m_pMomentumPhi = 0;
        delete m_tree; m_tree = 0;
    }
    if (tHistSvc && ((tHistSvc->regTree(m_treeFolder+m_treeName, m_tree)).isFailure()) ) {
        ATH_MSG_ERROR( "initialize() Could not register the validation Tree -> Switching Tree output off !" );
        delete m_tree; m_tree = 0;
        delete m_pPositionX; m_pPositionX = 0; 
        delete m_pPositionY; m_pPositionY = 0;
        delete m_pPositionZ; m_pPositionZ = 0;
        delete m_pPositionR; m_pPositionR = 0;
        delete m_pMomentumX; m_pMomentumX = 0;
        delete m_pMomentumY; m_pMomentumY = 0;
        delete m_pMomentumZ; m_pMomentumZ = 0;
        delete m_pMomentumMag; m_pMomentumMag = 0;
	delete m_pMomentumEta; m_pMomentumEta = 0;
	delete m_pMomentumPhi; m_pMomentumPhi = 0;
    }    
    
    return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::PositionMomentumWriter::finalize()
{
    // delete the memory 
    delete m_pPositionX; m_pPositionX = 0; 
    delete m_pPositionY; m_pPositionY = 0;
    delete m_pPositionZ; m_pPositionZ = 0;
    delete m_pPositionR; m_pPositionR = 0;
    delete m_pMomentumX; m_pMomentumX = 0;
    delete m_pMomentumY; m_pMomentumY = 0;
    delete m_pMomentumZ; m_pMomentumZ = 0;
    delete m_pMomentumMag; m_pMomentumMag = 0;
    delete m_pMomentumEta; m_pMomentumEta = 0;
    delete m_pMomentumPhi; m_pMomentumPhi = 0;
    // return success
    return StatusCode::SUCCESS;
}


void Trk::PositionMomentumWriter::initializeTrack(const Amg::Vector3D& pos,
                                                  const Amg::Vector3D& mom,
                                                  double m, int pdg) const
{
    if (m_tree){
        // initialize the parmeters
        m_pM   = m;
        m_pEta = mom.eta();
        m_pPhi = mom.phi();
        m_pE   = sqrt(mom.mag()*mom.mag()+m*m);
        m_pPdg = pdg;
        // record the momentum / position
        m_pPositionX->push_back(pos.x());
        m_pPositionY->push_back(pos.y());
        m_pPositionZ->push_back(pos.z());
        m_pPositionR->push_back(pos.perp());
        m_pMomentumX->push_back(mom.x());
        m_pMomentumY->push_back(mom.y());
        m_pMomentumZ->push_back(mom.z());
        m_pMomentumMag->push_back(mom.mag());
        m_pMomentumEta->push_back(mom.eta());
	m_pMomentumPhi->push_back(mom.phi());
    }
}

void Trk::PositionMomentumWriter::recordTrackState(const Amg::Vector3D& pos, 
                                                   const Amg::Vector3D& mom) const
{
    if (m_tree){
        // record the momentum / position
        m_pPositionX->push_back(pos.x());
        m_pPositionY->push_back(pos.y());
        m_pPositionZ->push_back(pos.z());
        m_pPositionR->push_back(pos.perp());
        m_pMomentumX->push_back(mom.x());
        m_pMomentumY->push_back(mom.y());
        m_pMomentumZ->push_back(mom.z());
        m_pMomentumMag->push_back(mom.mag());
	m_pMomentumEta->push_back(mom.eta());
	m_pMomentumPhi->push_back(mom.phi());
    }
}

void Trk::PositionMomentumWriter::finalizeTrack() const
{
    if (m_tree){
        
        if (m_pMomentumX->size()){
            // the index of the last momentum record
            size_t lIndex = size_t(m_pMomentumX->size()-1);
            Amg::Vector3D lastMom(m_pMomentumX->at(lIndex),
                                  m_pMomentumY->at(lIndex),
                                  m_pMomentumZ->at(lIndex));
            // end parameters                      
            m_eE   = sqrt(lastMom.mag()*lastMom.mag()+m_pM*m_pM);
            m_eEta = lastMom.eta();
            m_ePhi = lastMom.phi();
        }
        
        // fill the tree 
        m_tree->Fill();
        // re-initialize
        m_pEta = 0.;
        m_pPhi = 0.;
        m_pE   = 0.;
        m_eEta = 0.;
        m_ePhi = 0.;
        m_eE   = 0.;
        m_pM   = 0.;
        m_pPdg = 0;
        // clear the 
        m_pPositionX->clear();
        m_pPositionY->clear();
        m_pPositionZ->clear();
        m_pPositionR->clear();
        m_pMomentumX->clear();
        m_pMomentumY->clear();
        m_pMomentumZ->clear();
        m_pMomentumMag->clear();
	m_pMomentumEta->clear();
	m_pMomentumPhi->clear();
    }
}
