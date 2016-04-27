/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrackSummaryNtupleTool.h
//   Header file for TrackSummaryNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig -at- cern.ch, Pavel.Ruzicka -at- cern.ch
///////////////////////////////////////////////////////////////////
#ifndef TRK_TRACKSUMMARYNTUPLETOOL_H
#define TRK_TRACKSUMMARYNTUPLETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkValInterfaces/ITrackValidationNtupleTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {

  class TrackSummary;

/** @class TrackSummaryNtupleTool
    @brief Tool to add information from TrackSummary objects to the tracking ntuple.
    
    Uses the plugin concept from the ITrackValidationNtupleTool to add variables to a
    Trk::Track or Trk::TrackParticleBase oriented ntuple writer.
    
    @author Wolfgang.Liebig (at) cern.ch
**/

class TrackSummaryNtupleTool : virtual public Trk::ITrackValidationNtupleTool, public AthAlgTool {
public:

    // standard AlgToolmethods
    TrackSummaryNtupleTool(const std::string&,const std::string&,const IInterface*);
    ~TrackSummaryNtupleTool();

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

    // summary filling common to Track and TrackParticle input
    StatusCode fillTrackSummary(const Trk::TrackSummary* summary) const;
    bool m_addParticleID;
    
    //----  extraction from TrackSummary -----
    mutable unsigned char         m_numberOfPixelHits;             //!< number of measurements in the Pixels  
    mutable unsigned char         m_numberOfContribPixelLayers;    //!< number of contributing layers of the pixel detector
    mutable unsigned char         m_numberOfPixelHoles;            //!< number of pixel layers on track with absence of hits 
    mutable unsigned char         m_numberOfPixelDeadSensors;      //!< number of dead pixel sensors crossed
    mutable unsigned char         m_numberOfSCTHits;               //!< number of measurements in the SCT 
    mutable unsigned char         m_numberOfSCTHoles;              //!< number of SCT holes
    mutable unsigned char         m_numberOfSCTDeadSensors;        //!< number of dead SCT sensors crossed
    mutable unsigned char         m_numberOfTRTHits;               //!< number of TRT hits
    mutable unsigned char         m_numberOfTRTHoles;              //!< number of TRT holes
    mutable unsigned char         m_numberOfTRTDeadStraws;         //!< number of dead TRT straws crossed
    mutable unsigned char         m_numberOfTRTHighThresholdHits;  //!< number of TRT hits which pass the high threshold

    mutable float                 m_dedx;                          //!< pixel dEdx 
    mutable int                   m_ngoodhitsdedx;                 //!< number of hits used for calculating pixel dEdx
    mutable float                 m_massdedx;                      //!< mass calculated using pixel dEdx
    mutable std::vector<float>   *m_likelihoodsdedx;               //!< vector of likelihoods (pion/kaon/proton) from Pixel dEdx

    mutable unsigned long    m_idHitPattern;                  //!< bit word carrying information about hit layers
};


} // end of namespace


#endif // TRK_TRACKSUMMARYNTUPLETOOL_H
