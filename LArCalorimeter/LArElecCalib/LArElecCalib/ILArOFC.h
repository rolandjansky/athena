/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARELECCALIB_ILAROFC_H
#define LARELECCALIB_ILAROFC_H

#include "CLIDSvc/CLASS_DEF.h" 
#include "LArElecCalib/LArVectorProxy.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"

#include <vector> 

class ILArOFC {
  /** This class defines the interface for accessing Optimal Filtering coefficients for each channel
   * provide both offline and online ID access
   * @author W.Lampl, S.Laplace
   * @version  0-0-1 , 10/12/2003
   *
   * History:
   *  - 08/02/2004, S. Laplace: new online ID
   */
  
 public: 

  typedef LArVectorProxy OFCRef_t;

  virtual ~ILArOFC() {};
  
  /** access to OFCs by online ID, gain, and tbin (!=0 for testbeam)
   */ 
  virtual OFCRef_t OFC_a(const HWIdentifier& id,
                         int gain,
                         int tbin=0) const =0 ;
  
  virtual OFCRef_t OFC_b(const HWIdentifier& id,
                         int gain,
                         int tbin=0) const =0 ;
  
  
  /** access to OFCs by offline ID, gain, and tbin (!=0 for testbeam)
   */ 
  
  virtual OFCRef_t OFC_a(const Identifier& id,
                         int gain,
                         int tbin=0) const =0;
  
  virtual OFCRef_t OFC_b(const Identifier& id,
                         int gain,
                         int tbin=0) const =0;

  
  virtual float timeOffset(const Identifier&  CellID, int gain) const =0;   //Useful only in the TB case
  virtual float timeOffset(const HWIdentifier&  CellID, int gain) const =0; //Useful only in the TB case

  //For the TB / cosmic case: retrieve the number of time-bins (aka "phases")
  virtual unsigned nTimeBins(const HWIdentifier&  CellID, int gain) const =0;
  virtual unsigned nTimeBins(const Identifier&  CellID, int gain) const =0;
 
  //For the TB / cosmic case: retrieve the witdth of the time bin (default 24 bins in 25 ns)
  virtual float timeBinWidth(const HWIdentifier&  CellID, int gain) const=0;
  virtual float timeBinWidth(const Identifier&  CellID, int gain) const=0;
} ;

CLASS_DEF( ILArOFC, 204091343, 1) 


//ConditionsContainer clid for athenaMT"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<ILArOFC> , 136784899 , 1 )

#endif 
