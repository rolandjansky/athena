/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTAUXDYNREADER_H
#define ROOTAUXDYNREADER_H

#include "AthContainers/AuxStoreInternal.h" 
#include "RootAuxDynIO/RootAuxDynIO.h" 

#include "TClass.h"
#include "TTree.h"

#include <map>
#include <string>


class RootAuxDynReader : public IRootAuxDynReader
{
public :

  struct BranchInfo
  {
    enum Status { NotInitialized, Initialized, TypeError, NotFound };

    TBranch*      branch = 0;
    TClass*       tclass = 0;

    TClass*       SE_tclass = 0;
    EDataType     SE_edt = kOther_t;
    
    bool          isPackedContainer = false;
    bool          needsSE = false;
    enum Status   status = NotInitialized;

    SG::auxid_t   auxid;
    std::string   attribName;

    void setAddress(void* data) const;
  };

  RootAuxDynReader(TBranch *, int store_holder_offset);

  
  virtual void addReaderToObject(void* object, size_t ttree_row, std::recursive_mutex* iomtx = nullptr );

  void init(bool standalone);
  const BranchInfo& getBranchInfo(const SG::auxid_t& auxid, const SG::AuxStoreInternal& store);


  void  addBytes(size_t bytes);

  size_t getBytesRead();

  void resetBytesRead(); 

  const SG::auxid_set_t& auxIDs() const;

protected:
  // map of attribute name to TBranch* as read from the file
  std::map<std::string, TBranch*>       m_branchMap;
  // map auxid -> branch info. not sure if it can be different from m_branchMap
  std::map<SG::auxid_t, BranchInfo>     m_branchInfos;
  // auxids that could be found in registry for attribute names from the m_branchMap
  SG::auxid_set_t                       m_auxids;
  
  TTree*                                m_tree = 0;
  std::string                           m_baseBranchName;
  // counter for bytes read
  size_t                                m_bytesRead = 0;
  // offset of the AxuStoreHolder base class in the objects read by the Reader
  int                                   m_storeHolderOffset = -1;
  bool                                  m_initialized = false;
  std::string                           m_key;
};


#endif
