/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloDetectorElements.h
 *
 * @brief Calo Subsystem specific Detector Elements + Dummy element for testing
 *
 * $Id: CaloDetectorElements.h,v 1.13 2009-04-22 19:54:16 ssnyder Exp $
 */

#ifndef CALODETECTORELEMENTS_H
#define CALODETECTORELEMENTS_H

#include "CaloDetDescr/CaloDetDescrElement.h"

#include "LArReadoutGeometry/EMBCell.h"
#include "LArReadoutGeometry/EMBDetectorRegion.h"
#include "LArReadoutGeometry/EMECCell.h"
#include "LArReadoutGeometry/EMECDetectorRegion.h"
#include "LArReadoutGeometry/HECCell.h"
#include "LArReadoutGeometry/HECDetectorRegion.h"
#include "LArReadoutGeometry/FCALTile.h"
#include "LArReadoutGeometry/FCALModule.h"

// MBTS
#include "Identifier/Identifier.h"

class CaloDetDescriptor;

/**
 * @struct CaloElementPositionShift
 *
 * @brief dx,dy,dz displacement of the calorimeter cell caused by sagging
 */

typedef struct 
{
  float dx;
  float dy;
  float dz;
} CaloElementPositionShift;

/**
 * @class EMBDetectorElement
 *
 * @brief LAr EMB Detector Element
 */
class EMBDetectorElement : public CaloDetDescrElement
{
 public:
  /** @brief Constructor, takes all necessary parameters for the base class constructor 
      plus some specific parameters for this class
      @param subcaloHash [IN] for the base class
      @param onl1 [IN] for the base class
      @param onl2 [IN] for the base class
      @param descriptor [IN] for the base class
      @param embCell [IN] EMB Cell description from LArReadoutGeometry
      @param embRegion [IN] EMB Region description from LArReadoutGeometry
   */
  EMBDetectorElement(const IdentifierHash subcaloHash,
		     const IdentifierHash onl1,
		     const IdentifierHash onl2,
		     const CaloDetDescriptor* descriptor,
		     EMBCellConstLink& embCell,
		     const EMBDetectorRegion* embRegion,
		     bool isTestBeam);
  
  /** @brief EMB Cell description from LArReadoutGeometry
   */
  inline EMBCellConstLink         getEMBCell() const          {return m_cell;}
  /** @brief EMB Region description from LArReadoutGeometry
   */
  inline const EMBDetectorRegion* getEMBDetectorRegion() const
  {return m_region;}
  
  /** @brief the method called by CaloAlignTool, updates alignment information
      @param embCell [IN] misaligned EMB Cell description from LArReadoutGeometry
      @param embRegion [IN] misaligned EMB Region description from LArReadoutGeometry
   */
  void updateAlignment(EMBCellConstLink& embCell,
		       const EMBDetectorRegion* embRegion,
		       const CaloElementPositionShift* posShift = 0);

  /** @brief get layer
   */
  int getLayer() const;

 private:
  /** @brief EMB Cell description from LArReadoutGeometry
   */
  EMBCellConstLink m_cell;
  /** @brief EMB Region description from LArReadoutGeometry
   */
  const EMBDetectorRegion* m_region;
  
  /** @brief default constructor hidden
   */
  EMBDetectorElement();
  /** @brief copy constructor hidden
   */
  EMBDetectorElement & operator=(const EMBDetectorElement &right);
  
  /** @brief initialize base description
   */
  void init_description(const CaloElementPositionShift* posShift = 0);

  /** @brief Fill all missing fields of CaloDetDescrElement which
      have not been filled by init_description()
   */
  void init_interpretation();  
};

/**
 * @class EMECDetectorElement
 *
 * @brief LAr EMEC Detector Element
 */
