/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   V0ContainerCnv.h
// author: Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
// March 2007
//  
//-----------------------------------------------------------------------------
#ifndef V0CONTAINER_CNV_H
#define V0CONTAINER_CNV_H


#include "GaudiKernel/MsgStream.h"

#include "TrkEventCnvTools/ITrkEventCnvTool.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrkV0Vertex/V0Container.h"
// #include "TrkEventTPCnv/V0ContainerCnv_tlp1.h"
#include "TrkEventTPCnv/V0ContainerCnv_tlp2.h"


//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
// typedef Trk::V0Container_tlp1    V0Container_PERS;
typedef Trk::V0Container_tlp2	V0Container_PERS;

typedef T_AthenaPoolCustomCnv<V0Container, V0Container_PERS> V0ContainerCnvBase;

//-----------------------------------------------------------------------------
// Converter for V0Collection object
//-----------------------------------------------------------------------------
class V0ContainerCnv : public V0ContainerCnvBase
{
 
 friend class CnvFactory<V0ContainerCnv>;

 protected:
  
public:
  V0ContainerCnv( ISvcLocator *svcloc );
protected:
  virtual StatusCode initialize() override;
  
  virtual V0Container_PERS *createPersistent( V0Container *transCont) override;
  virtual V0Container      *createTransient() override;

  AthenaPoolTopLevelTPCnvBase*	getTopLevelTPCnv() { return &m_TPConverter; }

 private:
  void    updateLog(); //!< This method modifies m_log to indicate the current key being converted

  IMessageSvc          *m_msgSvc;
  MsgStream             m_log;
  
  V0ContainerCnv_tlp2    m_TPConverter;

 };//end of class definitions
 

#endif //V0CONTAINER_CNV_H
