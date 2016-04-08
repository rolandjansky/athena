/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNMENT_ALIGNTRACKPREPROCESSOR_IH
#define TRKALIGNMENT_ALIGNTRACKPREPROCESSOR_IH

/**
   @file IAlignTrackPreProcessor.h
   @class IAlignTrackPreProcessor
   
   @brief Generic interface for alignment algtools which process a track
   collection and return a collection of AlignTracks 
   (e.g. refits, with or without vertexing, track selection ...)
   
   @author Robert Harrington <roberth@bu.edu>
   @author Daniel Kollar <daniel.kollar@cern.ch>
   @date 10/1/09
*/

#include "GaudiKernel/IAlgTool.h"
#include "DataModel/DataVector.h"

#include <fstream>

class TFile;

namespace Trk {

  class Track;
  class AlignTrack;

  static const InterfaceID 
    IID_Trk_IAlignTrackPreProcessor("IAlignTrackPreProcessor",1,0);

  class IAlignTrackPreProcessor: virtual public IAlgTool {

  public:
    static const InterfaceID& interfaceID();

    /** Main processing of track collection.
        The method should create AlignTracks but fill them into a collection of Tracks.
        The reason is that AlignTracks are needed further in the alignment processing
        but we want to store the newly created TrackCollection into StoreGate. */
    virtual DataVector<Trk::Track>* processTrackCollection(const DataVector<Trk::Track>* trks) = 0;

    /** sets the output stream for the logfile */
    virtual void setLogStream(std::ostream * os) { m_logStream = os; }
    
    /** Sets output ntuple file, to be implemented if needed for detector-specific implementation. */
    virtual void setNtuple(TFile*) {} 

    /** writes tree and histogrms to ntuple */
    virtual StatusCode fillNtuple() { return StatusCode::SUCCESS; }

    /** methods added for the full VTX fit: */
    virtual void accumulateVTX(const AlignTrack* ) {}

    virtual void solveVTX() {}

    /** Print processing summary to logfile. */
    virtual void printSummary() {}

  protected:
    
    std::ostream * m_logStream; //!< logfile output stream

  };

  inline const InterfaceID& IAlignTrackPreProcessor::interfaceID()
  {
    return IID_Trk_IAlignTrackPreProcessor;
  }

} // namespace Trk

#endif // TRKALIGNMENT_ALIGNTRACKPREPROCESSOR_IH
