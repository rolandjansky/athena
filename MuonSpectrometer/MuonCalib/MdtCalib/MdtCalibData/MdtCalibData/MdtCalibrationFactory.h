/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MDTCALIBRATIONFACTORY_H
#define MUONCALIB_MDTCALIBRATIONFACTORY_H


#include "MdtCalibData/CalibFunc.h"

#include <string>
#include <vector>


namespace MuonCalib {

  class IMdtBackgroundCorFunc;
  class IMdtBFieldCorFunc;
  class IMdtSlewCorFunc;
  class IMdtTempCorFunc;
  class IMdtWireSagCorFunc;
  class IRtRelation;
  class IRtResolution;

/** @class MdtCalibrationFactory
Factory to create instances of correction functions 
*/
  class MdtCalibrationFactory {
    typedef CalibFunc::ParVec ParVec;
  public:
    static IMdtBackgroundCorFunc* createBackgroundCorFunc( std::string name, 
							   const ParVec& pars);

    static IMdtBFieldCorFunc*     createBFieldCorFunc( std::string name, 
						       const ParVec& pars);

    static IMdtSlewCorFunc*       createMdtSlewCorFunc( std::string name, 
							const ParVec& pars);

    static IMdtTempCorFunc*       createMdtTempCorFunc( std::string name, 
							const ParVec& pars);
  
    static IMdtWireSagCorFunc*    createMdtWireSagCorFunc( std::string name, 
							   const ParVec& pars);
  
    static IRtRelation*           createRtRelation( std::string name, 
						    const ParVec& pars);
  
    static IRtResolution*         createRtResolution( std::string name, 
						      const ParVec& pars);
  
  };

}  //namespace MuonCalib

#endif
