/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class CompetingPixelClustersOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool for CompetingPixelClustersOnTrack creation
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

// Header
#include "InDetCompetingRIOsOnTrackTool/CompetingPixelClustersOnTrackTool.h"

//#include "GaudiKernel/MsgStream.h"

// data model
#include "TrkEventPrimitives/LocalParameters.h"
#include "InDetPrepRawData/PixelCluster.h"
//#include "InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack.h"
//#include "TrkParameters/AtaStraightLine.h"
//#include "TrkSurfaces/DiscSurface.h"
//#include "TrkSurfaces/StraightLineSurface.h"

#include "InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack.h"


// interfaces
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkToolInterfaces/IWeightCalculator.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::CompetingPixelClustersOnTrackTool::CompetingPixelClustersOnTrackTool(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa ) :
        AthAlgTool(ty,na,pa),
        m_Pixel_ROTCreator("InDet::PixelClusterOnTrackTool/PixelClusterOnTrackTool"),
        m_WeightCalculator("Trk::DAF_SimpleWeightCalculator/DAF_WeightCalculator"),
        m_extrapolator("Trk::Extrapolator/AtlasExtrapolator")
{
    declareInterface<ICompetingPixelClustersOnTrackCreator>(this);
    declareProperty("ToolForPixelClusterOnTrackCreation",   m_Pixel_ROTCreator,         "PixelClusterOnTrackCreator needed for the creation of CompetingPixelClustersOnTrack");
    declareProperty("ToolForWeightCalculation",             m_WeightCalculator,         "Tool for weight (assignment probability) calculation");
    declareProperty("WeightCutValueBarrel",                 m_jo_BarrelCutValue=13.81551,"lambda parameter (intrinsic roadwidth) for measurements in the Barrel part");
    declareProperty("WeightCutValueEndCap",                 m_jo_EndCapCutValue=13.81551,"lambda parameter (intrinsic roadwidth) for measurements in the EndCap part");
    declareProperty("Extrapolator",                         m_extrapolator,             "Extrapolator tool");
}

///////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////

InDet::CompetingPixelClustersOnTrackTool::~CompetingPixelClustersOnTrackTool() {}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::CompetingPixelClustersOnTrackTool::initialize() {
    StatusCode sc = AlgTool::initialize();

    ATH_MSG_INFO("Pixel ROTCreation by : " << m_Pixel_ROTCreator.name() );
    ATH_MSG_INFO("weight calculation by: " << m_WeightCalculator.name() );
    ATH_MSG_INFO("WeightCutValues are  : " << m_jo_BarrelCutValue<< " (barrel) and "
         << m_jo_EndCapCutValue<< " (end-cap)");

    // Get the correction tool to create PixelClusters on Track
    sc = m_Pixel_ROTCreator.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve the Pixel ROT creator: "<< m_Pixel_ROTCreator );
        return sc;
    }

    // Get the weightCalculator tool to calc the assignment probabilities
    sc = m_WeightCalculator.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve Tool for weight (assignment probability) calculation: "<< m_WeightCalculator );
        return sc;
    }
    // -------------
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

StatusCode InDet::CompetingPixelClustersOnTrackTool::finalize() {
    StatusCode sc = AlgTool::finalize();
    return sc;
}

