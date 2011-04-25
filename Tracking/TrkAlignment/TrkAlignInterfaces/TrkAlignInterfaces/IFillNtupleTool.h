/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNINTERFACES_IFILLNTUPLETOOL_IH
#define TRKALIGNINTERFACES_IFILLNTUPLETOOL_IH

#include "GaudiKernel/IAlgTool.h"

/**
   @file IFillNtupleTool.h
   @class IFillNtupleTool
   
   @brief Interface for tool used to fill an ntuple with useful information for 
   debugging, etc.  Implementation is detector-specific and does not have to be 
   implemented if not needed.
   
   @author Robert Harrington <roberth@bu.edu>
   @date 1/5/08
*/

#include <fstream>

static const InterfaceID 
IID_TRKALIGNINTERFACES_IFillNtupleTool("IFillNtupleTool",1,0);

class TFile;

namespace Trk {
  
  class AlignTrack;
  
  class IFillNtupleTool: virtual public IAlgTool {
    
  public:
    virtual ~IFillNtupleTool() {}
    
    static const InterfaceID& interfaceID();
    
    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;

    /** dumps track information to ntuple */
    virtual void dumpTrack(int itrk, const AlignTrack* alignTrack) = 0;
    
    /** stores hitmap for writing to ntuple */
    virtual void storeHitmap() = 0;
    
    /** fills ntuple with hit information */
    virtual void fillHitmap() = 0;
    
    /** fills ntuple with event and track summary information */
    virtual void fillSummary() = 0;
    
    /** write statistics out to log file */
    virtual void showStatistics() = 0;
    
    /** used for numerical derivatives to write chi2 vs. chamber positions */
    virtual void fillChi2VChamberShift(const int, const int, int*,
				       double**,double**,double**,
				       double**,double**,double** ) {}
    
    /** returns pointer to TFile containing ntuple */
    //virtual TFile* ntupleFile() { return 0; }

    /** sets the output stream for the logfile */
    virtual void setLogStream(std::ostream * os) { m_logStream = os; }

    /** sets the output stream for the logfile */
    virtual void setNtuple(TFile* ntuple) = 0;

    /** writes trees and histograms to ntuple */
    virtual StatusCode fillNtuple() = 0;

  protected:
    std::ostream * m_logStream; //!< logfile output stream

  }; // end class definition
  
  inline const InterfaceID& IFillNtupleTool::interfaceID()
  { return   IID_TRKALIGNINTERFACES_IFillNtupleTool; }
  
} // end namespace  

#endif // TRKALIGNINTERFACES_IFILLNTUPLETOOL_IH

