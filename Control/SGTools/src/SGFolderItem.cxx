/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SGTools/SGFolderItem.h"

bool
SG::FolderItem::isFolder() const {
  return false;
  //FIXME  return (m_id == ClassID_traits<Folder>::ID());
}

