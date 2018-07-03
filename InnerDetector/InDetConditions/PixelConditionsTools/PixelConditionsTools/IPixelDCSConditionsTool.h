/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPIXELDCSCONDITIONSTOOL_H
#define IPIXELDCSCONDITIONSTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

class IPixelDCSConditionsTool: virtual public ISiliconConditionsTool {
  public:
    virtual ~IPixelDCSConditionsTool() = default;

    DeclareInterfaceID(IPixelDCSConditionsTool, 1, 0);

    virtual std::string PixelFSMState(const Identifier& elementId) const = 0;
    virtual std::string PixelFSMState(const IdentifierHash& elementHash) const = 0;

    virtual std::string PixelFSMStatus(const Identifier& elementId) const = 0;
    virtual std::string PixelFSMStatus(const IdentifierHash& elementHash) const = 0;


//    virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const =0;
//    virtual bool isGood(const IdentifierHash& hashId) const =0;

}; 

#endif // IPIXELDCSCONDITIONSTOOL_H