class EMECDetectorElement : public CaloDetDescrElement
{
 public:
  /** @brief Constructor, takes all necessary parameters for the base class constructor 
      plus some specific parameters for this class
      @param subcaloHash [IN] for the base class
      @param onl1 [IN] for the base class
      @param onl2 [IN] for the base class
      @param descriptor [IN] for the base class
      @param emecCell [IN] EMEC Cell description from LArReadoutGeometry
      @param emecRegion [IN] EMEC Region description from LArReadoutGeometry
   */
  EMECDetectorElement(const IdentifierHash subcaloHash,
		      const IdentifierHash onl1,
		      const IdentifierHash onl2,
		      const CaloDetDescriptor* descriptor,
		      EMECCellConstLink& emecCell,
		      const EMECDetectorRegion* emecRegion,
		      bool isTestBeam);

  /** @brief EMEC Cell description from LArReadoutGeometry
   */
  inline EMECCellConstLink         getEMECCell() const         {return m_cell;}
  /** @brief EMEC Region description from LArReadoutGeometry
   */
  inline const EMECDetectorRegion* getEMECDetectorRegion() const
  {return m_region;}

  /** @brief the method called by CaloAlignTool, updates alignment information
      @param emecCell [IN] misaligned EMEC Cell description from LArReadoutGeometry
      @param emecRegion [IN] misaligned EMEC Region description from LArReadoutGeometry
   */
  void updateAlignment(EMECCellConstLink& emecCell,
		       const EMECDetectorRegion* emecRegion,
		       const CaloElementPositionShift* posShift = 0);

  /** @brief get layer
   */
  int getLayer() const;

 private:
  /** @brief EMEC Cell description from LArReadoutGeometry
   */
  EMECCellConstLink m_cell;
  /** @brief EMEC Region description from LArReadoutGeometry
   */
  const EMECDetectorRegion* m_region;
 
  /** @brief default constructor hidden
   */
  EMECDetectorElement();
  /** @brief copy constructor hidden
   */
  EMECDetectorElement & operator=(const EMECDetectorElement &right);

  /** @brief initialize base description
   */
  void init_description(bool isTestBeam,
			const CaloElementPositionShift* posShift = 0);

  /** @brief Fill all missing fields of CaloDetDescrElement which
      have not been filled by init_description()
   */
  void init_interpretation();

};


/**
 * @class HECDetectorElement
 *
 * @brief LAr HEC Detector Element
 */
class HECDetectorElement : public CaloDetDescrElement
{
 public:
  /** @brief Constructor, takes all necessary parameters for the base class constructor 
      plus some specific parameters for this class
      @param subcaloHash [IN] for the base class
      @param onl1 [IN] for the base class
      @param onl2 [IN] for the base class
      @param descriptor [IN] for the base class
      @param hecCell [IN] HEC Cell description from LArReadoutGeometry
      @param hecRegion [IN] HEC Region description from LArReadoutGeometry
   */
  HECDetectorElement(const IdentifierHash subcaloHash,
		     const IdentifierHash onl1,
		     const IdentifierHash onl2,
		     const CaloDetDescriptor* descriptor,
		     HECCellConstLink& hecCell,
		     const HECDetectorRegion* hecRegion,
		     bool isTestBeam);

  /** @brief HEC Cell description from LArReadoutGeometry
   */
  inline HECCellConstLink         getHECCell()    const       {return m_cell;}
  /** @brief HEC Region description from LArReadoutGeometry
   */
  inline const HECDetectorRegion* getHECDetectorRegion() const
  {return m_region;}

  /** @brief the method called by CaloAlignTool, updates alignment information
      @param hecCell [IN] misaligned HEC Cell description from LArReadoutGeometry
      @param hecRegion [IN] misaligned HEC Region description from LArReadoutGeometry
   */
  void updateAlignment(HECCellConstLink& hecCell,
		       const HECDetectorRegion* hecRegion,
		       const CaloElementPositionShift* posShift = 0);

  /** @brief get layer
   */
  int getLayer() const;

 private:
  /** @brief HEC Cell description from LArReadoutGeometry
   */
  HECCellConstLink m_cell;
  /** @brief HEC Region description from LArReadoutGeometry
   */
  const HECDetectorRegion* m_region;
  
