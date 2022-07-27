// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/IInputRename.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2016
 * @brief Interface to retrieve input rename map.
 */


#ifndef ATHENAKERNEL_IINPUTRENAME_H
#define ATHENAKERNEL_IINPUTRENAME_H


#include "AthenaKernel/RCUObject.h"
#include "CxxUtils/sgkey_t.h"
#include "GaudiKernel/IInterface.h"
#include <unordered_map>


namespace Athena {


/**
 * @brief Interface to retrieve input rename map.
 *
 * The address remapping service may be configured to rename objects on input.
 * When this is done, the keys stored in ElementLink/DataLink also need to
 * be adjusted.  This interface allows fetching a map that gives the
 * needed transformation of SG keys.  The returned object is
 * synchronized via RCU.
 */
class IInputRename
  : virtual public IInterface
{
public:
  /// Type of the input rename map: sgkey_t -> sgkey_t.
  struct Rename
  {
    SG::sgkey_t m_sgkey;
    std::string m_key;
  };
  typedef SG::SGKeyMap<Rename> InputRenameMap_t;
  typedef RCUObject<InputRenameMap_t> InputRenameRCU_t;

  DeclareInterfaceID (IInputRename,1,0);


  /**
   * @brief Retrieve a pointer to the input rename map.
   *
   * May return null if no renaming is to be done.
   */
  virtual const InputRenameRCU_t* inputRenameMap() const = 0;
};


} // namespace Athena


#endif // not ATHENAKERNEL_IINPUTRENAME_H
