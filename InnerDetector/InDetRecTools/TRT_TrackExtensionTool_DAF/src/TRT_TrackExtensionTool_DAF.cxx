/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
//#include "GaudiKernel/ToolFactory.h"
//#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/ListItem.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrkParameters/TrackParameters.h"

#include "MagFieldInterfaces/IMagFieldSvc.h" 
// tools:
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "InDetRecToolInterfaces/ITRT_DetElementsRoadMaker.h"
#include "InDetCompetingRIOsOnTrackTool/ICompetingTRT_DriftCirclesOnTrackCreator.h"

#include "TrkTrack/Track.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "TRT_TrackExtensionTool_DAF/TRT_TrackExtensionTool_DAF.h"

#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "InDetIdentifier/TRT_ID.h"
//#include "InDetReadoutGeometry/TRT_DetectorManager.h"

//#include "TrkPrepRawData/PrepRawData.h"

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
            m_siliconTrkParams(nullptr),
            m_compROTcreator("InDet::CompetingTRT_DriftCirclesOnTrackTool/CompetingTRT_DriftCirclesOnTrackTool"),
            m_jo_annealingFactor(81.),
            m_roadtool("InDet::TRT_DetElementsRoadMaker_xk/TRT_DetElementsRoadMaker"),
            m_propagator("Trk::RungeKuttaPropagator/Propagator"),
            m_fieldServiceHandle("AtlasFieldSvc",n),
            m_fieldmode("MapSolenoid"),
            m_trtID(nullptr)
{
    m_measurement.reserve(200);

    declareInterface<ITRT_TrackExtensionTool>(this);

    declareProperty("MagFieldSvc",              m_fieldServiceHandle,   "magnetic filed service"); 
    declareProperty("MagneticFieldMode",        m_fieldmode,          "field mode of the field tool");
    declareProperty("PropagatorTool",           m_propagator,           "Propagator tool");
    declareProperty("CompetingDriftCircleTool", m_compROTcreator,       "Tool for the creation of CompetingTRT_DriftCirclesOnTrack");
    declareProperty("RoadTool",                 m_roadtool,             "TRT Road Tool for the search of Detector Elements");
    declareProperty("TRT_DriftCircleContainer", m_jo_trtcontainername,   "Name of the container of TRT measurements (TRT_DriftCircles)");
    declareProperty("InitialAnnealingFactor",   m_jo_annealingFactor,    "Annealing factor (temperature) used to calculate the initial assignment probabilities of a group of competing TRT measurements. Should be choosen identical to the first entry of the annealing schedule of the Deterministic Annealing Filter");
    declareProperty("SimpleElementWiseExtension", m_jo_simpleExtension,  "Do simple element wise extension or do sophisticated grouping of measurements?");
    declareProperty("RoadWidth",                m_jo_roadwidth,          "Width of the road of measurements (the RoadTool uses its own road width!)");
    declareProperty("MaxGroupDistance",         m_jo_maxGroupDistance,   "Maximum distance of measurement groups in sophisticated grouping");
//    declareProperty("MinGroupDistance", m_jo_minGroupDistance);
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

    // -------------
    // set the magnetic field properties:
    // Get magnetic field service
    if( !m_fieldServiceHandle.retrieve() ){ 
        ATH_MSG_FATAL("Failed to retrieve " << m_fieldServiceHandle );
        return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Retrieved " << m_fieldServiceHandle ); 

    // Build MagneticFieldProperties
    Trk::MagneticFieldProperties* pMF = 0; 
    if     (m_fieldmode == "NoField"    ) pMF = new Trk::MagneticFieldProperties(Trk::NoField  );
    else if(m_fieldmode == "MapSolenoid") pMF = new Trk::MagneticFieldProperties(Trk::FastField);
    else                                  pMF = new Trk::MagneticFieldProperties(Trk::FullField);
    m_fieldprop = *pMF;
    delete pMF;
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
    m_detectorElements.reserve(200);
    m_propagatedTrackParameters.reserve(200);

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
    //     out<<std::endl;
    //     if(m_nprint)
    //         return dumpEvent(out);
    //     return dumpConditions(out);
    return out;
}


///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

//MsgStream& InDet::TRT_TrackExtensionTool_DAF::dumpConditions( MsgStream& out ) const {
//     int n = 62-m_propagator.size();
//     std::string s1;
//     for(int i=0; i<n; ++i)
//         s1.append(" ");
//     s1.append("|");
//     n     = 62-m_field.size();
//     std::string s2;
//     for(int i=0; i<n; ++i)
//         s2.append(" ");
//     s2.append("|");
//
//     std::string fieldmode[9] ={"NoField"       ,"ConstantField","SolenoidalField",
//                                "ToroidalField" ,"Grid3DField"  ,"RealisticField" ,
//                                "UndefinedField","AthenaField"  , "?????"         };
//
//     int mode = m_fieldprop.magneticFieldMode();
//     if(mode<0 || mode>8 )
//         mode = 8;
//
//     n     = 62-fieldmode[mode].size();
//     std::string s3;
//     for(int i=0; i<n; ++i)
//         s3.append(" ");
//     s3.append("|");
//     n     = 62-m_riontrack.size();
//     std::string s5;
//     for(int i=0; i<n; ++i)
//         s5.append(" ");
//     s5.append("|");
//     n     = 62-m_roadmaker.size();
//     std::string s6;
//     for(int i=0; i<n; ++i)
//         s6.append(" ");
//     s6.append("|");
//
//     out<<"|----------------------------------------------------------------------"
//     <<"-------------------|"
//     <<std::endl;
//     out<<"| Tool for propagation    | "<<m_propagator   <<s1<<std::endl;
//     out<<"| Tool for rio  on track  | "<<m_riontrack    <<s5<<std::endl;
//     out<<"| Tool for road builder   | "<<m_roadmaker    <<s6<<std::endl;
//     out<<"| Tool for magnetic field | "<<m_field        <<s2<<std::endl;
//     out<<"| Magnetic field mode     | "<<fieldmode[mode]<<s3<<std::endl;
//     out<<"| TRT road half width (mm)| "
//     <<std::setw(12)<<std::setprecision(5)<<m_roadwidth
//     <<"                                                  |"<<std::endl;
//     out<<"| Min number DriftCircles | "
//     <<std::setw(12)<<m_minNumberDCs
//     <<"                                                  |"<<std::endl;
//     out<<"| Use drift time  ?       | "
//     <<std::setw(12)<<m_usedriftrad
//     <<"                                                  |"<<std::endl;
//     out<<"|----------------------------------------------------------------------"
//     <<"-------------------|"
//     <<std::endl;
//     return out;
// }

///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////

// MsgStream& InDet::TRT_TrackExtensionTool_DAF::dumpEvent( MsgStream& out ) const {
//     return out;
// }

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_TrackExtensionTool_DAF::dump( std::ostream& out ) const {
    return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

// MsgStream& InDet::operator    <<
// (MsgStream& sl,const InDet::TRT_TrackExtensionTool_DAF& se) {
//     return se.dump(sl);
// }

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

// std::ostream& InDet::operator <<
// (std::ostream& sl,const InDet::TRT_TrackExtensionTool_DAF& se) {
//     return se.dump(sl);
// }

///////////////////////////////////////////////////////////////////
// Track extension init for a new event
///////////////////////////////////////////////////////////////////
void InDet::TRT_TrackExtensionTool_DAF::newEvent() {
    // -----------
    // get the container with TRT RIOs
    m_trtcontainer = 0;
    StatusCode sc = evtStore()->retrieve(m_trtcontainer, m_jo_trtcontainername);
    if(sc.isFailure()) {
        ATH_MSG_DEBUG("Could not get TRT_DriftCircleContainer");
    }
}

///////////////////////////////////////////////////////////////////
// Main methods for track extension to TRT
///////////////////////////////////////////////////////////////////
std::vector<const Trk::MeasurementBase*>& InDet::TRT_TrackExtensionTool_DAF::extendTrack(
        const Trk::Track& track) {
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
            return m_measurement;
        }
    }
    // call main function
    return extendTrack(*trackPar);
}


