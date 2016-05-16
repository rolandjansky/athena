/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PyTrigDecisionToolD3PD.cxx 726661 2016-02-28 01:18:32Z ssnyder $

// Local include(s):
#include "TrigRootAnalysis/PyTrigDecisionToolD3PD.h"

ClassImp( D3PD::PyTrigDecisionToolD3PD )

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
   PyTrigDecisionToolD3PD::PyTrigDecisionToolD3PD( TTree* eventTree,
                                                   TTree* confTree,
                                                   const TString& prefix )
      : TNamed( "PyTrigDecisionToolD3PD", "PyTriggerDecisionTool" ), 
        m_tdt( eventTree, confTree, prefix ) {

   }

   /**
    * Perhaps the most important function that people need. It can tell the user
    * whether a given LVL1 item or HLT chain passed in the current event.
    *
    * By default the function returns the "physics decision", but it can be asked
    * to give more detailed results as well. The function tries to be smart, so if
    * the user asks for information that is not available in the given D3PD, it
    * should just print a warning. (And return kFALSE.)
    *
    * For a description of the types of trigger results, have a look at the
    * documentation of the TrigDefs::DecisionTypes enumeration.
    *
    * Just forwards the call to the internal tool that does the actual work.
    *
    * @param name The name of the trigger we're interested in
    * @param type The type of the trigger decision to look up
    * @returns <code>kTRUE</code> if the trigger was passed in the current event,
    *          <code>kFALSE</code> otherwise
    */
   Bool_t PyTrigDecisionToolD3PD::IsPassed( const std::string& name,
                                            TrigDefs::DecisionTypes type ) {

      return m_tdt.IsPassed( name, type );
   }

   /**
    * This is a very slow function. It uses the <code>GetPassedL1Triggers(...)</code>,
    * <code>GetPassedL2Triggers(...)</code> and <code>GetPassedEFTriggers(...)</code>
    * functions, and merges their results.
    *
    * So if you know what kind of triggers you're looking for, you're better off
    * using one of the specific functions.
    *
    * Just forwards the call to the internal tool that does the actual work.
    *
    * @param type The trigger decision type you're interested in
    * @returns The names of all the triggers that fired in the current event
    */
   std::vector< std::string >
   PyTrigDecisionToolD3PD::
   GetPassedTriggers( D3PD::TrigDefs::DecisionTypes type ) {

      return m_tdt.GetPassedTriggers( type );
   }

   /**
    * Get a list of all the LVL1 triggers that passed in the current event.
    * Keep in mind that the function is quite slow. It has to do multiple lookups
    * in an std::map, that I don't see avoiding. It also returns the name list
    * by value, which can take a lot of CPU cycles. (Creating the object on the
    * heap and returning a pointer to it would be terribly prone to memory leaks.)
    *
    * Use this function only in interactive analysis, or just in the final steps
    * of an analysis. (On a "small" number of events.)
    *
    * Just forwards the call to the internal tool that does the actual work.
    *
    * @param type The trigger decision type you're interested in
    * @returns The names of all the LVL1 items that fired in the current event
    */
   std::vector< std::string >
   PyTrigDecisionToolD3PD::
   GetPassedL1Triggers( D3PD::TrigDefs::DecisionTypes type ) {

      return m_tdt.GetPassedL1Triggers( type );
   }

      /**
    * Get a list of all the LVL2 triggers that passed in the current event.
    * Keep in mind that the function is quite slow. It has to do multiple lookups
    * in an std::map, that I don't see avoiding. It also returns the name list
    * by value, which can take a lot of CPU cycles. (Creating the object on the
    * heap and returning a pointer to it would be terribly prone to memory leaks.)
    *
    * Use this function only in interactive analysis, or just in the final steps
    * of an analysis. (On a "small" number of events.)
    *
    * Just forwards the call to the internal tool that does the actual work.
    *
    * @param type The trigger decision type you're interested in
    * @returns The names of all the LVL2 items that fired in the current event
    */
   std::vector< std::string >
   PyTrigDecisionToolD3PD::
   GetPassedL2Triggers( D3PD::TrigDefs::DecisionTypes type ) {

      return m_tdt.GetPassedL2Triggers( type );
   }

   /**
    * Get a list of all the EF triggers that passed in the current event.
    * Keep in mind that the function is quite slow. It has to do multiple lookups
    * in an std::map, that I don't see avoiding. It also returns the name list
    * by value, which can take a lot of CPU cycles. (Creating the object on the
    * heap and returning a pointer to it would be terribly prone to memory leaks.)
    *
    * Use this function only in interactive analysis, or just in the final steps
    * of an analysis. (On a "small" number of events.)
    *
    * Just forwards the call to the internal tool that does the actual work.
    *
    * @param type The trigger decision type you're interested in
    * @returns The names of all the EF items that fired in the current event
    */
   std::vector< std::string >
   PyTrigDecisionToolD3PD::
   GetPassedEFTriggers( D3PD::TrigDefs::DecisionTypes type ) {

      return m_tdt.GetPassedEFTriggers( type );
   }

   /**
    * Just forwards the call to the internal tool that does the actual work.
    *
    * @returns The pointer to the currently used even-wise tree
    */
   ::TTree* PyTrigDecisionToolD3PD::GetEventTree() const {

      return m_tdt.GetEventTree();
   }

   /**
    * This function can be used to instruct the object to start using a new event-wise
    * TTree. This is very useful when processing multiple D3PD files.
    *
    * Note that it is possible to provide a null pointer to the function. In this
    * case the object will be reset, and the previous TTree will be released without
    * connecting to a new one.
    *
    * Also note that unfortunately we can't just check if the passed TTree pointer
    * is the same as the cached one. ROOT has the habit of putting the same sort
    * of TTree into the same memory space when the second file is opened for instance.
    * I've seen some weird problems because of this.
    *
    * Just forwards the call to the internal tool that does the actual work.
    *
    * @param eventTree The event-wise D3PD TTree
    * @returns <code>kTRUE</code> if the operation was successful,
    *          <code>kFALSE</code> otherwise
    */
   ::Bool_t PyTrigDecisionToolD3PD::SetEventTree( ::TTree* eventTree ) {

      return m_tdt.SetEventTree( eventTree );
   }

   /**
    * This function can come in *very* handy when implementing an event selection
    * using a D3PD. Using this function it's very easy to copy over the trigger
    * decision information of the selected events into an output TTree.
    *
    * Just forwards the call to the internal tool that does the actual work.
    *
    * @param outputTree The TTree to write the trigger decision branches to
    * @param detailLevel The detail level for which the branches should be copied
    * @returns <code>kTRUE</code> if the operation was successful,
    *          <code>kFALSE</code> otherwise
    */
   ::Bool_t PyTrigDecisionToolD3PD::WriteToTree( ::TTree* outputTree,
                                                 ::Int_t detailLevel ) {

      return m_tdt.WriteToTree( outputTree, detailLevel );
   }

   /**
    * Since the Trig::IConfigAccess interface no longer returns a reference
    * to the concrete TrigConfigSvcD3PD object, a new function had to be
    * created here to be able to pass a new configuration tree to an existing
    * TDT object.
    *
    * Just forwards the call to the internal tool that does the actual work.
    *
    * @param confTree Pointer to a trigger configuration metadata tree
    * @returns <code>kTRUE</code> if the operation was successful,
    *          <code>kFALSE</code> if it wasn't
    */
   ::Bool_t PyTrigDecisionToolD3PD::SetConfigTree( ::TTree* confTree ) {

      return m_tdt.SetConfigTree( confTree );
   }

   /**
    * Unfortunately however I tried I didn't manage to figure out an elegant way
    * of automatically figuring out which event the tool should be operating on.
    * Of course if the user issues TTree::GetEntry(...) calls on the event-wise
    * TTree himself/herself, then the tool gets the correct data automatically.
    * But since this is usually a slow thing to do, the user can also explicitly
    * request the tool to read a new event using this function.
    *
    * Just forwards the call to the internal tool that does the actual work.
    *
    * @param entry The entry from the event-wise tree that should be loaded
    * @param getall Flag telling whether all the sub-branches should be read
    *               (Set to "1" when writing out pre-selected events!)
    * @returns The number of bytes read
    */
   ::Int_t PyTrigDecisionToolD3PD::GetEntry( ::Long64_t entry,
                                             ::Int_t getall ) {

      return m_tdt.GetEntry( entry, getall );
   }

   /**
    * Get the detail level that the D3PD was produced with
    * 
    * Just forwards the call to the internal tool that does the actual work.
    */
   Int_t PyTrigDecisionToolD3PD::GetDetailLevel() {

      return m_tdt.GetDetailLevel( );
   }

   /**
    * Create a ChainGroup object from a list of trigger patterns. It's possible to use
    * regular expressions. For instance the following creates a chain group of all the
    * EF muon chains: ["EF_mu.*"].
    *
    * Just forwards the call to the internal tool that does the actual work.
    *
    * @param patterns A list of trigger name patterns
    * @returns A ChainGroup object describing the requested triggers
    */
   D3PD::ChainGroup
   PyTrigDecisionToolD3PD::
   GetChainGroup( const std::vector< std::string >& patterns ) {

      return m_tdt.GetChainGroup( patterns );
   }

   /**
    * This overloaded function can be helpful when you only want to use one trigger
    * name pattern. Notice that it's possible to give a comma separated list of trigger
    * names to the function. So the following is valid for instance:
    * "L1_MU6,L1_EM5,L1_J.*".
    *
    * Just forwards the call to the internal tool that does the actual work.
    *
    * @param patterns A list of trigger name patterns
    * @returns A ChainGroup object describing the requested triggers
    */
   D3PD::ChainGroup
   PyTrigDecisionToolD3PD::GetChainGroup( const std::string& pattern ) {

      return m_tdt.GetChainGroup( pattern );
   }

   /**
    * This function is used by the other parts of the TDT to access the trigger
    * configuration service.
    *
    * Just forwards the call to the internal tool that does the actual work.
    *
    * @returns A reference to the trigger configuration service object
    */
   D3PD::IITrigConfigSvcD3PD&
   PyTrigDecisionToolD3PD::GetConfigSvc( ::Bool_t update ) {

      return m_tdt.GetConfigSvc( update );
   }

} // namespace D3PD
