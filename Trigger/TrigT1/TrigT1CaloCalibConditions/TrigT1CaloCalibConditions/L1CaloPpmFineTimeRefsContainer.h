/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPMFINETIMEREFSCONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPMFINETIMEREFSCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmFineTimeRefs.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

#include <map>

/**
*  Container of L1CaloPpmFineTimeRefs objects, inherit
*  from the abstract base class AbstractL1CaloConditionContainer.
*  Implement the IL1CaloPersistenceCapable interface.
*
*  @author Damien Prieur <damien.prieur@cern.ch>
*/

class string;

typedef std::map<unsigned int, L1CaloPpmFineTimeRefs> L1CaloPpmFineTimeRefsMap;

class L1CaloPpmFineTimeRefsContainer: public DataObject, public AbstractL1CaloPersistentCondition {

private:
	enum eAttrSpecification { eErrorCode, eRefValue, eCriterion, eCalibValue };

public:
        enum eCoolFolders { ePpmFineTimeRefss };

        typedef std::map<unsigned int, L1CaloPpmFineTimeRefs>::const_iterator const_iterator;

	L1CaloPpmFineTimeRefsContainer();
	L1CaloPpmFineTimeRefsContainer(const std::map<L1CaloPpmFineTimeRefsContainer::eCoolFolders, std::string>& folderKeysMap);
	virtual ~L1CaloPpmFineTimeRefsContainer() {};
	virtual DataObject* makePersistent() const;

	// IL1CaloPersistenceCapable interface
	using AbstractL1CaloPersistentCondition::makeTransient; // to unhide the default implementation of makeTransient
	virtual void makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap);

	const L1CaloPpmFineTimeRefs* ppmFineTimeRefs(unsigned int channelId) const;
	const L1CaloPpmFineTimeRefs* ppmFineTimeRefs(const L1CaloCoolChannelId& channelId) const;
	void addPpmFineTimeRefs(unsigned int channelId, const L1CaloPpmFineTimeRefs& ppmFineTimeRefs);
	void addPpmFineTimeRefs(const L1CaloCoolChannelId& channelId, const L1CaloPpmFineTimeRefs& ppmFineTimeRefs);

	inline L1CaloPpmFineTimeRefsMap::const_iterator begin() const { return m_mPpmFineTimeRefsMap.begin(); };
	inline L1CaloPpmFineTimeRefsMap::const_iterator end()   const { return m_mPpmFineTimeRefsMap.end(); };

	virtual std::vector<std::string> coolInputKeys() const;
	virtual std::string coolOutputKey() const;

	virtual void clear();

	void dump() const;

private:
	std::string coolFolderKey(L1CaloPpmFineTimeRefsContainer::eCoolFolders efolder) const;

	L1CaloPpmFineTimeRefsMap m_mPpmFineTimeRefsMap;
	std::map<L1CaloPpmFineTimeRefsContainer::eCoolFolders, std::string> m_mCoolFoldersKeysMap;

};

CLASS_DEF(L1CaloPpmFineTimeRefsContainer , 1107733553 , 1 ) //number taken from $ clid -m "L1CaloPpmFineTimeRefsContainer"

#endif
