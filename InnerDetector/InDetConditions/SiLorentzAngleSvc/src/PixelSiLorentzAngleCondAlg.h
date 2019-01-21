/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELSILORENTZANGLECONDALG
#define PIXELSILORENTZANGLECONDALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "PixelConditionsData/PixelDCSConditionsData.h"
#include "PixelConditionsTools/IPixelDCSConditionsTool.h"
#include "GaudiKernel/ICondSvc.h"
#include "SiPropertiesSvc/ISiPropertiesTool.h"

#include "SiLorentzAngleSvc/SiLorentzAngleCondData.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

// forward declarations
namespace InDetDD {
  class PixelDetectorManager;
}  

namespace MagField {
  class IMagFieldSvc;
}

class PixelSiLorentzAngleCondAlg: public AthAlgorithm {
  public:
    PixelSiLorentzAngleCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelSiLorentzAngleCondAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:
    const PixelID* m_pixid;
    const InDetDD::PixelDetectorManager* m_detManager;

    ServiceHandle<ICondSvc> m_condSvc;
    ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;

    SG::ReadCondHandleKey<PixelDCSConditionsData> m_readKeyTemp{this, "ReadKeyeTemp", "PixelDCSTempCondData",         "Key of input sensor temperature conditions folder"};
    SG::ReadCondHandleKey<PixelDCSConditionsData> m_readKeyHV  {this, "ReadKeyHV",    "PixelDCSHVCondData",           "Key of input bias voltage conditions folder"};

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyBFieldSensor{this, "ReadKeyBFieldSensor", "/EXT/DCS/MAGNETS/SENSORDATA", "Key of input B-field sensor"};
    SG::WriteCondHandleKey<SiLorentzAngleCondData> m_writeKey{this, "WriteKey", "PixelSiLorentzAngleCondData", "Key of output SiLorentzAngleCondData"};

    ToolHandle<IPixelDCSConditionsTool> m_DCSConditionsTool{this, "PixelDCSConditionsTool", "PixelDCSConditionsTool", "Tool to retrieve Pixel information"};

    ToolHandle<ISiPropertiesTool>   m_siPropertiesTool{this, "SiPropertiesTool", "SiPropertiesTool", "Tool to retrieve SiProperties"};

    // Properties
    double                   m_nominalField;
    bool                     m_useMagFieldSvc;
    bool                     m_useMagFieldDcs;
    double                   m_correctionFactor;

    Amg::Vector3D getMagneticField(const IdentifierHash& elementHash) const;
};

#endif // PIXELSILORENTZANGLECONDALG
