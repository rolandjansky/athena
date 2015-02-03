/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "WriteData.h"

#include <list>
#include <vector>

#include "AthExStoreGateExample/MyDataObj.h"
#include "MyContObj.h"
#include "MapStringFloat.h"
#include "StoreGateExample_ClassDEF.h"

#include "DataModel/DataVector.h"
#include "StoreGate/StoreGateSvc.h"
#include "DataModel/DataLink.h"
#include "DataModel/ElementLink.h"

#include "AthenaKernel/DefaultKey.h"
#include "AthenaKernel/errorcheck.h"

/////////////////////////////////////////////////////////////////////////////

WriteData::WriteData(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode WriteData::initialize(){
  ATH_MSG_INFO ("in initialize()");
  //locate the StoreGateSvc and initialize our local ptr
  StatusCode sc = evtStore().retrieve();
  if (!sc.isSuccess()) ATH_MSG_ERROR ("Could not find StoreGateSvc");
  return sc;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode WriteData::execute() {
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

  // Part 1: Recording objects to the SG 
  //         We create a MyDataObj on the heap (using "new")
  //         (so that later we can pass ownership to SG)
  //
  MyDataObj *dobj = new MyDataObj;
  dobj->val(1);

  // now we record dobj. SG will identify dobj as the first obj
  //  of type MyDataObj
  StatusCode sc = evtStore()->record(dobj, SG::DEFAULTKEY, false);  
  //         Remember NEVER delete an object recorded in the SG:
  //         SG owns the object after it has been recorded
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR (" could not register dobj");
    return( onError() );
  }

  //now we create a second MyDataObj instance...
  MyDataObj *dobj2 = new MyDataObj;
  dobj2->val(2);

  //...try to record it as we did for the first. Since dobj2 is also a
  //MyDataObj we expect to see an error printed
  ATH_MSG_WARNING ("we expect  an error message here");
  sc = evtStore()->record(dobj2, SG::DEFAULTKEY, false);
  ATH_MSG_WARNING ("end of error message");
  if ( !sc.isFailure() ) {
    ATH_MSG_ERROR
      (" allowed to register two default objects for type MyDataObj");
    return( onError() );
  }
  // The error handling will have deleted dobj2.

  //here we go again...
  MyDataObj *dobj3 = new MyDataObj;
  dobj3->val(3);

  //... but this time we register the dobj3 using this algo name as key
  //sc = evtStore()->record(dobj3, name(), true);  
  sc = evtStore()->record(dobj3, name());  
  //the last argument of record, which for now defaults to true, is "allowMods"
  //by setting it to true we allow downstream algorithms to modify dobj3
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR (" could not register Data Object");
    return( onError() );
  }
  //now dobj3 is known both as the default MyDataObj and the MyDataObj
  //with key "WriteData"
  //Notice that while in this case we used a std::string as key, this
  //need not be the case. Any type satisfying the constraints specified
  //in StoreGate/constraints/KeyConcept.h is a valid key
  //Currently (release 1.3.5) KeyConcept requires a key to be convertible 
  //to and from a std::string (hence a string is a valid key)
  //From release 2.0.0 (or 1.3.6) the only requirement on the key type
  //will be to be sortable, i.e. to provide a "less than" operator <
  

  ///////////////////////////////////////////////////////////////////////

  // Part 2: storing collections in the SG

  //first we create two instances on MyContObj
  //(they can be created on the stack as std::vector copies them anyway

  MyContObj* mco1 = new MyContObj;
  MyContObj* mco2 = new MyContObj;
  mco1->set(11.3, 132);
  mco2->set(41.7, 291);

  DataVector<MyContObj> *cobj = new DataVector<MyContObj>;
  cobj->reserve(10);
  cobj->push_back(mco1);
  cobj->push_back(mco2);  // if I push back this, it core dumps in map record

  //finally we record the list itself. As before we don't need to specify 
  //a key. This make sense for collections, as in many applications (Atlfast)
  //there is only one instance of each collection in the whole app
  sc = evtStore()->record(cobj, SG::DEFAULTKEY, false);  
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR (" could not register list<MyContObj>");
    return( onError() );
  }

  //
  // as above with a vector of integers
  //        

  //again notice how the vector is created on the heap
  std::vector<float> *vFloat = new std::vector<float>;
  vFloat->push_back(1.0);
  vFloat->push_back(2.0);
  vFloat->push_back(3.0);

  sc = evtStore()->record(vFloat, SG::DEFAULTKEY, false);  
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR (" could not register vector<float>");
    return( onError() );
  }

  //
  // now we record a map 
  //        

  MapStringFloat *m = new MapStringFloat;
  (*m)["uno"]=1.0;
  (*m)["due"]=2.0;

  sc = evtStore()->record(m, SG::DEFAULTKEY, false);  
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR (" could not register MapStringFloat");
    return( onError() );
  }

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

  dobj2 = new MyDataObj;
  dobj2->val(2);
  CHECK( evtStore()->record(dobj2, "dobj2") );

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


  //Now let's put all this VecElemLink in a list 
  std::list<VecElemLink> *pLinkList = new std::list<VecElemLink>;
  pLinkList->push_back(aLink);
  pLinkList->push_back(thirdElementLink);

  //and finally let's record the list of links into SG
  sc = evtStore()->record(pLinkList, name(), false);  
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR (" could not register the list of DataLinks");
    return( onError() );
  }
  //
  // Part 3b: create a vector of links to the elements of the map above
  //        
  typedef ElementLink<MapStringFloat> MapElemLink;
  std::vector<MapElemLink>* linkVector = new std::vector<MapElemLink>;

  linkVector->push_back(MapElemLink(*m, "uno"));
  MapElemLink mLink;
  mLink.toContainedElement(*m, (*m)["due"]);
  linkVector->push_back(mLink);

  sc = evtStore()->record(linkVector, SG::DEFAULTKEY, false);  
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR (" could not register the vector of DataLinks");
    return( onError() );
  }

  // Part 4
  // make a link to dobj as its base class after it has been registered 
  // this allows later to retrieve different concrete types as a common ABC 

  const BaseClass * pDO = 0; 
  if ( (evtStore()->symLink(dobj, pDO)).isFailure() ) {
    ATH_MSG_ERROR (" could not make link to BaseClass");
    return( onError() );
  }
   
  // make a link as its base class,  with the same name
  if ( ( evtStore()->symLink(dobj3, pDO)).isFailure() ) {
    ATH_MSG_ERROR (" could not make link to BaseClass");
    return( onError() );
  }

  // Part 5
  // finally dump the sturcture of the StoreGate before returning
  ATH_MSG_INFO (" registered all data objects");
  ATH_MSG_INFO (" StoreGate structure before returning from execute \n"
		<< evtStore()->dump());

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode WriteData::finalize() {

  ATH_MSG_INFO ("in finalize()");
  return StatusCode::SUCCESS;
}


StatusCode WriteData::onError() {

  ATH_MSG_ERROR ("Dumping StoreGate after error occurred\n" 
		 << evtStore()->dump());
  return StatusCode::FAILURE;
}