std::vector<const Trk::MeasurementBase*>& InDet::TRT_TrackExtensionTool_DAF::extendTrack(
        const Trk::TrackParameters& trackPar) {
    StatusCode sc(StatusCode::SUCCESS);
    // fake check of StatusCode to avoid "unchecked return codes" warning
    if (sc.isFailure()) {
        ATH_MSG_FATAL("StatusCode has gone insane!" );
    }
    // ------------
    // reset the output vector
    //m_measurement.erase(m_measurement.begin(), m_measurement.end());
    m_measurement.clear();
    // reset the detElements vector
    m_detectorElements.clear();
    // reset the vector with propagated trackParameters
    m_propagatedTrackParameters.clear();

    // ------------
    // stop immediately if no RIO container could be retrieved
    if(!m_trtcontainer) {
        ATH_MSG_DEBUG("no RIO container retrieved, stop!");
        return m_measurement;
    }

    m_siliconTrkParams = &trackPar;

    ATH_MSG_DEBUG("starting TRT detector elements road maker with initial TrackParemeters: "<< *m_siliconTrkParams );
    // ----------------------------------
    // start the TRT detector elements road maker to get a list of possibly interesting detector elements
    std::list<const InDetDD::TRT_BaseElement*> detElements;
    m_roadtool->detElementsRoad(*m_siliconTrkParams, Trk::alongMomentum, detElements);

    ATH_MSG_DEBUG("TRT detector elements road maker found "<< detElements.size()<< " detElements" );

    // ----------------------------------
    // extrapolate to get a list of global track positions:
    // (extrapolate just parameters, not covariance)

    //    m_proptool->globalPositions(G,Tp,m_fieldprop,m_bounds,S,Trk::pion);
    // the index of the detElement vector where the track changes from one sub type to another
    //   in the most general case the track crosses the TRT in the way: Endcap, Barrel, Encap (including cosmics)
    int beginBarrelRoad = 0;
    int beginSecondEndcapRoad = 0;
    //int nDetElements = 0;
    // was the barrel already crossed?
    bool barrelCrossed = false;
    // start extrapolation with TrkParameters from Silicon
    const Trk::TrackParameters* previousTrkPar = m_siliconTrkParams;
    // loop over detElements
    std::list<const InDetDD::TRT_BaseElement*>::const_iterator detElIter=detElements.begin();
    if(detElIter == detElements.end()) {
        ATH_MSG_DEBUG("TRT_DetElementsRoadMaker found no road, stop!");
        return m_measurement;
    }
    for(; detElIter!=detElements.end(); ++detElIter) {
        // propagate without boundary checks to detElement
        //const Trk::TrackParameters* nextTrkPar = m_propagator->propagateParameters(*previousTrkPar, (*detElIter)->surface(), Trk::alongMomentum, false, m_fieldprop, Trk::pion);
        const Trk::TrackParameters* nextTrkPar = m_propagator->propagateParameters(*previousTrkPar, (*detElIter)->surface(), Trk::alongMomentum, false, m_fieldprop, Trk::nonInteracting);
        if(!nextTrkPar) {
            // propagate directly to this detElement and hope that the Fitter will do a better job:
            ATH_MSG_DEBUG("step by step propagation of track parameters to TRT detector element failed: Try direct propagation (this may cause problems for the Fitter)");
            ATH_MSG_DEBUG("Problem was in " << (*detElIter)->type() <<" at (" << (*detElIter)->center().x() <<", "<< (*detElIter)->center().y() <<", "<< (*detElIter)->center().z() << ")" );
            nextTrkPar = m_propagator->propagateParameters(*m_siliconTrkParams, (*detElIter)->surface(), Trk::alongMomentum, false, m_fieldprop, Trk::nonInteracting);
            if (!nextTrkPar) {
                ATH_MSG_WARNING("direct propagation of track parameters to TRT detector element failed:");
                ATH_MSG_WARNING("   this detector element will be dropped and RIOs on the road may be lost!");
                continue;
            }
            //return m_measurement;
        } // end if !nextTrkPar
        ATH_MSG_VERBOSE("propagated TrackParameters: ("<< nextTrkPar->position().x() <<", "<< nextTrkPar->position().y() <<", "<< nextTrkPar->position().z() << ") in detectorType: " << (*detElIter)->type() );

        // append the detElement and the trkParameters to the vectors
        m_detectorElements.push_back((*detElIter));
        m_propagatedTrackParameters.push_back(nextTrkPar);

        // set the index if subdetector crossed:
        if ((*detElIter)->type() == InDetDD::TRT_BaseElement::BARREL) {
            if (!barrelCrossed) {
                barrelCrossed = true;
                beginBarrelRoad = m_detectorElements.size()-1;
            }
        } else {
            if ( (barrelCrossed) && (beginSecondEndcapRoad == 0) )  {
                //barrelCrossed = true;
                beginSecondEndcapRoad = m_detectorElements.size()-1;
            }
        }

        // prepare for next propagation
        previousTrkPar = nextTrkPar;
        //nDetElements++;
    } // end for loop over detElements

    ATH_MSG_DEBUG("Barrel Road starts at index "<< beginBarrelRoad << ",  second Encap Road at "<< beginSecondEndcapRoad << " with a total of "<< m_detectorElements.size()<< " detElements" );

    if (m_jo_simpleExtension) {
        ATH_MSG_DEBUG("run the simpleExtension" );
        // -----------------
        // build the complete extension element-wise:
        sc = elementWiseExtension(0, m_detectorElements.size()-1);
    } else {
        // -----------------
        // build the extensions for the three road parts and combine them:
        // endcap before barrel
        if (beginBarrelRoad > 0) {
            ATH_MSG_DEBUG("start processing of endcap road before barrel" );
            sc = elementWiseExtension(0, beginBarrelRoad-1);
        } else if (!barrelCrossed) {
            ATH_MSG_DEBUG("start processing of endcap road (endcap only track)" );
            sc = elementWiseExtension(0, m_detectorElements.size()-1);
        }
        // barrel
        if (barrelCrossed && (!(sc.isFailure()))) {
            ATH_MSG_DEBUG("start processing of barrel road" );
            if (beginSecondEndcapRoad > 0) {
                // endcap was reached after the barrel was crossed
                sc = groupedBarrelExtension(beginBarrelRoad, beginSecondEndcapRoad-1);
            } else {
                // after the barrel no endcap was hit
                sc = groupedBarrelExtension(beginBarrelRoad, m_detectorElements.size()-1);
            }
        }
        // encap after barrel
        if ((beginSecondEndcapRoad > 0) && !(sc.isFailure())) {
            ATH_MSG_DEBUG("start processing of endcap road after barrel" );
            sc = elementWiseExtension(beginSecondEndcapRoad, m_detectorElements.size()-1);
        }
    }// end if (simpleExtension)

    // did one of the extensions fail?
    if (sc.isFailure()) {
        ATH_MSG_WARNING("extensions failed: Return empty MeasurementBase vector" );
        // delete extension made so far
        std::vector<const Trk::MeasurementBase*>::const_iterator createdCompROTsIter = m_measurement.begin();
        for(; createdCompROTsIter!=m_measurement.end(); ++createdCompROTsIter) {
            delete (*createdCompROTsIter);
        }
        m_measurement.clear();
    }


    // ------------------------------------------
    // delete all the propagated track parameters:
    std::vector<const Trk::TrackParameters*>::const_iterator trkParIter = m_propagatedTrackParameters.begin();
    for(; trkParIter!=m_propagatedTrackParameters.end(); ++trkParIter) {
        delete (*trkParIter);
    }
    m_propagatedTrackParameters.clear();
    m_detectorElements.clear();

    ATH_MSG_DEBUG("** done: Made TRT extension with "<< m_measurement.size() << " CompetingTRT_DriftCirclesOnTrack");
    return m_measurement;
}

