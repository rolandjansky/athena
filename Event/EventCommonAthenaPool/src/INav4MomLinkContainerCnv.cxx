/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** INav4MomLinkContainerCnv.cxx
 *  @author: Karsten Koeneke (DESY)
 *  @date July 2009
 *  @brief this defines the t/p conversion of a vector
 *         of ElementLinks to INavigable4MomentumCollection
 *****************************************************************/

#include "DataModelAthenaPool/VectorElementLinkCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/VectorElementLink_p1.h"
#include "DataModelAthenaPool/ElementLink_p3.h"

// This classes header
#include "INav4MomLinkContainerCnv.h"


// Create the persistent object from the transient one
INav4MomLinkContainer_PERS* INav4MomLinkContainerCnv::createPersistent( INav4MomLinkContainer* transCont )
{
  // Create the message service for this class
  MsgStream log( messageService(), "INav4MomLinkContainerConverter" );

  // Do the actual creation of the persistent object
  INav4MomLinkContainer_PERS *persObj = m_TPConverter.createPersistent( transCont, log );

  // Write a debug message out
  log << MSG::DEBUG << "Success" << endreq;

  return persObj;
}


// Create the transient object from the persistent one
INav4MomLinkContainer* INav4MomLinkContainerCnv::createTransient()
{
  // Create the message service for this class
  MsgStream log( messageService(), "INav4MomLinkContainerConverter" );

  // Define the pool IDs
  static pool::Guid  p1_guid("A7F0A4C5-F343-4724-B317-FB5A890355FA");

  if( compareClassGuid(p1_guid) )
    {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< INav4MomLinkContainer_PERS > col_vect( poolReadObject< INav4MomLinkContainer_PERS >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
    }
  else
    {
      throw std::runtime_error("Unsupported persistent version of Data Collection");
    }
}

