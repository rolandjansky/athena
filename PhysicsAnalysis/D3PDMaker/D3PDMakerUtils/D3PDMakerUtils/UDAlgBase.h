// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/UDAlgBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Helper base class for algorithms filling UserData.
 */


#ifndef D3PDMAKERUTILS_UDALGBASE_H
#define D3PDMAKERUTILS_UDALGBASE_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IUserDataSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include <string>


namespace D3PD {


/**
 * @brief Helper base class for algorithms filling UserData.
 */
class UDAlgBase
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  UDAlgBase (const std::string& name,
             ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Add a UD element decoration.
   * @param iabc The element to which to add the decoration.
   * @param label The UD label.  The prefix will be added.
   * @param val The value to add.
   */
  template <class T>
  StatusCode deco (const IAthenaBarCode& iabc,
                   const std::string& label,
                   T& val) const;


private:
  /// Property: UserDataSvc.
  ServiceHandle<IUserDataSvc> m_userDataSvc;

  /// Property: Prefix to add to UD labels.
  std::string m_udprefix;
};


} // namespace D3PD


#include "D3PDMakerUtils/UDAlgBase.icc"


#endif // D3PDMAKERUTILS_UDALGBASE_H
