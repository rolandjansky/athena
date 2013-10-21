/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DAF_ValidationNtupleHelper.cxx
//   Source file for class DAF_ValidationNtupleHelper
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////


//Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ITHistSvc.h"

#include "GaudiKernel/ISvcLocator.h"

#include "TTree.h"
#include "TBranch.h"
// Trk
#include "TrkValTools/DAF_ValidationNtupleHelper.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

#include "TrkSurfaces/Surface.h"

#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"


// constructor
Trk::DAF_ValidationNtupleHelper::DAF_ValidationNtupleHelper(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
AthAlgTool(t,n,p),
        m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
        m_isUnbiased(0)
 {
    declareInterface<IValidationNtupleHelperTool>(this);
    declareProperty("IgnoreMissingTrackCovarianceForPulls", mjo_ignoreMissTrkCov = false,   "Do not warn, if track states do not have covariance matries when calculating pulls");
    declareProperty("WriteMeasurementPositionOfROTs",       mjo_writeHitPositions = true,   "Write measurement positions?");
    declareProperty("ResidualPullCalculatorTool",           m_residualPullCalculator,       "Tool to calculate residuals and pulls");
}

// destructor
Trk::DAF_ValidationNtupleHelper::~DAF_ValidationNtupleHelper() {}



//////////////
/// initialize
//////////////
StatusCode Trk::DAF_ValidationNtupleHelper::initialize() {

    StatusCode sc;

    // get the ResidualPullCalculator
    sc = m_residualPullCalculator.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL ("Could not retrieve "<< m_residualPullCalculator <<" (to calculate residuals and pulls) " );
        return sc;
    }

    return StatusCode::SUCCESS;
}

///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode Trk::DAF_ValidationNtupleHelper::finalize() {
    ATH_MSG_INFO ( "finalize() successful in " << name() );
    return StatusCode::SUCCESS;
}


