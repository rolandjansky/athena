/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class CompetingTRT_DriftCirclesOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool for CompetingTRT_DriftCirclesOnTrack creation
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

// Header
#include "InDetCompetingRIOsOnTrackTool/CompetingTRT_DriftCirclesOnTrackTool.h"

//#include "GaudiKernel/MsgStream.h"

// data model
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrack.h"

// interfaces
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkToolInterfaces/IWeightCalculator.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::CompetingTRT_DriftCirclesOnTrackTool::CompetingTRT_DriftCirclesOnTrackTool(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa ) :
        AthAlgTool(ty,na,pa),
        m_TRT_ROTCreator("InDet::TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackTool"),
        m_weightCalculator("Trk::DAF_SimpleWeightCalculator/DAF_WeightCalculator"),
        m_extrapolator("Trk::Extrapolator/AtlasExtrapolator")
{
    declareInterface<ICompetingTRT_DriftCirclesOnTrackCreator>(this);
    declareInterface<ICompetingRIOsOnTrackTool>(this);
    declareProperty("ToolForTRT_DriftCircleOnTrackCreation",m_TRT_ROTCreator,           "TRT_DriftCircleOnTrackCreator needed for the creation of CompetingPixelClustersOnTrack");
    declareProperty("ToolForWeightCalculation",             m_weightCalculator,         "Tool for weight (assignment probability) calculation");
    declareProperty("WeightCutValueBarrel",                 m_jo_BarrelCutValue=6.66,    "lambda parameter (intrinsic roadwidth) for measurements in the Barrel part");
    declareProperty("WeightCutValueEndCap",                 m_jo_EndCapCutValue=6.66,    "lambda parameter (intrinsic roadwidth) for measurements in the EndCap part");
    declareProperty("Extrapolator",                         m_extrapolator,             "Extrapolator tool");
}

///////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////

InDet::CompetingTRT_DriftCirclesOnTrackTool::~CompetingTRT_DriftCirclesOnTrackTool() {}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::CompetingTRT_DriftCirclesOnTrackTool::initialize() {
    StatusCode sc = AlgTool::initialize();

    //ATH_MSG_DEBUG("ToolForTRT_DriftCircleOnTrackCreation: " << m_jo_ToolForROTCreation);
    //ATH_MSG_DEBUG("ToolForWeightCalculation: " << m_jo_ToolForWeightCalc);
    ATH_MSG_DEBUG("WeightCutValueBarrel: " << m_jo_BarrelCutValue);
    ATH_MSG_DEBUG("WeightCutValueEndCap: " << m_jo_EndCapCutValue);

    // Get the correction tool to create TRT_DriftCircles on Track
    sc = m_TRT_ROTCreator.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve the TRT ROT creator: "<< m_TRT_ROTCreator );
        return sc;
    }

    // Get the weightCalculator tool to calc the assignment probabilities
    sc = m_weightCalculator.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve Tool for weight (assignment probability) calculation: "<< m_weightCalculator );
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

StatusCode InDet::CompetingTRT_DriftCirclesOnTrackTool::finalize() {
    StatusCode sc = AlgTool::finalize();
    return sc;
}

