/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class CompetingSCT_ClustersOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool for CompetingSCT_ClustersOnTrack creation
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

// Header
#include "InDetCompetingRIOsOnTrackTool/CompetingSCT_ClustersOnTrackTool.h"

//#include "GaudiKernel/MsgStream.h"

// data model
#include "TrkEventPrimitives/LocalParameters.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetCompetingRIOsOnTrack/CompetingSCT_ClustersOnTrack.h"
//#include "TrkParameters/AtaStraightLine.h"
//#include "TrkSurfaces/DiscSurface.h"
//#include "TrkSurfaces/StraightLineSurface.h"
//#include "TrkWeightCalculator/IWeightCalculator.h"
// interfaces
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkToolInterfaces/IWeightCalculator.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::CompetingSCT_ClustersOnTrackTool::CompetingSCT_ClustersOnTrackTool(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa ) :
        AthAlgTool(ty,na,pa),
        m_SCT__ROTCreator("InDet::SCT_ClusterOnTrackTool/SCT_ClusterOnTrackTool"),
        m_WeightCalculator("Trk::DAF_SimpleWeightCalculator/DAF_WeightCalculator"),
        m_extrapolator("Trk::Extrapolator/AtlasExtrapolator")
{
    declareInterface<ICompetingSCT_ClustersOnTrackCreator>(this);
    declareInterface<ICompetingRIOsOnTrackTool>(this);
    declareProperty("ToolForSCT_ClusterOnTrackCreation",    m_SCT__ROTCreator,          "SCT_ClusterOnTrackCreator needed for the creation of CompetingSCT_ClustersOnTrack");
    declareProperty("ToolForWeightCalculation",             m_WeightCalculator,         "Tool for weight (assignment probability) calculation");
    declareProperty("WeightCutValueBarrel",                 m_jo_BarrelCutValue=10.8275, "lambda parameter (intrinsic roadwidth) for measurements in the Barrel part");
    declareProperty("WeightCutValueEndCap",                 m_jo_EndCapCutValue=10.8275, "lambda parameter (intrinsic roadwidth) for measurements in the EndCap part");
    declareProperty("Extrapolator",                         m_extrapolator,             "Extrapolator tool");
}

///////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////

InDet::CompetingSCT_ClustersOnTrackTool::~CompetingSCT_ClustersOnTrackTool() {}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::CompetingSCT_ClustersOnTrackTool::initialize() {
    StatusCode sc = AlgTool::initialize();

    ATH_MSG_INFO("SCT ROTCreation by   : " << m_SCT__ROTCreator.name() );
    ATH_MSG_INFO("weight calculation by: " << m_WeightCalculator.name() );
    ATH_MSG_INFO("WeightCutValues are  : " << m_jo_BarrelCutValue<< " (barrel) and " 
         << m_jo_EndCapCutValue<< " (end-cap)");

    // Get the correction tool to create SCT_Clusters on Track
    sc = m_SCT__ROTCreator.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve the SCT ROT creator: "<< m_SCT__ROTCreator );
        return sc;
    }

    // Get the weightCalculator tool to calc the assignment probabilities
    sc = m_WeightCalculator.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve Tool for weight (assignment probability) calculation: "<< m_WeightCalculator );
        return sc;
    }
    // Get extrapolator
    sc = m_extrapolator.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve extrapolator: "<< m_extrapolator );
        return sc;
    }

    return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::CompetingSCT_ClustersOnTrackTool::finalize() {
    StatusCode sc = AlgTool::finalize();
    return sc;
}

