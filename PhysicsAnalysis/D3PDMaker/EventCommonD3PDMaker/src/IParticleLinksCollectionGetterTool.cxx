/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/IParticleLinksCollectionGetterTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2010
 * @brief Getter for IParticleLinks.
 */


#include "IParticleLinksCollectionGetterTool.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
IParticleLinksCollectionGetterTool::IParticleLinksCollectionGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
  m_typename = "IParticleLinks";
}


} // namespace D3PD
