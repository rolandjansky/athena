/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALORXGAINCONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALORXGAINCONTAINER_H


#include "CLIDSvc/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloRxGain.h"
#include "TrigT1CaloCalibConditions/L1CaloRxCoolChannelId.h"

/**
*  Container of L1CaloRxGain objects, inherit
*  from the abstract base class AbstractL1CaloConditionContainer.
*  Implement the IL1CaloPersistenceCapable interface.
*
*  @author Damien Prieur <damien.prieur@cern.ch>
*/

typedef std::map<unsigned int, L1CaloRxGain> L1CaloRxGainMap;

class L1CaloRxGainContainer: public DataObject, public AbstractL1CaloPersistentCondition {

private:
	enum eAttrSpecification { eIdentifier, eCrate, eSlot, eChannel, eGain, eComment };

public:
        enum eCoolFolders { eRxGainFolder };

	typedef std::map<unsigned int, L1CaloRxGain>::const_iterator const_iterator;

	L1CaloRxGainContainer();
	L1CaloRxGainContainer(const std::map<L1CaloRxGainContainer::eCoolFolders, std::string>& folderKeysMap);
	virtual ~L1CaloRxGainContainer() {};
	virtual DataObject* makePersistent() const;

	// IL1CaloPersistenceCapable interface
	using AbstractL1CaloPersistentCondition::makeTransient; // to unhide the default implementation of makeTransient
	virtual void makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap);

	const L1CaloRxGain* rxGain(unsigned int channelId) const;
	const L1CaloRxGain* rxGain(const L1CaloRxCoolChannelId& channelId) const;
	void addRxGain(unsigned int channelId, const L1CaloRxGain& rxGain);
	void addRxGain(const L1CaloRxCoolChannelId& channelId, const L1CaloRxGain& rxGain);

	inline L1CaloRxGainMap::const_iterator begin() const { return m_mRxGainMap.begin(); };
	inline L1CaloRxGainMap::const_iterator end()   const { return m_mRxGainMap.end(); };

	virtual std::vector<std::string> coolInputKeys() const;
	virtual std::string coolOutputKey() const;


	virtual void clear();

	void dump() const;

private:
	std::string coolFolderKey(L1CaloRxGainContainer::eCoolFolders efolder) const;

	L1CaloRxGainMap m_mRxGainMap;
	std::map<L1CaloRxGainContainer::eCoolFolders, std::string> m_mCoolFoldersKeysMap;

};

CLASS_DEF(L1CaloRxGainContainer , 1163860999 , 1 )

#endif
