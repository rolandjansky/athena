/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRDISABLEDCHANNELCONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRDISABLEDCHANNELCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannel.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

#include <map>
#include <string>

/**
*  Container of L1CaloPprDisabledChannel objects, inherit
*  from the abstract base class AbstractL1CaloConditionContainer.
*  Implement the IL1CaloPersistenceCapable interface.
*
*  @author Damien Prieur <damien.prieur@cern.ch>
*/

typedef std::map<unsigned int, L1CaloPprDisabledChannel> L1CaloPprDisabledChannelMap;

class L1CaloPprDisabledChannelContainer : public virtual DataObject, public virtual AbstractL1CaloPersistentCondition {

private:
	enum eAttrSpecification { eChanDeadErrorCode, eChanCalibErrorCode, eCaloDisableMask, eEtaDisableMask, ePhiDisableMask, eChanDisableMask, eNoiseCut, eDisabledBits };

public:
	enum eCoolFolders { ePpmDeadChannels, ePprChanCalib, eDisabledTowers, eOKS2COOL };

	typedef std::map<unsigned int, L1CaloPprDisabledChannel>::const_iterator const_iterator;

	L1CaloPprDisabledChannelContainer();
	L1CaloPprDisabledChannelContainer(const std::map<L1CaloPprDisabledChannelContainer::eCoolFolders, std::string>& folderKeysMap);
	virtual ~L1CaloPprDisabledChannelContainer() {};
	virtual DataObject* makePersistent() const;

	// IL1CaloPersistenceCapable interface
	using AbstractL1CaloPersistentCondition::makeTransient; // to unhide the default implementation of makeTransient
	virtual void makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap);

	const L1CaloPprDisabledChannel* pprDisabledChannel(unsigned int channelId) const;
	const L1CaloPprDisabledChannel* pprDisabledChannel(const L1CaloCoolChannelId& channelId) const;
	void addPprDisabledChannel(unsigned int channelId, const L1CaloPprDisabledChannel& pprDisabledChannel);
	void addPprDisabledChannel(const L1CaloCoolChannelId& channelId, const L1CaloPprDisabledChannel& pprDisabledChannel);

	inline L1CaloPprDisabledChannelMap::const_iterator begin() const { return m_mPprDisabledChannelMap.begin(); };
	inline L1CaloPprDisabledChannelMap::const_iterator end()   const { return m_mPprDisabledChannelMap.end(); };

	virtual std::vector<std::string> coolInputKeys() const;
	virtual std::string coolOutputKey() const;

	virtual void clear();

	void dump() const;

private:
	virtual std::string coolFolderKey(L1CaloPprDisabledChannelContainer::eCoolFolders efolder) const;

	L1CaloPprDisabledChannelMap m_mPprDisabledChannelMap;
	std::map<L1CaloPprDisabledChannelContainer::eCoolFolders, std::string> m_mCoolFoldersKeysMap;

};

CLASS_DEF(L1CaloPprDisabledChannelContainer , 1341715947 , 1 )

#endif
