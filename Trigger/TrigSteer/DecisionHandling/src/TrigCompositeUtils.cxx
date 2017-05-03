#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "AthContainers/AuxElement.h"

#include "DecisionHandling/TrigCompositeUtils.h"



static SG::AuxElement::Accessor< std::vector< int > > readWriteAccessor( "decisions" );
static SG::AuxElement::ConstAccessor< std::vector<int> > readOnlyAccessor( "decisions" );

namespace TrigCompositeUtils {  

  DecisionOutput::DecisionOutput() {
    decisions = std::make_unique<DecisionContainer>();
    aux       = std::make_unique<DecisionAuxContainer>();
    decisions->setStore(aux.release());
  }

  StatusCode DecisionOutput::record(const EventContext& ctx, const SG::WriteHandleKey<DecisionContainer>& key) {
    SG::WriteHandle<DecisionContainer> handle(key, ctx);
    return handle.record(std::move(decisions), std::move(aux));
  }

  StatusCode DecisionInput::retrieve(const EventContext& ctx, const SG::ReadHandleKey<DecisionContainer>& key) {
    SG::ReadHandle<DecisionContainer> handle(key, ctx);
    decisions = handle.get();
    return StatusCode::SUCCESS;
  }

  
  Decision* newDecisionIn (DecisionContainer* dc) {
    Decision * x = new Decision;
    dc->push_back(x);
    return x;
  }


  void addDecisionID( DecisionID id,  Decision* d) {   
    readWriteAccessor(*d).push_back(id);

  }

  
  void decisionIDs(const Decision* d, DecisionIDContainer& destination ) {
    
    const std::vector<int>& decisions = readOnlyAccessor(*d);
    
    destination.insert(decisions.begin(), decisions.end());
  }

  bool passingIDs( const Decision* d,  const DecisionIDContainer& required) {
    for ( auto id : readOnlyAccessor(*d)) {
      if ( required.count(id) > 0 )
	return true;
    }
    return false;
  }
    
}
