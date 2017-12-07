/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <sys/time.h>
#include <iostream>
#include <iterator>
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AthLinks/ElementLinkVector.h"
#include "AthContainers/ConstDataVector.h"

#include "TestTools/initGaudi.h"

#include "TrigNavigation/TriggerElement.h"

#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/Navigation.icc"
#include "TrigNavigation/RoICacheHelper.h"

#include <boost/type_traits/is_same.hpp>
#include  <boost/type_traits/remove_const.hpp>
#include "TrigSerializeCnvSvc/TrigSerializeConverter.h"

#include "TestTypes.h"
#include "TestUtils.h"

StoreGateSvc* pStore(0);

double interval( struct timeval& begin, struct timeval& end) {
  return (end.tv_sec - begin.tv_sec)*1000. + (end.tv_usec - begin.tv_usec)*1e-3;
}
using namespace HLT;
using namespace TrigNavTest;
/*
void blah( DataVector<B>::const_iterator ) {}

void blah2() {
  BContainer bb;
  blah(bb.begin());
}
*/
/////////////////////////////////////////////////////////////////////////////
//  
//  features retrieval test
//  
//  
/////////////////////////////////////////////////////////////////////////////
template<class T, class C>
int testGetFeaturesOnDistinctTypes(MsgStream& log, Navigation* nav, TriggerElement* te, const std::string& label ) {
  

  const T* singleFromGet = 0;
  const T* singleFromGetRecent = 0;
  std::vector<const T*> multipleFromGet;
  std::vector<const T*> multipleFromGetRecent;
  const T* singleRecentThroughEL(0);
  std::vector<const T*> multipleRecentThroughELV;

  log << MSG::DEBUG << "testGetFeatures test for TE: " << te->getId() 
      << " and features of type " << ClassID_traits<T>::typeName() 
      << " with label " << label
      << endmsg;

  if ( nav->getFeature(te, singleFromGet, label) == false ) {
    log << MSG::ERROR << "falied to get: singleFromGetFeature" << endmsg; return -1;
  }

  const TriggerElement* tsource(0);
  std::string lsource;
  if ( nav->getRecentFeature(te, singleFromGetRecent, label, tsource, lsource) ) {
    if ( tsource == 0 && singleFromGetRecent != 0 ) {
      log << MSG::ERROR << "falied to get: singleFromGetFeature no source found " << singleFromGetRecent << " " << tsource  << endmsg; 
      return -1;
    } 
    if ( tsource ) {
      log << MSG::INFO << "worked to get: endmsg " << tsource->getId() << " label >" << label << "< back label >" << lsource << "< ptr: " <<  singleFromGetRecent << endmsg; 
      
    }
    log << MSG::INFO << "falied to get: singleFromGetFeature" << endmsg;    
  } else {
    if ( !tsource ) {
      log << MSG::ERROR << "in falied to get: singleFromGetFeature no source found " << tsource  << endmsg; 
      return -1;
    }
  }
  
  

  if ( nav->getFeatures(te, multipleFromGet, label) == false ) {
    log << MSG::ERROR << "falied to get: multipleFromGet" << endmsg;  return -1;
  } 
  if ( nav->getRecentFeatures(te, multipleFromGetRecent, label) == false ) {
    if ( label != "" ) {
      log << MSG::ERROR << "falied to get: multipleFromGetRecent" << endmsg; return -1;
    } else 
      log << MSG::INFO << "falied to get: multipleFromGetRecent but OK, label is empty string" << endmsg;
  } 

  typedef ElementLink<C> CEL;
  CEL el;

  if ( nav->getRecentFeatureLink<C, T>(te, el, label) == false ) {
    log << MSG::INFO << "falied to get: singleRecentThroughEL" << endmsg;
  } 
  if (el.isValid())
    singleRecentThroughEL = *el;

    
  typedef ElementLinkVector<C> CELV;
  CELV elv;  
  typename CELV::iterator elvIt;

  if ( nav->getRecentFeaturesLinks<C, T>(te, elv, label) == false ) {    
    if ( label != "" ) {
      log << MSG::ERROR << "falied to get: multipleRecentThroughELV" << endmsg; return -1;
    } else {
      log << MSG::INFO << "falied to get: multipleRecentThroughELV" << endmsg;
    }
  } 
  
  for ( elvIt = elv.begin(); elvIt != elv.end(); ++elvIt ) {
    el = *elvIt;
    const T* t = *el;
    multipleRecentThroughELV.push_back(const_cast<typename C::base_value_type *>(t));
  }
   
  log << MSG::DEBUG << "testGetFeatures passed features retrieval ... checking thier relations" << endmsg; 

   
   // cross check ordering
   if ( singleFromGet != 0 ) {
     if ( multipleFromGet.size() < 1 ) {
       log << MSG::ERROR << "singleFromGet & multipleFromGet give give inconsistent sizes " << multipleFromGet.size() << endmsg;
       return -1 ; 
     }
     if ( singleFromGet != multipleFromGet.back() ) {
       log << MSG::ERROR << "singleFromGet & multipleFromGet give wrong ordering" << endmsg;
       return -1 ;
     }
   }

  if ( singleFromGetRecent != 0 ) {
     if ( singleFromGetRecent != multipleFromGetRecent.back() ) {
       log << MSG::ERROR << "singleFromGetRecent & multipleFromGetRecent give wrong ordering" << endmsg;
       return -1 ;
     }
   }
  
  //cross check if EL based methog give the same
  if ( singleFromGetRecent != singleRecentThroughEL ) {
    log << MSG::ERROR << "singleFromGetRecent != singleRecentThroughEL" << endmsg;
    return -1 ;
  }


  if ( multipleRecentThroughELV.size() != multipleFromGetRecent.size() ) {
    log << MSG::ERROR << "multipleRecentThroughELV.size() " << multipleRecentThroughELV.size() << " != multipleFromGetRecent.size() " <<multipleFromGetRecent.size() << endmsg;
    return -1 ;
  }

  for ( unsigned i = 0; i < multipleRecentThroughELV.size() ; i++ ) {
    if ( multipleRecentThroughELV[i] != multipleFromGetRecent[i] ) {
      log << MSG::ERROR << "multipleRecentThroughELV[i] != multipleFromGetRecent[i] for i: " << i  << endmsg;
      return -1; 
    }
  }

  log << MSG::DEBUG << "testGetFeaturesOnDistinctTypes passed for TE: " << te->getId() 
      << " and features of type " << ClassID_traits<T>::typeName() 
      << " with label " << label
      << " singleGet gave " << ( (singleFromGet) ? 1 : 0 )
      << " multipleGet gave " << multipleFromGet.size()
      << endmsg;

  return 0;
}







