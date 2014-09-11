// dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGCOMPS_FOLDERRESETALG_H
#define SGCOMPS_FOLDERRESETALG_H 1
  /** @file SGDeleteAlg.h
   * @brief an algorithm that resets the data objects listed in a SG::Folder
   *
   * @author pcalafiura@lbl.gov - ATLAS Collaboration
   * $Id: SGDeleteAlg.h,v 1.1 2009-04-16 00:33:07 calaf Exp $
   **/

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class StoreGateSvc;
namespace SG {
  class IFolder;
}

/** @class SGDeleteAlg
 * @brief an algorithm that resets the data objects listed in a SG::Folder
 * @details the ToDelete property is a SG::Folder with the list
 * of class name (or clid)/key  pairs to delete from TargetStore.
 *  As usual one can use '*' as key value to delete all objects of a given type 
 * Example:
 @code
   mySGDeleteAlg.ToDelete.ItemList = [ "MyType#MyKey", "8101#Foo" ]
 @endcode
 * Notice that wildcarding on the key name is not yet supported
**/
class SGDeleteAlg : public AthAlgorithm
{
public:
  SGDeleteAlg(const std::string& name, ISvcLocator*);
  ~SGDeleteAlg();
  StatusCode initialize(); 
  StatusCode execute(); ///< reset the ItemsToDelete once per event
private:
  /// property: the store containing the object in ItemsToDelete
  ServiceHandle<StoreGateSvc> m_targetStore;
  /// property: this is a private Folder called ToDelete. It contains
  /// the list of items (data objects identified by a class name#key pair)
  /// to delete
  ToolHandle<SG::IFolder> m_toDelete;
};
#endif
