///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** INav4MomLinkContainerCnv.h
 *  @author: Karsten Koeneke (DESY)
 *  @date July 2009
 *  @brief this defines the t/p conversion of a vector
 *         of ElementLinks to INavigable4MomentumCollection
 *****************************************************************/

#ifndef EVENTCOMMONATHENAPOOL_INAV4MOMLINKCONTAINERCNV_H
#define EVENTCOMMONATHENAPOOL_INAV4MOMLINKCONTAINERCNV_H

// STL includes
#include <vector>

// DataModel includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "DataModelAthenaPool/VectorElementLinkCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "EventCommonTPCnv/INav4MomLinkContainerCnv_p1.h"
#include "DataModelAthenaPool/VectorElementLink_p1.h"
#include "DataModelAthenaPool/ElementLink_p3.h"
#include "EventCommonTPCnv/INav4MomLinkContainer_p1.h"
#include "NavFourMom/INav4MomLinkContainer.h"

/** The latest persistent representation type of INav4MomLinkContainer */
typedef INav4MomLinkContainer_p1  INav4MomLinkContainer_PERS;



/** Definition of the athena pool converter for INav4MomLinkContainer */
class INav4MomLinkContainerCnv : public T_AthenaPoolCustomCnv< INav4MomLinkContainer,
                                                               INav4MomLinkContainer_PERS >
{

  /** make the factory for this converter our friend */
  friend class CnvFactory< INav4MomLinkContainerCnv >;

protected:

  /** The constructor */
  INav4MomLinkContainerCnv( ISvcLocator* svcloc );

  /** Member method to create the persistent version */
  virtual INav4MomLinkContainer_PERS*   createPersistent ( INav4MomLinkContainer* transCont );

  /** Member method to create the transient version */
  virtual INav4MomLinkContainer*        createTransient ();

  /** Create the t/p converter */
  INav4MomLinkContainerCnv_p1 m_TPConverter;
};


// Inline constructor
inline INav4MomLinkContainerCnv::INav4MomLinkContainerCnv( ISvcLocator* svcloc ) :
  T_AthenaPoolCustomCnv< INav4MomLinkContainer, INav4MomLinkContainer_PERS >( svcloc )
{}


#endif // EVENTCOMMONATHENAPOOL_INAV4MOMLINKCONTAINERCNV_H