///////////////////////////////////////////////////////////////////
// InDet::CompetingTRT_DriftCirclesOnTrack production
///////////////////////////////////////////////////////////////////
const InDet::CompetingTRT_DriftCirclesOnTrack* InDet::CompetingTRT_DriftCirclesOnTrackTool::createCompetingROT(
    const std::list< const Trk::PrepRawData* >& RIO_List,
    const Trk::TrackParameters& trkPar,
    const Trk::IWeightCalculator::AnnealingFactor beta ) const {

    ATH_MSG_DEBUG("********* in createCompetingROT() ********** ");
    // get trkPar error in localZ for locZ cut
    double trkParErrorLocZ = 2.;
    if (trkPar.covariance()) {
      trkParErrorLocZ = sqrt((*trkPar.covariance())(Trk::locZ,Trk::locZ));
    }
    // vector of ROTs
    std::vector< const InDet::TRT_DriftCircleOnTrack* >* ROTvector = new std::vector<const InDet::TRT_DriftCircleOnTrack*>;
    // vector of assignmentProbs
    std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb > *assgnProbVector = new std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >;
    // type of TRT_BaseElement to check if all RIOs are of same type
    InDetDD::TRT_BaseElement::Type TRTtype = InDetDD::TRT_BaseElement::BARREL;
    bool TRTtypeSet = false;
    // maxium assignment propability for choosing the surface....
    Trk::CompetingRIOsOnTrack::AssignmentProb maximumAssignProb = 0;
    const InDet::TRT_DriftCircleOnTrack* ROTwithMaximumAssgnProb = 0;

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
        const InDet::TRT_DriftCircle* riopointer = dynamic_cast<const InDet::TRT_DriftCircle*>(*rioIter);
        if (!riopointer) {
            //ATH_MSG_WARNING("That's mean: given list of PrepRawData* contains a NULL pointer resp. not a TRT_DriftCircle!");
            ATH_MSG_WARNING("Given list of PrepRawData* contains a non TRT_DriftCircle!");
            ATH_MSG_WARNING("Entry will therefore be ignored!");
        } else {
            ATH_MSG_VERBOSE("Got next RIO in list");
            // ---------------------------------
            // check if all RIOs are of the same detector type:
            if (!TRTtypeSet) {
                TRTtype = riopointer->detectorElement()->type();
                TRTtypeSet = true;
            }
            if (riopointer->detectorElement()->type() != TRTtype) {
                ATH_MSG_WARNING("Given TRT PrepRawData do not have all the same detector type (Barrel resp. End-Cap):");
                ATH_MSG_WARNING("Current entry of PrepRawData list will be ignored!!!");
                // Ignore current RIO and jump do next RIO
                continue;
            }
            // ---------------------------------
            // create two ROTs from PrepRawData (left and right solution):

            // get RIO surface
            const Trk::Surface* RIOsurface = &(riopointer->detectorElement()->surface(riopointer->identify()));
            const Trk::StraightLineSurface* RIOsurfacePointer = dynamic_cast< const Trk::StraightLineSurface* > (RIOsurface);

            if (!RIOsurfacePointer) {
                ATH_MSG_WARNING("RIO has no StraightLineSurface: TRT_DriftCircleOnTrack can not be created");
            } else {
                //ATH_MSG_VERBOSE("Got StraightLineSurface from RIO");
                ATH_MSG_VERBOSE("RIOsurfacePointer: " << RIOsurfacePointer << " at ("
                                            << RIOsurfacePointer->center().x() << ", "
                                            << RIOsurfacePointer->center().y() << ", "
                                            << RIOsurfacePointer->center().z() << ")");
                // -----------------
                // create first ROT:
                // check if track parameters are expressed on the RIO surface
                const Trk::TrackParameters* trkParAtRIOsurface = 0;
                const Trk::TrackParameters* newTrackParameters = 0;
                if ( (*RIOsurface)==((trkPar.associatedSurface())) ) {
                    // track parameters are given on the surface of the RIO, use them
                    ATH_MSG_VERBOSE("TrackParameters are on RIO surface: GP ("
                                            << trkPar.position().x() << ", "
                                            << trkPar.position().y() << ", "
                                            << trkPar.position().z() << ") GM ("
                                            << trkPar.momentum().x() << ", "
                                            << trkPar.momentum().y() << ", "
                                            << trkPar.momentum().z() << ") LP ("
                                            << trkPar.localPosition()[Trk::driftRadius]  << ", "
                                            << trkPar.localPosition()[Trk::locZ]  << ")");
                    trkParAtRIOsurface = &trkPar;
                } else {
                    // first create trkParameter on the Surface of the RIO
                    // extrapolate to RIO surface
                    ATH_MSG_VERBOSE("Try to propagate TrackParameters to RIO surface");
                    newTrackParameters = m_extrapolator->extrapolateDirectly((trkParWithoutError ? *trkParWithoutError : trkPar), *RIOsurfacePointer,
                                                                           Trk::anyDirection, // propagate in any direction
                                                                           //Trk::alongMomentum, // propagate in any direction
                                                                           false, //do noBoundaryCheck!
                                                                           Trk::noHypothesis); // without material interaction
                    if (!newTrackParameters){
                        ATH_MSG_ERROR("TrackParameters could not be propagated to PrepRawData surface");
                        delete ROTvector;
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
                                            << trkParAtRIOsurface->localPosition()[Trk::driftRadius]  << ", "
                                            << trkParAtRIOsurface->localPosition()[Trk::locZ]  << ")");
                } // end if (track parameters on RIO surface)
                // test if track prediction is out of local z range of the drift tube
                bool outOfZbounds = false;
                const Trk::CylinderBounds& cylBounds = dynamic_cast<const Trk::CylinderBounds&>(RIOsurfacePointer->bounds());
                ATH_MSG_VERBOSE( "TRT surface bounds (r, Z): (" << cylBounds.r() << ", " << cylBounds.halflengthZ() << ")" );
                if ( fabs(trkParAtRIOsurface->localPosition()[Trk::locZ]) > cylBounds.halflengthZ() ) {
                    ATH_MSG_VERBOSE( "out of locZ bounds, trkParErrorLocZ="<<trkParErrorLocZ );
                    if ( fabs(trkParAtRIOsurface->localPosition()[Trk::locZ]) > cylBounds.halflengthZ() + 4.*trkParErrorLocZ ) {
                        ATH_MSG_VERBOSE( "set assign prob to 0." );
                        outOfZbounds = true;
                    }
                }
                // now create ROT
                const InDet::TRT_DriftCircleOnTrack* rot1 = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>( m_TRT_ROTCreator->correct(*riopointer, *trkParAtRIOsurface));
                if (!rot1) {
                    ATH_MSG_WARNING("first TRT_DriftCircleOnTrack could not be created");
                } else {
                    ATH_MSG_VERBOSE("Created first ROT");
                    // add ROT to vector of ROTs
                    ROTvector->push_back(rot1);
                    // call weightcalculator and calc assignment probabilty:
                    ATH_MSG_VERBOSE("Call weight calculator for non-normalized assignment probability");
                    Trk::CompetingRIOsOnTrack::AssignmentProb assgnProb1 = m_weightCalculator->calculateWeight(*trkParAtRIOsurface, *rot1, beta);
                    ATH_MSG_VERBOSE("Current ROT has assignment probability: " << assgnProb1 );
                    if (outOfZbounds) assgnProb1 = 0.;
                    assgnProbVector->push_back( assgnProb1 );
                    if (assgnProb1 >= maximumAssignProb) {
                        ATH_MSG_VERBOSE("Current ROT has maximum assignment probabilty for now");
                        maximumAssignProb = assgnProb1;
                        ROTwithMaximumAssgnProb = rot1;
                    }
                }// end else (!rot1)

                // ------------------
                // create second ROT:
                // first create mirrored trkParameter on the Surface of the RIO
		Amg::VectorX par = trkParAtRIOsurface->parameters();
                par[Trk::locR] = (-1.0) * par[Trk::locR];
		AmgSymMatrix(5)* covN = trkParAtRIOsurface->covariance() ? new AmgSymMatrix(5)(*trkParAtRIOsurface->covariance()) : 0;
		const Trk::TrackParameters* mirrorTrkPar = trkParAtRIOsurface->associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],
																    par[Trk::phi],par[Trk::theta],
																    par[Trk::qOverP],covN);
                // now create ROT
                const InDet::TRT_DriftCircleOnTrack* rot2 = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(m_TRT_ROTCreator->correct(*riopointer, *mirrorTrkPar));
                if (!rot2) {
                    ATH_MSG_WARNING("second TRT_DriftCircleOnTrack could not be created");
                } else {
                    ATH_MSG_VERBOSE("Created second ROT");
                    // add ROT to vector of ROTs
                    ROTvector->push_back(rot2);
                    // call weightcalculator and calc assignment probabilty:
                    // using trkParAtRIOsurface is not a bug (the other is mirrored parameter)!!!
                    Trk::CompetingRIOsOnTrack::AssignmentProb assgnProb2 = m_weightCalculator->calculateWeight(*trkParAtRIOsurface, *rot2, beta);
                    ATH_MSG_VERBOSE("Current ROT has assignment probability: " << assgnProb2 );
                    if (outOfZbounds) assgnProb2 = 0.;
                    assgnProbVector->push_back( assgnProb2 );
                    if (assgnProb2 >= maximumAssignProb) {
                        ATH_MSG_VERBOSE("Current ROT has maximum assignment probabilty for now");
                        maximumAssignProb = assgnProb2;
                        ROTwithMaximumAssgnProb = rot2;
                    }
                }// end else (!rot2)

                delete newTrackParameters;
                delete mirrorTrkPar;
            }// end else (!RIOsurfacePointer)
        } // end else (!riopointer)
    } // end for loop

    delete trkParWithoutError;
    trkParWithoutError = 0;

    // -------------------------------------
    // test if at least one ROT was created:
    if (ROTvector->size() <= 0) {
        ATH_MSG_ERROR("No valid TRT_DriftCircleOnTrack could be created:");
        ATH_MSG_ERROR("CompetingTRT_DriftCirclesOnTrack creation aborted!");
        //clean-up
        delete ROTvector;
        delete assgnProbVector;
        return 0;
    }
    ATH_MSG_DEBUG("List of competing TRT ROTs contains "<< ROTvector->size() << " TRT_DriftCirclesOnTrack");

    // -----------------------------------
    // normalize assignment probabilities:
    // copy ROTvector to base class vector (vector of RIO_OnTrack) because vector<> does not know inheritance structure
    std::vector< const Trk::RIO_OnTrack* >* baseROTvector = new std::vector<const Trk::RIO_OnTrack*>;
    std::vector< const InDet::TRT_DriftCircleOnTrack*>::const_iterator rotIter = ROTvector->begin();
    for (; rotIter!=ROTvector->end(); ++rotIter) {
        baseROTvector->push_back(*rotIter);
    }
    // call normalize()
    if(TRTtype == InDetDD::TRT_BaseElement::BARREL) {
        ATH_MSG_DEBUG("Call weight calculator for normalization now (Barrel cut)");
        m_weightCalculator->normalize(*assgnProbVector, baseROTvector, beta, m_jo_BarrelCutValue);
    } else {
        ATH_MSG_DEBUG("Call weight calculator for normalization now (end-cap cut)");
        m_weightCalculator->normalize(*assgnProbVector, baseROTvector, beta, m_jo_EndCapCutValue);
    }
    delete baseROTvector;
    baseROTvector=0;

    if (!ROTwithMaximumAssgnProb) {
        ATH_MSG_ERROR("No RIO_OnTrack with maximum assignment probability!");
        //clean-up
        delete ROTvector;
        delete assgnProbVector;
        return 0;
    }
    // ---------------------------------
    // check if ROTs have common surface
    ATH_MSG_VERBOSE("Check if ROTs have a common surface");
    bool nonVanishingROTsHaveCommonSurface=true;
    bool allROTsHaveCommonSurface=true;
    for (unsigned int i=0; i<ROTvector->size(); i++) {
        if (ROTvector->operator[](i)->associatedSurface() != ROTwithMaximumAssgnProb->associatedSurface()) {
            allROTsHaveCommonSurface = false;
            if (TRTtype == InDetDD::TRT_BaseElement::BARREL) {
                if ( (assgnProbVector->operator[](i)) > 0. ) {
                    nonVanishingROTsHaveCommonSurface=false;
                }
            } else {
                // in the endcaps we ignore negligible assignment probs to use a single tube as soon as possible
                // (avoids the ugly transform to polar-coordinates)
                if ( (assgnProbVector->operator[](i)) > 1.e-5 ) {
                    nonVanishingROTsHaveCommonSurface=false;
                } else {
                    // assgn prob is negligble, set it to zero:
                    (assgnProbVector->operator[](i)) = 0.;
                    ATH_MSG_VERBOSE("In endcap: assgn prob on second best straw is negligble, set to "<<(assgnProbVector->operator[](i)));
                }
            }
        }
    }
    // values of int ROTsHaveCommonSurface:
    //  no  yes  NA    (ROTs on a common surface)  NA means not checked yet
    //  0    1   2
    //     for all    | just ROTs with non-vanishing assgnProb
    //      * 1       |   * 3
    int ROTsHaveCommonSurface = 0;
    if (allROTsHaveCommonSurface) {
        ROTsHaveCommonSurface = 1;
        ATH_MSG_DEBUG("all ROTs have a common surface");
    }
    if (nonVanishingROTsHaveCommonSurface) {
        ROTsHaveCommonSurface += 3;
        ATH_MSG_DEBUG("and non-vanishing ROTs have a common surface");
    }
    // ----------------------------------
    // calculate effective measurement:
    // TRT barrel and TRT end-cap differ
    const Trk::LocalParameters* effectiveLocalPar = 0;
    const Amg::MatrixX* effectiveErrMat = 0;
    const Trk::Surface* assocSurface = 0;
    if(TRTtype == InDetDD::TRT_BaseElement::BARREL) {
        //calculation of effective measurements for Barrel-TRT
        // surface of the trkPar does not matter for standard calc
        assocSurface = &(ROTwithMaximumAssgnProb->associatedSurface());
        ATH_MSG_DEBUG("CompetingROT is in Barrel part");
        calcEffectiveMeasurement(effectiveLocalPar, effectiveErrMat, ROTvector, assgnProbVector, &trkPar, assocSurface, nonVanishingROTsHaveCommonSurface);

    } else {
        //calculation of effective measurements for EndCap-TRT
        ATH_MSG_DEBUG("CompetingROT is in end-cap part");
        if (nonVanishingROTsHaveCommonSurface) {
            // use standard calculation if just one straw is in the competition
            // surface of the trkPar does not matter for standard calc
            ATH_MSG_DEBUG("    but all non-vanishing ROTs have the same surface: use standard calc");
            assocSurface = &(ROTwithMaximumAssgnProb->associatedSurface());
            calcEffectiveMeasurement(effectiveLocalPar, effectiveErrMat, ROTvector, assgnProbVector, &trkPar, assocSurface, nonVanishingROTsHaveCommonSurface);
        } else {
            //            const Trk::DiscSurface* surf = dynamic_cast<Trk::DiscSurface*>(&(ROTwithMaximumAssgnProb->detectorElement()->surface()));
            //            if (!surf) {
            //                ATH_MSG_ERROR("Could not get right surface for endcap TRT:");
            //                ATH_MSG_ERROR("CompetingTRT_DriftCirclesOnTrack creation aborted!");
            //                // FIXME: delete everything
            //                return 0;
            //            }
            //            assocSurface = surf;
            //            const Trk::AtaDisc discpar* = new AtaDisc(trkPar.position(), trkPar.momentum(), trkPar.charge(), *surf);
            //            if (!discpar) {
            //                ATH_MSG_ERROR("Could not get TrackParameters on DiscSurface:");
            //                ATH_MSG_ERROR("CompetingTRT_DriftCirclesOnTrack creation aborted!");
            //                // FIXME: delete everything
            //                return 0;
            //            }
            //trkPar must be on common surface (localR needed in calculation)
            //            double localR_Track = discpar->parameters()[Trk::locR];

            // use DiscSurface as the common one
            assocSurface = &(ROTwithMaximumAssgnProb->detectorElement()->surface());
            // get LocR of trkPar on this surface
            // this is not the optimal way, but should be sufficient if trkPar is given
            // on one of the straws.
            // The LocR of trkPar has not such a great influence and the last iterations
            // where just one straw remains in competition are done on this StraightLineSurface anyway
            // TODO: check how this can be done in a better way
            const Amg::Vector2D* localTrkPar = assocSurface->globalToLocal(trkPar.position(), 10.); // use rather huge tolerance because z-coord does not matter
            if (!localTrkPar) {
                ATH_MSG_ERROR("Could not get TrackParameters on DiscSurface:");
                ATH_MSG_ERROR("CompetingTRT_DriftCirclesOnTrack creation aborted!");
                //clean-up
                delete ROTvector;
                delete assgnProbVector;
                return 0;
            }
            ATH_MSG_DEBUG("estimated TrackParametres on DiscSurface: ("<<(*localTrkPar)[Trk::locR]<<","<<(*localTrkPar)[Trk::locPhi]<<")");

            //            delete discpar;
            calcEffectiveEndCapMeasurement( effectiveLocalPar, effectiveErrMat, ROTvector, assgnProbVector, *localTrkPar, assocSurface);
            delete localTrkPar;
        }
    }

    if (!effectiveLocalPar) {
        ATH_MSG_ERROR("Could not produce effective localParameters");
        //clean-up
        delete effectiveErrMat;
        delete ROTvector;
        delete assgnProbVector;
        return 0;
    }
    if (!effectiveErrMat) {
        ATH_MSG_ERROR("Could not produce effective ErrorMatrix");
        //clean-up
        
        delete effectiveLocalPar;
        delete ROTvector;
        delete assgnProbVector;
        return 0;
    }

    // ---------------------------------------
    // create CompetingTRT_DriftCirclesOnTrack
    //return (new CompetingTRT_DriftCirclesOnTrack(assocSurface, ROTvector, assgnProbVector, effectiveLocalPar, effectiveErrMat, ROTsHaveCommonSurface));
    CompetingTRT_DriftCirclesOnTrack* theCompetingROT = new CompetingTRT_DriftCirclesOnTrack(assocSurface, ROTvector, assgnProbVector, effectiveLocalPar, effectiveErrMat, ROTsHaveCommonSurface);
    if (msgLvl(MSG::VERBOSE)) testCompetingROT(*theCompetingROT);
    return theCompetingROT;
}


