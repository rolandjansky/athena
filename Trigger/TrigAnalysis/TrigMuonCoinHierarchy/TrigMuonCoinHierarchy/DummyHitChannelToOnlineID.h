/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DUMMYHITCHANNELTOONLINEID_H
#define TRIGGER_DUMMYHITCHANNELTOONLINEID_H

//#include "MuonTGC_Cabling/MuonTGC_CablingSvc.h"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

namespace Trigger {
  class DummyHitChannelToOnlineID {
  public:
    /* Constructor */
    DummyHitChannelToOnlineID(std::string pre_isAside,
			      int pre_sector,
			      int pre_phi,
			      std::string pre_isForward,
			      std::string pre_isStrip,
			      int pre_layer,
			      int pre_chamber,
			      int pre_channel           );

    /* Destructor */
    virtual ~DummyHitChannelToOnlineID();

    /* Get subsystem number from dummy hit channel profile */
    int GetSubsystemNumber();

    /* Get octant number from dummy hit channel profile */
    int GetOctantNumber();

    /* Get module number from dummy hit channel profile */
    int GetModuleNumber();
    
    /* Get layer number from dummy hit channel profile */
    int GetLayerNumber();

    /* Get r number from dummy hit channel profile */
    int GetRNumber();

    /* Get wire or strip from dummy hit chanel profile */
    int GetWireOrStrip();

    /* Get channel number from dummy hit channel profile */
    int GetChannelNumber();

  private:
    std::string m_isAside;
    int m_sector;
    int m_phi;
    std::string m_isForward;
    std::string m_isStrip;
    int m_layer;
    int m_chamber;
    int m_channel;
    int m_triggerSectorId;

  };
} // end of nemespace Trriger

#endif // TRIGGER_DUMMYHITCHANNELTOONLINEID_H
