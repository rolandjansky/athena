/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ResidualValidationNtupleHelper.h
//   Header file for ResidualValidationNtupleHelper
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_RESIDUALVALIDATIONNTUPLEHELPER_H
#define TRK_RESIDUALVALIDATIONNTUPLEHELPER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkValInterfaces/IValidationNtupleHelperTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/TrackStateDefs.h"

class AtlasDetectorID;
class TTree;

namespace Trk {

class MeasurementBase;
class IResidualPullCalculator;

/** @class ResidualValidationNtupleHelper
    This validation helper tool writes residual data into the ntuple using the common Trk::ResidualPullCalculator.
    @author Sebastian.Fleischmann@cern.ch
*/

class ResidualValidationNtupleHelper : virtual public Trk::IValidationNtupleHelperTool, public AthAlgTool {
public:

    // standard AlgToolmethods
    ResidualValidationNtupleHelper(const std::string&,const std::string&,const IInterface*);
    ~ResidualValidationNtupleHelper();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();

    /** fill residual data */
    virtual StatusCode fillMeasurementData (
        const Trk::MeasurementBase*,
        const Trk::TrackParameters*,
        //const NTuple::Item<long>& numberOfHits,
        //const std::bitset<8>& detectorType
        const int& detectorType,
        const bool& isOutlier ) const;

    /** fill special data about holes on track (here: do nothing) */
    virtual StatusCode fillHoleData (
        const Trk::TrackStateOnSurface&,
        const int&) const;


    /** add items to the ntuple and configure the helper tool:
    should be called once (per detector type) by the steering tool (Trk::IValidationNtupleTool) */
    virtual StatusCode addNtupleItems (
        TTree* tree,
        const int& detectorType ) const;

    /** reset ntuple variables */
    virtual StatusCode resetVariables (
        const int& detectorType ) const;

private:

    double calcPull(
        const double residual,
        const double locMesCov,
        const double locTrkCov ) const;
    void givePullWarning() const;
    void fillValues(const int& detectorType, double res1=-1000., double pull1=-1000., double res2=-1000., double pull2=-1000.) const;

    mutable bool                m_pullWarning; //!< warn only once, if pull cannot be calculated
    mutable bool                m_trkParametersWarning; //!< warn only once, if no track parameters exist

    //! The residual and pull calculator tool
    ToolHandle<Trk::IResidualPullCalculator> m_residualPullCalculator;

    bool mjo_ignoreMissTrkCov;

    mutable int* m_isUnbiased;

    // --- ntuple variables, documentation picked up by scripts for wiki tables ---
    mutable std::vector<float>* m_PixPullPhi; //!< hit pull in pixel local phi coord for each Pixel meas't, vector of size nPixelHits UNIT:1
    mutable std::vector<float>* m_PixPullEta; //!< hit pull in pixel local eta coord for each Pixel meas't, vector of size nPixelHits UNIT:1
    mutable std::vector<float>* m_PixResPhi;  //!< hit residual in pixel local phi for each Pixel meas't, vector of size nPixelHits UNIT:mm
    mutable std::vector<float>* m_PixResEta;  //!< hit residual in pixel local eta for each Pixel meas't, vector of size nPixelHits UNIT:mm
    
    mutable std::vector<float>* m_SCTPull;    //!< hit pull in SCT local strip coord for each SCT meas't (corrects for fan-shaped strip arrangement in endcap, i.e. pull is perpendicular to strip direction), vector of size nSCTHits UNIT:1
    mutable std::vector<float>* m_SCTRes;     //!< hit residual in SCT local strip coord for each SCT meas't (corrects for fan-shaped strip arrangement in endcap, i.e. residual is perpendicular to strip direction), vector of size nSCTHits  UNIT:mm

    mutable std::vector<float>* m_TRTPull;    //!< hit pull in TRT drift radius for each TRT meas't, vector of size nTRTHits  UNIT:1
    mutable std::vector<float>* m_TRTRes;     //!< hit residual in TRT drift radius for each TRT meas't, vector of size nTRTHits  UNIT:mm

    mutable std::vector<float>* m_MDTPull;    //!< hit pull in MDT drift radius for each MDT meas't, vector of size nMDTHits  UNIT:1
    mutable std::vector<float>* m_MDTRes;     //!< hit residual in MDT drift radius for each MDT meas't, vector of size nMDTHits  UNIT:mm

    mutable std::vector<float>* m_RPCPull;    //!< hit pull in RPC strip coordinate for each RPC meas't, vector of size nRPCHits  UNIT:1
    mutable std::vector<float>* m_RPCRes;     //!< hit residual in RPC coordinate for each RPC meas't, vector of size nRPCHits  UNIT:mm

    mutable std::vector<float>* m_TGCPull;    //!< hit pull in TGC strip coordinate for each TGC meas't (corrects for fan-shaped strip arrangement), vector of size nTGCHits  UNIT:1
    mutable std::vector<float>* m_TGCRes;     //!< hit residual in TGC strip coordinate for each TGC meas't, vector of size nTGCHits  UNIT:mm

    mutable std::vector<float>* m_CSCPull;    //!< hit pull in CSC ??? coordinate for each CSC meas't, vector of size nCSCHits  UNIT:1
    mutable std::vector<float>* m_CSCRes;     //!< hit residual in CSC ??? coordinate for each CSC meas't, vector of size nCSCHits  UNIT:mm

};

 inline void Trk::ResidualValidationNtupleHelper::givePullWarning() const {
    if(!m_pullWarning) {
        m_pullWarning = true;
        msg(MSG::WARNING) << "no covariance of the track parameters given, can not calc pull!" << endreq;
        msg(MSG::WARNING) << "you may want to use the jobOption 'IgnoreMissingTrackCovariance' to calculate it anyhow." << endreq;
    }
 }
 inline void Trk::ResidualValidationNtupleHelper::fillValues(const int& detectorType, double res1, double pull1, double res2, double pull2) const {
   if (detectorType==TrackState::Pixel) {
        m_PixResPhi->push_back(res1);
        m_PixResEta->push_back(res2);
        m_PixPullPhi->push_back(pull1);
        m_PixPullEta->push_back(pull2);
    } else if (detectorType==TrackState::SCT) {
        m_SCTRes->push_back(res1);
        m_SCTPull->push_back(pull1);
    } else if (detectorType==TrackState::TRT) {
        m_TRTRes->push_back(res1);
        m_TRTPull->push_back(pull1);
    } else if (detectorType==TrackState::MDT) {
        m_MDTRes->push_back(res1);
        m_MDTPull->push_back(pull1);
    } else if (detectorType==TrackState::RPC) {
        m_RPCRes->push_back(res1);
        m_RPCPull->push_back(pull1);
    } else if (detectorType==TrackState::TGC) {
        m_TGCRes->push_back(res1);
        m_TGCPull->push_back(pull1);
    } else if (detectorType==TrackState::CSC) {
        m_CSCRes->push_back(res1);
        m_CSCPull->push_back(pull1);
    }
}

} // end of namespace

#endif // TRK_RESIDUALVALIDATIONNTUPLEHELPER_H
