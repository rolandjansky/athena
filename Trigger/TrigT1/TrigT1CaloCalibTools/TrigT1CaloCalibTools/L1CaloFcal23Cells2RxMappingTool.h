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

#ifndef _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLS_L1CALOFCAL23CELLS2RXMAPPINGTOOL_H_
#define _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLS_L1CALOFCAL23CELLS2RXMAPPINGTOOL_H_

#include <map>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigT1CaloCalibToolInterfaces/IL1CaloFcal23Cells2RxMappingTool.h"

namespace LVL1{

  class L1CaloFcal23Cells2RxMappingTool : virtual public IL1CaloFcal23Cells2RxMappingTool,public AthAlgTool{
    public:
      L1CaloFcal23Cells2RxMappingTool(const std::string& type, const std::string& name, const IInterface* parent);
      virtual ~L1CaloFcal23Cells2RxMappingTool();

      virtual StatusCode initialize();
      virtual StatusCode finalize();

      unsigned int offlineCell2RxId(const unsigned int& cellId) const;
      unsigned int onlineCell2RxId(const unsigned int& cellId) const;

    private:
      L1CaloFcal23Cells2RxMappingTool(const L1CaloFcal23Cells2RxMappingTool& rhs);
      L1CaloFcal23Cells2RxMappingTool& operator=(const L1CaloFcal23Cells2RxMappingTool& rhs);

      std::map<unsigned int,unsigned int>* m_mapOfflineCell2RxId;
      std::map<unsigned int,unsigned int>* m_mapOnlineCell2RxId;

      std::map<unsigned int,unsigned int>* m_mapOfflineCellNotConnectedToCalibLines;
      std::map<unsigned int,unsigned int>* m_mapOnlineCellNotConnectedToCalibLines;
  };
} // end of namespace
#endif
