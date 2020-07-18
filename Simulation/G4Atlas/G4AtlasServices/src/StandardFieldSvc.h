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

// Base classes
#include "G4MagFieldSvcBase.h"

// MagField cache
#include "MagFieldElements/AtlasFieldCache.h"

// forward declarations
namespace MagField {
    class AtlasFieldMap;
}

/*
 * This is needed due to the current threading model in simulation.
 *
 * We need a cache returning the field  for a specific position.
 * The cache might needs to be filled
 * or updated when the requested position changes.
 *
 * Ideally they would be passed by the caller.
 * Actually this is the
 * scheme we use in offline.
 * But this is not yet possible is simulations
 *
 * So we need to wrap the new machinery in a way that mimics
 * the TLS behavious of the to be
 * removed ATLAS svc
 */
#include <boost/thread/tss.hpp>
struct ATLASFieldCacheTLSWrapper
{
private:
  mutable boost::thread_specific_ptr<MagField::AtlasFieldCache>
    m_fieldCache_tls{};

public:
  double solFieldScale{ 1 };
  double torFieldScale{ 1 };
  const MagField::AtlasFieldMap* fieldMap{ nullptr };

  // Set up the thread specific cache
  MagField::AtlasFieldCache& getTLSCache() const
  {
    MagField::AtlasFieldCache* fieldCache = m_fieldCache_tls.get();
    // if we have none for this thread create one
    if (!fieldCache) {
      fieldCache =
        new MagField::AtlasFieldCache(solFieldScale, torFieldScale, fieldMap);
      m_fieldCache_tls.reset(fieldCache);
    }
    return *fieldCache;
  }

  // forward call to the thread specific cache
  void getField(const double* point, double* field) const
  {
    MagField::AtlasFieldCache& fieldCache = getTLSCache();
    fieldCache.getField(point, field);
  }
};

/// @class AtlasField
/// @brief G4 wrapper around the main ATLAS magnetic field cache.
class AtlasField : public G4MagneticField
{
  public:
    /// Construct the field object from conditions object
    AtlasField(const MagField::AtlasFieldMap* fieldMap){
        m_fieldCache.fieldMap = fieldMap;
    }

    MagField::AtlasFieldCache& fieldCache() { return m_fieldCache.getTLSCache(); }

    /// Implementation of G4 method to retrieve field value
    void GetFieldValue(const double *point, double *field) const
    {
      m_fieldCache.getField(point, field);
    }

  private:
    /// Field cache TLS Wrapper
    ATLASFieldCacheTLSWrapper   m_fieldCache{};
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
    ~StandardFieldSvc() {}

    /// Athena method. called at initialization time, being customized here.
    StatusCode initialize() override final;

  protected:
    /// Create/retrieve the AtlasField object
    G4MagneticField* makeField() override final;

  private:

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
