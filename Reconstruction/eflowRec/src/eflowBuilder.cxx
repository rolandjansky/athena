/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowBuilder.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R. Tovey
CREATED:  8th November, 2001

PURPOSE:  Loop over tracks and clusters to build calo objects, then 
          loop over tracks and clusters within calo objects to build
          eflow objects.

********************************************************************/

// INCLUDE HEADER FILES:

#include "eflowRec/eflowBuilder.h"

// INCLUDE GAUDI HEADER FILES:
 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ListItem.h"

#include <algorithm> 
#include <math.h>

//  END OF HEADER FILES INCLUDE

//  CONSTRUCTOR:
    
eflowBuilder::eflowBuilder(const std::string& name, 
		       ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator), 
  m_eflowPreparationAlgName("eflowPreparation/caloObjectBuilderAlg"),
  m_eflowObjectBuilderAlgName("eflowObjectBuilder/objectBuilderAlg"),
  m_eflowQuantitiesAlgName("eflowQuantities/quantAlg")
{

// The following properties can be specified at run-time
// (declared in jobOptions file)
                            
  // Names of various sub-algorithms:
  declareProperty("eflowPreparationAlgName", 
		  m_eflowPreparationAlgName);
  declareProperty("eflowObjectBuilderAlgName", m_eflowObjectBuilderAlgName);
  declareProperty("eflowQuantitiesAlgName", m_eflowQuantitiesAlgName);
  m_eflowPreparationAlg = nullptr;
  m_eflowObjectBuilderAlg = nullptr;
  m_eflowQuantitiesAlg = nullptr;
}

eflowBuilder::~eflowBuilder()
{ }

StatusCode eflowBuilder::initialize()
{

  Algorithm* pAlg;
  StatusCode sc;

  // Get pointer to StoreGateSvc and cache:

  //////////////////////////////////////////////////////////////////
  // Create the eflowPreparation Algorithm:

  std::string type, name;

  ListItem caloObjectBuilderAlg(m_eflowPreparationAlgName);
  sc = createSubAlgorithm(caloObjectBuilderAlg.type(), 
			  caloObjectBuilderAlg.name(), pAlg);

  if( sc.isFailure() ) 
  {
    ATH_MSG_DEBUG( "Unable to create " << m_eflowPreparationAlgName );
    m_eflowPreparationAlg = nullptr;
  } 
  else
  {
    m_eflowPreparationAlg = dynamic_cast<AthAlgorithm*>(pAlg);
  }
  
  //////////////////////////////////////////////////////////////////
  // Create the eflowObjectBuilder sub-algorithm:

  ListItem objectBuilderAlg(m_eflowObjectBuilderAlgName);
  sc = createSubAlgorithm(objectBuilderAlg.type(), 
			  objectBuilderAlg.name(), pAlg);
  if( sc.isFailure() ) 
  {
    ATH_MSG_DEBUG( "Unable to create " << m_eflowObjectBuilderAlgName );
    m_eflowObjectBuilderAlg = nullptr;
  } 
  else
  {
    m_eflowObjectBuilderAlg = dynamic_cast<AthAlgorithm*>(pAlg);
  }

  //////////////////////////////////////////////////////////////////
  // Create the eflowQuantities sub-algorithm:

  ListItem quantAlg(m_eflowQuantitiesAlgName);
  sc = createSubAlgorithm(quantAlg.type(), quantAlg.name(), pAlg);
  if( sc.isFailure() ) 
  {
    ATH_MSG_DEBUG( "Unable to create " << m_eflowQuantitiesAlgName );
    m_eflowQuantitiesAlg = nullptr;
  } 
  else
  {
    m_eflowQuantitiesAlg = dynamic_cast<AthAlgorithm*>(pAlg);
  }

/////////////////////////////////////////////////////////////////

  ATH_MSG_VERBOSE( "Using the Algorithms:"  );
  ATH_MSG_VERBOSE(  m_eflowPreparationAlgName  );
  ATH_MSG_VERBOSE( m_eflowObjectBuilderAlgName  );
  ATH_MSG_VERBOSE( m_eflowQuantitiesAlgName  );


  return sc;
}


/////////////////////////////////////////////////////////////////

StatusCode eflowBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////

// ATHENA EXECUTE METHOD:
   
StatusCode eflowBuilder::execute()
{  

  StatusCode sc;

  ATH_MSG_DEBUG( "Executing eflowBuilder"  );

  // Build Calorimeter Objects

  if (m_eflowPreparationAlg)
    {
      sc = m_eflowPreparationAlg->execute();
    }

  // Build Energy Flow Objects

  if (sc.isSuccess() && m_eflowObjectBuilderAlg)
    {
      sc = m_eflowObjectBuilderAlg->execute();
    }

  // Calculate parameters

  if (sc.isSuccess() && m_eflowQuantitiesAlg)
    {
      sc = m_eflowQuantitiesAlg->execute();
    }

  return StatusCode::SUCCESS;

}
