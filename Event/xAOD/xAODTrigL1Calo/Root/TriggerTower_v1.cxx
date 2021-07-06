/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTower_v1.cxx 638578 2015-01-09 17:00:15Z morrisj $

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
    setEmADC( digits );
    setEmLUT( et );
    setEmBCIDvec( bcid );
    setEmBCIDext( digits_bcid );
    setEmError( error );
    setEmPeak( peak );
    setEmADCPeak( adc_peak );
  }
  
  /// Fill HAD towers with digits, calibrated ET vectors and status flags
  void TriggerTower_v1::addHad(const std::vector<int>& digits, const std::vector<int>& et, 
                               const std::vector<int>& digits_bcid, const std::vector<int>& bcid, 
                               int error, int peak, int adc_peak)
  {
    setHadADC( digits );
    setHadLUT( et );
    setHadBCIDvec( bcid );
    setHadBCIDext( digits_bcid );
    setHadError( error );
    setHadPeak( peak );
    setHadADCPeak( adc_peak );    
  }
  
  /// Specify the location of tower. Internally calls setEta();setPhi();
  void TriggerTower_v1::setCoord(double eta,double phi)
  {
    setEta( eta );
    setPhi( phi );
  }
      
  /// Final EM energy value used by the algorithms
  int TriggerTower_v1::emEnergy() const
  {
    return emLUT()[ emPeak() ];
  }
  
  /// Final HAD energy value used by the algorithms
  int TriggerTower_v1::hadEnergy() const
  {
    return hadLUT()[ hadPeak() ];
  }
  
  /// get EM BCID at peak
  int TriggerTower_v1::emBCID() const
  {
    // Fix for misfilled MC TriggerTowers
    if( ( emBCIDvec().size() != emLUT().size() ) &&
        ( emBCIDvec().size() == emADC().size() ) ){      
            return emBCIDvec()[ emADCPeak() ];
    }
    return emBCIDvec()[ emPeak() ];
  }
  
  /// get HAD BCID at peak
  int TriggerTower_v1::hadBCID() const
  {
    // Fix for misfilled MC TriggerTowers
    if( ( hadBCIDvec().size() != hadLUT().size() ) &&
        ( hadBCIDvec().size() == hadADC().size() ) ){      
            return hadBCIDvec()[ hadADCPeak() ];
    }
    return hadBCIDvec()[ hadPeak() ];    
  }
  
  /// Is EM saturated?
  bool TriggerTower_v1::isEMSaturated() const
  {
    // return true if the TriggerTower is saturated
    // i.e. it has an energy greater than the saturation threshold
    // otherwise, return false;
    
    return emEnergy() >= m_saturation;
  }
  
  /// Is HAD saturated?
  bool TriggerTower_v1::isHadSaturated() const
  {
    // return true if the TriggerTower is saturated
    // i.e. it has an energy greater than the saturation threshold
    // otherwise, return false;
    
    return hadEnergy() >= m_saturation;    
  }

} // namespace xAOD
