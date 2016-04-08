/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IMdtCalibration.h
//   Header file for class IMdtCalibration
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_IMDTCALIBRATION_H
#define MUONCALIB_IMDTCALIBRATION_H

#include <string>
#include <vector>

namespace MuonCalib {

  class IMdtCalibrationOutput;
  class MuonCalibSegment;

  /**
     @class IMdtCalibration
     Interface to Mdt calibration algortihms.
   
     @author Niels.Van.Eldik@cern.ch
  */

  class IMdtCalibration {
  public:
    typedef std::vector<MuonCalibSegment*>                MuonSegVec;
    typedef std::vector<MuonCalibSegment*>::iterator      MuonSegIt;
    typedef std::vector<MuonCalibSegment*>::const_iterator MuonSegCit;

  public:
    /** constructor, string used to identify the instance */
    IMdtCalibration(std::string name) : m_name(name) {}

    /** destructor */
    virtual ~IMdtCalibration() {}

    /** initialize algoritm */
    virtual void  setInput( const IMdtCalibrationOutput* input) = 0;

    /** final analyse results */
    virtual const IMdtCalibrationOutput* analyseSegments( const MuonSegVec& segs ) = 0;

    /** retrieve results from calibration */
    virtual const IMdtCalibrationOutput* getResults() const = 0;

    /** returns name (region) of instance */
    virtual const std::string& name() { return m_name; }

  private:
    std::string m_name;
  };

}

#endif
