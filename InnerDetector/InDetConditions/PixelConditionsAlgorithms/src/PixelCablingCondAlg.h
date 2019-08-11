/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCABLINGCONDALG_H
#define PIXELCABLINGCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelReadoutSpeedData.h"
#include "PixelConditionsData/PixelCablingCondData.h"

#include "InDetIdentifier/PixelID.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

#include <map>
#include <stdint.h>
#include <string>
#include <istream>

class PixelCablingCondAlg : public AthAlgorithm {  
  public:
    PixelCablingCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelCablingCondAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:
    const PixelID* m_pixelID;
    ServiceHandle<ICondSvc> m_condSvc;
    bool m_useConditions;
    uint32_t m_rodidForSingleLink40;
    bool m_dump_map_to_file;
    std::string m_final_mapping_file;

    SG::ReadCondHandleKey<PixelReadoutSpeedData> m_readoutspeedKey
    {this, "PixelReadoutSpeedData", "PixelReadoutSpeedData", "Pixel readout speed data"};

    SG::ReadCondHandleKey<AthenaAttributeList> m_readKey
    {this, "ReadKey", "/PIXEL/CablingMap", "Input cabling folder"};

    SG::WriteCondHandleKey<PixelCablingCondData> m_writeKey
    {this, "WriteKey", "PixelCablingCondData", "Output cabling data"};

};

#endif
