/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloDetDescr/CaloDetDescrElement.h
 *
 * @brief Definition of CaloDetDescrElement
 *
 * $Id: CaloDetDescrElement.h,v 1.32 2009-03-30 09:56:32 tsulaia Exp $
 */

#ifndef CALODETECTORELEMENT_H
#define CALODETECTORELEMENT_H

#include "Identifier/Identifiable.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "CaloDetDescr/CaloDetDescriptor.h"

/** @class CaloDetDescrElement
   
  @brief This class groups all DetDescr information related to a CaloCell. Provides a generic interface
  for all calo cells. Subsystem specific information is included in daughter clases defined
  CaloDetectorElements.h

  The elements are creanted in different ways for LAr and Tile

  LAr: Subsystem specific readout geometry description is built by LArGeoModel
  applications and stored in the special objects defined by LArReadoutGeometry package.
  These objects are then used by converter in order to create CaloDetDescrElements for LAr.

  Tile: CaloDetDescrElements for Tile are built by TileGeoModel application and stored
  into TileDetDescrManager. The converter simply copies them from Tile manager to the Calo manager.

  The CaloDetDescrManager holds an array of pointers to the elements, and all the access to them.
  
  COORDINATE SYSTEMS and CONVENTION :

  1) the PHI convention is the Atlas one : -pi,pi 

  2) because the term calorimeter stands for 5 sub-detectors,
     we have to deal with 3 coordinate systems :

     "cartesian coordinates" are what it says (FCAL,TILE)
     "cylindric coordinates" are --eta-- phi and r (EMB)
     "ec-cylindric coordinates" are --eta-- phi and --z-- (EMEC,HEC)

     The class is made to work in both: when set_cylindric is
     used, the cartesian coordinates are re-computes and 
     vice-versa. The only exception is for cell size : dx, dy, dz
     is not translated into deta, dphi, dr ( ... yet )

     To avoid confusion, the name of the variables is explicit.

  3) the calorimeter reconstruction needs 2 coordinate systems :

     "raw" stands for Ideal/pointing positions (name is historical) 
     the other variables, not labeled as raw, are real and sensitive
     to (mis)alignment

  4) the cells volume are read from the Geometry DB
 */
class CaloDetDescrElement : public Identifiable
{
 protected:

  /** @brief Constructor
      @param subcaloHash [IN] hash id for the cell
      @param onl1 [IN] online identifier for Tile (1)
      @param onl2 [IN] online identifier for Tile (2)
      @param descriptor [IN] descriptor object
   */
  CaloDetDescrElement(const IdentifierHash subcaloHash,
		      const IdentifierHash onl1,
		      const IdentifierHash onl2,
		      const CaloDetDescriptor* descriptor);
 public:
  /** @brief virtual destructor
   */
  virtual ~CaloDetDescrElement();

  /** @brief cell eta
   */
  float eta() const;
  /** @brief cell phi
   */
  float phi() const;
  /** @brief cell r
   */
  float r() const;
  /** @brief cell eta_raw
   */
  float eta_raw() const;
  /** @brief cell phi_raw
   */
  float phi_raw() const;
  /** @brief cell r_raw
   */
  float r_raw() const;
  /** @brief cell dphi
   */
  float dphi() const;
  /** @brief cell deta
   */
  float deta() const;
  /** @brief cell dr
   */
  float dr() const;
    
  /** @brief cell x
   */
  float x() const;
  /** @brief cell y
   */
  float y() const;
  /** @brief cell z
   */
  float z() const;
  /** @brief cell x_raw
   */
  float x_raw() const;
  /** @brief cell y_raw
   */
  float y_raw() const;
  /** @brief cell z_raw
   */
  float z_raw() const;
  /** @brief cell dx
   */
  float dx() const;
  /** @brief cell dy
   */
  float dy() const;
  /** @brief cell dz
   */
  float dz() const;
  
  /** @brief cell volume
   */
  float volume() const;

