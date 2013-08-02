/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARRTERM_H
#define LARCONDITIONS_ILARRTERM_H

#include "CLIDSvc/CLASS_DEF.h" 
class Identifier;
class HWIdentifier;

class ILArRterm {
/** This class defines the interface for accessing Rterm
   * @stereotype Interface
   * @author M. Fanti
   * @version  0-0-1 , 21/07/2005
   *
   *
   */
  
 public: 

  virtual ~ILArRterm() {};

  // online ID
  virtual const float& Rterm(const HWIdentifier& id , int gain )  const = 0 ;
  
  // offline ID 
  virtual const float& Rterm(const Identifier& id , int gain )  const =0;
  
  enum {ERRORCODE = LArElecCalib::ERRORCODE};
  
} ;

CLASS_DEF( ILArRterm,127393399,1) 

#endif 
