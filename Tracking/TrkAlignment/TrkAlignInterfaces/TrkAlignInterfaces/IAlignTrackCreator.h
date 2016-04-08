/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNINTERFACES_ALIGN_TRACK_CREATOR_IH
#define TRKALIGNINTERFACES_ALIGN_TRACK_CREATOR_IH

#include "GaudiKernel/IAlgTool.h"

/**
   @file IAlignTrackCreator.h
   @class IAlignTrackCreator
   
   @brief Interface for tool used to create an AlignTrack.

   @author Robert Harrington <roberth@bu.edu>
   @date 1/5/08
*/

#include <fstream>

namespace Trk {
  
  class AlignTrack;
  class Track;

  static const InterfaceID 
    IID_TRK_IAlignTrackCreator("IAlignTrackCreator",1,0);
  
  class IAlignTrackCreator: virtual public IAlgTool {

  public:    
    IAlignTrackCreator();
    virtual ~IAlignTrackCreator() {}
    
    static const InterfaceID& interfaceID();
    
    /** creates AlignTrack from Trk::Track */
    virtual bool processAlignTrack(AlignTrack* alignTrack) const = 0; 
    
    /** Sets flags to indicate whether hits from particular detector elements are to be stored on the AlignTrack.  (Not used at the moment.) */
    void setDoPixel(bool doPixel) { m_doPixel = doPixel; }
    void setDoSCT(bool doSCT) { m_doSCT = doSCT; }
    void setDoTRT(bool doTRT) { m_doTRT = doTRT; }
    void setDoMDT(bool doMDT) { m_doMDT = doMDT; }

    /** sets the output stream for the logfile */
    virtual void setLogStream(std::ostream * os) { m_logStream = os; }

  protected:

    bool m_doPixel;
    bool m_doSCT;
    bool m_doTRT;
    bool m_doMDT;

    std::ostream * m_logStream; //!< logfile output stream
    
  }; // end class definition
  
  inline IAlignTrackCreator::IAlignTrackCreator() : m_doPixel(1), m_doSCT(1), m_doTRT(1), m_doMDT(1), m_logStream(0){}
  inline const InterfaceID& IAlignTrackCreator::interfaceID()
  { return   IID_TRK_IAlignTrackCreator; }
  
} // end namespace

#endif // TRKALIGNINTERFACES_ALIGN_TRACK_CREATOR_IH
