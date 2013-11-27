/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BuildH6HitCollections
// 10-Nov-2003 Bill Seligman

// Transform the internal LArG4 hit collections into the external form
// needed by AtlasG4Sim.

// 09-Feb-2004 WGS: Include the possibility of building calibration hits.

#ifndef LArG4_BuildH6HitCollections_H
#define LArG4_BuildH6HitCollections_H

#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include <vector>

// Forward declarations
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class CaloDM_ID;
class IMessageSvc;
class Identifier;
class LArG4Identifier;

namespace LArG4 {

  class BuildH6HitCollections
  {
  public:

    // Constructor and destructor
    BuildH6HitCollections();
    virtual ~BuildH6HitCollections();

    // The meat of this class: Build a vector of the different hit
    // collections associated with this event.
    typedef LArHitContainer hitCollection_t;     // define a type
    virtual void ChangeHitCollections(LArHitContainer* emecHitContainer, LArHitContainer* hecHitContainer ); // a method that uses the type

    // A separate routine for the (optional) calibration hits.
    typedef CaloCalibrationHitContainer calibCollection_t;
    virtual std::vector<calibCollection_t*> CreateCalibrationHitCollections();

    // Utility routine shared by the above methods: convert a
    // LArG4Identifier (used internally solely by LArG4) to a standard
    // Athena Identifier.
    virtual Identifier ConvertID(const LArG4Identifier&);

  protected:
    // These data members are protected, not private, so that a class
    // in the testbeam code can inherit from this class and use these
    // members.

    // We have to convert hit identifiers.  These private members save
    // the location of the identifier helper classes.
    const LArEM_ID*     m_larEmID;
    const LArFCAL_ID*   m_larFcalID;
    const LArHEC_ID*    m_larHecID;
    const CaloDM_ID*    m_caloDmID;

    // Pointer to Athena message service.
    IMessageSvc* m_msgSvc;

  };

} // namespace LArG4

#endif // LArG4_BuildH6HitCollections_H
