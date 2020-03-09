/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   VertexContainerCnv.h
// author: Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef VXCONTAINER_CNV_H
#define VXCONTAINER_CNV_H

#include "GaudiKernel/MsgStream.h"

#include "TrkEventCnvTools/ITrkEventCnvTool.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "AthenaPoolCnvSvc/AthenaPoolCnvTPExtension.h"

#include "VxVertex/VxContainer.h"

// #include "TrkEventTPCnv/VxContainerCnv_tlp1.h"
#include "TrkEventTPCnv/VxContainerCnv_tlp2.h"


// typedef Trk::VxContainer_tlp1    VxContainer_PERS;
typedef Trk::VxContainer_tlp2    VxContainer_PERS;

typedef T_AthenaPoolCustomCnv<VxContainer, VxContainer_PERS> VxContainerCnvBase;

class VxContainerCnv : public VxContainerCnvBase, public AthenaPoolCnvTPExtension {
 
 friend class CnvFactory<VxContainerCnv>;
 
 protected:
  
public:
  VxContainerCnv( ISvcLocator *svcloc );
protected:
  virtual StatusCode initialize() override;
  
  virtual VxContainer_PERS *createPersistent( VxContainer *transCont) override;
  virtual VxContainer      *createTransient() override;

  virtual AthenaPoolTopLevelTPCnvBase*   getTopLevelTPCnv() override { return &m_TPConverter; }

 
 private:
  IMessageSvc          *m_msgSvc;
  MsgStream             m_log;
  
  // VxContainerCnv_tlp1    m_TPConverter;
  VxContainerCnv_tlp2    m_TPConverter;

};//end of class definitions

#endif // VXCONTAINER_CNV_H
