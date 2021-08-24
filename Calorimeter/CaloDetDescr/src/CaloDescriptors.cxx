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
#include "GaudiKernel/SystemOfUnits.h"


// To Define Globally:
//
// m_lar_*
//

// -- EMBDescriptor --
EMBDescriptor::EMBDescriptor(const Identifier& id, 
			     const AtlasDetectorID* helper,
			     const CaloCell_ID* calo_helper,
			     const EMBDetectorRegion* embRegion):
  CaloDetDescriptor(id,helper,calo_helper),
  m_region(embRegion)
{
  init_description();
  init_interpretation();
}

EMBDescriptor::~EMBDescriptor()
{
}

void EMBDescriptor::init_description()
{
  // -- m_calo_phi_min/m_calo_phi_max 
  // -- m_calo_z_min/m_calo_z_max
  // -- m_calo_r_min/m_calo_r_max
  /*
  m_calo_phi_min = 10000.;
  m_calo_phi_max = -10000.;
  m_calo_z_min = 10000.;
  m_calo_z_max = -10000.;

  double eta_min = 10000.;
  double eta_max = -10000.;

  for (unsigned int iPhi=m_region->beginPhiIndex();iPhi<m_region->endPhiIndex();iPhi++) 
    for (unsigned int iEta=m_region->beginEtaIndex();iEta<m_region->endEtaIndex();iEta++) 
    {	
      EMBCellConstLink cellPtr = m_region->getEMBCell(iEta,iPhi);

      if(cellPtr)
      { 
	if(cellPtr->getPhiLocalLower() < m_calo_phi_min)
	  m_calo_phi_min = cellPtr->getPhiLocalLower();
	if(cellPtr->getPhiLocalUpper() > m_calo_phi_max)
	  m_calo_phi_max = cellPtr->getPhiLocalUpper();

	if(eta_min > cellPtr->getEtaMin())
	  eta_min = cellPtr->getEtaMin();
	if(eta_max < cellPtr->getEtaMax())
	  eta_max = cellPtr->getEtaMax();
	
	if(m_calo_z_min > cellPtr->getZMinLocal(EMBCell::FRONT))
	  m_calo_z_min = cellPtr->getZMinLocal(EMBCell::FRONT);
	if(m_calo_z_min > cellPtr->getZMinLocal(EMBCell::BACK))
	  m_calo_z_min = cellPtr->getZMinLocal(EMBCell::BACK);
	if(m_calo_z_max < cellPtr->getZMaxLocal(EMBCell::FRONT))
	  m_calo_z_max = cellPtr->getZMaxLocal(EMBCell::FRONT);
	if(m_calo_z_max < cellPtr->getZMaxLocal(EMBCell::BACK))
	  m_calo_z_max = cellPtr->getZMaxLocal(EMBCell::BACK);
      }
    }

  m_calo_r_min = m_calo_z_min/sinh(eta_max);
  m_calo_r_max = m_calo_z_min/sinh(eta_min);
  */

  // -- m_transform
  const Amg::Transform3D &xfDef = m_region->getDefAbsoluteTransform();
  const Amg::Transform3D &xfAbs = m_region->getAbsoluteTransform();

  m_transform = xfAbs*xfDef.inverse();
}

void EMBDescriptor::init_interpretation()
{
}


// -- EMECDescriptor --
EMECDescriptor::EMECDescriptor(const Identifier& id, 
			       const AtlasDetectorID* helper,
			       const CaloCell_ID* calo_helper,
			       const EMECDetectorRegion* emecRegion):
  CaloDetDescriptor(id,helper,calo_helper),
  m_region(emecRegion)
{
  init_description();
  init_interpretation();
}

EMECDescriptor::~EMECDescriptor()
{
}

