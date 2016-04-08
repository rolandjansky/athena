/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file WriteDataReentrant.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief 
 */


#undef NDEBUG
#include "WriteDataReentrant.h"

#include <list>
#include <vector>

#include "AthExStoreGateExample/MyDataObj.h"
#include "MyContObj.h"
#include "MapStringFloat.h"
#include "StoreGateExample_ClassDEF.h"

#include "AthContainers/DataVector.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteHandle.h"
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
#include "TestTools/expect_exception.h"

#include "AthenaKernel/DefaultKey.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/make_unique.h"

/////////////////////////////////////////////////////////////////////////////


WriteDataReentrant::WriteDataReentrant(const std::string& name,
                                       ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
  declareProperty ("DObjKey", m_dobjKey = "dobj");
  declareProperty ("DObjKey2", m_dobjKey2 = "dobj2");
  declareProperty ("DObjKey3", m_dobjKey3 = name);
  //declareProperty ("DObjKey4", m_dobjKey4 = "dobj4");
  declareProperty ("CObjKey", m_cobjKey = "cobj");
  declareProperty ("VFloatKey", m_vFloatKey = "vFloat");
  declareProperty ("PLinkListKey", m_pLinkListKey = name);
  declareProperty ("MKey", m_mKey = "mkey");
  declareProperty ("LinkVectorKey", m_linkVectorKey = "linkvec");
  declareProperty ("TestObjectKey", m_testObjectKey = "testobj");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode WriteDataReentrant::initialize()
{
  ATH_MSG_INFO ("in initialize()");
  ATH_CHECK( m_dobjKey.initialize() );
  ATH_CHECK( m_dobjKey2.initialize() );
  ATH_CHECK( m_dobjKey3.initialize() );
  //ATH_CHECK( m_dobjKey4.initialize() );
  ATH_CHECK( m_cobjKey.initialize() );
  ATH_CHECK( m_vFloatKey.initialize() );
  ATH_CHECK( m_pLinkListKey.initialize() );
  ATH_CHECK( m_mKey.initialize() );
  ATH_CHECK( m_linkVectorKey.initialize() );
  ATH_CHECK( m_testObjectKey.initialize() );

  m_testObject = new TestDataObject(10);
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode WriteDataReentrant::execute_r (const EventContext& ctx) const
{
  //this example illustrates how to record objects into the StoreGate(SG)
  //with and without providing a key
  //It then covers the new DataLink class and its usage as a persistable
  //reference among objects in the SG
  //Finally it shows how to use symlinks to record an object using its 
  //concrete type and its base class(es).

  ATH_MSG_INFO ("in execute()");

  //  For algorithms that produce new data objects like this one 
  // the SG works like a bulletin board: the algorithm posts the
  // new object to the SG (optionally providing a "name" for it).
  // The SG assigns a unique identifier to the object and puts it on display
  // alongside others of the same type.
  // At preset intervals all objects are removed from the board to make room
  // for new ones and filed.

  // Part 1: Recording objects to SG 
  SG::WriteHandle<MyDataObj> dobj (m_dobjKey, ctx);
  dobj = CxxUtils::make_unique<MyDataObj>(1);

  //now we create a second MyDataObj instance...
  //...try to record it as we did for the first. Since dobj2 is also a
  //MyDataObj we expect to see an error 
  ATH_MSG_WARNING ("we expect  an error message here");
  EXPECT_EXCEPTION (std::runtime_error,
                    dobj = CxxUtils::make_unique<MyDataObj>(2));
  ATH_MSG_WARNING ("end of error message");

  //here we go again...
  //... but this time we register the dobj3 using this algo name as key
  auto dobj3 = SG::makeHandle (m_dobjKey3, ctx);
  dobj3 = CxxUtils::make_unique<MyDataObj>(3);

  SG::WriteHandle<TestDataObject> testobj (m_testObjectKey, ctx);
  if (m_testObject->refCount() != 1) std::abort();
  ATH_CHECK( testobj.record (m_testObject) );
  if (m_testObject->refCount() != 2) std::abort();

#if 0  
  {
    SG::WriteHandle<MyDataObj> dobj4 (m_dobjKey4, ctx);
    ATH_CHECK( dobj4.recordOrRetrieve (CxxUtils::make_unique<MyDataObj>(4)) );
    MyDataObj* pp = &*dobj4;
    ATH_CHECK( dobj4.recordOrRetrieve (CxxUtils::make_unique<MyDataObj>(4)) );
    assert (pp == &*dobj4);
  }
#endif
  
  ///////////////////////////////////////////////////////////////////////

  // Part 2: storing collections in the SG

  SG::WriteHandle<DataVector<MyContObj> > cobj (m_cobjKey, ctx);
  ATH_CHECK( cobj.record (CxxUtils::make_unique<DataVector<MyContObj> >()) );
  cobj->reserve(10);
  cobj->push_back (CxxUtils::make_unique<MyContObj> (11.3, 132));
  cobj->push_back (CxxUtils::make_unique<MyContObj> (41.7, 291));

  // as above with a vector of integers
  SG::WriteHandle<std::vector<float> > vFloat (m_vFloatKey, ctx);
  vFloat = CxxUtils::make_unique<std::vector<float> >();
  vFloat->push_back(1.0);
  vFloat->push_back(2.0);
  vFloat->push_back(3.0);

  SG::WriteHandle<MapStringFloat> m (m_mKey, ctx);
  ATH_CHECK( m.record (CxxUtils::make_unique<MapStringFloat>()) );
  (*m)["uno"]=1.0;
  (*m)["due"]=2.0;

  ///////////////////////////////////////////////////////////////////////
  //
  // Part 3: Data Links
  //
  // Data links are persistable references. Typically they will replace 
  // pointers from one object in the event to another, hence they have
  // the usual "pointer" syntax (op *, op ->, op !, ...)
  // (to be pedantic they have the moniker syntax as they don't define ++, --)
  // For example in Atlfast::Cell
  // class Cell {
  //   ...
  // private:
  //    std::vector<const HepMC::Particle*> m_particles;
  // };
  // m_particles would become
  //    std::vector<HepMCLink<Particle>::type > m_particles;
  //
  // To understand data links it is important to observe that the SG 
  // is not able to identify every single object in the event.
  // In particular individual elements of a container (the MyContObj 
  // in the vector above or the Particles in McEventCollection) are not
  // recorded individually into SG, only their owner (the container) is.
  // For this reason data links are supported by two class templates:
  //  DataLink<DATAOBJ> is a link to a data object i.e. an object
  //   identified by SG
  //  ElementLink<CONTAINER> is a link to an element of a container (which 
  //   in turn is a SG data object)
  // To make a DataLink persistable we need to provide its SG identifier 
  //  (its type and key). 
  // To make an ElementLink persistable we need two pieces of information: 
  //  i) the SG identifier (type/key) of the data object owning the element 
  // ii) the identifier of the contained object within the owner
  // For example when we write out a link to the third element of 
  // our vector<float> we want to write the SG id of the vector 
  // and the index (2) of the element in the vector
  //
  // Since the indexing mechanism of a container depends on the container
  // type we need to specialize ElementLink for different containers
  // Elements of STL sequences (lists, vectors, deques but also DataVector 
  // and DataList) are dealt with automatically. 
  // For other STL or STL-like containers, the data object designer (or the
  // client must "help" ElementLink identifying the type of container
  // Three macros in the header StoreGate/DeclareIndexingPolicy.h
  // are provided to this end:
  //  for sequences use CONTAINER_IS_SEQUENCE( MySequence )
  //  for sets use CONTAINER_IS_SET( MySet )
  //  for maps and related containers use CONTAINER_IS_MAP( MyMap )
  // 
  // It is also possible to define custom specializations of ElementLink
  //  for custom containers (e.g. GenParticleLink for McEventCollection). 
  //
  //Enough! Let's now create our first DataLink. It refers to the first
  // MyDataObj we recorded. DataLink constructors use references 
  // as input arguments
  //
  // DataLink referring to a storable object
  //
  DataLink<MyDataObj> dobjLink(*dobj);
  //since dobj is identifiable in the SG a reference to it is all we need

  SG::WriteHandle<MyDataObj> dobj2 (m_dobjKey2, ctx);
  dobj2 = CxxUtils::make_unique<MyDataObj> (2);

  //Otherwise one could first create an empty link
  DataLink<MyDataObj> dobjLink2;
  //and later on set it to refer to its target
  dobjLink2.toStorableObject(*dobj2);


  // Added Aug 30, 2001  HMA 
  // DataLink made from a key. This should work for either an object
  // already registered in the store, or an object that has a
  // persistency for it.  we use dobj3, which is an object registered with 
  // a key. 

  DataLink<MyDataObj> dobjLink3(name()); 
  // now access it.  DataLink will do a retrieve to get it from the store. 
  dobjLink3->val(); 

  //
  // ElementLinks referring to contained objects
  //
  typedef ElementLink<std::vector<float> > VecElemLink;


  VecElemLink thirdElementLink(*vFloat, 2);  //THIS CRASHES SUN CC

  //sometimes we would not know the index of the element we want to refer to
  
  //reference to an element of the vector
  float& anElement = vFloat->operator[](0);

  //starting from an empty link
  VecElemLink aLink;
  //we can refer it to its target without knowing its index
  aLink.toContainedElement(*vFloat, anElement);
  //it is better though to remember that toContainedElement for an
  //ElementLink performs a linear search of "anElement" into "vFloat".
  // If vFloat has a million elements think twice before using 
  // toContainedElement!

  SG::WriteHandle<std::list<VecElemLink> > pLinkList (m_pLinkListKey, ctx);
  pLinkList = CxxUtils::make_unique<std::list<VecElemLink> >();
  pLinkList->push_back(aLink);
  pLinkList->push_back(thirdElementLink);

  //
  // Part 3b: create a vector of links to the elements of the map above
  //        
  typedef ElementLink<MapStringFloat> MapElemLink;
  SG::WriteHandle<std::vector<MapElemLink> > linkVector (m_linkVectorKey, ctx);
  linkVector = CxxUtils::make_unique<std::vector<MapElemLink> >();
  linkVector->push_back(MapElemLink(*m, "uno"));
  MapElemLink mLink;
  mLink.toContainedElement(*m, (*m)["due"]);
  linkVector->push_back(mLink);

  // Part 4
  // make a link to dobj as its base class after it has been registered 
  // this allows later to retrieve different concrete types as a common ABC

  const BaseClass * pDO = 0; 
  if ( (evtStore()->symLink(dobj.cptr(), pDO)).isFailure() ) {
    ATH_MSG_ERROR (" could not make link to BaseClass");
    return( onError() );
  }
   
  // make a link as its base class,  with the same name
  if ( ( evtStore()->symLink(dobj3.cptr(), pDO)).isFailure() ) {
    ATH_MSG_ERROR (" could not make link to BaseClass");
    return( onError() );
  }

  // Part 5
  // finally dump the structure of the StoreGate before returning
  ATH_MSG_INFO (" registered all data objects");
  ATH_MSG_INFO (" StoreGate structure before returning from execute \n"
		<< evtStore()->dump());

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode WriteDataReentrant::finalize()
{
  ATH_MSG_INFO ("in finalize()");
  return StatusCode::SUCCESS;
}


StatusCode WriteDataReentrant::onError() const
{
  ATH_MSG_ERROR ("Dumping StoreGate after error occurred\n" 
		 << evtStore()->dump());
  return StatusCode::FAILURE;
}

