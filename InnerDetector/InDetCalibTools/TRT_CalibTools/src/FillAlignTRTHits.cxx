/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:
PACKAGE:

AUTHORS:
CREATED:

PURPOSE: Tool


********************************************************************/

// INCLUDES:
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrkParameters/TrackParameters.h"
#include "FillAlignTRTHits.h"
#include "TRT_CalibData/TrackInfo.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "EventPrimitives/EventPrimitives.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ConditionsData/RtRelation.h"
#include "CommissionEvent/ComTime.h"
#include "TrkToolInterfaces/IUpdator.h"

#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"
#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"

//#include "VxVertex/VxContainer.h"
#include "xAODTracking/VertexContainer.h"


FillAlignTRTHits::FillAlignTRTHits(const std::string& type, const std::string& name, const IInterface* parent) :
	AthAlgTool(type, name, parent),
	m_DetID(nullptr), m_TRTID(nullptr),
	//	m_driftFunctionTool("TRT_DriftFunctionTool"),
	m_trtcaldbSvc("ITRT_CalDbSvc", name),
	m_neighbourSvc("ITRT_StrawNeighbourSvc", name),
	m_TRTStrawSummarySvc("InDetTRTStrawStatusSummarySvc",name),
	m_maxDistance(2.8),
	m_maxTimeResidual(150),
	m_minTimebinsOverThreshold(2),
	m_maxTrackChisquarePerDof(10),
	m_numOfHitsTotal(0),
	m_numOfHitsAccepted(0),
	m_numOfProcessedTracks(0),
	m_DoMCCosmicTimeShift(0),
	m_updator(nullptr),
	m_f(nullptr), m_ntuple(nullptr)
{
	declareInterface<IFillAlignTrkInfo>(this);
	//	declareProperty("TRTDriftFunctionTool", m_driftFunctionTool);
	declareProperty("TRTCalDbSvc",m_trtcaldbSvc);
	declareProperty("NeighbourSvc",m_neighbourSvc);
	declareProperty("maxDistance",m_maxDistance) ;
	declareProperty("maxTimeResidual",m_maxTimeResidual) ;
	declareProperty("minTimebinsOverThreshold",m_minTimebinsOverThreshold) ;
	declareProperty("maxTrackChisquarePerDof",m_maxTrackChisquarePerDof) ;
	declareProperty("DoMCCosmicTimeShift",m_DoMCCosmicTimeShift);
	declareProperty("TRTStrawSummarySvc",  m_TRTStrawSummarySvc);
}

StatusCode FillAlignTRTHits::initialize(){
	msg(MSG::INFO) << "initialize() " << endmsg;
	if ((detStore()->retrieve(m_DetID,"AtlasID")).isFailure()) {
		msg(MSG::FATAL) << "Problem retrieving ATLASDetectorID helper" << endmsg;
		return StatusCode::FAILURE;
	}
	if ((detStore()->retrieve(m_TRTID)).isFailure()) {
		msg(MSG::FATAL) << "Problem retrieving TRTID helper" << endmsg;
		return StatusCode::FAILURE;
	}
	if(m_trtcaldbSvc.retrieve().isFailure()) {
		msg(MSG::FATAL) << "Could not get TRTCalDbSvc !" << endmsg;
		return StatusCode::FAILURE;
	}
	if(StatusCode::SUCCESS!=m_neighbourSvc.retrieve() ) {
		msg(MSG::FATAL) <<"Could not get TRTStrawNeighbourSvc !"<<endmsg;
		return StatusCode::FAILURE;
	}

	// To extract ToT Corrections:
	// Get DriftFunction tool servise
	//
	if ( m_driftFunctionTool.retrieve().isFailure() ) {
		msg(MSG::FATAL) << m_driftFunctionTool.propertyName() << ": Failed to retrieve tool " << m_driftFunctionTool.type() << endmsg;
		return StatusCode::FAILURE;
	} else {
		msg(MSG::INFO) << m_driftFunctionTool.propertyName() << ": Retrieved tool " << m_driftFunctionTool.type() << endmsg;
	}

	// use updator to get unbiased states
	if ( ! m_updatorHandle.empty() ) {
		StatusCode sc = m_updatorHandle.retrieve();
		if (sc.isFailure()) {
			msg(MSG::FATAL) << "Could not retrieve measurement updator tool: "<< m_updatorHandle << endmsg;
			return sc;
		}
		m_updator = &(*m_updatorHandle);
	} else {
		ATH_MSG_DEBUG ("No Updator for unbiased track states given, use normal states!");
		m_updator = 0;
	}

	// The tool to get the argon status:
	if (!m_TRTStrawSummarySvc.empty() && m_TRTStrawSummarySvc.retrieve().isFailure() ) {
		ATH_MSG_ERROR ("Failed to retrieve StrawStatus Summary " << m_TRTStrawSummarySvc);
		ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
		return StatusCode::FAILURE;
	} else {
		if ( !m_TRTStrawSummarySvc.empty()) msg(MSG::INFO) << "Retrieved tool " << m_TRTStrawSummarySvc << endmsg;
	}

	m_f = new TFile("basic.root","RECREATE");
	m_ntuple = new TNtuple("ntuple","TRT calibration ntuple","run:evt:lbn:nvx:trk:det:lay:mod:stl:stw:brd:chp:sid:locx:locy:locz:x:y:z:r:dr:t:rtrack:drrtrack:rtrackunbias:drrtrackunbias:ttrack:ttrackunbias:t0:ephase:phi:theta:pt:qoverp:d0:ToT:HT:ToTCorrection:HTCorrection:isArgonStraw");
	return StatusCode::SUCCESS;
}


