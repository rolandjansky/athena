/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloDescriptors.h
 *
 * @brief LAr subsystem specific descriptors
 *
 * @author Vakho Tsulaia
 *
 * $Id: CaloDescriptors.h,v 1.3 2009-04-22 19:54:16 ssnyder Exp $
 */

#ifndef CALODETDESCR_CALODESCRIPTORS_H
#define CALODETDESCR_CALODESCRIPTORS_H

#include "CaloDetDescr/CaloDetDescriptor.h"

class EMBDetectorRegion;
class EMECDetectorRegion;
class HECDetectorRegion;
class FCALModule;

class GeoAlignmentStore;

/**
 * @class EMBDescriptor
 *
 * @brief LAr EMB Descriptor
 */

class EMBDescriptor : public CaloDetDescriptor
{
 public:
  /**
   * @brief Constructor
   * @param id [IN] region identifier (for base class constructor)
   * @param helper [IN] ID helper (for base class constructor)
   * @param calo_helper [IN] ID helper (for base class constructor)
   * @param embRegion [IN] EMB Region Descriptor from LArReadoutGeometry
   */
  EMBDescriptor(const Identifier& id
		, const AtlasDetectorID* helper
		, const CaloCell_ID* calo_helper
		, const EMBDetectorRegion* embRegion
		, const GeoAlignmentStore* alignStore = nullptr);

  /**
   * @brief Destructor
   */
  virtual ~EMBDescriptor();

 private:
  /**
   * @brief default constructor hidden
   */
  EMBDescriptor() = delete;

  /**
   * @brief copy constructor hidden
   */
  EMBDescriptor & operator=(const EMBDescriptor &right) = delete;
};

/**
 * @class EMECDescriptor
 *
 * @brief LAr EMEC Descriptor
 */
class EMECDescriptor : public CaloDetDescriptor
{
 public:
  /**
   * @brief Constructor
   * @param id [IN] region identifier (for base class constructor)
   * @param helper [IN] ID helper (for base class constructor)
   * @param calo_helper [IN] ID helper (for base class constructor)
   * @param emecRegion [IN] EMEC Region Descriptor from LArReadoutGeometry
   */
  EMECDescriptor(const Identifier& id
		 , const AtlasDetectorID* helper
		 , const CaloCell_ID* calo_helper
		 , const EMECDetectorRegion* emecRegion
		 , const GeoAlignmentStore* alignStore = nullptr);

  /**
   * @brief Destructor
   */
  virtual ~EMECDescriptor();

 private:
  /**
   * @brief default constructor hidden
   */
  EMECDescriptor() = delete;

  /**
   * @brief copy constructor hidden
   */
  EMECDescriptor & operator=(const EMECDescriptor &right) = delete;
};

/**
 * @class HECDescriptor
 *
 * @brief LAr HEC Descriptor
 */
class HECDescriptor : public CaloDetDescriptor
{
 public:
  /**
   * @brief Constructor
   * @param id [IN] region identifier (for base class constructor)
   * @param helper [IN] ID helper (for base class constructor)
   * @param calo_helper [IN] ID helper (for base class constructor)
   * @param hecRegion [IN] HEC Region Descriptor from LArReadoutGeometry
   */
  HECDescriptor(const Identifier& id
		, const AtlasDetectorID* helper
		, const CaloCell_ID* calo_helper
		, const HECDetectorRegion* hecRegion
		, const GeoAlignmentStore* alignStore = nullptr);
  /**
   * @brief Destructor
   */
  virtual ~HECDescriptor();

 private:
  /**
   * @brief default constructor hidden
   */
  HECDescriptor() = delete;

  /**
   * @brief copy constructor hidden
   */
  HECDescriptor & operator=(const HECDescriptor &right) = delete;
};

/**
 * @class FCALDescriptor
 *
 * @brief LAr FCAL Descriptor
 */
class FCALDescriptor : public CaloDetDescriptor
{
 public:
  /**
   * @brief Constructor
   * @param id [IN] region identifier (for base class constructor)
   * @param helper [IN] ID helper (for base class constructor)
   * @param calo_helper [IN] ID helper (for base class constructor)
   * @param fcalModule [IN] FCAL Module Descriptor from LArReadoutGeometry
   */
  FCALDescriptor(const Identifier& id
		 , const AtlasDetectorID* helper
		 , const CaloCell_ID* calo_helper
		 , const FCALModule* fcalModule
		 , const GeoAlignmentStore* alignStore = nullptr);

  /**
   * @brief Destructor
   */
  virtual ~FCALDescriptor();

 private:
  /**
   * @brief default constructor hidden
   */
  FCALDescriptor() = delete;

  /**
   * @brief copy constructor hidden
   */
  FCALDescriptor & operator=(const FCALDescriptor &right) = delete;
};

#endif
