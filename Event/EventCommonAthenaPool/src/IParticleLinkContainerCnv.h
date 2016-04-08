///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** IParticleLinkContainerCnv.h
 *  @author: Karsten Koeneke (DESY)
 *  @date July 2009
 *  @brief this defines the Athena POOL converter for a vector
 *         of ElementLinks to IParticleContainer
 *****************************************************************/

#ifndef EVENTCOMMONATHENAPOOL_IPARTICLELINKCONTAINERCNV_H
#define EVENTCOMMONATHENAPOOL_IPARTICLELINKCONTAINERCNV_H

// STL includes
#include <vector>

// DataModel includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "DataModelAthenaPool/VectorElementLinkCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "EventCommonTPCnv/IParticleLinkContainerCnv_p1.h"
#include "DataModelAthenaPool/VectorElementLink_p1.h"
#include "DataModelAthenaPool/ElementLink_p3.h"
#include "EventCommonTPCnv/IParticleLinkContainer_p1.h"
#include "NavFourMom/IParticleLinkContainer.h"

/** The latest persistent representation type of IParticleLinkContainer */
typedef IParticleLinkContainer_p1  IParticleLinkContainer_PERS;



/** Definition of the athena pool converter for IParticleLinkContainer */
class IParticleLinkContainerCnv : public T_AthenaPoolCustomCnv< IParticleLinkContainer,
                                                                IParticleLinkContainer_PERS >
{

  /** make the factory for this converter our friend */
  friend class CnvFactory< IParticleLinkContainerCnv >;

protected:

  /** The constructor */
  IParticleLinkContainerCnv( ISvcLocator* svcloc );

  /** Member method to create the persistent version */
  virtual IParticleLinkContainer_PERS*   createPersistent ( IParticleLinkContainer* transCont );

  /** Member method to create the transient version */
  virtual IParticleLinkContainer*        createTransient ();

  /** Create the t/p converter */
  IParticleLinkContainerCnv_p1 m_TPConverter;
};


// Inline constructor
inline IParticleLinkContainerCnv::IParticleLinkContainerCnv( ISvcLocator* svcloc ) :
  T_AthenaPoolCustomCnv< IParticleLinkContainer, IParticleLinkContainer_PERS >( svcloc )
{}


#endif // EVENTCOMMONATHENAPOOL_IPARTICLELINKCONTAINERCNV_H
