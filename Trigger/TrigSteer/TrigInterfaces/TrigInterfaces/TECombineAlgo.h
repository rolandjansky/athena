/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINTERFACES_TECombineAlgo_H
#define TRIGINTERFACES_TECombineAlgo_H

#include "TrigInterfaces/TECreateAlgo.h"

namespace HLT
{

  /**
   * @brief Class providing helper methods to iterate over input TEs and create output TEs.
   *
   * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
   * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
   *
   * This class implements the helper methods used by HLT algorithms to iterate over input
   * input TEs and create output TEs.
   */

  class TECombineAlgo : public TECreateAlgo {
    
  public:
    
    /**
     * @brief Constructor; gets the algorithm name and service locator as arguments.
     */
    TECombineAlgo(const std::string& name, ISvcLocator* pSvcLocator)
      : TECreateAlgo(name, pSvcLocator) { }
      
    /**
     * @brief Destructor.
     */
    ~TECombineAlgo() { }
      
    
    /**
     * @brief Method preparing the iteration over the list of TEs of the given types.
     * @return HLT::ErrorCode for the initialization of the iteration process.
     * @param inputs vector of input TE types.
     * @param topologicalStartFrom optional topological types from which to start.
     *
     * This method must be reimplemented in order to correctly initialize the iteration
     * over the list of TEs of the given types.
     */
    virtual ErrorCode init_inputTE_iteration(std::vector<unsigned int>& inputTypes,
					     const std::vector<unsigned int>& topologicalStartFrom) = 0;

    /**
     * @brief Method performing a step of the iteration over the list of TEs of the given types.
     * @return HLT::ErrorCode for the current step of the iteration process.
     * @param inputTEs vector of input TEs corresponding to the current iteration step.
     * @param topologicalStartFrom optional topological types from which to start.
     *
     * This method must be reimplemented in order to correctly perform the iteration
     * over the list of TEs of the given types.
     */
    virtual ErrorCode next_inputTE_iteration( TEVec& inputTEs,
					      const std::vector<unsigned int>& topologicalStartFrom) = 0;


    /**
     * @brief Method implementing creation and processing of the output TEs.
     * @return HLT::ErrorCode for TE creation and processing.
     * @param inputs vector of input TE types.
     * @param output TE type.
     * @param outputTEs vector of newly created output TE pointers.
     * @param topologicalStartFrom optional topological types from which to start.
     *
     * This method creates and processes the new output TEs. It initializes the iteration over
     * input TEs, using the init_inputTE_iteration method; then it iterates over input TEs,
     * using the next_inputTE_iteration method, creating the corresponding output TE and navigation
     * links and processing each combination, using the processTE method.
     */
    virtual HLT::ErrorCode createAndProcessTEs(std::vector<unsigned int>& inputs, 
					       unsigned int outputType,
					       TEVec& outputTEs,
					       const std::vector<unsigned int>& topologicalStartFrom);

    /**
     * @brief this method deciding about acceptance of the inputs
     * This is very similar in functionality to the plugin_processTE except that the output TE is not known yet
     */
    virtual HLT::ErrorCode plugin_acceptInputs(TEVec& inputTEs, bool& pass) = 0;


  };

} // end namespace

#endif
