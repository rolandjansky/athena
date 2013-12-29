/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         InDetSimpleV0Finder.cxx  -  Description
                             -------------------
    begin   : 20-07-2005
    authors : Evelina Bouhova-Thacker (Lancaster University), Rob Henderson (Lancater University)
    email   : e.bouhova@cern.ch, r.henderson@lancaster.ac.uk
    changes : June 1 2006
    author  : Kirill Prokofiev <Kirill.Prokofiev@cern.ch> 
              Changed to use the ExtendedVxCandidate..

 ***************************************************************************/

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ISvcLocator.h"

#include "VxVertex/ExtendedVxCandidate.h"
#include "VxVertex/VxContainer.h"
#include "TrkV0Vertex/V0Hypothesis.h"
#include "TrkV0Vertex/V0Candidate.h"
#include "TrkV0Vertex/V0Container.h"
#include "InDetV0Finder/InDetSimpleV0Finder.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkV0Fitter/TrkV0VertexFitter.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "InDetConversionFinderTools/InDetConversionFinderTools.h"
#include "InDetConversionFinderTools/ConversionFinderUtils.h"

#include "GaudiKernel/IPartPropSvc.h"
#include "DataModel/ElementLink.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"

#include <vector>
#include <cmath>

namespace InDet
{
  
  InDetSimpleV0Finder::InDetSimpleV0Finder(const std::string &n, ISvcLocator *pSvcLoc)
    :
    AthAlgorithm(n, pSvcLoc),
    m_iVertexFitter("Trk::V0VertexFitter"),
    m_iVKVertexFitter("Trk::TrkVKalVrtFitter"),
    m_V0Tools("Trk::V0Tools"),
    m_helpertool("InDet::ConversionFinderUtils"),
    m_trkSelector("InDet::TrackSelectorTool"),
    m_vertexEstimator("InDet::VertexPointEstimator"),
    m_extrapolator("Trk::Extrapolator/InDetExtrapolator"),
    m_useV0Fitter(true),
    m_minVertProb(0.0001),
    m_samesign(false),
    m_TrkParticleCollection("TrackParticleCandidate"),
    m_V0CandidatesOutputName("V0Candidates")
  {
    declareProperty("VertexFitterTool", m_iVertexFitter);
    declareProperty("VKVertexFitterTool", m_iVKVertexFitter);

    declareProperty("V0Tools",m_V0Tools);
    declareProperty("ConversionFinderHelperTool", m_helpertool);
    declareProperty("TrackSelectorTool", m_trkSelector);
    declareProperty("VertexPointEstimator", m_vertexEstimator);
    declareProperty("Extrapolator", m_extrapolator);
    declareProperty("useV0Fitter", m_useV0Fitter, "use the V0Fitter instead of the VKVertexFitter");

    declareProperty("minVertProb", m_minVertProb, "cut on vertex-fit chi2 probability (max value)");
    declareProperty("AddSameSign", m_samesign);
    declareProperty("UsePrimaryVertexTracks", m_usePVtracks=false, "Use or ignore tracks which are used by the primary vertex");
    declareProperty("TrackParticleCollection", m_TrkParticleCollection, "StoreGate name of input TrackParticle collection");
    declareProperty("V0CandidatesOutputName", m_V0CandidatesOutputName="SimpleV0Candidates");
  }

  InDetSimpleV0Finder::~InDetSimpleV0Finder() {}

  StatusCode InDetSimpleV0Finder::initialize()
  {
    StatusCode sc;

    // Get the right vertex fitting tool from ToolSvc 
    if (m_useV0Fitter) {
      if ( m_iVertexFitter.retrieve().isFailure() ) {
	msg(MSG::FATAL) << "Failed to retrieve tool " << m_iVertexFitter << endreq;
	return StatusCode::FAILURE;
      } else {
	msg(MSG::INFO) << "Retrieved tool " << m_iVertexFitter << endreq;
      }
  
    } else {
      if ( m_iVKVertexFitter.retrieve().isFailure() ) {
	msg(MSG::FATAL) << "Failed to retrieve tool " << m_iVKVertexFitter << endreq;
	return StatusCode::FAILURE;
      } else {
	msg(MSG::INFO) << "Retrieved tool " << m_iVKVertexFitter << endreq;
      }

    }

    sc = resetStatistics();     // reset counters
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Error in resetStatistics !" << endreq;
      return sc;
    }

    // uploading the corresponding V0 tools
    if ( m_V0Tools.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_V0Tools << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_V0Tools << endreq;
    }

