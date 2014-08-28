/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
// PAUhggUserDataTool.h (c) 
//////////////////////////////////////////////////////////////////////


#ifndef PHOTONANALYSISUTILS_HGGUSERDATATOOL_H
#define PHOTONANALYSISUTILS_HGGUSERDATATOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "PhotonAnalysisUtils/IPAUhggUserDataTool.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "TTree.h"

#include <iostream>

class PAUhggUserDataTool : public AthAlgTool, virtual public IPAUhggUserDataTool
{
 public:
  
  /** AlgTool like constructor */
  PAUhggUserDataTool(const std::string&,const std::string&,const IInterface*);
  
  /**Virtual destructor*/
  virtual ~PAUhggUserDataTool();
  
  /** AlgTool initialize method.*/
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();
  
  virtual IPAUhggUserDataTool::UserData & accessUserData() ;
  virtual void saveUserData() ;

  virtual bool DumpingNtuple() { return m_dumpNtuple; };

 private:
  
  StatusCode init_tree() ;
    
  template <class T> void addBranch(const std::string &branchname, T &obj, const std::string &leaflist) {
    if ( m_udtree ) {
      m_udtree->Branch(branchname.c_str(), &obj, leaflist.c_str());
    } else {
      // std::cout << ">>>> HggUserData TTree not booked <<<<" << std::endl ;
    }
  }

  template <class T> void addBranch(const std::string &branchname, T *&obj) {
    obj = new T();
    if ( m_udtree ) {
      m_udtree->Branch(branchname.c_str(), &obj); 
    } else {
      // std::cout << ">>>> HggUserData TTree not booked <<<<" << std::endl ;
    }   
  }

  bool m_dumpNtuple ;
  TTree* m_udtree ;
  bool m_firstEvt ;
  std::string m_StreamName, m_TreeName ;
  IPAUhggUserDataTool::UserData m_ud ;

  ITHistSvc * m_thistSvc;

};


#endif // PHOTONANALYSISUTILS_HGGUSERDATATOOL_H
