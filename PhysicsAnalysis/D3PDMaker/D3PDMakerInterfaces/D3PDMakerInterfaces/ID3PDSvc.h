// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerInterfaces/ID3PDSvc.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2009
 * @brief Abstract interface for service to create D3PD trees.
 */


#ifndef D3PDMAKERINTERFACES_ID3PDSVC_H
#define D3PDMAKERINTERFACES_ID3PDSVC_H

#include "GaudiKernel/IService.h"
#include <string>


namespace D3PD {


class ID3PD;


/**
 * @brief Abstract interface for service to create D3PD trees.
 */
class ID3PDSvc
  : virtual public IService
{
public:
  /// Gaudi interface definition.
  DeclareInterfaceID(ID3PDSvc, 1, 0);


  /**
   * @brief Create a new D3PD tree.
   * @param name The name of the new tree.
   * @param d3pd[out] The created tree.
   */
  virtual StatusCode make (const std::string& name, ID3PD* & d3pd) = 0;

private:
  ID3PDSvc& operator= (const ID3PDSvc&);
  ID3PDSvc& operator= (ID3PDSvc&&);
};


} // namespace D3PD


#endif // not D3PDMAKERINTERFACES_ID3PDSVC_H
