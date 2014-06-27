/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PAUDiPhotonFillerTool.h
 * @author Mike Hance <hance@hep.upenn.edu>
 * @date August, 2010
 * @brief Fill PAU related variables for diphotons
 */


#include "PAUDiPhotonFillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "PhotonAnalysisUtils/PAUDiPhoton.h"

#include "VxVertex/VxContainer.h"
#include "Particle/TrackParticleContainer.h"

//#include "CaloUtils/CaloClusterVertexCorrection.h"
#include "ParticleEvent/TrackParticleAssocs.h"

namespace D3PD {


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PAUDiPhotonFillerTool::PAUDiPhotonFillerTool (const std::string& type,
						const std::string& name,
						const IInterface* parent)
    : BlockFillerTool<PAU::DiPhoton> (type, name, parent)
      //,m_photonTrackIsolationTool( "TrackIsolationTool/EMTrackIsolationPhotonTrackIsolationTool" )
  {
    //declareProperty( "PhotonTrackIsolationTool", m_photonTrackIsolationTool, "the track isolation tool for photons" );
    declareProperty( "TrackParticleContainerName", m_trackParticleContainerName="TrackParticleCandidate");
    declareProperty( "VxPrimaryContainerName", m_vxPrimaryContainerName="VxPrimaryCandidate");
    declareProperty( "TrackMapName", m_trackMapName="GSFTrackAssociation");
    declareProperty( "UseDefaultPVForTrackIsol", m_useIDPV=false); //will use diphoton vertex if false
    book().ignore(); // Avoid coverity warnings.
  }
  
  PAUDiPhotonFillerTool::~PAUDiPhotonFillerTool()
  {
  }
  
  StatusCode PAUDiPhotonFillerTool::initialize()
  {
    //CHECK( m_photonTrackIsolationTool.retrieve() );  
    return StatusCode::SUCCESS;
  }
  
