/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_IL1CALOPERSISTENCECAPABLE_H
#define TRIGT1CALOCALIBCONDITIONS_IL1CALOPERSISTENCECAPABLE_H

#include  "AthenaPoolUtilities/CondAttrListCollection.h"
#include  "AthenaPoolUtilities/AthenaAttributeList.h"

/**
 *  IL1CaloConditionContainer pure abstract class.
 *  Interface for L1Calo conditions objects to be
 *  persitified into COOL DB tables.
 *
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */

class DataObject;

class IL1CaloPersistenceCapable {
public:
	virtual ~IL1CaloPersistenceCapable() {};

	// Used by L1CondSvc to create persistent collection (CondAttrListCollection) out of the transient container
	virtual DataObject* makePersistent() const = 0;

	// Used by L1CondSvc to create transient container from persistent collections retrieved from the DB
	//virtual void makeTransient(const CondAttrListCollection*& condAttrListCollection) = 0;
	virtual void makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap) = 0;
	//virtual void makeTransient(const AthenaAttributeList*& condAttributeList) = 0;
	virtual void makeTransient(const std::map<std::string, AthenaAttributeList*> condAttributeList) = 0;
	virtual std::string conditionType() const = 0;
	//virtual std::string storeGateKey() const = 0; //to be supressed

	virtual std::vector<std::string> coolInputKeys() const = 0;
	virtual std::string coolOutputKey() const = 0;

	virtual void clear() = 0;
};

#endif
