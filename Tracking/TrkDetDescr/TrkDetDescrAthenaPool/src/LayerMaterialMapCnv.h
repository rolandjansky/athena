/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialMapCnv.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_LAYERMATERIALMAP_CNV_H
#define TRK_LAYERMATERIALMAP_CNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/MsgStream.h"

#include "TrkGeometry/LayerMaterialMap.h"
#include "TrkDetDescrTPCnv/LayerMaterialMapCnv_tlp1.h"

//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef Trk::LayerMaterialMap_tlp1 LayerMaterialMap_PERS;

typedef T_AthenaPoolCustomCnvWithKey<Trk::LayerMaterialMap, LayerMaterialMap_PERS> LayerMaterialMapCnvBase;

//-----------------------------------------------------------------------------
// Converter for TrackCollection object
//-----------------------------------------------------------------------------
class LayerMaterialMapCnv : public LayerMaterialMapCnvBase
{
friend class CnvFactory<LayerMaterialMapCnv>;

protected:
public:
  LayerMaterialMapCnv( ISvcLocator *svcloc );
protected:
  virtual StatusCode initialize() override;

  virtual LayerMaterialMap_PERS* createPersistentWithKey ( Trk::LayerMaterialMap *transCont,
                                                           const std::string& key) override;
  virtual Trk::LayerMaterialMap* createTransientWithKey (const std::string& key) override;

  virtual AthenaPoolTopLevelTPCnvBase* getTopLevelTPCnv() { return &m_TPConverter_tlp1; }

private:
  IMessageSvc                     *m_msgSvc;
  LayerMaterialMapCnv_tlp1        m_TPConverter_tlp1;
};


#endif // TRK_LAYERMATERIALMAP_CNV_H
