/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  ***************************************************************************
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *

//
// Mapping tool for Fcal 23 calo cells to receivers
//

#ifndef TRIGT1CALOCALIBTOOLS_L1CALOFCAL23CELLS2RXMAPPINGTOOL_H
#define TRIGT1CALOCALIBTOOLS_L1CALOFCAL23CELLS2RXMAPPINGTOOL_H

#include <unordered_map>
#include <vector>

#include "AsgTools/AsgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigT1CaloCalibToolInterfaces/IL1CaloFcal23Cells2RxMappingTool.h"

namespace LVL1 {

class L1CaloFcal23Cells2RxMappingTool : virtual public IL1CaloFcal23Cells2RxMappingTool, public asg::AsgTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( L1CaloFcal23Cells2RxMappingTool , LVL1::IL1CaloFcal23Cells2RxMappingTool )

public:
  /// constructor
  L1CaloFcal23Cells2RxMappingTool( const std::string& name );
  /// destructor
  virtual ~L1CaloFcal23Cells2RxMappingTool();
  /// delete the big 4
  L1CaloFcal23Cells2RxMappingTool() = delete;
  L1CaloFcal23Cells2RxMappingTool(const L1CaloFcal23Cells2RxMappingTool& rhs) = delete;
  L1CaloFcal23Cells2RxMappingTool(L1CaloFcal23Cells2RxMappingTool&& rhs) = delete;
  L1CaloFcal23Cells2RxMappingTool& operator=(const L1CaloFcal23Cells2RxMappingTool& rhs) = delete;


  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual unsigned int offlineCell2RxId(const unsigned int& cellId) const override;
  virtual unsigned int onlineCell2RxId(const unsigned int& cellId) const override;

  enum mapType {
    mapOfflineCell2RxId,
    mapOnlineCell2RxId,
    mapOfflineCellNotConnectedToCalibLines,
    mapOnlineCellNotConnectedToCalibLines,
    numMaps
  };


private:
      unsigned int lookup (mapType type, unsigned int cellId) const;
      std::unordered_map<unsigned int,unsigned int> m_maps[numMaps];
};
} // end of namespace
#endif