///////////////////////////////////////////////////////////////////
// find an element-wise extension (ie. the RIOs in a CompROT belong to one detElement)
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionTool_DAF::elementWiseExtension(int beginIndex, int endIndex) const {


    const double squaredMaxBarrelRIOdistance = m_jo_roadwidth * m_jo_roadwidth;
    const double squaredMaxEndcapRIOdistance = m_jo_roadwidth * m_jo_roadwidth;

    // -----------------------
    // get all the RIOs on the detElements
    InDet::TRT_DriftCircleContainer::const_iterator containerIterator;

    double lastEndCapZ = -10000.;
    // create a new list of RIOs
    std::list< const Trk::PrepRawData * > RIOlist;

    ATH_MSG_DEBUG("looping over detElements" );
    // loop over detElements
    for(int index = beginIndex; index <= endIndex; ++index) {
        // ----------
        // global position of the track prediction
        // get the global position of the track prediction on the detElement
        Amg::Vector3D trkPos( m_propagatedTrackParameters[index]->position() );
        //ATH_MSG_INFO("track prediction at: ("<< trkPos.x() <<", "<< trkPos.y() <<", "<< trkPos.z() << ")" );
        //ATH_MSG_INFO("detElement surface: " );
        //m_detectorElements[index]->surface().dump(log);
        // ignore the z coordinate:
        trkPos[Amg::z]=0.;
        // determine the subdetector type
        bool isBarrel = (m_detectorElements[index]->type() ==  InDetDD::TRT_BaseElement::BARREL);
        bool createCompROT = true;
        if (!isBarrel) {
            if ( m_detectorElements[index]->surface().center().z() == lastEndCapZ ) {
                // this detector element belongs to the same wheel, so we add its measurements to
                // the previous group
                createCompROT = false;
            } else {
                lastEndCapZ = m_detectorElements[index]->surface().center().z();
            }
        } else {
            lastEndCapZ = -10000.;
        }
        if ( (RIOlist.size() > 0) && createCompROT ) {
            ATH_MSG_DEBUG("try to create CompetingTRT_DriftCirclesOnTrackTool with " << RIOlist.size() << " RIOs" );
            const Trk::MeasurementBase* compROT = m_compROTcreator->createCompetingROT(RIOlist, *(m_propagatedTrackParameters[index-1]), m_jo_annealingFactor);
            if (!compROT) {
                ATH_MSG_WARNING("current CompetingTRT_DriftCirclesOnTrack could not be created:");
                ATH_MSG_WARNING("   the RIOs on this detElement will be skipped!");
                //continue;
            } else {
                // ---------------
                // append the created compROT to the MeasurementBase vector:
                m_measurement.push_back(compROT);
            }
            // RIOs were used in the CompetingROT, clear the list
            RIOlist.clear();
        }
        //ATH_MSG_VERBOSE("detector type: " << m_detectorElements[index]->type() );
        // ------------
        // Driftcircle collection and contained RIOs
        ATH_MSG_VERBOSE("trying to get detElement for index "<< index );
        // get the id of the detElement
        IdentifierHash detElementID = m_detectorElements[index]->identifyHash();
        // get the driftCircleCollection belonging to this id
        containerIterator = m_trtcontainer->indexFind(detElementID);

        if(containerIterator==m_trtcontainer->end()) {
            ATH_MSG_DEBUG("for the current detectorElement no DriftCircleContainer seems to exist");
            continue;
        }
//        if ((*containerIterator)->begin()==(*containerIterator)->end()){
//           ATH_MSG_DEBUG("containerIterator->begin == containerIterator->end");
//        }

        ATH_MSG_DEBUG( "There are "  << (*containerIterator)->size() << " entries in the TRT_DriftCircleCollection" );
        // loop over RIOs in the collection
        InDet::TRT_DriftCircleCollection::const_iterator driftCircleIterator = (*containerIterator)->begin();
        for (; driftCircleIterator != (*containerIterator)->end(); driftCircleIterator++) {
            // get the straw center of the RIO
            Amg::Vector3D strawGlobPos( m_detectorElements[index]->center( (*driftCircleIterator)->identify() ) );
            ATH_MSG_DEBUG("straw center at: ("<< strawGlobPos.x() <<", "<< strawGlobPos.y() << ")" );
            strawGlobPos[Amg::z]=0.;
            if (isBarrel) {
                // calc squared distance in the x-y-plane
                double distance = (trkPos - strawGlobPos).squaredNorm();
                //double distance = strawGlobPos.distance2(trkPos);
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
                //double distance = (strawGlobPos - trkPos).perp2( m_detectorElements[index]->strawAxis(straw) );
                double distance = perp2(strawGlobPos-trkPos, m_detectorElements[index]->strawAxis(straw));
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
        const Trk::MeasurementBase* compROT = m_compROTcreator->createCompetingROT(RIOlist, *(m_propagatedTrackParameters[endIndex]), m_jo_annealingFactor);
        if (!compROT) {
            ATH_MSG_WARNING("current CompetingTRT_DriftCirclesOnTrack could not be created:");
            ATH_MSG_WARNING("   the RIOs on this detElement will be skipped!");
            //continue;
        } else {
            // ---------------
            // append the created compROT to the MeasurementBase vector:
            m_measurement.push_back(compROT);
        }
    }
    return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// find a barrel extension with RIOs grouped along the globalPositions of the track
///////////////////////////////////////////////////////////////////
StatusCode InDet::TRT_TrackExtensionTool_DAF::groupedBarrelExtension(int beginIndex, int endIndex) const {


    //double squaredMaxGroupDistance = m_jo_maxGroupDistance * m_jo_maxGroupDistance;
    // the pre-cut for RIOs (use twice the real road width):
    const double squaredMaxRIOdistanceFromTrackOnDetElement = m_jo_roadwidth*m_jo_roadwidth*16;

    // -------------------
    // loop over the road elements and extract the global positions
    //    of the track prediction in the x-y-plane:

    std::vector<Amg::Vector3D*> trackGlobalPos;
    trackGlobalPos.reserve(200);

    //     // (x,y) of global track positions
    //     vector<double>  trackGlobalPosX;
    //     vector<double>  trackGlobalPosY;
    // index of the last global position belonging to the detElement
    std::vector<int> detElementGlobPosIndex(m_propagatedTrackParameters.size(), 0);
    //     trackGlobalPosX.reserve(200);
    //     trackGlobalPosY.reserve(200);
    //detElementIndex.reserve(200);

    // do the first iteration manually to get the
    //    last global position
    //    double lastPosX = m_propagatedTrackParameters[beginIndex]->position().x();
    //    double lastPosY = m_propagatedTrackParameters[beginIndex]->position().y();
    //    trackGlobalPosX.push_back( lastPosX );
    //    trackGlobalPosY.push_back( lastPosY );
    ATH_MSG_DEBUG("looping over detElements between index " << beginIndex << " and " << endIndex << "to produce a list of global positions" );

    Amg::Vector3D* lastPos = new Amg::Vector3D( m_propagatedTrackParameters[beginIndex]->position() );
    ATH_MSG_VERBOSE("global position: ("<< lastPos->x() <<", "<< lastPos->y() << ")" );
    // ignore z coordinate (along the straw)
    (*lastPos)[Amg::z]=0.;
    trackGlobalPos.push_back( lastPos );
    detElementGlobPosIndex[beginIndex] =  trackGlobalPos.size()-1;

    for(int index = beginIndex+1; index <= endIndex; ++index) {
        // get the global track position and fill it into vectors
        //double posX = m_propagatedTrackParameters[index]->position().x();
        //double posY = m_propagatedTrackParameters[index]->position().y();
        Amg::Vector3D* Pos = new Amg::Vector3D( m_propagatedTrackParameters[index]->position() );
        // ignore z coordinate (along the straw)
        (*Pos)[Amg::z]=0.;

        //if ( ((posX-lastPosX)*(posX-lastPosX) + (posY-lastPosY)*(posY-lastPosY)) > m_jo_maxGroupDistance * m_jo_maxGroupDistance ) {
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

                //detElementIndex.push_back( index );
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
    
    // -----------------------
    // get all the RIOs on the detElements
    InDet::TRT_DriftCircleContainer::const_iterator containerIterator;
    // loop over detElements
    for(int index = beginIndex+1; index <= endIndex; ++index) {
        ATH_MSG_VERBOSE("trying to get detElement for index "<< index );
        // get the id of the detElement
        IdentifierHash detElementID = m_detectorElements[index]->identifyHash();
        // get the driftCircleCollection belonging to this id
        containerIterator = m_trtcontainer->indexFind(detElementID);
        // loop over RIOs in the collection
        if(containerIterator==m_trtcontainer->end()) {
            ATH_MSG_DEBUG("for the current detectorElement no DriftCircleContainer seems to exist");
            continue;
        }
//         if ((*containerIterator)->begin()==(*containerIterator)->end()){
//            ATH_MSG_DEBUG("containerIterator->begin == containerIterator->end");
//         }
        ATH_MSG_DEBUG( "There are "  << (*containerIterator)->size() << " entries in the TRT_DriftCircleCollection" );

        InDet::TRT_DriftCircleCollection::const_iterator driftCircleIterator = (*containerIterator)->begin();
        for (; driftCircleIterator != (*containerIterator)->end(); driftCircleIterator++) {
            // get the straw center of the RIO
            Amg::Vector3D strawGlobPos( m_detectorElements[index]->center( (*driftCircleIterator)->identify() ) );
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
                    //newDistance = strawGlobPos.distance2(*(trackGlobalPos[i]));
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
                //double distBeforeMin = strawGlobPos.distance2(*(trackGlobalPos[minDistIndex-1]));
                double distBeforeMin = (*(trackGlobalPos[minDistIndex-1]) - strawGlobPos).squaredNorm();
                // distance to the point after the minimum
                //double distAfterMin = strawGlobPos.distance2(*(trackGlobalPos[minDistIndex+1]));
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
        //const Trk::TrackParameters* TrkPar = m_propagator->propagateParameters(*m_siliconTrkParams, RIOsurface, Trk::alongMomentum, false, m_fieldprop, Trk::pion);
        const Trk::TrackParameters* TrkPar = m_propagator->propagateParameters(*m_siliconTrkParams, RIOsurface, Trk::alongMomentum, false, m_fieldprop, Trk::nonInteracting);
        if (!TrkPar) {
            ATH_MSG_WARNING("propagation of track parameters to the RIO surface failed:");
            ATH_MSG_WARNING("   this group of RIOs will skipped!");
            continue;
        }
        ATH_MSG_DEBUG("try to create CompetingTRT_DriftCirclesOnTrackTool with " << groupedRIOs[groupIndex]->size() << " RIOs" );

        //const InDet::CompetingTRT_DriftCirclesOnTrack* compROT = m_compROTcreator->createCompetingROT(*groupedRIOs[groupIndex], *TrkPar, m_jo_annealingFactor);
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
        m_measurement.push_back(compROT);
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
Trk::TrackSegment* InDet::TRT_TrackExtensionTool_DAF::findSegment(const Trk::TrackParameters&) {
    return 0;
}
///////////////////////////////////////////////////////////////////
// Methods for track extension to TRT for pixles+sct tracks 
// and new track production
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::TRT_TrackExtensionTool_DAF::newTrack(const Trk::Track&)
{ 
  return 0;
}
