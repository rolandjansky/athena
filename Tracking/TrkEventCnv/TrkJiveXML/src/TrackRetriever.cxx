/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkJiveXML/TrackRetriever.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"


#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkEventPrimitives/JacobianThetaPToCotThetaPt.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTruthData/TrackTruthKey.h"
#include "TrkEventUtils/TrackStateOnSurfaceComparisonFunction.h"
#include "AthLinks/ElementLink.h"
#include "AthContainers/DataVector.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

#include "TrkMeasurementBase/MeasurementBase.h"

// for residuals
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkEventPrimitives/ResidualPull.h"

// for detector id
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "GaudiKernel/SystemOfUnits.h"

namespace JiveXML {
  ///Namespace for all helper functions
  namespace TrackRetrieverHelpers {

		/**
		* Obtain the perigee paramets for a given track, if available, and fill
		* them in the corresponding data vectors. Perigee parameters are written
		* out in the old format using @f$ \cot\theta @f$ and @f$ q/p_T @f$
		* @return the perigee parameter object for further use
		*/
		const Trk::Perigee* getPerigeeParameters( const Trk::Track* track,
		    DataVect& pt, DataVect& d0, DataVect& z0, DataVect& phi0,
		    DataVect& cotTheta, DataVect& covMatrix){

			/**
			* Get perigee parameters in old format (@f$ d_0 @f$, @f$ z_0 @f$, @f$ \phi @f$, @f$ \cot\theta @f$, @f$ q/p_T @f$),
			* whereas tracking uses (@f$ d_0 @f$, @f$ z_0 @f$, @f$ \phi @f$, @f$ \theta @f$, q/p),
			* therefore a transformation of the covariance matrix is needed
			*/
			const Trk::Perigee *perigee = track->perigeeParameters();

			//return immediately if there is no perigee information
			if (! perigee) return nullptr ;

			//write out p_T
			if ((perigee->parameters())[Trk::qOverP]==0) pt.push_back(DataType(9999.));
			else pt.push_back( (perigee->charge() > 0) ? DataType(perigee->pT()/Gaudi::Units::GeV) : DataType((-perigee->pT())/Gaudi::Units::GeV));

			d0.push_back(DataType((perigee->parameters())[Trk::d0]/Gaudi::Units::cm));
			z0.push_back(DataType(perigee->parameters()[Trk::z0]/Gaudi::Units::cm));
			phi0.push_back(DataType(perigee->parameters()[Trk::phi0]));

			if (perigee->parameters()[Trk::theta] == 0.) cotTheta.push_back(DataType(9999.));
			else cotTheta.push_back(DataType(1./tan(perigee->parameters()[Trk::theta])));

			// CLHEP->Eigen migration. jpt Dec'13
			// https://twiki.cern.ch/twiki/bin/viewauth/Atlas/MigrationCLHEPtoEigen
			// https://twiki.cern.ch/twiki/bin/viewauth/Atlas/MigrationToUpdatedEDM#Changes_to_TrkParameters

			/// get transformed covariance matrix
			AmgSymMatrix(5) covVert;

			const AmgSymMatrix(5)* covariance = perigee->covariance(); //perigee cannot be null here
			if (perigee && covariance) {
				// do trafo to old format
				double measuredTheta = perigee->parameters()[Trk::theta];
				double measuredQoverp = perigee->parameters()[Trk::qOverP];
				const Trk::JacobianThetaPToCotThetaPt theJac( measuredTheta, measuredQoverp );
				covVert = covariance->similarity(theJac);
			}else{
				for ( int ii=0; ii<20; ii++){ // placeholder. Do this nicer.
					covVert(ii) = 0.;
				}
			}
			//Scale covariance matrix values to get good resolution with fixed
			//precision in JiveXML data

			const long scale = 10000;
			const double thisScale(scale/100.);
			// Migration: Now only has diagonal elements from covariance matrix ?
			covMatrix.push_back(DataType(covVert(0)*thisScale)); // 5 elements
			covMatrix.push_back(DataType(covVert(1)*thisScale));
			covMatrix.push_back(DataType(covVert(2)*thisScale));
			covMatrix.push_back(DataType(covVert(3)*thisScale));
			covMatrix.push_back(DataType(covVert(4)*thisScale));

			// Used to be 15 elements before migration, so need to put 10 placeholders
			for ( int i=0; i<10; i++){
				covMatrix.push_back(DataType( 0. ));
			}

		//All for perigee, return object for use by other functions
		return perigee ;
		}

