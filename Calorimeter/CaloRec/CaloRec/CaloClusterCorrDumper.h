/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOCLUSTERCORRDUMPER
#define CALOREC_CALOCLUSTERCORRDUMPER

/**
 * @class CaloClusterCorrDumper
 * @author Walter Lampl <walter.lampl@cern.ch>
 * @date March, 28th 2006
 *
 * This algorithm has only an @c initialize() and a @c finalize() method.
 * It instantiates all ClusterCorrection Tools given by jobOptions. The
 * cluster correction constants (@c ToolConstants) are expected to be
 * initialized by job options. In the @c finalize method, this algorithm
 * records these objects to the detector store so that they can be 
 * streamed out to a POOL file. 
 */



#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloRec/ToolWithConstantsMixin.h"
#include <vector>
#include <string>

class StoreGateSvc;

class CaloClusterCorrDumper : public AthAlgorithm
{

 public:

  CaloClusterCorrDumper(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CaloClusterCorrDumper();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
 private:

  //std::string m_inlineFolder;

  /** @brief The list of tool names (jobOptions)*/
  std::vector<std::string> m_correctionToolNames;

  /** @brief the actual list of tools corresponding to above names */
  std::vector<CaloRec::ToolWithConstantsMixin*>  m_correctionTools; 


  /** @brief Name of the text file where to store the constants
   */
  std::string m_fileName;

};

#endif // CALOREC_CALOCLUSTERCORRDUMPER
