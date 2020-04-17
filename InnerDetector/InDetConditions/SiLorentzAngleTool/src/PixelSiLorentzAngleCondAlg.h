/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file SiLorentzAngleTool/PixelSiLorentzAngleCondAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Return Lorentz angle information for pixel.
 */

#ifndef PIXELSILORENTZANGLECONDALG
#define PIXELSILORENTZANGLECONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "PixelConditionsData/PixelDCSHVData.h"
#include "PixelConditionsData/PixelDCSTempData.h"
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "SiPropertiesTool/ISiPropertiesTool.h"

#include "SiLorentzAngleTool/SiLorentzAngleCondData.h"
#include "InDetIdentifier/PixelID.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// forward declarations
namespace MagField {
  class IMagFieldSvc;
}

class PixelSiLorentzAngleCondAlg: public AthReentrantAlgorithm {
  public:
    PixelSiLorentzAngleCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelSiLorentzAngleCondAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

  private:
    ServiceHandle<ICondSvc> m_condSvc;
    ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;

    SG::ReadCondHandleKey<PixelDCSTempData> m_readKeyTemp
    {this, "ReadKeyeTemp", "PixelDCSTempCondData", "Key of input sensor temperature conditions folder"};

    SG::ReadCondHandleKey<PixelDCSHVData> m_readKeyHV
    {this, "ReadKeyHV", "PixelDCSHVCondData", "Key of input bias voltage conditions folder"};

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyBFieldSensor
    {this, "ReadKeyBFieldSensor", "/EXT/DCS/MAGNETS/SENSORDATA", "Key of input B-field sensor"};

    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey
    {this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};

    SG::WriteCondHandleKey<SiLorentzAngleCondData> m_writeKey
    {this, "WriteKey", "PixelSiLorentzAngleCondData", "Key of output SiLorentzAngleCondData"};

    ToolHandle<ISiPropertiesTool> m_siPropertiesTool
    {this, "SiPropertiesTool", "SiPropertiesTool", "Tool to retrieve SiProperties"};

    // Properties
    DoubleProperty           m_nominalField  {this, "NominalField", 2.0834*Gaudi::Units::tesla, "Default nominal field"};
    BooleanProperty          m_useMagFieldSvc{this, "UseMagFieldSvc", true};
    BooleanProperty          m_useMagFieldDcs{this, "UseMagFieldDcs", true};
    DoubleProperty           m_correctionFactor{this, "CorrectionFactor", 0.9, "Lorentz angle correction factor"};
    unsigned int             m_maxHash;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    Amg::Vector3D getMagneticField(const InDetDD::SiDetectorElement* element) const;
    Amg::Vector3D getMagneticField(MagField::AtlasFieldCache& m_fieldCache,
                                   const InDetDD::SiDetectorElement* element) const;
    // Read handle for conditions object to get the field cache
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj",
                                                                       "Name of the Magnetic Field conditions object key"};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

#endif
