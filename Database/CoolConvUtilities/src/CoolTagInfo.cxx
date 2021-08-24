/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// CoolTagInfo.cxx - implementation of AtlCoolCopy helper class
// Richard Hawkings, 29/2/08

#include "CoolTagInfo.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IFolderSet.h"
#include <iostream>

CoolTagInfo::CoolTagInfo(const cool::IDatabasePtr& db,const std::string& folder,
			 const std::string& destfolder,
		     const std::string& sourcetag,const std::string& desttag,
			 const std::string& taglabel) {
  m_folder=destfolder;
  m_tag=desttag;
  if (db->existsFolder(folder)) {
    cool::IFolderPtr fptr=db->getFolder(folder);
    m_desc=fptr->tagDescription(sourcetag);
    m_status=fptr->tagLockStatus(sourcetag);
  } else if (db->existsFolderSet(folder)) {
    cool::IFolderSetPtr fptr=db->getFolderSet(folder);
    m_desc=fptr->tagDescription(sourcetag);
    m_status=fptr->tagLockStatus(sourcetag);
  } else {
    m_desc="";
    m_status=cool::HvsTagLock::UNLOCKED;
    std::cout << "Could not access tag information for folder " << folder << 
      " tag " << sourcetag << std::endl;
  }
  // override description with input label if set
  if (!taglabel.empty()) m_desc=taglabel;
}

bool CoolTagInfo::write(const cool::IDatabasePtr& db,bool copylock) const {
  try {
    if (db->existsFolder(m_folder)) {
      cool::IFolderPtr fptr=db->getFolder(m_folder);
      if (!m_desc.empty()) fptr->setTagDescription(m_tag,m_desc);
      if (copylock) fptr->setTagLockStatus(m_tag,m_status);
    } else if (db->existsFolderSet(m_folder)) {
      cool::IFolderSetPtr fptr=db->getFolderSet(m_folder);
      if (!m_desc.empty()) fptr->setTagDescription(m_tag,m_desc);
      if (copylock) fptr->setTagLockStatus(m_tag,m_status);
    } else {
      std::cout << "Cannot access tag " << m_tag << " in folder " << m_folder 
		<< " to write tag info" << std::endl;
      return false;
    }
  }
  catch (cool::Exception& e) {
    std::cout << "Exception writing tag info for tag " << m_tag << 
      " in folder " << m_folder << std::endl;
    return false;
  }
  return true;
}
