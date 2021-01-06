// this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJET_TEST_JETFACTORY_H
#define XAODJET_TEST_JETFACTORY_H

#include <map>
#include <vector>
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

#ifndef XAOD_STANDALONE
// allows to test EL 
#include "AthLinks/ElementLink.h"
#include "SGTools/TestStore.h" 
using namespace SGTest;
#else
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/TActiveStore.h"
#endif


namespace xAOD {
  namespace JetTests {


    template<typename T>
    void record ATLAS_NOT_THREAD_SAFE (T* cont, const std::string & name) {
#ifndef XAOD_STANDALONE
  // *********************** init in Athena 
# ifdef SGTOOLS_CURRENTEVENTSTORE_H
  initTestStore();
# else
  SG::getDataSourcePointerFunc = getTestDataSourcePointer;
# endif
  store.record(cont, name); 
  store.record((xAOD::IParticleContainer*)cont, name);  // also store the container as an IParticleContainer
#else
  // *********************** init in standalone 

  gErrorIgnoreLevel = kWarning;
  assert( xAOD::Init() );
  // Create a TEvent object:
  xAOD::TEvent eventstore(xAOD::TEvent::kClassAccess);

  xAOD::TStore tds;
  xAOD::TStore* transDataStore = xAOD::TActiveStore::store();

  assert( transDataStore != 0 );

  tds.record(cont, name).ignore();
 
#endif
      
    }



    /// Create and return an empty JetContainer. JetContainer has an AuxContainer associated and is recorded in an event store so its elements can be used as ElementLink.
    JetContainer * createEmptyJetContainer ATLAS_NOT_THREAD_SAFE (const std::string & name)  {
      JetContainer * jetCont = new JetContainer();
      JetAuxContainer* aux = new xAOD::JetAuxContainer();
      jetCont->setStore(aux);
      record(jetCont, name);
      return jetCont;
    }
    


    /// Fill input JetContainer with new jets which 4-momentum are given by jet4moms
    void fillJets(JetContainer & jetCont,  const  std::vector<xAOD::JetFourMom_t>& jet4moms ){      
      for( const xAOD::JetFourMom_t &v : jet4moms ){
	jetCont.push_back(new xAOD::Jet());
	xAOD::Jet *jet = jetCont.back();
	jet->setJetP4( v ); // set the P4 of the last inserted jet
      }  
      
    }

    /// Fill input JetContainer with new jets which 4-momentum are given by jet4moms and with attributes as given by the map "jetAttMap". It is expected that vectors in this map have same lenght as jet4moms
    void fillJets(JetContainer & jetCont,  const  std::vector<xAOD::JetFourMom_t>& jet4moms,
		  const std::map<std::string, std::vector<float>> jetAttMap
		  ){      
      int i=0;
      for( const xAOD::JetFourMom_t &v : jet4moms ){
	jetCont.push_back(new xAOD::Jet());
	xAOD::Jet *jet = jetCont.back();
	jet->setJetP4( v ); // set the P4 of the last inserted jet
	for( const auto& attpair : jetAttMap) { jet->setAttribute<float>(attpair.first, attpair.second[i]);}
	i++;
      }  
      
    }


    /// Fill input JetContainer with a list of test jets
    void fillStandardTestJets(JetContainer& jetCont){
      static const std::vector<xAOD::JetFourMom_t> jet4moms =
	{
	 xAOD::JetFourMom_t(40000, 0, 1, 10000 ),
	 xAOD::JetFourMom_t(40000, 0.01, 2, 10000 ),
	 xAOD::JetFourMom_t(40000, 0.01, 2, 0.),
	 xAOD::JetFourMom_t(40000, 0.2, 2, 10000.),
	 xAOD::JetFourMom_t(40000, 1.0, 2, 10000.),
	 xAOD::JetFourMom_t(40000, 1.001, 2, 10000.),
	 xAOD::JetFourMom_t(40000, 2., 2, 10000.),
	 xAOD::JetFourMom_t(40000, 3., 2, 10000.),
	 xAOD::JetFourMom_t(40000, 3.22, 2, 10000.),
	 xAOD::JetFourMom_t(40000, 4., 2, 10000.),
	 xAOD::JetFourMom_t(40000, 5., 2, 10000.),
	 xAOD::JetFourMom_t(40000, 5.5, 2, 10000.),
	 
	 xAOD::JetFourMom_t(0.1, 1.0, 2, 10000.),
	 xAOD::JetFourMom_t(100000, 1.0, 2, 10000.),
	 xAOD::JetFourMom_t(500000, 1.0, 2, 10000.),
	 xAOD::JetFourMom_t(1000000, 1.0, 2, 10000.),
	 xAOD::JetFourMom_t(10000000, 1.0, 2, 10000.),
	 
	 xAOD::JetFourMom_t(100000, 1.0, 2, 100000.),
	 xAOD::JetFourMom_t(100000, 1.0, 2, 1000000.)
	};
      
      std::map< std::string, std::vector<float> > jetAttMap =
	{ { "Width", std::vector<float>(jet4moms.size(), 0.123 )} };
      fillJets(jetCont, jet4moms, jetAttMap);

      // also set scales :
      for(Jet* jet: jetCont){
	jet->setJetP4(xAOD::JetScale::JetConstitScaleMomentum, 0.9*jet->jetP4());
	jet->setJetP4(xAOD::JetScale::JetEMScaleMomentum, 0.7*jet->jetP4());
      }
    }
    
    
    
    /// Create and return an empty CaloClusterAuxContainer. CaloClusterAuxContainer has an AuxContainer associated and is recorded in an event store so its elements can be used as ElementLink.
    CaloClusterContainer * createEmptyCaloClusterContainer ATLAS_NOT_THREAD_SAFE (const std::string & name) {
      CaloClusterContainer * clustCont = new CaloClusterContainer();
      xAOD::CaloClusterAuxContainer* aux = new xAOD::CaloClusterAuxContainer();
      clustCont->setStore(aux);
      record(clustCont, name);
      return clustCont;
    }
    
    
    /// Fill input CaloClusterAuxContainer with a list of test clusters (with only their default and Raw 4-momentum set)
    void fillStandardTestClusters ATLAS_NOT_THREAD_SAFE (CaloClusterContainer & clustCont){
      
      for(int i=0; i<10; i++){
	xAOD::CaloCluster *cl = new xAOD::CaloCluster();
	clustCont.push_back(cl);
	
#define SETCLUSTERMOM( E, eta, phi ) cl->setE(E);cl->setEta(eta);cl->setPhi(phi);cl->setM(0)
#define SETCLUSTERRAWMOM( E, eta, phi ) cl->setRawE(E);cl->setRawEta(eta);cl->setRawPhi(phi);cl->setRawM(0)
	SETCLUSTERMOM( 1000+i*1.1, -5+i*0.1, 0);
	SETCLUSTERRAWMOM( 500+i*2.2, -5+i*0.1, 0);
	
      }
    }
    
    
  }
}

#endif

