#include "AthContainers/AuxElement.h"
#include "TrigConfHLTData/HLTUtils.h"


#include "ViewAlgs/TrigCompositeUtils.h"



static SG::AuxElement::Accessor< std::vector< int > > readWriteAccessor( "decisions" );
static SG::AuxElement::ConstAccessor< std::vector<int> > readOnlyAccessor( "decisions" );

namespace TrigCompositeUtils {  
  
  Decision* newDecisionIn (DecisionContainer* dc) {
    Decision * x = new Decision;
    dc->push_back(x);
    return x;
  }


  void addDecisionID( DecisionID id,  Decision* d) {   
    readWriteAccessor(*d).push_back(id);

  }

  void addDecisionID( const std::string& s, Decision* d) {
    addDecisionID(TrigConf::HLTUtils::string2hash(s, "decision"), d);
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
