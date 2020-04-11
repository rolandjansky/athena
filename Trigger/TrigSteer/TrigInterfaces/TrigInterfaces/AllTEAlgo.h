/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINTERFACES_AllTEAlgo_H
#define TRIGINTERFACES_AllTEAlgo_H


#include "TrigInterfaces/TECreateAlgo.h"

class TrigRoiDescriptor;
class TrigSuperRoi;

namespace HLT
{

  /**
   * @brief Base class for algorithms processing arbitrary lists of TEs (AllTE).
   *
   * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
   * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
   *
   * This is the class from which developers should inherit their implementation of an AllTE algorithm.
   *
   * The main characteristics of an AllTE algorithm are:
   * - receives N input TE types, producing output TEs of a single type;
   * - receives as input all TE instances of each requested input type;
   * - runs once per event, independently on the number of RoIs; can be unseeded;
   * - the developer is responsible for the creation of output TEs.
   *
   * The AllTE algorithm developer must re-implement the following three methods:
   * - hltInitialize(); this method must contain the code to be executed during the initialization phase,
   *   before starting a new run; example: memory allocation for static objects;
   * - hltExecute(); this method must contain the code to be executed for each event; this is the core part
   *   of the algorithm, taking care of reconstructing the relevant quantities, creating the output TEs with
   *   attached the reconstructed features;
   * - hltFinalize(); this method must contain the code to be executed during the finalization phase,
   *   after the run has been stopped; example: freeing the memory allocated to static objects during the
   *   initialization.
   *
   * Some general implementation guidelines that developers should follow, e.g. regarding features handling
   * or RoIDescriptors usage, can be found in HLT::FexAlgo class documentation.
   *
   * \par Specific guidelines for AllTE algorithms developers
   *
   * In order to decouple algorithm development from the implementation details for the Navigation service,
   * the developers should use the protected addRoI() methods to create the output TEs (to which an RoI will
   * be associated).
   */

  class AllTEAlgo : public TECreateAlgo {

  public:

    /**
     * @brief Constructor; gets the algorithm name and service locator as arguments.
     */
    AllTEAlgo(const std::string& name, ISvcLocator* pSvcLocator);


    /**
     * @brief Destructor.
     */
    ~AllTEAlgo() { }

    /** 
     * @brief Method implementing the algorithmic code, executed for each event.
     * @return HLT::ErrorCode for the execution of the algorithm.
     * @param inputs vector of input TEs containing all the TEs of all the types on which the algorithm
     *        has to be executed;
     * @param output type of the output TEs.
     *
     * This is the core method of a Combo algorithm, where developers must put all the algorithmic
     * intelligence needed to reconstruct features starting from the information retrieved from the input
     * TEs and to create the corresponding output TEs.
     */
    virtual HLT::ErrorCode hltExecute(std::vector<TEVec>& inputs,
				      unsigned int output) = 0;

    /**
     * @brief Method to register ROB requests. Should be overridden by algorithms.
     */
    virtual HLT::ErrorCode prepareRobRequests(const std::vector<HLT::TEConstVec>& inputs );

    virtual bool reset() final { return TECreateAlgo::reset(); }

  protected:

    /** 
     * @brief Method enabling to add a new TE corresponding to a given RoIDescriptor.
     * @return pointer to the newly added TriggerElement.
     * @param type type of the TE to be added;
     * @param roiDescriptor pointer to the RoI descriptor to be attached to the TE; null pointer by default.
     *
     * This method is specific to AllTE algorithms and enables to create a new TE node and attach an RoI to
     * it, given its RoI descriptor.
     */
    TriggerElement* addRoI(unsigned int type, const TrigRoiDescriptor* roiDescriptor = 0);
    //   TriggerElement* addRoI(unsigned int type, const TrigSuperRoi*      roiDescriptor ); // no longer needed

    /** 
     * @brief Method enabling to add a new TE corresponding to a given RoI direction.
     * @return pointer to the newly added TriggerElement.
     * @param type type of the TE to be added;
     * @param eta eta direction of the RoI to be attached to the TE;
     * @param phi phi direction of the RoI to be attached to the TE.
     *
     * This method is specific to AllTE algorithms and enables to create a new TE node and attach an RoI to
     * it, given its eta/phi direction.
     * NB: OBSOLETE - DO NOT USE
     */
    //    TriggerElement* addRoI(unsigned int type, double eta, double phi);

    /**
     * @brief Common method to register ROB requests to be called by the steering.
     */
    virtual HLT::ErrorCode processRobRequests(const std::vector< unsigned int >& inputs );

  private:

    /**
     * @brief Method implementing creation and processing of the output TEs.
     * @return HLT::ErrorCode for TE creation and processing.
     * @param inputs vector of input TE types.
     * @param output TE type.
     * @param outputTEs vector of newly created output TE pointers.
     *
     * This method performs the creation of output TEs and the processing of all the input TEs. In this case, this
     * means: retrieving all the TEs of all the input types; call the user-implemented hltExecute method, which
     * will do the input processing and output creation; retrieve and return the output TEs created during the
     * execution.
     */
    virtual HLT::ErrorCode createAndProcessTEs(std::vector<unsigned int>& inputs,
					       unsigned int outputType,
					       TEVec& outputTEs,
					       const std::vector<unsigned int>& topologicalStartFrom);

    /**
     * @brief Method implementing the processing of an output TE.
     * @return HLT::ErrorCode for TE processing.
     * @param inputTEs vector of input TEs.
     * @param outputTE output TE pointer.
     *
     * This method performs the processing of a input/output TEs set. This method will never be called, since AllTE
     * algorithms have the responsibility to create their output TEs on their own (see the plugin_createAndProcessTEs()
     * method).
     */
    virtual HLT::ErrorCode plugin_processTE(TEConstVec& inputTEs, TriggerElement* outputTE);
    
    bool m_provideInactiveTEs;

  };

} // end namespace
#endif
