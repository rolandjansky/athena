/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** IParticleLinkContainerCnv.cxx
 *  @author: Karsten Koeneke (DESY)
 *  @date July 2009
 *  @brief this defines the Athena POOL converter for a vector
 *         of ElementLinks to IParticleContainer
 *****************************************************************/

#include "DataModelAthenaPool/VectorElementLinkCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/VectorElementLink_p1.h"
#include "DataModelAthenaPool/ElementLink_p3.h"

// This classes header
#include "IParticleLinkContainerCnv.h"

// This is here as a checkreq workaround.
// The generated pool files depend on EventKernel, but nothing in src does
// directly.  So checkreq was fiving different errors before and after pool
// generation.  By adding an explicit dependency here, those are made
// consistent.
#include "EventKernel/IParticle.h"


// Create the persistent object from the transient one
IParticleLinkContainer_PERS* IParticleLinkContainerCnv::createPersistent( IParticleLinkContainer* transCont )
{
  // Create the message service for this class
  MsgStream log( messageService(), "IParticleLinkContainerConverter" );

  // Do the actual creation of the persistent object
  IParticleLinkContainer_PERS *persObj = m_TPConverter.createPersistent( transCont, log );

  // Write a debug message out
  log << MSG::DEBUG << "Success" << endreq;

  return persObj;
}


// Create the transient object from the persistent one
IParticleLinkContainer* IParticleLinkContainerCnv::createTransient()
{
  // Create the message service for this class
  MsgStream log( messageService(), "IParticleLinkContainerConverter" );

  // Define the pool IDs
  static pool::Guid  p1_guid("E82C71AF-AC5C-453B-9A35-FA45A849838E");

  if( compareClassGuid(p1_guid) )
    {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< IParticleLinkContainer_PERS > col_vect( poolReadObject< IParticleLinkContainer_PERS >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
    }
  else
    {
      throw std::runtime_error("Unsupported persistent version of Data Collection");
    }
}

