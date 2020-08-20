/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsAlgorithms/PixelCablingCondAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date December, 2019
 * @brief Store pixel cabling map in PixelCablingCondData.
 */

#ifndef PIXELCABLINGCONDALG_H
#define PIXELCABLINGCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelModuleData.h"
#include "PixelConditionsData/PixelReadoutSpeedData.h"
#include "PixelConditionsData/PixelCablingCondData.h"

#include "InDetIdentifier/PixelID.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

#include <map>
#include <stdint.h>
#include <string>
#include <istream>

class PixelCablingCondAlg : public AthReentrantAlgorithm {  
  public:
    PixelCablingCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelCablingCondAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    const PixelID* m_pixelID{nullptr};
    ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

    Gaudi::Property<bool> m_recordInInitialize
    {this, "RecordInInitialize", true, "(Temporary) write the cond object in initialize"};

    // Keep this untile SegionSelectorTable is fixed.
    Gaudi::Property<uint32_t> m_rodidForSingleLink40
    {this, "RodIDForSingleLink40", 0, "(Temporary) const link speed"};

    Gaudi::Property<std::string> m_final_mapping_file
    {this, "MappingFile", "PixelCabling/Pixels_Atlas_IdMapping_2016.dat", "Read cabling map from file"};

    SG::ReadCondHandleKey<PixelModuleData> m_moduleDataKey
    {this, "PixelModuleData", "PixelModuleData", "Pixel module data"};

    SG::ReadCondHandleKey<PixelReadoutSpeedData> m_readoutspeedKey
    {this, "PixelReadoutSpeedData", "PixelReadoutSpeedData", "Pixel readout speed data"};

    SG::ReadCondHandleKey<AthenaAttributeList> m_readKey
    {this, "ReadKey", "/PIXEL/CablingMap", "Input cabling folder"};

    SG::WriteCondHandleKey<PixelCablingCondData> m_writeKey
    {this, "WriteKey", "PixelCablingCondData", "Output cabling data"};
};

#endif
