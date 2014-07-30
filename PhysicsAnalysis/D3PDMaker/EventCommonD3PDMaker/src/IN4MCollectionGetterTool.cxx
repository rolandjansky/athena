/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/IN4MCollectionGetterTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Getter tool to retrieve an @c INavigable4MomentumCollection from SG.
 */


#include "IN4MCollectionGetterTool.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
IN4MCollectionGetterTool::IN4MCollectionGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : D3PD::SGCollectionGetterTool<INavigable4MomentumCollection>
         (type, name, parent)
{
}


} // namespace D3PD
