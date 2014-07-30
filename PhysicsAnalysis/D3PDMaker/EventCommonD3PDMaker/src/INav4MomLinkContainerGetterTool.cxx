/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/INav4MomLinkContainerGetterTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2012
 * @brief Getter tool to retrieve an @c INav4MomLinkContainer from SG.
 */


#include "INav4MomLinkContainerGetterTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
INav4MomLinkContainerGetterTool::INav4MomLinkContainerGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : D3PD::SGCollectionGetterTool<INav4MomLinkContainer>
         (type, name, parent)
{
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode INav4MomLinkContainerGetterTool::initialize()
{
  // Force the TypeName --- doesn't make sense to set it to anything else.
  CHECK (this->setProperty ("TypeName", "INav4MomLinkContainer"));
  CHECK (SGCollectionGetterTool<INav4MomLinkContainer>::initialize());
  return StatusCode::SUCCESS;
}


} // namespace D3PD
