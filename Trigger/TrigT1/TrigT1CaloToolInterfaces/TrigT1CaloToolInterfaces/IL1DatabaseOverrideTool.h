// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigT1CaloToolInterfaces/IL1DatabaseOverrideTool.h
 * @author Veit Scharf <veit.scharf@cern.ch>
 * @date Feb, 2016
 * @brief Interface of a tool that modifies L1Calo database folders.
 *
 * The PPm simulation uses several database folders as inputs. This tool
 * interface provides a method to modify each one of those.
 */
#ifndef ILVL1L1DATABASEOVERRIDETOOL_H
#define ILVL1L1DATABASEOVERRIDETOOL_H

#include "GaudiKernel/IAlgTool.h"

class L1CaloPprChanDefaultsContainer;
class L1CaloPprChanCalibContainer;
class L1CaloDisabledTowersContainer;
class L1CaloPpmDeadChannelsContainer;

namespace LVL1 {
  static const InterfaceID IID_IL1DatabaseOverrideTool("LVL1::IL1DatabaseOverrideTool", 1, 0);

  class IL1DatabaseOverrideTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID() { return IID_IL1DatabaseOverrideTool; }

    // each method modifies the database folder
    virtual StatusCode modifyChanDefaults(L1CaloPprChanDefaultsContainer&) = 0;
    virtual StatusCode modifyChanCalib(L1CaloPprChanCalibContainer&) = 0;
    virtual StatusCode modifyDisabledTowers(L1CaloDisabledTowersContainer&) = 0;
    virtual StatusCode modifyDeadChannels( L1CaloPpmDeadChannelsContainer&) = 0;
  };
} // end of namespace

#endif 
