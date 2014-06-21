/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUD3PDMAKER_TRACKCALOPARAMSFILLERTOOL_H
#define TAUD3PDMAKER_TRACKCALOPARAMSFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrkParameters/TrackParameters.h"

namespace Rec { class TrackParticle; }
namespace Trk { class Track; }
class IExtrapolateToCaloTool;
class CaloDepthTool;

namespace D3PD {


/**
 * @brief Block filler tool for tau PID.
 *
 */
    class TrackCaloParamsFillerTool
    : public BlockFillerTool<Rec::TrackParticle>
    {
    public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    TrackCaloParamsFillerTool (const std::string& type,
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
    virtual StatusCode fill (const Rec::TrackParticle& p);


    private:
        ToolHandle<IExtrapolateToCaloTool> m_extrapolateToCalo;
        ToolHandle<CaloDepthTool>       m_calodepth;

    /// Variables from each stored discriminant output
    float* m_exToCalo_eta;
    float* m_exToCalo_phi;


    const Trk::TrackParameters* extrapolateToCaloEM1(const Trk::Track* track);

    };


} // namespace D3PD


#endif // not TAUD3PDMAKER_TRACKCALOPARAMSFILLERTOOL_H

