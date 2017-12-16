//  ***************************************************************************
//  *                                                                         *
//  *   This program is free software; you can redistribute it and/or modify  *
//  *   it under the terms of the GNU General Public License as published by  *
//  *   the Free Software Foundation; either version 2 of the License, or     *
//  *   (at your option) any later version.                                   *
//  *                                                                         *
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *
//  ***************************************************************************/

//
// Mapping tool for Fcal 23 calo cells to receivers
//

#ifndef TRIGT1CALOCALIBTOOLS_L1CALOFCAL23CELLS2RXMAPPINGTOOL_H
#define TRIGT1CALOCALIBTOOLS_L1CALOFCAL23CELLS2RXMAPPINGTOOL_H

#include <map>
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


  virtual StatusCode initialize();
  virtual StatusCode finalize();

  unsigned int offlineCell2RxId(const unsigned int& cellId) const;
  unsigned int onlineCell2RxId(const unsigned int& cellId) const;

private:
  std::map<unsigned int, unsigned int>* m_mapOfflineCell2RxId;
  std::map<unsigned int, unsigned int>* m_mapOnlineCell2RxId;

  std::map<unsigned int, unsigned int>* m_mapOfflineCellNotConnectedToCalibLines;
  std::map<unsigned int, unsigned int>* m_mapOnlineCellNotConnectedToCalibLines;
};
} // end of namespace
#endif
