/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackCaloParamsFillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "Particle/TrackParticle.h"
#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
#include "CaloDetDescr/CaloDepthTool.h"


namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackCaloParamsFillerTool::TrackCaloParamsFillerTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : BlockFillerTool<Rec::TrackParticle> (type, name, parent),
    m_extrapolateToCalo("exToCalo"),
    m_calodepth("CaloDepthTool")
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode TrackCaloParamsFillerTool::book()
{
    CHECK( m_extrapolateToCalo.retrieve() );
    CHECK( m_calodepth.retrieve() );
    CHECK( addVariable ("exToCalo_eta", m_exToCalo_eta) );
    CHECK( addVariable ("exToCalo_phi", m_exToCalo_phi) );
    return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode TrackCaloParamsFillerTool::fill (const Rec::TrackParticle& p)
{
    *m_exToCalo_eta = -1111.;
    *m_exToCalo_phi = -1111.;

    const Trk::Track* track = p.originalTrack();
    if(track)
    {
        const Trk::TrackParameters* track_params = extrapolateToCaloEM1(track);
        if(track_params)
        {
            *m_exToCalo_eta = track_params->position().eta();
            *m_exToCalo_phi = track_params->position().phi();
        }
        else
        {
            ATH_MSG_ERROR("extrapolateToCaloEM1 returned 0");
        }
    }
    else
    {
        ATH_MSG_DEBUG("TrackParticle.originalTrack() returned 0.");
    }

    return StatusCode::SUCCESS;
}


const Trk::TrackParameters* TrackCaloParamsFillerTool::extrapolateToCaloEM1(const Trk::Track* track)
{
    const double offset = 0.0;

    // determine which CaloSample to extrapolate to
    // http://alxr.usatlas.bnl.gov/lxr/source/atlas/Reconstruction/RecoTools/TrackToCalo/src/ExtrapolateToCaloTool.cxx#492
    CaloCell_ID::CaloSample sample;
    double track_eta = track->perigeeParameters()->eta();
    double distbar = m_calodepth->deta(CaloCell_ID::EMB1, track_eta);
    double distec = m_calodepth->deta(CaloCell_ID::EME1, track_eta);
    ATH_MSG_DEBUG(" TrackTo ...Strip : for eta= " << track_eta << " dist to Barrel =" << distbar 
                  << " to endcap =" << distec);
    if (distbar < 0 ) sample = CaloCell_ID::EMB1;
    else if (distec < 0 ) sample = CaloCell_ID::EME1;
    else if ( distbar < distec) sample = CaloCell_ID::EMB1;
    else sample = CaloCell_ID::EME1;
    ATH_MSG_DEBUG( "  => will shoot in sample " << sample );

    const Trk::TrackParameters* track_params = m_extrapolateToCalo->extrapolate(
               *track, sample, offset, Trk::alongMomentum, Trk::pion );

    return track_params;
}


} // namespace D3PD