/////////////////////////////////////////////////////////////////////////////
// the same as above but when collections are involved



template<class T>
int testGetFeaturesOnSameTypes(MsgStream& log, Navigation* nav, TriggerElement* te, const std::string& label ) {

  const T* singleFromGet(0);
  const T* singleFromGetRecent(0);
  std::vector<const T*> multipleFromGet;
  std::vector<const T*> multipleFromGetRecent;
  T multipleRecentThroughELV;
  multipleRecentThroughELV.clear(SG::VIEW_ELEMENTS);
  T multipleRecentThroughPlain;
  multipleRecentThroughPlain.clear(SG::VIEW_ELEMENTS);

  log << MSG::DEBUG << "testGetFeaturesOnSameTypes test for TE: " << te->getId() 
      << " and features of type " << ClassID_traits<T>::typeName() 
      << " with label " << label
      << endmsg;

  if ( nav->getFeature(te, singleFromGet, label) == false ) {
    log << MSG::ERROR << "falied to get: singleFromGetFeature" << endmsg; return -1;
  }
  if ( nav->getRecentFeature(te, singleFromGetRecent, label) == false ) {
    log << MSG::INFO << "falied to get: singleFromGetRecentFeature" << endmsg; 
  } 
  if ( nav->getFeatures(te, multipleFromGet, label) == false ) {
    log << MSG::ERROR << "falied to get: multipleFromGet" << endmsg;  return -1;
  } 
  if ( nav->getRecentFeatures(te, multipleFromGetRecent, label) == false ) {
     log << MSG::INFO << "falied to get: multipleFromGetRecent" << endmsg;
  } 

  typename std::vector<const T*>::const_iterator pIt;
  for ( pIt = multipleFromGetRecent.begin(); pIt != multipleFromGetRecent.end(); ++pIt) {
    multipleRecentThroughPlain.insert(    multipleRecentThroughPlain.end(), (*pIt)->begin(), (*pIt)->end() ); 
  }

  typedef ElementLink<T> CEL;
  CEL el;

  if ( nav->getRecentFeatureLink<T, T>(te, el, label) == true ) {
    log << MSG::ERROR << "managed to get: singleRecentThroughEL this method for same types should return false" << endmsg; return -1;
  } 
    
  typedef ElementLinkVector<T> CELV;
  CELV elv;  
  typename CELV::iterator elvIt;

  if ( nav->getRecentFeaturesLinks<T, T>(te, elv, label) == false ) {
     log << MSG::INFO << "falied to get: multipleRecentThroughELV" << endmsg;
  } 
  
  for ( elvIt = elv.begin(); elvIt != elv.end(); ++elvIt ) {
    el = *elvIt;
    typedef typename T::base_value_type S;
    const S* t = *el;
    multipleRecentThroughELV.push_back((S*)t);
  }

  log << MSG::DEBUG << "testGetFeatures passed features retrieval ... checking thier relations" << endmsg; 

   
   // cross check ordering
   if ( singleFromGet != 0 ) {
     if ( multipleFromGet.size() < 1 ) {
       log << MSG::ERROR << "singleFromGet & multipleFromGet give give inconsistent sizes " << multipleFromGet.size() << endmsg;
       return -1 ; 
     }
     if ( singleFromGet != multipleFromGet.back() ) {
       log << MSG::ERROR << "singleFromGet & multipleFromGet give wrong ordering" << endmsg;
       return -1 ;
     }
   }

  if ( singleFromGetRecent != 0 ) {
     if ( singleFromGetRecent != multipleFromGetRecent.back() ) {
       log << MSG::ERROR << "singleFromGetRecent & multipleFromGetRecent give wrong ordering" << endmsg;
       return -1 ;
     }
   }
  
  //cross check if EL based methog give the same
  

  if ( multipleRecentThroughELV.size() != multipleRecentThroughPlain.size() ) {
    log << MSG::ERROR << "multipleRecentThroughELV.size() != multipleRecentThroughPlain.size()" << endmsg;
    return -1 ;
  }

  for ( unsigned i = 0; i < multipleRecentThroughELV.size() ; i++ ) {
    if ( multipleRecentThroughELV[i] != multipleRecentThroughPlain[i] ) {
      log << MSG::ERROR << "multipleRecentThroughELV[i] != multipleRecentThroughPlain[i] for i: " << i  << endmsg;
      return -1; 
    }
  } 

  log << MSG::DEBUG << "testGetFeaturesOnSameTypes passed for TE: " << te->getId() 
      << " and features of type " << ClassID_traits<T>::typeName() 
      << " with label " << label
      << " singleGet gave " << ( (singleFromGet) ? 1 : 0 )
      << " multipleGet gave " << multipleFromGet.size()
      << endmsg;

  return 0;
}


