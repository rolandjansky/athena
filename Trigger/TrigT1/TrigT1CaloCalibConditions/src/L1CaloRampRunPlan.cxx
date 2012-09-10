/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloRampRunPlan.h"

#include <string>
#include <sstream>
#include <iostream>

// AttributeList
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "GaudiKernel/MsgStream.h"


L1CaloRampRunPlan::L1CaloRampRunPlan(): AbstractL1CaloPersistentCondition("AthenaAttributeList"),
    m_bEndOfRampRun(false),
    m_nEventsPerStep(0),
    m_itCurrentStep(0),
    m_iCurrentEvent(-1)
{
    this->addSpecification(eEventsPerStep, std::string("EventsPerStep"), std::string("int")   );
    this->addSpecification(eEnergySteps,   std::string("EnergySteps"),   std::string("string"));
}

L1CaloRampRunPlan::L1CaloRampRunPlan(int nEventsPerStep, std::vector<double> vEnergySteps): AbstractL1CaloPersistentCondition("AthenaAttributeList"),
    m_bEndOfRampRun(false),
    m_nEventsPerStep(nEventsPerStep),
    m_vEnergySteps(vEnergySteps),
    m_iCurrentEvent(-1)
{
    this->addSpecification(eEventsPerStep, std::string("EventsPerStep"), std::string("int")   );
    this->addSpecification(eEnergySteps,   std::string("EnergySteps"),   std::string("string"));

    m_itCurrentStep = m_vEnergySteps.begin();
}

DataObject* L1CaloRampRunPlan::makePersistent() const {

	// Create AttributeListSpecification according to the attributes to be recorded
	// this is required to create the AttributeList objs
	coral::AttributeListSpecification* attrSpecification = this->createAttributeListSpecification();
	if(!attrSpecification->size()) return NULL;


    AthenaAttributeList* attrList = new AthenaAttributeList(*attrSpecification);

    // Set value of each row for the current channel
    int nEventsPerStep = this->eventsPerStep();
    std::vector<double> vEnergySteps  = this->energySteps();

    (*attrList)[ this->specificationName(eEventsPerStep)    ].data<int>() = nEventsPerStep;


    // export energies to a string
    std::ostringstream oss;
    std::vector<double>::const_iterator energy_it = vEnergySteps.begin();
    for (;energy_it!=vEnergySteps.end();++energy_it) {
        oss << *energy_it << " ";
    }
    (*attrList)[ this->specificationName(eEnergySteps) ].data<std::string>()= oss.str();

	return (DataObject*) attrList;
}

void L1CaloRampRunPlan::makeTransient(const AthenaAttributeList*& athenaAttributeList) {

    int nEventsPerStep   = (*athenaAttributeList)[ this->specificationName(eEventsPerStep) ].data<int>();
    std::string strEnergySteps  = (*athenaAttributeList)[ this->specificationName(eEnergySteps) ].data<std::string>();

    //tokenize strTable
    int buf;
    std::stringstream ss(strEnergySteps); // insert the table string into a stream
    std::vector<double> vEnergySteps; // create vector to hold our words
    while (ss >> buf) vEnergySteps.push_back(buf);

    m_nEventsPerStep = nEventsPerStep;

    m_vEnergySteps  = vEnergySteps;
    m_itCurrentStep = m_vEnergySteps.begin();
}

void L1CaloRampRunPlan::nextEvent() {
    if(m_bEndOfRampRun) {
        //log << MSG::INFO << "No more steps to generate - End of ramp run" << endreq;
        //std::cout << "End of ramp run - No more steps to iterate" << std::endl;
        return;
    }

    ++m_iCurrentEvent;

    if(m_iCurrentEvent==(m_nEventsPerStep)) {
        ++m_itCurrentStep;
        m_iCurrentEvent=0;
        if(m_itCurrentStep!=m_vEnergySteps.end()) {
            //std::cout << "nEventsPerStep reached, next step: "<< *m_itCurrentStep << std::endl;
        } else {
            //std::cout << "nEventsPerStep reached, no more steps to iterate"<< std::endl;
            m_iCurrentEvent = -1;
            m_bEndOfRampRun = true;
            return;
        }
    }
}

std::string L1CaloRampRunPlan::status() {
    std::ostringstream oss;
    if(!m_bEndOfRampRun) {
        oss << "L1CaloRampRunPlan - Current event: "<<m_iCurrentEvent<<", current step: "<<*m_itCurrentStep;
        //if(m_itCurrentStep!=0) oss <<*m_itCurrentStep;
        //else oss << "not initialized, there might be a problem !";
    } else {
        oss << "L1CaloRampRunPlan: No more events/steps to process";
    }
    return oss.str();
}


std::ostream& operator<<(std::ostream& output, const L1CaloRampRunPlan& rplan) {
    output << "nEventsPerStep: "<< rplan.m_nEventsPerStep<<" ";
    std::vector<double> vSteps = rplan.energySteps();
    output<<" nsteps: "<< vSteps.size()<<" - steps: ";
    std::vector<double>::const_iterator it = vSteps.begin();
    for(;it!=vSteps.end();++it) {
        output << *it<<" ";
    }
    return output;
}

