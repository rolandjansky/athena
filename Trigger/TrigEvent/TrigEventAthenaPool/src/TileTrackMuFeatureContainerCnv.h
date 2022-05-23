/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @class  : TileTrackMuFeatureContainerCnv
 *
 * @brief transient persistent converter for TrigMuonContainer
 *
 * @author Heuijin Lim (hjlim@hep.anl.gov)
 * @Created: Jul. 7. 2008
 *
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TILETRACKMUFEATURECONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TILETRACKMUFEATURECONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainerCnv_p3.h"

//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef TileTrackMuFeatureContainer_p3   TileTrackMuFeatureContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TileTrackMuFeatureContainer, TileTrackMuFeatureContainer_PERS> TileTrackMuFeatureContainerCnvBase;
 
//-----------------------------------------------------------------------------
// Converter for TileTrackMuFeatureCollection object
//-----------------------------------------------------------------------------
class TileTrackMuFeatureContainerCnv : public TileTrackMuFeatureContainerCnvBase {
  
  friend class CnvFactory<TileTrackMuFeatureContainerCnv>;
  
 protected:
  
public:
  TileTrackMuFeatureContainerCnv( ISvcLocator *svcloc ): TileTrackMuFeatureContainerCnvBase(svcloc){}
protected:
  
  virtual TileTrackMuFeatureContainer_PERS *createPersistent( TileTrackMuFeatureContainer *transObj);
  virtual TileTrackMuFeatureContainer      *createTransient();

private:
  TileTrackMuFeatureContainerCnv_tlp1   m_converter1;
  TileTrackMuFeatureContainerCnv_p3     m_converter;

};//end of class definitions


#endif //TILETRACKMUFEATURECONTAINER_CNV_H