		/**
		* Get a list of track-State on Surfaces for measurement and
		* outlier hits, sorted using the perigee comparison functions
		* @return a std::vector of Trk::TrackStateOnSurface*
		*/
		std::vector<const Trk::TrackStateOnSurface*> getTrackStateOnSurfaces( const Trk::Track* track, const Trk::Perigee* perigee, bool doHitsSorting){
			// vector for the return object
			std::vector<const Trk::TrackStateOnSurface*> TSoSVec;

			// loop over TrackStateOnSurfaces to extract interesting ones
			DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos = track->trackStateOnSurfaces()->begin();
			for (; tsos!=track->trackStateOnSurfaces()->end(); ++tsos) {
				// include measurements AND outliers:
				if ((*tsos)->type(Trk::TrackStateOnSurface::Measurement) ||  (*tsos)->type(Trk::TrackStateOnSurface::Outlier) ) {
					// add to temp vector
					TSoSVec.push_back(*tsos);
				} // end if TSoS is measurement or outlier
			} // end loop over TSoS

			// sort the selected TSoS, if not already sorted using a comparison functor

			if (perigee) {
				//Get hold of the comparison functor
				Trk::TrackStateOnSurfaceComparisonFunction *compFunc
				    = new Trk::TrackStateOnSurfaceComparisonFunction(perigee->position(), perigee->momentum(\
				    ));
				if (compFunc){
					if (doHitsSorting) {
						//Sort track state on surface if needed
						if (TSoSVec.size() > 2 && !is_sorted(TSoSVec.begin(), TSoSVec.end(), *compFunc))
						  std::sort(TSoSVec.begin(), TSoSVec.end(), *compFunc);
					}
				}
				delete compFunc;
			} // end if compFunc

			//Now return that vector
			return TSoSVec;
		}

		/**
		* Get polyline hits if available. Polyline tracks that have less than 2 points are not useful - skip
		*/
		void getPolylineFromHits( const std::vector<const Trk::TrackStateOnSurface*>& TSoSVec,
		    DataVect& polylineX, DataVect& polylineY, DataVect& polylineZ, DataVect& numPolyline){
			int numPoly = 0 ;
			if (TSoSVec.size() > 1) {
				//Loop over track state on surfaces
				std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsosIter;
				const double onetenth(0.1);
				for (tsosIter=TSoSVec.begin(); tsosIter!=TSoSVec.end(); ++tsosIter) {
					// get global track position
					if (!(*tsosIter)->trackParameters()) continue ;
					const Amg::Vector3D& pos = (*tsosIter)->trackParameters()->position();
					polylineX.push_back(DataType(pos.x()*onetenth));
					polylineY.push_back(DataType(pos.y()*onetenth));
					polylineZ.push_back(DataType(pos.z()*onetenth));
					++numPoly;
				}
			}
			//Store counter as well
			numPolyline.push_back(DataType(numPoly));
		}


