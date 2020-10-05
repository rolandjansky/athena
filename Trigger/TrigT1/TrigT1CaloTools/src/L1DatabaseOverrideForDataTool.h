// -*- C++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file L1DatabaseOverrideForData.h
 * @author Veit Scharf <veit.scharf@cern.ch>
 * @date Feb, 2016
 * @brief Modifies the database folders to be able to run PPM simulation on data.
 *
 * The PPM simulation uses several database folders as inputs. These folders
 * have the same structure as the ones used for configuring the online system.
 * This makes it possible to run the PPM simulation on data. 
 *
 * The database payload layout between data and simulation is different for two
 * fields of the PprChanDefaults folder. In the online database bcidDecision1
 * and satOverride1 correspond to the highest thresholds while for the offline
 * database these corresponds to the lowers threshold. This tool doesn't do much
 * more than exchange bcidDecision1 with bcidDecision3 (same for satOverrideX).
 */
#ifndef LVL1L1DATABASEOVERRIDEFORDATATOOL_H
#define LVL1L1DATABASEOVERRIDEFORDATATOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigT1CaloToolInterfaces/IL1DatabaseOverrideTool.h"

namespace LVL1 {

class L1DatabaseOverrideForDataTool : virtual public IL1DatabaseOverrideTool,
                                              public AthAlgTool
{
public:
  L1DatabaseOverrideForDataTool(const std::string&,const std::string&,const IInterface*);
  /** default destructor */
  virtual ~L1DatabaseOverrideForDataTool();
  
  StatusCode modifyChanDefaults(L1CaloPprChanDefaultsContainer&);
  StatusCode modifyChanCalib(L1CaloPprChanCalibContainer&);
  StatusCode modifyDisabledTowers(L1CaloDisabledTowersContainer&);
  StatusCode modifyDeadChannels(L1CaloPpmDeadChannelsContainer&);
};

} // end of namespace

#endif // LVL1L1DATABASEOVERRIDEFORDATATOOL_H
