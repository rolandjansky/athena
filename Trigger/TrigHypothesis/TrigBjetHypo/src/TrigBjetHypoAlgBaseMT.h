/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGBJETHYPO_TRIGBJETHYPOALGBASEMT_H
#define TRIGBJETHYPO_TRIGBJETHYPOALGBASEMT_H 1

#include "DecisionHandling/HypoBase.h"

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthLinks/ElementLinkVector.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

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
  virtual StatusCode execute( const EventContext& context ) const = 0;

 protected:
  TrigBjetHypoAlgBaseMT();

  StatusCode retrievePreviousDecisionContainer( const EventContext&,
						const TrigCompositeUtils::DecisionContainer*& ) const;

  template < class T >
    StatusCode retrieveObjectFromStoreGate( const EventContext&,
					    ElementLinkVector< DataVector< T > >&,
					    const SG::ReadHandleKey< DataVector< T > >&,
					    const TrigCompositeUtils::DecisionContainer* ) const;
  
  template < class T >
    StatusCode retrieveObjectFromEventView( const EventContext&,
                                            ElementLinkVector< DataVector< T > >&,
                                            const SG::ReadHandleKey< DataVector< T > >&,
                                            const TrigCompositeUtils::DecisionContainer* ) const;

  virtual StatusCode attachLinkToDecisions( const EventContext&,
					    const TrigCompositeUtils::DecisionContainer*,
					    std::vector< TrigCompositeUtils::Decision* >& ) const = 0;

}; 

// ======================================================================================== //

template < class T >
StatusCode TrigBjetHypoAlgBaseMT::retrieveObjectFromStoreGate( const EventContext& context,
							       ElementLinkVector< DataVector< T > >& ELs,
							       const SG::ReadHandleKey< DataVector< T > >& inputKey,
							       const TrigCompositeUtils::DecisionContainer* /*prevDevisionContainer*/) const {
  
  ATH_MSG_DEBUG( "Retrieving object from StoreGate from " << inputKey.key() );

  SG::ReadHandle< DataVector<T >  > ContainerHandle = SG::makeHandle( inputKey,context );
  CHECK( ContainerHandle.isValid() );
  const DataVector< T > *Collection = ContainerHandle.get();

  for (const T* obj : *Collection) {
    ElementLink< DataVector< T > > Link = ElementLink< DataVector< T >  >(*Collection, obj->index());
    ELs.push_back( Link );
  }

  return StatusCode::SUCCESS;
}

template < class T >
StatusCode TrigBjetHypoAlgBaseMT::retrieveObjectFromEventView( const EventContext& context,
                                                               ElementLinkVector< DataVector< T > >& ELs,
                                                               const SG::ReadHandleKey< DataVector< T > >& inputKey,
                                                               const TrigCompositeUtils::DecisionContainer* prevDevisionContainer) const {

  ATH_MSG_DEBUG( "Retrieving object from Event View from " << inputKey.key() );

  for ( const TrigCompositeUtils::Decision* previousDecision: *prevDevisionContainer ) {
    // get View
    ElementLink< ViewContainer > viewEL = previousDecision->objectLink< ViewContainer >( TrigCompositeUtils::viewString() );
    ATH_CHECK( viewEL.isValid() );
    ATH_MSG_DEBUG( "Retrieved View" );

    SG::ReadHandle< DataVector< T > > Handle = ViewHelper::makeHandle( *viewEL, inputKey, context );
    ATH_CHECK( Handle.isValid() );
    ATH_MSG_DEBUG ( "EventView " << (*viewEL)->name() << " has object's container of size: " << Handle->size() );

    size_t Counter = 0;
    for ( auto it = Handle->begin(); it != Handle->end(); ++it, ++Counter ) {
      ElementLink< DataVector< T > > EL = ViewHelper::makeLink< DataVector< T > >( *viewEL, Handle, Counter );
      ATH_CHECK( EL.isValid() );
      ELs.push_back( EL );
    }
  }

  return StatusCode::SUCCESS;
}

#endif //> !TRIGBJETHYPO_TRIGBJETETHYPOALG_H
