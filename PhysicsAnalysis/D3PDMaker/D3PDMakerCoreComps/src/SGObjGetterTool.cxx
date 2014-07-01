/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SGObjGetterTool.cxx 486128 2012-03-02 08:54:20Z krasznaa $
/**
 * @file D3PDMakerCoreComps/src/SGObjGetterTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Getter tool to retrieve single objects from StoreGate.
 */


#include "SGObjGetterTool.h"
#include "SGTools/DataProxy.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
SGObjGetterTool::SGObjGetterTool (const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent)
  : SGGetterImpl (name, evtStore()),
    ObjGetterToolImpl (type, name, parent)
{
  SGGETTERIMPL_PROPS;
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode SGObjGetterTool::initialize()
{
  CHECK( SGGetterImpl::initializeImpl() );
  return AthAlgTool::initialize();
}


} // namespace D3PD