void EMECDescriptor::init_description()
{
  // -- m_calo_phi_min/m_calo_phi_max 
  // -- m_calo_z_min/m_calo_z_max
  // -- m_calo_r_min/m_calo_r_max
  /*
  m_calo_phi_min = 10000.;
  m_calo_phi_max = -10000.;
  m_calo_z_min = 10000.;
  m_calo_z_max = -10000.;

  double eta_min = 10000.;
  double eta_max = -10000.;

  for (unsigned int iPhi=m_region->beginPhiIndex();iPhi<m_region->endPhiIndex();iPhi++) 
    for (unsigned int iEta=m_region->beginEtaIndex();iEta<m_region->endEtaIndex();iEta++) 
    {	
      EMECCellConstLink cellPtr = m_region->getEMECCell(iEta,iPhi);

      if(cellPtr)
      { 
	if(cellPtr->getPhiLocalLower()<m_calo_phi_min)
	  m_calo_phi_min = cellPtr->getPhiLocalLower();
	if(cellPtr->getPhiLocalUpper()>m_calo_phi_max)
	  m_calo_phi_max = cellPtr->getPhiLocalUpper();

	if(eta_min > cellPtr->getEtaMin())
	  eta_min = cellPtr->getEtaMin();
	if(eta_max < cellPtr->getEtaMax())
	  eta_max = cellPtr->getEtaMax();
	
	if(m_calo_z_min > cellPtr->getZLocal(EMECCell::FRONT))
	  m_calo_z_min = cellPtr->getZLocal(EMECCell::FRONT);
	if(m_calo_z_min > cellPtr->getZLocal(EMECCell::BACK))
	  m_calo_z_min = cellPtr->getZLocal(EMECCell::BACK);
	if(m_calo_z_max < cellPtr->getZLocal(EMECCell::FRONT))
	  m_calo_z_max = cellPtr->getZLocal(EMECCell::FRONT);
	if(m_calo_z_max < cellPtr->getZLocal(EMECCell::BACK))
	  m_calo_z_max = cellPtr->getZLocal(EMECCell::BACK);
      }
    }

  m_calo_r_min = m_calo_z_min/sinh(eta_max);
  m_calo_r_max = m_calo_z_min/sinh(eta_min);
  */
  // -- m_transform
  const Amg::Transform3D &xfDef = m_region->getDefAbsoluteTransform();
  // we need to apply the famous ZShift. 
  Amg::Transform3D xfNominal;
  if(m_region->getEndcapIndex()==0)
    xfNominal = Amg::Translation3D(Amg::Vector3D(0.,0.,m_region->getProjectivityDisplacement()*Gaudi::Units::cm))*xfDef; // Negative EMEC
  else
    xfNominal = Amg::Translation3D(Amg::Vector3D(0.,0.,-m_region->getProjectivityDisplacement()*Gaudi::Units::cm))*xfDef; // Positive EMEC

  const Amg::Transform3D &xfAbs = m_region->getAbsoluteTransform();

  m_transform = xfAbs*xfNominal.inverse();
}

void EMECDescriptor::init_interpretation()
{
}

// -- HECDescriptor --
HECDescriptor::HECDescriptor(const Identifier& id, 
			     const AtlasDetectorID* helper,
			     const CaloCell_ID* calo_helper,
			     const HECDetectorRegion* hecRegion):
  CaloDetDescriptor(id,helper,calo_helper),
  m_region(hecRegion)
{
  init_description();
  init_interpretation();
}

HECDescriptor::~HECDescriptor()
{
}

