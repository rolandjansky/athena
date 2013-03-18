// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
  * @file L1CaloD3PDMaker/L1CaloPPMdbMCFillerTool.h
  * @author John Morris <john.morris@cern.ch>
  * @brief Block filler tool for L1Calo PPM Database information.
  */

#ifndef _L1CALOD3PDMAKER_L1CALOPPMDBFILLERTOOL_H_
#define _L1CALOD3PDMAKER_L1CALOPPMDBFILLERTOOL_H_

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

namespace D3PD{

  class L1CaloPPMdbMCFillerTool : public BlockFillerTool<void>
  {
  public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    L1CaloPPMdbMCFillerTool (const std::string& type,
                           const std::string& name,
                           const IInterface* parent);

    /// Standard Gaudi initialize method.
    virtual StatusCode initialize();

    virtual StatusCode book();
    virtual StatusCode fill();
    
  protected:
    
    float AtlasPhi(const float& L1CaloPhi);      
    
  private:
    ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools> m_tools;  
    
    std::string m_triggerTowerLocation;
    std::string m_caloCellsLocation;
    std::string m_larDigitLocation;
    std::string m_tileDigitsLocation;
    
    std::string m_dbPpmChanCalib;
    std::string m_dbRxGains;
    std::string m_dbRxStatus;
    
    bool m_firstEvent;
    
    std::vector<double>*                         m_eta;
    std::vector<double>*                         m_l1caloPhi;
    std::vector<double>*                         m_atlasPhi;
    
    std::vector<unsigned int>*                   m_coolId;

    std::vector<int>*                            m_isTile;
    std::vector<int>*                            m_nCells;    
    std::vector<std::vector<int> >*              m_cellsLayerNames;
    std::vector<std::vector<int> >*              m_nCellsByLayer;  
  };
}
#endif
