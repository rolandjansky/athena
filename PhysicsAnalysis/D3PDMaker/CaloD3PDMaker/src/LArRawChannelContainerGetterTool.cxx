/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawChannelContainerGetterTool.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
LArRawChannelContainerGetterTool::LArRawChannelContainerGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : D3PD::SGCollectionGetterTool<LArRawChannelContainer>
         (type, name, parent)
{
}


} // namespace D3PD