///////////////////////////////////////////////////////////////////
// InDet::CompetingTRT_DriftCirclesOnTrack update
///////////////////////////////////////////////////////////////////
void InDet::CompetingTRT_DriftCirclesOnTrackTool::updateCompetingROT(
    //const InDet::CompetingTRT_DriftCirclesOnTrack& compROT,
    Trk::CompetingRIOsOnTrack& baseCompROT,
    const Trk::TrackParameters& trkPar,
    const Trk::IWeightCalculator::AnnealingFactor beta
    //const bool recreateROTs=false
) const {

    //   TODO:  if recreateROTs==true call standard createCompROT method
    
    ATH_MSG_DEBUG("********* in updateCompetingROT() **********");
    // get trkPar error in localZ for locZ cut
    double trkParErrorLocZ = 2.;
    if (trkPar.covariance()) {
      trkParErrorLocZ = sqrt( (*trkPar.covariance())(Trk::locZ,Trk::locZ));
    }
    
    // cast baseCompROT to CompTRT_DConTrack:

    InDet::CompetingTRT_DriftCirclesOnTrack* compROT = dynamic_cast< InDet::CompetingTRT_DriftCirclesOnTrack* >(&baseCompROT);
    if (!compROT) {
        ATH_MSG_ERROR("Given CompetingRIOsOnTrack is not a CompetingTRT_DriftCirclesOnTrack!");
        ATH_MSG_ERROR("Update of assignment probabilities aborted!!!");
        return;
    }
    std::vector< InDet::CompetingTRT_DriftCirclesOnTrack::AssignmentProb >* assgnProbVector
      = new std::vector< InDet::CompetingTRT_DriftCirclesOnTrack::AssignmentProb >;
    // get maxium assignment propability for choosing the surface....
    Trk::CompetingRIOsOnTrack::AssignmentProb maximumAssignProb = 0;
    unsigned int maximumAssignProbIndex = 0;
    const InDet::TRT_DriftCircleOnTrack* ROTwithMaximumAssgnProb = 0;

    // clone TrkParameters without error to force the extrapolator to do propagation without error matrix
    const Trk::TrackParameters* trkParWithoutError = trkPar.clone();

    // ----------------------
    // loop over all ROTs in the CompetingROT
    ATH_MSG_VERBOSE("trackSurfacePointer: " << trkPar.associatedSurface() << " at ("
                                << trkPar.associatedSurface().center().x() << ", "
                                << trkPar.associatedSurface().center().y() << ", "
                                << trkPar.associatedSurface().center().z() << ")");
    ATH_MSG_VERBOSE("loop over ROTs:");
    const Trk::TrackParameters* newTrackParameters = 0;
    for (unsigned int i=0; i<compROT->numberOfContainedROTs(); i++) {
        const Trk::StraightLineSurface* ROTsurfacePointer = dynamic_cast< const Trk::StraightLineSurface* > (&(compROT->rioOnTrack(i).associatedSurface()));
        if (!ROTsurfacePointer) throw std::logic_error("Unhandled surface.");
        ATH_MSG_VERBOSE("ROTsurfacePointer: " << ROTsurfacePointer << " at ("
                                    << ROTsurfacePointer->center().x() << ", "
                                    << ROTsurfacePointer->center().y() << ", "
                                    << ROTsurfacePointer->center().z() << ")");
        // check if track parameters are expressed on the RIO surface
        const Trk::TrackParameters* trkParAtROTsurface = 0;
        if ( (*ROTsurfacePointer)==((trkPar.associatedSurface())) ) {
            // track parameters are given on the surface of the ROT, use them
            ATH_MSG_VERBOSE("TrackParameters are on ROT surface: GP ("
                                    << trkPar.position().x() << ", "
                                    << trkPar.position().y() << ", "
                                    << trkPar.position().z() << ") GM ("
                                    << trkPar.momentum().x() << ", "
                                    << trkPar.momentum().y() << ", "
                                    << trkPar.momentum().z() << ") LP ("
                                    << trkPar.localPosition()[Trk::driftRadius]  << ", "
                                    << trkPar.localPosition()[Trk::locZ]  << ")");
            trkParAtROTsurface = &trkPar;
        } else {
            // check if we already created track parameters on the surface of the ROT
            if (newTrackParameters) {
	      if ( (*ROTsurfacePointer)==((newTrackParameters->associatedSurface())) ) {
                    // track parameters are given on the surface of the ROT, use them
                    trkParAtROTsurface = newTrackParameters;
                } // end if already created trackParameters on the ROT surface
            } // end if (newTrackParameters)
        } // end if (TrackParameters on ROT surface exist)
        if (!trkParAtROTsurface) {
            // first create trkParameter on the Surface of the ROT
            // extrapolate to ROT surface
            ATH_MSG_VERBOSE("Try to propagate TrackParameters to ROT surface");
            ATH_MSG_VERBOSE("Try to propagate TrackParameters to RIO surface");
            delete newTrackParameters;
            newTrackParameters = m_extrapolator->extrapolateDirectly((trkParWithoutError ? *trkParWithoutError : trkPar), *ROTsurfacePointer,
                                                                    Trk::anyDirection, // propagate in any direction
                                                                    //Trk::alongMomentum, // propagate in any direction
                                                                    false, //do noBoundaryCheck!
                                                                    Trk::noHypothesis); // without material interaction
            if (!newTrackParameters){
                ATH_MSG_ERROR("TrackParameters could not be propagated to RIO_OnTrack surface");
                delete assgnProbVector;
                return;
            } // end if (extrapolation failed)
            // const Trk::AtaStraightLine* trkParAtRIOsurface1 = new Trk::AtaStraightLine(trkPar.position(), trkPar.momentum(), trkPar.charge(), *RIOsurfacePointer);
            trkParAtROTsurface = newTrackParameters;
            ATH_MSG_VERBOSE("propagated TrackParameters on ROT surface: GP ("
                                    << trkParAtROTsurface->position().x() << ", "
                                    << trkParAtROTsurface->position().y() << ", "
                                    << trkParAtROTsurface->position().z() << ") GM ("
                                    << trkParAtROTsurface->momentum().x() << ", "
                                    << trkParAtROTsurface->momentum().y() << ", "
                                    << trkParAtROTsurface->momentum().z() << ") LP ("
                                    << trkParAtROTsurface->localPosition()[Trk::driftRadius]  << ", "
                                    << trkParAtROTsurface->localPosition()[Trk::locZ]  << ")");
        } // end if (!trkParAtROTsurface)

        // call weightcalculator and calc assignment probabilty:
        Trk::CompetingRIOsOnTrack::AssignmentProb assgnProb = m_weightCalculator->calculateWeight(*trkParAtROTsurface, (compROT->rioOnTrack(i)), beta);
        ATH_MSG_VERBOSE("non-normalized weight for ROT "<< i << ": " << assgnProb );
        
        // test if track prediction is out of local z range of the drift tube
        const Trk::CylinderBounds& cylBounds = dynamic_cast<const Trk::CylinderBounds&>(ROTsurfacePointer->bounds());
        ATH_MSG_VERBOSE( "TRT surface bounds (r, Z): (" << cylBounds.r() << ", " << cylBounds.halflengthZ() << ")" );
        if ( fabs(trkParAtROTsurface->localPosition()[Trk::locZ]) > cylBounds.halflengthZ() ) {
            ATH_MSG_VERBOSE( "out of locZ bounds, trkParErrorLocZ="<<trkParErrorLocZ );
            if ( fabs(trkParAtROTsurface->localPosition()[Trk::locZ]) > cylBounds.halflengthZ() + 4.*trkParErrorLocZ ) {
                ATH_MSG_VERBOSE( "set assign prob to 0." );
                assgnProb = 0.;
            }
        }

        
        assgnProbVector->push_back( assgnProb );
        if (assgnProb >= maximumAssignProb) {
            ATH_MSG_VERBOSE("current ROT has maximum assignment probability up to now" );
            maximumAssignProb = assgnProb;
            maximumAssignProbIndex = i;
            ROTwithMaximumAssgnProb = &compROT->rioOnTrack(i);
        }
    } // end for loop
    delete newTrackParameters;
    newTrackParameters = 0;

    delete trkParWithoutError;
    trkParWithoutError = 0;

    if (maximumAssignProb > 0. ) {
        // -----------------------------------
        // normalize assignment probabilities:
        // copy ROTvector to base class vector (vector of RIO_OnTrack) because vector<> does not know inheritance structure
        std::vector< const Trk::RIO_OnTrack* >* baseROTvector = new std::vector<const Trk::RIO_OnTrack*>;
        std::vector< const InDet::TRT_DriftCircleOnTrack* >::const_iterator rotIter = compROT->containedROTs().begin();
        for (; rotIter!=compROT->containedROTs().end(); ++rotIter) {
            baseROTvector->push_back(*rotIter);
        }
        // call normalize()
        ATH_MSG_VERBOSE("normalize the assignment probabilities");
        if(compROT->rioOnTrack(0).detectorElement()->type() == InDetDD::TRT_BaseElement::BARREL) {
            ATH_MSG_DEBUG("Call weight calculator for normalization now (Barrel cut)");
            m_weightCalculator->normalize(*assgnProbVector, baseROTvector, beta, m_jo_BarrelCutValue);
        } else {
            ATH_MSG_DEBUG("Call weight calculator for normalization now (end-cap cut)");
            m_weightCalculator->normalize(*assgnProbVector, baseROTvector, beta, m_jo_EndCapCutValue);
        }
        delete baseROTvector;
    } else {
        ATH_MSG_VERBOSE("all ROTs have probability 0.");
        ROTwithMaximumAssgnProb = &compROT->rioOnTrack(0); // to avoid segfaults when asking for the surface
        maximumAssignProbIndex = 0;
    }
    // ---------------------------------
    // set pointer in compROT to the new assgnProbVector
    delete compROT->m_assignProb;
    compROT->m_assignProb = assgnProbVector;

    bool isBarrel = (compROT->rioOnTrack(0).detectorElement()->type() == InDetDD::TRT_BaseElement::BARREL);
    // ---------------------------------
    // check if ROTs have common surface
    if (!ROTwithMaximumAssgnProb) {
        ATH_MSG_ERROR("No RIO_OnTrack with maximum assignment probability!");
        delete assgnProbVector;
        return;
    }
    
    bool nonVanishingROTsHaveCommonSurface=true;
    bool allROTsHaveCommonSurface=true;
    for (unsigned int i=0; i<compROT->numberOfContainedROTs(); i++) {
        if (compROT->rioOnTrack(i).associatedSurface() != ROTwithMaximumAssgnProb->associatedSurface()) {
            allROTsHaveCommonSurface = false;
            if (isBarrel) {
                if ( (assgnProbVector->operator[](i)) > 0. ) {
                    nonVanishingROTsHaveCommonSurface=false;
                }
            } else {
                // in the endcaps we ignore negligible assignment probs to use a single tube as soon as possible
                // (avoids the ugly transform to polar-coordinates)
                if ( (assgnProbVector->operator[](i)) > 1.e-5 ) {
                    nonVanishingROTsHaveCommonSurface=false;
                } else {
                    // assgn prob is negligble, set it to zero:
                    (assgnProbVector->operator[](i)) = 0.;
                    ATH_MSG_VERBOSE("In endcap: assgn prob on second best straw is negligble, set to "<<compROT->assignmentProbability(i));
                }
            }
        }
    }
    // values of int ROTsHaveCommonSurface:
    //  no  yes  NA    (ROTs on a common surface)  NA means not checked yet
    //  0    1   2
    //     for all    | just ROTs with non-vanishing assgnProb
    //      * 1       |   * 3
    int ROTsHaveCommonSurface = 0;
    if (allROTsHaveCommonSurface) {
        ROTsHaveCommonSurface = 1;
        ATH_MSG_DEBUG("all ROTs have a common surface");
    }
    if (nonVanishingROTsHaveCommonSurface) {
        ROTsHaveCommonSurface += 3;
        ATH_MSG_DEBUG("non-vanishing ROTs have a common surface");
    }

    // update m_ROTsHaveCommonSurface in compROT
    compROT->m_ROTsHaveCommonSurface = ROTsHaveCommonSurface;
    // ----------------------------------
    // calculate effective measurement:
    // TRT barrel and TRT end-cap differ
    const Trk::LocalParameters* effectiveLocalPar = 0;
    const Amg::MatrixX* effectiveErrMat = 0;
    const Trk::Surface* assocSurface = 0;
    if(isBarrel) {
        //calculation of effective measurements for Barrel-TRT
        // surface of the trkPar does not matter for standard calc
        assocSurface = &(ROTwithMaximumAssgnProb->associatedSurface());
        calcEffectiveMeasurement(effectiveLocalPar, effectiveErrMat, &(compROT->containedROTs()), assgnProbVector, &trkPar, assocSurface, nonVanishingROTsHaveCommonSurface);

    } else {
        //calculation of effective measurements for EndCap-TRT
        if (nonVanishingROTsHaveCommonSurface) {
            // use standard calculation if just one straw is in the competition
            // surface of the trkPar does not matter for standard calc
            assocSurface = &(ROTwithMaximumAssgnProb->associatedSurface());
            calcEffectiveMeasurement(effectiveLocalPar, effectiveErrMat, &(compROT->containedROTs()), assgnProbVector, &trkPar, assocSurface, nonVanishingROTsHaveCommonSurface);
        } else {
            // use DiscSurface as the common one
            assocSurface = &(ROTwithMaximumAssgnProb->detectorElement()->surface());
            // get LocR of trkPar on this surface
            // this is not the optimal way, but should be sufficient if trkPar is given
            // on one of the straws.
            // The LocR of trkPar has not such a great influence and the last iterations
            // where just one straw remains in competition are done on this StraightLineSurface anyway
            // TODO: check how this can be done in a better way
            const Amg::Vector2D* localTrkPar = assocSurface->globalToLocal(trkPar.position(), 10.); // use rather huge tolerance because z-coord does not matter
            if (!localTrkPar) {
                ATH_MSG_ERROR("Could not get TrackParameters on DiscSurface:");
                ATH_MSG_ERROR("CompetingTRT_DriftCirclesOnTrack update aborted!");
                // FIXME: delete everything
                return;
            }
            ATH_MSG_DEBUG("estimated TrackParametres on DiscSurface: ("<<(*localTrkPar)[Trk::locR]<<","<<(*localTrkPar)[Trk::locPhi]<<")");
            calcEffectiveEndCapMeasurement( effectiveLocalPar, effectiveErrMat, &(compROT->containedROTs()), assgnProbVector, *localTrkPar, assocSurface);
            delete localTrkPar;
        } // end else (nonVanishingROTsHaveCommonSurface)
    } // end TRT end-cap

    // update compROT:
    // update maximum assign prob index:
    compROT->m_indexMaxAssignProb = maximumAssignProbIndex;
    // update surface
    if (!compROT->m_associatedSurface->associatedDetectorElement())
        delete compROT->m_associatedSurface;
    compROT->m_associatedSurface=assocSurface;
    // delete global position (will be recreated in competingROT itself
    delete compROT->m_globalPosition;
    compROT->m_globalPosition = 0; // very important otherwise segfault in destructor...
    //calling CompetingTRT_DriftCirclesOnTrack::setLocalParametersAndErrorMatrix() does not work here!
    // have to set the effective measurement and error matrix directly:
    //const_cast<InDet::CompetingTRT_DriftCirclesOnTrack*>(compROT)->setLocalParametersAndErrorMatrix();
    const_cast<InDet::CompetingTRT_DriftCirclesOnTrack*>(compROT)->m_localParams = (*effectiveLocalPar);
    const_cast<InDet::CompetingTRT_DriftCirclesOnTrack*>(compROT)->m_localCovariance = (*effectiveErrMat);
    delete effectiveErrMat;
    delete effectiveLocalPar;
    if (msgLvl(MSG::VERBOSE)) testCompetingROT(*compROT);
}


