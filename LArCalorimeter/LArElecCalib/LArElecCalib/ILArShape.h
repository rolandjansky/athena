/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARSHAPE_H
#define LARCONDITIONS_ILARSHAPE_H

#include "CLIDSvc/CLASS_DEF.h" 
#include "LArElecCalib/LArVectorProxy.h"
class Identifier;
class HWIdentifier;


class ILArShape {
  /** This class defines the interface for accessing Shape (Nsample variable, Dt = 25 ns fixed)
   * @stereotype Interface
   * @author S. Laplace
   * @version  0-0-1 , 29/01/2004
   *
   * History:
   *  - 08/02/2004, S. Laplace: new online ID
   *
   */
  
 public: 

  typedef LArVectorProxy ShapeRef_t;

  virtual ~ILArShape() {};

  // online ID
  virtual ShapeRef_t Shape   (const HWIdentifier& id,
                              int gain,
                              int tbin = 0,
                              int mode = 0 )  const = 0 ;
  virtual ShapeRef_t ShapeDer(const HWIdentifier& id,
                              int gain,
                              int tbin = 0,
                              int mode = 0 )  const = 0 ;
  
  // offline ID
  virtual ShapeRef_t Shape   (const Identifier& id,
                              int gain,
                              int tbin = 0,
                              int mode = 0 )  const = 0 ;  
  virtual ShapeRef_t ShapeDer(const Identifier& id,
                              int gain,
                              int tbin = 0,
                              int mode = 0 )  const = 0 ;
  
  
} ;

CLASS_DEF( ILArShape,245731716,1) 

//ConditionsContainer clid for athenaMT"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<ILArShape> , 80375854 , 1 )

#endif 
