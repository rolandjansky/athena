/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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



#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/IToolWithConstants.h"
#include "CaloRec/Blob2ToolConstants.h"
#include <vector>
#include <string>


class CaloClusterCorrDBWriter : public AthReentrantAlgorithm
{

 public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  
 private:
  /** @brief The list of tools */
  ToolHandleArray<IToolWithConstants> m_tools
  { this, "ClusterCorrectionTools", {}, "Cluster correction tools" };

  /** @brief Key for the DetectorStore (jobOptions) 
   * The ToolConstants will be recorded with this key.
   */
  StringProperty m_key
  { this, "key", "" };

  StringProperty m_inlineFolder
  { this, "COOLInlineFolder", "" };

  ToolHandle<Blob2ToolConstants> m_blobTool
  { this, "Blob2ToolConstants", "Blob2ToolConstants" };
};

#endif // CALOREC_CALOCLUSTERCORRDBWRITER
