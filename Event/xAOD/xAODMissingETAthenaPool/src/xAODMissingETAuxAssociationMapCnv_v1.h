// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETAuxAssociationMapCnv_v1.h 692581 2015-09-02 09:27:19Z khoo $
#ifndef XAODMISSINGETATHENAPOOL_XAODMISSINGETAUXASSOCIATIONMAPCNV_V1_H
#define XAODMISSINGETATHENAPOOL_XAODMISSINGETAUXASSOCIATIONMAPCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODMissingET/versions/MissingETAuxAssociationMap_v1.h"
#include "xAODMissingET/MissingETAuxAssociationMap.h"

/// Converter class used for reading xAOD::MissingETAuxAssociationMap_v1
///
/// This converter implements the conversion from xAOD::MissingETAuxAssociationMap_v1
/// to the latest version of the class. In a way that makes this converter
/// usable both from the POOL converter, and the BS converter.
/// Much borrowed from xAODEgammaAthenaPool
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
/// @author Teng Jian Khoo <Teng.Jian.Khoo@cern.ch>
///
class xAODMissingETAuxAssociationMapCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::MissingETAuxAssociationMap,
                                 xAOD::MissingETAuxAssociationMap_v1 > {

public:
   /// Default constructor
   xAODMissingETAuxAssociationMapCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::MissingETAuxAssociationMap_v1* oldObj,
                             xAOD::MissingETAuxAssociationMap* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::MissingETAuxAssociationMap*,
                             xAOD::MissingETAuxAssociationMap_v1*,
                             MsgStream& log );

}; // class xAODMissingETAuxAssociationMapCnv_v1

#endif // XAODMISSINGETATHENAPOOL_XAODMISSINGETAUXASSOCIATIONMAPCNV_V1_H
