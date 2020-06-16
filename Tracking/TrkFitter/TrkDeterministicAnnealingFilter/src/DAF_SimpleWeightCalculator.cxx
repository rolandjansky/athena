/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DAF_SimpleWeightCalculator.cxx
//   Source file for class DAF_SimpleWeightCalculator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////


// Trk
#include "TrkDeterministicAnnealingFilter/DAF_SimpleWeightCalculator.h"
#include "GaudiKernel/MsgStream.h"

#include "TrkEventPrimitives/LocalParameters.h"

#include "TrkSurfaces/Surface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"


#include "GaudiKernel/PhysicalConstants.h" // for pi
// standard libs
#include <cmath>

// constructor
Trk::DAF_SimpleWeightCalculator::DAF_SimpleWeightCalculator(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
AthAlgTool(t,n,p) {
    declareInterface<IWeightCalculator>(this);
}

// destructor
Trk::DAF_SimpleWeightCalculator::~DAF_SimpleWeightCalculator() {}



// initialize
StatusCode Trk::DAF_SimpleWeightCalculator::initialize() {
    StatusCode s = AthAlgTool::initialize();
    if (s.isFailure()) return s;

    ATH_MSG_INFO( "initialize() successful in " << name() );
    return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::DAF_SimpleWeightCalculator::finalize() {

    ATH_MSG_INFO( "finalize() successful in " << name() );
    return StatusCode::SUCCESS;
}

//calculate the assignment probabilities (assignment weights) for a collection of measurements
const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >* Trk::DAF_SimpleWeightCalculator::calculateWeights (
    const std::vector< const Trk::TrackParameters* >* trkPars,
    const std::vector< const Trk::RIO_OnTrack* >* ROTs,
    const AnnealingFactor beta,
    const double cutValue,
    const bool doNormalization ) const {

    ATH_MSG_DEBUG("--->  calculate weight for a collection of measurements");
    std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >* assgnProbVec = new std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >;

    //check if vectors have the same length
    if ( ROTs->size() != trkPars->size() ) {
        ATH_MSG_ERROR("vector of RIO_OnTrack and TrackParameters do not have the same size: assignmentProbabilities cannot be calculated");
        delete assgnProbVec;
        return nullptr;
    } 
        // -----------------------------
        // loop over ROTs to calc non-normalized assignment probabilities
        ATH_MSG_VERBOSE("loop over ROTs");
        for (unsigned int i=0; i < ROTs->size(); i++) {
            Trk::CompetingRIOsOnTrack::AssignmentProb prob = calculateWeight( *(trkPars->operator[](i)), *(ROTs->operator[](i)), beta );
            ATH_MSG_VERBOSE("assignment probability for ROT "<< i << ": " << prob );
            assgnProbVec->push_back( prob );
        }
        if (doNormalization) {
            // normalize assignment probabilities
            ATH_MSG_VERBOSE("call normalize()");
            normalize( *assgnProbVec, ROTs, beta, cutValue);
        }
        return assgnProbVec;
    
}

//calculate the assignment probability (assignment weight) for a single measurement
//(no normalization can be done in this case)
Trk::CompetingRIOsOnTrack::AssignmentProb Trk::DAF_SimpleWeightCalculator::calculateWeight (
    const Trk::TrackParameters& trkPar,
    const Trk::RIO_OnTrack& ROT,
    const AnnealingFactor beta) const {

    ATH_MSG_DEBUG("---> calculate weight for a single measurement");

    //check whether TrackParameters are given on the right surface
    if ( (ROT.associatedSurface()) !=  (trkPar.associatedSurface()) ) {
        ATH_MSG_ERROR("RIO_OnTrack and TrackParameters do not have the same associatedSurface: assignmentProbability cannot be calculated");
        return 0;
    } 
        ATH_MSG_VERBOSE("start calculation:");
        // code can be written in one line; avoiding this 'cause of readability

        // -----------------------------------------
        // calculate exponential in the multivariate Gaussian:
        // State to measurement dimensional reduction Matrix ( n x m )
        Amg::MatrixX H(ROT.localParameters().expansionMatrix());
        ATH_MSG_VERBOSE("size of reduction matrix: " << H.cols() << "x" << H.rows() );
        ATH_MSG_VERBOSE("dimension of TrackParameters: " << trkPar.parameters().rows() );
        // residual of the measurement r = (m_i - Hx)
	      Amg::VectorX r = ROT.localParameters() - H * trkPar.parameters();
        // (m_i - Hx)^T * V^{-1} * (m_i - Hx) = (m_i - Hx)^T * G * (m_i - Hx)
        Amg::MatrixX weight  = ROT.localCovariance().inverse();
        double exponential =  r.dot(weight*r)/(2.*beta);
        if (msgLvl(MSG::VERBOSE)) {
            trkPar.dump(msg(MSG::VERBOSE));
            msg(MSG::VERBOSE)<<"local parameters of ROT: " << ROT.localParameters()<<endmsg;
            msg(MSG::VERBOSE)<<"locX of parameters: ROT: " << ROT.localParameters()[Trk::locX] << " track: " << trkPar.parameters()[Trk::locX]<< " H*track: " << (H * trkPar.parameters())[Trk::locX] <<endmsg;
            msg(MSG::VERBOSE)<<"norm of residual: " << r.norm() <<endmsg;
            msg(MSG::VERBOSE)<<"ROT weight(locX): " << weight(Trk::locX,Trk::locX) <<endmsg;
            msg(MSG::VERBOSE)<<"exponent of prob: " << exponential <<endmsg;
        }
        return ( std::exp(-exponential) );
    // end if (equal surfaces)
}

//normalize given assignment probabilities (assignment weights) using a given cutValue and annealing factor
const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >* 
Trk::DAF_SimpleWeightCalculator::normalize (
    const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >* assgnProbs,
    const std::vector< const Trk::RIO_OnTrack* >* ROTs,
    const AnnealingFactor beta,
    const double cutValue ) const {

    // copy given assgnProbs to new vector
    ATH_MSG_DEBUG("copy vector<AssignmentProb> to a new one");
    auto newAssgnProbs  = new std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >(*assgnProbs);
    //*newAssgnProbs = *assgnProbs;
    ATH_MSG_DEBUG("call other normalize()");
    Trk::DAF_SimpleWeightCalculator::normalize( *newAssgnProbs, ROTs, beta, cutValue );
    return newAssgnProbs; //original assgnProbs is left unchanged
}



//normalize given assignment probabilities (assignment weights) using a given cutValue and annealing factor
void 
Trk::DAF_SimpleWeightCalculator::normalize (
    std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >& assgnProbs,
    const std::vector< const Trk::RIO_OnTrack* >* ROTs,
    const AnnealingFactor beta,
    const double cutValue ) const {

    if (assgnProbs.size() != ROTs->size()) {
        ATH_MSG_ERROR("sizes of AssignmentProb vector and RIO_OnTrack vector do not match: no normalization done");
        return;
    } 
        ATH_MSG_DEBUG("starting normalization:");
        // ----------------------------
        // calculate sum of assgnProbs:
        double assgnProbSum = 0;
        std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >::const_iterator probIter = assgnProbs.begin();
        for (; probIter!=assgnProbs.end(); ++probIter) {
            assgnProbSum += (*probIter);
        }
        if (assgnProbSum > 0.) {
            // -----------------------------------------
            // calculate the cut value:
            double cutFactor = std::exp(-cutValue/(beta*2.));
            ATH_MSG_VERBOSE("   sum of non-normalized assgn-probs: " << assgnProbSum );
            ATH_MSG_VERBOSE("   cut value: " << cutFactor );
            //----------------------------------------
            // calculate new assignment probabilities:
            double factor = 1./(assgnProbSum + cutFactor);
            //        probIter = assgnProbs.begin();
            //        for (; probIter!=assgnProbs.end(); ++probIter) {
            //            *probIter *= factor;
            //        }
            for (unsigned int i=0; i < assgnProbs.size(); i++) {
                assgnProbs[i] *= factor;
            }
        } // end if (assgnProbSum > 0.)
    // end if(vector sizes match)
}


