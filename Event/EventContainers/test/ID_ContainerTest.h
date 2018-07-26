/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  virtual int initialize() ; 
  virtual int finalize()  ; 

  virtual int execute(); 

private: 
  int m_ncollections; 
  int m_nskip; 
  int m_nhits; 
  int m_test; 
  IDC_TEST::MyCollectionContainer* m_container;   

} ;
  