template<class T>
int testFindOwners(MsgStream& log, Navigation* nav, T* t, bool isRealyAttached=true) {
  std::vector<const TriggerElement*> tes;
  if (nav->findOwners(t, tes)  == false) {
    log << MSG::ERROR << "find owners failed for : " << t << " of type " << ClassID_traits<T>::typeName()  << endmsg;
      return -1; 
    }
  if ( isRealyAttached && tes.size() == 0 ) {
    log << MSG::ERROR << "find owners found 0 TEs for : " << t << " of type " << ClassID_traits<T>::typeName()  << endmsg;
    return -1; 
  }

  if ( (!isRealyAttached) && tes.size() != 0 ) {      
    log << MSG::ERROR << "find owners worked for : " << t << " of type " << ClassID_traits<T>::typeName()  << endmsg;    
    return -1; 
  }
  return 0;
}




template<class T, class C>
int checkGetFeaturesOnAllTEsOnDistinctTypes(MsgStream& log, Navigation* nav, TriggerElement* te, const std::string& label) {


  if ( testGetFeaturesOnDistinctTypes<T,C>(log, nav, te, label) != 0 )
    return -1;

  std::vector<TriggerElement*> succ = nav->getDirectSuccessors(te);
  std::vector<TriggerElement*>::iterator i;
  
  log << MSG::DEBUG << "looping over successors " << succ.size() << endmsg;
  for ( i = succ.begin(); i != succ.end(); ++i ) {

    if ( checkGetFeaturesOnAllTEsOnDistinctTypes <T,C>(log, nav, *i, label) != 0 )
      return -1;
  }
  return 0;
}