void HECDescriptor::init_description()
{
  // -- m_calo_phi_min/m_calo_phi_max 
  // -- m_calo_z_min/m_calo_z_max
  // -- m_calo_r_min/m_calo_r_max
  /*
  m_calo_phi_min = 10000.;
  m_calo_phi_max = -10000.;
  m_calo_z_min = 10000.;
  m_calo_z_max = -10000.;

  double eta_min = 10000.;
  double eta_max = -10000.;

  for (unsigned int iPhi=m_region->beginPhiIndex();iPhi<m_region->endPhiIndex();iPhi++) 
    for (unsigned int iEta=m_region->beginEtaIndex();iEta<m_region->endEtaIndex();iEta++) 
    {	
      HECCellConstLink cellPtr = m_region->getHECCell(iEta,iPhi);

      if(cellPtr)
      { 
	if(cellPtr->getPhiLocalLower()<m_calo_phi_min)
	  m_calo_phi_min = cellPtr->getPhiLocalLower();
	if(cellPtr->getPhiLocalUpper()>m_calo_phi_max)
	  m_calo_phi_max = cellPtr->getPhiLocalUpper();

	if(eta_min > cellPtr->getEtaMinNominal())
	  eta_min = cellPtr->getEtaMinNominal();
	if(eta_max < cellPtr->getEtaMaxNominal())
	  eta_max = cellPtr->getEtaMaxNominal();
	
	if(m_calo_z_min > cellPtr->getZLocal(HECCell::FRONT))
	  m_calo_z_min = cellPtr->getZLocal(HECCell::FRONT);
	if(m_calo_z_min > cellPtr->getZLocal(HECCell::BACK))
	  m_calo_z_min = cellPtr->getZLocal(HECCell::BACK);
	if(m_calo_z_max < cellPtr->getZLocal(HECCell::FRONT))
	  m_calo_z_max = cellPtr->getZLocal(HECCell::FRONT);
	if(m_calo_z_max < cellPtr->getZLocal(HECCell::BACK))
	  m_calo_z_max = cellPtr->getZLocal(HECCell::BACK);
      }
    }

  m_calo_r_min = m_calo_z_min/sinh(eta_max);
  m_calo_r_max = m_calo_z_min/sinh(eta_min);

  */

  // -- m_transform
  const Amg::Transform3D &xfDef = m_region->getDefAbsoluteTransform();
  // we need to apply the famous ZShift
  Amg::Transform3D xfNominal;
  if(m_region->getEndcapIndex()==0)
    xfNominal = Amg::Translation3D(Amg::Vector3D(0.,0.,m_region->getProjectivityDisplacement()*Gaudi::Units::cm))*xfDef; // Negative HEC
  else
    xfNominal = Amg::Translation3D(Amg::Vector3D(0.,0.,-m_region->getProjectivityDisplacement()*Gaudi::Units::cm))*xfDef; // Positive HEC

  const Amg::Transform3D &xfAbs = m_region->getAbsoluteTransform();

  m_transform = xfAbs*xfNominal.inverse();
}

void HECDescriptor::init_interpretation()
{
}

// -- FCALDescriptor --
FCALDescriptor::FCALDescriptor(const Identifier& id, 
			       const AtlasDetectorID* helper,
			       const CaloCell_ID* calo_helper,
			       const FCALModule* fcalModule):
  CaloDetDescriptor(id,helper,calo_helper),
  m_module(fcalModule)
{
  init_description();
  init_interpretation();
}

FCALDescriptor::~FCALDescriptor()
{
}

void FCALDescriptor::init_description()
{
  // -- m_transform
  const Amg::Transform3D &xfDef = m_module->getDefAbsoluteTransform();
  // we need to apply the famous ZShift.
  Amg::Transform3D xfNominal;
  if(m_module->getEndcapIndex()==FCALModule::POS)
    xfNominal = Amg::Translation3D(Amg::Vector3D(0.,0.,m_module->getProjectivityDisplacement()*Gaudi::Units::cm))*xfDef; // Negative FCAL
  else
    xfNominal = Amg::Translation3D(Amg::Vector3D(0.,0.,-m_module->getProjectivityDisplacement()*Gaudi::Units::cm))*xfDef; // Positive FCAL

  const Amg::Transform3D &xfAbs = m_module->getAbsoluteTransform();

  m_transform = xfAbs*xfNominal.inverse();
}

void FCALDescriptor::init_interpretation()
{
}

