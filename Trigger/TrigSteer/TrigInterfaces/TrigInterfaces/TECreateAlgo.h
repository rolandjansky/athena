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

    

    bool m_inTECreateMode; //!< flag for calls to ExecMonitor()

  private:

    //    TrigTimer* m_totalTimeWithCreate;
  };
  

} // end namespace

#endif
