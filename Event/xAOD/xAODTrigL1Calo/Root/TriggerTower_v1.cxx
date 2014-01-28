/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTower_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/TriggerTower_v1.h"

namespace xAOD{  

  TriggerTower_v1::TriggerTower_v1() :
    SG::AuxElement()
  {        
  }
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v1, std::vector<int>, emLUT , setEmLUT )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v1, std::vector<int>, hadLUT , setHadLUT )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v1, std::vector<int>, emADC , setEmADC )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v1, std::vector<int>, hadADC , setHadADC )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v1, std::vector<int>, emBCIDvec , setEmBCIDvec )                                     
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v1, std::vector<int>, hadBCIDvec , setHadBCIDvec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v1, std::vector<int>, emBCIDext , setEmBCIDext )                                     
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TriggerTower_v1, std::vector<int>, hadBCIDext , setHadBCIDext )                                                                                                     
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v1, float, phi, setPhi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v1, float, eta, setEta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v1, unsigned int, key , setKey )   
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v1, int, emError , setEmError ) 
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v1, int, hadError , setHadError )   
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v1, int, emPeak , setEmPeak ) 
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v1, int, emADCPeak , setEmADCPeak )    
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v1, int, hadPeak , setHadPeak ) 
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TriggerTower_v1, int, hadADCPeak , setHadADCPeak )    
    
  /// Fill EM towers with digits, calibrated ET vectors and status flags
  void TriggerTower_v1::addEM(const std::vector<int>& digits, const std::vector<int>& et, 
                              const std::vector<int>& digits_bcid, const std::vector<int>& bcid, 
                              int error, int peak, int adc_peak)
  {
    this->setEmADC( digits );
    this->setEmLUT( et );
    this->setEmBCIDvec( bcid );
    this->setEmBCIDext( digits_bcid );
    this->setEmError( error );
    this->setEmPeak( peak );
    this->setEmADCPeak( adc_peak );
  }
  
  /// Fill HAD towers with digits, calibrated ET vectors and status flags
  void TriggerTower_v1::addHad(const std::vector<int>& digits, const std::vector<int>& et, 
                               const std::vector<int>& digits_bcid, const std::vector<int>& bcid, 
                               int error, int peak, int adc_peak)
  {
    this->setHadADC( digits );
    this->setHadLUT( et );
    this->setHadBCIDvec( bcid );
    this->setHadBCIDext( digits_bcid );
    this->setHadError( error );
    this->setHadPeak( peak );
    this->setHadADCPeak( adc_peak );    
  }
  
  /// Specify the location of tower. Internally calls setEta();setPhi();
  void TriggerTower_v1::setCoord(double eta,double phi)
  {
    this->setEta( eta );
    this->setPhi( phi );
  }
      
  /// Final EM energy value used by the algorithms
  int TriggerTower_v1::emEnergy() const
  {
    return this->emLUT()[ this->emPeak() ];
  }
  
  /// Final HAD energy value used by the algorithms
  int TriggerTower_v1::hadEnergy() const
  {
    return this->hadLUT()[ this->hadPeak() ];
  }
  
  /// get EM BCID at peak
  int TriggerTower_v1::emBCID() const
  {
    // Fix for misfilled MC TriggerTowers
    if( ( this->emBCIDvec().size() != this->emLUT().size() ) &&
        ( this->emBCIDvec().size() == this->emADC().size() ) ){      
            return this->emBCIDvec()[ this->emADCPeak() ];
    }
    return this->emBCIDvec()[ this->emPeak() ];
  }
  
  /// get HAD BCID at peak
  int TriggerTower_v1::hadBCID() const
  {
    // Fix for misfilled MC TriggerTowers
    if( ( this->hadBCIDvec().size() != this->hadLUT().size() ) &&
        ( this->hadBCIDvec().size() == this->hadADC().size() ) ){      
            return this->hadBCIDvec()[ this->hadADCPeak() ];
    }
    return this->hadBCIDvec()[ this->hadPeak() ];    
  }
  
  /// Is EM saturated?
  bool TriggerTower_v1::isEMSaturated() const
  {
    // return true if the TriggerTower is saturated
    // i.e. it has an energy greater than the saturation threshold
    // otherwise, return false;
    
    if( this->emEnergy() >= m_saturation ){
      return true;
    }
    return false;
  }
  
  /// Is HAD saturated?
  bool TriggerTower_v1::isHadSaturated() const
  {
    // return true if the TriggerTower is saturated
    // i.e. it has an energy greater than the saturation threshold
    // otherwise, return false;
    
    if( this->hadEnergy() >= m_saturation ){
      return true;
    }
    return false;    
  }

} // namespace xAOD
