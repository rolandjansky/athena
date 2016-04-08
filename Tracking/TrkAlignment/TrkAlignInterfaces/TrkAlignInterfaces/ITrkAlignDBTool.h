/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNINTERFACES_ALIGNDBTOOL_IH
#define TRKALIGNINTERFACES_ALIGNDBTOOL_IH

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include <fstream>

/**
   @file ITrkAlignDBTool.h
   @class ITrkAlignDBTool
   
   @brief Interface for tool used to manage the retrieval of alignment 
   information from conditions data or other sources, such as temporary ASCII 
   files, and to write constants to same.

   To be implemented for each detector-type separately.

   @author Robert Harrington <roberth@bu.edu>
   @date 1/5/08
*/

class TFile;

namespace Trk {

  static const InterfaceID 
    IID_TRKALIGNINTERFACES_ITrkAlignDBTool("ITrkAlignDBTool",1,0);
  
  class ITrkAlignDBTool: virtual public IAlgTool {

  public:
    static const InterfaceID& interfaceID();
    
    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;

    /** writes alignment parameters to output */
    virtual void writeAlignPar() {}

    /** sets ntuple if needed */
    virtual void setNtuple(TFile*) {}

    /** sets the output stream for the logfile */
    virtual void setLogStream(std::ostream * os) { m_logStream = os; }

    /** sets up initial alignment constants before solving */
    virtual StatusCode preSolve() { return StatusCode::SUCCESS; }

  protected:
    std::ostream * m_logStream; //!< logfile output stream

  private:
    bool m_AlignDBToolFormat; //!< text file has AlignDBTool or the GlobalChi2 format
    
  };
  
  inline const InterfaceID& ITrkAlignDBTool::interfaceID()
  { return   IID_TRKALIGNINTERFACES_ITrkAlignDBTool; }
  
} // end namespace
  
#endif // TRKALIGNINTERFACES_ALIGNDBTOOL_IH
