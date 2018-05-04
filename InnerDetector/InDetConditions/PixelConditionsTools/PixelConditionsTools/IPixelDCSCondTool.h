/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPIXELDCSCONDTOOL_H
#define IPIXELDCSCONDTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

class IPixelDCSCondTool: virtual public IAlgTool {

  public:

    virtual ~IPixelDCSCondTool() = default;

    DeclareInterfaceID(IPixelDCSCondTool, 1, 0);

    virtual bool canReportAbout(InDetConditions::Hierarchy h) const =0;
    virtual float getHV(const Identifier& elementId) const =0;
    virtual float getHV(const IdentifierHash& hashId) const =0;
    virtual float getTemperature(const Identifier& elementId) const =0;
    virtual float getTemperature(const IdentifierHash& hashId) const =0;

    virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const =0;
    virtual bool isGood(const IdentifierHash& hashId) const =0;

/*
    virtual double getTemperature(const Identifier& module_id) const = 0;
    virtual double getHV(const Identifier& module_id) const = 0;
    virtual std::string getFSMStatus(const Identifier& module_id) const = 0;
    virtual std::string getFSMState(const Identifier& module_id) const = 0;

    virtual double getTemperature(const IdentifierHash& id_hash) const = 0;
    virtual double getHV(const IdentifierHash& id_hash) const = 0;
    virtual std::string getFSMStatus(const IdentifierHash& id_hash) const = 0;
    virtual std::string getFSMState(const IdentifierHash& id_hash) const = 0;

    virtual StatusCode printData() const = 0;
*/

}; 

#endif // IPIXELDCSCONDTOOL_H
