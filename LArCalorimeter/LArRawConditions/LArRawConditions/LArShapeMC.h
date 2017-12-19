/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARSHAPEMC_H
#define LARRAWCONDITIONS_LARSHAPEMC_H

#include "LArElecCalib/ILArShape.h" 
#include "LArRawConditions/LArConditionsContainer.h"
#include "LArRawConditions/LArShapeP1.h"

#include "GaudiKernel/ToolHandle.h"

#include <vector>

class LArCablingService;
class ILArMCSymTool;

/** Implementation of the interface ILArShape for MC
 *  It does not derive from LArShapeComplete anymore,
 *  but still from the old LArShape implementation,
 * , and implements the phi-z symmetry
 *
 * @author S. Laplace
 * @version  0-0-2 , 18/06/2008
 */

  class LArShapeMC: public ILArShape, 
		    public LArConditionsContainer<LArShapeP1>
{
 public: 

  typedef ILArShape::ShapeRef_t ShapeRef_t;
  typedef LArConditionsContainer<LArShapeP1> CONTAINER;

  LArShapeMC();
  
  virtual ~LArShapeMC( );
  virtual StatusCode initialize( );
  
  // retrieving Shape using online ID
  virtual  ShapeRef_t Shape   (const HWIdentifier&  CellID,
                               int gain,
                               int tbin,
                               int mode) const ;
  virtual  ShapeRef_t ShapeDer(const HWIdentifier&  CellID,
                               int gain,
                               int tbin,
                               int mode) const ;
  
  // retrieving Shape using offline ID
  virtual  ShapeRef_t Shape   (const Identifier&  CellID,
                               int gain,
                               int tbin,
                               int mode) const;
  virtual  ShapeRef_t ShapeDer(const Identifier&  CellID,
                               int gain,
                               int tbin,
                               int mode) const;
    
 private: 
  
  // helper for MC z-phi symmetry 
  ToolHandle<ILArMCSymTool> m_larmcsym;
};


#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArShapeMC,119125774,1)
CONDCONT_DEF( LArShapeMC, 106594226, ILArShape );
#endif 