		/**
		* Retrieve all the basic hit information from the Trk::TrackStateOnSurface
		* @return the reconstruction input object (RIO) for further use
		*/
		const Trk::RIO_OnTrack* getBaseInfoFromHit( const Trk::TrackStateOnSurface* tsos, const AtlasDetectorID* idHelper,
		  DataVect& isOutlier, DataVect& hits, DataVect& driftSign, DataVect& tsosDetType){

			//Get corresponding measurement
			const Trk::MeasurementBase *measurement = tsos->measurementOnTrack();

			//If measurement is invalid, return imediately
			if ( ! measurement ) return nullptr ;

			// get RIO_OnTrack
			const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(measurement);

			// check for competing RIO_OnTracks
			if (!rot) {
				// try to get identifier by CompetingROT:
				const Trk::CompetingRIOsOnTrack* comprot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(measurement);
				//Get the input object with highest probability
				if (comprot) rot = &(comprot->rioOnTrack(comprot->indexOfMaxAssignProb()));
			}

			//If there is still no RIO_onTrack, return Null
			if (!rot) return nullptr ;

			// Now start writing out values:
			// Check if this is an outlier hit
			isOutlier.push_back(DataType(tsos->type(Trk::TrackStateOnSurface::Outlier)));

			//Now try to get the identifier, create an empty invalid one if no rot
			Identifier hitId (rot->identify());
			//Check if it is valid, othwerise store 0
			hits.push_back( hitId.is_valid()?DataType( hitId.get_compact() ):DataType(0) );

			// get sign of drift radius for TRT measurements
			int theDriftSign = 0;
			if (idHelper->is_trt(hitId)) {
				// get local parameters
				theDriftSign = measurement->localParameters()[Trk::driftRadius] > 0. ? 1 : -1;
			}
			driftSign.push_back(DataType(theDriftSign));

			//Now get the detector type of the hit
			if ( !hitId.is_valid() ){
				tsosDetType.push_back(DataType("unident"));
			} else if (idHelper->is_pixel(hitId) ) {
				tsosDetType.push_back(DataType("PIX")); // is PIX in Atlantis
			} else if (idHelper->is_sct(hitId)) {
				tsosDetType.push_back(DataType("SIL")); // is SIL in Atlantis
			} else if (idHelper->is_trt(hitId)) {
				tsosDetType.push_back(DataType("TRT"));
			} else if (idHelper->is_mdt(hitId)) {
				tsosDetType.push_back(DataType("MDT"));
			} else if (idHelper->is_csc(hitId)) {
				tsosDetType.push_back(DataType("CSC"));
			} else if (idHelper->is_rpc(hitId)) {
				tsosDetType.push_back(DataType("RPC"));
			} else if (idHelper->is_tgc(hitId)) {
				tsosDetType.push_back(DataType("TGC"));
			} else {
				tsosDetType.push_back(DataType("unident"));
			}

			//Return the reco input object
			return rot;
		}


		/**
		* Get the residual pull information from the Trk::TrackStateOnSurface hit
		*/
		void getResidualPullFromHit( const Trk::TrackStateOnSurface* tsos, const Trk::RIO_OnTrack* rot,
		    const ToolHandle<Trk::IResidualPullCalculator> & residualPullCalculator,
		    DataVect& tsosResLoc1, DataVect& tsosResLoc2, DataVect& tsosPullLoc1, DataVect& tsosPullLoc2 ){

			//Define default return values for invalid states
			double ResLoc1 = -99.;
			double ResLoc2 = -99.;
			double PullLoc1 = -99.;
			double PullLoc2 = -99.;

			// get TrackParameters on the surface to calculate residual
			const Trk::TrackParameters* tsosParameters = tsos->trackParameters();

			//Check we got the parameters
			if (tsosParameters){

				/**
				* Using track residual tool: ResidualPullCalculator
				* -excerpt from Tracking/TrkValidation/TrkValTools/src/BasicValidationNtupleTool.cxx
				*/

				//Get the residualPull object
				const Trk::ResidualPull* residualPull = residualPullCalculator->residualPull(rot, tsosParameters,Trk::ResidualPull::Biased);

				if (residualPull) {
					//Get the first residual
					ResLoc1 = residualPull->residual()[Trk::loc1];
					//Get the second residual for more than 1 dimension
					if (residualPull->dimension() >= 2) ResLoc2 = residualPull->residual()[Trk::loc2];

					if ((residualPull->isPullValid()) ) {
						//Get the first residual
						PullLoc1 = residualPull->pull()[Trk::loc1];
						//Get the second residual for more than 1 dimension
						if (residualPull->dimension() >= 2) PullLoc2 = residualPull->pull()[Trk::loc2];
					}
				} // end if residualPull
			} // end if tsosParameters

			//Finally store the values
			tsosResLoc1.push_back( ResLoc1 );
			tsosResLoc2.push_back( ResLoc2 );
			tsosPullLoc1.push_back( PullLoc1 );
			tsosPullLoc2.push_back( PullLoc2 );
		}

