/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionToolD3PD.cxx 502443 2012-05-25 10:43:25Z krasznaa $

// STL include(s):
#include <algorithm>
#include <utility>

// Local include(s):
#include "TrigRootAnalysis/TrigDecisionToolD3PD.h"

ClassImp( D3PD::TrigDecisionToolD3PD )

namespace D3PD {

   /**
    * To read any encoded trigger information from a D3PD, the file has to contain
    * a metadata tree called "TrigConfTree". This is located in the file in a
    * subdirectory called "<d3pd tree name>Meta". For instance in the egammaD3PDs
    * the tree is under "egammaMeta/TrigConfTree". If you don't have such a tree
    * in your file, the tool will not be usable.
    *
    * The tool of course also needs to access the event-level TTree holding the
    * event-wise trigger information.
    *
    * @param eventTree Pointer to the event-wise D3PD TTree
    * @param confTree  Pointer to "TrigConfTree"
    * @param prefix    Prefix used by the trigger tools. "trig_" by default.
    */
   TrigDecisionToolD3PD::TrigDecisionToolD3PD( TTree* eventTree, TTree* confTree,
                                               const TString& prefix )
      : TNamed( "TrigDecisionToolD3PD", "Trigger decision tool" ),
        Trig::DataAccess( eventTree, prefix ),
        Trig::ConfigAccess( confTree ) {

   }

} // namespace D3PD
