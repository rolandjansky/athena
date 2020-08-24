/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/MdtCalibNtupleMaker.h"

#include "MuonCalibEventBase/MuonCalibSegment.h"

#include <iostream>

namespace MuonCalib {

  MdtCalibNtupleMaker::MdtCalibNtupleMaker( std::string name ) :
    IMdtCalibration(name), m_ntMaker(name) {
    m_printLevel = 0;
    m_ntMaker.setPrintLevel(0);
  }

  const IMdtCalibrationOutput* MdtCalibNtupleMaker::analyseSegments( const IMdtCalibration::MuonSegVec &segs ){
    MuonSegCit seg_it = segs.begin();
    MuonSegCit seg_it_end = segs.end();
    for(;seg_it!=seg_it_end;++seg_it){
      handleSegment( **seg_it );
    }
    return getResults() ;
  }

  bool  MdtCalibNtupleMaker::handleSegment( MuonCalibSegment &seg ) {
    m_ntMaker.setEvent(0);
    m_ntMaker.handleSegment(seg);
    m_ntMaker.finishEvent();
    return true;
  }  

  bool  MdtCalibNtupleMaker::analyse() {
    // write ntuple to file
    m_ntMaker.writeTree();

    //  empty routine
    return true;
  }
  
  const IMdtCalibrationOutput* MdtCalibNtupleMaker::getResults() const {
    return 0;
  }

  void MdtCalibNtupleMaker::setInput( const IMdtCalibrationOutput* /*calib_in*/ ) {
    // empty routine
  }

  bool MdtCalibNtupleMaker::converged() const {
    return true;
  }

}  //namespace MuonCalib
