/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINTERFACES_FexAlgo_H
#define TRIGINTERFACES_FexAlgo_H

#include "TrigInterfaces/TECombineAlgo.h"

namespace HLT
{

  /**
   * @brief Base class for feature extraction (FEX) algorithms.
   *
   * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
   * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
   *
   * This is the class from which developers should inherit their implementation of a
   * feature extraction algorithm.
   *
   * The main characteristics of a feature extraction algorithm are:
   * - runs on a single input TE type, producing a single output TE type;
   * - runs once on each available input TE instance of the required input type;
   * - produces an output TE for each input TE passing the cuts;
   * - can produce new data (feature extraction, computations), store it in EDM objects ("features")
   *   and associate them to the output TE.
   *
   * \par General guidelines for HLT algorithms developers
   *
   * The FEX algorithm developer must re-implement the following three methods:
   * - hltInitialize(); this method must contain the code to be executed during the initialization phase,
   *   before starting a new run; example: memory allocation for static objects, like look-up tables or
   *   histograms, to be accessed during all the run;
   * - hltExecute(); this method must contain the code to be executed on each input TE passed to the
   *   algorithm; this is the core part of the algorithm, taking care of performing calculations,
   *   creating new features and attaching them to the output TE;
   * - hltFinalize(); this method must contain the code to be executed during the finalization phase,
   *   after the run has been stopped; example: freeing the memory allocated to static objects during the
   *   initialization.
   *
   * The error codes returned by the user-implemented methods should follow these recommendations:
   * - the algorithm should always signal the errors encountered as explicitly as possible; the most common
   *   error code values are listed below:
   *   - HLT::OK: the good case;
   *   - HLT::ERROR, HLT::FATAL: generic errors;
   *   - HLT::BAD_JOB_SETUP: when a tool/service/property that the algorithm depends on isn't available
   *     (mostly in hltInitialize);
   *   - HLT::NAV_ERROR: error while dealing with the Navigation;
   *   - HLT::SG_ERROR: error dealing with StoreGate;
   *   - HLT::NUM_ERROR: error while performing numerical computations;
   *   - HLT::TOOL_FAILURE: when a tool returns an error state;
   * - in cases where another algorithm or tool (e.g. the Navigation) already returns an HLT::ErrorCode,
   *   this should be simply passed on; e.g. HLT::ErrorCode stat = getFeature(...); if (stat != HLT::OK)
   *   return stat;
   * - the complete list of possible error codes can be found in TrigSteeringEvent/Enums.h; anyway some
   *   values, such as HLT::BAD_ALGO_CONFIG, HLT::INVALID_TE and HLT::NO_*_RESULTS are mainly for internal
   *   use.
   *
   * In order to get/attach features from/to the input/output TE, the following guidelines should be followed:
   * - the normal use-case in a FEX algo is to read features from the input TE and attach them to the output TE;
   *   this is the standard case used in the following examples; anyway, in case more than one FEX algorithm
   *   are executed in the same sequence and the second needs features attached by first, then it must look for
   *   features giving the output TE as a starting point for feature retrieval;
   * - to get features, the getFeatures() method inherited from HLT::Algo class should be used; this requires to
   *   specify the vector of features to be retrieved, from which TE they should be taken and an optional
   *   identifier label; the behaviour of getFeatures() is to search backwards from the given TE until it finds
   *   a predecessor to which features of the given type are attached; when it finds such a TE, it returns only
   *   the features attached to this one TE and proceeds no further; example usage: HLT::ErrorCode status =
   *   getFeatures(outputTE, vectorOfFeatures, "a label");
   * - to get a single most recent feature of a given type stored in a TE or its predecessors, the getFeature()
   *   method inherited from HLT::Algo class should be used; this requires to specify the feature to be retrieved,
   *   from which TE it should be taken and an optional identifier label; example usage: HLT::ErrorCode status =
   *   getFeature(outputTE, myFeature, "a_label");
   * - to attach features, the attachFeature() method inherited from HLT::TECreateAlgo class should be used; this
   *   requires to specify the feature to be attached, to which TE they should be attached and an optional
   *   identifier label; example usage: HLT::ErrorCode status = attachFeature(outputTE, myFeature, "a label");
   * - the developer isn't required to storing the feature in StoreGate; this is done automatically by the
   *   HLT::Navigation class, if allowed by the job (e.g. not in an online LVL2 job);
   * - if some cases require a feature to be stored in StoreGate in all conditions (i.e. object must be in
   *   StoreGate in order to be accessed by an offline tool), the recordAndAttachFeature() method inherited
   *   from the HLT::TECreateAlgo should be used; this requires to specify the feature to be attached, to which
   *   TE they should be attached, an optional identifier label and a key; if an empty string variable is passed
   *   as key, after the call it will contain the StoreGate key used to store the feature; if a non-empty string
   *   is passed, it will be used to give an alias to the object (e.g. passing "egammaRoI0" as key and "gRec" as
   *   label will record an object with the key "HLT_AutoKey_gRec_3245_0" and give it alias "egammaRoI0");
   *   example usage: HLT::ErrorCode status = recordAndAttachFeature(myTE, myFeature, theKey, "a_label").
   *
   * Concerning the handling of RoIDescriptors, the following instructions should be followed:
   * - from the steering framework point of view the RoIDescriptors are treated exactly as any other feature;
   *   therefore, to retrieve an RoIDescriptor the getFeature() method should be used; if no feature label
   *   is given to getFeature(), the most recent descriptor is returned; example usage: HLT::ErrorCode
   *   status = getFeature(inputTE, roiDescriptor, "a_label");
   * - in order to update the description of a processed RoI, a newly created RoIDescriptor should be attached as
   *   a feature to the currently processed output TE using the attachFeature() method; developers are encouraged
   *   to use labels, when storing their modified RoIDescriptors, to distinguish between RoIDescriptors produced
   *   by different algorithms; example usage: HLT::ErrorCode status = attachFeature(outputTE, myRoiDescriptor, "a label").
   *
   * Access to the message stream (MsgStream) service is made possible using some helper methods of the HLT::Algo
   * base class; in particular:
   * - the MsgStream for the HLT algorithm is automatically created in the HLT::Algo base class;
   * - the user can get the MsgService and check the message level configured for his algorithm using the accessor
   *   msg() to access the MsgStream, and msgLvl() to access the current level of output detail.
   *
   * Access to the timer service (TrigTimerSvc) is embedded in some helper methods of the HLT::Algo base class.
   * The TimerSvc is initialized automatically in the base class, and timers can be manipulated using helper functions:
   * - to get a pointer to a new timer, the addTimer() method must be used; this returns a pointer to the newly
   *   created timer and requires a label to identify it in a descriptive way;
   * - in case a property is to be associated to the requested timer (e.g. for normalization purposes, like counting
   *   the number of processed objects), an additional parameter must be passed to the addTimer() method, specifying
   *   the name of the property.
   *
   * \par Specific guidelines for FEX algorithms developers
   *
   * To achieve an additional optimization of the time spent processing all the possible input TEs, pre-cuts can
   * be performed on the list of input TEs to be processed. This is achieved in FEX algorithms by implementing
   * the member method acceptInput(). The steering will create an output TE only for those input TEs that passed
   * this selection. Please note that the acceptInput() method is seen only for the first FEX algo in a sequence.
   */

