/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MDTCALIBNTUPLEMAKER_H
#define MUONCALIB_MDTCALIBNTUPLEMAKER_H

#include "MdtCalibInterfaces/IMdtCalibration.h"
#include "MuonCalibNtuple/MuonCalibBranchNtuple.h"


namespace MuonCalib {

  class IMdtCalibrationOutput;
  class MuonCalibSegment;
  /**
     @class MdtCalibNtupleMaker
     Implementation of a rt calibration to produce an ntuple per region
   
     @author Niels.Van.Eldik@cern.ch
  */
  class MdtCalibNtupleMaker : public IMdtCalibration {
  public:
    MdtCalibNtupleMaker( std::string name );  //!< constructor
    const IMdtCalibrationOutput* analyseSegments( const IMdtCalibration::MuonSegVec &segs ); //!< Loops over segmentvector and calls handleSegment
    void  setInput( const IMdtCalibrationOutput* /*rt_in*/ ); //!< empty routine
    bool  analyse();                                          //!< writes tree
    bool  converged() const;                                  //!< always returning true
    const IMdtCalibrationOutput* getResults() const;          //!< always returning 0.
    void setPrintLevel(int level) { m_printLevel = level; }   //!< set print level 

  private:
    bool  handleSegment( MuonCalibSegment &seg );             //!< Calls MuonCalibBranchNtuple::handleSegment().
    int m_printLevel;                                         //!< printlevel of output

    MuonCalibBranchNtuple m_ntMaker;                          //!< Ntuple maker
  };

}  //namespace MuonCalib

#endif  
 