template<class T>
int checkGetFeaturesOnAllTEsOnSameTypes(MsgStream& log, Navigation* nav, TriggerElement* te, const std::string& label) {


  if ( testGetFeaturesOnSameTypes<T>(log, nav, te, label) != 0 )
    return -1;

  std::vector<TriggerElement*> succ = nav->getDirectSuccessors(te);
  std::vector<TriggerElement*>::iterator i;
  
  log << MSG::DEBUG << "looping over successors " << succ.size() << endmsg;
  for ( i = succ.begin(); i != succ.end(); ++i ) {

    if ( checkGetFeaturesOnAllTEsOnSameTypes <T>(log, nav, *i, label) != 0 )
      return -1;
  }
  return 0;
}
//****************************************************************************************
bool construction_test(HLT::Navigation* hns) {
  BEGIN_TEST("Construction");
  TriggerElement* initial =  hns->getInitialNode();
  if ( not initial ) REPORT_AND_STOP("Can not get initial node");
  if ( not HLT::Navigation::isInitialNode(initial) ) REPORT_AND_STOP("Initial node not initial");


  TriggerElement* roi1    =  hns->addRoINode(initial); //roi1->setId(1); // id are for the test only
  TriggerElement* roi2    =  hns->addRoINode(initial); //roi2->setId(2);
  TriggerElement* roi3    =  hns->addRoINode(initial); //roi3->setId(3);
  if ( HLT::Navigation::isInitialNode(roi3) ) REPORT_AND_STOP("The RoI node considered as initial");
  
    
  TriggerElement* roi1eltr1 = hns->addNode(roi1, 11);
  TriggerElement* roi1eltr2 = hns->addNode(roi1, 12);
  TriggerElement* roi1eltr3 = hns->addNode(roi1, 13);
  TriggerElement* roi1eltr4 = hns->addNode(roi1, 14);


  TriggerElement* clu1 = hns->addNode(roi1eltr1, 111); 
  TriggerElement* track1 = hns->addNode(clu1, 1111);   
  TriggerElement* el1 = hns->addNode(track1, 11111); 
  //  el1->attachFeature(new A);

  // another roi
  TriggerElement* roi2mutr1 = hns->addNode(roi2, 21); 
  TriggerElement* roi2mutr2 = hns->addNode(roi2, 22); roi2mutr2->getId();

  REPORT_AND_CONTINUE( "after adding inital nodes" );

  // build big structure
  std::vector<TriggerElement*> telist;
  telist.push_back(roi2mutr1);
  telist.push_back(track1);
  TriggerElement* zz = hns->addNode(telist,   33301);
  zz->setErrorState(true);
  TriggerElement* zz2 = hns->addNode(telist,  33311);  zz2->getId();
  TriggerElement* zz1 = hns->addNode(zz,      33321); 
  /*TriggerElement* zz1prim =*/  hns->addNode(zz1, 33323); 
  /*TriggerElement* zz1prim2 =*/ hns->addNode(telist, 33324, false, true); 

  REPORT_AND_CONTINUE("RoIs made");

  if ( ! HLT::Navigation::haveCommonRoI(roi1eltr1, roi1eltr2) ) 
    REPORT_AND_STOP( "this TEs (2 direct successors) should have indicate that they have common RoI"  );

  
  if ( ! HLT::Navigation::haveCommonRoI(roi1, roi1eltr4) ) 
    REPORT_AND_STOP( "this TEs (RoI and direct successor) should have indicate that they have common RoI"  );

  
  if ( ! HLT::Navigation::haveCommonRoI(roi1, clu1) ) 
    REPORT_AND_STOP( "this TEs (RoI and indirect successor) should have indicate that they have common RoI"  );
  
  
  if ( ! HLT::Navigation::haveCommonRoI(roi1eltr3, el1) ) 
    REPORT_AND_STOP( "this TEs (2 indirect successors) should have indicate that they have common RoI"  );
  
  
  if ( HLT::Navigation::haveCommonRoI(roi2mutr1, el1) ) 
    REPORT_AND_STOP( "this simple TEs (direct successor and indeirect successor) should have indicate that they have distinct RoI"  );


  if ( HLT::Navigation::haveCommonRoI(roi2mutr1, roi1) ) 
    REPORT_AND_STOP( "this simple TEs (direct successor and RoI) should have indicate that they have distinct RoI"  );


  if ( ! HLT::Navigation::haveDistinctRoI(roi3, zz) ) 
    REPORT_AND_STOP( "this simple TEs (RoI and Topo) should have indicate that they have distinct RoI"  );


  if ( ! HLT::Navigation::haveDistinctRoI(roi1, roi2) ) 
    REPORT_AND_STOP( "this simple TEs (RoI and Topo) should have indicate that they have distinct RoI"  );
  

  REPORT_AND_CONTINUE("RoIs Common/Distinct check made");


  // add ghost TES
  TriggerElement* ghost;
  ghost = hns->addNode(telist, 33399, true); ghost->setActiveState(false);
  ghost = hns->addNode(telist, 33399, true); 
  ghost = hns->addNode(telist, 33399, true); 
  ghost = hns->addNode(telist, 33399, true); 
  ghost = hns->addNode(telist, 33399, true); 
  ghost = hns->addNode(telist, 33399, true); 
  ghost = hns->addNode(telist, 33399, true); 

  // build one long branch
  TriggerElement* te1 = roi1eltr1;
  TriggerElement* te2;
  int i;
  for ( i = 0; i < 25; i++ ) { 
    te2 = hns->addNode(te1, 5000+i); 
    te1 = te2;
  }
  
  // add many leafs to one of the RoI
  for ( i = 0; i < 24; i++ ) { 
    te1 = hns->addNode(roi1, 50); 
    // bunch of inactive TEes
    TriggerElement* te3 = hns->addNode(roi1, 60); 
    te3->setActiveState(false);

    // every of it has 5 leafs
    telist.clear();
    for ( int j = 0; j < 5; j++ ) {
      te2 = hns->addNode(te1, te1->getId()+100); 
      telist.push_back(te2);
    }
    // a big join
    te1 = hns->addNode(telist, te1->getId()+1000); 
  }

  std::vector<TriggerElement*> typeVec;
  hns->getAllOfType(50, typeVec);
  if (! HLT::Navigation::haveCommonRoI( typeVec[0], typeVec[1] ) ) 
    REPORT_AND_STOP( "this simple TEs (2 sibling successors) should have indicate that they have distinct RoI"  );
  

  END_TEST;
}

