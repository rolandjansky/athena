/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibData/MdtCalibrationFactory.h"  
#include "MdtCalibData/MdtBackgroundCorFunc0.h"
#include "MdtCalibData/MdtBFieldCorFunc0.h"
#include "MdtCalibData/MdtSlewCorFunc0.h"
#include "MdtCalibData/MdtTempCorFunc0.h"
#include "MdtCalibData/MdtWireSagCorFunc0.h"
#include "MdtCalibData/RtRelationLookUp.h"
#include "MdtCalibData/RtResolutionLookUp.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>

namespace MuonCalib{

IMdtBackgroundCorFunc *MdtCalibrationFactory::createBackgroundCorFunc(std::string name, const CalibFunc::ParVec& pars) {
  if( name == "MdtBackgroundCorFunc0" ) {
    return new MdtBackgroundCorFunc0(pars);
  }else{
    MsgStream log(Athena::getMessageSvc(),"MdtCalibrationFactory");
    log<<MSG::WARNING<<"createBackgroundCorFunc: <wrong name>"<< name<<endmsg;
    return nullptr;
  }
}

IMdtBFieldCorFunc *MdtCalibrationFactory::createBFieldCorFunc( std::string name, const CalibFunc::ParVec& pars) {
  if( name == "MdtBFieldCorFunc0" ) {
    return new MdtBFieldCorFunc0(pars);
  }else{
    MsgStream log(Athena::getMessageSvc(),"MdtCalibrationFactory");
    log<<MSG::WARNING<<"createBFieldCorFunc: <wrong name>"<< name<<endmsg;
    return nullptr;
  }
}
  
IMdtSlewCorFunc *MdtCalibrationFactory::createMdtSlewCorFunc( std::string name, const CalibFunc::ParVec& pars) {
  if( name == "MdtSlewCorFunc0" ) {
    return new MdtSlewCorFunc0(pars);
  }else{
    MsgStream log(Athena::getMessageSvc(),"MdtCalibrationFactory");
    log<<MSG::WARNING<<"createMdtSlewCorFunc: <wrong name>"<< name<<endmsg;
    return nullptr;
  }
}
 
IMdtTempCorFunc *MdtCalibrationFactory::createMdtTempCorFunc( std::string name, const CalibFunc::ParVec& pars) {
  if( name == "MdtTempCorFunc0" ) {
    return new MdtTempCorFunc0(pars);
  }else{
    MsgStream log(Athena::getMessageSvc(),"MdtCalibrationFactory");
    log<<MSG::WARNING<<"createMdtTempCorFunc: <wrong name>"<< name<<endmsg;
    return nullptr;
  }
}

IMdtWireSagCorFunc* MdtCalibrationFactory::createMdtWireSagCorFunc( std::string name, const CalibFunc::ParVec& pars) {
  if( name == "MdtWireSagCorFunc0" ) {
    return new MdtWireSagCorFunc0(pars);
  }else{
    MsgStream log(Athena::getMessageSvc(),"MdtCalibrationFactory");
    log<<MSG::WARNING<<"createMdtWireSagCorFunc: <wrong name>"<< name<<endmsg;
    return nullptr;
  }
}
  
IRtRelation* MdtCalibrationFactory::createRtRelation( std::string name, const CalibFunc::ParVec& pars) {
  if( name == "RtRelationLookUp" ) {
    return new RtRelationLookUp(pars);
  }else{
    MsgStream log(Athena::getMessageSvc(),"MdtCalibrationFactory");
    log<<MSG::WARNING<<"createRtRelation: <wrong name>"<< name<<endmsg;
    return nullptr;
  }
}

IRtResolution* MdtCalibrationFactory::createRtResolution( std::string name, const CalibFunc::ParVec& pars) {
  if( name == "RtResolutionLookUp" ) {
    return new RtResolutionLookUp(pars);
  }else{
    MsgStream log(Athena::getMessageSvc(),"MdtCalibrationFactory");
    log<<MSG::WARNING<<"createRtResolution: <wrong name>"<< name<<endmsg;
    return nullptr;
  }
}

}  //namespace MuonCalib
