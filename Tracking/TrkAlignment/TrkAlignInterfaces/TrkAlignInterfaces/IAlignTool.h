/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNINTERFACES_ALIGNTOOL_IH
#define TRKALIGNINTERFACES_ALIGNTOOL_IH

#include "GaudiKernel/IAlgTool.h"
#include <fstream>

/**
   @file IAlignTool.h
   @class IAlignTool
   
   @brief Interface for tool used to accumulate information for each track, 
   and then to solve for the final alignment parameters.
   
   @author Robert Harrington <roberth@bu.edu>
   @date 1/5/08
*/

class TFile;


namespace Trk { 

  class AlignTrack;

  static const InterfaceID 
    IID_TRK_IAlignTool("IAlignTool",1,0);
  
  class IAlignTool: virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID();

    virtual StatusCode firstEventInitialize() = 0;

    virtual bool accumulate(const AlignTrack* alignTrack) = 0;

    virtual bool accumulateFromFiles() = 0;
    
    virtual StatusCode solve() = 0;    

    /** sets the output stream for the logfile */
    virtual void setLogStream(std::ostream * os) { m_logStream = os; }

    /** sets ntuple */
    virtual void setNtuple(TFile*) {}

    /** writes tree to ntuple */
    virtual StatusCode fillNtuple() { return StatusCode::SUCCESS; }

  protected:
    std::ostream * m_logStream; //!< logfile output stream
    
  }; // end of class definition

  inline const InterfaceID& IAlignTool::interfaceID() {
    return   IID_TRK_IAlignTool;
  }

} // end of namespace


#endif // TRKALIGNINTERFACES_ALIGNTOOL_IH