StatusCode FillAlignTRTHits::finalize(){
	m_f->Write();
	m_f->Close();
	std::cout << "CALIBSTAT TRKS: " << m_numOfProcessedTracks << std::endl;
	std::cout << "CALIBSTAT HTOT: " << m_numOfHitsTotal << std::endl;
	std::cout << "CALIBSTAT HACC: " << m_numOfHitsAccepted << std::endl;
	return StatusCode::SUCCESS;
}


bool FillAlignTRTHits::fill(const Trk::Track* aTrack, TRT::TrackInfo* output,
                            const ComTime* theComTime, const xAOD::EventInfo& eventInfo,
                            const xAOD::VertexContainer& vertices) {

	++m_numOfProcessedTracks;
	float rtrackunbias = 0;
	float drrtrackunbias = 0;
	float drrtrack = 0;
	float ttrackunbias = 0;
	(*output)[TRT::Track::numberOfPixelHits] = 0;
	(*output)[TRT::Track::numberOfSCTHits] = 0;
	(*output)[TRT::Track::numberOfTRTHits] = 0;
	// loop over the TrackStateonSurfaces

	const Trk::TrackParameters *unbiasedTrkParameters(0);
	const Trk::TrackStateOnSurface* HitOnTrackToRemove(0);

	double timecor = 0.;
	const Trk::Track* pTrack = aTrack ;
	const Trk::Perigee* mesp = pTrack->perigeeParameters();
	float lbn = -1;
	float nvrt_rec = -1;
	double phi = 10;
	double theta = 100;
	double pt = 0;
	double qoverp = 0;
	double d0 = 0;

	if(mesp){
		phi   = mesp->parameters()[Trk::phi0];
		theta = mesp->parameters()[Trk::theta];
		float ptinv = fabs(mesp->parameters()[Trk::qOverP]) / sin(theta);
		qoverp = mesp->parameters()[Trk::qOverP];
		if (ptinv != 0) {
			pt = 1. / ptinv;
		} else {
			pt = 1e24;
		}
		d0 = mesp->parameters()[Trk::d0];
	}


	if (theComTime) {
		timecor = theComTime->getTime() + m_DoMCCosmicTimeShift ;
	}

	lbn = (float)eventInfo.lumiBlock();
	//Number of Prim vertex:
	/*
	  const VxContainer* vxContainer(0);
	  if ( StatusCode::SUCCESS ==  evtStore()->retrieve( vxContainer,"VxPrimaryCandidate")) {
	  nvrt_rec = (float) vxContainer->size();
	  } else {
	  ATH_MSG_ERROR("Could not get vertex container!");
	  }
	*/
	nvrt_rec = 0;
	int countVertices(0);
	for (const xAOD::Vertex* vx : vertices) {
		if (vx->vertexType() == xAOD::VxType::PriVtx) {
			if ( vx-> nTrackParticles() >= 3) countVertices++;
		}
	}
	nvrt_rec = countVertices;

	auto tsos = aTrack->trackStateOnSurfaces()->begin();
	auto tsosEnd = aTrack->trackStateOnSurfaces()->end();

	const Trk::MeasurementBase* mesb = nullptr;
	const Trk::RIO_OnTrack* rotp = nullptr;
	const InDet::TRT_DriftCircle* dcp = nullptr;
	const Trk::TrackParameters* tparp = nullptr;
	const Trk::TrackParameters* mparp = nullptr;
	const InDet::TRT_DriftCircleOnTrack* trtcirc = nullptr;
	const TRTCond::RtRelation* rtrelation = nullptr;


	for (; tsos != tsosEnd; ++tsos) {
		mesb = (*tsos)->measurementOnTrack();
		rotp = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
		if(rotp != 0) {
			Identifier ident = rotp->identify();
			if (m_DetID->is_sct(ident)) {
				(*output)[TRT::Track::numberOfSCTHits]++;
			} else if (m_DetID->is_trt(ident)) {
				(*output)[TRT::Track::numberOfTRTHits]++;
				++m_numOfHitsTotal;
				trtcirc = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(rotp);
				if (trtcirc != 0) {
					dcp = trtcirc->prepRawData();
					tparp = ((*tsos)->trackParameters());
					mparp = (tparp);

					if (tparp == 0) {
						if (msgLvl(MSG::DEBUG)) msg() << "strange: trk parameters not available" << endmsg;
					}
					if (dcp == 0) {
						msg(MSG::ERROR)  << "strange: prepRawData not available" << endmsg;
					}

					if (mparp && dcp) {
						TRT::HitInfo* newhit = new TRT::HitInfo();
						output->push_back(newhit); // do we make sure this one is deleted properly

						(*newhit)[TRT::Hit::ident] = ident.get_identifier32().get_compact();
						(*newhit)[TRT::Hit::detector] = m_TRTID->barrel_ec(ident);
						(*newhit)[TRT::Hit::layer] = m_TRTID->layer_or_wheel(ident);
						(*newhit)[TRT::Hit::phiModule] = m_TRTID->phi_module(ident);
						(*newhit)[TRT::Hit::strawLayer] = m_TRTID->straw_layer(ident);
						(*newhit)[TRT::Hit::straw] = m_TRTID->straw(ident);
						(*newhit)[TRT::Hit::side] = static_cast<int>(trtcirc->side());

						// Local wire specific
						(*newhit)[TRT::Hit::signedDriftRadius] = rotp->localParameters()[Trk::driftRadius];
						(*newhit)[TRT::Hit::errorSignedDriftRadius] = sqrt(Amg::error(rotp->localCovariance(),Trk::driftRadius)) ;
						bool isvalid = false;


						(*newhit)[TRT::Hit::driftTime] = dcp->driftTime(isvalid) - timecor;
						(*newhit)[TRT::Hit::driftTimeStatus] = isvalid ;
						// this is the integer drift time, but multiplied by the scale.
						// to calculate the residual in the drift time, I need to have the t0 as well.
						// I'd also like to store the drift velocity
						if (!isvalid) (*newhit)[TRT::Hit::driftTime] = -1.0;

						(*newhit)[TRT::Hit::t0] = m_trtcaldbSvc->getT0(ident) ;
						//(*newhit)[TRT::Hit::TimeoverThreshold]= dcp ? dcp->timeOverThreshold() : -1.0;
						(*newhit)[TRT::Hit::TimeoverThreshold] = dcp->timeOverThreshold() ;
						//CORRECT FOR TUBEHITS!!!:
						//const TRTCond::RtRelation*
						rtrelation = m_trtcaldbSvc->getRtRelation(ident) ;
						// added High Level Threshold information
						(*newhit)[TRT::Hit::HTLevel] = dcp->highLevel();
						// Extract the correction in the db for the ToT:
						float tot = (*newhit)[TRT::Hit::TimeoverThreshold];
						float ToTCorrection = m_driftFunctionTool->driftTimeToTCorrection(tot, ident); // (rawTime -= m_driftFunctionTool->driftTimeToTCorrection((*r)->timeOverThreshold(), id);     )

						// Extract the correction for HT:
						float HTCorrection = 0;
						if ((*newhit)[TRT::Hit::HTLevel]){
							HTCorrection = m_driftFunctionTool->driftTimeHTCorrection(ident); // (rawTime += m_driftFunctionTool->driftTimeHTCorrection(id);         )
						}

						(*newhit)[TRT::Hit::positionOnWire] = tparp->parameters()[Trk::locZ];

						(*newhit)[TRT::Hit::trackDriftRadius] = tparp->parameters()[Trk::driftRadius];
						(*newhit)[TRT::Hit::errorPositionOnWire] = sqrt(Amg::error(*(mparp->covariance()),Trk::locZ));
						(*newhit)[TRT::Hit::errorTrackDriftRadius] = sqrt(Amg::error(*(mparp->covariance()),Trk::driftRadius));
						// calculate the 'trktime' and the 'trkdriftvelocity'
						if( rtrelation ) {
							(*newhit)[TRT::Hit::trackDriftTime] = rtrelation->drifttime(fabs( (*newhit)[TRT::Hit::trackDriftRadius] )) ;
							(*newhit)[TRT::Hit::driftVelocity] = rtrelation->drdt( (*newhit)[TRT::Hit::trackDriftTime] ) ;
						}

						(*newhit)[TRT::Hit::trackT0]= timecor ;

						int chip = 0;
						int board = -1;
						m_neighbourSvc->getChip(ident,chip);
						if(abs(m_TRTID->barrel_ec(ident))<2){
							board = m_neighbourSvc->chipToBoardBarrel(chip, m_TRTID->layer_or_wheel(ident));
						} else if (chip<12) {
							board = 0;
						} else {
							chip = chip-20;
							board = 1;
						}

						// Prepare for Xe-Ar mixed conditions:
						int isArgonStraw = 0;
						if (!m_TRTStrawSummarySvc.empty()) {
							if (m_TRTStrawSummarySvc->getStatusHT(ident) != TRTCond::StrawStatus::Good) {
								isArgonStraw = 1;
							}
						}


						float h_trkDistance = (*newhit)[TRT::Hit::trackDriftRadius];

						float h_driftTime = (*newhit)[TRT::Hit::driftTime] - (*newhit)[TRT::Hit::t0];
						float h_trkDriftTime = (*newhit)[TRT::Hit::trackDriftTime];
						float h_timeResidual = h_driftTime - h_trkDriftTime;

						float h_trkVariance = (*newhit)[TRT::Hit::errorTrackDriftRadius] * (*newhit)[TRT::Hit::errorTrackDriftRadius];

						bool h_hasValidDriftTime = (*newhit)[TRT::Hit::driftTimeStatus] ;

						float h_timeOverThreshold = (*newhit)[TRT::Hit::TimeoverThreshold] ;

						float h_residual = (*newhit)[TRT::Hit::signedDriftRadius] - (*newhit)[TRT::Hit::trackDriftRadius] ;
						float h_residualVariance = h_trkVariance + ((*newhit)[TRT::Hit::errorSignedDriftRadius] * (*newhit)[TRT::Hit::errorSignedDriftRadius]);
						float h_chiSquare = h_residual*h_residual/h_residualVariance ;

						bool hitsel=false;
						if( fabs( h_trkDistance )  < m_maxDistance &&
						    fabs( h_timeResidual ) < m_maxTimeResidual &&
						    h_trkVariance > 0 &&
						    h_hasValidDriftTime &&
						    h_timeOverThreshold/3.125 >= m_minTimebinsOverThreshold &&
						    ((*output)[TRT::Track::chiSquare] - h_chiSquare) / ((*output)[TRT::Track::degreesOfFreedom] - 1) < m_maxTrackChisquarePerDof ){
							hitsel = true;
						}

						/// INCLUDE TO HAVE UNBIAS RESIDUAL!!
						rtrackunbias    = 0;
						drrtrackunbias  = 0;
						drrtrack        = sqrt(Amg::error(*(mparp->covariance()),Trk::driftRadius));
						ttrackunbias    = 0;

						if (m_updator){
							tparp = ((*tsos)->trackParameters());
							HitOnTrackToRemove = *tsos;

							if(HitOnTrackToRemove){
								unbiasedTrkParameters = m_updator->removeFromState(*(HitOnTrackToRemove->trackParameters()),
								                                                   HitOnTrackToRemove->measurementOnTrack()->localParameters(),
								                                                   HitOnTrackToRemove->measurementOnTrack()->localCovariance());
								ATH_MSG_DEBUG ("TrackParameters 1: " << *(HitOnTrackToRemove->trackParameters()));
							}
							else if (msgLvl(MSG::DEBUG)) {
								msg() << "TrackParameters 1: nullptr" << endmsg;
							}

							if(unbiasedTrkParameters){
								const Trk::TrackParameters *unmparp = (unbiasedTrkParameters);
								rtrackunbias  = unbiasedTrkParameters->parameters()[Trk::driftRadius];
								drrtrackunbias = sqrt(Amg::error(*(unmparp->covariance()),Trk::driftRadius));
								//drrtrackunbias = sqrt(unbiasedTrkParameters->localErrorMatrix().covValue(Trk::driftRadius));
								if( rtrelation )  ttrackunbias = rtrelation->drifttime(fabs( rtrackunbias ));
								ATH_MSG_DEBUG("Unbiased TrackParameters 2: " << *unbiasedTrkParameters );
								ATH_MSG_DEBUG("Radius : " << (*newhit)[TRT::Hit::trackDriftRadius] );
								ATH_MSG_DEBUG("Radius 2: " << rtrackunbias );
							}

						}
						/// END INCLUDE TO HAVE UNBIAS RESIDUAL!!
						float const ntvar[40]={
							(float)(*output)[TRT::Track::run],
							(float)(*output)[TRT::Track::event],
							lbn,
							nvrt_rec,
							(float)(*output)[TRT::Track::trackNumber],

							(float)(*newhit)[TRT::Hit::detector],
							(float)(*newhit)[TRT::Hit::layer],
							(float)(*newhit)[TRT::Hit::phiModule],
							(float)(*newhit)[TRT::Hit::strawLayer],
							(float)(*newhit)[TRT::Hit::straw],
							(float)board,
							(float)chip,
							(float)m_TRTID->straw_id((*newhit)[TRT::Hit::detector],
							                         (*newhit)[TRT::Hit::phiModule],
							                         (*newhit)[TRT::Hit::layer],
							                         (*newhit)[TRT::Hit::strawLayer],
							                         (*newhit)[TRT::Hit::straw]).get_identifier32().get_compact() ,
							(float)tparp->position().x(),
							(float)tparp->position().y(),
							(float)tparp->position().z(),
							(float)(rotp->detectorElement()->center(ident)).x(),
							(float)(rotp->detectorElement()->center(ident)).y(),
							(float)(rotp->detectorElement()->center(ident)).z(),

							(*newhit)[TRT::Hit::signedDriftRadius],
							(*newhit)[TRT::Hit::errorSignedDriftRadius],
							(*newhit)[TRT::Hit::driftTime],
							(*newhit)[TRT::Hit::trackDriftRadius],
							drrtrack,
							rtrackunbias,
							drrtrackunbias,
							(*newhit)[TRT::Hit::trackDriftTime],
							ttrackunbias,
							m_trtcaldbSvc->getT0(ident),
							(float)timecor,
							(float)phi ,
							(float)theta,
							(float)pt ,
							(float)qoverp ,
							(float)d0  ,
							(*newhit)[TRT::Hit::TimeoverThreshold],
							(*newhit)[TRT::Hit::HTLevel],
							ToTCorrection,
							HTCorrection,
							(float)isArgonStraw
						};

						if (hitsel) {
							++m_numOfHitsAccepted;
							m_ntuple->Fill(ntvar);
						}

					}
				} else {
					msg(MSG::ERROR) << "TRT drift RIO cast failed - no hit stored" << endmsg;
				}


			} // identified TRT hit
			else if (m_DetID->is_pixel(ident)) (*output)[TRT::Track::numberOfPixelHits]++;
		} // non-zero ROTpointer
	} // end loop on Surfaces
	if (msgLvl(MSG::VERBOSE)) msg() << "Track has " << (*output)[TRT::Track::numberOfTRTHits] << " TRT hits --> of which "
	                                << output->size() << " hits had FULL info available" << endmsg;



	delete unbiasedTrkParameters;
	if(msgLvl(MSG::INFO)) msg() << "Delete all : " << endmsg;


	return true;
}
