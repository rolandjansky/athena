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

/**
 * The ATLASFieldCacheTLSWrapper
 * is needed due to the current threading model in simulation.
 *
 * In general clients need a MagField::AtlasFieldCache
 * returning the magnetic field values
 * for a specific position.
 *
 * The MagField::AtlasFieldCache refer to a specific "cell" and might
 * need to be filled or updated when the position changes.
 * In the other hand subsequent calls more often
 * than not re-use the info from the same "cell".
 * One can look at MagFieldElements for more details.
 *
 * Ideally objects would be passed by/be local to the caller.
 * Actually this is what happens in offline/online
 * where we moved completely to Condition Objects.
 * i.e AtlasFieldCacheCondObj.
 *
 * But this is not yet possible in simulation.
 * So we need to wrap things in a way that mimics
 * the relevant TLS behaviour of the to be removed ATLAS Svc.
 *
 * The end result is that for now the ATLASFieldCacheTLSWrapper 
 * is to be used instead of AtlasFieldCacheCondObj in simulation.
 *
 * It has similar payload.
 * The major difference is that when we use the
 * condition Object each user of the magnetic field
 * ends up with its own local MagField::AtlasFieldCache so there is
 * no need for synchronisation.
 *
 * In the other hand here we provide thread-specific
 * MagField::AtlasFieldCache (s) to allow for the TLS
 * scheme needed by the simulation.
 *
 */
struct ATLASFieldCacheTLSWrapper
{
private:
  MagField::AtlasFieldCache createCache() const
  {
    return MagField::AtlasFieldCache(solFieldScale, torFieldScale, fieldMap);
  }

public:
  ///Sclae for solenoid
  double solFieldScale{ 1 };
  ///Scale for toroid 
  double torFieldScale{ 1 };
  ///Not owning ptr 
  const MagField::AtlasFieldMap* fieldMap{ nullptr };

  /// Method setting up the TLS 
  /// and returning ref to 
  /// the TLS MagField::AtlasFieldCache
  MagField::AtlasFieldCache& getTLSCache() const
  {
    static thread_local MagField::AtlasFieldCache fieldCache = createCache();
    return fieldCache;
  }

  /// getField method, forwarding to the TLS object
  void getField(const double* point, double* field) const
  {
    MagField::AtlasFieldCache& fieldCache = getTLSCache();
    fieldCache.getField(point, field);
  }
};

/// @class AtlasField
/// @brief G4 wrapper around the main ATLAS magnetic field cache or field svc for forward field.
class AtlasField : public G4MagneticField
{
  public:
    /// Construct the field object from conditions object
    AtlasField(const MagField::AtlasFieldMap* fieldMap) {
            m_fieldCache.fieldMap = fieldMap;
    }
    /// Construct the field object from the IMagFieldSvc
    AtlasField(MagField::IMagFieldSvc* m) :
        m_magFieldSvc(m)
        {}
            

    MagField::AtlasFieldCache& fieldCache() { return m_fieldCache.getTLSCache(); }

    /// Implementation of G4 method to retrieve field value
    void GetFieldValue(const double *point, double *field) const
    {
        if (m_magFieldSvc) m_magFieldSvc->getField(point, field);
        else               m_fieldCache.getField(point, field);
    }

  private:
    /// Field cache TLS Wrapper
    ATLASFieldCacheTLSWrapper   m_fieldCache{};

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

    // field map held locally in svc - in Athena, this would go into a conditions object
    std::unique_ptr<MagField::AtlasFieldMap> m_fieldMap;
};

#endif // G4ATLASSERVICES_StandardFieldSvc_H
