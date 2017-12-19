/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "AthLinks/AssociationMap.h"
#include "SGTools/CurrentEventStore.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "CxxUtils/checker_macros.h"
#include <iostream>
#include <vector>


#include "SGTools/TestStore.h"
using namespace SGTest;


class Cluster
{
public:
  Cluster() : m_e(0.) { };
  Cluster(double e) : m_e(e) { };
  ~Cluster() { };
  double getE() const { return m_e; }
private:
  double m_e;
};

template <class T>
struct DV : public std::vector<T*>
{
  typedef T base_value_type;
};

class ClusterContainer : public DV< Cluster >
{
public:
  ClusterContainer() : DV< Cluster >() { };
  virtual ~ClusterContainer() { };
};

CLASS_DEF( ClusterContainer, 12345, 1)

class Track
{
public:
  Track() : m_p(0.) { };
  Track(double p) : m_p(p) { };
  ~Track() { };
  double getP() const { return m_p; }
private:
  double m_p;
};

class TrackContainer : public DV< Track >
{
public:
  TrackContainer() : DV< Track >() { };
  virtual ~TrackContainer() { };
};

CLASS_DEF( TrackContainer, 54321, 1 )

class PTAss
  : public AssociationMap< ClusterContainer, TrackContainer >
{
public:

  PTAss() : AssociationMap< ClusterContainer, TrackContainer >() { }
  virtual ~PTAss() { }
};

CLASS_DEF( PTAss , 94601450 , 1 )

class TTAss : public AssociationMap<TrackContainer,TrackContainer>
{
public:
  TTAss() : AssociationMap< TrackContainer, TrackContainer >() { }
  virtual ~TTAss() { }
};

CLASS_DEF( TTAss, 67890, 1 )