  class FexAlgo : public TECombineAlgo
  {
    
  public:
    
    /**
     * @brief Constructor; gets the algorithm name and service locator as arguments.
     */
    FexAlgo(const std::string& name, ISvcLocator* pSvcLocator);

    /**
     * @brief Destructor.
     */
    virtual ~FexAlgo() { }

    /** 
     * @brief Method performing validation of the input TE before execution.
     * @return HLT::ErrorCode for the input checking.
     * @param te pointer to the TE to be evaluated.
     * @param pass boolean result of the TE evaluation.
     *
     * This method is used by the steering to filter TEs before they are accepted for execution
     * as inputs to an algorithm. The default overloaded method always accepts the TE and
     * returns HLT::OK; this method must be overloaded by the developer to implement a real selection
     * on input TEs.
     */
    virtual HLT::ErrorCode acceptInput(const TriggerElement*, 
				       bool& pass) { pass = true; return HLT::OK; }

    /** 
     * @brief Method implementing the algorithmic code, executed once per input TE.
     * @return HLT::ErrorCode for the execution of the algorithm.
     * @param input pointer to the input TE on which the algorithms has to be executed.
     * @param output pointer to the output TE, to be updated during the execution of the algorithm.
     *
     * This is the core method of a FEX algorithm, where developers must put all the algorithmic
     * intelligence needed to reconstruct features for a given TE. A single TE is provided as input
     * and all the reconstructed features must be attached to a single output TE.
     */
    virtual HLT::ErrorCode hltExecute(const TriggerElement* input, 
				      TriggerElement* output) = 0;
  