		/**
		* Get the barcode of the associated truth track
		*/
		void getTruthFromTrack( const Trk::Track* track, const TrackCollection* trackCollection,
		    const TrackTruthCollection* truthCollection, DataVect& barcode){

			if (!truthCollection) {
				//Fill with zero if none found
				barcode.push_back(DataType(0));
				return ;
			}

			//Get the element link to this track collection
			ElementLink<TrackCollection> tracklink;
			tracklink.setElement(track);
			tracklink.setStorableObject(*trackCollection);

			//try to find it in the truth collection
			std::map<Trk::TrackTruthKey,TrackTruth>::const_iterator tempTrackTruthItr = truthCollection->find(tracklink);

			//Fill with zero if none found
			if (tempTrackTruthItr == truthCollection->end()){
				//Fill with zero if none found
				barcode.push_back(DataType(0));
				return ;
		  }

			//if found, Store the barcode
			barcode.push_back(DataType((*tempTrackTruthItr).second.particleLink().barcode()));
		}

		} //namespace TrackRetrieverHelpers

		/**
		* This is the standard AthAlgTool constructor
		* @param type   AlgTool type name
		* @param name   AlgTool instance name
		* @param parent AlgTools parent owning this tool
		**/
		TrackRetriever::TrackRetriever(const std::string& type,const std::string& name,const IInterface* parent):
		  AthAlgTool(type,name,parent),
		  m_typeName("Track"),
		  m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
		  m_idHelper(nullptr) {
				//Declare the interface
				declareInterface<IDataRetriever>(this);
				m_trackSumTool = ToolHandle<Trk::ITrackSummaryTool>("Trk::TrackSummaryTool/InDetTrackSummaryTool");
				//Properties
				declareProperty("ResidualPullCalculator" , m_residualPullCalculator, "ToolHandle to ResidualPullCaclulator" );
				declareProperty("PriorityTrackCollection", m_PriorityTrackCollection = "CombinedInDetTracks", "Track collections to retrieve first, shown as default in Atlantis");
				declareProperty("OtherTrackCollections"   , m_OtherTrackCollections , "Track collections to retrieve, all if empty");
				declareProperty("TrackTruthColName"       , m_TrackTruthCollection = "TrackTruthCollection",
				"Track collection from which to retrieve the truth associations for the priority track collection");
				declareProperty("DoWriteHLT"              , m_doWriteHLT = false, "Whether to write HLTAutoKey objects");
				declareProperty("DoWriteResiduals"        , m_doWriteResiduals = true, "Whether to write TrackResiduals");
				declareProperty("DoHitsSorting"    , m_doHitsSorting = false, "Whether to sort hits (TrackStateOnSurfaces)");
				declareProperty("DoHitsDetails"    , m_doHitsDetails = true, "Whether to write hits details (TrackStateOnSurfaces)");
		}

		/**
		* Initialize before event loop
		* - retrieve the residual-pull tool
		* - setup the ID helper
		*/
		StatusCode TrackRetriever::initialize() {
			//Set up ATLAS ID helper to be able to identify the RIO's det-subsystem.
			if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
			  msg(MSG::FATAL) << "Could not get AtlasDetectorID helper" << endmsg;
			return StatusCode::FAILURE;
			}
			// try to retrieve residual-pull calculation only if requested
			if (m_doWriteResiduals){
				if ( m_residualPullCalculator.retrieve().isFailure()) {
					//Give a warning
					ATH_MSG_WARNING( "Cannot retrieve ResidualPullCalculator tool " << m_residualPullCalculator);
					ATH_MSG_WARNING( "  -> will not write out residuals and pulls! " );
					//switch of residual writing
					m_doWriteResiduals = false ;
				}
			}
			//All fine

