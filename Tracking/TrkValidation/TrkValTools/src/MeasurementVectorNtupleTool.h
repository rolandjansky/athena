/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// PerigeeParametersNtupleTool.h
//   Header file for PerigeeParametersNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann -at- cern.ch, Wolfgang.Liebig -at- cern.ch
///////////////////////////////////////////////////////////////////
#ifndef TRK_MEASUREMENTVECTORNTUPLETOOL_H
#define TRK_MEASUREMENTVECTORNTUPLETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkValInterfaces/ITrackValidationNtupleTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkParameters/TrackParameters.h"

class AtlasDetectorID;

namespace Trk {

  class IUpdator;
  class ITrackHoleSearchTool;
  class IValidationNtupleHelperTool;
  class IResidualPullCalculator;
  class MeasurementBase;
  class MeasurementTypeID;

/** @class MeasurementVectorNtupleTool
    @brief This validation tool writes basic information about Trk::Track
    into an ntuple; it is also the steering tool for all Trk::IValidationNtupleHelperTool.
    
    You use an existing ntuple tree (which has to have the same track-wise structure)
    and just use the Trk::BasicValidationNtupleTool to add some ntuple variables.
    
    One can give a list of Trk::IValidationNtupleHelperTool for each detector type by joboptions
    which are called by Trk::BasicValidationNtupleTool if a measurement of the related detector
    is found on the track. Those Trk::IValidationNtupleHelperTool can add items to the ntuple
    and fill special data needed for validation.
    
    Every entry (track) contains the event number, a track id (numbering the tracks within
    one event) and an optional iteration index, which can be used if different
    intermediate results of a track are produced in the refit procedure. The track id is the
    same for all tracks generated within one iteration process and reset to 0 every
    time a new event is started.

**/

class MeasurementVectorNtupleTool : virtual public Trk::ITrackValidationNtupleTool, public AthAlgTool {
public:

    // standard AlgToolmethods
    MeasurementVectorNtupleTool(const std::string&,const std::string&,const IInterface*);
    ~MeasurementVectorNtupleTool();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();

    /** @brief add branches to the tree
      Should be called once (per track collection and tree) dunring the initialisation phase by the
      calling algorithm (usually Trk::TrackValidationNtupleWriter) */
    virtual StatusCode addNtupleItems ( TTree* tree ) const;

    /** fill ntuple data of a given track without writing the record.
    - if this method is called twice without writing the ntuple inbetween the first data will be lost! */
    virtual StatusCode fillTrackData (
        const Trk::Track&,
        const int iterationIndex,
        const Trk::FitterStatusCode fitStatCode ) const;

    /** fill ntuple data of a given TrackParticle without writing the record.
        - if this method is called twice without writing the ntuple inbetween the first data will be lost! */
    virtual StatusCode fillTrackParticleData ( const Trk::TrackParticleBase& ) const;

    //! fill data about the truth match (score, parameter-pulls etc)
    virtual StatusCode fillTrackTruthData ( const TrackParameters*&,
                                            const TrackTruth&,
                                            const int) const;

    /** fill ntuple data of a given proto-trajectory (function used for fitter validation) */
    virtual StatusCode fillProtoTrajectoryData (
        const Trk::ProtoTrajectory&,
        const int iterationIndex,
        const Trk::Perigee*,
        const Trk::FitterStatusCode fitStatCode ) const;

    /** reset the variables after writing the record to disk ntuple */
    virtual void resetVariables( ) const;

private:

    /** fill variables with call for each measurement TSoS */
    StatusCode fillMeasurementData (
                    const Trk::MeasurementBase*,
                    const Trk::TrackParameters*,
                    const bool& isOutlier,
                    const int& detectorType) const;
    /** call the helper tools */
    StatusCode callHelperTools (
                    const Trk::MeasurementBase*,
                    const Trk::TrackParameters*,
                    const bool& isOutlier,
                    const int& detectorType,
                    const int& nCurrentHit) const;
    /** fill ntuple data of holes on track without writing the record */
    virtual StatusCode fillHoleData (
        const Trk::TrackStateOnSurface&) const;

    const AtlasDetectorID*  m_idHelper;   //!< Used to find out the sub-det from PRD->identify().
    MeasurementTypeID*      m_detTypeHelper;

    ToolHandle<Trk::IUpdator>                   m_updatorHandle;            //!< Tool handle of updator for unbiased states
    Trk::IUpdator*                              m_updator;                  //!< updator for unbiased states
    
    ToolHandle<Trk::IResidualPullCalculator>    m_residualPullCalculator;   //!< The residual and pull calculator tool
    ToolHandle<Trk::ITrackHoleSearchTool>       m_holeSearchTool;           //!< The hole search tool
    ToolHandleArray<IValidationNtupleHelperTool> m_PixelNtupleHelperToolHandles;    //!< jobOption: List of Ntuple validation helper tools for Pixel
    ToolHandleArray<IValidationNtupleHelperTool> m_SCTNtupleHelperToolHandles;      //!< jobOption: List of Ntuple validation helper tools for SCT
    ToolHandleArray<IValidationNtupleHelperTool> m_TRTNtupleHelperToolHandles;      //!< jobOption: List of Ntuple validation helper tools for TRT
    ToolHandleArray<IValidationNtupleHelperTool> m_MDTNtupleHelperToolHandles;      //!< jobOption: List of Ntuple validation helper tools for MDT
    ToolHandleArray<IValidationNtupleHelperTool> m_CSCNtupleHelperToolHandles;      //!< jobOption: List of Ntuple validation helper tools for CSC
    ToolHandleArray<IValidationNtupleHelperTool> m_RPCNtupleHelperToolHandles;      //!< jobOption: List of Ntuple validation helper tools for RPC
    ToolHandleArray<IValidationNtupleHelperTool> m_TGCNtupleHelperToolHandles;      //!< jobOption: List of Ntuple validation helper tools for TGC
    ToolHandleArray<IValidationNtupleHelperTool> m_GeneralNtupleHelperToolHandles;  //!< jobOption: List of Ntuple validation helper tools for data independent from detector technology

