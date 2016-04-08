/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#if 0 // underlying tool changed to xAOD
// $Id$
/**
 * @file BoostedTopD3PDMaker/src/MiniIsolationElectronFillerTool.cxx
 * @author Lucy Kogan <lucy.anne.kogan@cern.ch>
 * @date Jul 2012
 * @brief Fill mini isolation variables for electron.
 */


#include "BoostedTopD3PDMaker/MiniIsolationElectronFillerTool.h"

#include "ITrackToVertex/ITrackToVertex.h"
#include "egammaInterfaces/IEMTrackIsolationTool.h"
#include "Particle/TrackParticleContainer.h"
#include "egammaEvent/egamma.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MiniIsolationElectronFillerTool::MiniIsolationElectronFillerTool (const std::string& type,
						    const std::string& name,
						    const IInterface* parent)
    : BlockFillerTool<egamma> (type, name, parent),
      m_trackToVertexTool("Reco::TrackToVertex")
  {
    declareProperty ("KtDist", m_ktdist = 10.);
    
    declareProperty("EMTrackIsolationTool", m_EMTrackIsolationTool);

    declareProperty("TrackToVertexTool", m_trackToVertexTool);
    
    declareProperty("TracksContainer", m_TracksContainer = "TrackParticleCandidate");
    declareProperty("VertexContainter",m_VxContainer="VxPrimaryCandidate");

    declareProperty("minDR", m_mindR=0.01);

    //Two maxDR's ===> so that you can have two variables with different dR max's without re-calculating isolation.
    //NB: IT IS IMPORTANT THAT maxDR is GREATER than maxDR_2ndVariable 
    declareProperty("maxDR", m_maxdR=0.4);
    declareProperty("maxDR_2ndVariable", m_maxdR_2=0.3);

    book().ignore();  // Avoid coverity warnings.
  }


  /**
   * @brief Standard Gaudi initialize method.
   */
  StatusCode MiniIsolationElectronFillerTool::initialize()
  {
    ATH_MSG_DEBUG("GETTING TOOLS");
    CHECK( m_trackToVertexTool.retrieve() );
    
    if(m_EMTrackIsolationTool.retrieve().isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve "<<m_EMTrackIsolationTool);
      return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("Retrieved Tool "<<m_EMTrackIsolationTool);

    return StatusCode::SUCCESS;
  }


  /**
   * @brief Book variables for this block.
   */
  StatusCode MiniIsolationElectronFillerTool::book()
  {
    ATH_MSG_DEBUG("BOOKING VARIABLES");
    CHECK( addVariable ("max40_ptsum",  m_ptsum,  "Electron Mini Isolation using standard isolation tools with max dR 0.4") ); 
    CHECK( addVariable ("max40_nTrks",   m_nTracks,   "Number of tracks used for electron mini isolation with max dR 0.4") );  

    //Second variable (_2) for an extra value of dR max
    CHECK( addVariable ("max30_ptsum",  m_ptsum_2,  "Electron Mini Isolation using standard isolation tools with max dR 0.3") ); 
    CHECK( addVariable ("max30_nTrks",   m_nTracks_2,   "Number of tracks used for electron mini isolation with max dR 0.3") );          
    return StatusCode::SUCCESS;
  }


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  StatusCode MiniIsolationElectronFillerTool::fill (const egamma& electron)
  {
    ATH_MSG_DEBUG("IN ELECTRON FILLER");
    *m_ptsum = 999999.;
    *m_nTracks = 999999.;

    //Get Vertex container and track container (needed for EMIsolationTool)
    const VxContainer* vxContainer = 0;
    CHECK(evtStore()->retrieve(vxContainer, m_VxContainer));
    
    const Rec::TrackParticleContainer * trackContainer = 0;
    CHECK(evtStore()->retrieve( trackContainer, m_TracksContainer));
     
    //dR_vector contains 3 cone sizes to be used by isolation tool
    //Mini isolation only needs one (kt/lepton_pt)
    //But the tool doesn't return a value if you only give it one cone size since it expects 3
    //So... Pass it the miniIso dR as the first element of the vector 
    //And put 30 and 40 for the second two
    //Because of this when retrieving the value of ptSum for miniiso need to call ptcone20 
    //as this is where the tool stores the ptsum corresponding to the first cone size
    std::vector<double> dR_vector;
    
    ATH_MSG_DEBUG("Calculate pt etc.");

    const CaloCluster * elec_cluster = electron.cluster();
    float elec_pt = (elec_cluster) ? elec_cluster->p() / std::cosh(electron.eta()) : electron.pt();

    if(fabs(elec_pt) > 0){
      //Calculate cone size  
      double dR = m_ktdist*1000 / elec_pt;
      if ( dR < m_mindR ) { dR = m_mindR; }
      if( dR > m_maxdR ){ dR = m_maxdR; }

      //FOR TESTING ONLY!!!! 
      //dR = 0.4;
      
      dR_vector.push_back(dR);
      //To make tool happy that there are 3 cone sizes                                                                                                                          
      dR_vector.push_back(0.3);
      dR_vector.push_back(0.4);
      
      ATH_MSG_DEBUG("Calculate isolation");
      
      
      //Get isolation                                                                                                                                          
      StatusCode statusIso;
      statusIso =  m_EMTrackIsolationTool->calcptcone(&electron,
						      dR_vector,
						      trackContainer,
						      vxContainer);
      
      if(statusIso.isFailure()){ ATH_MSG_ERROR("Track iso tool pt cone calc failed"); }
      else{
	ATH_MSG_DEBUG("ptcone20 " <<  m_EMTrackIsolationTool->ptcone20());
	//See note above regarding ptcone20----> actually gives you the miniiso isolation
	float isolation = m_EMTrackIsolationTool->ptcone20(); 
	float numberOfTracks = m_EMTrackIsolationTool->nucone20();

	float isolation_30 = m_EMTrackIsolationTool->ptcone30();
	float numberOfTracks_30 = m_EMTrackIsolationTool->nucone30();

	float isolation_2 = isolation;
	float numberOfTracks_2 = numberOfTracks;

	if( dR > m_maxdR_2 ){  
	  isolation_2 = isolation_30;
	  numberOfTracks_2 = numberOfTracks_30;
	}

	(*m_ptsum) = isolation;
	(*m_nTracks) = numberOfTracks;

	//For adding an extra MI variable with a different dR_max without needing to calculate isolations twice!
	(*m_ptsum_2) = isolation_2;
	(*m_nTracks_2) = numberOfTracks_2;
      }
    }

    return StatusCode::SUCCESS;
  }


} // namespace D3PD
#endif
