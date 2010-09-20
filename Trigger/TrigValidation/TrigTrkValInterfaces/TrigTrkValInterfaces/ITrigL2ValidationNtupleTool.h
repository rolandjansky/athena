/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ITrigL2ValidationNtupleTool.h
//   Header file for interface of TrigL2ValidationNtupleTools
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Pavel.Ruzicka@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef ITRIGL2VALIDATIONNTUPLETOOL_H
#define ITRIGL2VALIDATIONNTUPLETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>

class TrigInDetTrack;
class TrigInDetTrackFitPar;


static const InterfaceID IID_ITrigL2ValidationNtupleTool("ITrigL2ValidationNtupleTool",1,0);

/** @class ITrigL2ValidationNtupleTool
    provides the interface for validation tools which write special information
    about generated tracks into ntuples.
*/

class ITrigL2ValidationNtupleTool : virtual public IAlgTool {
public:
     /**Interface ID, declared here, and defined below*/
    static const InterfaceID& interfaceID();

    /** @brief add branches to the tree
      Should be called once (per track collection and tree) dunring the initialisation phase by the calling algorithm
      (usually Trk::TrackValidationNtupleWriter) */
    virtual StatusCode addNtupleItems ( TTree* tree ) const = 0;

    /** fill AND write ntuple data of a given track */
    virtual StatusCode writeTrackData ( const TrigInDetTrack&) const = 0;
    
    /** fill ntuple data of a given track without writing the record.
    - if this method is called twice without writing the ntuple inbetween the first data will be lost! */
    virtual StatusCode fillTrackData ( const TrigInDetTrack&) const = 0;
        
    /** fill ntuple data of a given track without writing the record.
    - if this method is called twice without writing the ntuple inbetween the first data will be lost! */
    virtual StatusCode fillTrackParameters ( const TrigInDetTrackFitPar* ) const = 0;

    /** write the filled data into the ntuple */
    virtual StatusCode writeRecord( TTree* tree ) const = 0;
};

inline const InterfaceID& ITrigL2ValidationNtupleTool::interfaceID() {
    return IID_ITrigL2ValidationNtupleTool;
}



#endif // ITRIGL2VALIDATIONNTUPLETOOL_H
