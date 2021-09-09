/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloDetDescr/CaloDescriptors.h"

#include "LArReadoutGeometry/EMBCell.h"
#include "LArReadoutGeometry/EMBDetectorRegion.h"
#include "LArReadoutGeometry/EMECCell.h"
#include "LArReadoutGeometry/EMECDetectorRegion.h"
#include "LArReadoutGeometry/HECCell.h"
#include "LArReadoutGeometry/HECDetectorRegion.h"
#include "LArReadoutGeometry/FCALTile.h"
#include "LArReadoutGeometry/FCALModule.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "GaudiKernel/SystemOfUnits.h"

// -- EMBDescriptor --
EMBDescriptor::EMBDescriptor(const Identifier& id
			     , const AtlasDetectorID* helper
			     , const CaloCell_ID* calo_helper
			     , const EMBDetectorRegion* embRegion
			     , const GeoAlignmentStore* alignStore)
  : CaloDetDescriptor(id,helper,calo_helper)
{
  const Amg::Transform3D &xfDef = embRegion->getDefAbsoluteTransform(alignStore);
  const Amg::Transform3D &xfAbs = embRegion->getAbsoluteTransform(alignStore);

  m_transform = xfAbs*xfDef.inverse();
}

EMBDescriptor::~EMBDescriptor()
{
}

// -- EMECDescriptor --
EMECDescriptor::EMECDescriptor(const Identifier& id
			       , const AtlasDetectorID* helper
			       , const CaloCell_ID* calo_helper
			       , const EMECDetectorRegion* emecRegion
			       , const GeoAlignmentStore* alignStore)
  : CaloDetDescriptor(id,helper,calo_helper)
{
  // -- m_transform
  const Amg::Transform3D &xfDef = emecRegion->getDefAbsoluteTransform(alignStore);
  // we need to apply the famous ZShift. 
  Amg::Transform3D xfNominal;
  if(emecRegion->getEndcapIndex()==0) {
    xfNominal = Amg::Translation3D(Amg::Vector3D(0.,0.,emecRegion->getProjectivityDisplacement()*Gaudi::Units::cm))*xfDef; // Negative EMEC
  }
  else {
    xfNominal = Amg::Translation3D(Amg::Vector3D(0.,0.,-emecRegion->getProjectivityDisplacement()*Gaudi::Units::cm))*xfDef; // Positive EMEC
  }
  const Amg::Transform3D &xfAbs = emecRegion->getAbsoluteTransform(alignStore);

  m_transform = xfAbs*xfNominal.inverse();
}

EMECDescriptor::~EMECDescriptor()
{
}

// -- HECDescriptor --
HECDescriptor::HECDescriptor(const Identifier& id
			     , const AtlasDetectorID* helper
			     , const CaloCell_ID* calo_helper
			     , const HECDetectorRegion* hecRegion
			     , const GeoAlignmentStore* alignStore)
  : CaloDetDescriptor(id,helper,calo_helper)
{
  // -- m_transform
  const Amg::Transform3D &xfDef = hecRegion->getDefAbsoluteTransform(alignStore);
  // we need to apply the famous ZShift
  Amg::Transform3D xfNominal;
  if(hecRegion->getEndcapIndex()==0) {
    xfNominal = Amg::Translation3D(Amg::Vector3D(0.,0.,hecRegion->getProjectivityDisplacement()*Gaudi::Units::cm))*xfDef; // Negative HEC
  }
  else {
    xfNominal = Amg::Translation3D(Amg::Vector3D(0.,0.,-hecRegion->getProjectivityDisplacement()*Gaudi::Units::cm))*xfDef; // Positive HEC
  }
  const Amg::Transform3D &xfAbs = hecRegion->getAbsoluteTransform(alignStore);

  m_transform = xfAbs*xfNominal.inverse();
}

HECDescriptor::~HECDescriptor()
{
}

// -- FCALDescriptor --
FCALDescriptor::FCALDescriptor(const Identifier& id
			       , const AtlasDetectorID* helper
			       , const CaloCell_ID* calo_helper
			       , const FCALModule* fcalModule
			       , const GeoAlignmentStore* alignStore)
  : CaloDetDescriptor(id,helper,calo_helper)
{
  // -- m_transform
  const Amg::Transform3D &xfDef = fcalModule->getDefAbsoluteTransform(alignStore);
  // we need to apply the famous ZShift.
  Amg::Transform3D xfNominal;
  if(fcalModule->getEndcapIndex()==FCALModule::POS) {
    xfNominal = Amg::Translation3D(Amg::Vector3D(0.,0.,fcalModule->getProjectivityDisplacement()*Gaudi::Units::cm))*xfDef; // Negative FCAL
  }
  else {
    xfNominal = Amg::Translation3D(Amg::Vector3D(0.,0.,-fcalModule->getProjectivityDisplacement()*Gaudi::Units::cm))*xfDef; // Positive FCAL
  }
  const Amg::Transform3D &xfAbs = fcalModule->getAbsoluteTransform(alignStore);

  m_transform = xfAbs*xfNominal.inverse();
}

FCALDescriptor::~FCALDescriptor()
{
}
