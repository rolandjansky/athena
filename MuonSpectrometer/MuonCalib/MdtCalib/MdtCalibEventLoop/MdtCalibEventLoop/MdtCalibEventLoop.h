/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MdtCalibEventLoop.h
//   Header file for class MdtCalibEventLoop
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_MDTCALIBEVENTLOOP_H
#define MUONCALIB_MDTCALIBEVENTLOOP_H

#include <vector>
#include <iostream>
#include <string>

namespace MuonCalib {
  /**
     @class MdtCalibEventLoop
     Class collects segments for a given chamber and performs several loops
     over the sample until the calibration algorithm thinks it converged

     @author Niels.Van.Eldik@cern.ch
  */


  class IMdtCalibration;
  class IMdtCalibrationOutput;
  class MuonCalibSegment;

  class MdtCalibEventLoop {
  public:
    typedef std::vector<MuonCalibSegment*>                MuonSegVec;
    typedef std::vector<MuonCalibSegment*>::iterator      MuonSegIt;
    typedef std::vector<MuonCalibSegment*>::const_iterator MuonSegCit;

  public:
    /** constructor */
    MdtCalibEventLoop(std::string regionKey);

    /** destructor */
    ~MdtCalibEventLoop();

    /** handle segment (for now store) */
    bool  handleSegment( const MuonCalibSegment* seg);

    /** analyse segments using IRtCalibration */
    void  performAnalysis();

    /** set pointer to RtCalibration */
    void  setCalibImp( IMdtCalibration* calibImp);

    /** return results rt calibration */
    const IMdtCalibrationOutput* calibrationResults() const;

    /** set maximum number of iteration (default = 10) */
    void setMaxIterations(unsigned int max);

    /** set print level */
    void setPrintLevel(int level) { m_printLevel = level; }

  private:
  
    IMdtCalibration*           m_calibrationImp;      //!< pointer to calibration implementation
    MuonSegVec                  m_segments;            //!< segment collection

    const IMdtCalibrationOutput* m_calibrationResult; //!< results calibration

    /** print level */
    int m_printLevel;

    /** region key */
    std::string m_regionKey;
  };

  inline 
    const IMdtCalibrationOutput* MdtCalibEventLoop::calibrationResults() const { return m_calibrationResult; }

}

#endif
