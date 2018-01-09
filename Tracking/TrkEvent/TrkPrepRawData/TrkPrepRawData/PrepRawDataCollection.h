/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PrepRawDataCollection.h
//   Header file for class PrepRawDataCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to contain all the RIOs of a detector element:
// module for Pixel or wafer for SCT or layer for TRT, 
// for Muons:
///////////////////////////////////////////////////////////////////
// Version 1.0 15/07/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRKPREPRAWDATA_PREPRAWDATACOLLECTION_H
#define TRKPREPRAWDATA_PREPRAWDATACOLLECTION_H

// Base classes
#include "Identifier/Identifiable.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "AthContainers/DataVector.h"


// Needed class
#include "TrkPrepRawData/PrepRawData.h"

namespace Trk{

class PrepRawDataCollectionCopyConstructorCalled{};

template< class PrepRawDataT >
class PrepRawDataCollection :  public Identifiable, public DataVector< PrepRawDataT > {

	///////////////////////////////////////////////////////////////////
	// Public methods:
	///////////////////////////////////////////////////////////////////
	public:
	
	// Constructor with parameters:
	//   Hashed offline identifier of the DE
	PrepRawDataCollection(const IdentifierHash idHash);
	PrepRawDataCollection();
	/**added for 10.0.0 as a hack for HLT. DO NOT USE IT. It will throw an exception*/
	PrepRawDataCollection(const PrepRawDataCollection& prd);
	
	// Destructor:
	virtual ~PrepRawDataCollection();
	
	
	///////////////////////////////////////////////////////////////////
	// Const methods:
	///////////////////////////////////////////////////////////////////
	
	// identifier of this detector element:
	virtual Identifier identify() const override final;

	virtual IdentifierHash identifyHash() const override final;

	void setIdentifier(Identifier id);	

	// plottable
	virtual std::string type() const;
	
	///////////////////////////////////////////////////////////////////
	// Private methods:
	///////////////////////////////////////////////////////////////////

	private:
	
	PrepRawDataCollection &operator=(const PrepRawDataCollection&);
	
	///////////////////////////////////////////////////////////////////
	// Private data:
	///////////////////////////////////////////////////////////////////
	private:
	const IdentifierHash m_idHash; 
        Identifier m_id; // identifier of the DE
};
// member functions that use Collection T
#include"TrkPrepRawData/PrepRawDataCollection.icc"
}

#endif // TRKPREPRAWDATA_PREPRAWDATACOLLECTION_H
