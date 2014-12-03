/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JetTrackTruthMatching.cxx
//   Source file for class JetTrackTruthMatching
///////////////////////////////////////////////////////////////////

#include "ParticleJetTools/JetTrackTruthMatching.h"

#include "DataModel/ElementLink.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "ParticleTruth/TrackParticleTruth.h"
#include "ParticleTruth/TrackParticleTruthKey.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"

#include "GeneratorObjects/HepMcParticleLink.h"
#include "GeneratorObjects/McEventCollection.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"

///
// #include "CaloEvent/CaloCell.h"
// #include "Navigation/NavigationToken.h"
///
#include "JetEvent/Jet.h"
#include "JetTagEvent/TrackAssociation.h"

namespace Analysis
{

// constructor
JetTrackTruthMatching::JetTrackTruthMatching(const std::string& t, const std::string& n, const IInterface* p) :
  AlgTool(t,n,p),
  m_storeGate(0),
  m_trackParticlesName("TrackParticleCandidate"),
  m_trackParticleTruthMapName("TrackParticleTruthCollection"),
  m_mcEventCollection("TruthEvents"),
  m_jetLabel(0)
{
    declareInterface<IJetTruthMatching>(this);  
    declareProperty("TrackParticlesName",        m_trackParticlesName);
    declareProperty("TrackParticleTruthMapName", m_trackParticleTruthMapName);
    declareProperty("McEventCollection",         m_mcEventCollection);
}

// destructor
JetTrackTruthMatching::~JetTrackTruthMatching()
{}

// Athena standard methods
// initialize
StatusCode JetTrackTruthMatching::initialize()
{
    
    MsgStream mlog(msgSvc(),name());
//     m_printParameterSettings();
    /* ----------------------------------------------------------------------------------- */
    /*                        RETRIEVE SERVICES FROM STOREGATE                             */
    /* ----------------------------------------------------------------------------------- */
    // 1. Retrieve the StoreGate Service itself
    if (service("StoreGateSvc", m_storeGate).isFailure())
    {
        // this method just gives out an error message
        mlog << MSG::FATAL << "StoreGateSvc not found." << endreq;
        return StatusCode::FAILURE;
    };
    return StatusCode::SUCCESS;
}

// finalize
StatusCode JetTrackTruthMatching::finalize()
{
    MsgStream mlog(msgSvc(), name());
    mlog << MSG::INFO << name() << " finalize() success" << endreq;
    return StatusCode::SUCCESS;
}


bool JetTrackTruthMatching::matchJet(const Jet& myJet)
{
    MsgStream mlog(msgSvc(),name());
//     mlog << MSG::VERBOSE << name() << " execute() begin." << endreq;

    /* ----------------------------------------------------------------------------------- */
    /*                     RETRIEVE TRACKPARTICLE CONTAINER FROM STOREGATE                           */
    /* ----------------------------------------------------------------------------------- */
    // this is only needed that the find of the TrackParticleTruth objects works
    // Remember: the key of the map is an ElementLink<Rec::TrackParticleContainer>
    // in order that two EL are equal they have to be setStorableObject to the same
    // collection
    const Rec::TrackParticleContainer* origTrackPC(0);
    StatusCode sc = m_storeGate->retrieve(origTrackPC, m_trackParticlesName);
    if (sc.isFailure())
    {
        mlog << MSG::DEBUG << m_trackParticlesName << " not found in StoreGate." << endreq;
//         return sc;
    };
    
    /* ----------------------------------------------------------------------------------- */
    /*                     Retrieve TrackParticle Truth Info                               */
    /* ----------------------------------------------------------------------------------- */
    // retrieve associated TrackTruth from StoreGate (Track*, TrackTruth)
    const TrackParticleTruthCollection* tpTruthColl(0);
    sc = m_storeGate->retrieve(tpTruthColl, m_trackParticleTruthMapName);
    if (sc.isFailure())
    {
        mlog << MSG::DEBUG << m_trackParticleTruthMapName << " not found in StoreGate." << endreq;
//         return sc;
    }

    /* ----------------------------------------------------------------------------------- */
    /*                     Retrieve McEventCollection                                      */
    /* ----------------------------------------------------------------------------------- */
    const McEventCollection* myMcEventCollection(0);
    sc = m_storeGate->retrieve(myMcEventCollection, m_mcEventCollection);
    if (sc.isFailure())
    {
        mlog << MSG::DEBUG << m_mcEventCollection << " not found in StoreGate." << endreq;
//         return sc;
    }

    mlog << MSG::DEBUG << "Number of TrackParticles in event: " << tpTruthColl->size() << endreq;
    
    return m_jetContainsBMeson(myJet,origTrackPC,tpTruthColl,myMcEventCollection);

    /* ----------------------------------------------------------------------------------- */
    /*                      STORING RESULTS IN STOREGATE                                   */
    /* ----------------------------------------------------------------------------------- */

//     mlog << MSG::VERBOSE << name() << " execute() end." << endreq;
//     return StatusCode::SUCCESS;
}

/** print parameter settings of the truth match tool */
void JetTrackTruthMatching::m_printParameterSettings()
{
// not implemented yet
}


bool JetTrackTruthMatching::m_jetContainsBMeson(const Jet& myJet, 
              const Rec::TrackParticleContainer* origTrackPC,
              const TrackParticleTruthCollection* tpTruthColl,
              const McEventCollection* myMcEventCollection )
{
//     std::cout << origTrackPC << "\t" << tpTruthColl << "\t" << myMcEventCollection << std::endl;
    MsgStream mlog(msgSvc(),name());
    bool trueBParticleInJet = false;
    int numTrackParticles(1);
    int sameBfound(0);
    int theBarcode(0);
    
    const HepMC::GenEvent* GenEvent=*(myMcEventCollection->begin());
    /// loop over particles in jet to see if there are B's
    ///
    m_jetLabel=0;    
    std::vector<const Rec::TrackParticle*>* trackVector = myJet.getAssociation<TrackAssociation>("Tracks",true)->tracks(); //JBdV add the true arg 
    for (std::vector<const Rec::TrackParticle*>::iterator jetItr = trackVector->begin(); jetItr != trackVector->end() ; ++jetItr )
    {
        const Rec::TrackParticle* aTemp = *jetItr;

        /// get the truth to this TrackParticle
        ElementLink< Rec::TrackParticleContainer > myLink;
        myLink.setElement(const_cast<Rec::TrackParticle*>(aTemp));
        myLink.setStorableObject(*origTrackPC);
        
        TrackParticleTruthCollection::const_iterator tempTPTItr = tpTruthColl->find(Rec::TrackParticleTruthKey(myLink));

        if (tempTPTItr == tpTruthColl->end() )
        {
          mlog << MSG::DEBUG << "No TrackParticle truth found" << endreq;
          continue;
        }

        const HepMcParticleLink& temHepMcLink = (*tempTPTItr).second.particleLink();
        double prob((*tempTPTItr).second.probability());
        theBarcode=temHepMcLink.barcode();
        if (theBarcode == 0) 
        {
          mlog << MSG::DEBUG << "Barcode of TrackParticle " << numTrackParticles << " is zero! : " << theBarcode << "\t" << prob<< endreq;
        }

        const HepMC::GenParticle* theGenParticle = GenEvent->barcode_to_particle(theBarcode);
        if (theGenParticle == 0)
        {
            mlog << MSG::DEBUG << "No barcode match found for TrackParticle " << numTrackParticles << endreq;
            continue;
        }
        
        /// now theGenParticle of the mc collection points to the truth particle
        /// of the TrackParticle in question
        /// If this TrackParticle has a B as one of its ancestor the jet will be taken as a bjet
        HepMC::GenVertex* prodVert = theGenParticle->production_vertex();
        //prodVert->print();
        
        HepMC::GenVertex::particle_iterator parentParticlesOfVertex;
        for ( parentParticlesOfVertex  = prodVert->particles_begin(HepMC::ancestors);
                parentParticlesOfVertex != prodVert->particles_end(HepMC::ancestors)  ;
                ++parentParticlesOfVertex )
        {
            int pdg((*parentParticlesOfVertex)->pdg_id());
            int rest1(abs(pdg%1000));
            int rest2(abs(pdg%10000));
            if ( (rest2 >= 5000 && rest2 < 6000) || (rest1 >= 500 && rest1 < 600) )
            {
                theBarcode = (*parentParticlesOfVertex)->barcode();
                mlog << MSG::DEBUG << "BParticle: " << pdg << " with barcode: " << theBarcode << endreq;
                trueBParticleInJet = true;
                m_jetLabel=5;
                sameBfound++;
            }
        }
        if (!trueBParticleInJet)
        {
          for ( parentParticlesOfVertex  = prodVert->particles_begin(HepMC::ancestors);
                  parentParticlesOfVertex != prodVert->particles_end(HepMC::ancestors)  ;
                  ++parentParticlesOfVertex )
          {
              int pdg((*parentParticlesOfVertex)->pdg_id());
              int rest1(abs(pdg%1000));
              int rest2(abs(pdg%10000));
              if ( (rest2 >= 4000 && rest2 < 5000) || (rest1 >= 400 && rest1 < 500) )
              {
                  theBarcode = (*parentParticlesOfVertex)->barcode();
                  mlog << MSG::DEBUG << "CParticle: " << pdg << " with barcode: " << theBarcode << endreq;
                  m_jetLabel=4;
              }
          }
        }
      numTrackParticles++;
    }
    mlog << MSG::DEBUG << "The same BParticle has been found " << sameBfound << " times." << endreq;
    return true;
}

}

