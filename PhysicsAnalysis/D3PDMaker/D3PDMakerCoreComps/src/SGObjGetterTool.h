// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SGObjGetterTool.h 486128 2012-03-02 08:54:20Z krasznaa $
/**
 * @file D3PDMakerCoreComps/src/SGObjGetterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Getter tool to retrieve single objects from StoreGate.
 */


#ifndef D3PDMAKERCORECOMPS_SGOBJGETTERTOOL_H
#define D3PDMAKERCORECOMPS_SGOBJGETTERTOOL_H


#include "D3PDMakerUtils/ObjGetterToolImpl.h"
#include "D3PDMakerUtils/SGGetterImpl.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include <string>


namespace D3PD {


/**
 * @brief Getter tool to retrieve single objects from StoreGate.
 *
 * This is a Getter tool that retrieves a single object from StoreGate,
 * given the type name and SG key.  This class should be able to handle
 * any type that can be put into StoreGate.
 *
 * Properties:
 *   TypeName   - Name of the type of the object being retrieved.
 *   SGKey      - StoreGate key of the object being retrieved.
 *                Can also be a comma or space-separated list.
 *                In that case, the first key that actually exists
 *                in the data store will be used.
 *   ClassIDSvc - ClassIDSvc instance to use.
 */
class SGObjGetterTool
  : public SGGetterImpl, public ObjGetterToolImpl
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  SGObjGetterTool (const std::string& type,
                   const std::string& name,
                   const IInterface* parent);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();
};


} // namespace D3PD


#endif // not D3PDMAKERCORECOMPS_SGOBJGETTERTOOL_H
