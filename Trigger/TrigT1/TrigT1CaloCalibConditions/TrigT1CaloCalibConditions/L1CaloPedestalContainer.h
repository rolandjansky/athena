/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPEDESTALCONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPEDESTALCONTAINER_H

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloPedestal.h"

/**
 *  L1CaloPedestalContainer class.
 *  Container of L1CaloPedestal objects, inherit
 *  from DataVector<L1CaloPedestal> and from the
 *  the abstract base class AbstractL1CaloConditionContainer.
 *  Implement the IL1CaloPersistenceCapable interface.
 *
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */

class L1CaloPedestalContainer: public DataVector<L1CaloPedestal>, public AbstractL1CaloPersistentCondition {

public:
	L1CaloPedestalContainer();
	virtual ~L1CaloPedestalContainer() {};
	virtual DataObject* makePersistent() const;

        // Hide from reflex to avoid warning from reflex shadow class.
#ifndef __REFLEX__
	using AbstractL1CaloPersistentCondition::makeTransient; // to unhide the default implementation of makeTransient
	virtual void makeTransient(const CondAttrListCollection*& condAttrListCollection);
#endif

private:
	enum eAttrSpecification { eMean, eRMS };

};

CLASS_DEF(L1CaloPedestalContainer , 1302642590 , 1 )

#endif