///////////////////////////////////////
/// addNtupleItems
///////////////////////////////////////
StatusCode Trk::DAF_ValidationNtupleHelper::addNtupleItems (
    TTree* tree,
    //const std::bitset<8>& detectorType
    const int& ) const {

    // retrieve item (is the used track state unbiased?) from ntuple
    TBranch* trackStatesUnbiasedBranch = tree->GetBranch("TrackStatesUnbiased");
    if (!trackStatesUnbiasedBranch) {
        ATH_MSG_ERROR ("Unable to get Branch TrackStatesUnbiased in ntuple" );
        return StatusCode::FAILURE;
    }
    void* variableAdr = static_cast<void*>(trackStatesUnbiasedBranch->GetAddress());
    if (!variableAdr) {
      ATH_MSG_ERROR ( "Unable to get variable address of Branch TrackStatesUnbiased" );
      return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE ("Address of variable: " << variableAdr);
    m_isUnbiased = static_cast<int*>(variableAdr);

    // add items
    m_nContainedROTs        = new std::vector<int>();
    m_indexOfMaxAssgnProb   = new std::vector<int>();
    m_maxAssgnProb          = new std::vector<float>();
    tree->Branch("CompROTnContainedROTs",       &m_nContainedROTs);         // number of contained ROTs in the CompetingRIOsOnTrack (1-dim array: CompROTnContainedROTs[nHits])
    tree->Branch("CompROTindexOfMaxAssgnProb",  &m_indexOfMaxAssgnProb);    // index of the ROT with the maximum assignment probability
    tree->Branch("CompROTmaxAssgnProb",         &m_maxAssgnProb);           // maximum assignment probability

//     m_AssgnProbs        = new std::vector< <std::vector<float> >();
//     m_compROTResLocX    = new std::vector< <std::vector<float> >();
//     m_compROTResLocY    = new std::vector< <std::vector<float> >();
//     m_compROTPullLocX   = new std::vector< <std::vector<float> >();
//     m_compROTPullLocY   = new std::vector< <std::vector<float> >();
//     m_nt->Branch("CompROTcontainedROTsAssgnProbs",  &m_AssgnProbs);         // assignment probabilities of the contained ROTs (2-dim array: m_AssgnProbs[m_nHits][maxContainedROTs])
//     m_nt->Branch("CompROTcontainedROTsResLocX",     &m_compROTResLocX);     // residual in locX of the contained ROTs
//     m_nt->Branch("CompROTcontainedROTsResLocY",     &m_compROTResLocY);     // residual in locY of the contained ROTs
//     m_nt->Branch("CompROTcontainedROTsPullLocX",    &m_compROTPullLocX);    // pull in locX of the contained ROTs
//     m_nt->Branch("CompROTcontainedROTsPullLocY",    &m_compROTPullLocY);    // pull in locY of the contained ROTs
// 
//     if (mjo_writeHitPositions){
//         m_hitX          = new std::vector< <std::vector<float> >();
//         m_hitY          = new std::vector< <std::vector<float> >();
//         m_hitZ          = new std::vector< <std::vector<float> >();
//         m_hitLoc1       = new std::vector< <std::vector<float> >();
//         m_hitLoc2       = new std::vector< <std::vector<float> >();
//         m_hitSurfaceX   = new std::vector< <std::vector<float> >();
//         m_hitSurfaceY   = new std::vector< <std::vector<float> >();
//         m_hitSurfaceZ   = new std::vector< <std::vector<float> >();
//         m_nt->Branch("CompROTcontainedROTsHitX",        &m_hitX);
//         m_nt->Branch("CompROTcontainedROTsHitY",        &m_hitY);
//         m_nt->Branch("CompROTcontainedROTsHitZ",        &m_hitZ);
//         m_nt->Branch("CompROTcontainedROTsHitLocal1",   &m_hitLoc1);
//         m_nt->Branch("CompROTcontainedROTsHitLocal2",   &m_hitLoc2);
//         m_nt->Branch("CompROTcontainedROTsHitSurfaceX", &m_hitSurfaceX);
//         m_nt->Branch("CompROTcontainedROTsHitSurfaceY", &m_hitSurfaceY);
//         m_nt->Branch("CompROTcontainedROTsHitSurfaceZ", &m_hitSurfaceZ);
//     }

    ATH_MSG_VERBOSE ("added items to ntuple");
    return StatusCode::SUCCESS;
}

////////////////////////////////////////
/// fill CompetingRIOsOnTrack data
////////////////////////////////////////
StatusCode Trk::DAF_ValidationNtupleHelper::fillMeasurementData (
    const Trk::MeasurementBase* measurement,
    const Trk::TrackParameters* trkParameters,
    //const NTuple::Item<long>& numberOfHits,
    //const std::bitset<8>& detectorType
    const int&,
    const bool& isOutlier) const {

    if (isOutlier) {
        // we do nothing for outliers, because competing ROTs are no outliers by definition
        return StatusCode::SUCCESS;
    }
    if (!trkParameters) {
      ATH_MSG_ERROR ( "no TrackParameters given" );
      return StatusCode::FAILURE;
    }

    const Trk::CompetingRIOsOnTrack* comprot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(measurement);
    if (comprot) {
        ATH_MSG_VERBOSE ( "measurement is CompetingRIOsOnTrack, fill ntuple variables" );
        //--------------------------
        // status info about compROT
        m_nContainedROTs->push_back(comprot->numberOfContainedROTs());
        unsigned int indexOfMaxAssgnProb =  comprot->indexOfMaxAssignProb();
        m_indexOfMaxAssgnProb->push_back(indexOfMaxAssgnProb);
        m_maxAssgnProb->push_back(comprot->assignmentProbability(indexOfMaxAssgnProb));

/*        for (unsigned int i=0; i < comprot->numberOfContainedROTs(); i++ ) {
            m_AssgnProbs[m_nHits-1][i] = comprot->assignmentProbability(i);            
            const Trk::RIO_OnTrack* currentROT = comprot->rioOnTrack(i);
            // --------------------------------------
            // fill residuals and pulls
            m_compROTResLocX[m_nHits-1][i] = -1000.;
            m_compROTResLocY[m_nHits-1][i] = -1000.;
            m_compROTPullLocX[m_nHits-1][i] = -1000.;
            m_compROTPullLocY[m_nHits-1][i] = -1000.;    
            const Trk::ResidualPull* residualPull = m_residualPullCalculator->residualPull(comprot->rioOnTrack(i), trkParameters, (m_isUnbiased==1)); //, detectorType);
            if (residualPull) {
                m_compROTResLocX[m_nHits-1][i] = residualPull->residual()[Trk::loc1];
                if (residualPull->dimension() >= 2)  m_compROTResLocY[m_nHits-1][i] = residualPull->residual()[Trk::loc2];
                if ((residualPull->isPullValid()) || mjo_ignoreMissTrkCov ) {
                    m_compROTPullLocX[m_nHits-1][i] = residualPull->pull()[Trk::loc1];
                    if (residualPull->dimension() >= 2)  m_compROTPullLocY[m_nHits-1][i] = residualPull->pull()[Trk::loc2];
                } else {
                    // warn only once!!!
                    //m_pullWarning = true;
                    log << MSG::WARNING << "no covariance of the track parameters given, can not calc pull!" << endreq;
                    log << MSG::WARNING << "you may want to use the jobOption 'IgnoreMissingTrackCovarianceForPulls' to calculate it anyhow." << endreq;
                }
                delete residualPull;
            } else {
                log << MSG::WARNING << "ResidualPullCalculator failed!" << endreq;
            }

            if (mjo_writeHitPositions){
                const Trk::Surface& hitSurface  = currentROT->associatedSurface();
                const Trk::GlobalPosition& sfc  = hitSurface.center();
            
                const Trk::LocalParameters& lp = currentROT->localParameters();
                const Trk::GlobalPosition&  gp  = currentROT->globalPosition();
                
                //!< @todo change back when DriftCircle::globalPosition() is fixed 
                if (detectorType==TRT){
                    // fill measurement global position
                    m_hitX[m_nHits-1][i] = float(sfc.x());
                    m_hitY[m_nHits-1][i] = float(sfc.y());
                    m_hitZ[m_nHits-1][i] = float(sfc.z());
                } else {
                    m_hitX[m_nHits-1][i] = float(gp.x());
                    m_hitY[m_nHits-1][i] = float(gp.y());
                    m_hitZ[m_nHits-1][i] = float(gp.z());
                }
            
                // fill surface global position
                m_hitSurfaceX[m_nHits-1][i]      = float(sfc.x());
                m_hitSurfaceY[m_nHits-1][i]      = float(sfc.y());
                m_hitSurfaceZ[m_nHits-1][i]      = float(sfc.z());
            
                // fill measurement local position
                if (detectorType==Pixel) {
                    m_hitLoc1[m_nHits-1][i] = float(lp[Trk::locX]);
                    m_hitLoc2[m_nHits-1][i] = float(lp[Trk::locY]);
                }
            
                if (detectorType==SCT) {
                    m_hitLoc1[m_nHits-1][i] = float(lp[Trk::locX]);
                    // check if second coordinate available:
                    if (lp.parameterKey() >= 2) {
                        m_hitLoc2[m_nHits-1][i] = float(lp[Trk::locY]);
                    } else {
                        m_hitLoc2[m_nHits-1][i] = -1000.;
                    }
                }
            
                if (detectorType==TRT) {
                    m_hitLoc1[m_nHits-1][i] = float(lp[Trk::driftRadius]);
                    m_hitLoc2[m_nHits-1][i] = -1000.;
                }            
            }*/
            
//             m_compROTResLocX[m_nHits-1][i] = comprot->rioOnTrack(i)->localParameters()[Trk::locX] - trkParameters->parameters()[Trk::locX];
//             // check if second coordinate available:
//             if (comprot->rioOnTrack(i)->localParameters().parameterKey() >= 2) {
//                 m_compROTResLocY[m_nHits-1][i] = comprot->rioOnTrack(i)->localParameters()[Trk::locY] - trkParameters->parameters()[Trk::locY];
//             } else {
//                 m_compROTResLocY[m_nHits-1][i] = -1000.;
//             }
//             if (measuredTrkParameters) {
//                 m_compROTPullLocX[m_nHits-1][i] = calcPull(m_compROTResLocX[m_nHits-1][i],
//                                                   comprot->rioOnTrack(i)->localErrorMatrix().covValue(Trk::locX),
//                                                   measuredTrkParameters->localErrorMatrix().covValue(Trk::locX));
//                 // check if second coordinate available:
//                 if (comprot->rioOnTrack(i)->localParameters().parameterKey() >= 2) {
//                     m_compROTPullLocY[m_nHits-1][i] = calcPull(m_compROTResLocY[m_nHits-1][i],
//                                                       comprot->rioOnTrack(i)->localErrorMatrix().covValue(Trk::locY),
//                                                       measuredTrkParameters->localErrorMatrix().covValue(Trk::locY));
//                 } else {
//                     m_compROTPullLocY[m_nHits-1][i] = -1000.;
//                 }
//             } else if(mjo_ignoreMissTrkCov) {
//                 m_compROTPullLocX[m_nHits-1][i] = calcPull(m_compROTResLocX[m_nHits-1][i],
//                                                   comprot->rioOnTrack(i)->localErrorMatrix().covValue(Trk::locX),
//                                                   0);
//                 // check if second coordinate available:
//                 if (comprot->rioOnTrack(i)->localParameters().parameterKey() >= 2) {
//                     m_compROTPullLocY[m_nHits-1][i] = calcPull(m_compROTResLocY[m_nHits-1][i],
//                                                       comprot->rioOnTrack(i)->localErrorMatrix().covValue(Trk::locY),
//                                                       0);
//                 } else {
//                     m_compROTPullLocY[m_nHits-1][i] = -1000.;
//                 }
//             }
//        }
    } else {
      m_nContainedROTs->push_back(0);
      m_indexOfMaxAssgnProb->push_back(0);
      m_maxAssgnProb->push_back(0.);
      ATH_MSG_VERBOSE ( "measurement is not a CompetingRIOsOnTrack, ignore it");
    }
    return StatusCode::SUCCESS;
}

StatusCode Trk::DAF_ValidationNtupleHelper::resetVariables (
        const int&  ) const {
    m_nContainedROTs->clear();
    m_indexOfMaxAssgnProb->clear();
    m_maxAssgnProb->clear();
    return StatusCode::SUCCESS;
}

StatusCode Trk::DAF_ValidationNtupleHelper::fillHoleData (
        const Trk::TrackStateOnSurface&,
        const int&) const {
    // we do nothing with holes
    return StatusCode::SUCCESS;
}