  /** @brief for algorithm working in transverse Energy
   */
  float sinTh() const;

  /** @brief for fast px py pz computation
   */
  float cosTh() const;

  /** @brief for fast px py pz computation
   */
  float cotTh() const;

  /** @brief for fast px py pz computation
   */
  float sinPhi() const;

  /** @brief for fast px py pz computation
   */
  float cosPhi() const;

  /** @brief set cell volume
   */
  void set_volume(double volume);
  /** @brief set cell online identifiers (Tile)
   */
  void set_online(const IdentifierHash onl1,
		  const IdentifierHash onl2);

  /** @brief cell identifier
   */
  Identifier identify () const;

  /** @brief cell online identifier 1
   */
  IdentifierHash onl1() const;
  /** @brief cell online identifier 2
   */
  IdentifierHash onl2() const;
  /** @brief cell calo hash
   */
  IdentifierHash calo_hash() const;
  /** @brief cell subcalo hash
   */
  IdentifierHash subcalo_hash() const;

  /** @brief cell subcalo hash 
      same as subcalo_hash(), but kept for backward compatibility 
  */
  IdentifierHash identifyHash() const;

  /** @brief cell descriptor
   */
  const CaloDetDescriptor* descriptor () const;

  /** @brief print element contents
   */
  virtual void print() const;

  /** @brief cell sampling
   */
  CaloCell_ID::CaloSample getSampling() const ;
  /** @brief cell subcalo
   */
  CaloCell_ID::SUBCALO getSubCalo() const ;

  /** @brief cell belongs to EM calorimeter
   */
  bool is_lar_em() const; 
  /** @brief cell belongs to EM barrel
   */
  bool is_lar_em_barrel() const; 
  /** @brief cell belongs to EM end cap
   */
  bool is_lar_em_endcap() const; 
  /** @brief cell belongs to the inner wheel of EM end cap
   */
  bool is_lar_em_endcap_inner() const; 
  /** @brief cell belongs to the outer wheel of EM end cap
   */
  bool is_lar_em_endcap_outer() const; 
  /** @brief cell belongs to HEC
   */
  bool is_lar_hec() const;
  /** @brief cell belongs to FCAL
   */
  bool is_lar_fcal() const; 
  /** @brief cell belongs to Tile
   */
  bool is_tile() const; 

  /** @brief cell layer
   */
  virtual int getLayer() const;
  
 private:
  /** @brief Constructor, hidden
   */
  CaloDetDescrElement();
  
  const CaloDetDescriptor* m_descriptor;
  IdentifierHash  m_caloHash;    
  
 protected:    
  /** @brief cylindric coordinates : eta
   */
  float m_eta;
  /** @brief cylindric coordinates : phi
   */
  float m_phi;

  /** @brief this one is cached for algorithm working in transverse Energy
   */
  float m_sinTh;

  /** @brief this one is cached for algorithm working in transverse Energy
   */
  float m_cosTh;

  /** @brief cylindric coordinates : delta eta
   */
  float m_deta;
  /** @brief cylindric coordinates : delta phi
   */
  float m_dphi;

  /** @brief cell volume
   */
  float m_volume;    
  
  /** @brief cache to allow fast px py pz computation
   */
  float m_sinPhi;

  /** @brief cache to allow fast px py pz computation
   */
  float m_cosPhi;

  /** @brief cylindric coordinates : r
   */
  float m_r;
  /** @brief cylindric coordinates : eta_raw
   */
  float m_eta_raw;
  /** @brief cylindric coordinates : phi_raw
   */
  float m_phi_raw;
  /** @brief cylindric coordinates : r_raw
   */
  float m_r_raw;
  /** @brief cylindric coordinates : delta r
   */
  float m_dr;
  
