/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// ********************************************************************
//
// NAME:     L1CaloMonitoringCaloTool.h
// PACKAGE:  TrigT1CaloCalibTools
//
// AUTHOR:   Peter Faulkner
//
// ********************************************************************
#ifndef TRIGT1CALOCALIBTOOLS_L1CALOMONITORINGCALOTOOL_H
#define TRIGT1CALOCALIBTOOLS_L1CALOMONITORINGCALOTOOL_H

#include <string>
#include <vector>

#include "AsgTools/AsgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "TrigT1CaloCalibToolInterfaces/IL1CaloMonitoringCaloTool.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloMatchCell2Tower.h"
#include "LArCabling/LArOnOffIdMapping.h"

class IInterface;
class StatusCode;

class Identifier;
class CaloLVL1_ID;

namespace LVL1 {

  class L1CaloMonitoringCaloTool: public IL1CaloMonitoringCaloTool, public asg::AsgTool
  {
    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS( L1CaloMonitoringCaloTool , LVL1::IL1CaloMonitoringCaloTool ) 
    
   public:
      /// constructor
    L1CaloMonitoringCaloTool( const std::string& name );
    /// destructor
    virtual ~L1CaloMonitoringCaloTool(){}
    /// delete the big 4
    L1CaloMonitoringCaloTool() = delete;
    L1CaloMonitoringCaloTool(const L1CaloMonitoringCaloTool & rhs) = delete;
    L1CaloMonitoringCaloTool(L1CaloMonitoringCaloTool&& rhs) = delete;
    L1CaloMonitoringCaloTool& operator=(const L1CaloMonitoringCaloTool& rhs) = delete;   

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    StatusCode loadCaloCells();
    float et(const Identifier& ttid) const;
    float caloQuality(const Identifier& ttid) const;

   private:

    int towerIndex(const Identifier& ttId) const;
    int region(const int index) const;
    int etaBin(const int index) const;

    SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey
      {this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

    ToolHandle<LVL1::IL1CaloMatchCell2Tower> m_cellMatch
      { this, "L1CaloMatchCell2Tower",  "LVL1::L1CaloMatchCell2Tower",  "L1CaloMatchCell2Tower" };
    const CaloLVL1_ID* m_lvl1Helper;

    std::string m_caloCellContainerName;

    std::vector<float> m_energySums;
    std::vector<float> m_quality;
    std::vector<float> m_denom;
    std::vector<float> m_sinTh;
    std::vector<unsigned int> m_cellIds;
    std::vector<int> m_ttIdx;
    unsigned int m_maxCells;

    int m_events;
    int m_lastRun;
    int m_lastEvent;
    int m_sideOffset;
    int m_layerOffset;
    std::vector<int> m_binOffset;
    std::vector<int> m_indexOffset;
    std::vector<int> m_etaShift;
  
    static const int s_maxTowers = 7168;
    static const int s_nregions  = 4;
    static const int s_nsinThBins = 33;
  };

} // end namespace

#endif
