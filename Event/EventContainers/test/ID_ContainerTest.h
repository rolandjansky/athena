/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// 
// Test algorithm for IdentifiableContainer
// 

namespace IDC_TEST{
class MyCollectionContainer; 
} 


class ID_ContainerTest {
public: 
  ID_ContainerTest(); 

  virtual int initialize(EventContainers::Mode mode) ; 
  virtual int finalize()  ; 

  virtual int execute(EventContainers::Mode mode); 

private: 
  int m_ncollections; 
  int m_nskip; 
  int m_nhits; 
  int m_test; 
  IDC_TEST::MyCollectionContainer* m_container;   

} ;
  


