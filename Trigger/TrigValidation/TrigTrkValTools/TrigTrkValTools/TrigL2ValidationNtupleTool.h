/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//   TrigL2ValidationNtupleTool.h
//   Header file for TrigL2ValidationNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Pavel.Ruzicka@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRIGL2VALIDATIONNTUPLETOOL_H
#define TRIGL2VALIDATIONNTUPLETOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigTrkValInterfaces/ITrigL2ValidationNtupleTool.h"
#include <vector>

class TTree;


/** @class BasicValidationNtupleTool
    @brief This validation tool writes basic information about Trk::Track
    into an ntuple; it is also the steering tool for all Trk::IValidationNtupleHelperTool.
    
    It can book the ntuple, create the tree directory and add basic entries.
    This tool is also able to write the current record to the ntuple file and start a new entry.
    You can also use an existing ntuple tree (which has to have the same track-wise structure)
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
    
    Perigee parameters are filled for every track if available.

    @author Pavel.Ruzicka@cern.ch
*/

class TrigL2ValidationNtupleTool: virtual public ITrigL2ValidationNtupleTool, public AthAlgTool {
public:

    // standard AlgToolmethods
    TrigL2ValidationNtupleTool(const std::string&,const std::string&,const IInterface*);
    ~TrigL2ValidationNtupleTool();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();

    /** @brief add branches to the tree
      Should be called once (per track collection and tree) dunring the initialisation phase by the calling algorithm
      (usually Trk::TrackValidationNtupleWriter) */
    virtual StatusCode addNtupleItems (
        TTree* tree ) const;

    /** fill AND write ntuple data of a given track */
    virtual StatusCode writeTrackData (const TrigInDetTrack&) const;

    /** fill ntuple data of a given track without writing the record.
    - if this method is called twice without writing the ntuple inbetween the first data will be lost! */
    virtual StatusCode fillTrackData (const TrigInDetTrack&) const;

    /** write the filled data into the ntuple */
    virtual StatusCode writeRecord(TTree* tree) const;

    /** fill ntuple data of a given track without writing the record.
    - if this method is called twice without writing the ntuple inbetween the first data will be lost! */
    virtual StatusCode fillTrackParameters (
        const TrigInDetTrackFitPar* ) const;

private:   
    void resetVariables() const;

    // jobOptions
    bool m_bookNewNtuple;               //!< jobOption: book new ntuple?
    std::string m_ntupleFileName;       //!< jobOption: Ntuple file name
    std::string m_ntupleDirName;        //!< jobOption: Ntuple directory name
    std::string m_ntupleTreeName;       //!< jobOption: Ntuple tree name
    
    TTree* m_nt;                         //!< Pointer to the NTuple tree

    mutable int             m_TrackIDcounter;
    mutable unsigned int    m_lastEventNumber;

    // --- ntuple variables, doxygen comments are also basis for wiki texts --- 
    mutable int                  m_runNumber;   	//!< run number the track belongs to
    mutable int                  m_eventNumber; 	//!< event number the track belongs to 
    mutable int                  m_TrackID;     	//!< number of the track within the current event
    mutable unsigned int         m_trackAuthor;  	//!< Track author SiTrack = 1, IdScan = 2, TrtXK = 4
   
    mutable float       m_Rec_d0;      //!< reconstructed track params: local d0 at perigee    UNIT:mm
    mutable float       m_Rec_z0;      //!< reconstructed track params: local z0 at perigee    UNIT:mm
    mutable float       m_Rec_phi0;    //!< reconstructed track params: angle phi at perigee   UNIT:rad
    mutable float       m_Rec_eta;     //!< reconstructed track params: pseudorapidity         UNIT:1
    mutable float       m_Rec_pT;      //!< reconstructed track params: curvature q/p          UNIT:1MeV

    mutable float       m_errord0;     //!< error on local d0 at perigee    UNIT:mm
    mutable float       m_errorz0;     //!< error on local z0 at perigee    UNIT:mm
    mutable float       m_errorphi0;   //!< error on angle phi at perigee   UNIT:rad
    mutable float       m_erroreta;    //!< error on angle theta at perigee UNIT:rad
    mutable float       m_errorpT;     //!< error on pT                     UNIT:1/MeV

    mutable float      m_chi2;        //!< chi2 of the track fit normalized on the number of DOF
  
    mutable int         m_nPixelHits;          //!< number of measurements in the Pixels  (without outliers)
    mutable int         m_nSCTHits;            //!< number of measurements in the SCT (without outliers)
    mutable int         m_nTRTHits;            //!< number of measurements in the TRT
    mutable int         m_nTRTHighTrshHits;    //!< number of measurements in the TRT with high treshold

    const static float  s_errorEntry;  //!< used for error entry
};

#endif // TRIGL2VALIDATIONNTUPLETOOL_H

