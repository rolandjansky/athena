/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNINTERFACES_IDERIVCALCTOOL_IH
#define TRKALIGNINTERFACES_IDERIVCALCTOOL_IH

/**
   @file IDerivCalcTool.h
   @class IDerivCalcTool
   
   @brief Interface for tool used to select tracks, to determine detectors 
   (ID or MS) for which to perform alignment, and to calculate first and 
   second derivatives of track chi2 with respect to track and alignment 
   parameters.  
   
   Based on InDetAlignAlgs/SiGlobalChi2AlignAlgs.
   
   @author Robert Harrington <roberth@bu.edu>
   @date 1/5/08
*/

#include "GaudiKernel/IAlgTool.h"

#include <fstream>

namespace Trk {

  class Track;
  class AlignTSOS;
  class AlignTrack;

  static const InterfaceID
    IID_TRKALIGNINTERFACES_IDerivCalcTool("IDerivCalcTool",1,0);
  
  class IDerivCalcTool : virtual public IAlgTool {
    
  public:

    /** enum of different solving options */
    enum SolveOption { 
      NONE                 = 0, //!< not solve in any case (to be used when ipc)
      SOLVE                = 1, //!< solving after data accumulation (LAPACK)
      SOLVE_FAST           = 2, //!< Fast (Eigen method) solving after data accumulation
      DIRECT_SOLVE         = 3, //!< direct solving (LAPACK), already available matrix & vector
      DIRECT_SOLVE_FAST    = 4, //!< direct Fast (Eigen method) solving, already available matrix & vector
      DIRECT_SOLVE_CLUSTER = 5  //!< computation of alignment parameters from SCALAPAK already solved matrix
    }; // this is also defined in TrkGlobAlign class

    /** Virtual destructor */
    virtual ~IDerivCalcTool() {}

    /** Retrieve interface ID */
    static const InterfaceID& interfaceID();

    /** solving option (see enum above) */
    void setSolveOption(int solveOption) { m_solveOption=solveOption; }

    /** sets partial derivatives of residuals w.r.t. alignment parameters for all AlignTSOS on alignTrack */
    virtual bool setDerivatives(AlignTrack* alignTrack) = 0;

    /** write statistics to log file */
    virtual void showStatistics() = 0;
    
    //bool storeSuperStructures() { return m_storeSuperStructures; }

    /** sets residual covariance matrix */
    virtual bool setResidualCovMatrix(AlignTrack* alignTrack) const = 0;

    /** sets the output stream for the logfile */
    virtual void setLogStream(std::ostream * os) { m_logStream = os; }

  protected:
    //bool m_storeSuperStructures;  //!< Store Super Structures information in txt file ?

    std::ostream * m_logStream; //!< logfile output stream

  private:
    
    int  m_solveOption;
            
  }; // end of class
  
  inline const InterfaceID& IDerivCalcTool::interfaceID() {
    return IID_TRKALIGNINTERFACES_IDerivCalcTool;
  }

} // end of namespace

  
#endif // TRKALIGNINTERFACES_IDERIVCALCTOOL_IH
