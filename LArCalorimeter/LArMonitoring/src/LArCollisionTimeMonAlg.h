/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArCollisionTimeMonAlg
 * @author Margherita Spalla <margherita.spalla@cern.ch> (migrated from LArCollisionTimeMonTool by Vikas Bansal <Vikas@uvic.ca>)
 *
 */

#ifndef LAR_COLLISION_TIME_MON_TOOL_H
#define LAR_COLLISION_TIME_MON_TOOL_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "LArRecEvent/LArCollisionTime.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "LumiBlockData/BunchCrossingCondData.h"

#include <string>
#include <vector>



class LArCollisionTimeMonAlg: public AthMonitorAlgorithm
{
 public:
  LArCollisionTimeMonAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /** @brief Default destructor */
  virtual ~LArCollisionTimeMonAlg();

  /** @brief Overwrite dummy method from AlgTool */
  virtual StatusCode initialize() override;


  /** Called each event */
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;


  
 private:

 //MonGroup(s) name
 Gaudi::Property<std::string> m_MonGroupName {this,"CollTimeGroupName","LArCollisionTimeMonGroup"};
 Gaudi::Property<std::string> m_InTrain_MonGroupName {this,"InTrain_CollTimeGroupName",""};


  // Other properties
  Gaudi::Property<double> m_timeUnit {this,"TimeUnit",Gaudi::Units::nanosecond};
  Gaudi::Property<int> m_distance {this,"TrainFrontDistance",30*Gaudi::Units::nanosecond}; // distance from train front to fill second histos
  Gaudi::Property<bool> m_IsOnline {this,"IsOnline",false};
  Gaudi::Property<float> m_timeCut {this,"timeDiffCut",5.0*Gaudi::Units::nanosecond};
  Gaudi::Property<int> m_minCells {this,"nCells",2};
  Gaudi::Property<bool> m_eWeighted {this,"eWeighted",true};

  //containers' handles
  SG::ReadHandleKey<LArCollisionTime> m_LArCollisionTimeKey{this, "Key", "LArCollisionTime", "Key for the LArCollisionTime data"};

  SG::ReadCondHandleKey<BunchCrossingCondData> m_bunchCrossingKey{this, "BunchCrossingKey", "BunchCrossingData", "Key BunchCrossing CDO" };

  SG::ReadDecorHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "LArStatusFlag", "EventInfo.larFlag", "Key for EventInfo object"};
};

#endif