/////////////////////////////////////////////////////////////////////
// create simple competing ROTs (one tube, two left-right ROTs
/////////////////////////////////////////////////////////////////////
const Trk::CompetingRIOsOnTrack* 
InDet::CompetingTRT_DriftCirclesOnTrackTool::createSimpleCompetingROT(
    const Trk::PrepRawData&     rio,
    const Trk::TrackParameters& trkPars,
    const Trk::IWeightCalculator::AnnealingFactor beta,
    const Trk::TrackState::MeasurementType) const {

  // --- ID-or-MS check for tube-detectors
  const Trk::Surface*  PrdSf = &(rio.detectorElement()->surface(rio.identify()));
  const Trk::StraightLineSurface* DriftSurface = 
    dynamic_cast< const Trk::StraightLineSurface* > (PrdSf);
  if (!DriftSurface || PrdSf != &trkPars.associatedSurface()) {
    ATH_MSG_WARNING( "detector hit is not of type drift circle!" );
    return 0;
  }
  if (PrdSf->center().perp()>1500. || PrdSf->center().z() > 3000.) {
    ATH_MSG_WARNING( "input PRD is in the enemy's territory, wrong detector" );
    return 0;
  }

  // --- make first ROT (the L/R solution nearest to prediction) and vectors
  const Trk::RIO_OnTrack* rot1 = m_TRT_ROTCreator->correct(rio, trkPars);
  if (!rot1) {
    ATH_MSG_WARNING( "first and only DriftCircleOnTrack could not be created.");
    return 0;
  }
  std::vector< const Trk::RIO_OnTrack* >* ROTvector = new std::vector<const Trk::RIO_OnTrack*>;
  std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb > *assgnProbVector = 
    new std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >;
  ROTvector->push_back(rot1);
  Trk::CompetingRIOsOnTrack::AssignmentProb assgnProb1 = 
    m_weightCalculator->calculateWeight(trkPars, *rot1, beta);
  assgnProbVector->push_back( assgnProb1 );
  ATH_MSG_VERBOSE("Created first ROT with r="
                          << rot1->localParameters()[Trk::locX] <<" and ass.prob = "
                          << assgnProb1);
  
  // --- create mirrored ROT, but protect against case where DC creators return a wire/tube hit.
  if (fabs(rot1->localParameters()[Trk::locX]) > 0.001) {

    Amg::VectorX par = trkPars.parameters();
    ATH_MSG_VERBOSE("track prediction at " << par[Trk::locR]);
    par[Trk::locR] = (-1.0) * par[Trk::locR];
    AmgSymMatrix(5)* covN = trkPars.covariance() ? new AmgSymMatrix(5)(*trkPars.covariance()) : 0;
    const Trk::TrackParameters* mirrorTrkPar = trkPars.associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],
													    par[Trk::phi],par[Trk::theta],
													    par[Trk::qOverP],covN);
    const Trk::RIO_OnTrack* rot2 = m_TRT_ROTCreator->correct(rio,*mirrorTrkPar);
    if (!rot2) {
      ATH_MSG_DEBUG("2nd DriftCircleOnTrack is not created");
    } else {
      ATH_MSG_VERBOSE("Created second ROT with r=" << rot2->localParameters()[Trk::locX]);
      // add ROT to vector of ROTs
      ROTvector->push_back(rot2);
      // call weightcalculator and calc assignment probabilty:
      Trk::CompetingRIOsOnTrack::AssignmentProb assgnProb2 = 
        m_weightCalculator->calculateWeight(trkPars, *rot2, beta); // must be trkPars, not mirror
      ATH_MSG_VERBOSE("Current ROT has assignment probability: " << assgnProb2 );
      assgnProbVector->push_back( assgnProb2 );
      if (assgnProb2 >= assgnProb1) {
        ATH_MSG_VERBOSE("Current ROT has maximum assignment probabilty for now");
      }
    } // end else (!rot2)
    delete mirrorTrkPar;
  }

  // --- call normalize()
  double thisWeightCut = (fabs(trkPars.position().z())>800.0 ? 
                          m_jo_EndCapCutValue : m_jo_BarrelCutValue );
  m_weightCalculator->normalize(*assgnProbVector, ROTvector, beta, thisWeightCut);

  double meanWeight = assgnProbVector->at(0) /sqrt( ROTvector->at(0)->localCovariance()(Trk::locX,Trk::locX));
  double meanDriftR = meanWeight * ROTvector->at(0)->localParameters()[Trk::locX];
  if (ROTvector->size()==2) {
    double addWeight = assgnProbVector->at(1) / sqrt( ROTvector->at(1)->localCovariance()(Trk::locX,Trk::locX));
    meanDriftR      += addWeight * ROTvector->at(1)->localParameters()[Trk::locX];
    meanWeight      += addWeight;
  }
  if (meanWeight<=1.0e-12) {
    meanWeight = 1.0e-10;
    ATH_MSG_DEBUG("mean weight 0, rescale to: " << meanWeight );
  }
  Amg::MatrixX meanCovMatrix(1,1);
  meanCovMatrix(0,0) = 1./meanWeight;
  meanDriftR = meanCovMatrix(Trk::locX,Trk::locX) * meanDriftR;
  Trk::DefinedParameter radiusPar(meanDriftR,Trk::locX);
  Trk::LocalParameters effectiveLocalPar(radiusPar);
  ATH_MSG_VERBOSE("filling rot with lPar,err = " << effectiveLocalPar <<" and "<< meanCovMatrix);
  
  // --- create final CompetingTRT_DriftCirclesOnTrack
  std::vector< const InDet::TRT_DriftCircleOnTrack* >* DCvector =
    new std::vector<const InDet::TRT_DriftCircleOnTrack*>;
  const InDet::TRT_DriftCircleOnTrack* dc1 = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>
    ( ROTvector->at(0));
  if (!dc1) throw std::logic_error("Not a TRT_DriftCircleOnTrack");
  DCvector->push_back(dc1);
  if (ROTvector->size() > 1) {
    const InDet::TRT_DriftCircleOnTrack* dc2
      = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>( ROTvector->at(1));
    if (!dc2) throw std::logic_error("Not a TRT_DriftCircleOnTrack");
      DCvector->push_back(dc2);
  }
  InDet::CompetingTRT_DriftCirclesOnTrack* theCompetingROT = 
    new InDet::CompetingTRT_DriftCirclesOnTrack
    (PrdSf, DCvector, assgnProbVector, 
     new Trk::LocalParameters(effectiveLocalPar), new Amg::MatrixX(meanCovMatrix), 4 /* 4 = common surface*/ );
  //    testCompetingROT(*theCompetingROT);
  delete ROTvector;
  return theCompetingROT;
}


