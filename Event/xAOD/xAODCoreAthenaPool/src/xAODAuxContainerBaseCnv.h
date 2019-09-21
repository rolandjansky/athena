// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODAuxContainerBaseCnv.h 660871 2015-04-16 08:20:17Z krasznaa $
#ifndef XAODCOREATHENAPOOL_XAODAUXCONTAINERBASECNV_H
#define XAODCOREATHENAPOOL_XAODAUXCONTAINERBASECNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::AuxContainerBase >
   xAODAuxContainerBaseCnvBase;

/**
 * @short POOL converter for the xAOD::AuxContainerBase class
 *
 * We need a custom converter for this class in order to get the
 * selection set copied.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 660871 $
 * $Date: 2015-04-16 10:20:17 +0200 (Thu, 16 Apr 2015) $
 */
class xAODAuxContainerBaseCnv : public xAODAuxContainerBaseCnvBase {

public:
   /// Converter constructor
   using xAODAuxContainerBaseCnvBase::xAODAuxContainerBaseCnvBase;

protected:
   /// Function preparing the container to be written out
   virtual xAOD::AuxContainerBase*
   createPersistentWithKey( xAOD::AuxContainerBase* trans,
                            const std::string& key ) override;

}; // class xAODAuxContainerBaseCnv


#endif // XAODCOREATHENAPOOL_XAODAUXCONTAINERBASECNV_H