///////////////////////////////////////////////////////////////////
// InDet::CompetingSCT_ClustersOnTrack production
///////////////////////////////////////////////////////////////////
const InDet::CompetingSCT_ClustersOnTrack* InDet::CompetingSCT_ClustersOnTrackTool::createCompetingROT(
    const std::list< const Trk::PrepRawData* >& RIO_List,
    const Trk::TrackParameters& trkPar,
    const Trk::IWeightCalculator::AnnealingFactor beta ) const {

    ATH_MSG_DEBUG("********* in createCompetingROT() ********** ");
    // vector of ROTs
    std::vector< const InDet::SCT_ClusterOnTrack* >* ROTvector = new std::vector<const InDet::SCT_ClusterOnTrack*>;
    // vector of assignmentProbs
    std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb > *assgnProbVector = new std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >;
    // type of TRT_BaseElement to check if all RIOs are of same type
    //InDetDD::TRT_BaseElement::Type* TRTtype = 0;
    const Trk::Surface* detElementSurface = 0;
    const Trk::TrackParameters* trkParAtRIOsurface = 0;
    const Trk::TrackParameters* newTrackParameters = 0;
    bool isBarrel = true;
    // maxium assignment propability for choosing the surface....
    //Trk::CompetingRIOsOnTrack::AssignmentProb maximumAssignProb = 0;
    //const InDet::SCT_ClusterOnTrack* ROTwithMaximumAssgnProb = 0;

    ATH_MSG_VERBOSE("trackSurfacePointer: " << trkPar.associatedSurface() << " at ("
                                << trkPar.associatedSurface().center().x() << ", "
                                << trkPar.associatedSurface().center().y() << ", "
                                << trkPar.associatedSurface().center().z() << ")");
    // ----------------------
    // loop over all given PrepRawData
    std::list<const Trk::PrepRawData*>::const_iterator rioIter = RIO_List.begin();
    for (; rioIter!=RIO_List.end(); ++rioIter) {
        // check if given pointer is not NULL
        const InDet::SCT_Cluster* riopointer = dynamic_cast<const InDet::SCT_Cluster*>(*rioIter);
        if (!riopointer) {
            //ATH_MSG_WARNING("That's mean: given list of PrepRawData* contains a NULL pointer resp. not a SCT_Cluster!");
            ATH_MSG_WARNING("Given list of PrepRawData* contains a non SCT_Cluster!");
            ATH_MSG_WARNING("Entry will therefore be ignored!");
            continue;
        }
        // ---------------------------------------------------
        // get trackParameters on the surface of the first RIO
        if (!detElementSurface) {
            // get surface of detector element of first RIO
            detElementSurface = &(riopointer->detectorElement()->surface());
            isBarrel = riopointer->detectorElement()->isBarrel();
            // check if track parameters are expressed on the RIO surface
            if ( (*detElementSurface)==((trkPar.associatedSurface())) ) {
                // track parameters are given on the surface of the RIO, use them
                ATH_MSG_VERBOSE("TrackParameters are on RIO surface: GP ("
                                        << trkPar.position().x() << ", "
                                        << trkPar.position().y() << ", "
                                        << trkPar.position().z() << ") GM ("
                                        << trkPar.momentum().x() << ", "
                                        << trkPar.momentum().y() << ", "
                                        << trkPar.momentum().z() << ") LP ("
                                        << trkPar.localPosition()[Trk::distEta]  << ", "
                                        << trkPar.localPosition()[Trk::distPhi]  << ")");
                trkParAtRIOsurface = &trkPar;
            } else {
                // first create trkParameter on the Surface of the RIO
                // clone TrkParameters without error to force the extrapolator to do propagation without error matrix
                const Trk::TrackParameters* trkParWithoutError = trkPar.clone();
                // extrapolate to RIO surface
                ATH_MSG_VERBOSE("Try to propagate TrackParameters to compROT surface");
                newTrackParameters = m_extrapolator->extrapolateDirectly((trkParWithoutError ? *trkParWithoutError : trkPar), *detElementSurface,
                                                                        Trk::anyDirection, // propagate in any direction
                                                                        false, //do noBoundaryCheck!
                                                                        Trk::nonInteracting); // without material interaction
                delete trkParWithoutError;
                trkParWithoutError = 0;
                if (!newTrackParameters){
                    ATH_MSG_ERROR("TrackParameters could not be propagated to PrepRawData surface");
                    delete ROTvector;
                    // FIXME: delete entries of ROT vector
                    delete assgnProbVector;
                    return 0;
                } // end if (extrapolation failed)
                // const Trk::AtaStraightLine* trkParAtRIOsurface1 = new Trk::AtaStraightLine(trkPar.position(), trkPar.momentum(), trkPar.charge(), *RIOsurfacePointer);
                trkParAtRIOsurface = newTrackParameters;
                ATH_MSG_VERBOSE("propagated TrackParameters on RIO surface: GP ("
                                        << trkParAtRIOsurface->position().x() << ", "
                                        << trkParAtRIOsurface->position().y() << ", "
                                        << trkParAtRIOsurface->position().z() << ") GM ("
                                        << trkParAtRIOsurface->momentum().x() << ", "
                                        << trkParAtRIOsurface->momentum().y() << ", "
                                        << trkParAtRIOsurface->momentum().z() << ") LP ("
                                        << trkParAtRIOsurface->localPosition()[Trk::distEta]  << ", "
                                        << trkParAtRIOsurface->localPosition()[Trk::distPhi]  << ")");
            } // end if (track parameters on RIO surface)
        } // end if(!detElementSurface)

        // ---------------------------------
        // check if all RIOs are on the same detector module:
        if (riopointer->detectorElement()->surface() != (*detElementSurface)) {
            ATH_MSG_WARNING("Given SCT_ PrepRawData do not belong to the same detector element:");
            ATH_MSG_WARNING("Current entry of PrepRawData list will be ignored!!!");
            // Ignore current RIO and jump do next RIO
            continue;
        }
        // ---------------------------------
        // create ROT from PrepRawData:
        const InDet::SCT_ClusterOnTrack* rot = dynamic_cast<const InDet::SCT_ClusterOnTrack*>( m_SCT__ROTCreator->correct(*riopointer, *trkParAtRIOsurface));
        if (!rot) {
            ATH_MSG_WARNING("SCT_ClusterOnTrack could not be created");
        } else {
            //ATH_MSG_VERBOSE("Created ROT");
            // add ROT to vector of ROTs
            ROTvector->push_back(rot);
            // call weightcalculator and calc assignment probabilty:
            ATH_MSG_VERBOSE("Call weight calculator for non-normalized assignment probability");
            Trk::CompetingRIOsOnTrack::AssignmentProb assgnProb = m_WeightCalculator->calculateWeight(*trkParAtRIOsurface, *rot, beta);
            ATH_MSG_VERBOSE("Current ROT has assignment probability: " << assgnProb );
            assgnProbVector->push_back( assgnProb );
//            if (assgnProb >= maximumAssignProb) {
//                 ATH_MSG_VERBOSE("Current ROT has maximum assignment probabilty for now");
//                 maximumAssignProb = assgnProb;
//                 ROTwithMaximumAssgnProb = rot;
//             }
        }// end else (!rot)
    } // end for loop
    delete newTrackParameters;

    // -------------------------------------
    // test if at least one ROT was created:
    if (ROTvector->size() <= 0) {
        ATH_MSG_ERROR("No valid SCT_ClusterOnTrack could be created:");
        ATH_MSG_ERROR("CompetingSCT_ClustersOnTrack creation aborted!");
        //clean-up
        delete ROTvector;
        delete assgnProbVector;
        return 0;
    }
    ATH_MSG_DEBUG("List of competing SCT_ ROTs contains "<< ROTvector->size() << " SCT_ClustersOnTrack");

    // -----------------------------------
    // normalize assignment probabilities:
    // copy ROTvector to base class vector (vector of RIO_OnTrack) because vector<> does not know inheritance structure
    std::vector< const Trk::RIO_OnTrack* >* baseROTvector = new std::vector<const Trk::RIO_OnTrack*>;
    std::vector< const InDet::SCT_ClusterOnTrack*>::const_iterator rotIter = ROTvector->begin();
    for (; rotIter!=ROTvector->end(); ++rotIter) {
        baseROTvector->push_back(*rotIter);
    }
    // call normalize()
    if (isBarrel) {
        ATH_MSG_DEBUG("Call weight calculator for normalization now (Barrel cut)");
        m_WeightCalculator->normalize(*assgnProbVector, baseROTvector, beta, m_jo_BarrelCutValue);
    } else {
        ATH_MSG_DEBUG("Call weight calculator for normalization now (end-cap cut)");
        m_WeightCalculator->normalize(*assgnProbVector, baseROTvector, beta, m_jo_EndCapCutValue);
    }
    delete baseROTvector;

    // ---------------------------------------
    // create CompetingSCT_ClustersOnTrack
    //return (new CompetingSCT_ClustersOnTrack(assocSurface, ROTvector, assgnProbVector, effectiveLocalPar, effectiveErrMat, ROTsHaveCommonSurface));
    CompetingSCT_ClustersOnTrack* theCompetingROT = new CompetingSCT_ClustersOnTrack(ROTvector, assgnProbVector);
    if (msgLvl(MSG::VERBOSE)) testCompetingROT(*theCompetingROT);
    return theCompetingROT;
}


