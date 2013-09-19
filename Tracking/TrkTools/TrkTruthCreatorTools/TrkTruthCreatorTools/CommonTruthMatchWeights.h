/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Code that is common to several track-truth matching tools.
 *
 * @author Andrei Gaponenko <agaponenko@lbl.gov> 
 */

#ifndef COMMONTRUTHMATCHWEIGHTS_H
#define COMMONTRUTHMATCHWEIGHTS_H

#include "AthenaBaseComps/AthAlgTool.h"

#include <vector>

namespace Trk {
  
  class CommonTruthMatchWeights : public AthAlgTool
  {
  public:
    CommonTruthMatchWeights(const std::string& type, const std::string& name, const IInterface* parent);

    virtual StatusCode initialize();    
    
  protected:
    // Subdetector weights
    std::vector<double> m_subDetWeights;
  };
  
} // end namespace Trk

#endif/*COMMONTRUTHMATCHWEIGHTS_H*/
