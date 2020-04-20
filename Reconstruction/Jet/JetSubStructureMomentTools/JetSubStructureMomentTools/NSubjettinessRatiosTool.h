/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ----------------------------------------------------------------
 * The default behavior of this tool is to use alpha = 1.0, but multiple
 * values of alpha can be used simultaneously. The property AlphaList
 * should be passed a list of floats. Values of < 0 or > 10 may result
 * in problematic output variable names and all values will be rounded
 * to the nearest 0.1. No suffix will be added to the outputs for alpha = 1.0
 * and for other values a suffix of _AlphaN will be added where N = int(10*alpha).
 *
 * The DoDichroic option adds dichroic N-subjettiness ratios described in
 * https://arxiv.org/abs/1612.03917
 * ----------------------------------------------------------------
 */

#ifndef jetsubstructuremomenttools_nsubjetinessratiostool_header
#define jetsubstructuremomenttools_nsubjetinessratiostool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"
#include "JetSubStructureMomentTools/NSubjettinessHelper.h"

class NSubjettinessRatiosTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(NSubjettinessRatiosTool, IJetModifier)

    public:
      // Constructor and destructor
      NSubjettinessRatiosTool(std::string name);

      StatusCode initialize();

      int modifyJet(xAOD::Jet &jet) const;

    private:
      std::vector<float> m_rawAlphaVals; /// Vector of input values before cleaning
      std::vector<float> m_alphaVals; /// Local vector for cleaned up inputs
      bool m_doDichroic;
};

#endif