//****************************************************************************************
bool counting_test(HLT::Navigation* hns) {
  BEGIN_TEST("counting");
  

  
  // test queries by type
  std::vector<TriggerElement*> typeVec;
  hns->getAllOfType(50, typeVec);

  if ( typeVec.size() != 24 ) 
    REPORT_AND_STOP( "Query by type incorrect, should have returned 25 TE: " << typeVec.size() );


  typeVec.clear();
  hns->getAllOfType(60, typeVec);
  if ( typeVec.size() != 0 ) 
    REPORT_AND_STOP( "Query by type incorrect, should have returned 0 TE (all are inactive): " << typeVec.size() )

  
  hns->getAllOfType(60, typeVec, false);
  if ( typeVec.size() != 24 ) 
    REPORT_AND_STOP( "Query by type incorrect, should have returned 0 TE (all are inactive, but shouldnot look at the status): " << typeVec.size() );

  REPORT_AND_CONTINUE( "SUCCESS TEST of getAllOfType method" );
  
  if ( hns->countAllOfType(33399) != 6 ) 
    REPORT_AND_STOP( "Count by type incorrect, got active only: " << hns->countAllOfType(33399) << " inestead of " << 6 );


  if ( hns->countAllOfType(33399, false) != 7 ) 
    REPORT_AND_STOP( "Count by type incorrect, got active and inactive: " << hns->countAllOfType(33399) << " inestead of " << 7 );

  
  END_TEST;

}
//****************************************************************************************
bool seeding_test(HLT::Navigation* /*hns*/) {
  BEGIN_TEST("seeding");
  


  
  END_TEST;

}

TriggerElement* getTE(Navigation* hns, int id) {
  std::vector<TriggerElement*> tes;
  hns->getAllOfType(id, tes);
  if ( tes.size() == 1 ) 
    return tes[0];
  return 0;  
}
bool all_not_null(const std::initializer_list<TriggerElement*>& all) {
  for ( TriggerElement* te: all )
    if (te == 0) return false;
  return true;
}

