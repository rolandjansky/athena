/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DUMMYDETELEMENTSURFACECNV_P1_H
#define DUMMYDETELEMENTSURFACECNV_P1_H

class DummyDetElementSurfaceCnv_p1
  : public ITPConverterFor<Trk::Surface>
{
public:
  virtual void persToTransUntyped(const void* /*pers*/, void* /*trans*/, MsgStream& /*log*/) { abort(); }
  virtual void transToPersUntyped(const void* /*trans*/, void* /*pers*/, MsgStream& /*log*/) { abort(); }
  virtual void	reservePStorage( size_t /*size*/ ) { abort(); }
  virtual TPObjRef virt_toPersistent( const TransBase_t* /*trans*/, MsgStream& /*log*/) { abort(); }
  virtual Trk::DetElementSurface* virt_createTransFromPStore( unsigned /*index*/, MsgStream &/*log*/ ) { return 0; }
  virtual void	pstoreToTrans( unsigned /*index*/, TransBase_t* /*transObj*/, MsgStream& /*log*/) { abort(); }
  virtual const std::type_info& persistentTInfo() const { return typeid(void*); }
};

#endif/*DUMMYDETELEMENTSURFACECNV_P1_H*/
