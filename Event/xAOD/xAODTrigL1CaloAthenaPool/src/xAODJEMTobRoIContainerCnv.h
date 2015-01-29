// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id
#ifndef XAODTRIGL1CALOATHENAPOOL_XAODJEMTOBROICONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODJEMTOBROICONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/JEMTobRoIContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::JEMTobRoIContainer , 
                               xAOD::JEMTobRoIContainer >
        xAODJEMTobRoIContainerCnvBase;

/**
  * @short POOL converter for the xAOD::JEMTobRoIContainer class
  *
  *        Simple converter class making the xAOD::JEMTobRoIContainer class known to POOL
  *
  * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  * @author John Morris <john.morris@cern.ch>
  *
  * $Revision
  * $Date 
  *
  */

class xAODJEMTobRoIContainerCnv : public xAODJEMTobRoIContainerCnvBase {

  // Declare the factory as our friend:
  friend class CnvFactory< xAODJEMTobRoIContainerCnv >;

  public:
    /// Converter constructor
    xAODJEMTobRoIContainerCnv( ISvcLocator* svcLoc );

    /// Function preparing the container to be written out
    virtual xAOD::JEMTobRoIContainer* createPersistent( xAOD::JEMTobRoIContainer* trans );
    /// Function reading in the persistent object
    virtual xAOD::JEMTobRoIContainer* createTransient();

  private:
    /// Function preparing a vertex object for persistence
    void toPersistent( xAOD::JEMTobRoI* ) const;

}; // class xAODJEMTobRoIContainerCnv
#endif