//****************************************************************************************
bool topo_test(HLT::Navigation* hns) {
  BEGIN_TEST("topo");
  TriggerElement* clu1      = getTE(hns, 111);
  TriggerElement* zz        = getTE(hns, 33301);
  TriggerElement* roi1eltr1 = getTE(hns, 11);
  TriggerElement* roi1eltr2 = getTE(hns, 12);
  TriggerElement* roi2 = getTE(hns, 21);
  TriggerElement* roi2mutr2 = getTE(hns, 22);
  
  if ( not all_not_null({clu1, zz, roi1eltr1, roi1eltr2, roi2}) )
    REPORT_AND_STOP("Some TEs are 0");

    if ( HLT::Navigation::isTopological(clu1) != false ) 
      REPORT_AND_STOP ( "clu1 TE is recognized as topological while it is not" );
  
  if ( HLT::Navigation::isTopological(zz) != true ) 
    REPORT_AND_STOP( "zz TE is not recognized as topological while it is " );
  
  REPORT_AND_CONTINUE(  "SUCCESS tests of isTopological method" );
  if ( ! HLT::Navigation::haveCommonSeed(roi1eltr1, roi1eltr2) ) 
    REPORT_AND_STOP( "this TEs (2 direct successors) should have indicate that they have common seed"  );



  if ( ! HLT::Navigation::haveDistinctSeed(roi1eltr1, roi2) ) 
    REPORT_AND_STOP( "this TEs (normal and RoI) should have indicate that they have distinct seed"  );


  if ( HLT::Navigation::getSuccessor(clu1, 33301) != zz ) 
    REPORT_AND_STOP( "getSuccessor is failing to find right TE"  );


  if ( HLT::Navigation::getSuccessor(roi2mutr2, 33301) != 0 ) 
    REPORT_AND_STOP ("getSuccessor fond TE which is wrong"  );



  REPORT_AND_CONTINUE( " of haveCommonSeed method" );

  


  END_TEST;

}

//****************************************************************************************
bool single_feature_test(HLT::Navigation* hns) {
  BEGIN_TEST("single feature operations test");
  TriggerElement* clu1        = getTE(hns, 111);
  TriggerElement* track1      = getTE(hns, 1111);
  TriggerElement* el1      = getTE(hns, 11111);

  TestA* a1 = new TestA(1);
  TestA* a2 = new TestA(2);
  TestA* a3 = new TestA(3);
  TestA* a4 = new TestA(4);
  TestA* a5 = new TestA(5);
  std::string key;
  if ( hns->attachFeature(clu1, a1, Navigation::ObjectCreatedByNew, key, "Single") == false ) 
    REPORT_AND_STOP( "Attach feature returned false " );

  if ( hns->attachFeature(clu1, a2, Navigation::ObjectCreatedByNew, key, "Single") == false ) 
    REPORT_AND_STOP( "Attach second feature failed" );


  std::vector<const TestA*> avec;

  hns->getFeatures(clu1, avec);
  if ( avec.size() != 2 ) 
    REPORT_AND_STOP( "No. of attached features wrong (2): " << avec.size()  );

  
  hns->getFeatures(track1, avec);
  if ( avec.empty() ) 
    REPORT_AND_STOP( "attached features visible in other TEs"  );



  avec.clear();
  hns->attachFeature(clu1, a3, Navigation::ObjectCreatedByNew, key, "ToolTestA");
  hns->attachFeature(clu1, a4, Navigation::ObjectCreatedByNew, key, "ToolB");
 
 
  hns->getFeatures(clu1, avec);
  if ( avec.size() != 4 ) 
    REPORT_AND_STOP ( "No. of attached features wrong, problem with labeled features (4): " << avec.size()  );


  avec.clear();
  hns->getFeatures(clu1, avec, "ToolTestA" );
  if ( avec.size() != 1 )
    REPORT_AND_STOP( "No. of attached features wrong, does not regard labels (1): " << avec.size() );
  
  avec.clear();
  hns->attachFeature(track1, a5, Navigation::ObjectCreatedByNew, key, "ToolB");
  hns->getFeaturesInRoI(el1, avec);
  if ( avec.size() != 5 ) 
    REPORT_AND_STOP( "No. of attached features wrong, does not see all RoI: " << avec.size() );
  
  std::vector<const TestA*> lastA; 
  hns->getRecentFeatures(el1, lastA );
  if ( lastA.empty() ) 
    REPORT_AND_STOP( "getLastFeature retrieves null" );

  if ( lastA.back()->a != 5 ) 
    REPORT_AND_STOP( "getLastFeature retrieves wrong feature"  );

  lastA.clear();
  hns->getRecentFeatures(el1, lastA, "ToolTestA" );
  if ( lastA.empty() ) 
    REPORT_AND_STOP( "getLastFeature retrieves null" );
  if ( lastA.back()->a != 3 ) 
    REPORT_AND_STOP ( "getLastFeature retrieves wrong feature" );

  REPORT_AND_CONTINUE("Get features worked fine");

  END_TEST;
}

