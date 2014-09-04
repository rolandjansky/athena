/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINTERFACES_TECreateAlgo_H
#define TRIGINTERFACES_TECreateAlgo_H

#include "TrigInterfaces/Algo.h"
#include "TrigNavigation/RoICacheHelper.h"
#include "TrigSteeringEvent/RobRequestInfo.h"
#include <sstream>

class StoreGateSvc;



namespace HLT
{
  class ComboIteratorBase;

  /**
   * @brief Class providing helper methods to create output TEs.
   *
   * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
   * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
   *
   * This class implements the helper methods used by HLT algorithms to create output TEs.
   */

  class TECreateAlgo : public Algo, public RoICacheHelper {

  public:

    /**
     * @brief Constructor; gets the algorithm name and service locator as arguments.
     */
    TECreateAlgo(const std::string& name, ISvcLocator* pSvcLocator)
      : Algo(name, pSvcLocator), m_inTECreateMode(false) { }

    /**
     * @brief Destructor.
     */
    ~TECreateAlgo() { }

    /**
     * @brief Method performing algorithm reset.
     *
     */
    virtual bool reset();

    // Needed to call the private createAndProcessTEs
    friend class Sequence;

  protected:

    /**
     * @brief Method performing the Initialize transition.
     * @return StatusCode for the Initialize process.
     *
     * Case the base class Initialize method.
     */
    StatusCode initialize();

    /**
     * @brief Common method to register ROB requests to be called by
     * the steering. Implementated in ComboAlgo, FexAlgo, and AllTEAlgo
     */
    virtual HLT::ErrorCode processRobRequests(const std::vector< unsigned int >& inputs ) = 0;

  private:


    /**
     * @brief method implementing processing when algo is not first in a sequence thus not responsible for TEs creation
     * This method is reimplementation of Algo::processTEs because of caching
     * @param outputTEs vector of output for which the algorithm should run
     * @return HLT::ErrorCode of the execution (most severe) 
     */       
    virtual HLT::ErrorCode processTEs(TEVec& outputTEs);

    /**
     * @brief Method used to attach a new feature to a TE.
     * @return HLT::ErrorCode for feature attaching.
     * @param te pointer to the TE to which the feature must be attached.
     * @param feature pointer to the feature to be attached.
     * @param key store key used to identify the attached feature.
     * @param label label used to identify the attached feature.
     * @param mm memory management for the HLT::Navigation.
     *
     * This method enables to attach a new feature to a TE.
     */
    template<class T> HLT::ErrorCode attachFeature(TriggerElement* te, T* feature,
						   std::string& key,
						   const std::string& label,
						   HLT::Navigation::MemoryManagement mm);
  protected:
    
    /**
     * @brief Method used to get a new ComboIterator over input TEs.
     * @return the requested newly created ComboIterator.
     * @param inputs list of input TE types to be combined.
     * @param onlyActive option specifying if only active TEs must be used; set to true by default.
     *
     * This method is used to create a new ComboIterator over a list of input TEs. It is possible to
     * decide whether only active TEs will be used to perform combinations.
     */
    ComboIteratorBase* newComboIterator(std::vector<unsigned int>& inputs,
					bool onlyActive = true);

    /**
     * @brief Method used to get a new ComboIterator over input TEs.
     * @return the requested newly created ComboIterator.
     * @param inputs list of input TE types to be combined.
     * @param topologicalStartFrom topological types from which to start.
     * @param onlyActive option specifying if only active TEs must be used; set to true by default.
     *
     * This method is used to create a new ComboIterator over a list of input TEs, starting from
     * a list of topological TEs, used for refinement of topological objects. It is possible to
     * decide whether only active TEs will be used to perform combinations.
     */
    ComboIteratorBase* newComboIterator(std::vector<unsigned int>& inputs,
					const std::vector<unsigned int>& topologicalStartFrom,
					bool onlyActive = true);

    /**
     * @brief Method used to attach a new feature to a TE.
     * @return HLT::ErrorCode for feature attaching.
     * @param te pointer to the TE to which the feature must be attached.
     * @param feature pointer to the feature to be attached.
     * @param label label used to identify the attached feature.
     *
     * This method enables to attach a new feature to a TE; this is the method recommended for
     * attaching features in case no StoreGate storage is required.
     */
    template<class T> HLT::ErrorCode attachFeature(TriggerElement* te, T* feature, 
						   const std::string& label = "");
    
    /**
     * @brief Method used to store a new feature and attach it to a TE.
     * @return HLT::ErrorCode for feature recording and attaching.
     * @param te pointer to the TE to which the feature must be attached.
     * @param feature pointer to the feature to be attached.
     * @param key store key used to identify the attached feature.
     * @param label label used to identify the attached feature.
     *
     * This method enables to store a new feature and attach it to a TE.
     */
    template<class T> HLT::ErrorCode recordAndAttachFeature(TriggerElement* te, T* feature,
							    std::string& key,
							    const std::string& label = "");

    /**
     * @brief Method used to attach an already stored feature to a TE.
     * @return HLT::ErrorCode for feature attaching.
     * @param te pointer to the TE to which the feature must be attached.
     * @param feature pointer to the feature to be attached.
     * @param key store key used to identify the attached feature.
     * @param label label used to identify the attached feature.
     *
     * This method enables to store a new feature and attach it to a TE.
     */
    template<class T> HLT::ErrorCode reAttachFeature(TriggerElement* te, T* feature,
						     std::string& key,
						     const std::string& label = "");

    bool m_inTECreateMode; //!< flag for calls to ExecMonitor()

  private:

    //    TrigTimer* m_totalTimeWithCreate;
  };
  
  template<class T> HLT::ErrorCode 
    TECreateAlgo::attachFeature(TriggerElement* te, T* feature,	std::string& key,
				const std::string& label, 
				HLT::Navigation::MemoryManagement mm) {    
    if (!feature) {
      msg() << MSG::FATAL << "Cannot attach null feature!" << endreq;
      return HLT::FATAL;
    }
    
    if (!m_config || !m_config->getNavigation() ||
	!m_config->getNavigation()->attachFeature(te, feature, mm, key, label)) {
      msg() << MSG::ERROR << "Failed to attach feature!" << endreq;
      return HLT::NAV_ERROR;
    }
    
    return HLT::OK;
  }
  
  template<class T> HLT::ErrorCode 
    TECreateAlgo::attachFeature(TriggerElement* te, T* feature,	const std::string& label) {

    // not used
    std::string key = "";
    return attachFeature(te, feature, key, label, HLT::Navigation::ObjectCreatedByNew);
  }

  template<class T> HLT::ErrorCode 
    TECreateAlgo::recordAndAttachFeature(TriggerElement* te, T* feature, std::string& key,
					 const std::string& label) { 

    std::string aliasKey = key;
    HLT::ErrorCode status = attachFeature(te, feature, key, label, HLT::Navigation::ObjectToStoreGate);
    if (status != HLT::OK) return status;

    if (aliasKey == "") return HLT::OK;
    if (!store() || store()->setAlias(feature, aliasKey).isFailure()) 
      return HLT::NAV_ERROR;

    return HLT::OK;
  }

  template<class T> HLT::ErrorCode 
    TECreateAlgo::reAttachFeature(TriggerElement* te, T* feature, std::string& key,
				  const std::string& label) {
    
    return attachFeature(te, feature, key, label, HLT::Navigation::ObjectInStoreGate);
  }

} // end namespace

#endif
