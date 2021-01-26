// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
* @file MissingETD3PDMaker/src/MissingETJetsInfoFillerTool.h
 * @author Jet Goodson <jgoodson@cern.ch>
 * @date Oct, 2009
 * @brief Block filler tool for Missing ET Jets cleaning variable objects.
 */


#ifndef MISSINGETD3PPDMAKER_MISSINGETJETSINFOFILLERTOOL_H
#define MISSINGETD3PPDMAKER_MISSINGETJETSINFOFILLERTOOL_H


#include "JetVariablesTool.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"	
#include "Particle/TrackParticleContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"

class MissingET;


namespace D3PD {


/**
 * @brief Block filler tool for Missing ET objects.
 */
class MissingETJetsInfoFillerTool
  : public BlockFillerTool<JetCollection>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MissingETJetsInfoFillerTool (const std::string& type,
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
  virtual StatusCode fill (const JetCollection& jets);
  

private:
  std::string m_trackContainerKey;

  /// Variable JetPtWeightedEventEMFraction
  ToolHandle<JetVariablesTool> m_jetVariables;

  float* m_ev_emf;
  float* m_ev_jetsize;
  float* m_leadingJetEt;
  float* m_leadingJetEta;
  float* m_ev_numtrks;
 
};


} // namespace D3PD


#endif // not MISSINGETD3PPDMAKER_MISSINGETJETSINFOFILLERTOOL_H
