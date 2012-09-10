/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRLUTCONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRLUTCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloPprLut.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

/**
*  Container of L1CaloPprLut objects, inherit
*  from the abstract base class AbstractL1CaloConditionContainer.
*  Implement the IL1CaloPersistenceCapable interface.
*
*  @author Damien Prieur <damien.prieur@cern.ch>
*/

typedef std::map<unsigned int, L1CaloPprLut> L1CaloPprLutMap;

class L1CaloPprLutContainer: public DataObject, public AbstractL1CaloPersistentCondition {


private:
	enum eAttrSpecification { eLutStrategy, eLutOffset, eLutSlope, eLutNoiseCut, eLutParameters, ePedValue, ePedMean, eErrorCode};


public:
	enum eCoolFolders { ePprLutChanCalib };
 
        typedef std::map<unsigned int, L1CaloPprLut>::const_iterator const_iterator;

	L1CaloPprLutContainer();
	L1CaloPprLutContainer(const std::map<L1CaloPprLutContainer::eCoolFolders, std::string>& folderKeysMap);
	virtual ~L1CaloPprLutContainer() {};
	virtual DataObject* makePersistent() const;

        // Hide from reflex to avoid warning from reflex shadow class.
#ifndef __REFLEX__
	// IL1CaloPersistenceCapable interface
	using AbstractL1CaloPersistentCondition::makeTransient; // to unhide the default implementation of makeTransient
	virtual void makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap);
#endif

	const L1CaloPprLut* pprLut(unsigned int channelId) const;
	const L1CaloPprLut* pprLut(const L1CaloCoolChannelId& channelId) const;
	void addPprLut(unsigned int channelId, const L1CaloPprLut& pprLut);
	void addPprLut(const L1CaloCoolChannelId& channelId, const L1CaloPprLut& pprLut);

	inline L1CaloPprLutMap::const_iterator begin() const { return m_mPprLutMap.begin(); };
	inline L1CaloPprLutMap::const_iterator end()   const { return m_mPprLutMap.end(); };

	virtual std::vector<std::string> coolInputKeys() const;
	virtual std::string coolOutputKey() const;

	virtual void clear();

	void dump() const;

private:
	std::string coolFolderKey(L1CaloPprLutContainer::eCoolFolders efolder) const;

	L1CaloPprLutMap m_mPprLutMap;
	std::map<L1CaloPprLutContainer::eCoolFolders, std::string> m_mCoolFoldersKeysMap;

};

CLASS_DEF(L1CaloPprLutContainer , 1225798781 , 1 )

#endif
