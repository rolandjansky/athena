/**
 * @file TrigNavTools/src/getFeatureOccurrences.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2020
 * @brief Return number of occurrences of each feature.
 *
 * Broken out from the old TrigNavigationSlimmingTool (B. Smith / T. Bold).
 */


#include "TrigNavTools/getFeatureOccurrences.h"
#include "TrigNavigation/Navigation.h"
#include "getLabel.h"


namespace HLT {
namespace TrigNavTools {


/// Return number of occurrences of each feature.
std::map<std::string, int>
getFeatureOccurrences (const HLT::NavigationCore* navigation)
{
  std::map<std::string, int> featureOccurrences;

  if (navigation && navigation->getInitialNode()) {
  
    for ( auto te : navigation->getAllTEs() ) {
      for ( const auto& fea: te->getFeatureAccessHelpers() ) {
        featureOccurrences[SlimmingHelper::getLabel(*navigation, fea)] += 1;
      }
    }
  }

  return featureOccurrences;
}


}} // namespace HLT::TrigNavTools