			// get TrackSummaryTool
			if ( m_trackSumTool.retrieve().isFailure() ) {
			  ATH_MSG_WARNING(  "Failed to retrieve tool " << m_trackSumTool );
			} else {
			  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved tool " << m_trackSumTool << endmsg;
			}
			return StatusCode::SUCCESS;
		}



		/**
		* For each track collection retrieve all data
		* - loop over tracks in all collections
		* - for each track get basic parameters
		* @param FormatTool the tool that will create formated output from the DataMap
		*/
		StatusCode TrackRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
			//be verbose
			if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving " << dataTypeName() << endmsg;
			//Generate a list of requested track collections
			typedef std::pair< TrackCollection , std::string > tracksNamePair;
			std::vector< tracksNamePair > requestedTrackColls;
			//First try to get hold of the priorty track collection
			const TrackCollection* tracks = NULL ;
			if (evtStore()->retrieve(tracks, m_PriorityTrackCollection).isFailure()){
				ATH_MSG_WARNING( "Unable to retrieve requested priority track collection "<< m_PriorityTrackCollection);
			} else {
				//Add this to the list of requested collections
				requestedTrackColls.push_back(tracksNamePair(*tracks,m_PriorityTrackCollection));
			}

			//If we have been given an explicit list, try to retrieve these in the order
			//they were given
			std::vector<std::string>::iterator CollNameItr = m_OtherTrackCollections.begin();
			for ( ; CollNameItr != m_OtherTrackCollections.end(); ++CollNameItr){
				const TrackCollection* tracks = NULL ;
				if (evtStore()->retrieve(tracks, (*CollNameItr)).isFailure()){
					ATH_MSG_WARNING( "Unable to retrieve requested track collection " << (*CollNameItr) );
				continue ;
		    }
				//skip if it's priority collection again
				if ((*CollNameItr) == m_PriorityTrackCollection){ continue; }
				//Add them to the list of requested collections
				requestedTrackColls.push_back(tracksNamePair(*tracks,(*CollNameItr)));
			} // end OtherCollections loop
			//If no collections had been requested explicitly, loop over all of them
			if (m_OtherTrackCollections.empty()) {
				//Get an iterator over all other track collections
				const DataHandle<TrackCollection> trackCollIter, trackCollEnd;
				if ((evtStore()->retrieve(trackCollIter, trackCollEnd)).isFailure()){
					ATH_MSG_ERROR( "Unable to retrieve track collection iterator" );
					return StatusCode::RECOVERABLE;
				}

			//Next loop over all collections
			for (; trackCollIter!=trackCollEnd; trackCollIter++) {
				//Check if this is an HLT-AutoKey collection
				if ((trackCollIter.key().find("HLT",0) != std::string::npos) && (!m_doWriteHLT)){
				 ATH_MSG_DEBUG( "Ignoring HLT-AutoKey collection " << trackCollIter.key());
				 continue ;
			  }
				//Ignore TRTSeed and MuonSlimmedTrackCollections
				if ( (trackCollIter.key()=="TRTSeeds") || (trackCollIter.key() =="MuonSlimmedTrackCollection")) {
					ATH_MSG_DEBUG( "Always ignoring collection " << trackCollIter.key() );
					continue ;
				}
				//Next try to retrieve the actual collection
				if (evtStore()->retrieve(tracks,trackCollIter.key()).isFailure()){
					ATH_MSG_DEBUG( "Unable to retrieve collection " << trackCollIter.key());
					continue ;
				}
				//Check if this collection is not already in the list
				if (std::find(requestedTrackColls.begin(), requestedTrackColls.end(),tracksNamePair(*tracks,trackCollIter.key())) != requestedTrackColls.end())
				continue ;

				//Add this to the list of requested collections
				requestedTrackColls.push_back(tracksNamePair(*tracks,trackCollIter.key()));
			} //loop over all track collections
		}

		/**
		* Second step:
		* Now loop over all collections a retrieve the actual values
		**/

		//Loop over the collection list we have assembled above
		std::vector<tracksNamePair>::iterator tracksNamePairItr = requestedTrackColls.begin();
		for ( ; tracksNamePairItr != requestedTrackColls.end(); ++tracksNamePairItr){
			//save some typing by getting a handle on the collection pointer
			const TrackCollection* trackCollection =&((*tracksNamePairItr).first);
			std::string collectionName = (*tracksNamePairItr).second;
			//Some sanity checks
			if ( trackCollection->size() == 0){
			  ATH_MSG_DEBUG( "Ignoring empty track collection " << collectionName );
			} else {
			  ATH_MSG_DEBUG( "Retrieving data for track collection " << collectionName);
			}

			/**
			* Try to find the appropiate truth collection
			* StoreGate key can be either 'TruthCollection' or just 'Truth' suffix. Check both.
			* Using 'SG::contains' check first, avoids spitting out SG warnings
			*/
			const TrackTruthCollection *truthCollection = NULL ;
			//Check for given truth collection
			if ( collectionName == m_PriorityTrackCollection ){
				if ( evtStore()->contains<TrackTruthCollection>(m_TrackTruthCollection) ){
					evtStore()->retrieve(truthCollection, m_TrackTruthCollection).ignore();
					ATH_MSG_DEBUG( "Found TrackTruthCollection with key " << m_TrackTruthCollection );
				}
			//Check for name+TruthCollection
			} else if ( evtStore()->contains<TrackTruthCollection>(collectionName+"TruthCollection") ){
				evtStore()->retrieve(truthCollection, collectionName+"TruthCollection").ignore();
			  ATH_MSG_DEBUG( "Found TrackTruthCollection with key " <<  collectionName << "TruthCollection" );
			//Check for name+Truth
			} else if ( evtStore()->contains<TrackTruthCollection>(collectionName+"Truth") ){
				evtStore()->retrieve(truthCollection, collectionName+"Truth").ignore();
			  ATH_MSG_DEBUG( "Found TrackTruthCollection with key " <<  collectionName << "Truth" );
			// No matching track truth collection found at all
			} else {
			  ATH_MSG_DEBUG( "Could not find matching TrackTruthCollection for " << collectionName );
			  truthCollection = nullptr ;
		  }

			// Make a list of track-wise entries and reserve enough space
			DataVect id; id.reserve(trackCollection->size());
			DataVect chi2; chi2.reserve(trackCollection->size());
			DataVect numDoF; numDoF.reserve(trackCollection->size());
			DataVect trackAuthor; trackAuthor.reserve(trackCollection->size());
			DataVect barcode; barcode.reserve(trackCollection->size());
			DataVect numHits; numHits.reserve(trackCollection->size());
			DataVect numPolyline; numPolyline.reserve(trackCollection->size());
			DataVect nBLayerHits; nBLayerHits.reserve(trackCollection->size());
			DataVect nPixHits; nPixHits.reserve(trackCollection->size());
			DataVect nSCTHits; nSCTHits.reserve(trackCollection->size());
			DataVect nTRTHits; nTRTHits.reserve(trackCollection->size());

			// vectors with measurement- or TrackStateOnSurface-wise entries
			// reserve space later on
			DataVect polylineX;
			DataVect polylineY;
			DataVect polylineZ;
			DataVect tsosResLoc1;
			DataVect tsosResLoc2;
			DataVect tsosPullLoc1;
			DataVect tsosPullLoc2;
			DataVect tsosDetType;
			DataVect isOutlier;
			DataVect driftSign;
			DataVect hits;

			//Store wether this collection has perigee parameters
			DataVect pt; pt.reserve(trackCollection->size());
			DataVect d0; d0.reserve(trackCollection->size());
			DataVect z0; z0.reserve(trackCollection->size());
			DataVect phi0; phi0.reserve(trackCollection->size());
			DataVect cotTheta; cotTheta.reserve(trackCollection->size());
			//Covariance matrix has 15 entries per track
			DataVect covMatrix; covMatrix.reserve(trackCollection->size() * 15 );

			// Now loop over all tracks in the collection
			TrackCollection::const_iterator track;
			for (track=trackCollection->begin(); track!=trackCollection->end(); ++track) {
				/**
				* General track fit info
				*/
				id.push_back(DataType(id.size())); //<! simple counter starting from 0
				chi2.push_back(DataType((*track)->fitQuality()->chiSquared()));
				numDoF.push_back(DataType((*track)->fitQuality()->numberDoF()));
				trackAuthor.push_back(DataType((*track)->info().trackFitter()));

				/**
				* Get truth Information
				*/
				TrackRetrieverHelpers::getTruthFromTrack(*track,trackCollection,truthCollection,barcode);

				/**
				* Get Perigee parameters (if available)
				*/
				const Trk::Perigee* perigee = TrackRetrieverHelpers::getPerigeeParameters(*track, pt, d0, z0, phi0, cotTheta, covMatrix);

				/**
				* Get number of Pix/SCT/TRT hits
				*/
        std::unique_ptr<Trk::TrackSummary> summary = nullptr;
				summary = m_trackSumTool->summary(**track);

				if(not summary){
					ATH_MSG_DEBUG( "Track summary is NULL " );
					nBLayerHits.push_back(DataType(0));
					nPixHits.push_back(DataType(0));
					nSCTHits.push_back(DataType(0));
					nTRTHits.push_back(DataType(0));
				}else{
					nBLayerHits.push_back(DataType(summary->get(Trk::numberOfInnermostPixelLayerHits)));
					nPixHits.push_back(DataType(summary->get(Trk::numberOfPixelHits)));
					nSCTHits.push_back(DataType(summary->get(Trk::numberOfSCTHits)));
					nTRTHits.push_back(DataType(summary->get(Trk::numberOfTRTHits)));
				}

				/**
				* Get sorted list of track state on surfaces
				*/
				// Vector of interesting TrackStateOnSurfaces
				std::vector<const Trk::TrackStateOnSurface*> TSoSVec = TrackRetrieverHelpers::getTrackStateOnSurfaces(*track,perigee,m_doHitsSorting);

				/**
				* Get polyline information
				*/
				// Reserving some space for polyline hits
				polylineX.reserve(polylineX.size()+TSoSVec.size());
				polylineY.reserve(polylineY.size()+TSoSVec.size());
				polylineZ.reserve(polylineZ.size()+TSoSVec.size());

				//And fill them
				TrackRetrieverHelpers::getPolylineFromHits(TSoSVec,polylineX,polylineY,polylineZ,numPolyline);

				/**
				* RIO association and outlier id
				*/
				//Reserve some space for resPull and other hit info
				isOutlier.reserve(isOutlier.size()+TSoSVec.size());
				hits.reserve(hits.size()+TSoSVec.size());
				driftSign.reserve(driftSign.size()+TSoSVec.size());
				tsosResLoc1.reserve(tsosResLoc1.size()+TSoSVec.size());
				tsosResLoc2.reserve(tsosResLoc2.size()+TSoSVec.size());
				tsosPullLoc1.reserve(tsosPullLoc1.size()+TSoSVec.size());
				tsosPullLoc2.reserve(tsosPullLoc2.size()+TSoSVec.size());
				tsosDetType.reserve(tsosDetType.size()+TSoSVec.size());

				//Now loop over tracks and fill them
				std::vector< const Trk::TrackStateOnSurface* >::const_iterator TSoSItr = TSoSVec.begin();
				//Count number of hits stored in this loop
				long nHits = 0;
				if (m_doHitsDetails){ // disable only for HeavyIons !
					for (; TSoSItr != TSoSVec.end(); ++TSoSItr){
						// This produces the full long debug dump for TSoS:
						ATH_MSG_VERBOSE( (**TSoSItr) );
						//Get the basic hit information
						const Trk::RIO_OnTrack* rot = TrackRetrieverHelpers::getBaseInfoFromHit(*TSoSItr, m_idHelper, isOutlier, hits, driftSign, tsosDetType );
						//tell if this didn't work out
						if (!rot){
							ATH_MSG_VERBOSE( "Could not obtain RIO for TSoS of type " << (*TSoSItr)->dumpType() );
							continue ;
						}
						//count this as a hit
						++nHits;

						//if we shell retrieve residuals, also get those
						if (m_doWriteResiduals){
							TrackRetrieverHelpers::getResidualPullFromHit( *TSoSItr, rot, m_residualPullCalculator, tsosResLoc1, tsosResLoc2, tsosPullLoc1, tsosPullLoc2);
						}
					}
				} // end hits details

				//Store number of retrieved hits for which we have retrieved information
				numHits.push_back(nHits);

			} // end loop over tracks in collection

			//Now fill everything in a datamap
			DataMap DataMap;
			// Start with mandatory entries
			DataMap["id"] = id;
			DataMap["chi2"] = chi2;
			DataMap["numDoF"] = numDoF;
			DataMap["trackAuthor"] = trackAuthor;
			DataMap["barcode"] = barcode;
			DataMap["numHits"] = numHits;
			DataMap["nBLayerHits"] = nBLayerHits;
			DataMap["nPixHits"] = nPixHits;
			DataMap["nSCTHits"] = nSCTHits;
			DataMap["nTRTHits"] = nTRTHits;
			DataMap["numPolyline"] = numPolyline;

			// if perigee parameters are not available, leave the corresponding subtags empty.
			// This way atlantis knows that such tracks can only be displayed as polylines.
			if (pt.size() > 0){
				DataMap["pt"] = pt;
				DataMap["d0"] = d0;
				DataMap["z0"] = z0;
				DataMap["phi0"] = phi0;
				DataMap["cotTheta"] = cotTheta;
				DataMap["covMatrix multiple=\"15\""] = covMatrix;
			}

			// vectors with measurement- or TrackStateOnSurface-wise entries
			if ( polylineX.size() > 0){
				std::string numPolyPerTrack = DataType(polylineX.size()/((double)id.size())).toString();
				DataMap["polylineX multiple=\"" + numPolyPerTrack + "\""] = polylineX;
				DataMap["polylineY multiple=\"" + numPolyPerTrack + "\""] = polylineY;
				DataMap["polylineZ multiple=\"" + numPolyPerTrack + "\""] = polylineZ;
			}

			if ( hits.size() > 0){
				std::string numHitsPerTrack = DataType(hits.size()/((double)id.size())).toString();
				DataMap["hits multiple=\"" + numHitsPerTrack + "\""] = hits;
				DataMap["isOutlier multiple=\""+numHitsPerTrack+"\""] = isOutlier;
				DataMap["driftSign multiple=\""+numHitsPerTrack+"\""] = driftSign;

				if (m_doWriteResiduals){
					// hits counter in principle not needed anymore:
					DataMap["numTsos"] = numHits;
					DataMap["tsosResLoc1 multiple=\""+numHitsPerTrack+"\""] = tsosResLoc1;
					DataMap["tsosResLoc2 multiple=\""+numHitsPerTrack+"\""] = tsosResLoc2;
					DataMap["tsosPullLoc1 multiple=\""+numHitsPerTrack+"\""] = tsosPullLoc1;
					DataMap["tsosPullLoc2 multiple=\""+numHitsPerTrack+"\""] = tsosPullLoc2;
					DataMap["tsosDetType multiple=\""+numHitsPerTrack+"\""] = tsosDetType;
				}
			}

			//forward data to formating tool
			if ( FormatTool->AddToEvent(dataTypeName(), collectionName, &DataMap).isFailure())
			  return StatusCode::RECOVERABLE;

			//Be verbose
			if (msgLvl(MSG::DEBUG)) {
				msg(MSG::DEBUG) << dataTypeName() << " collection " << collectionName;
				msg(MSG::DEBUG) << " retrieved with " << id.size() << " entries"<< endmsg;
			}

		} //loop over track collections

		//All collections retrieved okay
		return StatusCode::SUCCESS;

	} //retrieve
} //namespace JiveXML