/////////////////////////////////////////////////////////////////////
// put new (e.g. broad-error) ROT into existing CompROT and run update
/////////////////////////////////////////////////////////////////////
void InDet::CompetingTRT_DriftCirclesOnTrackTool::reequipCompetingROT
(Trk::CompetingRIOsOnTrack*& modifiableCompROT,
 const Trk::RIO_OnTrack*     newROT,
 const Trk::TrackParameters& trkPar,
 const Trk::IWeightCalculator::AnnealingFactor beta,
 const Trk::TrackState::MeasurementType ) const
{
  InDet::CompetingTRT_DriftCirclesOnTrack* ctt
    = dynamic_cast<InDet::CompetingTRT_DriftCirclesOnTrack*>(modifiableCompROT);
  const InDet::TRT_DriftCircleOnTrack* newStraw 
    = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(newROT);
  if (!ctt || !newStraw) {
    ATH_MSG_WARNING( "inconsistent use of reequipCompetingROT() " );
    return;
  }
  std::vector<const InDet::TRT_DriftCircleOnTrack*>::const_iterator
    rotIter = ctt->m_containedChildRots->begin();
  for (; rotIter!=ctt->m_containedChildRots->end(); ++rotIter)
    delete (*rotIter);
  delete ctt->m_containedChildRots;
  ctt->m_containedChildRots = new std::vector<const InDet::TRT_DriftCircleOnTrack*>;
  ctt->m_containedChildRots->push_back(newStraw);

  this->updateCompetingROT(*modifiableCompROT, trkPar, beta);
}

