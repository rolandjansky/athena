/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimplePatternSelector.h
//   Header file for class SimplePatternSelector
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_SIMPLEPATTERNSELECTOR_H
#define MUONCALIB_SIMPLEPATTERNSELECTOR_H

#include "MdtCalibInterfaces/ICalibPatternSelector.h"

namespace MuonCalib {

  class MuonCalibPattern;

  /**@class SimplePatternSelector
     Basic Pattern selector. Gets MdtSegments and tests if they satisfy the selection criteria:

     - chi2 

     - munber of mdt segments 

     - number of phi segments

     Returns true if they do.
   
     @author Niels.Van.Eldik@cern.ch
  */

  class SimplePatternSelector : public ICalibPatternSelector{
  public:
    /** constructor */
    SimplePatternSelector(double chi2_max, unsigned int mdtSeg_min,
			  unsigned int phiSeg_min);
  
    /** test MdtSegment on selection criteria */
    virtual bool   select(const MuonCalibPattern& seg) const;

    /** set print level */
    void setPrintLevel(int level) { m_printLevel = level; }

  private:
    /** selection criteria */
    double m_chi2_max;         //!< maximum chi2 of pattern
    unsigned int m_mdtSeg_min; //!< minimum number of mdt segments on pattern

    /** print level */
    int m_printLevel;
  };

}

#endif
