/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGTOOLS_IFOLDER_H
#define SGTOOLS_IFOLDER_H

#include <set>
#include <string>

#ifndef GAUDIKERNEL_IALGTOOL_H
 #include "GaudiKernel/IAlgTool.h"
#endif

#include "SGTools/SGFolderItem.h"

namespace SG {  
  /** @class SG::IFolder
   * @brief a run-time configurable list of data objects
   *
   * @author pcalafiura@lbl.gov - ATLAS Collaboration
   * $Id: SGIFolder.h,v 1.3 2008-04-24 00:55:21 calaf Exp $
   **/
  class IFolder : public virtual IAlgTool
  {
  public:
    /// the list we manage
    typedef std::set<FolderItem> ItemList; //FIXME would be nice to move to SG::Folder

    /// \name access the ItemList
    //@{
    typedef ItemList::const_iterator const_iterator;
    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
    //@}
    ///add a data object identifier to the list
    virtual StatusCode add(const std::string& typeName, const std::string& skey) = 0;
    ///add a data object identifier to the list
    virtual StatusCode add(const CLID& clid, const std::string& skey) = 0;

    ///clear the folder contents
    virtual void clear() = 0;

    ///update list of items
    virtual void updateItemList(bool checkValidCLID) = 0;
    

    static const InterfaceID& interfaceID() {
      static const InterfaceID IID( "SG::IFolder", 1, 0 );
      return IID;
    }
  };
} //ns SG

#endif
