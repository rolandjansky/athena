// -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGCOMPS_FOLDER_H
#define SGCOMPS_FOLDER_H 1
  /** @file SGFolder.h
   * @brief a run-time configurable list of data objects
   *
   * @author pcalafiura@lbl.gov - ATLAS Collaboration
   * $Id: SGFolder.h,v 1.8 2009-04-16 00:33:07 calaf Exp $
   **/

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "SGTools/SGIFolder.h"

#include "AthenaKernel/IClassIDSvc.h"

namespace SG {
  class Folder_tester;

  /** @class Folder
   * @brief a run-time configurable list of data objects
   * @details the ItemList property, used to initialize the folder, is a
   * list of class name (or clid)/key pairs.
   * One can use '*' as key value to add all objects of a given type 
   * to the Folder.
   * Examples:
   @code
    ItemList = [ "DataHeader#*", "AthenaAttributeList#SimpleTag", "8101/Foo" ]
   @endcode
   *
   * Normally, an object will be written as the type as which it was recorded
   * in StoreGate, regardless of the type used to request it in the ItemList.
   * But if the type name ends with a !, then the object will be written
   * as the exact type which was requested in the ItemList.
   **/
  class Folder : public virtual IFolder, public virtual AthAlgTool
  {
  public:
    /// \name structors
    //@{
    Folder(const std::string& type,
	   const std::string& name,
	   const IInterface* parent);
    virtual ~Folder() override;
    //@}

    /// the list we manage
    typedef IFolder::ItemList ItemList;

    /// \name access the ItemList
    //@{
    typedef IFolder::const_iterator const_iterator;
    virtual const_iterator begin() const override { return m_list.begin(); }
    virtual const_iterator end() const override { return m_list.end(); }
    //@}

    ///add a data object identifier to the list. Notice that if the typename
    ///is not yet in the ClassIDSvc registry the entry will be ignored and
    ///and add will return StatusCode::FAILURE
    virtual StatusCode add(const std::string& typeName, const std::string& skey) override;
    ///add a data object identifier to the list. The clid is not checked
    ///against the ClassIDSvc registry
    virtual StatusCode add(const CLID& clid, const std::string& skey) override {
      const bool DONTCHECKVALIDCLID(false);
      return add(clid, skey, DONTCHECKVALIDCLID, false);
    }

    ///clear the folder contents
    virtual void clear() override { m_list.clear(); }


    ///update contents of the ItemList
    virtual void updateItemList(bool checkValidCLID) override;

    /// \name AlgTool boilerplate 
    //@{
    virtual StatusCode initialize() override;
    /// Query for a given interface
    virtual StatusCode queryInterface(const InterfaceID& , void** ) override;
    //@}

  private:

    ServiceHandle<IClassIDSvc> m_pCLIDSvc;
    /// property: the list of items (data objects identified by a class name/key pair)
    StringArrayProperty m_itemList;
    void decodeItemList(Gaudi::Details::PropertyBase&) { 
      const bool DONTCHECKVALIDCLID(false);
      updateItemList(DONTCHECKVALIDCLID); 
    }
    void decodeItem(const std::string& item, bool checkValidCLID);
    friend class Folder_tester;
    ///add a data object identifier to the list, optionally checking if
    ///the clid is known to ClassIDSvc.
    StatusCode add(const CLID& clid, const std::string& skey, 
		   bool checkValidCLID, bool exact);

    ItemList m_list; 
    /// property: check if item types are known to ClassIDSvc
    BooleanProperty m_checkItems;

  };
} //ns SG

#endif
