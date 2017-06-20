/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCALIBUTILS_LArFill_H
#define LARCALIBUTILS_LArFill_H

/********************************************************************

NAME:     LArFill.h (to test classes of the current package)
AUTHORS:  S. Laplace
CREATED:  Jan. 2004

********************************************************************/

#include "AthenaBaseComps/AthAlgorithm.h" 

#include "LArIdentifier/LArOnlineID.h"
#include "LArCabling/LArCablingService.h"

#include "LArRawConditions/LArRampComplete.h"
#include "LArRawConditions/LArDAC2uAComplete.h"
#include "LArRawConditions/LAruA2MeVComplete.h"

#include "LArElecCalib/ILArRamp.h"
#include "LArElecCalib/ILArDAC2uA.h" 
#include "LArElecCalib/ILAruA2MeV.h" 

#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IOVSvcDefs.h" 

#include "LArElecCalib/ILArADC2MeVTool.h"

class LArFill : public AthAlgorithm
{

 public:

  LArFill(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArFill();

  StatusCode initialize();
  StatusCode execute();
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

 private:

  //  const LArEM_ID*           em_id;
  const LArOnlineID*        m_lar_on_id;

  LArRampComplete*   m_adc2dac;
  LArDAC2uAComplete* m_dac2ua; 
  LAruA2MeVComplete* m_ua2mev; 

  const DataHandle<ILArRamp>   m_dd_ADC2DAC; 
  const DataHandle<ILArDAC2uA> m_dd_DAC2uA; 
  const DataHandle<ILAruA2MeV> m_dd_uA2MeV; 

  std::string m_ADC2MeVToolName; 
  std::string m_keyADC2DAC, m_keyDAC2uA, m_keyuA2MeV; 
  ILArADC2MeVTool* m_ADC2MeVtool;

};
#endif
