/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNINTERFACES_TRACKCOLLECTIONPROVIDER_IH
#define TRKALIGNINTERFACES_TRACKCOLLECTIONPROVIDER_IH

#include "GaudiKernel/IAlgTool.h"

#include "TrkTrack/TrackCollection.h"

#include <fstream>

/**
   @file ITrackCollectionProvider.h
   @class ITrackCollectionProvider
   
   @brief Interface for tool used to get a collection of tracks.
   
   @author Robert Harrington <roberth@bu.edu>
   @date 7/29/2010
*/

	
class TFile;


namespace Trk { 
  
  static const InterfaceID 
    IID_TRK_ITrackCollectionProvider("ITrackCollectionProvider",1,0);
  
  class ITrackCollectionProvider: virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID();

    virtual StatusCode trackCollection(const TrackCollection*& tracks) = 0;

    /** sets the output stream for the logfile */
    virtual void setLogStream(std::ostream * os) { m_logStream = os; }

    /** sets ntuple */
    virtual void setNtuple(TFile*) {}

    /** writes tree to ntuple */
    virtual StatusCode fillNtuple() { return StatusCode::SUCCESS; }


    /** Print statistical summary to logfile. */
    virtual void printSummary() {}

  protected:
    std::ostream * m_logStream; //!< logfile output stream
    
  }; // end of class definition

  inline const InterfaceID& ITrackCollectionProvider::interfaceID() {
    return   IID_TRK_ITrackCollectionProvider;
  }

} // end of namespace


#endif // TRKALIGNINTERFACES_TRACKCOLLECTIONPROVIDER_IH
