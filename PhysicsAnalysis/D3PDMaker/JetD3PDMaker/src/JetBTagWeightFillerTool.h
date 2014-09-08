// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetBTagWeightFillerTool.h
 * @author Jet Goodson <jgoodson@cern.ch>
 * @date January, 2010
 * @brief Block filler tool for Jet BTag weight related quantities
 * @ This is only tempory until this tool or equivalent gets put into JetD3PDMaker.
 *@ At that point it will be removed from JetD3PDMaker
 */

#ifndef JetD3PDMAKER_JETBTAGWEIGHTFILLERTOOL_H
#define JetD3PDMAKER_JETBTAGWEIGHTFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


/**
 * @brief Block filler tool for Jet BTag.
 */
class JetBTagWeightFillerTool
  : public BlockFillerTool<Jet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetBTagWeightFillerTool (const std::string& type,
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
  virtual StatusCode fill (const Jet& p);
  
  
private:

  double* m_flavorweight; //generic weight, which one is actually recovered depends on key given, just like higher level code --- or, if all are selected, this one fills the baseline tag weight
    
  double* m_TrackCounting2D;
  double* m_JetProb;
  double* m_IP1D;
  double* m_IP2D;
  double* m_IP3D;
  double* m_SV0;
  double* m_SV1;
  double* m_SV2;
  double* m_JetFitterTag;
  double* m_JetFitterCOMB;
  double* m_JetFitterTagNN;
  double* m_JetFitterCOMBNN;
  double* m_SoftMuonTag;
  double* m_SoftElectronTag;
  double* m_IP3DSV1;



  bool m_doAllTags;
  std::string m_tagKey;
};


} // namespace D3PD


#endif // not JetD3PDMAKER_JETBTAGWEIGHTFILLERTOOL_H
