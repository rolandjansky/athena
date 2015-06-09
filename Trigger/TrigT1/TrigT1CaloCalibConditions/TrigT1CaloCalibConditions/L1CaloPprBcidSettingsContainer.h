/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRBCIDSETTINGSCONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRBCIDSETTINGSCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloPprBcidSettings.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

#include <map>

/**
*  Container of L1CaloPprBcidSettings objects, inherit
*  from the abstract base class AbstractL1CaloConditionContainer.
*  Implement the IL1CaloPersistenceCapable interface.
*
*  @author Damien Prieur <damien.prieur@cern.ch>
*/

typedef std::map<unsigned int, L1CaloPprBcidSettings> L1CaloPprBcidSettingsMap;

class L1CaloPprBcidSettingsContainer: public DataObject, public AbstractL1CaloPersistentCondition {

private:
	enum eAttrSpecification { eExtBcidThreshold, eSatBcidThreshLow, eSatBcidThreshHigh, eSatBcidLevel, eBcidEnergyRangeLow, eBcidEnergyRangeHigh, eFirStartBit,
	                          eBcidDecision1, eSatOverride1, eBcidDecision2, eSatOverride2, eBcidDecision3, eSatOverride3,
	                          ePeakFinderCond, eDecisionSource };

public:
	enum eCoolFolders { eBCIDChanDefaults, eBCIDChanCalib };

	typedef std::map<unsigned int, L1CaloPprBcidSettings>::const_iterator const_iterator;

	L1CaloPprBcidSettingsContainer();
	L1CaloPprBcidSettingsContainer(const std::map<L1CaloPprBcidSettingsContainer::eCoolFolders, std::string>& folderKeysMap);
	virtual ~L1CaloPprBcidSettingsContainer() {};
	virtual DataObject* makePersistent() const;

	// IL1CaloPersistenceCapable interface
	using AbstractL1CaloPersistentCondition::makeTransient; // to unhide the default implementation of makeTransient
	virtual void makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap);

	const L1CaloPprBcidSettings* bcidSettings(unsigned int channelId) const;
	const L1CaloPprBcidSettings* bcidSettings(const L1CaloCoolChannelId& channelId) const;

	void addBcidSettings(unsigned int channelId, const L1CaloPprBcidSettings& bcidSettings);
	void addBcidSettings(const L1CaloCoolChannelId& channelId, const L1CaloPprBcidSettings& bcidSettings);

	inline L1CaloPprBcidSettingsMap::const_iterator begin() const { return m_mBCIDSettingsMap.begin(); };
	inline L1CaloPprBcidSettingsMap::const_iterator end()   const { return m_mBCIDSettingsMap.end(); };

	virtual std::vector<std::string> coolInputKeys() const;
	virtual std::string coolOutputKey() const;

	virtual void clear();

	void dump() const;

private:
	std::string coolFolderKey(L1CaloPprBcidSettingsContainer::eCoolFolders efolder) const;

	L1CaloPprBcidSettingsMap m_mBCIDSettingsMap;
	std::map<L1CaloPprBcidSettingsContainer::eCoolFolders, std::string> m_mCoolFoldersKeysMap;

};

CLASS_DEF(L1CaloPprBcidSettingsContainer , 1154803560 , 1 )

#endif
