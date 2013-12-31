// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file QcdD3PDMaker/src/MuonSpShowerFillerTool.h
 * @author Michiru Kaneda <Michiru.Kaneda@cern.ch>
 * @date Jan., 2011
 * @brief Block filler tool for MuonSpShower
 */

#ifndef QCDD3PDMAKER_MUONSPSHOWERFILLERTOOL_H
#define QCDD3PDMAKER_MUONSPSHOWERFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "StoreGate/StoreGateSvc.h"

namespace Rec{
class MuonSpShower;
}

namespace QcdD3PD {


class MuonSpShowerFillerTool
  : public D3PD::BlockFillerTool<Rec::MuonSpShower>

{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MuonSpShowerFillerTool (const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual StatusCode book ();
  virtual StatusCode fill (const Rec::MuonSpShower &p);


private:
  ServiceHandle<StoreGateSvc> m_storeGate;

  /// Variable: Position
  float* m_eta;
  float* m_phi;
  /// Variable: Hits
  unsigned short* m_trigHits;
  unsigned short* m_innerHits;
  unsigned short* m_middleHits;
  unsigned short* m_outerHits;
  /// Variable: Segments
  unsigned short* m_innerSegments;
  unsigned short* m_middleSegments;
  unsigned short* m_outerSegments;

  /// Property: Fill Position?
  bool m_doPosition;
  /// Property: Fill Hits?
  bool m_doHits;
  /// Property: Fill Segments?
  bool m_doSegments;

};


} // namespace QcdD3PD


#endif // not QCDD3PDMAKER_MUONSPSHOWERFILLERTOOL_H