//****************************************************************************************
TestBContainer* makeTestB(int value, size_t size=3) {
  TestBContainer * tb = new TestBContainer;
  TestAuxB* aux = new TestAuxB; // this will leak memory but it is more convenient for testing
  tb->setStore(aux);
  for ( size_t i = 0; i < size; ++i )
    tb->push_back(new TestB(value));  

  return tb;
}


bool container_feature_test(HLT::Navigation* hns) {
  BEGIN_TEST("container feature operations test");
  TriggerElement* clu1        = getTE(hns, 111);
  /* TriggerElement* track1 =*/ getTE(hns, 1111);
  TriggerElement* el1      = getTE(hns, 11111);

  TestBContainer *tb1 = makeTestB(1);
  std::string key;
  hns->attachFeature(clu1, tb1, HLT::NavigationCore::ObjectCreatedByNew, key, "Test1");

  TestBContainer * tb2 = makeTestB(2, 2);
  hns->attachFeature(clu1, tb2, HLT::NavigationCore::ObjectCreatedByNew, key, "BContainer2"); // this one has Aux
  
  // get from the derived TE
  const TestBContainer *bret(0);
  hns->getRecentFeature(el1, bret);
  if ( bret == 0 )
    REPORT_AND_STOP( "Failed to get back last container container feature" );

  if ( bret->size() != 2 )
    REPORT_AND_STOP( "Failed to get back correct last container container feature" );
  
  //  if ( bret->getStore() == 0 ) 
  //    REPORT_AND_STOP( "What we got back for the decorated has no decoration" );

  bret = 0;
  hns->getRecentFeature(el1, bret, "Test1");
  if ( bret == 0)
    REPORT_AND_STOP( "getRecentFeature does not respect labels" );

  
  END_TEST;
}




//****************************************************************************************
bool serialize_test(HLT::Navigation* hns, HLT::Navigation* newhns) {
  BEGIN_TEST("serialize test");
  std::vector<uint32_t> serialized;
  std::vector<unsigned> breaks;
  hns->serialize(serialized, breaks);
  

  newhns->deserialize(serialized);
  REPORT_AND_CONTINUE( "serialize/deserialze sequence worked" );


  if ( not HLT::Navigation::isCompatibleTree(newhns->getInitialNode(), hns->getInitialNode() ) ) 
    REPORT_AND_STOP( "serialization/deserialization does not rebuild the the same structure" );


  END_TEST;
}

bool const_attach_test(HLT::Navigation* hns) {
  BEGIN_TEST("const attach test");
  typedef ConstDataVector<TestBContainer> ConstTestBContainer; // this is container of consts
  ConstTestBContainer* cb = new ConstTestBContainer;
  cb->push_back(new TestB(68));
  cb->push_back(new TestB(78));
  std::string key;
  TriggerElement* el1      = getTE(hns, 11111);
 
  if ( hns->attachFeature(el1, cb, HLT::NavigationCore::ObjectCreatedByNew, key, "ConstBContainer") == false )
    REPORT_AND_STOP("attach of ConsTestBContainer failed");

  const ConstTestBContainer* rcb{nullptr};
  if ( hns->getFeature(el1, rcb) == false ) 
    REPORT_AND_STOP("Retrieve of the ConstTestBContainer failed");

  if ( rcb == nullptr ) 
    REPORT_AND_STOP("Retrieve of the ConstTestBContainer failed, the pointer is 0");
  if ( rcb->at(0)->value() != 68 ) 
    REPORT_AND_STOP("Got incorrect container");


  // now try to reuse some TestB object stored in another non-const container
  const TestBContainer* tb2{nullptr};
  TriggerElement* clu1        = getTE(hns, 111);
  const bool stat =  hns->getFeature(clu1, tb2, "BContainer2"); 
  if ( stat == false ) 
    REPORT_AND_STOP("Can not get the non-const TestBContainer object, error");
  if ( tb2 == nullptr ) 
    REPORT_AND_STOP("Can not get the non-const TestBContainer object, pointer is null");

  REPORT_AND_CONTINUE("Got the already attached container "<< tb2->size());

  if ( tb2->size() != 2 )
    REPORT_AND_STOP("Can not get the non-const TestBContainer object, size is to small");
  ConstTestBContainer* cb2 = new ConstTestBContainer;
  cb2->push_back(tb2->at(1));
  if ( cb2->size() != 1 ) 
    REPORT_AND_STOP("Can not push_back into the ConstTestBContainer");

  REPORT_AND_CONTINUE("Prepared ConstTestBContainer wiht contnt already present in another non-cont container" );

  if ( hns->attachFeature(el1, cb2, HLT::NavigationCore::ObjectCreatedByNew, key, "ConstBContainer2") == false )
    REPORT_AND_STOP("Can not attach ConstTestBContainer");
  

  END_TEST;
}