    // Get the extrapolator
    if ( m_extrapolator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endreq;
    }

    // Get the helpertool from ToolSvc
    if ( m_helpertool.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_helpertool << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_helpertool << endreq;
    }

    // Get the track selector tool from ToolSvc
    if ( m_trkSelector.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_trkSelector << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_trkSelector << endreq;
    }

    // Get the vertex point estimator tool from ToolSvc
    if ( m_vertexEstimator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_vertexEstimator << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_vertexEstimator << endreq;
    }


    msg(MSG::INFO) << "Initialization successful" << endreq;

    return StatusCode::SUCCESS;
  }

  StatusCode InDetSimpleV0Finder::execute()
  {
    StatusCode sc;

    m_events_processed ++;

    // making a concrete fitter for the V0Fitter
    Trk::TrkV0VertexFitter* concreteVertexFitter=0;
    if (m_useV0Fitter) {
      concreteVertexFitter = dynamic_cast<Trk::TrkV0VertexFitter * >(&(*m_iVertexFitter));
      if(concreteVertexFitter == 0) {
	ATH_MSG_FATAL("The vertex fitter passed is not a V0 Vertex Fitter");
	return StatusCode::FAILURE;
      }
    }

    // Retrieve track particles from StoreGate
    const Trk::TrackParticleBaseCollection* TPC(0);
    sc = evtStore()->retrieve(TPC,m_TrkParticleCollection);
    if(sc.isFailure()){
      ATH_MSG_ERROR("No track particle collection with name " << m_TrkParticleCollection << " found in StoreGate!");
      return StatusCode::SUCCESS;
    } else {
      ATH_MSG_DEBUG("Found track particle collection " << m_TrkParticleCollection << " in StoreGate!");
    }
    ATH_MSG_DEBUG("Track particle container size " <<  TPC->size());

    // V0Container to store V0Candidates in StoreGate
    V0Container* theV0Container = new V0Container;

    // track preselection
    std::vector<const Trk::TrackParticleBase*> posTracks; posTracks.clear();
    std::vector<const Trk::TrackParticleBase*> negTracks; negTracks.clear();
    const Trk::Vertex* vx = 0;

    if (TPC->size() > 1) {
      Trk::TrackParticleBaseCollection::const_iterator tpIt;
      for ( tpIt =(*TPC).begin(); tpIt !=(*TPC).end(); ++tpIt ){
	const Trk::Perigee * aPerigee = dynamic_cast<const Trk::Perigee*>(&(*tpIt)->definingParameters());
	if (aPerigee == 0) ATH_MSG_DEBUG("No perigees for the tracks!!!");
	if (aPerigee != 0) {
	  double charge = 1.;
	  if (aPerigee->parameters()[Trk::qOverP] < 0.) charge = -1.;
	  if (m_trkSelector->decision(**tpIt, vx)) {
            if (m_samesign) {
              posTracks.push_back(*tpIt);
              negTracks.push_back(*tpIt);
            } else {
              if (charge > 0) {
                posTracks.push_back(*tpIt);
              } else {
                negTracks.push_back(*tpIt);
              }
            }
	  }
	}
      }
    }
    ATH_MSG_DEBUG("number of tracks passing preselection, positive " << posTracks.size() << " negative " << negTracks.size());

    // select tracks pairs with opposite charge
    if (posTracks.size() > 0 && negTracks.size() > 0) {

      std::vector<const Trk::TrackParticleBase*>::const_iterator tpIt1;
      std::vector<const Trk::TrackParticleBase*>::const_iterator tpIt2;

      // loop over positive tracks
      unsigned int i1 = 0;
      for (tpIt1 = posTracks.begin(); tpIt1 != posTracks.end(); ++tpIt1) {
	const Trk::Perigee * aPerigee1 = dynamic_cast<const Trk::Perigee*>(&(*tpIt1)->definingParameters());
	if (aPerigee1 == 0) { 
	  ATH_MSG_DEBUG("No perigees for the tracks!!!"); 
	  continue; // with next positive track
	}
	const Trk::TrackSummary* tSum1 = (*tpIt1)->trackSummary();
	if (!tSum1) continue;
        double charge1 = 1.;
        if (aPerigee1->parameters()[Trk::qOverP] < 0.) charge1 = -1.;

	// loop over negative tracks
        unsigned int i2 = 0;
	for (tpIt2 = negTracks.begin(); tpIt2 != negTracks.end(); ++tpIt2) {
	  const Trk::Perigee * aPerigee2 = dynamic_cast<const Trk::Perigee*>(&(*tpIt2)->definingParameters());
	  if (aPerigee2 == 0) { 
	    ATH_MSG_DEBUG("No perigees for the tracks!!!"); 
	    continue; // with next negative track
	  }

	  const Trk::TrackSummary* tSum2 = (*tpIt2)->trackSummary();
	  if (!tSum2) continue;
          double charge2 = 1.;
          if (aPerigee2->parameters()[Trk::qOverP] < 0.) charge2 = -1.;

          if (m_samesign && i1 <= i2) continue;
          if (*tpIt1 == *tpIt2) continue;

	  // particleOriginType : return A TrackParticleOrigin enum (see TrkEventPrimitives/VertexType.h for definitions)
	  // NoVtx   = 0,      //!< Dummy vertex, TrackParticle was not used in vertex fit --> exclude tracks which have been used for the primary vertex
	  if ( m_usePVtracks || ( (*tpIt1)->particleOriginType() == 0 && (*tpIt2)->particleOriginType() == 0) ) {

	    // find a starting point
	    Amg::Vector3D startingPoint(0.,0.,0.);
	    int sflag = 0;
	    int errorcode = 0;
	    startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(aPerigee1,aPerigee2,sflag,errorcode);

	    if (errorcode == 0 || errorcode == 5) {
	      // Get measured track parameters at first hit
              const Trk::TrackParameters* atrack1 = m_helpertool->getTrkParticleParameters((*tpIt1));
              const Trk::TrackParameters* atrack2 = m_helpertool->getTrkParticleParameters((*tpIt2));

	      // pair pre-selection cuts
	      if ( atrack1 && atrack2 && doFit(atrack1, atrack2, startingPoint) ) {

		Trk::Vertex vertex(startingPoint);
		std::vector<const Trk::TrackParameters*> pairV0;
		pairV0.clear();
		pairV0.push_back(atrack1);
		pairV0.push_back(atrack2);

		// vertex fit
		ATH_MSG_DEBUG("mass-unconstrained fit");

		Trk::VxCandidate* myExtVxCandidate(0);
		if (m_useV0Fitter) {
		  myExtVxCandidate = concreteVertexFitter->fit(pairV0, vertex);
		} else {
		  myExtVxCandidate = m_iVKVertexFitter->fit(pairV0, vertex);
		}

		// protect against track sign change
                if (0 != myExtVxCandidate) {
                  const Trk::TrackParameters*  bPer0 = (*(myExtVxCandidate->vxTrackAtVertex()))[0]->perigeeAtVertex();
                  const Trk::TrackParameters*  bPer1 = (*(myExtVxCandidate->vxTrackAtVertex()))[1]->perigeeAtVertex();
                  double trkCharge0 = 1., trkCharge1 = 1.;
                  if (bPer0->parameters()[Trk::qOverP] < 0.) trkCharge0 = -1.;
                  if (bPer1->parameters()[Trk::qOverP] < 0.) trkCharge1 = -1.;
                  if ((trkCharge0*charge1 < 0.) || (trkCharge1*charge2 < 0.)) {
                    delete myExtVxCandidate;
                    myExtVxCandidate = 0;
                  }
                }

		if(myExtVxCandidate != 0) {
		  Trk::ExtendedVxCandidate* myVxCandidate = dynamic_cast<Trk::ExtendedVxCandidate* >(myExtVxCandidate);
      
		  if(myVxCandidate != 0) {
		    myVxCandidate->setVertexType(Trk::V0Vtx);

		    // check the chi^2 of the fit
		    if (m_V0Tools->vertexProbability( myVxCandidate ) >= m_minVertProb ) {

		      // unconstrained V0 hypothesis
		      Trk::V0Hypothesis* myUnconstrV0 = new Trk::V0Hypothesis(*myVxCandidate,0,0,0);
		      if (myUnconstrV0 != 0) {

			// store a V0Candidate
			std::vector<Trk::V0Hypothesis *> myV0Hyp;
			if(0!= myUnconstrV0) {
			  myV0Hyp.push_back(myUnconstrV0);
			}
        
			Trk::V0Candidate* myV0Candidate = new Trk::V0Candidate(myV0Hyp);
        
			// set LinkToTrackParticleBase
			std::vector<Trk::V0Hypothesis *>::iterator hb = (myV0Candidate->v0Hypothesis())->begin();
			std::vector<Trk::V0Hypothesis *>::iterator he = (myV0Candidate->v0Hypothesis())->end();
			for(;hb!=he;++hb) {
			  const std::vector<Trk::VxTrackAtVertex*>& trackAtVertexVec = *((*hb)->vxTrackAtVertex());
			  for (unsigned int i = 0; i < trackAtVertexVec.size(); i++) {
			    Trk::LinkToTrackParticleBase* newLinkToTrackPB = new Trk::LinkToTrackParticleBase();
			    if(i == 0) newLinkToTrackPB->setElement((*tpIt1));
			    if(i == 1) newLinkToTrackPB->setElement((*tpIt2));
			    newLinkToTrackPB->setStorableObject(*TPC);
			    trackAtVertexVec[i]->setOrigTrack(newLinkToTrackPB);
			  }
			} // end of loop over all hypothesis
			m_V0s_stored++;
			// check if correctly stored 
			if (msgLvl(MSG::DEBUG)) {
			  msg(MSG::DEBUG) << "found a V0" << endreq;
			  const Trk::V0Hypothesis* aUnconstr = m_V0Tools->v0Unconstr(myV0Candidate);
			  if(aUnconstr != 0) msg(MSG::DEBUG) << "V0Candidate UnconstrV0 prob: " << m_V0Tools->vertexProbability(aUnconstr) << endreq;
			}
			// write to the V0Container        
			theV0Container->push_back(myV0Candidate);
		      }
		    }  // chi2 cut
		  } // myVxCandidate is not zero
		  delete myExtVxCandidate;
		}
	      }  // doFit
	    }  // no suitable starting point found
	  }  // use tracks not associated to any vertex
          i2++;
	}  // loop over negative tracks
        i1++;
      }  // loop over positive tracks
    } // pos & neg track size > 0

    if ( theV0Container->size()==0) ATH_MSG_DEBUG("No Candidates found. Empty container returned");

    //---- Recording section: write the results to StoreGate ---//
    
    ATH_MSG_DEBUG("Recording to StoreGate: " << m_V0CandidatesOutputName << " size:" <<theV0Container->size());
    sc = evtStore()->record(theV0Container, m_V0CandidatesOutputName,false);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to record V0Container in TDS");
      return sc;
    }

    return StatusCode::SUCCESS;
  }// end execute block

  StatusCode InDetSimpleV0Finder::finalize()
  {
    msg(MSG::INFO)
      << "----------------------------------------------------------------------------------------------------------------------------------------------" << endreq
      << "\tSummary" << endreq 
      << "\tProcessed              : " << m_events_processed            << " events" << endreq
      << "\tStored                 : " << m_V0s_stored                  << " V0s" << endreq;
      msg(MSG::INFO) << "----------------------------------------------------------------------------------------------------------------------------------------------" << endreq;

    return StatusCode::SUCCESS;
  }

  StatusCode InDetSimpleV0Finder::resetStatistics() 
  {
    m_events_processed           = 0;
    m_V0s_stored                 = 0;

    return StatusCode :: SUCCESS;
  }


  bool InDetSimpleV0Finder::doFit(const Trk::TrackParameters* track1, const Trk::TrackParameters* track2, Amg::Vector3D &startingPoint)
  {
    bool pass = false;

    Amg::Vector3D globalPosition = startingPoint;
    Trk::PerigeeSurface perigeeSurface(globalPosition);

    const Trk::TrackParameters* chargePar1 = dynamic_cast<const Trk::TrackParameters*> (track1);
    const Trk::TrackParameters* chargePar2 = dynamic_cast<const Trk::TrackParameters*> (track2);

    if (chargePar1 != 0 && chargePar2 != 0) {
      const Trk::TrackParameters* extrapolatedPerigee1 = m_extrapolator->extrapolateDirectly(*chargePar1, perigeeSurface);
      if (extrapolatedPerigee1 == 0) {
        msg(MSG::DEBUG) << "extrapolation failed, using original perigee" << endreq;
        extrapolatedPerigee1 = chargePar1;
      }
      const Trk::TrackParameters* extrapolatedPerigee2 = m_extrapolator->extrapolateDirectly(*chargePar2, perigeeSurface);
      if (extrapolatedPerigee2 == 0) {
        msg(MSG::DEBUG) << "extrapolation failed, using original perigee" << endreq;
        extrapolatedPerigee2 = chargePar2;
      }
      if (extrapolatedPerigee1 != 0 && extrapolatedPerigee2 != 0) {
	pass = true;
      }
      delete extrapolatedPerigee1; delete extrapolatedPerigee2;
    }
  
    return pass;
  }

}//end of namespace InDet