///////////////////////////////////////////////////////////////////
// InDet::CompetingSCT_ClustersOnTrack update
///////////////////////////////////////////////////////////////////
void InDet::CompetingSCT_ClustersOnTrackTool::updateCompetingROT(
    //const InDet::CompetingSCT_ClustersOnTrack& compROT,
    Trk::CompetingRIOsOnTrack& baseCompROT,
    const Trk::TrackParameters& trkPar,
    const Trk::IWeightCalculator::AnnealingFactor beta
    //const bool recreateROTs=false
) const {

    //   TODO:  if recreateROTs==true call standard createCompROT method

    ATH_MSG_DEBUG("********* in updateCompetingROT() **********");

    // cast baseCompROT to CompSCT_ClusterOnTrack:
    InDet::CompetingSCT_ClustersOnTrack* compROT = dynamic_cast< InDet::CompetingSCT_ClustersOnTrack* >(&baseCompROT);
    if (!compROT) {
        ATH_MSG_WARNING("Given CompetingRIOsOnTrack is not a CompetingSCT_ClustersOnTrack!");
        ATH_MSG_WARNING("Update of assignment probabilities aborted!!!");
        return;
    }
    // new vector of assignmentProbs
    std::vector< InDet::CompetingSCT_ClustersOnTrack::AssignmentProb >* assgnProbVector = new std::vector< InDet::CompetingSCT_ClustersOnTrack::AssignmentProb >;
    // maxium assignment propability to update the index
    Trk::CompetingRIOsOnTrack::AssignmentProb maximumAssignProb = 0;
    unsigned int maximumAssignProbIndex = 0;

    ATH_MSG_VERBOSE("trackSurfacePointer: " << trkPar.associatedSurface() << " at ("
                                << trkPar.associatedSurface().center().x() << ", "
                                << trkPar.associatedSurface().center().y() << ", "
                                << trkPar.associatedSurface().center().z() << ")");
    const Trk::TrackParameters* newTrackParameters = 0;
    const Trk::TrackParameters* trkParAtROTsurface = 0;
    // ---------------------------------------------------
    // get trackParameters on the surface of the compROT
    // check if track parameters are expressed on the compROT surface
    if (compROT->associatedSurface()==((trkPar.associatedSurface())) ) {
        // track parameters are given on the surface of the RIO, use them
        ATH_MSG_VERBOSE("TrackParameters are on compROT surface: GP ("
                                << trkPar.position().x() << ", "
                                << trkPar.position().y() << ", "
                                << trkPar.position().z() << ") GM ("
                                << trkPar.momentum().x() << ", "
                                << trkPar.momentum().y() << ", "
                                << trkPar.momentum().z() << ") LP ("
                                << trkPar.localPosition()[Trk::distPhi]  << ", "
                                << trkPar.localPosition()[Trk::distEta]  << ")");
        trkParAtROTsurface = &trkPar;
    } else {
        // first create trkParameter on the Surface of the compROT
        // clone TrkParameters without error to force the extrapolator to do propagation without error matrix
        const Trk::TrackParameters* trkParWithoutError = trkPar.clone();
        ATH_MSG_VERBOSE("Try to propagate TrackParameters to compROT surface");
        newTrackParameters = m_extrapolator->extrapolateDirectly((trkParWithoutError ? *trkParWithoutError : trkPar), compROT->associatedSurface(),
                                                                Trk::anyDirection, // propagate in any direction
                                                                false, //do noBoundaryCheck!
                                                                Trk::nonInteracting); // without material interaction
        delete trkParWithoutError;
        trkParWithoutError = 0;
        if (!newTrackParameters){
            ATH_MSG_ERROR("TrackParameters could not be propagated to compROT surface:");
            ATH_MSG_ERROR("    CompetingSCT_ClustersOnTrack could not be updated!");
            delete assgnProbVector;
            return;
        } // end if (extrapolation failed)
        // const Trk::AtaStraightLine* trkParAtRIOsurface1 = new Trk::AtaStraightLine(trkPar.position(), trkPar.momentum(), trkPar.charge(), *RIOsurfacePointer);
        trkParAtROTsurface = newTrackParameters;
        ATH_MSG_VERBOSE("propagated TrackParameters on compROT surface: GP ("
                                << trkParAtROTsurface->position().x() << ", "
                                << trkParAtROTsurface->position().y() << ", "
                                << trkParAtROTsurface->position().z() << ") GM ("
                                << trkParAtROTsurface->momentum().x() << ", "
                                << trkParAtROTsurface->momentum().y() << ", "
                                << trkParAtROTsurface->momentum().z() << ") LP ("
                                << trkParAtROTsurface->localPosition()[Trk::distPhi]  << ", "
                                << trkParAtROTsurface->localPosition()[Trk::distEta]  << ")");
    } // end if (track parameters on compROT surface)
    // ----------------------
    // loop over all ROTs in the CompetingROT
    for (unsigned int i=0; i<compROT->numberOfContainedROTs(); i++) {
        // call weightcalculator and calc assignment probabilty:
        Trk::CompetingRIOsOnTrack::AssignmentProb assgnProb = m_WeightCalculator->calculateWeight(*trkParAtROTsurface, (compROT->rioOnTrack(i)), beta);
        ATH_MSG_VERBOSE("non-normalized weight for ROT "<< i << ": " << assgnProb );
        assgnProbVector->push_back( assgnProb );
        if (assgnProb >= maximumAssignProb) {
            ATH_MSG_VERBOSE("current ROT has maximum assignment probability up to now" );
            maximumAssignProb = assgnProb;
            maximumAssignProbIndex = i;
        }
    } // end for loop
    delete newTrackParameters;
    if (maximumAssignProb > 0. ) {
        // -----------------------------------
        // normalize assignment probabilities:
        // copy ROTvector to base class vector (vector of RIO_OnTrack) because vector<> does not know inheritance structure
        std::vector< const Trk::RIO_OnTrack* >* baseROTvector = new std::vector<const Trk::RIO_OnTrack*>;
        std::vector< const InDet::SCT_ClusterOnTrack* >::const_iterator rotIter = compROT->containedROTs().begin();
        for (; rotIter!=compROT->containedROTs().end(); ++rotIter) {
            baseROTvector->push_back(*rotIter);
        }
        // call normalize()
        ATH_MSG_VERBOSE("normalize the assignment probabilities");
        if(compROT->rioOnTrack(0).detectorElement()->isBarrel()) {
            ATH_MSG_DEBUG("Call weight calculator for normalization now (Barrel cut)");
            m_WeightCalculator->normalize(*assgnProbVector, baseROTvector, beta, m_jo_BarrelCutValue);
        } else {
            ATH_MSG_DEBUG("Call weight calculator for normalization now (end-cap cut)");
            m_WeightCalculator->normalize(*assgnProbVector, baseROTvector, beta, m_jo_EndCapCutValue);
        }
        delete baseROTvector;
    } else {
        ATH_MSG_VERBOSE("all ROTs have probability 0.");
        maximumAssignProbIndex = 0;
    }
    // ---------------------------------
    // update the competingROT
    // set new assignment probabilities
    delete compROT->m_assignProb;
    compROT->m_assignProb = assgnProbVector;
    // update maximum assign prob index:
    compROT->m_indexMaxAssignProb = maximumAssignProbIndex;
    // delete global position (will be recreated by the competingROT itself)
    if (compROT->m_globalPosition) {
        delete compROT->m_globalPosition.release().get();
    }
    // delete localParameters
    compROT->setLocalParametersAndErrorMatrix();
    if (msgLvl(MSG::VERBOSE)) testCompetingROT(*compROT);
}

