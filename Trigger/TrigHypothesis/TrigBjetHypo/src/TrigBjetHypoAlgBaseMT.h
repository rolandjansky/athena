/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGBJETHYPO_TRIGBJETHYPOALGBASEMT_H
#define TRIGBJETHYPO_TRIGBJETHYPOALGBASEMT_H 1

#include "DecisionHandling/HypoBase.h"

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthLinks/ElementLinkVector.h"

#include "TrigBjetEtHypoTool.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "AthViews/ViewHelper.h"

/**
 * @class Implements b-jet selection for the new HLT framework
 * @brief 
 **/

class TrigBjetHypoAlgBaseMT : public ::HypoBase {
 public: 

  TrigBjetHypoAlgBaseMT( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigBjetHypoAlgBaseMT();

  virtual StatusCode initialize() = 0;
  virtual StatusCode execute( const EventContext& ) const = 0;

 protected:
  TrigBjetHypoAlgBaseMT();

  StatusCode retrievePreviousDecisionContainer( const EventContext&,
						const TrigCompositeUtils::DecisionContainer*& ) const;


  template < class CONTAINER >
    StatusCode retrieveCollectionFromView( const EventContext&,
					   ElementLinkVector< CONTAINER >&,
					   const SG::ReadHandleKey< CONTAINER >&,
					   const TrigCompositeUtils::Decision* ) const;

  template < class CONTAINER >
    StatusCode retrieveObjectFromNavigation( const std::string&,
					     ElementLink< CONTAINER >&,
					     const TrigCompositeUtils::Decision* ) const;

  template < class CONTAINER >
    StatusCode retrieveCollectionFromNavigation( const std::string& linkName,
						 ElementLinkVector< CONTAINER >& objELs,
						 const TrigCompositeUtils::DecisionContainer* ) const;





  template < class CONTAINER >
    StatusCode retrieveObjectFromStoreGate( const EventContext&,
					    ElementLinkVector< CONTAINER >&,
					    const SG::ReadHandleKey< CONTAINER >& ) const;
  
  template < class CONTAINER >
    StatusCode retrieveObjectFromEventView( const EventContext&,
                                            ElementLinkVector< CONTAINER >&,
                                            const SG::ReadHandleKey< CONTAINER >&,
                                            const TrigCompositeUtils::DecisionContainer* ) const;


  template< class CONTAINER > 
    StatusCode attachObjectLinkToDecisionFromStoreGate( TrigCompositeUtils::Decision&,
							const SG::ReadHandleKey< CONTAINER >&,
							const std::string&,
							int objIndex = 0 ) const;

  template< class CONTAINER >
    StatusCode attachObjectLinkToDecisionsFromStoreGate( std::vector< TrigCompositeUtils::Decision* >&,
							 const SG::ReadHandleKey< CONTAINER >&,
                                                         const std::string&,
                                                         int forcedIndex = -1 ) const;

  template< class CONTAINER >
    StatusCode attachObjectLinkToDecisionFromEventView( const EventContext&,
							TrigCompositeUtils::Decision&,
							const SG::ReadHandleKey< CONTAINER >&,
							const std::string&,
							int objIndex = 0 ) const;


  template< class CONTAINER >
    StatusCode attachObjectLinkToDecisionsFromEventView( const EventContext&,
							 std::vector< TrigCompositeUtils::Decision* >&,
							 const SG::ReadHandleKey< CONTAINER >&,
							 const std::string&,
							 int forcedIndex = -1 ) const;

  template < class CONTAINER >
    StatusCode attachObjectCollectionLinkToDecisionsFromEventView( const EventContext&,
								   std::vector< TrigCompositeUtils::Decision* >&,
								   const SG::ReadHandleKey< CONTAINER >&,
								   const std::string& ) const;

}; 

// ======================================================================================== //

#include "TrigBjetHypoAlgBaseMT.icc"

// ======================================================================================== //

#endif


