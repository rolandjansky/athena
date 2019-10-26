/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MVFVertexContainerCnv.h
// author: Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef MVFVXCONTAINER_CNV_H
#define MVFVXCONTAINER_CNV_H

#include "GaudiKernel/MsgStream.h"

#include "TrkEventCnvTools/ITrkEventCnvTool.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolExtendingCnv.h"

#include "VxMultiVertex/MVFVxContainer.h"

#include "TrkEventTPCnv/MVFVxContainerCnv_tlp1.h"


//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef Trk::MVFVxContainer_tlp1  MVFVxContainer_PERS;

typedef T_AthenaPoolExtendingCnv<MVFVxContainer, MVFVxContainer_PERS> MVFVxContainerCnvBase;

class MVFVxContainerCnv : public MVFVxContainerCnvBase

{
 
 friend class CnvFactory<MVFVxContainerCnv>;

 protected:
  
public:
  MVFVxContainerCnv( ISvcLocator *svcloc );
protected:

  virtual StatusCode initialize() override;
  
  virtual MVFVxContainer_PERS *createPersistent( MVFVxContainer *transCont) override;

  virtual MVFVxContainer      *createTransient() override;

  virtual AthenaPoolTopLevelTPCnvBase*	getTopLevelTPCnv() override { return &m_TPConverter; }

  virtual AthenaPoolCnvTPExtension*  clone() override { return new MVFVxContainerCnv(0); }

  virtual void          readObjectFromPool( const std::string& token) override;

 private:
  MVFVxContainerCnv_tlp1	m_TPConverter;

};//end of class definitions

#endif // MVFVXCONTAINER_CNV_H
