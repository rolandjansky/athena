/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpacePointCollection.cxx
//   Implementation file for class SpacePointCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 14/10/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "TrkSpacePoint/SpacePointCollection.h"
#include "GaudiKernel/MsgStream.h"
// Constructor with parameters:
SpacePointCollection::SpacePointCollection(const IdentifierHash idHash) :
  m_idHash(idHash),
  m_id()
{}

// Constructor with parameters:
SpacePointCollection::SpacePointCollection(const SpacePointCollection& spc) :
  DataVector< Trk::SpacePoint >( spc ),
  m_idHash(),
  m_id()
{
	// added this ctor as a temporary hack for HLT
	// it should never be called.
	throw SpacePointCollectionCopyConstructorCalled();
}

// Destructor:
SpacePointCollection::~SpacePointCollection()
{ }

Identifier SpacePointCollection::identify() const
{
  return m_id;
}

IdentifierHash SpacePointCollection::identifyHash() const
{
  return m_idHash;
}

void SpacePointCollection::setIdentifier(Identifier id)
{
  m_id = id;
}

std::string SpacePointCollection::type() const
{
  return "SpacePointCollection";
}
/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const SpacePointCollection& coll) {
  sl << "SpacePointCollection: "
     << "identify()="<< coll.identify()
     << ", SP=[";
  SpacePointCollection::const_iterator it = coll.begin();
  SpacePointCollection::const_iterator itEnd = coll.end();
  for (;it!=itEnd;++it) sl<< (**it)<<", ";
  sl <<" ]"<<std::endl;
  return sl;
}

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const SpacePointCollection& coll) {
  sl << "SpacePointCollection: "
     << "identify()="<< coll.identify()
     << ", SP=[";
  SpacePointCollection::const_iterator it = coll.begin();
  SpacePointCollection::const_iterator itEnd = coll.end();
  for (;it!=itEnd;++it) sl<< (**it)<<", ";
  sl <<" ]"<<std::endl;
  return sl;
}
