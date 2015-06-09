/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPMDEADCHANNELCONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPMDEADCHANNELCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannel.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

#include <map>

/**
*  Container of L1CaloPpmDeadChannel objects, inherit
*  from the abstract base class AbstractL1CaloConditionContainer.
*  Implement the IL1CaloPersistenceCapable interface.
*
*  @author Damien Prieur <damien.prieur@cern.ch>
*/

class string;

typedef std::map<unsigned int, L1CaloPpmDeadChannel> L1CaloPpmDeadChannelMap;

class L1CaloPpmDeadChannelContainer: public DataObject, public AbstractL1CaloPersistentCondition {

private:
	enum eAttrSpecification { eErrorCode, eComment };

public:
        enum eCoolFolders { ePpmDeadChannels };

        typedef std::map<unsigned int, L1CaloPpmDeadChannel>::const_iterator const_iterator;

	L1CaloPpmDeadChannelContainer();
	L1CaloPpmDeadChannelContainer(const std::map<L1CaloPpmDeadChannelContainer::eCoolFolders, std::string>& folderKeysMap);
	virtual ~L1CaloPpmDeadChannelContainer() {};
	virtual DataObject* makePersistent() const;

	// IL1CaloPersistenceCapable interface
	using AbstractL1CaloPersistentCondition::makeTransient; // to unhide the default implementation of makeTransient
	virtual void makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap);

	const L1CaloPpmDeadChannel* ppmDeadChannel(unsigned int channelId) const;
	const L1CaloPpmDeadChannel* ppmDeadChannel(const L1CaloCoolChannelId& channelId) const;
	void addPpmDeadChannel(unsigned int channelId, const L1CaloPpmDeadChannel& ppmDeadChannel);
	void addPpmDeadChannel(const L1CaloCoolChannelId& channelId, const L1CaloPpmDeadChannel& ppmDeadChannel);

	inline L1CaloPpmDeadChannelMap::const_iterator begin() const { return m_mPpmDeadChannelMap.begin(); };
	inline L1CaloPpmDeadChannelMap::const_iterator end()   const { return m_mPpmDeadChannelMap.end(); };

	virtual std::vector<std::string> coolInputKeys() const;
	virtual std::string coolOutputKey() const;

	virtual void clear();

	void dump() const;

private:
	std::string coolFolderKey(L1CaloPpmDeadChannelContainer::eCoolFolders efolder) const;

	L1CaloPpmDeadChannelMap m_mPpmDeadChannelMap;
	std::map<L1CaloPpmDeadChannelContainer::eCoolFolders, std::string> m_mCoolFoldersKeysMap;

};

CLASS_DEF(L1CaloPpmDeadChannelContainer , 1216459634 , 1 )

#endif
