// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef BOOSTEDTOPD3PDMAKER_MINIISOLATIONELECTRONFILLERTOOL_H
#define BOOSTEDTOPD3PDMAKER_MINIISOLATIONELECTRONFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "EventKernel/INavigable4Momentum.h"

//namespace Rec {
//class TrackParticle;
//}

namespace Reco{
  class ITrackToVertex;
}

//class IIsolationTool;
class IEMTrackIsolationTool;
class egamma;


namespace D3PD {
  /**
   * @brief Fill track isolation variables.
   */
class MiniIsolationElectronFillerTool
  : public BlockFillerTool<egamma>
  {
  public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    MiniIsolationElectronFillerTool (const std::string& type,
			     const std::string& name,
			     const IInterface* parent);


    // Standard Gaudi initialize method.
    StatusCode initialize();


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
    virtual StatusCode fill (const egamma& electron);


  private:
    /// Paramter: Isolation cone size.
    //float m_coneSize;

    ToolHandle<IEMTrackIsolationTool> m_EMTrackIsolationTool;

    ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;

    /// Variable: Tracking isolation parameter.
    float* m_ptsum;
    float* m_ptsum_2;
    /// Variable: Calorimeter isolation parameter.
    float* m_nTracks;
    float* m_nTracks_2;

    double m_ktdist;
    double m_mindR;
    double m_maxdR;
    double m_maxdR_2;

    std::string m_TracksContainer;
    std::string m_VxContainer;


  };


} // namespace D3PD


#endif // not BOOSTEDTOPD3PDMAKER_MINIISOLATIONELECTRONFILLERTOOL_H
