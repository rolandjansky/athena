/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Trk_BasicValTrkParticleNtupleTool_h
#define Trk_BasicValTrkParticleNtupleTool_h


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkValInterfaces/IValidationNtupleTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include <vector>

class TTree;

namespace Trk {
class TrackSummary;
class TrackParticleBase;

/** @class BasicValTrkParticleNtupleTool
    @brief This validation tool writes basic information about Trk::TrackParticleBase
    into an ntuple; 
    
    It can book the ntuple, create the tree directory and add basic entries.
    This tool is also able to write the current record to the ntuple file and start a new entry.
    You can also use an existing ntuple tree (which has to have the same track-wise structure)
    and just use the Trk::BasicValidationNtupleTool to add some ntuple variables.
    
    Every entry (Rec:TrackPartcile) contains the event number, a track id (numbering the tracks within
    one event).
    
    Perigee parameters and track summary are filled for every track if available.

    @author Pavel.Ruzicka@cern.ch
*/

class BasicValTrkParticleNtupleTool : virtual public IValidationNtupleTool, public AthAlgTool {
public:
    // standard AlgToolmethods
    BasicValTrkParticleNtupleTool(const std::string&,const std::string&,const IInterface*);
    ~BasicValTrkParticleNtupleTool();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();

    /** @brief add branches to the tree
      Should be called once (per track collection and tree) dunring the initialisation phase by the calling algorithm
      (usually Trk::TrackValidationNtupleWriter) */
    virtual StatusCode addNtupleItems (
        TTree* tree ) const;

    /** fill AND write ntuple data of a given track */
    virtual StatusCode writeTrackData (
        const Trk::Track&,
        const int iterationIndex,
        const unsigned int fitStatCode ) const;

     /** fill AND write ntuple data of a given track particle */
    virtual StatusCode writeTrackParticleData (
        const Trk::TrackParticleBase& ) const;
    
    /** fill ntuple data of a given track without writing the record.
    - if this method is called twice without writing the ntuple inbetween the first data will be lost! */
    virtual StatusCode fillTrackData (
        const Trk::Track&,
        const int iterationIndex,
        const unsigned int fitStatCode ) const;

    /** fill ntuple data of a given track particle without writing the record.
    - if this method is called twice without writing the ntuple inbetween the first data will be lost! */
    virtual StatusCode fillTrackParticleData (
        const Trk::TrackParticleBase&) const;
        
    /** fill ntuple data of a given track without writing the record.
    - if this method is called twice without writing the ntuple inbetween the first data will be lost! */
    virtual StatusCode fillTrackParameter (
        const Trk::TrackParameters*,
        const int iterationIndex ) const;
    
    /** fill ntuple data of given measurement and track parameters without writing the record*/
    virtual StatusCode fillMeasurementData (
        const Trk::MeasurementBase*,
        const Trk::TrackParameters* ) const;
        
    /** fill ntuple data of an outlier measurement (without writing the
        record yet). The TrackParameters and FQoS are optional, i.e.
        NULL pointers should be handled by the implementation */
    virtual StatusCode fillOutlierData (
        const Trk::MeasurementBase*,
        const Trk::TrackParameters*,
        const Trk::FitQualityOnSurface* ) const;

    /** fill ntuple data of holes on track without writing the record */
    virtual StatusCode fillHoleData (
        const Trk::TrackStateOnSurface&) const;

    /** fill ntuple data of holes on track without writing the record */
    virtual StatusCode fillTrackTruthData ( const TrackParameters*&,
                                            const TrackTruth&, 
                                            const int truthIndex = -1 ) const;  

    /** fill AND write ntuple data of a given proto-trajectory (function used for fitter validation) */
    virtual StatusCode writeProtoTrajectoryData (
        const Trk::ProtoTrajectory&,
        const int iterationIndex,
        const Trk::Perigee* = 0,
        const unsigned int fitStatCode  = 0 ) const;

    /** write the filled data into the ntuple */
    virtual StatusCode writeRecord( TTree* tree ) const; 
private:
    StatusCode fillTrkParticlePerigee(const Trk::Perigee* perigee) const;
    StatusCode fillTrkParticleSummary (const Trk::TrackSummary* summary) const;
    StatusCode fillFitQualityData (const Trk::FitQuality* fitQuality) const;
    void resetVariables() const;

    // jobOptions
    bool m_bookNewNtuple;               //!< jobOption: book new ntuple?
    std::string m_ntupleFileName;       //!< jobOption: Ntuple file name
    std::string m_ntupleDirName;        //!< jobOption: Ntuple directory name
    std::string m_ntupleTreeName;       //!< jobOption: Ntuple tree name

    TTree* m_nt; //!< Pointer to the NTuple tree
    
    mutable int             m_TrackIDcounter;  //!< help variable to link with event property tree 
    mutable unsigned int    m_lastEventNumber; //!< help variable to link with event property tree

    // --- ntuple variables, doxygen comments are also basis for wiki texts --- 
    mutable int        		 m_runNumber;   //!< run number the track belongs to
    mutable int         	 m_eventNumber; //!< event number the track belongs to 
    mutable unsigned char        m_TrackID;     //!< number of the track within the current event
    
    //----  extraction from  Track Perigee -----
    mutable float       m_Rec_d0;      //!< reconstructed track params: local d0 at perigee    UNIT:mm
    mutable float       m_Rec_z0;      //!< reconstructed track params: local z0 at perigee    UNIT:mm
    mutable float       m_Rec_phi0;    //!< reconstructed track params: angle phi at perigee   UNIT:rad
    mutable float       m_Rec_eta;     //!< reconstructed track params: pseudorapidity         UNIT:1
    mutable float       m_Rec_qOverP;  //!< reconstructed track params: curvature q/p          UNIT:1/MeV

    mutable float       m_errord0;     //!< error on local d0 at perigee    UNIT:mm
    mutable float       m_errorz0;     //!< error on local z0 at perigee    UNIT:mm
    mutable float       m_errorphi0;   //!< error on angle phi at perigee   UNIT:rad
    mutable float       m_errortheta0; //!< error on angle theta at perigee UNIT:rad
    mutable float       m_errorqoverp; //!< error on curvature q/p          UNIT:1/MeV
    
    //----  extraction from  Fit Quality -----
    mutable float      		  m_chi2;                          //!< chi sqared of the track fit normalized to DOF

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

    mutable unsigned long    m_idHitPattern;                  //!< bit word carrying information about hit layers
     
    static const float  s_errorEntry;        //!< error entry costant

    SG::ReadHandleKey<xAOD::EventInfo>    m_evt  {this, "EvtInfo", "EventInfo", "EventInfo name"};
};


} // end of namespace Trk

#endif // BasicsValTrkParticleNtupleTool_h
