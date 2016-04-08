// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerInterfaces/IMetadataTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Write metadata to a D3PD.
 */


#ifndef D3PDMAKERINTERFACES_IMETADATATOOL_H
#define D3PDMAKERINTERFACES_IMETADATATOOL_H


#include "GaudiKernel/IAlgTool.h"
#include <typeinfo>


namespace D3PD {


class ID3PD;


/**
 * @brief Write metadata to a D3PD.
 */
class IMetadataTool
  : virtual public IAlgTool
{
public:
  /// Gaudi interface definition.
  DeclareInterfaceID(IMetadataTool, 1, 0);


  /**
   * @brief Write metadata to a D3PD.
   * @param d3pd The D3PD to which to write.
   */
  virtual StatusCode writeMetadata (ID3PD* d3pd) = 0;
};


} // namespace D3PD


#endif // not D3PDMAKERINTERFACES_IMETADATATOOL_H
