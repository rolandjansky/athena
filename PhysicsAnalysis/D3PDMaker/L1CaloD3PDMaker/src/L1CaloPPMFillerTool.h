// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
  * @file L1CaloD3PDMaker/L1CaloPPMFillerTool.h
  * @author John Morris <john.morris@cern.ch>
  * @date August 2010
  * @brief Block filler tool for L1Calo PPM information.
  */

#ifndef _L1CALOD3PDMAKER_L1CALOPPMFILLERTOOL_H_
#define _L1CALOD3PDMAKER_L1CALOPPMFILLERTOOL_H_

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
#include <TRandom3.h>

namespace D3PD{

  class L1CaloPPMFillerTool : public BlockFillerTool<void>
  {
  public:
       /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    L1CaloPPMFillerTool (const std::string& type,
			 const std::string& name,
			 const IInterface* parent);

    /// Standard Gaudi initialize method.
    virtual StatusCode initialize();

    /// Standard Gaudi finalize method.
    virtual StatusCode finalize();

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
    std::string m_dbPpmDeadChannels;

    bool m_isCalibration;
    double m_minCaloCellEnergy;
    int m_minADC;
    bool m_useRandom;
    double m_minRandom;
    bool m_isMC;
    int m_toolsMode;
   
    TRandom3* m_random;

    std::vector<unsigned int>*          m_em_coolId;
    std::vector<float>*                 m_em_ttCellsEnergy;
    std::vector<float>*                 m_em_ttCellsEt;
    std::vector<std::vector<float> >*   m_em_ttCellsEnergyByLayer;
    std::vector<std::vector<float> >*   m_em_ttCellsEtByLayer;
    std::vector<std::vector<float> >*   m_em_ttCellsEnergyByReceiver;
    std::vector<std::vector<float> >*   m_em_ttCellsEtByReceiver;    
    std::vector<int>*                   m_em_Peak;
    std::vector<int>*                   m_em_ADCPeak;
    std::vector< std::vector<int> >*    m_em_LUT;      
    std::vector< std::vector<int> >*    m_em_ADC;
    std::vector<int>*                   m_em_Energy;
    std::vector< std::vector<int> >*    m_em_BCIDvec;
    std::vector< std::vector<int> >*    m_em_BCIDext;
    std::vector<int>*                   m_em_Error;      
    std::vector<int>*                   m_em_BCID;
    std::vector<int>*                   m_em_IsSaturated;
    std::vector<int>*                   m_em_disabledTower;
    std::vector<int>*                   m_em_deadChannel;
    std::vector<int>*                   m_em_badCalo;  
    std::vector<float>*                 m_em_caloQuality;
    std::vector<float>*                 m_em_larTowerEnergy;

    std::vector<unsigned int>*          m_had_coolId;
    std::vector<float>*                 m_had_ttCellsEnergy;
    std::vector<float>*                 m_had_ttCellsEt;
    std::vector<float>*                 m_had_TileCellsEnergy;
    std::vector<std::vector<float> >*   m_had_ttCellsEnergyByLayer;
    std::vector<std::vector<float> >*   m_had_ttCellsEtByLayer;
    std::vector<std::vector<float> >*   m_had_ttCellsEnergyByReceiver;
    std::vector<std::vector<float> >*   m_had_ttCellsEtByReceiver;
    std::vector<int>*                   m_had_Peak;
    std::vector<int>*                   m_had_ADCPeak;
    std::vector< std::vector<int> >*    m_had_LUT;
    std::vector< std::vector<int> >*    m_had_ADC;
    std::vector<int>*                   m_had_Energy;
    std::vector< std::vector<int> >*    m_had_BCIDvec;
    std::vector< std::vector<int> >*    m_had_BCIDext;
    std::vector<int>*                   m_had_Error; 
    std::vector<int>*                   m_had_BCID;
    std::vector<int>*                   m_had_IsSaturated;
    std::vector<int>*                   m_had_disabledTower;
    std::vector<int>*                   m_had_deadChannel;
    std::vector<int>*                   m_had_badCalo;
    std::vector<float>*                 m_had_caloQuality;
    std::vector<float>*                 m_had_larTowerEnergy;
  

    // precache TTL1Cell <-> TT online ID
    typedef std::map<Identifier, const TileTTL1Cell*> IdTTL1CellMapType;
    IdTTL1CellMapType m_idTTL1CellMap; 

  };

} // namespace D3PD

#endif
