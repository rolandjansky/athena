/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialMapCnv.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_LAYERMATERIALMAP_CNV_H
#define TRK_LAYERMATERIALMAP_CNV_H

// Hack so we can access the private data. EJWM
#define protected public
#include "GaudiKernel/MsgStream.h"
#undef protected

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrkGeometry/LayerMaterialMap.h"
#include "TrkDetDescrTPCnv/LayerMaterialMapCnv_tlp1.h"

//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef Trk::LayerMaterialMap_tlp1 LayerMaterialMap_PERS;

typedef T_AthenaPoolCustomCnv<Trk::LayerMaterialMap, LayerMaterialMap_PERS> LayerMaterialMapCnvBase;

//-----------------------------------------------------------------------------
// Converter for TrackCollection object
//-----------------------------------------------------------------------------
class LayerMaterialMapCnv : public LayerMaterialMapCnvBase
{
friend class CnvFactory<LayerMaterialMapCnv>;

protected:
  LayerMaterialMapCnv( ISvcLocator *svcloc );
  ~LayerMaterialMapCnv();
  virtual StatusCode initialize();

  virtual LayerMaterialMap_PERS *createPersistent( Trk::LayerMaterialMap *transCont);
  virtual Trk::LayerMaterialMap *createTransient();

  virtual AthenaPoolTopLevelTPCnvBase* getTopLevelTPCnv() { return &m_TPConverter_tlp1; }

private:
  void    updateLog(); //!< This method modifies m_log to indicate the current key being converted

  IMessageSvc                     *m_msgSvc;
  MsgStream                       m_log;

  LayerMaterialMapCnv_tlp1        m_TPConverter_tlp1;
};

#endif // TRK_LAYERMATERIALMAP_CNV_H
