///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonContainerCnv.h 
// Header file for class MuonContainerCnv
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef RECATHENAPOOL_MUONCONTAINERCNV_H 
#define RECATHENAPOOL_MUONCONTAINERCNV_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// RecTPCnv includes
#include "RecTPCnv/MuonContainer_p5.h"
#include "RecTPCnv/MuonContainer_p6.h"

// muonEvent includes
#include "muonEvent/MuonContainer.h"

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

// RecTPCnv includes
#include "RecTPCnv/MuonContainerCnv_p6.h"
#include "RecTPCnv/MuonContainerCnv_p5.h"
#include "RecTPCnv/MuonContainerCnv_p4.h"
#include "RecTPCnv/MuonContainerCnv_p3.h"
#include "RecTPCnv/MuonContainerCnv_p2.h"
#include "RecTPCnv/MuonContainerCnv_p1.h"

// the latest persistent representation type of DataCollection:
typedef MuonContainer_p6  MuonContainer_PERS;

class MuonContainerCnv: public T_AthenaPoolCustomCnv<
                                 Analysis::MuonContainer, 
                                 MuonContainer_PERS 
                               > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<MuonContainerCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  MuonContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual MuonContainer_PERS*
    createPersistent( Analysis::MuonContainer* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual Analysis::MuonContainer* createTransient();

  /** initialize() - need to access Storegate */
  virtual StatusCode initialize();

private:

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  StoreGateSvc_t m_storeGate;
  std::string m_muonCaloEnergyContainerName;

  MuonContainerCnv_p1 m_cnv_p1;
  MuonContainerCnv_p2 m_cnv_p2;
  MuonContainerCnv_p3 m_cnv_p3;
  MuonContainerCnv_p4 m_cnv_p4;
  MuonContainerCnv_p5 m_cnv_p5;
  MuonContainerCnv_p6 m_cnv_p6;

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline MuonContainerCnv::MuonContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv< Analysis::MuonContainer, 
			 MuonContainer_PERS
			 >(svcLocator),
  m_storeGate( "StoreGateSvc", "MuonContainerCnv" ),
  m_muonCaloEnergyContainerName( "MuonCaloEnergyCollection" )
{}

#endif //> RECATHENAPOOL_MUONCONTAINERCNV_H