  /** @brief default constructor hidden
   */
  HECDetectorElement();
  /** @brief copy constructor hidden
   */
  HECDetectorElement & operator=(const HECDetectorElement &right);

  /** @brief initialize base description
   */
  void init_description(bool isTestBeam,
			const CaloElementPositionShift* posShift = 0);

  /** @brief Fill all missing fields of CaloDetDescrElement which
      have not been filled by init_description()
   */
  void init_interpretation();

};

/**
 * @class FCALDetectorElement
 *
 * @brief LAr FCAL Detector Element
 */
class FCALDetectorElement : public CaloDetDescrElement
{
 public:
  /** @brief Constructor, takes all necessary parameters for the base class constructor 
      plus some specific parameters for this class
      @param subcaloHash [IN] for the base class
      @param onl1 [IN] for the base class
      @param onl2 [IN] for the base class
      @param descriptor [IN] for the base class
      @param fcalTile [IN] FCAL Tile description from LArReadoutGeometry
      @param fcalModule [IN] FCAL Module description from LArReadoutGeometry
   */
  FCALDetectorElement(const IdentifierHash subcaloHash,
		      const IdentifierHash onl1,
		      const IdentifierHash onl2,
		      const CaloDetDescriptor* descriptor,
		      const FCALTile* fcalTile,
		      const FCALModule* fcalModule,
		      bool isTestBeam);

  /** @brief FCAL Tile description from LArReadoutGeometry
   */
  inline const FCALTile*    getFCALTile()  const  {return m_tile;}
  /** @brief FCAL Module description from LArReadoutGeometry
   */
  inline const FCALModule*  getFCALModule() const {return m_module;}

  /** @brief the method called by CaloAlignTool, updates alignment information
      @param fcalTile [IN] misaligned FCAL Tile description from LArReadoutGeometry
      @param fcalModule [IN] misaligned FCAL Module description from LArReadoutGeometry
   */
  void updateAlignment(const FCALTile* fcalTile,
		       const FCALModule* fcalModule,
		       const CaloElementPositionShift* posShift = 0);

  /** @brief get layer
   */
  int getLayer() const;

 private:
  /** @brief FCAL Tile description from LArReadoutGeometry
   */
  const FCALTile*   m_tile;
  /** @brief FCAL Module description from LArReadoutGeometry
   */
  const FCALModule* m_module;
  
  /** @brief default constructor hidden
   */
  FCALDetectorElement();
  /** @brief copy constructor hidden
   */
  FCALDetectorElement & operator=(const FCALDetectorElement &right);

  /** @brief initialize base description
   */
  void init_description(bool isTestBeam,
			const CaloElementPositionShift* posShift = 0);

  /** @brief Fill all missing fields of CaloDetDescrElement which
      have not been filled by init_description()
   */
  void init_interpretation();

};

/**
 * @class TileDetectorElement
 *
 * @brief Tile Detector Element
 */
class TileDetectorElement : public CaloDetDescrElement
{
 public:
  /** @brief Constructor, takes all necessary parameters for the base class constructor 
   */
  TileDetectorElement(const IdentifierHash subcaloHash,
		      const IdentifierHash onl1,
		      const IdentifierHash onl2,
		      const CaloDetDescriptor* descriptor);

  /** @brief set raw cylindric coordinates
   */
  void set_cylindric_raw(double eta_raw, 
			 double phi_raw, 
			 double r_raw);

  /** @brief set cylindric coordinates
   */
  void set_cylindric(double eta, 
                     double phi, 
                     double r);

  /** @brief set cylindric size deta
   */
  inline void set_deta(double deta) {m_deta = static_cast<float> (deta);}
  /** @brief set cylindric size dphi
   */
  inline void set_dphi(double dphi) {m_dphi = static_cast<float> (dphi);}
  /** @brief set cartezian size dz
   */
  inline void set_dz(double dz) {m_dz = static_cast<float> (dz);}
  /** @brief set cylindric size dr
   */
  inline void set_dr(double dr) {m_dr = static_cast<float> (dr);}
  /** @brief set z
   */
  inline void set_z(double z) {m_z = static_cast<float> (z);}
  /** @brief set r
   */
  inline void set_r(double r) {
    m_r = static_cast<float> (r);
    m_x = static_cast<float> (m_cosPhi * r);
    m_y = static_cast<float> (m_sinPhi * r);
  }
};

