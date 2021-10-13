/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOCELL_H
#define CALOEVENT_CALOCELL_H

/** 
   @class CaloCell
   @brief Data object for each calorimeter readout cell.
  
   CaloCell is a class with nearly all information on a calorimeter cell 
in a given event. It has 4-momentum behavior through P4EEtaPhiMBase 
inheritance, and Navigable behavior through INavigable4Momentum,
   and NavigableTerminalNode inheritance.
Further static information can be obtained through the CaloDetDescrElement 
obtained with caloDDE() method. 
 CaloCell is a base class to LArCell (which is identical) and TileCell.

    
   @author Hong Ma
   @author David Rousseau
   @author Peter Loch
 */


//  Update: 
//    Jan 10 2001 HMA
//          ordor of static/virtual/const 
//    Nov 12 2001 EDF
//          Inherit from Navigible.  Now CaloCell has a pure virtual method
//          and can not be instantiated directly.
//    June 13 2003 DR delegate to CaloDetDescrElement
//    Feb  25 2004 DR CaloGain data member
//    Apr  01 2004 PL new navigation and clean-up
//

#include "EventKernel/INavigable4Momentum.h"

#include "Navigation/NavigableTerminalNode.h"

#include "Identifier/Identifier.h"
#include "CaloIdentifier/CaloGain.h"
#include "FourMom/P4EEtaPhiMBase.h"
#include <math.h>
#include <iostream>
#include <functional>
#include <memory>

//class CaloDetDescrElement;
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "GeoPrimitives/GeoPrimitives.h"


class CaloCell: virtual public P4EEtaPhiMBase,
		virtual public INavigable4Momentum,
		public NavigableTerminalNode
{
public:

  /** @brief default constructor (should not be used, only there for persistency reason) */
  CaloCell();

  /** @brief Legacy constructor */
  CaloCell(const CaloDetDescrElement* caloDDE,
           float energy, 
	   float time, 
	   double quality,
	   CaloGain::CaloGain gain);

  /** @brief Main constructor */
  CaloCell(const CaloDetDescrElement* caloDDE,
           float energy, 
	   float time, 
	   uint16_t quality,
	   uint16_t provenance,
	   CaloGain::CaloGain gain);


  /** @brief Legacy constructor for MBTS cells which doesn't use caloDDE to get ID */
  CaloCell(const CaloDetDescrElement* caloDDE,
           const Identifier & ID,
           float energy, 
  	   float time, 
	   double quality,
	   CaloGain::CaloGain gain);

  /** @brief Main constructor for MBTS cells which doesn't use caloDDE to get ID */
  CaloCell(const CaloDetDescrElement* caloDDE,
           const Identifier & ID,
           float energy, 
	   float time, 
	   uint16_t quality,
	   uint16_t provenance,
	   CaloGain::CaloGain gain);


  /** @brief default destructor */
  virtual ~CaloCell ();

  /** @brief For initializing a cell we get from a @c DataPool. */
  void assign (const CaloDetDescrElement* caloDDE,
               const Identifier & ID,
               float energy, 
               float time, 
               uint16_t quality,
               uint16_t provenance,
               CaloGain::CaloGain gain);

  /** @brief get energy (data member) */
  virtual double energy () const final;
  /** @brief get phi (through CaloDetDescrElement) */
  virtual double phi    () const final;
  /** @brief get eta (through CaloDetDescrElement) */
  virtual double eta    () const final;
  /** @brief get sin(theta) (through CaloDetDescrElement) */
  virtual double sinTh  () const final;
  /** @brief get cos(theta) (derived from sinTh) */
  virtual double cosTh  () const final;
  /** @brief get sin(phi)  */
  virtual double sinPhi  () const final;
  /** @brief get cos(phi)  */
  virtual double cosPhi  () const final;
  /** @brief get cottan(theta)  */
  virtual double cotTh  () const final;
  /** @brief get mass (hardcoded to be null) */
  virtual double m      () const final;
  /** @brief get energy (data member) (synonym to method energy() */
  virtual double e      () const final;
  /** @brief get et  */
  virtual double et     () const final;
  /** @brief get x (through CaloDetDescrElement) */
  virtual float x      () const final;
  /** @brief get y (through CaloDetDescrElement) */
  virtual float y      () const final;
  /** @brief get z (through CaloDetDescrElement) */
  virtual float z      () const final;

  /** @brief get ID (from cached data member) 
   *  non-virtual and inline for fast access */
  Identifier    ID     () const ;

  // TODO: These can probably be made non-virtual.
  /** @brief get time (data member) */
  virtual float        time   () const final;
  /** @brief get quality (data member) */
  virtual uint16_t     quality() const  final;
  /** @brief get provenance (data member) */
  virtual uint16_t     provenance() const  final;
  /** @brief get gain (data member ) */
  virtual CaloGain::CaloGain  gain()    const  final;

  /** @brief check is cell is dead */
  virtual bool badcell() const;

  /** @brief get pointer to CaloDetDescrElement (data member) */
  const CaloDetDescrElement* caloDDE() const; 

  /** @brief set pointer to CaloDetDescrElement */
  virtual void setCaloDDE (const CaloDetDescrElement* caloDDE);
  /** @brief set energy */
  virtual void setEnergy (float energy);
  /** @brief add energy */
  virtual void addEnergy (float energy);
  /** @brief scale energy */
  virtual void scaleEnergy (float scale);
  /** @brief set time */
  virtual void setTime (float time);
  /** @brief set quality */
  virtual void setQuality (uint16_t quality);

 /** @brief set Quality (obsolete) */
  virtual void setQuality (double quality);

 /** @brief set Provenance */
  virtual void setProvenance (uint16_t prov);

  /** @brief set gain */
  virtual void setGain(CaloGain::CaloGain gain=CaloGain::INVALIDGAIN); 

  /** set 4Momentum (will throw exception since cannot be implemented) */
  virtual void set4Mom (const I4Momentum * const theI4Mom ) ;

  /** set 4Momentum (will throw exception since cannot be implemented) */
  virtual void set4Mom (const I4Momentum & theI4Mom);

  /** set 4Momentum (will throw exception since cannot be implemented) */
  virtual void set4Mom (const CLHEP::HepLorentzVector & theHlv ) ;


  /** @brief clone a CaloCell */ 
  virtual std::unique_ptr<CaloCell> clone() const;

  /** @brief Fast method to change the identity of a cell. */
  void set (const CaloDetDescrElement* caloDDE,
            const Identifier& ID);

  /** @brief Fast method to change the data of a cell. */
  void set (float energy,
            float time,
            uint16_t quality,
	    uint16_t provenance,
            CaloGain::CaloGain gain);
 

 /** @brief Fast method to change the data of a cell. OBSOLETE*/
  void set (float energy,
            float time,
            double quality,
            CaloGain::CaloGain gain);
  

protected:

  /** @brief energy (in MeV) */
  float      m_energy  ;
  /** @brief time */
  float      m_time    ;

  // Reflex doesn't handle unions completely correctly.
  // If you have a union as a data member, it will print a warning
  // at runtime.  So, hide the union from reflex.  (Only m_quality is used
  // in inlined code.)
  /** @brief quality */
#ifdef __REFLEX__
    int  m_quality ; 
#   define m_qualProv ((uint16_t*)&m_quality)
#   define m_tileQual ((uint8_t*)&m_quality)
#else
  union {
    int  m_quality ; 
    uint16_t m_qualProv[2];
    uint8_t m_tileQual[4];
  };
#endif
  /** @brief offline identifier */
  Identifier  m_ID      ;
  /** @brief gain */
  CaloGain::CaloGain m_gain ;
  /** @brief pointer to static CaloDetDescrElement to access information that does 
   *  not change from event to event */  
  const CaloDetDescrElement* m_caloDDE  ;

};


