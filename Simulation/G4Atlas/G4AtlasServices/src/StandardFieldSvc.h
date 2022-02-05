/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_StandardFieldSvc_H
#define G4ATLASSERVICES_StandardFieldSvc_H

// STL library
#include <string>

// Geant4
#include "G4MagneticField.hh"

// Gaudi/Athena
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

// Base classes
#include "G4MagFieldSvcBase.h"

// MagField cache
#include "MagFieldElements/AtlasFieldCache.h"

// forward declarations
namespace MagField {
    class AtlasFieldMap;
}

/// @class AtlasField
/// @brief G4 wrapper around the main ATLAS magnetic field cache or field svc for forward field.
class AtlasField : public G4MagneticField
{
  public:
    /// Construct the field object from conditions object
  AtlasField(double scaleSolenoid, double scaleToroid, const MagField::AtlasFieldMap* fieldMap) {
            m_fieldCache = MagField::AtlasFieldCache( scaleSolenoid, scaleToroid, fieldMap);
    }
    /// Construct the field object from the IMagFieldSvc
    AtlasField(MagField::IMagFieldSvc* m) :
        m_magFieldSvc(m)
        {}
            

    MagField::AtlasFieldCache& fieldCache() { return m_fieldCache; }

    /// Implementation of G4 method to retrieve field value
    void GetFieldValue(const double *point, double *field) const
    {
        if (m_magFieldSvc) m_magFieldSvc->getField(point, field);
        else               m_fieldCache.getField(point, field);
    }

  private:
    /// Field cache - mutable because getField modifies the cache
    mutable MagField::AtlasFieldCache m_fieldCache ATLAS_THREAD_SAFE;

    /// Pointer to the magnetic field service.
    /// We use a raw pointer here to avoid ServiceHandle overhead.
    MagField::IMagFieldSvc* m_magFieldSvc{nullptr};
};


/// @class StandardFieldSvc
/// @brief Athena service for constructing the AtlasField object
///
class StandardFieldSvc final : public G4MagFieldSvcBase
{
  public:

    /// Standard constructor
    StandardFieldSvc(const std::string& name, ISvcLocator* pSvcLocator);
    /// Empty destructor
    ~StandardFieldSvc() = default;

    /// Athena method. called at initialization time, being customized here.
    StatusCode initialize() override final;

  protected:
    /// Create/retrieve the AtlasField object
    G4MagneticField* makeField() override final;

  private:

    // There are two options for the magnetic field:
    //
    // 1) For solenoid and toroid, this has moved to use AtlasFieldCache and here we must create and
    //    same the field map
    // 2) For the forward quadrupole fields, we preserve the access to the magnetic field service
    //
    // The boolean flag UseMagFieldSvc is now used to differentiate between the two cases (default is false)
    //

    // flag to use magnet field service
    Gaudi::Property<bool> m_useMagFieldSvc {this, 
            "UseMagFieldSvc", false, "Use magnetic field service - Should ONLY be used for ForwardRegionFieldSvc"};
    
    /// Handle to the the Forward ATLAS magnetic field service
    ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc {this, "MagneticFieldSvc", ""};

    StatusCode createFieldMap();

    // properties taken from AtlasFieldMapCondAlg
    bool solenoidOn() { return (m_mapSoleCurrent > 0.0); }
    bool toroidOn()   { return (m_mapToroCurrent > 0.0); }

    /// map file names
    Gaudi::Property<std::string> m_fullMapFilename {this,
            "FullMapFile", "MagneticFieldMaps/bfieldmap_7730_20400_14m.root",
            "File storing the full magnetic field map"};
    Gaudi::Property<std::string> m_soleMapFilename {this,
                                                    "SoleMapFile", "MagneticFieldMaps/bfieldmap_7730_0_14m.root",
                                                    "File storing the solenoid-only magnetic field map"};
    Gaudi::Property<std::string> m_toroMapFilename {this,
                                                    "ToroMapFile", "MagneticFieldMaps/bfieldmap_0_20400_14m.root",
                                                    "File storing the toroid-only magnetic field map"};
    /// nominal current for the maps
    Gaudi::Property<double>      m_mapSoleCurrent  {this,
                                                    "MapSoleCurrent", 7730., "Nominal solenoid current (A)"};
    Gaudi::Property<double>      m_mapToroCurrent  {this,
                                                    "MapToroCurrent", 20400., "Nominal toroid current (A)"};
    ///  configurable current for map scaling
    Gaudi::Property<double>      m_useSoleCurrent  {this,
                                                    "UseSoleCurrent", 7730., "Configured solenoid current (A)"};
    Gaudi::Property<double>      m_useToroCurrent  {this,
                                                    "UseToroCurrent", 20400., "Configured toroid current (A)"};

    // field map held locally in svc - in Athena, this would go into a conditions object
    std::unique_ptr<MagField::AtlasFieldMap> m_fieldMap;
};

#endif // G4ATLASSERVICES_StandardFieldSvc_H
