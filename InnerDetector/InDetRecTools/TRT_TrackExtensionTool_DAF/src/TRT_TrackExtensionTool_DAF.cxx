/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::TRT_TrackExtensionTool_DAF
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Sebastian Fleischmann
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <utility>
#include "TrkParameters/TrackParameters.h"

// tools:
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "InDetRecToolInterfaces/ITRT_DetElementsRoadMaker.h"
#include "InDetCompetingRIOsOnTrackTool/ICompetingTRT_DriftCirclesOnTrackCreator.h"

#include "TrkTrack/Track.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "TRT_TrackExtensionTool_DAF/TRT_TrackExtensionTool_DAF.h"

#include "TRT_ReadoutGeometry/TRT_BaseElement.h"
#include "InDetIdentifier/TRT_ID.h"

//http://www-zeuthen.desy.de/geant4/clhep-2.0.4.3/BasicVector3D_8h-source.html
double perp2( const Amg::Vector3D& v1, const Amg::Vector3D& v2 ) {
    double mag2 = v2.squaredNorm();
    double dp = v1.dot(v2);
    if (mag2 > 0.) {
        return v1.squaredNorm() - dp*dp / mag2;
    } else {
        return v1.squaredNorm();
    }
}

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrackExtensionTool_DAF::TRT_TrackExtensionTool_DAF
(const std::string& t,const std::string& n,const IInterface* p)
        : AthAlgTool(t,n,p),
	    m_trtcontainer(nullptr),
            m_jo_trtcontainername("TRT_DriftCircles"),
            m_jo_roadwidth(10.),
            m_jo_simpleExtension(true),
            m_jo_maxGroupDistance(5.),
            m_jo_minGroupDistance(1.),
            m_compROTcreator("InDet::CompetingTRT_DriftCirclesOnTrackTool/CompetingTRT_DriftCirclesOnTrackTool"),
            m_jo_annealingFactor(81.),
            m_roadtool("InDet::TRT_DetElementsRoadMaker_xk/TRT_DetElementsRoadMaker"),
            m_propagator("Trk::RungeKuttaPropagator/Propagator"),
            m_fieldmode("MapSolenoid"),
            m_trtID(nullptr)
{

    declareInterface<ITRT_TrackExtensionTool>(this);

    declareProperty("MagneticFieldMode",        m_fieldmode,          "field mode of the field tool");
    declareProperty("PropagatorTool",           m_propagator,           "Propagator tool");
    declareProperty("CompetingDriftCircleTool", m_compROTcreator,       "Tool for the creation of CompetingTRT_DriftCirclesOnTrack");
    declareProperty("RoadTool",                 m_roadtool,             "TRT Road Tool for the search of Detector Elements");
    declareProperty("TRT_DriftCircleContainer", m_jo_trtcontainername,   "Name of the container of TRT measurements (TRT_DriftCircles)");
    declareProperty("InitialAnnealingFactor",   m_jo_annealingFactor,    "Annealing factor (temperature) used to calculate the initial assignment probabilities of a group of competing TRT measurements. Should be choosen identical to the first entry of the annealing schedule of the Deterministic Annealing Filter");
    declareProperty("SimpleElementWiseExtension", m_jo_simpleExtension,  "Do simple element wise extension or do sophisticated grouping of measurements?");
    declareProperty("RoadWidth",                m_jo_roadwidth,          "Width of the road of measurements (the RoadTool uses its own road width!)");
    declareProperty("MaxGroupDistance",         m_jo_maxGroupDistance,   "Maximum distance of measurement groups in sophisticated grouping");
}

///////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrackExtensionTool_DAF::~TRT_TrackExtensionTool_DAF() {}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionTool_DAF::initialize() {
    StatusCode sc = AlgTool::initialize();

    // Build MagneticFieldProperties
    if     (m_fieldmode == "NoField"    ) m_fieldprop = Trk::MagneticFieldProperties(Trk::NoField  );
    else if(m_fieldmode == "MapSolenoid") m_fieldprop = Trk::MagneticFieldProperties(Trk::FastField);
    else                                  m_fieldprop = Trk::MagneticFieldProperties(Trk::FullField);
    // -------------------
    // Get propagator tool
    sc = m_propagator.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve the propagator: "<< m_propagator );
        return sc;
    }

    // --------------------------------
    // Get CompetingRIOsOnTrack creator
    sc = m_compROTcreator.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve CompetingRIOsOnTrack creator: "<< m_compROTcreator );
        return sc;
    }

    // ------------------------------------
    // Get detector elements road maker tool
    sc = m_roadtool.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve the TRT road tool: "<< m_roadtool );
        return sc;
    }

    // -------------------------
    // Get  TRT identifier:
    // First get TRT manager
    if (detStore()->retrieve(m_trtID, "TRT_ID").isFailure()) {
        msg(MSG::FATAL) << "Could not get TRT ID helper" << endmsg;
        return StatusCode::FAILURE;
    }
    // ----------------------------
    // init the size of the vectors

    ATH_CHECK( m_jo_trtcontainername.initialize());

    ATH_CHECK( m_fieldCondObjInputKey.initialize());

    return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionTool_DAF::finalize() {
    StatusCode sc = AlgTool::finalize();
    return sc;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackExtensionTool_DAF::dump( MsgStream& out ) const {
    return out;
}


///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_TrackExtensionTool_DAF::dump( std::ostream& out ) const {
    return out;
}


