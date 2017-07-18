/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*- c++ -*-

// $Id: xAODJetTrigAuxContainerCnv_v1.h 797270 2017-02-15 00:03:55Z khoo $
#ifndef XAODJETATHENAPOOL_XAODJETTRIGAUXCONTAINERCNV_V1_H
#define XAODJETATHENAPOOL_XAODJETTRIGAUXCONTAINERCNV_V1_H
#ifndef SIMULATIONBASE
// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODJet/versions/JetTrigAuxContainer_v1.h"
#include "xAODJet/JetTrigAuxContainer.h"

/// Converter class used for reading xAOD::JetTrigAuxContainer_v1
///
/// This converter implements the conversion from xAOD::JetTrigAuxContainer_v1
/// to the latest version of the class. In a way that makes this converter
/// usable both from the POOL converter, and the BS converter.
/// Much borrowed from xAODEgammaAthenaPool
///
/// @author Teng Jian Khoo <Teng.Jian.Khoo@cern.ch>
///
class xAODJetTrigAuxContainerCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::JetTrigAuxContainer,
                                 xAOD::JetTrigAuxContainer_v1 > {

public:
   /// Default constructor
   xAODJetTrigAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::JetTrigAuxContainer_v1* oldObj,
                             xAOD::JetTrigAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::JetTrigAuxContainer*,
                             xAOD::JetTrigAuxContainer_v1*,
                             MsgStream& log );

}; // class xAODJetTrigAuxContainerCnv_v1
#endif // ifndef SIMULATIONBASE
#endif // XAODJETATHENAPOOL_XAODJETTRIGAUXCONTAINERCNV_V1_H
