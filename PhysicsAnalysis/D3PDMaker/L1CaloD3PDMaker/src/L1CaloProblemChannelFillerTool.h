// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
  * @file L1CaloD3PDMaker/L1CaloProblemChannelFillerTool.h
  * @author John Morris <john.morris@cern.ch>
  * @date August 2010
  * @brief Block filler tool for L1Calo Problem Channel information.
  */

#ifndef _L1CALOD3PDMAKER_L1CALOPROBLEMCHANNELFILLERTOOL_H_
#define _L1CALOD3PDMAKER_L1CALOPROBLEMCHANNELFILLERTOOL_H_

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/VoidBlockFillerTool.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TileEvent/TileTTL1CellContainer.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"

#include <vector>

// Event info
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

namespace D3PD{

  class L1CaloProblemChannelFillerTool : public BlockFillerTool<void>
  {
  public:
       /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    L1CaloProblemChannelFillerTool (const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent);

    /// Standard Gaudi initialize method.
    virtual StatusCode initialize();

    virtual StatusCode book();
    virtual StatusCode fill();

  protected:
    void preCacheTTL1Cell(const TileTTL1CellContainer* cont);

  private:
    ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools> m_tools;    

    std::string m_triggerTowerLocation;
    std::string m_caloCellsLocation;
    std::string m_larDigitLocation;
    std::string m_tileDigitsLocation;
    std::string m_tileTTL1ContainerName;
    std::string m_dbPpmDisabledTowers;

    int m_currentLumiBlock;    

    int*                         m_runNumber;
    int*                         m_lumiBlock;
    std::vector<unsigned int>*   m_emBadCalo;
    std::vector<unsigned int>*   m_emDisabled;    
    std::vector<unsigned int>*   m_hadBadCalo;
    std::vector<unsigned int>*   m_hadDisabled;

    const EventInfo* m_eventInfo;    

    // precache TTL1Cell <-> TT online ID
    typedef std::map<Identifier, const TileTTL1Cell*> IdTTL1CellMapType;
    IdTTL1CellMapType m_idTTL1CellMap;

  };
}
#endif
