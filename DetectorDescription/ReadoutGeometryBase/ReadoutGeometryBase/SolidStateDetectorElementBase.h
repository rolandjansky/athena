/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SolidStateDetectorElementBase.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef READOUTGEOMETRYBASE_SOLIDSTATEDETECTORELEMENTBASE_H
#define READOUTGEOMETRYBASE_SOLIDSTATEDETECTORELEMENTBASE_H

// Base class.
#include "TrkDetElementBase/TrkDetElementBase.h"

#include "ReadoutGeometryBase/DetectorDesign.h"
#include "ReadoutGeometryBase/SiCommonItems.h"
#include "ReadoutGeometryBase/SiCellId.h"

namespace InDetDD {

class SolidStateDetectorElementBase : public Trk::TrkDetElementBase {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:

    // Constructor
    SolidStateDetectorElementBase(const Identifier &id,
                                  const DetectorDesign *design,
                                  const GeoVFullPhysVol *geophysvol,
                                  SiCommonItems *commonItems);

    // Destructor
    ~SolidStateDetectorElementBase();

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Identification
    /// Methods to identify the element and identifier manipulation.
    //
    ///////////////////////////////////////////////////////////////////

    //@{

    /// identifier of this detector element:
    virtual Identifier identify() const;

    /// identifier hash
    virtual IdentifierHash identifyHash() const;

    /// Returns the id helper
    const AtlasDetectorID* getIdHelper() const;

    // Identifier <-> SiCellId (ie strip number or pixel eta_index,phi_index)
    // pure virtual so that the correct ID helper can be used in the derived classes

    /// Identifier from SiCellId
    virtual Identifier identifierFromCellId(const SiCellId & cellId) const = 0;

    /// SiCellId from Identifier
    virtual SiCellId   cellIdFromIdentifier(const Identifier & identifier) const = 0;

    //@}

    ///////////////////////////////////////////////////////////////////
    // Protected data:
    ///////////////////////////////////////////////////////////////////
protected:
    Identifier m_id; // identifier of this detector element
    IdentifierHash m_idHash; // hash id
    const DetectorDesign *m_design; // local description of this detector element
    SiCommonItems * m_commonItems;

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline Identifier SolidStateDetectorElementBase::identify() const
{
  return m_id;
}

inline IdentifierHash SolidStateDetectorElementBase::identifyHash() const
{
  return m_idHash;
}

inline const AtlasDetectorID* SolidStateDetectorElementBase::getIdHelper() const
{
  return m_commonItems->getIdHelper();
}

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SIDETECTORELEMENT_H