// inlines

inline CaloCell::CaloCell()
    : 
    m_energy(0),
    m_time(0),	
    m_quality(0),
    m_gain(CaloGain::UNKNOWNGAIN),
    m_caloDDE(0)
{}


#ifndef __REFLEX__
inline
void CaloCell::assign(const CaloDetDescrElement* caloDDE,
                        const Identifier & ID,
                        float   energy, 
                        float   time, 
                        uint16_t   quality,
                        uint16_t   provenance,
                        CaloGain::CaloGain gain)
{
  m_energy = energy;
  m_time = time;
  m_ID = ID;
  m_gain = gain;
  m_caloDDE = caloDDE;
  m_qualProv[0]=quality;
  m_qualProv[1]=provenance;
}
#endif



inline Identifier CaloCell::ID() const {
  return m_ID; 
}


/**
 * @brief A functional to get the ID from a CaloCell pointer.
 *
 * This can be used as the hash function to @c unordered_map and friends.
 */
struct CaloCellIDFcn {
  std::size_t operator() (const CaloCell* p) const
  {
    return p->ID().get_identifier32().get_compact();
  }
};


// Make this the default hash function for NavigationToken.
inline
std::size_t navigationHash (const CaloCell* p)
{
  return p->ID().get_identifier32().get_compact();
}

inline
const CaloDetDescrElement* CaloCell::caloDDE() const{ 
   return m_caloDDE;
}


inline
double CaloCell::energy() const
{                                          
  return m_energy;                         
}                                          

inline
double CaloCell::e() const
{
  return m_energy;
}


inline
double CaloCell::m() const
{ 
  // zero mass by convention
  return 0.;
}


inline
uint16_t CaloCell::quality() const 
{
  return m_qualProv[0];
}

inline
uint16_t CaloCell::provenance() const 
{
  // cppcheck-suppress objectIndex
  return m_qualProv[1];
}

inline
CaloGain::CaloGain CaloCell::gain() const 
{
  return m_gain;
}


inline
float CaloCell::time() const 
{
  return m_time;
}


inline
double CaloCell::phi() const
{ 
  return m_caloDDE->phi();
}


inline
double CaloCell::eta() const
{ 
  return m_caloDDE->eta();
}


inline
double CaloCell::sinTh() const
{ 
  return m_caloDDE->sinTh();
}


inline
double CaloCell::cosTh() const
{ 
  return m_caloDDE->cosTh();
}


inline
double CaloCell::cotTh() const
{ 
  return m_caloDDE->cotTh();
}


inline
double CaloCell::sinPhi() const
{ 
  return m_caloDDE->sinPhi();
}


inline
double CaloCell::cosPhi() const
{ 
  return m_caloDDE->cosPhi();
}

inline double
CaloCell::et() const
{
  return this->e() * this->sinTh();
}

inline
float CaloCell::x() const
{ 
  return m_caloDDE->x();
}


inline
float CaloCell::y() const
{ 
  return m_caloDDE->y();
}


inline
float CaloCell::z() const
{ 
  return m_caloDDE->z();
}


#endif

