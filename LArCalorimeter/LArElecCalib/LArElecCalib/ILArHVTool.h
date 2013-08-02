/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef ILARHVTOOL_H
#define ILARHVTOOL_H

// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include <vector>

static const InterfaceID IID_ILArHVTool("ILArHVTool", 1 ,0);


/**
 @class ILArHVTool 
 @brief Interface for Tool to provide HV information for readout cells.

 Environment:
     Software developed for the ATLAS Detector at the CERN LHC

 @author 
      Hong Ma <hma@bnl.gov>
-----------------------------------------------------------------------
*/

class Identifier;
class HWIdentifier;

class ILArHVTool: virtual public IAlgTool
{
 
  public: 
  virtual ~ILArHVTool () { } 

  static const InterfaceID& interfaceID() { return IID_ILArHVTool; }

  /** @brief struct for HV and weight pair 
   */
  struct HV_t {
    float hv;
    float weight; 
  }; 

  /** @brief  struct for Current and weight pair 
   */
  struct CURRENT_t {
    float current;
    float weight; 
  }; 


  /** brief  Given a Offline Readout ID, return values of HV and Weight 
   */
  virtual StatusCode getHV(const Identifier& id,
	 std::vector< HV_t > & v  ) =0 ; 

  /**  Given a Offline Readout ID, return values of Current and Weight 
   */
  virtual StatusCode getCurrent(const Identifier& id,
	 std::vector< CURRENT_t > & v  ) =0 ; 

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) = 0;


  /** Return the list of electrodes with HV updated during the last callback
   */
  virtual const std::vector<HWIdentifier>& getUpdatedElectrodes() = 0;

};

#endif
