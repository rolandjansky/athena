/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   VertexContainerCnv.h
// author: Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef VXCONTAINER_CNV_H
#define VXCONTAINER_CNV_H

// Hack so we can access the private data. EJWM
#define protected public
#include "GaudiKernel/MsgStream.h"
#undef protected

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
  
  VxContainerCnv( ISvcLocator *svcloc );
  ~VxContainerCnv();
  virtual StatusCode initialize();
  
  virtual VxContainer_PERS *createPersistent( VxContainer *transCont);
  virtual VxContainer      *createTransient();

  virtual AthenaPoolTopLevelTPCnvBase*   getTopLevelTPCnv() { return &m_TPConverter; }

 
 private:
  void    updateLog(); //!< This method modifies m_log to indicate the current key being converted
  
  IMessageSvc          *m_msgSvc;
  MsgStream             m_log;
  
  // VxContainerCnv_tlp1    m_TPConverter;
  VxContainerCnv_tlp2    m_TPConverter;

};//end of class definitions

#endif // VXCONTAINER_CNV_H
