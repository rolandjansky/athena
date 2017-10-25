/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LAROFCCOMPLETE_H
#define LARRAWCONDITIONS_LAROFCCOMPLETE_H

#include "LArElecCalib/ILArOFC.h" 
#include "LArRawConditions/LArOFCP1.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>
#include <iostream>

class LArCablingService ;

/** This class implements the ILArOFC interface
 * Two ways of filling this data class:
 *   - intra ATHENA
 *   - from NOVA DB
 *
 * @author W. Lampl, S. Laplace
 * @version  0-0-1 , 10/12/03
 *
 * History:
 *  - 08/02/2004, S. Laplace: new online ID
 */

class LArOFCComplete: public ILArOFC,
	public LArConditionsContainer<LArOFCP1>
 {
  
 public: 
  
  typedef ILArOFC::OFCRef_t OFCRef_t;
  typedef LArOFCP1                           LArCondObj;
  typedef LArConditionsContainer<LArCondObj> CONTAINER ;

  LArOFCComplete();
  
  virtual ~LArOFCComplete( );
  
  // retrieving coefficients using online ID
  
  virtual OFCRef_t OFC_a(const HWIdentifier&  CellID,
                         int gain,
                         int tbin=0) const ;
  virtual OFCRef_t OFC_b(const HWIdentifier&  CellID,
                         int gain,
                         int tbin=0) const ;
  
  // retrieving coefficients using offline ID
  
  virtual OFCRef_t OFC_a(const Identifier&  CellID,
                         int gain,
                         int tbin=0) const;  
  virtual OFCRef_t OFC_b(const Identifier&  CellID,
                         int gain,
                         int tbin=0) const;

  // retrieving time offset using online/offline ID

  virtual  float timeOffset(const Identifier&  CellID, int gain) const;
  virtual  float timeOffset(const HWIdentifier&  CellID, int gain) const;

  //For the TB / cosmic case: retrieve the number of time-bins (aka "phases")
  virtual unsigned nTimeBins(const HWIdentifier&  CellID, int gain) const;
  virtual unsigned nTimeBins(const Identifier&  CellID, int gain) const;
 
  //For the TB / cosmic case: retrieve the witdth of the time bin (default 24 bins in 25 ns)
  virtual float timeBinWidth(const HWIdentifier&  CellID, int gain) const;
  virtual float timeBinWidth(const Identifier&  CellID, int gain) const;


  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, int gain,
	   const std::vector<std::vector<float> >& vOFC_a,
	   const std::vector<std::vector<float> >& vOFC_b,
	   float timeOffset=0, float timeBinWidth=25./24.);

  void dumpOFC(std::string output_file_name) const ;

  //Overload the const-get method of the underlying LArConditionsContainer
  //for speed improvment
  CONTAINER::ConstReference get(const HWIdentifier id, int gain) const;

 private: 
  static const std::vector<float> m_empty;

  //In a typical application of this class, the methods ofc_a,ofc_b,timeBinWidht,etc., 
  //are called consecutivly for the same cell and gain. Therefore it makes sense to 
  //cache a pointer to the LArOFCP1 object.
  //Used by tthe overloaded const-get method
  struct cache_t {
    HWIdentifier id;
    int gain;
    CONTAINER::ConstReference obj;
  };
  mutable cache_t m_cache;
 };

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArOFCComplete, 101879462, 1) 
CONDCONT_DEF( LArOFCComplete , 144694594 , ILArOFC );

inline
LArOFCComplete::CONTAINER::ConstReference
LArOFCComplete::get(const HWIdentifier id, int gain) const {  
 if (m_cache.id!=id || m_cache.gain!=gain) {
    m_cache.obj=LArConditionsContainer<LArOFCP1>::get(id,gain);
    m_cache.id=id;
    m_cache.gain=gain;
  }
  return m_cache.obj;
}
 

#endif 
