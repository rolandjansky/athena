// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRDISABLEDCHANNELCONTAINERRUN2_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRDISABLEDCHANNELCONTAINERRUN2_H

#include "CLIDSvc/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"
#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannelContainer.h"

#include <map>

/**
 *  Container of L1CaloPprDisabledChannelRun2 objects, inherit
 *  from the abstract base class AbstractL1CaloConditionContainer.
 *  Implement the IL1CaloPersistenceCapable interface.
 *
 *  Modified to work with the Run-2 database.
 *
 *  @author Veit Scharf
 */


class L1CaloPprDisabledChannelContainerRun2 : public virtual L1CaloPprDisabledChannelContainer,
                                              public virtual DataObject,
                                              public virtual AbstractL1CaloPersistentCondition
{
public:
  using L1CaloPprDisabledChannelContainer::eCoolFolders;
  using L1CaloPprDisabledChannelContainer::const_iterator;

  L1CaloPprDisabledChannelContainerRun2();
  L1CaloPprDisabledChannelContainerRun2(const std::map<L1CaloPprDisabledChannelContainerRun2::eCoolFolders, std::string>& folderKeysMap);
  virtual ~L1CaloPprDisabledChannelContainerRun2() {};

  using L1CaloPprDisabledChannelContainer::makePersistent;
  using AbstractL1CaloPersistentCondition::makeTransient;
  using L1CaloPprDisabledChannelContainer::makeTransient;

  using L1CaloPprDisabledChannelContainer::pprDisabledChannel;
  using L1CaloPprDisabledChannelContainer::addPprDisabledChannel;

  using L1CaloPprDisabledChannelContainer::begin;
  using L1CaloPprDisabledChannelContainer::end;

  virtual std::vector<std::string> coolInputKeys() const;
  virtual std::string coolOutputKey() const;

  using L1CaloPprDisabledChannelContainer::clear;
  using L1CaloPprDisabledChannelContainer::dump;

private:
  std::string coolFolderKey(L1CaloPprDisabledChannelContainer::eCoolFolders efolder) const;
  std::map<L1CaloPprDisabledChannelContainerRun2::eCoolFolders, std::string> m_coolFoldersKeysMap;
};

CLASS_DEF( L1CaloPprDisabledChannelContainerRun2 , 1248179392 , 1 )

#endif
