/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SGFolderItem.h
 *  defines a class representing a folder item (data object clid/key)
 */
#ifndef SGTOOLS_FOLDERITEM_H
#define SGTOOLS_FOLDERITEM_H
#include <string>
#include "GaudiKernel/ClassID.h"
namespace SG {
  /** @class SG::FolderItem
   * @brief  a Folder item (data object) is identified by the clid/key pair
   *
   * If the @c exact flag is set, then the request is to write the object
   * as exactly that class, not as any derived class.
   *
   * @author pcalafiura@lbl.gov - ATLAS Collaboration
   * $Id: SGFolderItem.h,v 1.2 2008-03-26 22:36:54 calaf Exp $
   **/
  class FolderItem {
  public:
    FolderItem(CLID id=CLID_NULL,
               const std::string& key="",
               bool exact = false)
      : m_id(id), m_key(key), m_exact(exact) {}
    CLID id() const { return m_id; }
    const std::string& key() const { return m_key; }
    bool operator < (const FolderItem& rhs) const {
      return ( id() < rhs.id() || 
	       ( (id() == rhs.id()) && key() < rhs.key()) );
    }
    bool isFolder() const;
    bool exact() const { return m_exact; }
  private:
    CLID m_id;
    std::string m_key;
    bool m_exact;
  };
} //ns SG

#endif
