/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALOCOOLCHANNELID_H
#define L1CALOCOOLCHANNELID_H

#include "TrigT1CaloCalibConditions/L1CaloModuleType.h"

class L1CaloCoolChannelId {
	friend std::ostream& operator<<(std::ostream& output, const L1CaloCoolChannelId& r);

public:

  L1CaloCoolChannelId();
  L1CaloCoolChannelId(unsigned int crate,
                L1CaloModuleType mType,
                unsigned int module,
                unsigned int submodule,
                unsigned int channel, bool logical = true);
  L1CaloCoolChannelId(unsigned int id);
  L1CaloCoolChannelId(const L1CaloCoolChannelId& id);
  virtual ~L1CaloCoolChannelId() {}

  unsigned int crate()     const { return (m_id & 0xff000000) >> 24; }
  L1CaloModuleType moduleType() const;

  unsigned int module(bool logical=true) const;
  unsigned int subModule() const { return (m_id & 0x0000ff00) >> 8; }
  unsigned int channel()   const { return (m_id & 0x000000ff); }

  unsigned int id()        const { return m_id; }

  L1CaloCoolChannelId& operator=(const L1CaloCoolChannelId& id);
  bool operator==(const L1CaloCoolChannelId& id) const;
  bool operator<(const L1CaloCoolChannelId& id) const;

private:
  //L1CaloModuleType moduleType(unsigned int mTypeId) const;
  //unsigned int moduleTypeId(L1CaloModuleType mType) const;

  unsigned int logicalToPhysicalModule(L1CaloModuleType mType, unsigned int module) const;
  unsigned int physicalToLogicalModule(L1CaloModuleType mType, unsigned int module) const;


  void setId(unsigned int crate,
             L1CaloModuleType mType,
             unsigned int module,
             unsigned int submodule,
             unsigned int channel);
  unsigned int m_id;

};

#endif
