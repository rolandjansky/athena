/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRFIRCONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRFIRCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloPprFir.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

#include <map>

/**
*  Container of L1CaloPprFir objects, inherit
*  from the abstract base class AbstractL1CaloConditionContainer.
*  Implement the IL1CaloPersistenceCapable interface.
*
*  @author Damien Prieur <damien.prieur@cern.ch>
*/

class string;

typedef std::map<unsigned int, L1CaloPprFir> L1CaloPprFirMap;

class L1CaloPprFirContainer: public DataObject, public AbstractL1CaloPersistentCondition {

private:
	enum eAttrSpecification { eFirStartBit, eFirCoeff1, eFirCoeff2, eFirCoeff3, eFirCoeff4, eFirCoeff5 };

public:
        enum eCoolFolders { eFIRChanCalib };


        typedef std::map<unsigned int, L1CaloPprFir>::const_iterator const_iterator;

	L1CaloPprFirContainer();
	L1CaloPprFirContainer(const std::map<L1CaloPprFirContainer::eCoolFolders, std::string>& folderKeysMap);
	virtual ~L1CaloPprFirContainer() {};
	virtual DataObject* makePersistent() const;

	// IL1CaloPersistenceCapable interface
	using AbstractL1CaloPersistentCondition::makeTransient; // to unhide the default implementation of makeTransient
	virtual void makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap);

	const L1CaloPprFir* fir(unsigned int channelId) const;
	const L1CaloPprFir* fir(const L1CaloCoolChannelId& channelId) const;
	void addFir(unsigned int channelId, const L1CaloPprFir& fir);
	void addFir(const L1CaloCoolChannelId& channelId, const L1CaloPprFir& fir);

	inline L1CaloPprFirMap::const_iterator begin() const { return m_mFIRMap.begin(); };
	inline L1CaloPprFirMap::const_iterator end()   const { return m_mFIRMap.end(); };

	virtual std::vector<std::string> coolInputKeys() const;
	virtual std::string coolOutputKey() const;

	virtual void clear();

	void dump() const;

private:
	std::string coolFolderKey(L1CaloPprFirContainer::eCoolFolders efolder) const;

	L1CaloPprFirMap m_mFIRMap;
	std::map<L1CaloPprFirContainer::eCoolFolders, std::string> m_mCoolFoldersKeysMap;

	bool check4Bits(unsigned char value) const;
	bool checkRange4Bits2Complement(int value) const;
	bool checkRangeUnsignedInt(int value) const;
	int uChar4BitsToSignedInt(unsigned char value) const;
	unsigned char intToUChar4Bits2Complement(int value) const;

};

CLASS_DEF(L1CaloPprFirContainer , 1082821521 , 1 )

#endif
