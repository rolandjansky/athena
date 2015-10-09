/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARPEDESTALMC_H
#define LARRAWCONDITIONS_LARPEDESTALMC_H

#include "LArElecCalib/ILArPedestal.h" 

#include <vector>

class LArCablingService ;

/** Implementation of the interface ILArfSampl for MC: only one constant is needed for MC
 *
 * @author S. Laplace
 * @version  0-0-1 , 02/02/2004
 *
 * History:
 *  - 08/02/2004, S. Laplace: new online ID
 */

class LArPedestalMC: public ILArPedestal {
  
 public: 
  
  LArPedestalMC();
  
  virtual ~LArPedestalMC( );
  
  // retrieving Pedestal using online ID
  
  float pedestal(const HWIdentifier&  CellID, int gain) const ;
  
  float pedestalRMS(const HWIdentifier&  CellID, int gain) const ;
  
  float pedestal(const Identifier&  CellID, int gain) const;
  
  float pedestalRMS(const Identifier&  CellID, int gain) const;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const std::vector<float>& vPedestal,
           const std::vector<float>& vPedestalRMS);


 private: 
  friend class LArPedestalMCCnv_p1;

  // data member simpler than for data: 1 single number is needed for MC  
  std::vector<float> m_vPedestal, m_vPedestalRMS;
};

CLASS_DEF( LArPedestalMC, 27770770,1)
#endif 
