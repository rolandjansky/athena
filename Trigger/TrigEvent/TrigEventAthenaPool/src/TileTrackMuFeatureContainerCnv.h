/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TileTrackMuFeatureContainerCnv
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
  
  TileTrackMuFeatureContainerCnv( ISvcLocator *svcloc ): TileTrackMuFeatureContainerCnvBase(svcloc){}
  
  virtual TileTrackMuFeatureContainer_PERS *createPersistent( TileTrackMuFeatureContainer *transObj);
  virtual TileTrackMuFeatureContainer      *createTransient();
  
};//end of class definitions


#endif //TILETRACKMUFEATURECONTAINER_CNV_H
