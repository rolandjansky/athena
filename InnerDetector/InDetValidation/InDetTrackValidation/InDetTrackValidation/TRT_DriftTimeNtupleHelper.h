/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TRT_DriftTimeNtupleHelper.h
//   Header file for TRT_DriftTimeNtupleHelper
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// John.Alison@cern.ch Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef INDET_TRT_DRIFTTIMENTUPLEHELPER_H
#define INDET_TRT_DRIFTTIMENTUPLEHELPER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkValInterfaces/IValidationNtupleHelperTool.h"
#include "TrkParameters/TrackParameters.h"

class AtlasDetectorID;
class MeasurementBase;
class TRT_ID;
class TTree;
class ITRT_CalDbSvc;
class ITRT_StrawNeighbourSvc;

namespace InDet {


/** @class TRT_DriftTimeNtupleHelper
    This validation helper tool writes TRT drift circle data into the ntuple.
    The ntuple variables contain the drift time, the local hit and track positions,
    a highlevel indicator and information about the straw hit. This helper tool can be called by
    the Trk::BasicValidationNtupleTool.
    @author Sebastian.Fleischmann@cern.ch
*/

class TRT_DriftTimeNtupleHelper : virtual public Trk::IValidationNtupleHelperTool, public AthAlgTool {
public:

    // standard AlgToolmethods
    TRT_DriftTimeNtupleHelper(const std::string&,const std::string&,const IInterface*);
    ~TRT_DriftTimeNtupleHelper();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();

    /** fill drift time data */
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
        const int& ) const;


    /** add items to the ntuple and configure the helper tool:
    should be called once (per detector type) by the steering tool (Trk::IValidationNtupleTool) */
    virtual StatusCode addNtupleItems (
        TTree* tree,
        const int& detectorType ) const;

    /** reset ntuple variables */
    virtual StatusCode resetVariables (
        const int& detectorType ) const;

private:

    const TRT_ID* m_trtID; //!< the TRT ID helper
    
    ServiceHandle<ITRT_CalDbSvc> m_trtCondDbSvc; //!< the TRT cond DB svc, get the t0.
    ServiceHandle<ITRT_StrawNeighbourSvc> m_TRTStrawNeighbourSvc; //!< the TRT cond DB svc, get the pad and chip.

    // --- ntuple items. Document here to allow description to enter doxygen and wiki ---
    mutable std::vector<float>* m_Trt_R;   //!< measured TRT drift radius UNIT:mm
    mutable std::vector<float>* m_Trt_R_Err;   //!< err TRT drift radius UNIT:mm
    mutable std::vector<float>* m_Trt_Rtrack; //!< local track position of the track fit in the drift circle's coordinate system UNIT:mm
    mutable std::vector<float>* m_Trt_Rtrack_Err; //!< Error on the local track position of the track fit in the drift circle's coordinate system UNIT:mm
    mutable std::vector<float>* m_Trt_DTtrack; //!< Drift time prediction (using r(t) relation) based on local track fit position  UNIT:ns
    mutable std::vector<float>* m_Trt_t;   //!< TRT raw drift time (value -999.0 if not valid) UNIT:ns
    mutable std::vector<float>* m_Trt_tot; //!< TRT time over threshold UNIT:ns
    mutable std::vector<unsigned int>* m_Trt_bitpattern; //<! TRT bitted word from DAW UNIT:32-bit
    mutable std::vector<int>*   m_Trt_HL;  //!< TRT high-level threshold flag for TR hits UNIT:1:HL, 0:low
    mutable std::vector<int>*   m_Trt_strawlayer; //!< layer index of the straw, from TRT IdHelper (see also InDetIdentifier documentation)
    mutable std::vector<int>*   m_Trt_layer; //!< module index of the meas't, from TRT IdHelper
    mutable std::vector<int>*   m_Trt_straw; //!< straw index inside layer, from TRT IdHelper
    mutable std::vector<int>*   m_Trt_phi;   //!< phi module index, from TRT IdHelper
    mutable std::vector<int>*   m_Trt_bec;   //!< pos/neg barrel (+/-1) or pos/neg endcap (+/-2) UNIT:1:bar, 2:ec
    mutable std::vector<int>*   m_Trt_chip;  //!< the chip on which the hit is 
    mutable std::vector<int>*   m_Trt_pad;   //!< the HV pad on which the hit is 

};


} // end of namespace

#endif // INDET_TRT_DRIFTTIMENTUPLEHELPER_H