  /** @brief cartesian coordinates : X
   */
  float m_x;
  /** @brief cartesian coordinates : Y
   */
  float m_y;
  /** @brief cartesian coordinates : Z
   */
  float m_z;
  /** @brief cartesian coordinates : X raw
   */
  float m_x_raw;
  /** @brief cartesian coordinates : Y raw
   */
  float m_y_raw;
  /** @brief cartesian coordinates : Z raw
   */
  float m_z_raw;
  /** @brief cartesian coordinates : delta X
   */
  float m_dx;
  /** @brief cartesian coordinates : delta Y
   */
  float m_dy;
  /** @brief cartesian coordinates : delta Z
   */
  float m_dz;
  

  /** @brief Tiles need 2 online hashed id
   */
  IdentifierHash m_onl1;    
  /** @brief Tiles need 2 online hashed id
   */
  IdentifierHash m_onl2;    

  /** @brief In test beam configurations force XXX=XXX_RAW
   */
  void propagateRaw();

  // For MBTS
  virtual Identifier customID() const;
};

inline float CaloDetDescrElement::eta() const
{ return m_eta;}
inline float CaloDetDescrElement::phi() const
{ return m_phi;}
inline float CaloDetDescrElement::r() const
{ return m_r;}
inline float CaloDetDescrElement::eta_raw() const
{ return m_eta_raw;}
inline float CaloDetDescrElement::phi_raw() const
{ return m_phi_raw;}
inline float CaloDetDescrElement::r_raw() const
{ return m_r_raw;}
inline float CaloDetDescrElement::deta() const
{ return m_deta;}
inline float CaloDetDescrElement::dphi() const
{ return m_dphi;}
inline float CaloDetDescrElement::dr() const
{ return m_dr;}

inline float CaloDetDescrElement::x() const
{ return m_x;}
inline float CaloDetDescrElement::y() const
{ return m_y;}
inline float CaloDetDescrElement::z() const
{ return m_z;}
inline float CaloDetDescrElement::x_raw() const
{ return m_x_raw;}
inline float CaloDetDescrElement::y_raw() const
{ return m_y_raw;}
inline float CaloDetDescrElement::z_raw() const
{ return m_z_raw;}
inline float CaloDetDescrElement::dx() const
{ return m_dx;}
inline float CaloDetDescrElement::dy() const
{ return m_dy;}
inline float CaloDetDescrElement::dz() const
{ return m_dz;}
inline float CaloDetDescrElement::volume() const
{ return m_volume;}
inline float CaloDetDescrElement::sinTh() const
{ return m_sinTh;}
inline float CaloDetDescrElement::cosTh() const
{ return m_cosTh;}
inline float CaloDetDescrElement::cotTh() const
{ return m_cosTh/m_sinTh;}
inline float CaloDetDescrElement::sinPhi() const
{ return m_sinPhi;}
inline float CaloDetDescrElement::cosPhi() const
{ return m_cosPhi;}

inline void CaloDetDescrElement::set_volume(double volume)
{ m_volume = static_cast<float> (volume);}

inline IdentifierHash
CaloDetDescrElement::onl1() const
{ return m_onl1; }

inline IdentifierHash
CaloDetDescrElement::onl2() const
{ return m_onl2; }

inline IdentifierHash
CaloDetDescrElement::calo_hash() const
{
  return m_caloHash;
}

inline IdentifierHash
CaloDetDescrElement::subcalo_hash() const
{ return m_descriptor ?
    IdentifierHash(m_caloHash - m_descriptor->caloCellMin()) :
    m_caloHash;}

inline IdentifierHash
CaloDetDescrElement::identifyHash() const
{ return subcalo_hash(); }

inline const CaloDetDescriptor* 	
CaloDetDescrElement::descriptor	() const
{ return m_descriptor; }


inline
CaloCell_ID::SUBCALO CaloDetDescrElement::getSubCalo() const
{
  if(m_descriptor)
    return m_descriptor->getSubCalo();
  else
    return CaloCell_ID::NOT_VALID;
}

inline
bool CaloDetDescrElement::is_tile() const 
{ 
  if(m_descriptor)
    return m_descriptor->is_tile(); 
  else
    return false;
} 


#endif
