// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
  * @file L1CaloD3PDMaker/L1CaloPPMdbDataFillerTool.h
  * @author John Morris <john.morris@cern.ch>
  * @brief Block filler tool for L1Calo PPM Database information.
  */

#ifndef _L1CALOD3PDMAKER_L1CALOPPMDBDATAFILLERTOOL_H_
#define _L1CALOD3PDMAKER_L1CALOPPMDBDATAFILLERTOOL_H_

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/VoidBlockFillerTool.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TileEvent/TileTTL1CellContainer.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"
#include "LArRecUtils/LArHVCorrTool.h"

#include <vector>

namespace D3PD{

  class L1CaloPPMdbDataFillerTool : public BlockFillerTool<void>
  {
  public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    L1CaloPPMdbDataFillerTool (const std::string& type,
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
    ToolHandle<ILArHVCorrTool> m_LArHVCorrTool;      
    
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
    std::vector<double>*                         m_DacOffset;
    std::vector<double>*                         m_DacSlope;
    std::vector<double>*                         m_PedMean;
    std::vector<int>*                            m_PedValue;
    std::vector<int>*                            m_FullDelayData;
    std::vector<int>*                            m_SatBcidThreshLow;
    std::vector<int>*                            m_SatBcidThreshHigh;
    std::vector<int>*                            m_SatBcidLevel;
    std::vector<int>*                            m_BcidEnergyRangeLow;
    std::vector<int>*                            m_BcidEnergyRangeHigh;
    std::vector<int>*                            m_FirStartBit;
    std::vector<int>*                            m_FirCoeff1;
    std::vector<int>*                            m_FirCoeff2;
    std::vector<int>*                            m_FirCoeff3;
    std::vector<int>*                            m_FirCoeff4;
    std::vector<int>*                            m_FirCoeff5;
    std::vector<int>*                            m_LutStrategy;
    std::vector<int>*                            m_LutOffset;
    std::vector<int>*                            m_LutNoiseCut;
    std::vector<int>*                            m_LutSlope;
    std::vector<std::vector<float> >*            m_RxGain;
    std::vector<std::vector<int> >*              m_RxStatus;
    std::vector<std::vector<unsigned int> >*     m_RxId;
    std::vector<int>*                            m_isTile;
    std::vector<int>*                            m_nCells;
    std::vector<std::vector<int> >*              m_nCellsByLayer;
    std::vector<std::vector<int> >*              m_cellsLayerNames;
    std::vector<float>*                          m_nCellsNonNominal;
    std::vector<std::vector<float> >*            m_nCellsNonNominalByLayer;
    std::vector<float>*                          m_nonNominalMeanScale;
    std::vector<std::vector<float> >*            m_nonNominalMeanScaleByLayer;
    std::vector<std::vector<float> >*            m_nonNominalMeanScaleByReceiver;    
    
  };
}
#endif
