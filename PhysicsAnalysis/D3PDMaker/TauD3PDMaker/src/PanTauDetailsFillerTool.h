// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PanTauDetailsFillerTool.h 528845 2012-12-04 15:07:38Z tibristo $
/**
 * @file D3PDMaker/TauD3PDMaker/src/PanTauDetailsFillerTool.h
 * @author Sebastian Fleischmann
 * @date Jul, 2010
 * @brief Block filler tool for PanTauDetails variables.
 */

#ifndef TAUD3PDMAKER_PANTAUDETAILSFILLERTOOL_H
#define TAUD3PDMAKER_PANTAUDETAILSFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"

namespace Analysis { class TauJet; }

class TRandom2;

namespace D3PD {


/**
 * @brief Block filler tool for PanTau details.
 *
 */
class PanTauDetailsFillerTool
  : public BlockFillerTool<Analysis::TauJet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PanTauDetailsFillerTool (const std::string& type,
                     const std::string& name,
                     const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const Analysis::TauJet& p);

  virtual StatusCode finalize();

private:

    TRandom2* m_randomNumbers;   //!< random number generator for smearing of default values.

    std::vector< float* >        m_seedFeatureBranches;  // TMVA uses Float_t intenally, so float should be sufficient and
                                                        // may avoid trouble with inf
    std::vector<std::string>    m_featureNames;
    std::vector<float>          m_featureDefaultValues;
    std::vector<float>          m_featureDefaultValueSmearingSigma;
    bool                        m_smearDefaultValues;

};


} // namespace D3PD


#endif // PANTAUANALYSIS_PANTAUDETAILSFILLERTOOL_H