/**
 * @class MbtsDetectorElement
 *
 * @brief Mbts Detector Element
 */
class MbtsDetectorElement : public CaloDetDescrElement
{
 public:
  /** @brief Constructor, initializes base class constructor parameters with default values
   */
  MbtsDetectorElement();

  // Initialize data members
  inline void set_id(const Identifier& id) {m_id = id;}
  inline void set_z(double z) {m_z = static_cast<float> (z);}
  inline void set_dz(double dz) {m_dz = static_cast<float> (dz);}
  inline void set_r(double r) {m_r = static_cast<float> (r);}
  inline void set_dr(double dr) {m_dr = static_cast<float> (dr);}
  inline void set_eta(double eta) {m_eta = static_cast<float> (eta);}
  inline void set_deta(double deta) {m_deta = static_cast<float> (deta);}
  inline void set_phi(double phi) {m_phi = static_cast<float> (phi);}
  inline void set_dphi(double dphi) {m_dphi = static_cast<float> (dphi);}

  // Compute derived quantities
  void compute_derived();
 
 private:
  Identifier m_id;

  virtual Identifier customID() const;
};



/**
 * @brief Element to represent a SuperCell.
 *
 * Initializes its geometry from the corresponding set of offline cells.
 */
class CaloSuperCellDetectorElement
  : public CaloDetDescrElement
{
public:
  /**
   * @brief Constructor, initializes base class constructor parameters
   *        with default values
   * @param subcaloHash The hash code of this element, within the subcalo.
   * @param descriptor Descriptor for this element.
   */
  CaloSuperCellDetectorElement(const IdentifierHash subcaloHash,
                               const CaloDetDescriptor* descriptor)
    : CaloDetDescrElement (subcaloHash, 0, 0, descriptor) {}


  /**
   * @brief Update this element's geometry from the given list
   *        of offline elements.
   * @param fromelts List of offline elements.
   */
  StatusCode update (const std::vector<const CaloDetDescrElement*>& fromelts);


private:
  /**
   * @brief Set this element's geometry to default values.
   */
  StatusCode updateNull();


  /**
   * @brief Copy this element's geometry from the given offline element.
   */
  StatusCode updateSingle (const CaloDetDescrElement* fromelt);


  /**
   * @brief Update this element's geometry from a list of elements.
   *        For elements in the barrel / endcap.
   * @param fromelts List of offline elements.
   */
  StatusCode
  updateBE (const std::vector<const CaloDetDescrElement*>& fromelts);


  /**
   * @brief Update this element's geometry from a list of elements.
   *        For elements in the FCAL.
   * @param fromelts List of offline elements.
   */
  StatusCode
  updateFCAL (const std::vector<const CaloDetDescrElement*>& fromelts);
};


/**
 * @class DummyDetDescrElement
 *
 * @brief Dummy Detector Element for testing
 */
class DummyDetDescrElement : public CaloDetDescrElement
{
 public:
  /** @brief Constructor, takes all necessary parameters for the base class constructor 
   */
  DummyDetDescrElement(const IdentifierHash subcaloHash,
                       const IdentifierHash onl1,
                       const IdentifierHash onl2,
                       const CaloDetDescriptor* descriptor);

  /** @brief set cylindric coordinates
   */
  void set_cylindric(double eta, 
		     double phi, 
		     double r);

  /** @brief set raw cylindric coordinates
   */
  void set_cylindric_raw(double eta_raw, 
			 double phi_raw, 
			 double r_raw);

  /** @brief set cylindric size deta/dphi/dr
   */
  void set_cylindric_size(double deta, 
			  double dphi,  
			  double dr);
};

#endif
