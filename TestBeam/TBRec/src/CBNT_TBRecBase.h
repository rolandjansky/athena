/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef TBREC_CBNT_TBRECBASE_H
#define TBREC_CBNT_TBRECBASE_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TTree.h"

class HWIdentifier;
class LArOnlineID;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class MsgStream;

class CBNT_TBRecBase : public AthAlgorithm {

 public:
  CBNT_TBRecBase(const std::string & name, ISvcLocator * pSvcLocator);
  virtual ~CBNT_TBRecBase();

  //Standard algo methods
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  virtual StatusCode pre_execute();

  //These needs to be implemented by the deriving class
  virtual StatusCode CBNT_initialize() {return StatusCode::SUCCESS;}
  virtual StatusCode CBNT_execute() {return StatusCode::SUCCESS;}
  virtual StatusCode CBNT_finalize() {return StatusCode::SUCCESS;}
  virtual StatusCode CBNT_clear() {return StatusCode::SUCCESS;}

 private:
  bool m_initialized;

 protected:   
 // wrapper to add branches
 template <class T> void addBranch(const std::string &branchname, T &obj, const std::string &leaflist) {
     m_nt->Branch(branchname.c_str(), &obj, leaflist.c_str());
 }
 template <class T> void addBranch(const std::string &branchname, T *&obj) {
     obj = new T();
     m_nt->Branch(branchname.c_str(), &obj);    
  }
  template <class T> void addBranch(const std::string &branchname, T *&obj, int bufferSize, int splitLevel) {
     obj = new T();
     m_nt->Branch(branchname.c_str(), &obj, bufferSize, splitLevel);    
  }

  enum {NOT_VALID = -999};

  std::string m_ntpath, m_ntTitle;

  //Ntuple pointer
  TTree* m_nt;

  MsgStream* m_log;

  const LArEM_ID* m_emId;
  const LArHEC_ID* m_hecId;
  const LArFCAL_ID* m_fcalId;
  const LArOnlineID* m_onlineId;

};
#endif
