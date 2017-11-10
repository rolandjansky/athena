/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpacePointCollection.h
//   Header file for class SpacePointCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to contain all the SPs of a detector element:
// module for Pixel or wafer for SCT or layer for TRT, 
// for Muons:
// For SCT the DE is ambiguous, we are using the id of the R module (not the one with
// the stereo angle )
// The overlap space points are contained in the SpacePointOverlapCollection
///////////////////////////////////////////////////////////////////
// Version 1.0 14/10/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRKSPACEPOINT_SPACEPOINTCOLLECTION_H
#define TRKSPACEPOINT_SPACEPOINTCOLLECTION_H

// Base classes
#include "Identifier/Identifier.h"
#include "Identifier/Identifiable.h"
#include "Identifier/IdentifierHash.h"
#include "DataModel/DataVector.h"
#include "GaudiKernel/DataObject.h"
//class SpacePoint;
#include "TrkSpacePoint/SpacePoint.h"


class SpacePointCollectionCopyConstructorCalled{};


class SpacePointCollection 
	: public Identifiable,
	public DataVector< Trk::SpacePoint >
{
	///////////////////////////////////////////////////////////////////
	// Public methods:
	///////////////////////////////////////////////////////////////////
	public:	
	// Constructor with parameters:
	//   Hashed offline identifier of the DE
	SpacePointCollection(const IdentifierHash idHash);
	
	// Destructor:
	virtual ~SpacePointCollection();
	
	/**added for 10.0.0 as a hack for HLT. DO NOT USE IT. It will throw an exception*/
	SpacePointCollection(const SpacePointCollection& spc);
	
	
	///////////////////////////////////////////////////////////////////
	// Const methods:
	///////////////////////////////////////////////////////////////////

	virtual Identifier identify() const override final;
	
	virtual IdentifierHash identifyHash() const override final;
	
	void setIdentifier(Identifier id);

	virtual std::string type() const;
	
	///////////////////////////////////////////////////////////////////
	// Private methods:
	///////////////////////////////////////////////////////////////////
	private:
	
	SpacePointCollection();
	SpacePointCollection &operator=(const SpacePointCollection&);
	
	///////////////////////////////////////////////////////////////////
	// Private data:
	///////////////////////////////////////////////////////////////////
	private:
	const IdentifierHash m_idHash; 
	Identifier m_id; // identifier of the DE
};

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const SpacePointCollection& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const SpacePointCollection& coll);
///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(SpacePointCollection,1156991496,1)

#endif // TRKSPACEPOINT_SPACEPOINTCOLLECTION_H
