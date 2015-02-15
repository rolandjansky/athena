/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigInterface
 * @Class  : ComboAlgo
 *
 * @brief  base class for FexAlgo, HypoAlgo ... used by PESA HLT Algorithms
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 *
 * File and Version Information:
 * $Id: ComboAlgo.h,v 1.11 2008-03-20 10:30:56 tbold Exp $
 **********************************************************************************/

#ifndef TRIGINTERFACES_ComboAlgo_H
#define TRIGINTERFACES_ComboAlgo_H

#include "TrigInterfaces/TECombineAlgo.h"

namespace HLT
{
  class ComboIteratorBase;

  /**
   * @brief Base class for algorithms combining together more TEs (Combo).
   *
   * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
   * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
   *
   * This is the class from which developers should inherit their implementation of a TE-combining algorithm.
   *
   * The main characteristics of a Combo algorithm are:
   * - receives N input TE types, producing output TEs of a single type;
   * - performs all the combinatorial groupings of input TEs;
   * - is executed once on each combination of input TEs;
   * - an output TE is created for each validated combination. 
   *
   * The Combo algorithm developer must re-implement the following three methods:
   * - hltInitialize(); this method must contain the code to be executed during the initialization phase,
   *   before starting a new run; example: memory allocation for static objects;
   * - hltExecute(); this method must contain the code to be executed on each combination of input TEs passed
   *   to the algorithm; this is the core part of the algorithm, taking care of reconstructing the relevant
   *   quantities and attaching reconstructed features to the corresponding output TE;
   * - hltFinalize(); this method must contain the code to be executed during the finalization phase,
   *   after the run has been stopped; example: freeing the memory allocated to static objects during the
   *   initialization.
   *
   * Some general implementation guidelines that developers should follow, e.g. regarding features handling
   * or RoIDescriptors usage, can be found in HLT::FexAlgo class documentation.
   *
   * \par Specific guidelines for Combo algorithms developers
   *
   * To avoid processing all the possible combinations of input TEs, a preliminary selection can be performed.
   * This is achieved, in Combo algorithms, by implementing the member method acceptInputs(). The steering will
   * create an output TE only for those combination of input TEs that passed this selection. Please note that
   * the acceptInputs() method is seen only for the first Combo algo in a sequence.
   */

  class ComboAlgo : public TECombineAlgo {

  public:

    /**
     * @brief Constructor; gets the algorithm name and service locator as arguments.
     */
    ComboAlgo(const std::string& name, ISvcLocator* pSvcLocator);


    /**
     * @brief Destructor.
     */
    ~ComboAlgo();

    /** 
     * @brief Method performing validation of an input TE combination before execution.
     * @return HLT::ErrorCode for the input checking.
     * @param teVect vector of TEs corresponding to the combination to be validated;
     * @param pass boolean result of the combination selection.
     *
     * This method is used by the steering to filter combination of TEs before they are accepted
     * for execution of a Combo algorithm. In this case the default overloaded method always
     * accepts the combination of TEs and returns HLT::OK; this method must be overloaded by the
     * developer to implement a real selection on input TEs.
     */
    virtual HLT::ErrorCode acceptInputs(TEConstVec&, 
					bool& pass) { pass = true; return HLT::OK; }

    /** 
     * @brief Method implementing the algorithmic code, executed for each combination of input TEs.
     * @return HLT::ErrorCode for the execution of the algorithm.
     * @param inputs vector of input TEs corresponding to the combination on which the algorithm has
     *        to be executed.
     * @param output pointer to the output TE, to be updated during the execution of the algorithm.
     *
     * This is the core method of a Combo algorithm, where developers must put all the algorithmic
     * intelligence needed to reconstruct features for a given combination of TEs. A vector of TEs is
     * provided as input and all the reconstructed features must be attached to a single output TE.
     */
    virtual HLT::ErrorCode hltExecute(TEConstVec& inputs, 
				      TriggerElement* output) = 0;

    /**
     * @brief Method to register ROB requests. Should be overridden by algorithms.
     */
    virtual HLT::ErrorCode prepareRobRequests(const std::vector<HLT::TEConstVec>& inputs );


    /**
     * @brief Common method to register ROB requests to be called by the steering.
     */
    virtual HLT::ErrorCode processRobRequests(const std::vector< unsigned int >& inputs );

    /**
     * @brief The method reset called before each event.
     */
    virtual bool reset() final;


    /**
     * @brief provide access to the TriggerElement that a combination is based on
     */
    TriggerElement * getTopoTE() const;

    /**
     * @brief for the framework to set the TriggerElement that a combination is based on
     */
    void setTopoTE(TriggerElement *);




  private:
    
    /**
     * @brief Method preparing the iteration over the list of TEs of the given type.
     * @return HLT::ErrorCode for the initialization of the iteration process.
     * @param inputTypes vector of input TE types.
     * @param topologicalStartFrom optional topological types from which to start.
     *
     * This method initializes the iteration over the list of TEs to be processed. All the TEs of the required types
     * specified are retrieved, creating a ComboIterator to loop over all their possible combinations.
     */
    HLT::ErrorCode init_inputTE_iteration(std::vector<unsigned int>& inputTypes,
					  const std::vector<unsigned int>& topologicalStartFrom);

    /**
     * @brief Method performing a step of the iteration over the combinations of TEs of the given types.
     * @return HLT::ErrorCode for the current step of the iteration process.
     * @param inputTEs vector of input TEs corresponding to the current iteration step.
     * @param topologicalStartFrom optional topological types from which to start.
     *
     * This method performs a step in the iteration over the combinations of TEs to be processed. The next TE
     * combination resulting valid and accepted by the acceptInputs() method is made available to the steering in
     * the vector passed by reference.
     */
    HLT::ErrorCode next_inputTE_iteration(TEVec& inputTEs,
					  const std::vector<unsigned int>& topologicalStartFrom);
    
    /**
     * @brief Method implementing the processing of an output TE.
     * @return HLT::ErrorCode for TE processing.
     * @param inputTEs vector of input TEs.
     * @param outputTE output TE pointer.
     *
     * This method performs the processing of a input/output TEs combination. In this case, this simply calls the
     * hltExecute() method for the current combination.
     */
    virtual HLT::ErrorCode plugin_processTE(TEConstVec& inputTEs, TriggerElement* outputTE);

    /**
     * @brief as plugin_processTE but no output yet
     * @copydoc TECombineAlgo::plugin_acceptInputs(TEVec&, bool&)
     */
    virtual HLT::ErrorCode plugin_acceptInputs(TEVec& inputTEs, bool& pass);


    /** @brief Pointer to the internal ComboIterator used to iterate over TE combinations. */
    ComboIteratorBase* m_combIter;
    unsigned int m_combinationsLimit;
    unsigned int m_successfulCombinationsLimit;
    unsigned int m_combinationsCount;
    unsigned int m_successfulCombinationsCount;
     
    TriggerElement * m_topoTE { 0 }; // TriggerElement that the current combination is based on, in case we use the topo filter
    
  };

} // end namespace

#endif