  /**
   * @brief Book variables for this block.
   */
  StatusCode PAUDiPhotonFillerTool::book()
  {
    CHECK( addVariable ("fit_etaLead_zcommon"         , m_etaLead                     ) );
    CHECK( addVariable ("fit_etaSublead_zcommon"      , m_etaSublead                  ) );
    CHECK( addVariable ("fit_zcommon"                 , m_z                           ) );
    CHECK( addVariable ("fit_sigma_zcommon"           , m_zres                        ) );
    CHECK( addVariable ("fit_vtxLikelihood"           , m_vtxLikelihood               ) );
    
    CHECK( addVariable ("fit_mgg"                     , m_invmass                     ) );
    CHECK( addVariable ("fit_mgg_trueDir"             , m_invmassTrueDir              ) );
    CHECK( addVariable ("fit_mgg_trueE"               , m_invmassTrueE                ) );
    CHECK( addVariable ("fit_fitFlag"                 , m_invmass_flag                ) );
    
    CHECK( addVariable ("fit_ptgg"                    , m_ptgg                        ) );
    CHECK( addVariable ("fit_detagg"                  , m_detagg                      ) );
    CHECK( addVariable ("fit_cosThetaStar"            , m_cosTh                       ) );
    
    
    CHECK( addVariable ("onejet_cutStatus"            , m_onejet_cutStatus              ) );
    CHECK( addVariable ("onejet_ok"                   , m_onejet_ok                     ) );
    CHECK( addVariable ("onejet_mgg"                  , m_onejet_mgg                    ) );
    CHECK( addVariable ("onejet_mggj"                 , m_onejet_mggj                   ) );
    
    CHECK( addVariable ("twojet_cutStatus"            , m_twojet_cutStatus              ) );
    CHECK( addVariable ("twojet_ok"                   , m_twojet_ok                     ) );
    CHECK( addVariable ("twojet_mgg"                  , m_twojet_mgg                    ) );
    CHECK( addVariable ("twojet_mjj"                  , m_twojet_mjj                    ) );
    
    CHECK( addVariable ("HPV_zcommon"                      , m_hgg_HPV_zcommon ) );
    CHECK( addVariable ("HPV_sigma_zcommon"                , m_hgg_HPV_sigma_zcommon ) );
    CHECK( addVariable ("HPV_zcommon_primVtxLH"	         , m_hgg_HPV_zcommon_primVtxLH ) );
    CHECK( addVariable ("HPV_zcommon_primVtxNN"	         , m_hgg_HPV_zcommon_primVtxNN ) );
    CHECK( addVariable ("HPV_vtxLikelihood"	         , m_hgg_HPV_vtxLikelihood ) );
    CHECK( addVariable ("HPV_etaLead_zcommon"	         , m_hgg_HPV_etaLead_zcommon ) );
    CHECK( addVariable ("HPV_etaSublead_zcommon"	         , m_hgg_HPV_etaSublead_zcommon ) );
    CHECK( addVariable ("HPV_mgg"  		         , m_hgg_HPV_invmass ) );
    CHECK( addVariable ("HPV_etaLead_zcommon_primVtxLH"    , m_hgg_HPV_etaLead_zcommon_primVtxLH ) );
    CHECK( addVariable ("HPV_etaSublead_zcommon_primVtxLH" , m_hgg_HPV_etaSublead_zcommon_primVtxLH ) );
    CHECK( addVariable ("HPV_mgg_primVtxLH"	         , m_hgg_HPV_invmass_primVtxLH ) );
    CHECK( addVariable ("HPV_mgg_Ztruth"  	         , m_hgg_HPV_invmass_Ztruth ) );
    CHECK( addVariable ("HPV_ptgg"    		         , m_hgg_HPV_ptgg ) );
    CHECK( addVariable ("HPV_detagg"  		         , m_hgg_HPV_detagg ) );
    CHECK( addVariable ("HPV_dphigg"  		         , m_hgg_HPV_dphigg ) );
    CHECK( addVariable ("HPV_cosTh"   		         , m_hgg_HPV_cosTh ) );
    CHECK( addVariable ("HPV_cosThCS"                      , m_hgg_HPV_cosThCS ) );
    
    CHECK( addVariable ("ptcone20", m_hgg_ptcone20 ) );
    CHECK( addVariable ("ptcone30", m_hgg_ptcone30 ) );
    CHECK( addVariable ("ptcone40", m_hgg_ptcone40 ) );
    
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
  StatusCode PAUDiPhotonFillerTool::fill (const PAU::DiPhoton& p)
  {
    
    *m_etaLead        = p.etaLead        ;
    *m_etaSublead     = p.etaSublead     ;
    *m_z              = p.z              ;
    *m_zres           = p.zres           ;
    *m_vtxLikelihood  = p.vtxLikelihood  ;
    
    *m_invmass        = p.invmass        ;
    *m_invmassTrueDir = p.invmassTrueDir ;
    *m_invmassTrueE   = p.invmassTrueE   ;
    *m_invmass_flag   = p.invmass_flag   ;
    
    *m_ptgg           = p.ptgg           ;
    *m_detagg         = p.detagg         ;
    *m_cosTh          = p.cosTh          ;
    
    
    *m_onejet_cutStatus    = p.onejet_cutStatus      ;
    *m_onejet_ok           = p.onejet_ok             ;
    *m_onejet_mgg          = p.onejet_mgg            ;
    *m_onejet_mggj         = p.onejet_mggj           ;
    
    *m_twojet_cutStatus    = p.twojet_cutStatus      ;
    *m_twojet_ok           = p.twojet_ok             ;
    *m_twojet_mgg          = p.twojet_mgg            ;
    *m_twojet_mjj          = p.twojet_mjj            ;
    
    *m_hgg_HPV_zcommon                      = p.hgg_HPV_zcommon;                        
    *m_hgg_HPV_sigma_zcommon                = p.hgg_HPV_sigma_zcommon;                        
    *m_hgg_HPV_zcommon_primVtxLH	    = p.hgg_HPV_zcommon_primVtxLH;              
    *m_hgg_HPV_zcommon_primVtxNN	    = p.hgg_HPV_zcommon_primVtxNN;
    *m_hgg_HPV_vtxLikelihood		    = p.hgg_HPV_vtxLikelihood;                  
    *m_hgg_HPV_etaLead_zcommon		    = p.hgg_HPV_etaLead_zcommon;                
    *m_hgg_HPV_etaSublead_zcommon 	    = p.hgg_HPV_etaSublead_zcommon;             
    *m_hgg_HPV_invmass			    = p.hgg_HPV_invmass;                        
    *m_hgg_HPV_etaLead_zcommon_primVtxLH    = p.hgg_HPV_etaLead_zcommon_primVtxLH;      
    *m_hgg_HPV_etaSublead_zcommon_primVtxLH = p.hgg_HPV_etaSublead_zcommon_primVtxLH;   
    *m_hgg_HPV_invmass_primVtxLH	    = p.hgg_HPV_invmass_primVtxLH;              
    *m_hgg_HPV_invmass_Ztruth		    = p.hgg_HPV_invmass_Ztruth;                 
    *m_hgg_HPV_ptgg    	  		    = p.hgg_HPV_ptgg    ;                       
    *m_hgg_HPV_detagg  		  	    = p.hgg_HPV_detagg  ;                       
    *m_hgg_HPV_dphigg  			    = p.hgg_HPV_dphigg  ;                       
    *m_hgg_HPV_cosTh   			    = p.hgg_HPV_cosTh   ;                       
    *m_hgg_HPV_cosThCS                      = p.hgg_HPV_cosThCS ;                         


    /*
    const std::vector<PAU::egamma*>* photonList = p.getPhotons();
    
    // list of track isolation energies (one for each cone)
    std::vector<double> energies1;
    std::vector<double> energies2;
    std::vector<double> cones;
    cones.push_back(0.2);
    cones.push_back(0.3);
    cones.push_back(0.4);

    // correct for the primary vertex.
    Amg::Vector3D originPos(0,0,0); 
    const Rec::TrackParticleContainer* aTrackParticleContainer = 0;
    CHECK( evtStore()->retrieve( aTrackParticleContainer , m_trackParticleContainerName ));  
    
    VxContainer newContainer (SG::OWN_ELEMENTS);
    const VxContainer* vxTES = 0;
    const Trk::VxCandidate *vx = 0;
    if (m_useIDPV) {
      CHECK( evtStore()->retrieve( vxTES , m_vxPrimaryContainerName ));
      for (VxContainer::const_iterator it = vxTES->begin(); it != vxTES->end(); ++it) {
	if ((*it)->vertexType() == Trk::PriVtx) {
	  vx = *it;
	  break;
	}
      }
    } else {
      vx = p.hgg_HPV_vertex;
      if (vx) {
	Trk::VxCandidate* vxCand = vx->clone();
	vxCand->setVertexType(Trk::PriVtx);
	newContainer.push_back(vxCand);
	vxTES = &newContainer;
      } else {
	ATH_MSG_WARNING("No diphoton vertex found....");
      }
    }

    //const Trk::Vertex *vertex = 0;
    if (!vx) {
      ATH_MSG_WARNING("Vertex container VxPrimaryCandidate has no vertex.");  
    } else {
      originPos = vx->recVertex().position();
      //vertex = dynamic_cast<const Trk::Vertex *>(&(vx->recVertex()));
    }
    const CaloCluster* aCluster1 = photonList->at(0)->cluster();
    const CaloCluster* aCluster2 = photonList->at(1)->cluster();
        
    //const Amg::Vector3D vect1(originPos);
    //const Amg::Vector3D vect2(originPos);
    //CaloClusterChangeVertexState mychange1(aCluster1, &vect1);
    //CaloClusterChangeVertexState mychange2(aCluster2, &vect2);
        
    std::vector<Rec::TrackParticle*> aListOfTracks1;
    const Trk::VxCandidate* vx1 = photonList->at(0)->photon()->conversion();
    if (vx1) {
      std::vector<Trk::VxTrackAtVertex*> trks1(*(vx1->vxTrackAtVertex()));
      int numTracksAtVertex1 = trks1.size();
      for (int i=0; i<numTracksAtVertex1; i++) {
	Trk::VxTrackAtVertex* trk1 = trks1[i];
	const Rec::TrackParticle *trkP1 = getTrackParticle(trk1);
	const TrackParticleAssocs *trkassocs1 = 0;
	CHECK( evtStore()->retrieve( trkassocs1 , m_trackMapName ));
	Rec::TrackParticleContainer associatedElems1(SG::VIEW_ELEMENTS);
	if (trkassocs1->assocs(trkP1,associatedElems1)) {
	Rec::TrackParticle* trackP1 = associatedElems1.at(0);
	if (trackP1) aListOfTracks1.push_back(trackP1);
      }
    }
  }
    m_photonTrackIsolationTool->trackIsolation( aCluster1, cones, energies1, TrackIsolPtType::summedPt, false,
						0, 1.0, 0.0, aTrackParticleContainer, vxTES, &aListOfTracks1);


  std::vector<Rec::TrackParticle*> aListOfTracks2;
  const Trk::VxCandidate* vx2 = photonList->at(1)->photon()->conversion();
  if (vx2) {
    std::vector<Trk::VxTrackAtVertex*> trks2(*(vx2->vxTrackAtVertex()));
    int numTracksAtVertex2 = trks2.size();
    for (int i=0; i<numTracksAtVertex2; i++) {
      Trk::VxTrackAtVertex* trk2 = trks2[i];
      const Rec::TrackParticle *trkP2 = getTrackParticle(trk2);
      const TrackParticleAssocs *trkassocs2 = 0;
      CHECK( evtStore()->retrieve( trkassocs2 ,  m_trackMapName ));
      Rec::TrackParticleContainer associatedElems2(SG::VIEW_ELEMENTS);
      if (trkassocs2->assocs(trkP2,associatedElems2)) {
	Rec::TrackParticle* trackP2 = associatedElems2.at(0);
	if (trackP2) aListOfTracks2.push_back(trackP2);
      }
    }
  }
  m_photonTrackIsolationTool->trackIsolation( aCluster2, cones, energies2, TrackIsolPtType::summedPt, false,
					      0, 1.0, 0.0, aTrackParticleContainer, vxTES, &aListOfTracks2);

  std::vector<float> ptcone20, ptcone30, ptcone40;
  ptcone20.push_back(energies1[0]); ptcone20.push_back(energies2[0]);
  ptcone30.push_back(energies1[1]); ptcone30.push_back(energies2[1]);
  ptcone40.push_back(energies1[2]); ptcone40.push_back(energies2[2]);
  *m_hgg_ptcone20 = ptcone20;
  *m_hgg_ptcone30 = ptcone30;
  *m_hgg_ptcone40 = ptcone40;
  ptcone20.clear();  
  ptcone30.clear();  
  ptcone40.clear();
    */

    // isolation tool doesn't exist anymore, fill empty values
    std::vector<float> ptcone20, ptcone30, ptcone40;
  ptcone20.push_back(0); ptcone20.push_back(0);
  ptcone30.push_back(0); ptcone30.push_back(0);
  ptcone40.push_back(0); ptcone40.push_back(0);
  *m_hgg_ptcone20 = ptcone20;
  *m_hgg_ptcone30 = ptcone30;
  *m_hgg_ptcone40 = ptcone40;


  return StatusCode::SUCCESS;
  }
  
  /*
  const Rec::TrackParticle* PAUDiPhotonFillerTool::getTrackParticle(Trk::VxTrackAtVertex *trkAtVx) 
  {
    // protection against bad pointers
    if (trkAtVx==0) return 0;
    
    //find the link to the TrackParticleBase
    const Trk::ITrackLink* trkLink = trkAtVx->trackOrParticleLink();
    const Trk::TrackParticleBase* trkPB(0);
    if (0!= trkLink) {
      const Trk::LinkToTrackParticleBase* linktrkPB = dynamic_cast<const Trk::LinkToTrackParticleBase *>(trkLink);  
      if (0!= linktrkPB) {
	if (linktrkPB->isValid()) trkPB = linktrkPB->cachedElement(); 
      }//end of dynamic_cast check
    }//end of ITrackLink existance check
    
    //cast to TrackParticle
    if (trkPB) {
      const Rec::TrackParticle* trkP = dynamic_cast<const Rec::TrackParticle*>(trkPB);
      return trkP;
    } else {
      return 0;
    }
  }
  */

} // namespace D3PD
