/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALORAMPRUNPLAN_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALORAMPRUNPLAN_H

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "GaudiKernel/DataObject.h"

#include <iostream>
#include <vector>

/**
 *  Transient class defining ramps runplan objects defined
 *  by the online framework and retrieved from COOL
 *
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */

class L1CaloRampRunPlan : public AbstractL1CaloPersistentCondition, public DataObject {
    friend std::ostream& operator<<(std::ostream& output, const L1CaloRampRunPlan& rplan);

public:
    L1CaloRampRunPlan();
    L1CaloRampRunPlan(int nEventsPerStep, std::vector<double> vEnergySteps);

	virtual ~L1CaloRampRunPlan() {};

	virtual DataObject* makePersistent() const;
	using AbstractL1CaloPersistentCondition::makeTransient; // to unhide the default implementation of makeTransient
	virtual void makeTransient(const AthenaAttributeList*& attributeList);

	int eventsPerStep() const { return m_nEventsPerStep; };
	std::vector<double> energySteps() const { return m_vEnergySteps; };

	void nextEvent();
	double currentEnergy() { return *m_itCurrentStep; };
	std::string status();
	bool endOfRun() { return m_bEndOfRampRun; };

private:
	enum eAttrSpecification { eEventsPerStep, eEnergySteps };

    bool m_bEndOfRampRun;

    int m_nEventsPerStep;
    std::vector<double> m_vEnergySteps;

    //int m_iCurrentStep;
    std::vector<double>::iterator m_itCurrentStep;
    int m_iCurrentEvent;

};

CLASS_DEF(L1CaloRampRunPlan , 237325569 , 1 )

#endif
