/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNINTERFACES_ALIGN_TRACK_DRESSER_IH
#define TRKALIGNINTERFACES_ALIGN_TRACK_DRESSER_IH

#include "GaudiKernel/IAlgTool.h"

/**
   @file IAlignTrackDresser.h
   @class IAlignTrackDresser
   
   @brief Interface for tool used to dress an AlignTrack with derivatives 
   and other quantities needed for alignment.
   
   @author Robert Harrington <roberth@bu.edu>
   @date 1/5/08
*/

#include <fstream>

namespace Trk {

  class AlignTrack;

  static const InterfaceID 
    IID_TRK_IAlignTrackDresser("IAlignTrackDresser",1,0);
  
  class IAlignTrackDresser: virtual public IAlgTool {

  public:
    static const InterfaceID& interfaceID();
  
    /** Dresses AlignTrack with derivatives and any other information needed for alignment. */
    virtual bool dressAlignTrack(AlignTrack* alignTrack) const = 0;

     /** sets the output stream for the logfile */
     virtual void setLogStream(std::ostream * os) { m_logStream = os; }

  protected:
    std::ostream * m_logStream; //!< logfile output stream
    
  }; // end class definition
  
  inline const InterfaceID& IAlignTrackDresser::interfaceID()
  { return   IID_TRK_IAlignTrackDresser; }
  
} // end namespace

#endif // TRKALIGNINTERFACES_ALIGN_TRACK_DRESSER_IH

