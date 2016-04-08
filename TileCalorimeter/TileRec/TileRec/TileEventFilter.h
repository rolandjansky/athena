/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEREC_TILETRIGGERTYPEFILTER
#define TILEREC_TILETRIGGERTYPEFILTER
/**
 * @file TileEventFilter.h
 * @brief class definition for TileEventFilter
 */

/**
 * @class TileEventFilter
 * @brief Simple Filter algorithm which filter events using Trigger Type
 */
            
#include "AthenaBaseComps/AthFilterAlgorithm.h"

#include <vector> 

class string;
class ISvcLocator;
     
class TileEventFilter : public AthFilterAlgorithm {
 public:
  TileEventFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TileEventFilter();

  virtual StatusCode initialize(); 
  virtual StatusCode execute();
  virtual StatusCode finalize();
 private:
  int m_nProcessedEvents;
  int m_nAcceptedEvents;
  std::vector<uint32_t> m_triggerTypes;
};
#endif