/////////////////////////////////////////////////////////////////////
// INTERNAL: calculate eff. LocPars+Err according to the weights (BR)
/////////////////////////////////////////////////////////////////////
void InDet::CompetingTRT_DriftCirclesOnTrackTool::calcEffectiveMeasurement(
    const Trk::LocalParameters*& effectiveLocalPar,
    const Amg::MatrixX*& effectiveErrMat,
    const std::vector< const InDet::TRT_DriftCircleOnTrack* >* ROTs,
    const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >* assgnProbs,
    const Trk::TrackParameters* trkPar,
    const Trk::Surface* surf,
    bool nonVanishingROTsHaveCommonSurface
) const {

    ATH_MSG_VERBOSE("in calcEffectiveMeasurement():");
    unsigned int rotsnum = ROTs->size();
    double* driftCenterShift = new double[rotsnum];
    for (unsigned int i=0; i<rotsnum; i++)
        driftCenterShift[i] = 0; //TODO: check if this is right
    //
    if (!nonVanishingROTsHaveCommonSurface) {
        ATH_MSG_VERBOSE("Have to calc shift of drift centers:");
        // calculation of effective localParameters for Barrel-TRT:
        // calculate shift of the straw centers relative to the associated surface
        // in the plane normal to the assumedTrackParameters:
        //                 //<Trk::StraightLineSurface>m_associatedSurface;
        // calculate vector to project on first: perpendicular to straw axis and to track momentum
        // normal() is z-axis of the surface rotation, so it should be the z-direction of the StraightLineSurface
        const Amg::Vector3D& strawDirection = surf->normal();
        // or use TRT_BaseElement.strawAxis(int straw) for one straw...
        const Amg::Vector3D& globCenterSurface = surf->center();
        Amg::Vector3D projectionVector = strawDirection.cross(trkPar->momentum());
        projectionVector = projectionVector.unit();
        for (unsigned int i=0; i<rotsnum; i++) {
	  driftCenterShift[i] = projectionVector.dot(ROTs->operator[](i)->associatedSurface().center() - globCenterSurface);
            ATH_MSG_VERBOSE("driftCenterShift[" << i << "] = " << driftCenterShift[i] );
        }
    }
    // now use standard 1-dim calculation with correction of strawcenter shift

    if (ROTs->operator[](0)->localParameters().dimension() == 1) {
        //double meanDriftRadius = assignmentProbability(0) * rioOnTrack(0)->localErrorMatrix().weightValue(Trk::locX) * (rioOnTrack(0)->localParameters()[Trk::locX] + driftCenterShift[0]);

        // Mean weight value
        double meanWeight = 0;
        // mean driftradius
        double meanDriftRadius = 0;
        double currentWeight;
        // loop over ROTs
        ATH_MSG_VERBOSE(" loop over ROTs " );
        for (unsigned int i=0; i<rotsnum; i++) {
	  currentWeight = assgnProbs->operator[](i) /sqrt( ROTs->operator[](i)->localCovariance()(Trk::locX,Trk::locX));
            meanWeight += currentWeight;
            meanDriftRadius += currentWeight * (ROTs->operator[](i)->localParameters()[Trk::locX] + driftCenterShift[i]);
        }
        // create mean ErrorMatrix
        ATH_MSG_DEBUG("mean weight: " << meanWeight );
        if (meanWeight<=1.0e-12) {
            meanWeight = 1e-10; // use very small value, otherwise inversion will fail!
            ATH_MSG_DEBUG("mean weight 0, rescale to: " << meanWeight );
        }
	Amg::MatrixX meanCovMatrix(1, 1);
        meanCovMatrix(0,0) = 1./meanWeight;
        // devide mean DriftRadius by mean Weight
        meanDriftRadius = meanCovMatrix(Trk::locX,Trk::locX) * meanDriftRadius;
        // make new LocalParameters
        ATH_MSG_DEBUG("mean driftradius: " << meanDriftRadius );
        Trk::DefinedParameter radiusPar(meanDriftRadius,Trk::locX);
        effectiveLocalPar = new Trk::LocalParameters(radiusPar);
	effectiveErrMat = new Amg::MatrixX(meanCovMatrix);
    } else {
        // weird dimension for a TRT_DriftCircle
        // FIXME: decide what to do...
        delete[] driftCenterShift;
        ATH_MSG_ERROR("weird dimension of TRT_DriftCircle parameters" );
        return;
    }
    //        }
    delete[] driftCenterShift;
    //    }
    //     return *m_localParameters;
}

