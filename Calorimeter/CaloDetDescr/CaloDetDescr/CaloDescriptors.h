/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloDescriptors.h
 *
 * @brief LAr subsystem specific descriptors
 *
 * @author Vakho Tsulaia (tsulaia@mail.cern.ch)
 *
 * $Id: CaloDescriptors.h,v 1.3 2009-04-22 19:54:16 ssnyder Exp $
 */

#ifndef CALODESCRIPTORS_H
#define CALODESCRIPTORS_H

#include "CaloDetDescr/CaloDetDescriptor.h"

class EMBDetectorRegion;
class EMECDetectorRegion;
class HECDetectorRegion;
class FCALModule;

/**
 * @class EMBDescriptor
 *
 * @brief LAr EMB Descriptor
 */

class EMBDescriptor : public CaloDetDescriptor
{
 public:
  /** @brief Constructor
      @param id [IN] region identifier (for base class constructor)
      @param helper [IN] ID helper (for base class constructor)
      @param calo_helper [IN] ID helper (for base class constructor)
      @param embRegion [IN] EMB Region Descriptor from LArReadoutGeometry
   */
  EMBDescriptor(const Identifier& id, 
		const AtlasDetectorID* helper,
		const CaloCell_ID* calo_helper,
		const EMBDetectorRegion* embRegion);
  /** @brief Destructor
   */
  ~EMBDescriptor();

  /** @brief get pointer to the EMB Region object (from LArReadoutGeometry)
   */
  inline const EMBDetectorRegion* getEMBDetectorRegion() const
  {return m_region;} 

 private:
  /** @brief pointer to the EMB Region object (from LArReadoutGeometry)
   */
  const EMBDetectorRegion* m_region;

  /** @brief default constructor hidden
   */
  EMBDescriptor();

  /** @brief copy constructor hidden
   */
  EMBDescriptor & operator=(const EMBDescriptor &right);

  /** @brief initialize base description
   */
  void init_description();


  /** @brief Fill all missing fields of CaloDetDescriptor which
      have not been filled by init_description()
   */
  void init_interpretation();
};

/**
 * @class EMECDescriptor
 *
 * @brief LAr EMEC Descriptor
 */
class EMECDescriptor : public CaloDetDescriptor
{
 public:
  /** @brief Constructor
      @param id [IN] region identifier (for base class constructor)
      @param helper [IN] ID helper (for base class constructor)
      @param calo_helper [IN] ID helper (for base class constructor)
      @param emecRegion [IN] EMEC Region Descriptor from LArReadoutGeometry
   */
  EMECDescriptor(const Identifier& id,
		 const AtlasDetectorID* helper, 
		 const CaloCell_ID* calo_helper,
		 const EMECDetectorRegion* emecRegion);

  /** @brief Destructor
   */
  ~EMECDescriptor();

  /** @brief get pointer to the EMEC Region object (from LArReadoutGeometry)
   */
  inline const EMECDetectorRegion* getEMECDetectorRegion() const
  {return m_region;}

 private:
  /** @brief pointer to the EMEC Region object (from LArReadoutGeometry)
   */
  const EMECDetectorRegion* m_region;

  /** @brief default constructor hidden
   */
  EMECDescriptor();

  /** @brief copy constructor hidden
   */
  EMECDescriptor & operator=(const EMECDescriptor &right);

  /** @brief initialize base description
   */
  void init_description();

  /** @brief Fill all missing fields of CaloDetDescriptor which
      have not been filled by init_description()
   */
  void init_interpretation();

};

/**
 * @class HECDescriptor
 *
 * @brief LAr HEC Descriptor
 */
class HECDescriptor : public CaloDetDescriptor
{
 public:
  /** @brief Constructor
      @param id [IN] region identifier (for base class constructor)
      @param helper [IN] ID helper (for base class constructor)
      @param calo_helper [IN] ID helper (for base class constructor)
      @param hecRegion [IN] HEC Region Descriptor from LArReadoutGeometry
   */
  HECDescriptor(const Identifier& id,
		const AtlasDetectorID* helper, 
		const CaloCell_ID* calo_helper,
		const HECDetectorRegion* hecRegion);

  /** @brief Destructor
   */
  ~HECDescriptor();

  /** @brief get pointer to the HEC Region object (from LArReadoutGeometry)
   */
  inline const HECDetectorRegion* getHECDetectorRegion() const 
  {return m_region;}
 

 private:
  /** @brief pointer to the HEC Region object (from LArReadoutGeometry)
   */
  const HECDetectorRegion* m_region;

  /** @brief default constructor hidden
   */
  HECDescriptor();

  /** @brief copy constructor hidden
   */
  HECDescriptor & operator=(const HECDescriptor &right);


  /** @brief initialize base description
   */
  void init_description();

  /** @brief Fill all missing fields of CaloDetDescriptor which
      have not been filled by init_description()
   */
  void init_interpretation();

};

/**
 * @class FCALDescriptor
 *
 * @brief LAr FCAL Descriptor
 */
class FCALDescriptor : public CaloDetDescriptor
{
 public:
  /** @brief Constructor
      @param id [IN] region identifier (for base class constructor)
      @param helper [IN] ID helper (for base class constructor)
      @param calo_helper [IN] ID helper (for base class constructor)
      @param fcalModule [IN] FCAL Module Descriptor from LArReadoutGeometry
   */
  FCALDescriptor(const Identifier& id,
		 const AtlasDetectorID* helper, 
		 const CaloCell_ID* calo_helper,
		 const FCALModule* fcalModule);

  /** @brief Destructor
   */
  ~FCALDescriptor();

  /** @brief get pointer to the FCAL Module object (from LArReadoutGeometry)
   */
  inline const FCALModule* getFCALModule() const
  {return m_module;}

 private:
  /** @brief pointer to the FCAL Module object (from LArReadoutGeometry)
   */
  const FCALModule* m_module;

  /** @brief default constructor hidden
   */
  FCALDescriptor();

  /** @brief copy constructor hidden
   */
  FCALDescriptor & operator=(const FCALDescriptor &right);

  /** @brief initialize base description
   */
  void init_description();

  /** @brief Fill all missing fields of CaloDetDescriptor which
      have not been filled by init_description()
   */
  void init_interpretation();

};

#endif
