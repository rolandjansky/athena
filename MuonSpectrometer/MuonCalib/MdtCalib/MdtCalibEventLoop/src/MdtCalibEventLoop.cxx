/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibEventLoop/MdtCalibEventLoop.h"

#include "MdtCalibInterfaces/IMdtCalibrationOutput.h"

#include "MdtCalibInterfaces/IMdtCalibration.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"

#include <iostream>
#include <algorithm>

namespace MuonCalib {

  MdtCalibEventLoop::MdtCalibEventLoop(std::string regionKey) 
    : m_calibrationImp(NULL),
    m_calibrationResult(NULL),
    m_regionKey(regionKey)
  {
    m_printLevel = 1;
  }

  MdtCalibEventLoop::~MdtCalibEventLoop()
  { 
    MuonSegIt it = m_segments.begin();
    MuonSegIt it_end = m_segments.end();
    for( ;it!=it_end;++it) delete *it;
  }

  void MdtCalibEventLoop::performAnalysis()
  {
    if( m_printLevel >= 1 ) 
      std::cout << "MdtCalibEventLoop::performAnalysis for region " << m_regionKey << std::endl;

    // perform rt-calibration 
    m_calibrationResult = m_calibrationImp->analyseSegments( m_segments );

    if( m_printLevel >= 2 ) 
      std::cout << "new rtOutput " << m_calibrationResult << std::endl;

  
  }


  bool MdtCalibEventLoop::handleSegment( const MuonCalibSegment* cseg )
  {
    if( m_printLevel >= 4 ) 
      std::cout << "MdtCalibEventLoop::handleSegment " << cseg << std::endl;

    if( cseg ){
      // eventloop owns the segments so it should make a copy
      MuonCalibSegment* seg = new MuonCalibSegment(*cseg);

      // add segment to vector
      m_segments.push_back(seg);

    }else{
      std::cout << "MdtCalibEventLoop::handleSegment ERROR got NULL pointer " << std::endl;
      return false;
    }
    return true;
  }

  void MdtCalibEventLoop::setCalibImp( IMdtCalibration* calibImp)
  {
    if( m_printLevel >= 2 ) 
      std::cout << "MdtCalibEventLoop::setRtCalibTool >> new tool " 
		<< calibImp << std::endl;
    if( calibImp ){
      m_calibrationImp = calibImp;
    }else{
      std::cout << "MdtCalibEventLoop::setCalibImp ERROR got NULL pointer " << std::endl;
      m_calibrationImp = 0;
    }
  }

}
