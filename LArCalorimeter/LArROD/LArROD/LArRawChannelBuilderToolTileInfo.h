/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderToolTileInfo
 * @author Hong Ma
 * @brief Obtain ADC_max for the Tile using Optimal Filtering
 * 
 */

#ifndef LARROD_LARRAWCHANNELBUILDERTOOLTILEINFO_H
#define LARROD_LARRAWCHANNELBUILDERTOOLTILEINFO_H

#include "GaudiKernel/ToolHandle.h"

#include "LArROD/LArRawChannelBuilderToolBase.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "LArRecUtils/LArOFPeakRecoTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <string>

class LArDigit;
class CaloDetDescrManager;

class LArRawChannelBuilderToolTileInfo : public LArRawChannelBuilderToolBase {
  
 public:

  LArRawChannelBuilderToolTileInfo(const std::string& type,
				       const std::string& name,
				       const IInterface* parent);

  bool buildRawChannel(const LArDigit* digit,
		       float pedestal,
		       const std::vector<float>& ramps,
		       MsgStream* pLog);
  
  StatusCode initTool();

 private:
  
  float           m_Ecut;
  bool            m_skipSaturatedCells;
  unsigned short  m_AdcMax;
  bool            m_doQual;
  unsigned int    m_defaultPhase;
  unsigned int    m_minSample;
  unsigned int    m_maxSample;
   
  double m_globaltimeoffsettop, m_globaltimeoffsetbottom;

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{this,"CaloDetDescrManager", "CaloDetDescrManager"};

  //ComTime
  std::string              m_comTimeKey;

  // Peak reco
  ToolHandle<LArOFPeakRecoTool> m_peakReco;
  
  double m_delayTile;

  LArRawChannelBuilderToolTileInfo (const LArRawChannelBuilderToolTileInfo&);
  LArRawChannelBuilderToolTileInfo& operator= (const LArRawChannelBuilderToolTileInfo&);
};

#endif
