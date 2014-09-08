/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderPedestalDataBase
 * @author Rolf Seuster
 * @brief Tool obtaining the Pedestal from the database
 */

#ifndef LARROD_LARRAWCHANNELBUILDERPEDESTALDATABASE_H
#define LARROD_LARRAWCHANNELBUILDERPEDESTALDATABASE_H

#include "LArROD/LArRawChannelBuilderPedestalToolBase.h"
#include "StoreGate/DataHandle.h"

#include "CLHEP/Units/SystemOfUnits.h"

class LArRawChannelBuilderPedestalDataBase
: public LArRawChannelBuilderPedestalToolBase
{
 public:
  
  LArRawChannelBuilderPedestalDataBase(const std::string& type,
				       const std::string& name,
				       const IInterface* parent);
  
  bool pedestal(float& pedestal, MsgStream* pLog);
  
  StatusCode initTool();
  
 private:
  
  const DataHandle<ILArPedestal> m_larPedestal;
  
  std::string m_pedestalKey;

  LArRawChannelBuilderPedestalDataBase (const LArRawChannelBuilderPedestalDataBase&);
  LArRawChannelBuilderPedestalDataBase& operator= (const LArRawChannelBuilderPedestalDataBase&);
};

#endif