    bool m_useROTwithMaxAssgnProb;      //!< jobOption: use ROT with max. assignment probabilty in the case of Trk::CompetingRIOsOnTrack or use competingROT itself?
    bool mjo_ignoreMissTrkCov;          //!< jobOption: ignore missing covariance of track parameters for pull calculation?
    std::vector<const IValidationNtupleHelperTool*> m_PixelHelperTools;     //!< the helper tools for Pixel data
    std::vector<const IValidationNtupleHelperTool*> m_SCTHelperTools;       //!< the helper tools for SCT data
    std::vector<const IValidationNtupleHelperTool*> m_TRTHelperTools;       //!< the helper tools for TRT data
    std::vector<const IValidationNtupleHelperTool*> m_MDTHelperTools;       //!< the helper tools for MDT data
    std::vector<const IValidationNtupleHelperTool*> m_CSCHelperTools;       //!< the helper tools for CSC data
    std::vector<const IValidationNtupleHelperTool*> m_RPCHelperTools;       //!< the helper tools for RPC data
    std::vector<const IValidationNtupleHelperTool*> m_TGCHelperTools;       //!< the helper tools for TGC data
    std::vector<const IValidationNtupleHelperTool*> m_GeneralHelperTools;   //!< the helper tools for data independent from detector technology
    // DNA     ToolHandle< ITrackBreakpointAnalyser >  m_brempointAnalyser;
    mutable bool      m_pullWarning;    //!< warn only ones, if pull cannot be calculated
    mutable bool      m_UpdatorWarning; //!< warn only ones, if unbiased track states can not be calculated
    mutable bool      m_trkParametersWarning; //!< warn once only, if no track parameters exist
    bool              m_doTruth;        //!< switch to turn truth on/off
    bool              m_doHoleSearch;   //!< switch to turn hole search on/off
    mutable std::map<const IValidationNtupleHelperTool*, bool>  m_helperToolWarning;

    mutable int                 m_isUnbiased;  //!< flag if track states are unbiased to the related measurements (0:no, 1:yes)
    mutable std::vector<float>* m_residualLocX; //!< residual in Trk::LocX for each measurement on the track, vector of size nHits, value set to -1000 if meas't was outlier UNIT:mm
    mutable std::vector<float>* m_residualLocY; //!< residual in Trk::LocY for each meas't on the track (value is set to -1000 if no second local coord. exists for the meas't), vector of size nHits UNIT:mm
    mutable std::vector<float>* m_pullLocX; //!< pull in Trk::LocX for each measurement on the track, vector of size nHits, value set to -1000 if meas't was outlier UNIT:1
    mutable std::vector<float>* m_pullLocY; //!< pull in Trk::LocY for each measurement on the track (value is set to -1000 if no second local coord. exists for the meas't), vector of size nHits UNIT:1
    mutable std::vector<int>*   m_DetectorType; //!< Detector type of the measurement, enum according to TrkEventPrimitives/TrackStateDefs.h, vector of size nHits
    mutable std::vector<int>*   m_isOutlier; //!< flag if measurement is outlier (1) or not (0), vector of size nHits UNIT:0:no, 1:yes

    mutable int         m_nPixelHits; //!< number of measurements in the Pixels  (without outliers)
    mutable int         m_nSCTHits;   //!< number of measurements in the SCT (without outliers)
    mutable int         m_nTRTHits;   //!< number of measurements in the TRT (without outliers)
    mutable int         m_nMDTHits;   //!< number of measurements in the MDTs (without outliers)
    mutable int         m_nCSCHits;   //!< number of measurements in the CSC (without outliers)
    mutable int         m_nRPCHits;   //!< number of measurements in the RPC (without outliers)
    mutable int         m_nTGCHits;   //!< number of measurements in the TGC (without outliers)

    mutable std::vector<int>*   m_pixelHitIndex; //!< corresp. index for pixel measurement vector
    mutable std::vector<int>*   m_sctHitIndex; //!< corresp. index for SCT measurement vector
    mutable std::vector<int>*   m_trtHitIndex; //!< corresp. index for TRT measurement vector
    mutable std::vector<int>*   m_mdtHitIndex; //!< corresp. index for MDT measurement vector
    mutable std::vector<int>*   m_cscHitIndex; //!< corresp. index for CSC measurement vector
    mutable std::vector<int>*   m_rpcHitIndex; //!< corresp. index for RPC measurement vector
    mutable std::vector<int>*   m_tgcHitIndex; //!< icorresp. index for TGC measurement vector
    
    // hole data:
    mutable int         m_nHoles;                   //!< number of holes on track
    mutable int         m_nPixelHoles;                   //!< number of Pixel holes on track
    mutable int         m_nSctHoles;                   //!< number of SCT holes on track
    mutable std::vector<int>*   m_HoleDetectorType; //!< detector type of the hole (vector of size nHoles)


};


} // end of namespace


#endif // TRK_PERIGEEPARAMETERSNTUPLETOOL_H
