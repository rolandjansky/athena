/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOLCONFIG_RANGEFILTER_H
#define TRIGJETHYPOTOOLCONFIG_RANGEFILTER_H

#include "ITrigHypoJetVectorFilterConfig.h"
#include "AthenaBaseComps/AthAlgTool.h"

/*
 * range filter - orders jets (currently by pt), and returns
 * iterators to the begin, end positions of the ordered container.
 */

class TrigJetHypoToolConfig_rangefilter:
  public extends<AthAlgTool, ITrigHypoJetVectorFilterConfig> {
  
public:
  
  TrigJetHypoToolConfig_rangefilter(const std::string& type,
				    const std::string& name,
				    const IInterface* parent);
  
  virtual StatusCode initialize() override;
  virtual FilterPtr getHypoJetVectorFilter() const override;
  
 private:

  Gaudi::Property<std::size_t>
  m_begin{this, "begin", {0u}, "first position in range"};

  Gaudi::Property<std::size_t>
  m_end{this, "end", {0u}, "end (last + 1)  position in range"};
  
  StatusCode checkVals()  const;
 
};
#endif
