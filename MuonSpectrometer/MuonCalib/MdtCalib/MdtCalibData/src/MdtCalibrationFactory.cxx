/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibData/MdtCalibrationFactory.h"  
#include "MdtCalibData/MdtBackgroundCorFunc0.h"
#include "MdtCalibData/MdtBFieldCorFunc0.h"
#include "MdtCalibData/MdtSlewCorFunc0.h"
#include "MdtCalibData/MdtTempCorFunc0.h"
#include "MdtCalibData/MdtWireSagCorFunc0.h"
#include "MdtCalibData/RtRelationLookUp.h"
#include "MdtCalibData/RtResolutionLookUp.h"

#include <iostream>

namespace MuonCalib{

IMdtBackgroundCorFunc *MdtCalibrationFactory::createBackgroundCorFunc( std::string name, 
					      const CalibFunc::ParVec& pars) {
  if( name == "MdtBackgroundCorFunc0" ) {
    return new MdtBackgroundCorFunc0(pars);
  }else{
    std::cout << "MdtCalibrationFactory::createBackgroundCorFunc: <wrong name> " 
	      << name << std::endl;
    return 0;
  }
}

IMdtBFieldCorFunc *MdtCalibrationFactory::createBFieldCorFunc( std::string name, 
				      const CalibFunc::ParVec& pars) {
  if( name == "MdtBFieldCorFunc0" ) {
    return new MdtBFieldCorFunc0(pars);
  }else{
    std::cout << "MdtCalibrationFactory::createBFieldCorFunc: <wrong name> " 
	      << name << std::endl;
    return 0;
  }
}
  
IMdtSlewCorFunc *MdtCalibrationFactory::createMdtSlewCorFunc( std::string name, 
				       const CalibFunc::ParVec& pars) {
  if( name == "MdtSlewCorFunc0" ) {
    return new MdtSlewCorFunc0(pars);
  }else{
    std::cout << "MdtCalibrationFactory::createMdtSlewCorFunc: <wrong name> " 
	      << name << std::endl;
    return 0;
  }
}
 
IMdtTempCorFunc *MdtCalibrationFactory::createMdtTempCorFunc( std::string name, 
				       const CalibFunc::ParVec& pars) {
  if( name == "MdtTempCorFunc0" ) {
    return new MdtTempCorFunc0(pars);
  }else{
    std::cout << "MdtCalibrationFactory::createMdtTempCorFunc: <wrong name> " 
	      << name << std::endl;
    return 0;
  }
}

IMdtWireSagCorFunc* MdtCalibrationFactory::createMdtWireSagCorFunc( std::string name, 
				  const CalibFunc::ParVec& pars) {
  if( name == "MdtWireSagCorFunc0" ) {
    return new MdtWireSagCorFunc0(pars);
  }else{
    std::cout << "MdtCalibrationFactory::createMdtWireSagCorFunc: <wrong name> " 
	      << name << std::endl;
    return 0;
  }
}
  
IRtRelation* MdtCalibrationFactory::createRtRelation( std::string name, 
				   const CalibFunc::ParVec& pars) {
  if( name == "RtRelationLookUp" ) {
    return new RtRelationLookUp(pars);
  }else{
    std::cout << "MdtCalibrationFactory::createRtRelation: <wrong name> " 
	      << name << std::endl;
    return 0;
  }
}

IRtResolution* MdtCalibrationFactory::createRtResolution( std::string name, 
				     const CalibFunc::ParVec& pars) {
  if( name == "RtResolutionLookUp" ) {
    return new RtResolutionLookUp(pars);
  }else{
    std::cout << "MdtCalibrationFactory::createRtResolution: <wrong name> " 
	      << name << std::endl;
    return 0;
  }
}

}  //namespace MuonCalib
