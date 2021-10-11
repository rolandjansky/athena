/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// CoolTagInfo.h - helper class for AtlCoolCopy, store info about a tag
// Richard Hawkings, 29/2/08

#include<string>
#include "CoolKernel/HvsTagLock.h"
#include "CoolKernel/IDatabase.h"

namespace cool {
  class IHvsNode;
}

class CoolTagInfo {
 public:
  // generate from a given COOL folder and tag
  CoolTagInfo(const cool::IDatabasePtr& db, const std::string& folder,
	      const std::string& destfolder,
	      const std::string& sourcetag,const std::string& desttag,
	      const std::string& taglabel="");
  // store into a given COOL folder in a destination DB
  bool write(const cool::IDatabasePtr& db,const bool copylock) const;

 private:
  std::string m_folder;
  std::string m_tag;
  std::string m_desc;
  cool::HvsTagLock::Status m_status;
};
  
