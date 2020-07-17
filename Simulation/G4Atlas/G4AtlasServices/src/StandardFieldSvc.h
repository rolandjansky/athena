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
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

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
    // if we have none for this thread lest create one
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
    AtlasField(const AtlasFieldCacheCondObj* condObj){
      m_fieldCache.solFieldScale = condObj->solenoidFieldScaleFactor();
      m_fieldCache.torFieldScale = condObj->toriodFieldScaleFactor();
      m_fieldCache.fieldMap= condObj->fieldMap();
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

  // Read handle for conditions object to get the field cache
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey{
      this,
      "AtlasFieldCacheCondObj",
      "fieldCondObj",
      "Name of the Magnetic Field conditions object key"
    };
};

#endif // G4ATLASSERVICES_StandardFieldSvc_H
