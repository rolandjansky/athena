/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DirectTrackNtupleWriterTool.h
//   Header file for DirectTrackNtupleWriterTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_DIRECTTRACKVALIDATIONNTUPLETOOL_H
#define TRK_DIRECTTRACKVALIDATIONNTUPLETOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkValInterfaces/IDirectTrackNtupleWriter.h"

class TTree;

namespace Trk {

class IExtrapolator;
class ITrackValidationNtupleTool;
class Track;



/** @class DirectTrackNtupleWriterTool
    @brief This validation tool writes basic information about Trk::Track into an ntuple; it is also the steering tool for all Trk::IValidationNtupleTool.
    
    It books the ntuple, create the tree directory and add basic entries.
    With every call this tool writes the current record to the ntuple file and starts a new entry.
    One can give a list of Trk::IValidationNtupleTool by joboptions.
    
    Every entry (track) contains the event number, a track id (numbering the tracks within
    one event) and an optional iteration index, which can be used if different
    intermediate results of a track are produced in the refit procedure. The track id is the
    same for all tracks generated within one iteration process and reset to 0 every
    time a new event is started.
    Perigee parameters are filled for every track if available.

    @author Sebastian.Fleischmann@cern.ch
*/

class DirectTrackNtupleWriterTool : virtual public Trk::IDirectTrackNtupleWriter, public AthAlgTool {
public:

    // standard AlgToolmethods
    DirectTrackNtupleWriterTool(const std::string&,const std::string&,const IInterface*);
    ~DirectTrackNtupleWriterTool();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();

    /** fill AND write ntuple data of a given track */
    virtual StatusCode writeTrackData (
        const Trk::Track&,
        const int iterationIndex,
        const Trk::FitterStatusCode fitStatCode ) const;

    /** fill AND write ntuple data of a given track particle */
    virtual StatusCode writeTrackParticleData ( const Trk::TrackParticleBase& ) const;

    /** fill AND write ntuple data of a given proto-trajectory (function used for fitter validation) */
    virtual StatusCode writeProtoTrajectoryData (
        const Trk::ProtoTrajectory&,
        const int iterationIndex,
        const Trk::Perigee*,
        const Trk::FitterStatusCode fitStatCode ) const;
    
private:
    /** write the filled data into the ntuple */
    StatusCode writeRecord(TTree* tree) const;

    ToolHandleArray<Trk::ITrackValidationNtupleTool> m_ValidationNtupleTools;  //!< set of tools for writting Trk::Track into the Ntuple
    ToolHandle<Trk::IExtrapolator>                   m_extrapolatorHandle;     //!< extrapolator, in case tracks do not have Perigee
    std::string m_ntupleFileName;       //!< jobOption: Ntuple file name
    std::string m_ntupleDirName;        //!< jobOption: Ntuple directory name
    std::string m_ntupleTreeName;       //!< jobOption: Ntuple tree name
    //NTuple::Tuple* p_ntuple;            //!< Pointer to the ntuple
    TTree* m_nt; //!< Pointer to the NTuple tree
    bool              m_doTruth;        //!< switch to turn truth on/off
};


} // end of namespace

#endif // TRK_DIRECTTRACKVALIDATIONNTUPLETOOL_H

