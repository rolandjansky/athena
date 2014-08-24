/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/ElectronJetDRAssociator.cxx
 * @author clemencia mora <clemencia.mora.herrera@cern.ch>
 * @date 5 May 2010
 * @brief For every electron find jets in a cone and store lifetime, signed IP,
 *        and PTRel of relation.
 *
 * FIXME: This needs to be redone properly.
 */


#include "ElectronJetDRAssociator.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "D3PDMakerInterfaces/ICollectionGetterRegistryTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"
#include <cmath>


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
ElectronJetDRAssociator::ElectronJetDRAssociator (const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent)
  : Base (type, name, parent),
    m_trackToVertexIPEstimator ("Trk::TrackToVertexIPEstimator"),
    m_target(0),
    m_eg(0)
{
  declareProperty ("Target", m_targetLabel,
                   "Target label for the getter for the jets to which"
                   " to associate.");

  declareProperty ("VertexContainerName", 
                   m_vertexContainerName = "VxPrimaryCandidate",
                   "Primary vertex container name");

  declareProperty ("DRCut",  m_drcut = 0.7,
                   "DR cut between electrons and jets");

  declareProperty ("TrackToVertexIPEstimator",
                   m_trackToVertexIPEstimator,
                   "Tool for the estimation of the IPs to the vertex");
    
  declareProperty ("CollectionGetterRegistry", m_registry,
                   "The ICollectionGetterRegistryTool instance.");

  book().ignore();  // Avoid coverity warnings.
}
  
  
/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode ElectronJetDRAssociator::initialize()
{
  CHECK( m_trackToVertexIPEstimator.retrieve() );
  CHECK( Base::initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Configure during initialization: type-check.
 * @param tree Our parent for tuple making.
 * @param ti Gives the type of the object being passed to @c resetUntyped.
 *
 * @c configureD3PD should check that the type of the object coming as input
 * (to @c getUntyped)
 * is compatible with what it expects, and raise an error otherwise.
 */
StatusCode ElectronJetDRAssociator::configureD3PD (IAddVariable* tree,
                                                   const std::type_info& ti)
{
  CHECK( m_registry.retrieve() );
  CHECK( m_registry->get (m_targetLabel, this, m_target) );
  CHECK( Base::configureD3PD (tree, ti) );
  return StatusCode::SUCCESS;
}

  
/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode ElectronJetDRAssociator::reset (const xAOD::Electron& eg)
{
  m_eg = &eg;
  if (m_target)
    CHECK( m_target->reset (true) );
  return StatusCode::SUCCESS;
}
  

/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const xAOD::Jet* ElectronJetDRAssociator::next()
{
  if (!m_target) return 0;

  const xAOD::CaloCluster * clus= m_eg->caloCluster();
  if (!clus){
    ATH_MSG_WARNING("No cluster from the electron found!");
    return 0;
  }

  while (const xAOD::Jet* myjet = m_target->next<xAOD::Jet>()) {

    float dr = hypot (clus->p4().DeltaPhi(myjet->p4()),
                      myjet->eta() - clus->etaBE(2));
    ATH_MSG_DEBUG("The value of DR between Electron and Jet is dr= "<<dr);
    if (dr < m_drcut) {
      float signedIP=-9999.9;
      float PTRel=-9999.9;
      float reducedPt=-9999.9;
      ATH_MSG_DEBUG("CATA egamma pt clus " << clus->et() );
      if( findSignedIPAndPTRelEleJetPair( m_eg, myjet,
                                          signedIP, PTRel ).isFailure() )
      {
        continue;
      }
      if ( findReducedPt ( m_eg, myjet, reducedPt ).isFailure() )
	{
	  continue;
	}

      *m_signedIP = signedIP;
      *m_ptrel = PTRel;
      *m_dr = dr;
      *m_jetSize = myjet->numConstituents();
      *m_reducedPt = reducedPt;

      return myjet;
    }
  }
  
  return 0;
}
  

/**
 * @brief Create any needed tuple variables.
 *
 * This is called at the start of the first event.
 */
StatusCode ElectronJetDRAssociator::book()
{
  CHECK( addVariable( "dr" , m_dr,
                      "DR between electron and associated jet.") );
  CHECK( addVariable( "signedIP" , m_signedIP,
                      "Signed impact parameter between electron and jet.") );
  CHECK( addVariable( "ptrel", m_ptrel,
                      "Relative Pt between electron and jet.") );
  CHECK( addVariable( "jetSize", m_jetSize,
		      "Number of jet constituents.") );
  CHECK( addVariable( "reducedPt", m_reducedPt,
		      "Jet Pt with electron removed.") );
  CHECK( Base::book() );
  return StatusCode::SUCCESS;
}
  
/**
 * @brief Find the pT of the jet with electron removed.
 * @param eg The egamma object to analyze.
 * @param j The jet under consideration.
 * @reducedPt[out] Jet Pt calculated after subtraction of electron momentum.
 */
StatusCode
ElectronJetDRAssociator::findReducedPt (const xAOD::Electron* eg,
                                        const xAOD::Jet* j,
                                        float& reducedPt  )
{
  reducedPt=-9999.;

  if ( !eg || !j )
    return StatusCode::SUCCESS;

  const xAOD::TrackParticle* track = eg->trackParticle();
  if (!track) {
    // Could legitimately be 0 for forward electrons...
    return StatusCode::SUCCESS;
  }

  float px = j->px() - track->p4().Px();
  float py = j->py() - track->p4().Py();
  reducedPt = hypot (px, py);
  
  return StatusCode::SUCCESS;
}


/**
 * @brief Find the impact parameter of EG's track relative to the beamspot.
 * @param eg The egamma object to analyze.
 * @param d0beam[out] Transverse impact parameter wrt the beamspot.
 * @param z0beam[out] Longitudinal impact parameter wrt the beamspot.
 */
StatusCode
ElectronJetDRAssociator::findSignedIPAndPTRelEleJetPair (const xAOD::Electron* eg,
                                                         const xAOD::Jet* j0,
                                                         float& signedIP,
                                                         float& PTRel  )
{
  signedIP=-9999;
  PTRel =-9999;

  if( j0==0 )
    return StatusCode::SUCCESS;

  const xAOD::TrackParticle* track = eg->trackParticle();
  if (!track) {
    // Could legitimately be 0 for forward electrons...
    return StatusCode::SUCCESS;
  }

  PTRel= track->p4().Perp (j0->p4().Vect());

  CLHEP::Hep3Vector jetDirection( (j0)->px(), (j0)->py(), (j0)->pz());
  CLHEP::Hep3Vector unit = jetDirection.unit();

  if (!evtStore()->contains<xAOD::VertexContainer>(m_vertexContainerName))
    return StatusCode::SUCCESS;

  const xAOD::Vertex* vx = 0;
  {
    const xAOD::VertexContainer * vxcontainer;  
    if ( evtStore()->retrieve(vxcontainer, m_vertexContainerName).isFailure() )
      REPORT_MESSAGE (MSG::WARNING)
        << "Vertex container " << m_vertexContainerName 
        << " not found in StoreGate!";
    else if (!vxcontainer->empty())
      vx = vxcontainer->front();
  }

  if (!vx) {
    REPORT_MESSAGE (MSG::WARNING)
      << "No vertex found.";
    return StatusCode::SUCCESS;
  }

  signedIP = m_trackToVertexIPEstimator->get2DLifetimeSignOfTrack
    (track->perigeeParameters(), unit, *vx);
    
  return StatusCode::SUCCESS;
}


} // namespace D3PD


