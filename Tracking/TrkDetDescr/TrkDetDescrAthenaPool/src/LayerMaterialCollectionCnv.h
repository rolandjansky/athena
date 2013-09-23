/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialCollection.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_LAYERMATERIALCOLLCETION_CNV_H
#define TRK_LAYERMATERIALCOLLECTION_CNV_H

// Hack so we can access the private data. EJWM
#define protected public
#include "GaudiKernel/MsgStream.h"
#undef protected

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "AthenaPoolCnvSvc/AthenaPoolCnvTPExtension.h"

#include "TrkGeometry/LayerMaterialCollection.h"
#include "TrkDetDescrTPCnv/LayerMaterialCollectionCnv_tlp1.h"

//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef Trk::LayerMaterialCollection_tlp1 LayerMaterialCollection_PERS;

typedef T_AthenaPoolCustomCnv<Trk::LayerMaterialCollection, LayerMaterialCollection_PERS> LayerMaterialCollectionCnvBase;

//-----------------------------------------------------------------------------
// Converter for TrackCollection object
//-----------------------------------------------------------------------------
class LayerMaterialCollectionCnv
   : public LayerMaterialCollectionCnvBase,
     public AthenaPoolCnvTPExtension
{
friend class CnvFactory<LayerMaterialCollectionCnv>;

protected:
  LayerMaterialCollectionCnv( ISvcLocator *svcloc );
  ~LayerMaterialCollectionCnv();
  virtual StatusCode initialize();

  virtual LayerMaterialCollection_PERS *createPersistent( Trk::LayerMaterialCollection *transCont);
  virtual Trk::LayerMaterialCollection *createTransient();

  virtual AthenaPoolTopLevelTPCnvBase* getTopLevelTPCnv() { return &m_TPConverter_tlp1; }

private:
  void    updateLog(); //!< This method modifies m_log to indicate the current key being converted

  IMessageSvc                     *m_msgSvc;
  MsgStream                       m_log;

  LayerMaterialCollectionCnv_tlp1 m_TPConverter_tlp1;
};

#endif // TRK_LAYERMATERIALCOLLECTION_CNV_H