    /**
     * @brief Method to register ROB requests. Either this or the next
     * function should be overridden by algorithms.
     */
    virtual HLT::ErrorCode prepareRobRequests(const HLT::TEConstVec& inputs );

    /**
     * @brief Method to register ROB requests. Either this or the
     * previous function should be overridden by algorithms.
     */
    virtual HLT::ErrorCode prepareRobRequests(const HLT::TriggerElement* input );

    /**
     * @brief Method performing algorithm reset.
     */
    virtual bool reset() final { return TECombineAlgo::reset(); }

  protected:
     virtual HLT::ErrorCode processRobRequests(const std::vector< unsigned int >& inputs );


  private:

    /** @brief Feature caching enable/disable flag. */
    bool m_doCaching;

    /**
     * @brief Method preparing the iteration over the list of TEs of the given type.
     * @return HLT::ErrorCode for the initialization of the iteration process.
     * @param inputTypes vector of input TE types; only the first element of the vector is really used in this case.
     * @param topologicalStartFrom optional topological types from which to start.
     *
     * This method initializes the iteration over the list of TEs to be processed. All the TEs of the required type
     * specified are retrieved and stored in a TE vector; a TE iterator is then initialized at the beginning of this
     * vector.
     */
    HLT::ErrorCode init_inputTE_iteration(std::vector<unsigned int>& inputTypes,
					  const std::vector<unsigned int>& topologicalStartFrom);

    /**
     * @brief Method performing a step of the iteration over the list of TEs of the given type.
     * @return HLT::ErrorCode for the current step of the iteration process.
     * @param inputTEs vector of input TEs corresponding to the current iteration step; the vector is filled with
     * just one TE in this case.
     * @param topologicalStartFrom optional topological types from which to start.
     *
     * This method performs a step in the iteration over the vector of TEs to be processed. The next TE resulting
     * valid and accepted by the acceptInput method is made available to the steering in the vector passed by
     * reference.
     */
    virtual HLT::ErrorCode next_inputTE_iteration(TEVec& inputTEs,
						  const std::vector<unsigned int>& topologicalStartFrom);

    /**
     * @brief Method implementing the processing of an output TE.
     * @return HLT::ErrorCode for TE processing.
     * @param inputTEs vector of input TEs.
     * @param outputTE output TE pointer.
     *
     * This method performs the processing of a input/output TE couple. In this case, this means: handling the caching
     * of features, avoiding to execute twice the same algorithm on the same RoI; checking that the vector of input
     * TEs only contains one element; call the user-implemented hltExecute method for the input/output TE couple.
     */
    virtual HLT::ErrorCode plugin_processTE(TEConstVec& inputTEs, TriggerElement* outputTE);
    

    /**
     * @brief as plugin_processTE but no output yet
     * @copydoc TECombineAlgo::plugin_acceptInputs(TEVec&, bool&)
     */
    virtual HLT::ErrorCode plugin_acceptInputs(TEVec& inputTEs, bool& pass);

    /** @brief Vector of TEs to be processed. */
    TEVec m_TEs;
    
    /** @brief Iterator pointing to the next TE to be processed. */
    TEVec::iterator m_TEIter;
  };

} // end namespace
#endif