///////////////////////////////////////////////////////////////////
// Track extension init for a new event
///////////////////////////////////////////////////////////////////
std::unique_ptr<InDet::ITRT_TrackExtensionTool::IEventData>
InDet::TRT_TrackExtensionTool_DAF::newEvent(const EventContext& ctx) const {
    // -----------
    // get the container with TRT RIOs
   SG::ReadHandle<TRT_DriftCircleContainer> trtcontainer(m_jo_trtcontainername, ctx);

   if((not trtcontainer.isValid())) {
      std::stringstream msg;
      msg << "Missing TRT_DriftCircleContainer " << m_jo_trtcontainername.key();
      throw std::runtime_error( msg.str() );
   }

   std::unique_ptr<EventData> event_data(new EventData(trtcontainer.cptr()));
   event_data->m_measurement.reserve(200);
   event_data->m_propagatedTrackParameters.reserve(200);

   return std::unique_ptr<InDet::ITRT_TrackExtensionTool::IEventData>(event_data.release());
}

///////////////////////////////////////////////////////////////////
// Main methods for track extension to TRT
///////////////////////////////////////////////////////////////////
std::vector<const Trk::MeasurementBase*>&
InDet::TRT_TrackExtensionTool_DAF::extendTrack(const EventContext& ctx,
                                               const Trk::Track& track,
                                               InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const
{
  InDet::TRT_TrackExtensionTool_DAF::EventData &
     event_data=InDet::TRT_TrackExtensionTool_DAF::EventData::getPrivateEventData(virt_event_data);
    //StatusCode sc;

    const Trk::TrackParameters *trackPar = 0;
    // -------------
    // get the last TrackStateOnSurface of the Track: this should be the outermost Silicon measurement
    trackPar = track.trackStateOnSurfaces()->back()->trackParameters();
    if (!trackPar) {
        // if the last TSoS does not like us we use the perigee
        ATH_MSG_DEBUG("last track state has no trackParameters, use Perigee instead.");
        trackPar = track.perigeeParameters();
        if(!trackPar) {
            // now I am upset...
            ATH_MSG_ERROR("even the Perigee == Null, stop!");
            return event_data.m_measurement;
        }
    }
    // call main function
    return extendTrack(ctx, *trackPar,event_data);
}


std::vector<const Trk::MeasurementBase*>&
InDet::TRT_TrackExtensionTool_DAF::extendTrack(const EventContext& ctx,
                                               const Trk::TrackParameters& trackPar,
                                               InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const
{
   InDet::TRT_TrackExtensionTool_DAF::EventData &
      event_data=InDet::TRT_TrackExtensionTool_DAF::EventData::getPrivateEventData(virt_event_data);

    StatusCode sc(StatusCode::SUCCESS);
    // fake check of StatusCode to avoid "unchecked return codes" warning
    if (sc.isFailure()) {
        ATH_MSG_FATAL("StatusCode has gone insane!" );
    }
    // ------------
    // reset the output vector
    event_data.m_measurement.clear();
    // reset the detElements vector
    event_data.m_detectorElements.clear();
    // reset the vector with propagated trackParameters
    event_data.m_propagatedTrackParameters.clear();

    // ------------
    // stop immediately if no RIO container could be retrieved
    if(!event_data.m_trtcontainer) {
        ATH_MSG_DEBUG("no RIO container retrieved, stop!");
        return event_data.m_measurement;
    }

    event_data.m_siliconTrkParams = &trackPar;

    ATH_MSG_DEBUG("starting TRT detector elements road maker with initial TrackParemeters: "<< *event_data.m_siliconTrkParams );


    // Get AtlasFieldCache
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
    if (fieldCondObj == nullptr) {
        ATH_MSG_ERROR("InDet::TRT_TrackExtensionTool_xk::findSegment: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
    }
    MagField::AtlasFieldCache fieldCache;
    fieldCondObj->getInitializedCache (fieldCache);

    // ----------------------------------
    // start the TRT detector elements road maker to get a list of possibly interesting detector elements
    std::vector<const InDetDD::TRT_BaseElement*> detElements;
    m_roadtool->detElementsRoad(ctx, fieldCache, *event_data.m_siliconTrkParams, Trk::alongMomentum, detElements);

    ATH_MSG_DEBUG("TRT detector elements road maker found "<< detElements.size()<< " detElements" );

    // ----------------------------------
    // extrapolate to get a list of global track positions:
    // (extrapolate just parameters, not covariance)

    // the index of the detElement vector where the track changes from one sub type to another
    //   in the most general case the track crosses the TRT in the way: Endcap, Barrel, Encap (including cosmics)
    int beginBarrelRoad = 0;
    int beginSecondEndcapRoad = 0;
    //int nDetElements = 0;
    // was the barrel already crossed?
    bool barrelCrossed = false;
    // start extrapolation with TrkParameters from Silicon
    const Trk::TrackParameters* previousTrkPar = event_data.m_siliconTrkParams;
    // loop over detElements
    std::vector<const InDetDD::TRT_BaseElement*>::const_iterator detElIter=detElements.begin();
    if(detElIter == detElements.end()) {
        ATH_MSG_DEBUG("TRT_DetElementsRoadMaker found no road, stop!");
        return event_data.m_measurement;
    }
    for(; detElIter!=detElements.end(); ++detElIter) {
        // propagate without boundary checks to detElement
        const Trk::TrackParameters* nextTrkPar = m_propagator->propagateParameters(*previousTrkPar, (*detElIter)->surface(), Trk::alongMomentum, false, m_fieldprop, Trk::nonInteracting);
        if(!nextTrkPar) {
            // propagate directly to this detElement and hope that the Fitter will do a better job:
            ATH_MSG_DEBUG("step by step propagation of track parameters to TRT detector element failed: Try direct propagation (this may cause problems for the Fitter)");
            ATH_MSG_DEBUG("Problem was in " << (*detElIter)->type() <<" at (" << (*detElIter)->center().x() <<", "<< (*detElIter)->center().y() <<", "<< (*detElIter)->center().z() << ")" );
            nextTrkPar = m_propagator->propagateParameters(*event_data.m_siliconTrkParams, (*detElIter)->surface(), Trk::alongMomentum, false, m_fieldprop, Trk::nonInteracting);
            if (!nextTrkPar) {
                ATH_MSG_WARNING("direct propagation of track parameters to TRT detector element failed:");
                ATH_MSG_WARNING("   this detector element will be dropped and RIOs on the road may be lost!");
                continue;
            }
            //return m_measurement;
        } // end if !nextTrkPar
        ATH_MSG_VERBOSE("propagated TrackParameters: ("<< nextTrkPar->position().x() <<", "<< nextTrkPar->position().y() <<", "<< nextTrkPar->position().z() << ") in detectorType: " << (*detElIter)->type() );

        // append the detElement and the trkParameters to the vectors
        event_data.m_detectorElements.push_back((*detElIter));
        event_data.m_propagatedTrackParameters.push_back(nextTrkPar);

        // set the index if subdetector crossed:
        if ((*detElIter)->type() == InDetDD::TRT_BaseElement::BARREL) {
            if (!barrelCrossed) {
                barrelCrossed = true;
                beginBarrelRoad = event_data.m_detectorElements.size()-1;
            }
        } else {
            if ( (barrelCrossed) && (beginSecondEndcapRoad == 0) )  {
                //barrelCrossed = true;
                beginSecondEndcapRoad = event_data.m_detectorElements.size()-1;
            }
        }

        // prepare for next propagation
        previousTrkPar = nextTrkPar;
    } // end for loop over detElements

    ATH_MSG_DEBUG("Barrel Road starts at index "<< beginBarrelRoad << ",  second Encap Road at "<< beginSecondEndcapRoad << " with a total of "<< event_data.m_detectorElements.size()<< " detElements" );

    if (m_jo_simpleExtension) {
        ATH_MSG_DEBUG("run the simpleExtension" );
        // -----------------
        // build the complete extension element-wise:
        sc = elementWiseExtension(0, event_data.m_detectorElements.size()-1,event_data);
    } else {
        // -----------------
        // build the extensions for the three road parts and combine them:
        // endcap before barrel
        if (beginBarrelRoad > 0) {
            ATH_MSG_DEBUG("start processing of endcap road before barrel" );
            sc = elementWiseExtension(0, beginBarrelRoad-1,event_data);
        } else if (!barrelCrossed) {
            ATH_MSG_DEBUG("start processing of endcap road (endcap only track)" );
            sc = elementWiseExtension(0, event_data.m_detectorElements.size()-1,event_data);
        }
        // barrel
        if (barrelCrossed && (!(sc.isFailure()))) {
            ATH_MSG_DEBUG("start processing of barrel road" );
            if (beginSecondEndcapRoad > 0) {
                // endcap was reached after the barrel was crossed
                sc = groupedBarrelExtension(beginBarrelRoad, beginSecondEndcapRoad-1,event_data);
            } else {
                // after the barrel no endcap was hit
                sc = groupedBarrelExtension(beginBarrelRoad, event_data.m_detectorElements.size()-1,event_data);
            }
        }
        // encap after barrel
        if ((beginSecondEndcapRoad > 0) && !(sc.isFailure())) {
            ATH_MSG_DEBUG("start processing of endcap road after barrel" );
            sc = elementWiseExtension(beginSecondEndcapRoad, event_data.m_detectorElements.size()-1,event_data);
        }
    }// end if (simpleExtension)

    // did one of the extensions fail?
    if (sc.isFailure()) {
        ATH_MSG_WARNING("extensions failed: Return empty MeasurementBase vector" );
        // delete extension made so far
        std::vector<const Trk::MeasurementBase*>::const_iterator createdCompROTsIter = event_data.m_measurement.begin();
        for(; createdCompROTsIter!=event_data.m_measurement.end(); ++createdCompROTsIter) {
            delete (*createdCompROTsIter);
        }
        event_data.m_measurement.clear();
    }


    // ------------------------------------------
    // delete all the propagated track parameters:
    std::vector<const Trk::TrackParameters*>::const_iterator trkParIter = event_data.m_propagatedTrackParameters.begin();
    for(; trkParIter!=event_data.m_propagatedTrackParameters.end(); ++trkParIter) {
        delete (*trkParIter);
    }
    event_data.m_propagatedTrackParameters.clear();
    event_data.m_detectorElements.clear();

    ATH_MSG_DEBUG("** done: Made TRT extension with "<< event_data.m_measurement.size() << " CompetingTRT_DriftCirclesOnTrack");
    return event_data.m_measurement;
}

///////////////////////////////////////////////////////////////////
// find an element-wise extension (ie. the RIOs in a CompROT belong to one detElement)
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionTool_DAF::elementWiseExtension(int beginIndex,
                                                                   int endIndex,
                                                                   InDet::TRT_TrackExtensionTool_DAF::EventData &event_data) const
{


    const double squaredMaxBarrelRIOdistance = m_jo_roadwidth * m_jo_roadwidth;
    const double squaredMaxEndcapRIOdistance = m_jo_roadwidth * m_jo_roadwidth;


    double lastEndCapZ = -10000.;
    // create a new list of RIOs
    std::list< const Trk::PrepRawData * > RIOlist;

    ATH_MSG_DEBUG("looping over detElements" );
    // loop over detElements
    for(int index = beginIndex; index <= endIndex; ++index) {
        // ----------
        // global position of the track prediction
        // get the global position of the track prediction on the detElement
        Amg::Vector3D trkPos( event_data.m_propagatedTrackParameters[index]->position() );
        // ignore the z coordinate:
        trkPos[Amg::z]=0.;
        // determine the subdetector type
        bool isBarrel = (event_data.m_detectorElements[index]->type() ==  InDetDD::TRT_BaseElement::BARREL);
        bool createCompROT = true;
        if (!isBarrel) {
            if ( event_data.m_detectorElements[index]->surface().center().z() == lastEndCapZ ) {
                // this detector element belongs to the same wheel, so we add its measurements to
                // the previous group
                createCompROT = false;
            } else {
                lastEndCapZ = event_data.m_detectorElements[index]->surface().center().z();
            }
        } else {
            lastEndCapZ = -10000.;
        }
        if ( (RIOlist.size() > 0) && createCompROT ) {
            ATH_MSG_DEBUG("try to create CompetingTRT_DriftCirclesOnTrackTool with " << RIOlist.size() << " RIOs" );
            const Trk::MeasurementBase* compROT = m_compROTcreator->createCompetingROT(RIOlist, *(event_data.m_propagatedTrackParameters[index-1]), m_jo_annealingFactor);
            if (!compROT) {
                ATH_MSG_WARNING("current CompetingTRT_DriftCirclesOnTrack could not be created:");
                ATH_MSG_WARNING("   the RIOs on this detElement will be skipped!");
                //continue;
            } else {
                // ---------------
                // append the created compROT to the MeasurementBase vector:
                event_data.m_measurement.push_back(compROT);
            }
            // RIOs were used in the CompetingROT, clear the list
            RIOlist.clear();
        }
        // ------------
        // Driftcircle collection and contained RIOs
        ATH_MSG_VERBOSE("trying to get detElement for index "<< index );
        // get the id of the detElement
        IdentifierHash detElementID = event_data.m_detectorElements[index]->identifyHash();
        // get the driftCircleCollection belonging to this id
        auto container = event_data.m_trtcontainer->indexFindPtr(detElementID);

        if(container==nullptr) {
            ATH_MSG_DEBUG("for the current detectorElement no DriftCircleContainer seems to exist");
            continue;
        }

        ATH_MSG_DEBUG( "There are "  << container->size() << " entries in the TRT_DriftCircleCollection" );
        // loop over RIOs in the collection
        InDet::TRT_DriftCircleCollection::const_iterator driftCircleIterator = container->begin();
        for (; driftCircleIterator != container->end(); driftCircleIterator++) {
            // get the straw center of the RIO
            Amg::Vector3D strawGlobPos( event_data.m_detectorElements[index]->center( (*driftCircleIterator)->identify() ) );
            ATH_MSG_DEBUG("straw center at: ("<< strawGlobPos.x() <<", "<< strawGlobPos.y() << ")" );
            strawGlobPos[Amg::z]=0.;
            if (isBarrel) {
                // calc squared distance in the x-y-plane
                double distance = (trkPos - strawGlobPos).squaredNorm();
                ATH_MSG_DEBUG("distance in the x-y-plane: " << sqrt(distance) );
                // exclude RIOs too far from global track position on the detElement:
                if ( distance > squaredMaxBarrelRIOdistance ) {
                    ATH_MSG_DEBUG("distance too large, RIO will be dropped." );
                    continue;
                }
            } else {
                // get the straw number for the straw axis
                int straw = m_trtID->straw( (*driftCircleIterator)->identify() );
                // calc squared distance of straw and track prediction perpendicular to the straw axis
                double distance = perp2(strawGlobPos-trkPos, event_data.m_detectorElements[index]->strawAxis(straw));
                ATH_MSG_DEBUG("distance perp. to straw axis: " << sqrt(distance) );

                if ( distance > squaredMaxEndcapRIOdistance ) {
                    ATH_MSG_DEBUG("distance too large, RIO will be dropped." );
                    continue;
                }
            } // end else (isBarrel)
            // append the RIO to the RIO list
            RIOlist.push_back((*driftCircleIterator));
        }// end for (loop over DriftCircles)
    } // end for (loop over detElements)
    if ( RIOlist.size() > 0 ) {
        ATH_MSG_DEBUG("try to create CompetingTRT_DriftCirclesOnTrackTool with " << RIOlist.size() << " RIOs" );
        const Trk::MeasurementBase* compROT = m_compROTcreator->createCompetingROT(RIOlist, *(event_data.m_propagatedTrackParameters[endIndex]), m_jo_annealingFactor);
        if (!compROT) {
            ATH_MSG_WARNING("current CompetingTRT_DriftCirclesOnTrack could not be created:");
            ATH_MSG_WARNING("   the RIOs on this detElement will be skipped!");
        } else {
            // ---------------
            // append the created compROT to the MeasurementBase vector:
            event_data.m_measurement.push_back(compROT);
        }
    }
    return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// find a barrel extension with RIOs grouped along the globalPositions of the track
///////////////////////////////////////////////////////////////////
StatusCode
InDet::TRT_TrackExtensionTool_DAF::groupedBarrelExtension(int beginIndex,
                                                          int endIndex,
                                                          InDet::TRT_TrackExtensionTool_DAF::EventData &event_data) const
{


    // the pre-cut for RIOs (use twice the real road width):
    const double squaredMaxRIOdistanceFromTrackOnDetElement = m_jo_roadwidth*m_jo_roadwidth*16;

    // -------------------
    // loop over the road elements and extract the global positions
    //    of the track prediction in the x-y-plane:

    std::vector<Amg::Vector3D*> trackGlobalPos;
    trackGlobalPos.reserve(200);

    // index of the last global position belonging to the detElement
    std::vector<int> detElementGlobPosIndex(event_data.m_propagatedTrackParameters.size(), 0);

    // do the first iteration manually to get the
    //    last global position
    //    double lastPosX = m_propagatedTrackParameters[beginIndex]->position().x();
    //    double lastPosY = m_propagatedTrackParameters[beginIndex]->position().y();
    //    trackGlobalPosX.push_back( lastPosX );
    //    trackGlobalPosY.push_back( lastPosY );
    ATH_MSG_DEBUG("looping over detElements between index " << beginIndex << " and " << endIndex << "to produce a list of global positions" );

    Amg::Vector3D* lastPos = new Amg::Vector3D( event_data.m_propagatedTrackParameters[beginIndex]->position() );
    ATH_MSG_VERBOSE("global position: ("<< lastPos->x() <<", "<< lastPos->y() << ")" );
    // ignore z coordinate (along the straw)
    (*lastPos)[Amg::z]=0.;
    trackGlobalPos.push_back( lastPos );
    detElementGlobPosIndex[beginIndex] =  trackGlobalPos.size()-1;

    for(int index = beginIndex+1; index <= endIndex; ++index) {
        // get the global track position and fill it into vectors
        //double posX = m_propagatedTrackParameters[index]->position().x();
        //double posY = m_propagatedTrackParameters[index]->position().y();
        Amg::Vector3D* Pos = new Amg::Vector3D( event_data.m_propagatedTrackParameters[index]->position() );
        // ignore z coordinate (along the straw)
        (*Pos)[Amg::z]=0.;

        double distance = ((*Pos) - (*lastPos)).norm();
        ATH_MSG_VERBOSE("global position: ("<< Pos->x() <<", "<< Pos->y() << ") -> distance to previous: "<< distance );
        if ( distance > m_jo_maxGroupDistance ) {
            // distance between the global positions is too large: include more points
            int numberOfPoints = int(distance / m_jo_maxGroupDistance);
            Amg::Vector3D diffVector = (*Pos) - (*lastPos);
            for (int i=1; i<=numberOfPoints; i++) {
                Amg::Vector3D* newPos = new Amg::Vector3D( (*lastPos) + ((i / double(numberOfPoints+1)) * diffVector) );
                trackGlobalPos.push_back( newPos );
                ATH_MSG_VERBOSE("insert point with global position: ("<< newPos->x() <<", "<< newPos->y() << ")" );
            }
        }
        if (distance < m_jo_minGroupDistance){
            // ignore this global position
            detElementGlobPosIndex[index] = trackGlobalPos.size()-1;
            ATH_MSG_VERBOSE("ignore this global position");
            continue;
        }
        // append the Global Position and the related Index to the vector
        trackGlobalPos.push_back( Pos );
        detElementGlobPosIndex[index] = trackGlobalPos.size()-1;
        lastPos = Pos;
    }

    // the vector which stores the grouped RIOs
    std::vector< std::list< const Trk::PrepRawData* >* > groupedRIOs( trackGlobalPos.size(), 0 );
    // the vector for grouped RIOs with the minimal distances to the secant
    std::vector< double > groupedRIOsMinDistance( trackGlobalPos.size() );
    // the vector for grouped RIOs with the pointer to the RIO with the minimal distance
    std::vector< const InDet::TRT_DriftCircle* > minDistanceRIO( trackGlobalPos.size(), 0 );

    ATH_MSG_DEBUG("looping over detElements to get the DriftCircles" );
    int createdGroupCounter=0;

    // loop over detElements
    for(int index = beginIndex+1; index <= endIndex; ++index) {
        ATH_MSG_VERBOSE("trying to get detElement for index "<< index );
        // get the id of the detElement
        IdentifierHash detElementID = event_data.m_detectorElements[index]->identifyHash();
        // get the driftCircleCollection belonging to this id
        auto container = event_data.m_trtcontainer->indexFindPtr(detElementID);
        // loop over RIOs in the collection
        if(container==nullptr) {
            ATH_MSG_DEBUG("for the current detectorElement no DriftCircleContainer seems to exist");
            continue;
        }
        ATH_MSG_DEBUG( "There are "  << container->size() << " entries in the TRT_DriftCircleCollection" );

        InDet::TRT_DriftCircleCollection::const_iterator driftCircleIterator = container->begin();
        for (; driftCircleIterator != container->end(); driftCircleIterator++) {
            // get the straw center of the RIO
            Amg::Vector3D strawGlobPos( event_data.m_detectorElements[index]->center( (*driftCircleIterator)->identify() ) );
            strawGlobPos[Amg::z]=0.;

            // --------------------------------
            // search for the closest track global position to the straw center:
            // as a first try use the distance to the global pos of the track on the detElement
            // index of the global position with the minimal distance:
            unsigned int minDistIndex = detElementGlobPosIndex[index];
            //double minDistance = strawGlobPos.distance2(*(trackGlobalPos[minDistIndex]));
            double minDistance = (*(trackGlobalPos[minDistIndex]) - strawGlobPos).squaredNorm();
            ATH_MSG_VERBOSE("global position of the RIO belonging to globPos index "<< minDistIndex <<" : ("<< strawGlobPos.x() <<", "<< strawGlobPos.y() << ") --> distance: " << sqrt(minDistance) );

            // Perhaps already exclude RIOs too far from global track position on the detElement:
            if ( minDistance > squaredMaxRIOdistanceFromTrackOnDetElement ) {
                continue;
            }
            // search backward:
            double newDistance = -1.;
            for (int i = minDistIndex-1; i >= 0; i--) {
                //newDistance = strawGlobPos.distance2(*(trackGlobalPos[i]));
                newDistance = (*(trackGlobalPos[i]) - strawGlobPos).squaredNorm();
                if (newDistance < minDistance ) {
                    // new distance is smaller than the one we had before!
                    minDistIndex = i;
                    minDistance = newDistance;
                } else {
                    // new distance is larger: stop backward search
                    break;
                    //i = 0;
                }
            } // end for (backward search)
            if (newDistance == -1.) {
                // backward serach did not succeed in finding a closer global position, try forward search
                for (unsigned int i = minDistIndex+1; i < trackGlobalPos.size(); i++) {
                    newDistance = (*(trackGlobalPos[i]) - strawGlobPos).squaredNorm();
                    if (newDistance < minDistance ) {
                        // new distance is smaller than the one we had before!
                        minDistIndex = i;
                        minDistance = newDistance;
                    } else {
                        // new distance is larger: stop forward search
                        break;
                        //i = trackGlobalPos.size();
                    }
                } // end for (forward search)
            }// end if (nackward searched failed)
            ATH_MSG_VERBOSE("nearest point with index "<< minDistIndex <<" : ("<< trackGlobalPos[minDistIndex]->x() <<", "<< trackGlobalPos[minDistIndex]->y() << ") --> distance:"<< sqrt(minDistance) );

            // --------------
            // now that we found the global position with minimum distance, decide whether
            //   the RIO has to be sorted before or after this position.
            int groupIndex = 0;
            if (minDistIndex <= 0) {
                // we can not go in front of this position
                groupIndex = 0;
            } else if (minDistIndex >= (trackGlobalPos.size()-1)) {
                // we can not go beyond this position
                groupIndex = trackGlobalPos.size()-2;
            } else {
                // distance to the point before the minimum
                double distBeforeMin = (*(trackGlobalPos[minDistIndex-1]) - strawGlobPos).squaredNorm();
                // distance to the point after the minimum
                double distAfterMin = (*(trackGlobalPos[minDistIndex+1]) - strawGlobPos).squaredNorm();
                if (distBeforeMin < distAfterMin) {
                    groupIndex = minDistIndex-1;
                } else {
                    groupIndex = minDistIndex;
                }
            }
            // ------------
            // now decide whether or not the RIO lies on the road
            // calc the distance of the straw center to the secant
            //double distToSecant = ((*(trackGlobalPos[groupIndex]))- (*(trackGlobalPos[groupIndex+1]))).perp( (strawGlobPos - (*(trackGlobalPos[groupIndex]))) );
            //double distToSecant = (strawGlobPos - (*(trackGlobalPos[groupIndex]))).perp( ((*(trackGlobalPos[groupIndex]))- (*(trackGlobalPos[groupIndex+1]))) );
            double distToSecant = sqrt(perp2(strawGlobPos - (*(trackGlobalPos[groupIndex])),  ((*(trackGlobalPos[groupIndex]))- (*(trackGlobalPos[groupIndex+1]))) ) );
            ATH_MSG_VERBOSE(" belongs to group "<< groupIndex <<", distance to secant: "<< distToSecant );
            if (distToSecant > m_jo_roadwidth ) {
                // ignore the current RIO
                ATH_MSG_DEBUG("RIO too far from road: ignore" );
                continue;
            }
            // check whether the current group already contains some entries
            if (groupedRIOs[groupIndex]) {
                ATH_MSG_VERBOSE(" append RIO to group list " << groupIndex );
                // append the RIO to the RIO list of the current group
                groupedRIOs[groupIndex]->push_back((*driftCircleIterator));
                // check if the current RIO is closer to the secant
                if (distToSecant < groupedRIOsMinDistance[groupIndex] ) {
                    // put distance to secant in vector
                    groupedRIOsMinDistance[groupIndex] = distToSecant;
                    // put the RIO in the vector with the minimum distance RIOs
                    minDistanceRIO[groupIndex] = (*driftCircleIterator);
                    ATH_MSG_VERBOSE(" RIO has min. distance in the group" );
                }
            } else {
                // create a new list for this group
                ATH_MSG_VERBOSE(" create a new RIO list for group " << groupIndex );
                createdGroupCounter++;
                std::list< const Trk::PrepRawData* >* RIOlist = new std::list< const Trk::PrepRawData* >;
                RIOlist->push_back((*driftCircleIterator));
                // put the list into the vector for RIO lists
                groupedRIOs[groupIndex] = RIOlist;
                // put distance to secant in vector
                groupedRIOsMinDistance[groupIndex] = distToSecant;
                // put the RIO in the vector with the minimum distance RIOs
                minDistanceRIO[groupIndex] = (*driftCircleIterator);
            } // end else (no list of RIOs for current group)
        } // end for (loop over driftCircleContainer entries)
    } // end for (loop over detElements)

    // --------
    // delete the global positions
    std::vector< Amg::Vector3D* >::iterator globPosIterator = trackGlobalPos.begin();
    for (; globPosIterator != trackGlobalPos.end(); globPosIterator++ ) {
        delete (*globPosIterator);
        (*globPosIterator) = 0;
    }
    ATH_MSG_DEBUG("loop over groups to create CompetingRIOsOnTrack" );

    int usedGroupCounter=0;
    // --------------------------
    // create CompetingTRT_DriftCirclesOnTrack out of the groups
    for(unsigned int groupIndex = 0; groupIndex < groupedRIOs.size(); ++groupIndex) {
        // does a group exist for the current index?
        if (!groupedRIOs[groupIndex]) {
            // jump to the next group
            continue;
        }
        // produce TrackParameters for the group
        // get StraightLineSurface of the RIO closest to the prediction
        const Trk::Surface& RIOsurface = minDistanceRIO[groupIndex]->detectorElement()->surface(minDistanceRIO[groupIndex]->identify());
        // propagate to this surface
        const Trk::TrackParameters* TrkPar = m_propagator->propagateParameters(*event_data.m_siliconTrkParams, RIOsurface, Trk::alongMomentum, false, m_fieldprop, Trk::nonInteracting);
        if (!TrkPar) {
            ATH_MSG_WARNING("propagation of track parameters to the RIO surface failed:");
            ATH_MSG_WARNING("   this group of RIOs will skipped!");
            continue;
        }
        ATH_MSG_DEBUG("try to create CompetingTRT_DriftCirclesOnTrackTool with " << groupedRIOs[groupIndex]->size() << " RIOs" );

        const Trk::MeasurementBase* compROT = m_compROTcreator->createCompetingROT(*groupedRIOs[groupIndex], *TrkPar, m_jo_annealingFactor);
        if (!compROT) {
            ATH_MSG_WARNING("current CompetingTRT_DriftCirclesOnTrack could not be created:");
            ATH_MSG_WARNING("   this group of RIOs will skipped!");
            // delete the trkParameters
            delete TrkPar;
            continue;
        }
        // ---------------
        // append the created compROT to the MeasurementBase vector:
        // first we have to cast to the base class?
        event_data.m_measurement.push_back(compROT);
        // delete the propagated trkParams
        delete TrkPar;
        usedGroupCounter++;
    } // end for (loop over RIO groups)
    ATH_MSG_DEBUG("Used  " << usedGroupCounter << " from at total " << groupedRIOs.size() << " groups, should be used "<< createdGroupCounter);

    // delete the RIOlists
    std::vector< std::list< const Trk::PrepRawData* >* >::iterator RIOlistIterator = groupedRIOs.begin();
    for (; RIOlistIterator != groupedRIOs.end(); RIOlistIterator++ ) {
        // just delete the list, the entries (RIOs) belong to the driftcircle collection
        delete (*RIOlistIterator);
        (*RIOlistIterator) = 0;
    }

    return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Main methods for segment finding in TRT
///////////////////////////////////////////////////////////////////
Trk::TrackSegment*
InDet::TRT_TrackExtensionTool_DAF::findSegment(const EventContext& /*ctx*/,
                                               const Trk::TrackParameters&,
                                               InDet::ITRT_TrackExtensionTool::IEventData &) const
{
    return 0;
}
///////////////////////////////////////////////////////////////////
// Methods for track extension to TRT for pixles+sct tracks
// and new track production
///////////////////////////////////////////////////////////////////

Trk::Track*
InDet::TRT_TrackExtensionTool_DAF::newTrack(const EventContext& /*ctx*/,
                                            const Trk::Track&,
                                            InDet::ITRT_TrackExtensionTool::IEventData &) const
{
  return 0;
}