/////////////////////////////////////////////////////////////////////
// INTERNAL: calculate eff. LocPars+Err according to the weights (EC)
/////////////////////////////////////////////////////////////////////
void InDet::CompetingTRT_DriftCirclesOnTrackTool::calcEffectiveEndCapMeasurement(
    const Trk::LocalParameters* &effectiveLocalPar,
    const Amg::MatrixX* &effectiveErrMat,
    const std::vector< const InDet::TRT_DriftCircleOnTrack* >* ROTs,
    const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >* assgnProbs,
    const Amg::Vector2D& localTrack,
    const Trk::Surface* surf
    //const InDet::TRT_DriftCircleOnTrack* mostProbableROT,
    //const bool nonVanishingROTsHaveCommonSurface
) const {

    ATH_MSG_VERBOSE("calcEffectiveEndCapMeasurement:");
    unsigned int rotsnum = ROTs->size();
    // calculation of effective localParameters and ErrorMatrix for EndCap-TRT:
    //---------------
    // solution 1 (with measurements on different straws):
    // Use plane of TRT-wheel as measurement plane (DiscSurface).
    // For tracks with small angles between plane and momentum this is not
    // perfect, because the point of closest approach does not lie in
    // the plane.
    // Using DiscSurface as the plane for computing the effective measurement
    // means that the local measurements (DriftRadii) need to be converted to
    // polar coordinates.
    // Just one coordinate (the angle) is used for the effective measurement
    // because all the information about locR comes from the assumedTrackParameters.
    // Even this result depends (slightly) on the assumed locR, therefore
    // the real surface (StraighLineSurface) of the most probable hit should be
    // used in the last iteration steps of annealing (when just the ambiguities
    // of one straw have to solved anymore).
    // TODO: check if origin of m_associatedSurface is intersection point of straw directions
    // alpha: angle between hit and straw
    // tan(alpha) = DriftRadius/(locR of Track)
    //  => sigma_alpha = sigma_DriftRadius/(locR of Track) (for small alpha, should be valid here)
    // TODO: use correct linear error expansion
    // beta: angle between straw direction and coordinate axis of the DiscSurface
    //double localR_Track = trkPar->parameters()[Trk::locR];
    double meanPhi = 0.;
    double meanPhiWeight = 0.;
    double sumAssgnProb = 0.;
    ATH_MSG_VERBOSE(" loop over ROTs " );
    for (unsigned int i=0; i<rotsnum; i++) {
        //double alpha = atan2(ROTs->operator[](i)->localParameters()[Trk::locX], localR_Track);
        //ATH_MSG_DEBUG("weightValue="<<ROTs->operator[](i)->localErrorMatrix().weightValue(Trk::locX)<<" covValue="<<ROTs->operator[](i)->localErrorMatrix().covValue(Trk::locX)<<" sqrt(covValue)="<<sqrt(ROTs->operator[](i)->localErrorMatrix().covValue(Trk::locX))<<" locR="<<ROTs->operator[](i)->localParameters()[Trk::locX]<<" locRtrack="<<localTrack[Trk::locR]);
        double alpha = ((localTrack[Trk::locR]!=0.)?atan(ROTs->operator[](i)->localParameters()[Trk::locX]/localTrack[Trk::locR]):0.);
        //ATH_MSG_DEBUG("alpha["<<i<<"]="<< alpha <<"="<<(alpha/twopi*360.)<<"deg");
        double alphaWeight = localTrack[Trk::locR]*localTrack[Trk::locR] / sqrt( ROTs->operator[](i)->localCovariance()(Trk::locX,Trk::locX));
        //ATH_MSG_DEBUG("alphaWeight["<<i<<"]="<< alphaWeight<<" 1/alphaWeight["<<i<<"]="<< 1./alphaWeight<<" sqrt(1/alphaWeight["<<i<<"])="<< sqrt(1./alphaWeight) );
        // there must be an easier way...
        // perhaps use product of transforms?
        // TODO: check if this REALLY gives the correct angle in all possible cases
	const InDet::TRT_DriftCircleOnTrack& rot = *ROTs->operator[](i);
	const Identifier& id = rot.identify();
	int sdir = rot.detectorElement()->strawDirection();
	Amg::Vector3D dirt = surf->transform().rotation().col(1);
	Amg::Vector3D dirt2 = rot.detectorElement()->transform(id).linear()*Amg::Vector3D(0,0,1) * sdir;

        double beta = 1.; 
	double ptot = dirt.mag()*dirt2.mag();
	if(ptot > 0) {
	  beta = dirt.dot(dirt2)/ptot;
	  if(beta >  1) beta =  1;
	  if(beta < -1) beta = -1;
	  beta = acos(beta);
	}
        beta = (CLHEP::twopi/4.)-beta;
        double phi = -alpha + beta;
        ATH_MSG_DEBUG("beta["<<i<<"]="<< beta <<"="<<(beta/CLHEP::twopi*360.)<<"deg; -alpha+beta="<< phi );
        //meanPhi += assgnProbs->operator[](i) * alphaWeight * (alpha + beta);
        // HACK: correct the sign:
        if ((localTrack[Trk::locPhi]*phi)<.0) phi *= -1.;
        meanPhi += assgnProbs->operator[](i) * phi;
        meanPhiWeight += assgnProbs->operator[](i) * alphaWeight;
        ATH_MSG_DEBUG("assgnProb="<<assgnProbs->operator[](i)<<" meanPhi="<<meanPhi<<" meanPhiWeight="<<meanPhiWeight );
        sumAssgnProb += assgnProbs->operator[](i);
    }
    ATH_MSG_DEBUG("mean weight: " << meanPhiWeight );
    if (meanPhiWeight <= 1.0e-12) {
      meanPhiWeight = 1e-10; // use very small value otherwise inversion will fail!!!
      ATH_MSG_DEBUG("mean weight 0, rescale to: "<<meanPhiWeight);
    }
    Amg::MatrixX meanCovMatrix(1, 1);
    meanCovMatrix(0,0) = 1./meanPhiWeight;
    effectiveErrMat = new Amg::MatrixX(meanCovMatrix);
    // !!!!! do not use effectiveErrMat->covValue(Trk::locPhi) !!!!!
    //                  effectiveErrMat->covValue(0) has to be used
    // ErrorMatrix uses the internal index not the extrenal of LocalParameters!!!!!

    // scale mean value by mean weight
    //meanPhi = effectiveErrMat->covValue(Trk::locPhi) * meanPhi;
    meanPhi /= sumAssgnProb;
    ATH_MSG_DEBUG("mean phi: " << meanPhi );
    Trk::DefinedParameter radiusPar(meanPhi, Trk::locPhi);
    effectiveLocalPar = new Trk::LocalParameters(radiusPar);

    // solution 2 (just with the ambiguity of one straw):
    // Use the surface of the straw as measurement surface (StraightLineSurface).
    // in this case the standard function can be called.

}