bool external_collection_test(HLT::Navigation* hns) {
  BEGIN_TEST("external_collection test");
  TestBContainer* dav = new TestBContainer;
  dav->push_back(new TestB(1));
  dav->push_back(new TestB(2));
  
  if ( pStore->record(dav, "HLT_external").isFailure() )
    REPORT_AND_STOP("Failed to record in SG");

  hns->associateExternalCollection<TestBContainer>("external");
  // do it twice to see if it gives warnings
  hns->associateExternalCollection<TestBContainer>("external");
  END_TEST;
}


//****************************************************************************************
int main () {
  using std::cerr;
  using std::endl;
  using std::hex;
  using std::dec;
  //  blah2();

  //  using namespace Athena_test;


  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("test.txt",  pSvcLoc)) {
    cerr << "ERROR This test can not be run" << endl;
    return 0;
  }
  assert(pSvcLoc);


  MsgStream log(Athena::getMessageSvc(), "HLTNavigation_test");
  msglog = &log;





  if( pSvcLoc->service("StoreGateSvc", pStore, true).isSuccess() ) {
    log << MSG::DEBUG << "SG pointer: " << pStore << endmsg;
  } else REPORT_AND_STOP( "no SG available" );


  IToolSvc* toolSvc;

  if( pSvcLoc->service("ToolSvc", toolSvc, true).isSuccess()  ) {
    log << MSG::DEBUG << "ToolSvc pointer: " << toolSvc << endmsg;
  } else 
    ABORT ( "no ToolSvc available" );


  HLT::Navigation* hns;
  IAlgTool* algTool;
  if ( toolSvc->retrieveTool("HLT::Navigation/Navigation", algTool).isSuccess() ) {
    log << MSG::DEBUG << "OK navigation tool retrieved" << endmsg;
    hns = dynamic_cast< HLT::Navigation*>(algTool);
    if ( hns ) {
      log << MSG::DEBUG << "OK navigation casted" << endmsg;    
    } else 
      ABORT( "navigation cast failed" );    

  } else ABORT ("navigation tool NOT retrieved" );


  hns->reset();
  
  if ( !construction_test(hns) ) 
    ABORT("TEs construction test failed");


  if ( !counting_test(hns) ) 
    ABORT("Counting test failed");


  if ( !topo_test(hns) ) 
    ABORT("Topo test failed");


  if ( !single_feature_test(hns) ) 
    ABORT("Single feature manipulation");

  if ( !container_feature_test(hns) ) 
    ABORT("Container feature manipulation");

  if ( !const_attach_test(hns) ) {
    ABORT("ConstDV attaching test failed");
  }
  //  log << MSG::DEBUG << pStore->dump() << endmsg;
  if ( !external_collection_test(hns) ) {
    ABORT("ttest with externaly provided collection failed");
  }



  HLT::Navigation* newhns(0);
  if ( toolSvc->retrieveTool("HLT::Navigation/Navigation2", algTool).isSuccess() ) {
    REPORT_AND_CONTINUE( "OK navigation tool retrieved" );
    newhns = dynamic_cast< HLT::Navigation*>(algTool);
  }
  if ( !serialize_test(hns, newhns) )
    ABORT( "Serialization test failed" );


  
  
  REPORT_AND_CONTINUE( "SUCCESS test passed OK " );

  
  return 0;

}

