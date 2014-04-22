/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MVFVertexContainerCnv.h
// author: Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef MVFVXCONTAINER_CNV_H
#define MVFVXCONTAINER_CNV_H

// Hack so we can access the private data. EJWM
#define protected public
#include "GaudiKernel/MsgStream.h"
#undef protected

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
  
  MVFVxContainerCnv( ISvcLocator *svcloc );

  ~MVFVxContainerCnv();
  
  virtual StatusCode initialize();
  
  virtual MVFVxContainer_PERS *createPersistent( MVFVxContainer *transCont);

  virtual MVFVxContainer      *createTransient();

  virtual AthenaPoolTopLevelTPCnvBase*	getTopLevelTPCnv() { return &m_TPConverter; }

  virtual AthenaPoolCnvTPExtension*  clone() { return new MVFVxContainerCnv(0); }

  virtual void          readObjectFromPool( const std::string& token);

 private:
  // void    updateLog(); //!< This method modifies m_log to indicate the current key being converted
  
  // IMessageSvc          *m_msgSvc;
  // MsgStream             m_log;
  
  MVFVxContainerCnv_tlp1	m_TPConverter;

};//end of class definitions

#endif // MVFVXCONTAINER_CNV_H
