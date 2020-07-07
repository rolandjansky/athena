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

/** This class implements the ILArOFC interface
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
                         int tbin=0) const override;
  virtual OFCRef_t OFC_b(const HWIdentifier&  CellID,
                         int gain,
                         int tbin=0) const override;
  
  // retrieving time offset using online/offline ID

  virtual  float timeOffset(const HWIdentifier&  CellID, int gain) const override;

  //For the TB / cosmic case: retrieve the number of time-bins (aka "phases")
  virtual unsigned nTimeBins(const HWIdentifier&  CellID, int gain) const override;
 
  //For the TB / cosmic case: retrieve the witdth of the time bin (default 24 bins in 25 ns)
  virtual float timeBinWidth(const HWIdentifier&  CellID, int gain) const override;

  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, int gain,
	   const std::vector<std::vector<float> >& vOFC_a,
	   const std::vector<std::vector<float> >& vOFC_b,
	   float timeOffset=0, float timeBinWidth=25./24.);

  void dumpOFC(std::string output_file_name) const ;


 private: 
  static const std::vector<float> m_empty;

 };

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArOFCComplete, 101879462, 1) 
CONDCONT_DEF( LArOFCComplete , 144694594 , ILArOFC );


#endif 