void InDet::CompetingSCT_ClustersOnTrackTool::reequipCompetingROT
(Trk::CompetingRIOsOnTrack*& modifiableCompROT,
 const Trk::RIO_OnTrack*     newROT,
 const Trk::TrackParameters& trkPar,
 const Trk::IWeightCalculator::AnnealingFactor beta,
 const Trk::TrackState::MeasurementType ) const
{

  InDet::CompetingSCT_ClustersOnTrack* cst
    = dynamic_cast<InDet::CompetingSCT_ClustersOnTrack*>(modifiableCompROT);
  const InDet::SCT_ClusterOnTrack* newCluster 
    = dynamic_cast<const InDet::SCT_ClusterOnTrack*>(newROT);
  if (!cst || !newCluster) {
    ATH_MSG_WARNING( "inconsistent use of reequipCompetingROT() " );
    return;
  }
  std::vector<const InDet::SCT_ClusterOnTrack*>::const_iterator
    rotIter = cst->m_containedChildRots->begin();
  for (; rotIter!=cst->m_containedChildRots->end(); ++rotIter)
    delete (*rotIter);
  delete cst->m_containedChildRots;
  cst->m_containedChildRots = new std::vector<const InDet::SCT_ClusterOnTrack*>;
  cst->m_containedChildRots->push_back(newCluster);

  this->updateCompetingROT(*modifiableCompROT, trkPar, beta);
}


