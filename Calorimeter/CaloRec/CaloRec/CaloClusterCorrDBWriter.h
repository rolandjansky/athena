/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOCLUSTERCORRDBWRITER
#define CALOREC_CALOCLUSTERCORRDBWRITER

/**
 * @class CaloClusterCorrDBWriter
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
#include "GaudiKernel/ToolHandle.h"
#include "CaloRec/ToolWithConstantsMixin.h"
#include "CaloRec/Blob2ToolConstants.h"
#include <vector>
#include <string>


class StoreGateSvc;

class CaloClusterCorrDBWriter : public AthAlgorithm
{

 public:

  CaloClusterCorrDBWriter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CaloClusterCorrDBWriter();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
 private:

  /** @brief The list of tool names (jobOptions)*/
  std::vector<std::string> m_correctionToolNames;

  /** @brief Key for the DetectorStore (jobOptions) 
   * The ToolConstants will be recorded with this key.
   */
  std::string m_key;

  std::string m_inlineFolder;

  /** @brief the actual list of tools corresponding to above names */
  std::vector<CaloRec::ToolWithConstantsMixin*>  m_correctionTools; 

  ToolHandle<Blob2ToolConstants> m_blobTool;
  
};

#endif // CALOREC_CALOCLUSTERCORRDBWRITER