void test1 (SGTest::TestStore& store)
{
  std::cout << " *** AssociationMap test in progress: " << std::endl;

  std::cout << "Build fake data and associations:" << std::endl;

  ClusterContainer* cCont = new ClusterContainer();
  cCont->push_back(new Cluster(1.));
  cCont->push_back(new Cluster(2.));
  cCont->push_back(new Cluster(3.));
  std::cout << "Cluster Container...............: " << cCont->size() 
	    << " clusters" << std::endl;

  TrackContainer* tCont = new TrackContainer();
  tCont->push_back(new Track(-1.));
  tCont->push_back(new Track(-2.));
  tCont->push_back(new Track(-3.));
  std::cout << "Track Container.................: " << tCont->size() 
	    << " tracks" << std::endl;

  store.record (cCont, "cCont");
  store.record (tCont, "tCont");

  PTAss* aMap = new PTAss();
  size_t tCtr = 0;
  size_t cCtr = 0;

  /////////////////////////////////////////////////
  ///  Exploring AssociationMap non-const interface
  ///

  ClusterContainer::const_iterator cEnd  = cCont->end();

  for ( ClusterContainer::const_iterator cIter = cCont->begin(); 
	cIter != cEnd; 
	++cIter ) {
    for ( tCtr=0; tCtr<tCont->size(); ++tCtr ) {
      const Track* aTrack = (*tCont)[tCtr];
      std::cout << "Associate Clusters and Tracks...: [" << cCtr 
		<< "," << tCtr
		<< "] with data ["
		<< (*cIter)->getE()
		<< ","
		<< aTrack->getP()
		<< "]"
		<< std::endl;
      try {
	if ( 0 == tCtr ) {
	  aMap->addAssociation(cCont,(*cCont)[cCtr],tCont,(*tCont)[tCtr]);
	}
	else if ( 1 == tCtr ) {
	  aMap->addAssociation(ElementLink<ClusterContainer> ("cCont", cCtr),
                               ElementLink<TrackContainer> ("tCont", tCtr));
	}
        else {
	  aMap->addAssociation(cCont,cCtr,tCont,tCtr);
	}
      } catch(std::exception& error) {
	std::cerr << "Caught std::exception:" << std::endl
		  << error.what()
		  << std::endl;
      }
    }
    ++cCtr;
  }

  /////////////////////////////////////////////////
  ///  Exploring AssociationMap const interface
  ///

  const PTAss   * const cstMap    = aMap;
  const Cluster * const myCluster = (*cCont)[0];
  const Track   * const myTrack   = (*tCont)[0];

  std::cout << "List of objects in AssociationMap:" << std::endl;
  {
    PTAss::object_iterator cEnd = cstMap->endObject();
    for ( PTAss::object_iterator cIter = cstMap->beginObject();
	  cIter != cEnd;
	  ++cIter ) {
      const Cluster * const theCluster = (*cIter).getObject();
      assert (*(cIter.getObjectLink()) == theCluster);
      std::cout << "\tCluster "
		<< ": " /*<< theCluster->getE()*/ << std::endl;
      PTAss::asso_iterator tEnd = cstMap->endAssociation(theCluster);
      for ( PTAss::asso_iterator tIter = cstMap->beginAssociation(theCluster);
	    tIter != tEnd;
	    ++tIter ) {
	const Track * const theTrack = *tIter;
        assert (*(tIter.getLink()) == theTrack);
	std::cout << "\t\tTrack "
		  << ": " << theTrack->getP() << std::endl;
	const Track * const assoTrack = cstMap->getAssociation( tIter );
	assert( theTrack == assoTrack );

	assert( tIter == cstMap->findAssociation( theCluster, theTrack ) );
	assert( tIter == cstMap->findAssociation( cIter,      theTrack ) );

	assert( cstMap->containsAssociation( theCluster, theTrack ) );
	assert( cstMap->containsAssociation( theTrack ) );

	{
	  std::list<const Cluster*> objectList;
	  assert( cstMap->getObjects( theTrack, objectList ) );
	  assert( tCont->size() == objectList.size() );
	}

	{
	  std::list<const Cluster*> objectList;
	  assert( cstMap->getObjects( cstMap->beginAssociation(theCluster), 
				      objectList ) );
	  assert( tCont->size() == objectList.size() );
	}

	assert( tCont->size() == cstMap->getNumberOfAssociations(cIter) );
	assert( tCont->size() == cstMap->getNumberOfAssociations(theCluster) );
      }
    }
  }

  std::cout << "Check that AssociationMap contains myCluster : "
	    << ( cstMap->containsObject( myCluster ) ? "true" : "false" )
	    << std::endl;

  std::cout << "Number of associations for myCluster"
	    << "= " << cstMap->size( myCluster )
	    << std::endl;
  assert( tCont->size() == cstMap->size( myCluster ) );

  {
    PTAss::asso_list myTracks;
    cstMap->getAssociations( myCluster, myTracks );
    assert( tCont->size() == myTracks.size() );
  }
  {
    PTAss::asso_list myTracks;
    cstMap->getAssociations( cstMap->findObject(myCluster), myTracks );
    assert( tCont->size() == myTracks.size() );
  }

  std::cout << "Number of association objects:" << cstMap->size() << std::endl;
  assert( cCtr == cstMap->size() );


  ////////////////////////////////////////////////////
  ///  Exploring AssociationMap with OBJCONT=ASSOCONT
  ///

  std::cout << std::endl
	    << " *** Test AssociationMap<TrackContainer,TrackContainer> :" 
	    << std::endl;
  TTAss* ttAsso = new TTAss();

  /////////////////////////////////////////////////
  ///  Exploring AssociationMap non-const interface
  ///

  TrackContainer::const_iterator tEnd  = tCont->end();
  unsigned int tIdx = 0;
  for ( TrackContainer::const_iterator tIter = tCont->begin(); 
	tIter != tEnd; 
	++tIter,++tIdx ) {
    unsigned int assoIdx = tIdx+1;
    for ( TrackContainer::const_iterator assItr = tIter+1; 
	  assItr != tEnd; 
	  ++assItr,++assoIdx ) {
      if ( *tIter != *assItr ) {
	std::cout << "Associate Tracks and Tracks...: [" << tIdx 
		  << "," << assoIdx
		  << "] with data ["
		  << (*tIter)->getP()
		  << ","
		  << (*assItr)->getP()
		  << "]"
		  << std::endl;
	if ( tIdx - assoIdx == 1 ) {
	  ttAsso->addAssociation( tCont, *tIter, tCont, *assItr );
	} else {
	  ttAsso->addAssociation( tCont, tIdx,   tCont, assoIdx );
	}
      }
    }//> loop over tracks to be associated
  }//> loop over tracks
  
  /////////////////////////////////////////////////
  ///  Exploring AssociationMap const interface
  ///

  const TTAss * const ttMap   = ttAsso;
  
  assert( tCont->size()-1 == ttMap->size( myTrack ) );

  assert( tCont->size()-1 == ttMap->size() );
}


int main ATLAS_NOT_THREAD_SAFE () 
{
  initTestStore();
  test1 (store);

  return 0;
}