void InDet::CompetingSCT_ClustersOnTrackTool::testCompetingROT(const InDet::CompetingSCT_ClustersOnTrack& compROT)const {

    if (!msgLvl(MSG::VERBOSE)) return;

    msg(MSG::VERBOSE)<<"**Test**"<<endmsg;

    msg()<<"  - associatedSurface(): ";
    const Trk::Surface* assocsurface = &(compROT.associatedSurface());
    //msg(MSG::VERBOSE)<<"       distance of surface center from origin: " << assocsurface->center().distance()<<endmsg;
    msg()<< assocsurface << " at ("<< assocsurface->center().x() << ", "<< assocsurface->center().y() << ", "<< assocsurface->center().z() << ")" <<endmsg;

    msg()<<"  - containedROTs(): ";
    //msg()<<"       size of ROT vector: " <<  compROT.containedROTs().size()<<endmsg;
    msg()<<" numberOfContainedROTs(): " << compROT.numberOfContainedROTs()<<endmsg;

    msg()<<"  - eff. localParameters():";
    //msg()<<"       dimension of the effective local params: " <<  compROT.localParameters().dimension()<<endmsg;
    msg()<<" dim: " <<  compROT.localParameters().dimension();
    //msg()<<"       effective localPar[locX]: " <<  compROT.localParameters()[Trk::locX] <<endmsg;
    msg()<<" [locX]: " <<  compROT.localParameters()[Trk::locX] <<endmsg;

    if (compROT.assignmentProbability(compROT.indexOfMaxAssignProb()) > 0. ) {
        // if maxAssgnProb==0 the weight matrix is 0 and therefore singular => can not get covariance
        msg()<<"  - localErrorMatrix():";
        msg()<<" dimCov: " <<  compROT.localCovariance().rows();
        msg()<<" cov[loc1]: " <<  compROT.localCovariance()(Trk::loc1,Trk::loc1) << endmsg;
    }

    msg()<<"  - indexOfMaxAssignProb(): " <<  compROT.indexOfMaxAssignProb() <<endmsg;
    //msg()<<"        assignmentProbability( indexOfMaxAssignProb() ): " <<  compROT.assignmentProbability(compROT.indexOfMaxAssignProb()) <<endmsg;

    msg()<<"  - assignmentProbability(index) and rioOnTrack(index)"<<endmsg;
    for (unsigned int i=0; i<compROT.numberOfContainedROTs(); i++) {
        msg()<<"  ["<< i  <<"] assgnProb: "<< compROT.assignmentProbability(i)
                                            << " ROT ID: "<< compROT.rioOnTrack(i).identify().getString() 
                                            << " [locX]: "<< compROT.rioOnTrack(i).localParameters()[Trk::locX]
                                            << " ROT surf: "<< &(compROT.rioOnTrack(i).associatedSurface())
                                            << endmsg;
        if (compROT.assignmentProbability(i) > compROT.assignmentProbability(compROT.indexOfMaxAssignProb()) ) {
            ATH_MSG_ERROR(" assignment probability larger than maximum assignment probability!!!");
        }
    }


    msg()<<"  - eff. globalPosition(): ";
    //msg()<<"       distance of effective measurement from origin: " <<  compROT.globalPosition().distance()<<endmsg;
    msg()<< " ("<< compROT.globalPosition().x() << ", "<< compROT.globalPosition().y() << ", "<< compROT.globalPosition().z() << ")" <<endmsg;
    /*    msg()<<"  - clone()"<<endmsg;
        CompetingSCT_ClustersOnTrack* newcompROT = compROT.clone();
        if (!newcompROT){
            m_log<<MSG::ERROR<<"  Got no clone!"<<endmsg;
        } else {
            msg()<<"   Got the clone: do the same tests with it:"<<endmsg;
            
            msg()<<"  clone- associatedSurface()"<<endmsg;
            const Trk::Surface* cloneassocsurface = &(newcompROT->associatedSurface());
            msg()<<"       distance of surface center from origin: " << cloneassocsurface->center().distance()<<endmsg;
            
            msg()<<"  clone- containedROTs()"<<endmsg;
            msg()<<"       size of ROT vector: " <<  newcompROT->containedROTs().size()<<endmsg;
            msg()<<"  clone- numberOfContainedROTs(): " << newcompROT->numberOfContainedROTs()<<endmsg;
            
            msg()<<"  clone- localParameters()"<<endmsg;
            msg()<<"       dimension of the effective local params: " <<  newcompROT->localParameters().dimension()<<endmsg;
        
            msg()<<"  clone- localErrorMatrix()"<<endmsg;
            msg()<<"       dimension of covariance matrix: " <<  newcompROT->localErrorMatrix().covariance().num_row()<<endmsg;
            msg()<<"       error of first local coordinate:  " <<  newcompROT->localErrorMatrix().error(Trk::loc1)<<endmsg;
            msg()<<"       weight value of first local coordinate: " <<  newcompROT->localErrorMatrix().weightValue(Trk::loc1)<<endmsg;
            
            msg()<<"  clone- assignmentProbability(index) and rioOnTrack(index)"<<endmsg;
            for (unsigned int i=0; i<newcompROT->numberOfContainedROTs(); i++) {
                msg()<<"      ["<< i <<"] assgnProb: "<< newcompROT->assignmentProbability(i)<< "ROT identifier:"<< newcompROT->rioOnTrack(i)->identify().getString() <<endmsg;
            }
            
            msg()<<"  clone- globalPosition()"<<endmsg;
            msg()<<"       distance of effective measurement from origin: " <<  newcompROT->globalPosition().distance()<<endmsg;    
        }
        delete newcompROT;*/
    
}