void InDet::CompetingTRT_DriftCirclesOnTrackTool::testCompetingROT(const InDet::CompetingTRT_DriftCirclesOnTrack& compROT)const {

    if (!msgLvl(MSG::VERBOSE)) return;

    ATH_MSG_VERBOSE("**Test**");

    msg(MSG::VERBOSE) <<"  - associatedSurface(): ";
    const Trk::Surface* assocsurface = &(compROT.associatedSurface());
    //msg()<<MSG::VERBOSE<<"       distance of surface center from origin: " << assocsurface->center().distance()<<endmsg;
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
        msg()<<" cov[loc1]: " <<  compROT.localCovariance()(Trk::loc1,Trk::loc1)<<endmsg;
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
            msg(MSG::ERROR)<<" assignment probability larger than maximum assignment probability!!!"<<endmsg;
            msg(MSG::VERBOSE)<<endmsg;
        }
    }


    msg()<<"  - eff. globalPosition(): ";
    //msg()<<"       distance of effective measurement from origin: " <<  compROT.globalPosition().distance()<<endmsg;
    msg()<< " ("<< compROT.globalPosition().x() << ", "<< compROT.globalPosition().y() << ", "<< compROT.globalPosition().z() << ")" <<endmsg;
    /*    msg()<<"  - clone()"<<endmsg;
        CompetingTRT_DriftCirclesOnTrack* newcompROT = compROT.clone();
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
