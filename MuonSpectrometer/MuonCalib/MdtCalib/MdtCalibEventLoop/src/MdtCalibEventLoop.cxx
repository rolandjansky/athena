/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibEventLoop/MdtCalibEventLoop.h"
#include "MdtCalibInterfaces/IMdtCalibrationOutput.h"
#include "MdtCalibInterfaces/IMdtCalibration.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>
#include <algorithm>

namespace MuonCalib {

  MdtCalibEventLoop::MdtCalibEventLoop(std::string regionKey) :
    m_calibrationImp(nullptr),
    m_calibrationResult(nullptr),
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
    if( m_printLevel >= 1 ) {
      MsgStream log(Athena::getMessageSvc(),"MdtCalibEventLoop");
      log<<MSG::VERBOSE<<"MdtCalibEventLoop::performAnalysis for region " << m_regionKey<<endmsg;
    }

    // perform rt-calibration 
    m_calibrationResult = m_calibrationImp->analyseSegments( m_segments );

    if( m_printLevel >= 2 ) {
      MsgStream log(Athena::getMessageSvc(),"MdtCalibEventLoop");
      log<<MSG::DEBUG<<"new rtOutput " << m_calibrationResult<<endmsg;
    }

  
  }


  bool MdtCalibEventLoop::handleSegment( const MuonCalibSegment* cseg )
  {
    if( m_printLevel >= 4 ) {
      MsgStream log(Athena::getMessageSvc(),"MdtCalibEventLoop");
      log<<MSG::WARNING<<"MdtCalibEventLoop::handleSegment " << cseg<<endmsg;
    }

    if( cseg ){
      // eventloop owns the segments so it should make a copy
      MuonCalibSegment* seg = new MuonCalibSegment(*cseg);

      // add segment to vector
      m_segments.push_back(seg);

    }else{
      MsgStream log(Athena::getMessageSvc(),"MdtCalibEventLoop");
      log<<MSG::WARNING<<"MdtCalibEventLoop::handleSegment: got nullptr "<<endmsg;
      return false;
    }
    return true;
  }

  void MdtCalibEventLoop::setCalibImp( IMdtCalibration* calibImp)
  {
    if( m_printLevel >= 2 ) {
      MsgStream log(Athena::getMessageSvc(),"MdtCalibEventLoop");
      log<<MSG::DEBUG<<"MdtCalibEventLoop::setRtCalibTool >> new tool "<<calibImp<<endmsg;
    }
    if( calibImp ){
      m_calibrationImp = calibImp;
    }else{
      MsgStream log(Athena::getMessageSvc(),"MdtCalibEventLoop");
      log<<MSG::WARNING<<"MdtCalibEventLoop::setCalibImp: got nullptr "<<endmsg;
      m_calibrationImp = 0;
    }
  }

}
