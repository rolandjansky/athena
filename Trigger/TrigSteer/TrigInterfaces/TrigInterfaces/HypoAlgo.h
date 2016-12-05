/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINTERFACES_HypoAlgo_H
#define TRIGINTERFACES_HypoAlgo_H


#include "TrigInterfaces/Algo.h"

class TrigPassBits; 
class TrigPassFlags; 

namespace HLT
{

  /**
   * @brief Base class for hypothesis (Hypo) algorithms.
   *
   * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
   * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
   *
   * This is the class from which developers should inherit their implementation of an hypothesis algorithm.
   *
   * The main characteristics of an hypothesis algorithm are:
   * - is meant to activate or deactivate TEs, basing the decision on features already attached to the processed TEs;
   * - runs on a single output TE to be validated, producing a boolean decision for each processed TE;
   * - can't create new TEs;
   * - can't add features to the processed TEs;
   * - is re-run every time the trigger is re-run, except for those cases in which event selection is disabled.
   *
   * The Hypo algorithm developer must re-implement the following three methods:
   * - hltInitialize(); this method must contain the code to be executed during the initialization phase,
   *   before starting a new run; example: memory allocation for static objects;
   * - hltExecute(); this method must contain the code to be executed on each output TE passed to the
   *   algorithm; this is the core part of the algorithm, taking care of forming a decision for the processed TE
   *   and returning a boolean argument (false by default) that states if the TE has validated the hypotesis; 
   * - hltFinalize(); this method must contain the code to be executed during the finalization phase,
   *   after the run has been stopped; example: freeing the memory allocated to static objects during the
   *   initialization.
   *
   * Some general implementation guidelines that developers should follow, e.g. regarding features handling
   * or RoIDescriptors usage, can be found in HLT::FexAlgo class documentation.
   */

  class HypoAlgo : public Algo {

  public:

    /**
     * @brief Constructor; gets the algorithm name and service locator as arguments.
     */
    HypoAlgo(const std::string& name, ISvcLocator* pSvcLocator);


    /**
     * @brief Destructor.
     */
    virtual ~HypoAlgo() { }
        
    /** 
     * @brief Method implementing the algorithmic code, executed once per input TE.
     * @return HLT::ErrorCode for the execution of the algorithm.
     * @param pointer to the output TE, to be updated during the execution of the algorithm;
     * @param boolean variable set to true if the TE has been selected by the Hypo.
     *
     * This is the core method of an Hypo algorithm, where developers must put all the algorithmic
     * intelligence needed to implement the selection. The algorith is executed on a single output TE,
     * for which this method must correctly for set the corresponding boolean result for the hypotesis
     * validation.
     */
    virtual HLT::ErrorCode hltExecute(const TriggerElement* outputTE, bool& pass) = 0;

    virtual bool reset() final { return Algo::reset(); }

  protected:

    /**
     * @brief Accessor method for the algorithm rerun flag.
     * @return value of the re-run flag for this algorithm.
     *
     * Method used by the Steering to determine whether this algorithm should re-run every time
     * the trigger is re-run; in this case, an hypotesis algorithm is always re-run, except for those cases
     * in which its selection is disabled.
     */
    virtual bool runAlgo() const { return !m_isNoSelection; }


    template<class T> HLT::ErrorCode attachFeature(const TriggerElement* te, T* feature, 
						   const std::string& label = "") {
      return Algo::attachFeature(const_cast<TriggerElement*>(te), feature, label);
    }
    
    
    /**
     * @brief attached TrigPassBits object to the outputTE
     **/
    [[deprecated("use xAOD::TrigPassBits instead")]]
    HLT::ErrorCode attachBits(const TriggerElement* outputTE, const TrigPassBits*, const std::string& label="passbits");

    /**
     * @brief attach TrigPassFlags object to the outputTE
     **/
    [[deprecated("no longer supported")]]
    HLT::ErrorCode attachFlags(const TriggerElement* outputTE, const TrigPassFlags*, const std::string& label="passflags");


  private:

    /**
     * @brief Method implementing the processing of an output TE.
     * @return HLT::ErrorCode for TE processing.
     * @param inputTEs vector of input TEs;
     * @param outputTE output TE pointer.
     *
     * This method performs the processing of a vector of input TEs and an output TE. In this case, this means: call
     * the user-implemented hltExecute method for the input/output TEs; set the activation status of the output TE
     * according to the boolean decision produced by hltExecute.
     */
    HLT::ErrorCode plugin_processTE(TEConstVec& inputTEs, TriggerElement* outputTE);   
    bool m_forceAccept;

    virtual HLT::ErrorCode createAndProcessTEs(std::vector<unsigned int>& inputTypes, 
					       unsigned int outputType,
					       TEVec& outputTEs, 
					       const std::vector<unsigned int>& topologicalStartFrom);

  };

} // end namespace
#endif
