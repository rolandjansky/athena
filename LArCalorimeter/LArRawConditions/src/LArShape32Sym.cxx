/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArShape32Sym.h"
#include "LArRawConditions/LArShape32MC.h"
#include "LArRawConditions/LArMCSym.h"

LArShape32Sym::LArShape32Sym(const LArMCSym* mcsym,  const LArShape32MC* shapeComplete):
  m_mcSym(mcsym),
  m_shapeComplete(static_cast<const LArConditionsContainer<LArShapeP1>*>(shapeComplete))
{}
    
LArShape32Sym::~LArShape32Sym() {}


ILArShape::ShapeRef_t LArShape32Sym::Shape(const HWIdentifier&  hwid, int gain, 
				int /*tbin*/, int /*mode*/ ) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  const LArShapeP1& t = m_shapeComplete->get(symhwid,gain);
  return t.m_vShape;
}


ILArShape::ShapeRef_t LArShape32Sym::ShapeDer(const HWIdentifier&  hwid, int gain, 
				int /*tbin*/, int /*mode*/ ) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  const LArShapeP1& t = m_shapeComplete->get(symhwid,gain);
  return t.m_vShapeDer;
}



ILArShape::ShapeRef_t LArShape32Sym::Shape(const Identifier&  id, int gain, 
				int /*tbin*/, int /*mode*/ ) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOfl(id);
  const LArShapeP1& t = m_shapeComplete->get(symhwid,gain);
  return t.m_vShape;
}


ILArShape::ShapeRef_t LArShape32Sym::ShapeDer(const Identifier& id, int gain, 
				int /*tbin*/, int /*mode*/ ) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOfl(id);
  const LArShapeP1& t = m_shapeComplete->get(symhwid,gain);
  return t.m_vShapeDer;
}
