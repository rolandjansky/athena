/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderToolOFC
 * @author Rolf Seuster
 * @brief Obtain ADC_max by optimal filtering
 * 
 */

#ifndef LARROD_LARRAWCHANNELBUILDERTOOLOFC_H
#define LARROD_LARRAWCHANNELBUILDERTOOLOFC_H

#include "GaudiKernel/ToolHandle.h"

#include "LArROD/LArRawChannelBuilderToolBase.h"

#include "LArElecCalib/ILArOFCTool.h"

#include <vector>

class ILArShape;
class LArDigit;

class LArRawChannelBuilderToolOFC : public LArRawChannelBuilderToolBase
{
 public:
  
  LArRawChannelBuilderToolOFC(const std::string& type,
			      const std::string& name,
			      const IInterface* parent);
  
  bool buildRawChannel(const LArDigit* digit,
		       float pedestal,
		       const std::vector<float>& ramps,
		       MsgStream* pLog);
  
  StatusCode initTool();
  
 private:
  const std::vector<float>& OFC_a();
  const std::vector<float>& OFC_b();
  ToolHandle<ILArOFCTool> m_OFCTool;
  
  const ILArShape*       larShape();
  const ILArShape*       m_larShape;
  
  //Algo-properties  //HERE used ??
  float           m_Ecut;
  unsigned int    m_shapeMode;
  bool            m_skipSaturatedCells;
  unsigned short  m_AdcMax;
  
  std::vector<float> m_OFCtmp;
  std::string m_keyShape;
};

#endif