///////////////////////////////////////////////////////////////////
// InDet::CompetingPixelClustersOnTrack production
///////////////////////////////////////////////////////////////////
const InDet::CompetingPixelClustersOnTrack* InDet::CompetingPixelClustersOnTrackTool::createCompetingROT(
    const std::list< const Trk::PrepRawData* >& RIO_List,
    const Trk::TrackParameters& trkPar,
    const Trk::IWeightCalculator::AnnealingFactor beta ) const {

    ATH_MSG_DEBUG("********* in createCompetingROT() ********** ");
    // vector of ROTs
    std::vector< const InDet::PixelClusterOnTrack* >* ROTvector = new std::vector<const InDet::PixelClusterOnTrack*>;
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
    //const InDet::PixelClusterOnTrack* ROTwithMaximumAssgnProb = 0;

    ATH_MSG_VERBOSE("trackSurfacePointer: " << &trkPar.associatedSurface() << " at ("
                                << trkPar.associatedSurface().center().x() << ", "
                                << trkPar.associatedSurface().center().y() << ", "
                                << trkPar.associatedSurface().center().z() << ")");
    // clone TrkParameters without error to force the extrapolator to do propagation without error matrix
    const Trk::TrackParameters* trkParWithoutError = trkPar.clone();
    // ----------------------
    // loop over all given PrepRawData
    std::list<const Trk::PrepRawData*>::const_iterator rioIter = RIO_List.begin();
    for (; rioIter!=RIO_List.end(); ++rioIter) {
        // check if given pointer is not NULL
        const InDet::PixelCluster* riopointer = dynamic_cast<const InDet::PixelCluster*>(*rioIter);
        if (!riopointer) {
            //ATH_MSG_WARNING("That's mean: given list of PrepRawData* contains a NULL pointer resp. not a PixelCluster!");
            ATH_MSG_WARNING("Given list of PrepRawData* contains a non PixelCluster!");
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
                // extrapolate to RIO surface
                ATH_MSG_VERBOSE("Try to propagate TrackParameters to RIO surface");
                newTrackParameters = m_extrapolator->extrapolateDirectly((trkParWithoutError ? *trkParWithoutError : trkPar), *detElementSurface,
                                                                        Trk::anyDirection, // propagate in any direction
                                                                        false, //do noBoundaryCheck!
                                                                        Trk::nonInteracting); // without material interaction
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
            ATH_MSG_WARNING("Given Pixel PrepRawData do not belong to the same detector element:");
            ATH_MSG_WARNING("Current entry of PrepRawData list will be ignored!!!");
            // Ignore current RIO and jump do next RIO
            continue;
        }
        // ---------------------------------
        // create ROT from PrepRawData:
        const InDet::PixelClusterOnTrack* rot = dynamic_cast<const InDet::PixelClusterOnTrack*>( m_Pixel_ROTCreator->correct(*riopointer, *trkParAtRIOsurface));
        if (!rot) {
            ATH_MSG_WARNING("PixelClusterOnTrack could not be created");
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
    delete trkParWithoutError;
    trkParWithoutError = 0;

    // -------------------------------------
    // test if at least one ROT was created:
    if (ROTvector->size() <= 0) {
        ATH_MSG_ERROR("No valid PixelClusterOnTrack could be created:");
        ATH_MSG_ERROR("CompetingPixelClustersOnTrack creation aborted!");
        //clean-up
        delete ROTvector;
        delete assgnProbVector;
        return 0;
    }
    ATH_MSG_DEBUG("List of competing Pixel ROTs contains "<< ROTvector->size() << " PixelClustersOnTrack");

    // -----------------------------------
    // normalize assignment probabilities:
    // copy ROTvector to base class vector (vector of RIO_OnTrack) because vector<> does not know inheritance structure
    std::vector< const Trk::RIO_OnTrack* >* baseROTvector = new std::vector<const Trk::RIO_OnTrack*>;
    std::vector< const InDet::PixelClusterOnTrack*>::const_iterator rotIter = ROTvector->begin();
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
    // create CompetingPixelClustersOnTrack
    //return (new CompetingPixelClustersOnTrack(assocSurface, ROTvector, assgnProbVector, effectiveLocalPar, effectiveErrMat, ROTsHaveCommonSurface));
    CompetingPixelClustersOnTrack* theCompetingROT = new CompetingPixelClustersOnTrack(ROTvector, assgnProbVector);
    if (msgLvl(MSG::DEBUG)) testCompetingROT(*theCompetingROT);
    return theCompetingROT;
}


///////////////////////////////////////////////////////////////////
// InDet::CompetingPixelClustersOnTrack update
///////////////////////////////////////////////////////////////////
void InDet::CompetingPixelClustersOnTrackTool::updateCompetingROT(
    //const InDet::CompetingPixelClustersOnTrack& compROT,
    const Trk::CompetingRIOsOnTrack& baseCompROT,
    const Trk::TrackParameters& trkPar,
    const Trk::IWeightCalculator::AnnealingFactor beta
    //const bool recreateROTs=false
) const {

    //   TODO:  if recreateROTs==true call standard createCompROT method

    ATH_MSG_DEBUG("********* in updateCompetingROT() **********");

    // cast baseCompROT to CompPixelClusterOnTrack:
    const InDet::CompetingPixelClustersOnTrack* compROT = dynamic_cast< const InDet::CompetingPixelClustersOnTrack* >(&baseCompROT);
    if (!compROT) {
        ATH_MSG_ERROR("Given CompetingRIOsOnTrack is not a CompetingPixelClustersOnTrack!");
        ATH_MSG_ERROR("Update of assignment probabilities aborted!!!");
        return;
    }
    // new vector of assignmentProbs
    std::vector< InDet::CompetingPixelClustersOnTrack::AssignmentProb >* assgnProbVector = new std::vector< InDet::CompetingPixelClustersOnTrack::AssignmentProb >;
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
        // extrapolate to compROT surface
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
            ATH_MSG_ERROR("    CompetingPixelClustersOnTrack could not be updated!");
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
            //ROTwithMaximumAssgnProb = compROT->rioOnTrack(i);
        }
    } // end for loop
    delete newTrackParameters;
    if (maximumAssignProb > 0. ) {
        // -----------------------------------
        // normalize assignment probabilities:
        // copy ROTvector to base class vector (vector of RIO_OnTrack) because vector<> does not know inheritance structure
        std::vector< const Trk::RIO_OnTrack* >* baseROTvector = new std::vector<const Trk::RIO_OnTrack*>;
        std::vector< const InDet::PixelClusterOnTrack* >::const_iterator rotIter = compROT->containedROTs().begin();
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
    delete compROT->m_globalPosition;
    compROT->m_globalPosition = 0; // very important, otherwise segfault...
    const_cast<InDet::CompetingPixelClustersOnTrack*>(compROT)->setLocalParametersAndErrorMatrix();
    if (msgLvl(MSG::DEBUG)) testCompetingROT(*compROT);
}

void InDet::CompetingPixelClustersOnTrackTool::reequipCompetingROT
(Trk::CompetingRIOsOnTrack*& modifiableCompROT,
 const Trk::RIO_OnTrack*     newROT,
 const Trk::TrackParameters& trkPar,
 const Trk::IWeightCalculator::AnnealingFactor beta,
 const Trk::TrackState::MeasurementType ) const
{
  InDet::CompetingPixelClustersOnTrack* cpx
    = dynamic_cast<InDet::CompetingPixelClustersOnTrack*>(modifiableCompROT);
  const InDet::PixelClusterOnTrack* newCluster 
    = dynamic_cast<const InDet::PixelClusterOnTrack*>(newROT);
  if (!cpx || !newCluster) {
    ATH_MSG_WARNING( "inconsistent use of reequipCompetingROT() " );
    return;
  }
  std::vector<const InDet::PixelClusterOnTrack*>::const_iterator
    rotIter = cpx->m_containedChildRots->begin();
  for (; rotIter!=cpx->m_containedChildRots->end(); ++rotIter)
    delete (*rotIter);
  delete cpx->m_containedChildRots;
  cpx->m_containedChildRots = new std::vector<const InDet::PixelClusterOnTrack*>;
  cpx->m_containedChildRots->push_back(newCluster);

  this->updateCompetingROT(*modifiableCompROT, trkPar, beta);
}




void InDet::CompetingPixelClustersOnTrackTool::testCompetingROT(const InDet::CompetingPixelClustersOnTrack& compROT)const {

    if (!msgLvl(MSG::VERBOSE)) return;

    ATH_MSG_VERBOSE("**Test**");

    ATH_MSG_VERBOSE("  - associatedSurface(): ");
    const Trk::Surface* assocsurface = &(compROT.associatedSurface());
    //ATH_MSG_VERBOSE("       distance of surface center from origin: " << assocsurface->center().distance());
    ATH_MSG_VERBOSE( assocsurface << " at ("<< assocsurface->center().x() << ", "<< assocsurface->center().y() << ", "<< assocsurface->center().z() << ")" );

    ATH_MSG_VERBOSE("  - containedROTs(): ");
    //ATH_MSG_VERBOSE("       size of ROT vector: " <<  compROT.containedROTs().size());
    ATH_MSG_VERBOSE(" numberOfContainedROTs(): " << compROT.numberOfContainedROTs());

    ATH_MSG_VERBOSE("  - eff. localParameters():");
    //ATH_MSG_VERBOSE("       dimension of the effective local params: " <<  compROT.localParameters().dimension());
    ATH_MSG_VERBOSE(" dim: " <<  compROT.localParameters().dimension());
    //ATH_MSG_VERBOSE("       effective localPar[locX]: " <<  compROT.localParameters()[Trk::locX] );
    ATH_MSG_VERBOSE(" [locX]: " <<  compROT.localParameters()[Trk::locX] );

    if (compROT.assignmentProbability(compROT.indexOfMaxAssignProb()) > 0. ) {
        // if maxAssgnProb==0 the weight matrix is 0 and therefore singular => can not get covariance
        ATH_MSG_VERBOSE("  - localErrorMatrix():");
        ATH_MSG_VERBOSE(" dimCov: " <<  compROT.localCovariance().rows());
        ATH_MSG_VERBOSE(" cov[loc1]: " <<  compROT.localCovariance()(Trk::loc1,Trk::loc1));
    }

    ATH_MSG_VERBOSE("  - indexOfMaxAssignProb(): " <<  compROT.indexOfMaxAssignProb() );
    //ATH_MSG_VERBOSE("        assignmentProbability( indexOfMaxAssignProb() ): " <<  compROT.assignmentProbability(compROT.indexOfMaxAssignProb()) );

    ATH_MSG_VERBOSE("  - assignmentProbability(index) and rioOnTrack(index)");
    for (unsigned int i=0; i<compROT.numberOfContainedROTs(); i++) {
        ATH_MSG_VERBOSE("  ["<< i  <<"] assgnProb: "<< compROT.assignmentProbability(i)
                                            << " ROT ID: "<< compROT.rioOnTrack(i).identify().getString() 
                                            << " [locX]: "<< compROT.rioOnTrack(i).localParameters()[Trk::locX]
                                            << " ROT surf: "<< &(compROT.rioOnTrack(i).associatedSurface()));
        if (compROT.assignmentProbability(i) > compROT.assignmentProbability(compROT.indexOfMaxAssignProb()) ) {
            ATH_MSG_ERROR(" assignment probability larger than maximum assignment probability!!!");
        }
    }


    ATH_MSG_VERBOSE("  - eff. globalPosition(): ");
    //ATH_MSG_VERBOSE("       distance of effective measurement from origin: " <<  compROT.globalPosition().distance());
    ATH_MSG_VERBOSE( " ("<< compROT.globalPosition().x() << ", "<< compROT.globalPosition().y() << ", "<< compROT.globalPosition().z() << ")" );
    /*    ATH_MSG_VERBOSE("  - clone()");
        CompetingPixelClustersOnTrack* newcompROT = compROT.clone();
        if (!newcompROT){
            ATH_MSG_ERROR("  Got no clone!");
        } else {
            ATH_MSG_VERBOSE("   Got the clone: do the same tests with it:");
            
            ATH_MSG_VERBOSE("  clone- associatedSurface()");
            const Trk::Surface* cloneassocsurface = &(newcompROT->associatedSurface());
            ATH_MSG_VERBOSE("       distance of surface center from origin: " << cloneassocsurface->center().distance());
            
            ATH_MSG_VERBOSE("  clone- containedROTs()");
            ATH_MSG_VERBOSE("       size of ROT vector: " <<  newcompROT->containedROTs().size());
            ATH_MSG_VERBOSE("  clone- numberOfContainedROTs(): " << newcompROT->numberOfContainedROTs());
            
            ATH_MSG_VERBOSE("  clone- localParameters()");
            ATH_MSG_VERBOSE("       dimension of the effective local params: " <<  newcompROT->localParameters().dimension());
        
            ATH_MSG_VERBOSE("  clone- localErrorMatrix()");
            ATH_MSG_VERBOSE("       dimension of covariance matrix: " <<  newcompROT->localErrorMatrix().covariance().num_row());
            ATH_MSG_VERBOSE("       error of first local coordinate:  " <<  newcompROT->localErrorMatrix().error(Trk::loc1));
            ATH_MSG_VERBOSE("       weight value of first local coordinate: " <<  newcompROT->localErrorMatrix().weightValue(Trk::loc1));
            
            ATH_MSG_VERBOSE("  clone- assignmentProbability(index) and rioOnTrack(index)");
            for (unsigned int i=0; i<newcompROT->numberOfContainedROTs(); i++) {
                ATH_MSG_VERBOSE("      ["<< i <<"] assgnProb: "<< newcompROT->assignmentProbability(i)<< "ROT identifier:"<< newcompROT->rioOnTrack(i)->identify().getString() );
            }
            
            ATH_MSG_VERBOSE("  clone- globalPosition()");
            ATH_MSG_VERBOSE("       distance of effective measurement from origin: " <<  newcompROT->globalPosition().distance());    
        }
        delete newcompROT;*/
    
}

StatusCode InDet::CompetingPixelClustersOnTrackTool::updateCompetingROTprobs( 
        const Trk::CompetingRIOsOnTrack& baseCompROT, 
        const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >& assignmentProbs 
    ) const { 
    const InDet::CompetingPixelClustersOnTrack* compROT = dynamic_cast< const InDet::CompetingPixelClustersOnTrack* >(&baseCompROT); 
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
    delete compROT->m_globalPosition; 
    compROT->m_globalPosition = 0; // very important, otherwise segfault... 
    // recalc localParameters 
    const_cast<InDet::CompetingPixelClustersOnTrack*>(compROT)->setLocalParametersAndErrorMatrix();

    if (msgLvl(MSG::DEBUG)) testCompetingROT(*compROT); 
    return StatusCode::SUCCESS; 
} 
