/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPedestalContainer.h"

#include <string>

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

using std::cout;
using std::endl;

L1CaloPedestalContainer::L1CaloPedestalContainer():AbstractL1CaloPersistentCondition("CondAttrListCollection") {
	// Define DB rows names and types in order to construct the AttributeListSpecification object
	this->addSpecification(eMean, std::string("Mean"), std::string("double"));
	this->addSpecification(eRMS,  std::string("RMS"),  std::string("double"));
}


DataObject* L1CaloPedestalContainer::makePersistent() const {

	// return null if DataVector<> is empty
	if(this->size()==0) return NULL;

	// Create AttributeListSpecification according to the attributes to be recorded
	// this is required to create the AttributeList objs
	coral::AttributeListSpecification* attrSpecification = this->createAttributeListSpecification();
	if(!attrSpecification->size()) return NULL;


	CondAttrListCollection* attrListCollection = new CondAttrListCollection(true);

	//loop on pedestals in the container to create AttributeList objs to fill the CondAttrListCollection with
	L1CaloPedestalContainer::const_iterator pos = this->begin();
	L1CaloPedestalContainer::const_iterator pos_end = this->end();

	for(;pos!=pos_end;++pos) {
		const L1CaloPedestal* ped = (*pos);

		AthenaAttributeList attrList(*attrSpecification);

		// Set value of each row for the current channel
		attrList[ this->specificationName(eMean) ].setValue(ped->get_mean());
		attrList[ this->specificationName(eRMS)  ].setValue(ped->get_rms());
		unsigned int iChanNumber = ped->channelId();

		//std::ostringstream attrStream;
		//attrList.toOutputStream(attrStream);
		//cout << " ChanNum: " << iChanNumber << ", Attribute list: " << attrStream.str() << endl;

		// add AttributeList corresponding to channel iChanNumber to the CondAttrListCollection
		attrListCollection->add(iChanNumber, attrList);
	}
	return (DataObject*) attrListCollection;
}

void L1CaloPedestalContainer::makeTransient(const CondAttrListCollection*& attrListCollection) {

	//loop over CondAttrListCollection
	CondAttrListCollection::const_iterator it = attrListCollection->begin();
	for(;it!=attrListCollection->end();++it) {

                const coral::AttributeList& attrList = it->second;

		// Get value of each row for the current channel
		CondAttrListCollection::ChanNum chanNum = 	it->first;
		double mean = attrList[ this->specificationName(eMean) ].data<double>();
		double rms  = attrList[ this->specificationName(eRMS)  ].data<double>();

		// Create transient L1CaloPedestal obj
		L1CaloPedestal* pedestal = new L1CaloPedestal(chanNum, mean, rms);

		// Add L1CaloPedestal to DataVector<L1CaloPedestal>
		this->push_back(pedestal);
	}
}

