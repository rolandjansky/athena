/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_RPCSLTRIGGERHIT_H
#define MUONRDO_RPCSLTRIGGERHIT_H

#include<stdint.h>

#include "CLIDSvc/CLASS_DEF.h"

//
// S. Rosati Jun 2006
//

class RpcSLTriggerHit  {
  
 public:
  
  // Default constructor
  RpcSLTriggerHit() :
    m_rowinBcid(0),
    m_padid(0),
    m_ptid(0),
    m_roi(0),
    m_outerPlane(0),
    m_overlapPhi(0),
    m_overlapEta(0),
    m_triggerBcid(0), 
    m_isInput(true) { };
  
  // Constructor with arguments
  RpcSLTriggerHit(const uint16_t rowinBcid, const uint16_t padid, const uint16_t ptid, const uint16_t roi,
		  const uint16_t outerPlane, const uint16_t overlapPhi, const uint16_t overlapEta,
		  const uint16_t triggerBcid) :
    m_rowinBcid(rowinBcid),
    m_padid(padid),
    m_ptid(ptid),
    m_roi(roi), 
    m_outerPlane(outerPlane),
    m_overlapPhi(overlapPhi),
    m_overlapEta(overlapEta),
    m_triggerBcid(triggerBcid),
    m_isInput(true) { };


  // Get methods
  uint16_t rowinBcid()   const  {return m_rowinBcid;}
  uint16_t padId()  const  {return m_padid;}
  uint16_t ptId()   const  {return m_ptid;}
  uint16_t roi()  const  {return m_roi;}

  uint16_t outerPlane()  const {return m_outerPlane;}
  uint16_t overlapPhi()  const {return m_overlapPhi;}
  uint16_t overlapEta()  const {return m_overlapEta;}

  uint16_t triggerBcid() const {return m_triggerBcid;}
  bool     isInput()     const {return m_isInput;}

  // set methods
  void setIsInput(bool isInput) {m_isInput=isInput;}

 private:
  
  // bc within within the readout window
  uint16_t m_rowinBcid;
  uint16_t m_padid;
  uint16_t m_ptid;
  uint16_t m_roi;

  // Info variables split
  uint16_t m_outerPlane;
  uint16_t m_overlapPhi;
  uint16_t m_overlapEta;

  // bc of trigger hit
  uint16_t m_triggerBcid;

  // is input or output
  bool m_isInput;

};

CLASS_DEF( RpcSLTriggerHit , 115269504 , 1 )

#endif




