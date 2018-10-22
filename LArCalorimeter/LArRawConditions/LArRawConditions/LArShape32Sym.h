/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARSHAPE32SYM_H
#define LARRAWCONDITIONS_LARSHAPE32SYM_H

#include "LArElecCalib/ILArShape.h"
#include "Identifier/Identifier.h"

class LArMCSym;
class LArShape32MC;
class LArShapeP1;
template<typename LArShapeP1> class LArConditionsContainer;

class LArShape32Sym: public ILArShape {
  
 public:
  typedef ILArShape::ShapeRef_t ShapeRef_t;
  LArShape32Sym() = delete;
  LArShape32Sym(const LArMCSym* mcsym,  const LArShape32MC* shapeComplete);
  virtual ~LArShape32Sym( );

   // retrieving Shape using online ID
  virtual ShapeRef_t Shape   (const HWIdentifier&  CellID, int gain, int tbin = 0, int mode = 0 ) const;
  virtual ShapeRef_t ShapeDer(const HWIdentifier&  CellID, int gain, int tbin = 0, int mode = 0) const;
  
  // retrieving Shape using offline ID
  virtual ShapeRef_t Shape   (const Identifier&  CellID, int gain, int tbin = 0, int mode = 0) const;
  virtual ShapeRef_t ShapeDer(const Identifier&  CellID, int gain, int tbin = 0, int mode = 0) const;

 private: 
  const LArMCSym* m_mcSym;
  const LArConditionsContainer<LArShapeP1>* m_shapeComplete;
};

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArShape32Sym,237619693,1)
CONDCONT_DEF( LArShape32Sym,110670317 , ILArShape );
#endif 
