/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOD3PDMAKER_LARRAWCHANNELCONTAINERGETTERTOOL_H
#define CALOD3PDMAKER_LARRAWCHANNELCONTAINERGETTERTOOL_H


#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "LArRawEvent/LArRawChannelContainer.h"


namespace D3PD {


class LArRawChannelContainerGetterTool
  : public SGCollectionGetterTool<LArRawChannelContainer>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  LArRawChannelContainerGetterTool (const std::string& type,
                            const std::string& name,
                            const IInterface* parent);
};


} // namespace D3PD


#endif