StatusCode InDet::CompetingSCT_ClustersOnTrackTool::updateCompetingROTprobs( 
        Trk::CompetingRIOsOnTrack& baseCompROT, 
        const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >& assignmentProbs 
    ) const { 
    InDet::CompetingSCT_ClustersOnTrack* compROT = dynamic_cast< InDet::CompetingSCT_ClustersOnTrack* >(&baseCompROT); 
    if (!compROT) { 
        ATH_MSG_ERROR("Given CompetingRIOsOnTrack is not a CompetingPixelClustersOnTrack!"); 
        ATH_MSG_ERROR("Update of assignment probabilities aborted!!!"); 
        return StatusCode::FAILURE; 
    } 
    if (assignmentProbs.size() != compROT->numberOfContainedROTs() ) { 
        ATH_MSG_ERROR("Given size of vector of assignment probabilities does not match number of contained ROTs in CompetingRIOsOnTrack!"); 
        ATH_MSG_ERROR("Update of assignment probabilities aborted!!!"); 
        return StatusCode::FAILURE; 
    } 
    // --------------------------------- 
    // update the competingROT 
    // set new assignment probabilities 
    delete compROT->m_assignProb; 
    std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >* assgnProbVector = new std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >(assignmentProbs); 
    // update maximum assign prob index: 
    double maximumAssignProb = 0.; 
    compROT->m_indexMaxAssignProb = 0; 
    for (unsigned int i=0; i<assignmentProbs.size(); i++) { 
        if (assignmentProbs[i] >= maximumAssignProb) { 
            //ATH_MSG_VERBOSE("current ROT has maximum assignment probability up to now" ); 
            maximumAssignProb = assignmentProbs[i]; 
            compROT->m_indexMaxAssignProb = i; 
        } 
        if (assignmentProbs[i] < 0.) { 
            ATH_MSG_WARNING("Got negative assignment probability, setting to 0 !"); 
            (*(assgnProbVector))[i] = 0.; 
        } 
    } // end for loop 
    compROT->m_assignProb = assgnProbVector; 
    // delete global position (will be recreated by the competingROT itself) 
    if (compROT->m_globalPosition) {
        delete compROT->m_globalPosition.release().get(); 
    }
    // recalc localParameters 
    compROT->setLocalParametersAndErrorMatrix();
    if (msgLvl(MSG::DEBUG)) testCompetingROT(*compROT); 
    return StatusCode::SUCCESS; 
} 
