/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDCSCONDTOOL_H
#define PIXELDCSCONDTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "PixelConditionsTools/IPixelDCSCondTool.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelConditionsData/PixelDCSCondData.h"
#include <list>
#include <string>
#include <map>
#include <mutex>

class PixelDCSCondTool: public extends<AthAlgTool, IPixelDCSCondTool> {

  public:
    PixelDCSCondTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~PixelDCSCondTool() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual bool canReportAbout(InDetConditions::Hierarchy h) const override;
    virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
    virtual bool isGood(const IdentifierHash& hashId) const override;

    virtual float getHV(const Identifier& elementId) const override;
    virtual float getHV(const IdentifierHash& hashId) const override;
    virtual float getTemperature(const Identifier& elementId) const override;
    virtual float getTemperature(const IdentifierHash& hashId) const override;

  private:
    BooleanProperty m_readAllDBFolders;
    BooleanProperty m_returnHVTemp;
    mutable std::mutex m_mutex;
    mutable std::vector<EventContext::ContextEvt_t> m_cacheState;
    mutable std::vector<EventContext::ContextEvt_t> m_cacheHV;
    mutable std::vector<EventContext::ContextEvt_t> m_cacheTemp0;
    mutable Gaudi::Hive::ContextSpecificPtr<const PixelDCSCondData> m_pBadModules;
    mutable Gaudi::Hive::ContextSpecificPtr<const PixelDCSCondData> m_pModulesHV;
    mutable Gaudi::Hive::ContextSpecificPtr<const PixelDCSCondData> m_pModulesTemp0;
    SG::ReadCondHandleKey<PixelDCSCondData> m_condKeyState;
    SG::ReadCondHandleKey<PixelDCSCondData> m_condKeyHV;
    SG::ReadCondHandleKey<PixelDCSCondData> m_condKeyTemp0;
    const PixelID* m_pHelper;
    static const float s_defaultHV;
    static const float s_defaultTemperature;
    const PixelDCSCondData* getCondDataState(const EventContext& ctx) const;
    const PixelDCSCondData* getCondDataHV(const EventContext& ctx) const;
    const PixelDCSCondData* getCondDataTemp0(const EventContext& ctx) const;
};

#endif // PIXELDCSCONDTOOL_H
