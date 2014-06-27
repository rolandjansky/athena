/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PhotonPVFillerTool.cxx
 * @author Jean-Francois Marchand <jean-francois.marchand@cern.ch>
 * @date April 2011
 * @brief Block filler tool for primary vertex kinematics, from attached tracks. Adapted from TrackD3PDMaker/src/VertexKineFillerTool.cxx
 */


#include "PhotonPVFillerTool.h"
#include "Particle/TrackParticle.h"
#include "AthenaKernel/errorcheck.h"


using HepGeom::Vector3D;


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PhotonPVFillerTool::PhotonPVFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : BlockFillerTool<Trk::VxCandidate> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode PhotonPVFillerTool::book()
{
  CHECK( addVariable ("px",            m_px          ) );
  CHECK( addVariable ("py",            m_py          ) );
  CHECK( addVariable ("pz",            m_pz          ) );
  CHECK( addVariable ("E",             m_E           ) );
  CHECK( addVariable ("m",             m_m           ) );
  CHECK( addVariable ("nTracks",       m_nTracks     ) );
  CHECK( addVariable ("sumPt",         m_sumPt       ) );
  CHECK( addVariable ("sumPt2",        m_sumPt2      ) );
  CHECK( addVariable ("type",          m_type        ) );
  CHECK( addVariable ("missPt",        m_ptmiss      ) );
  CHECK( addVariable ("meanPt",        m_meanpt      ) );
  CHECK( addVariable ("nTracks_05GeV", m_nTracks05GeV) );
  CHECK( addVariable ("nTracks_07GeV", m_nTracks07GeV) );
  CHECK( addVariable ("nTracks_1GeV",  m_nTracks1GeV ) );
  CHECK( addVariable ("nTracks_2GeV",  m_nTracks2GeV ) );

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
StatusCode PhotonPVFillerTool::fill (const Trk::VxCandidate& p)
{
  const std::vector<Trk::VxTrackAtVertex*>* tracks = p.vxTrackAtVertex();
  std::vector<Trk::VxTrackAtVertex*>::const_iterator begintracks = tracks->begin();
  std::vector<Trk::VxTrackAtVertex*>::const_iterator endtracks = tracks->end();

  float sumpx = 0.0; 
  float sumpy = 0.0; 
  float sumpz = 0.0; 
  float sumpp = 0.0; 
  float sumpt = 0.0;
  float sumpt2 = 0.0;

  Amg::Vector3D vecSumMom(0,0,0);
  int Ntracks = 0;
  int Ntracks_05GeV = 0;
  int Ntracks_07GeV = 0;
  int Ntracks_1GeV = 0;
  int Ntracks_2GeV = 0;

  for (std::vector<Trk::VxTrackAtVertex*>::const_iterator i = begintracks; i!=endtracks; i++) {

    const Trk::Perigee* perigee = 0;
    if ( (*i)->perigeeAtVertex() )
      perigee = (const Trk::Perigee*) (*i)->perigeeAtVertex();
    else
      perigee = (const Trk::Perigee*) (*i)->initialPerigee();

    sumpx += perigee->momentum().x();
    sumpy += perigee->momentum().y();
    sumpz += perigee->momentum().z();
    sumpp += perigee->momentum().mag();
    sumpt += sqrt(perigee->momentum().x()*perigee->momentum().x()+perigee->momentum().y()*perigee->momentum().y()); 
    sumpt2 += perigee->momentum().x()*perigee->momentum().x()+perigee->momentum().y()*perigee->momentum().y();

    vecSumMom += perigee->momentum();
    if (perigee->momentum().perp()>500)  Ntracks_05GeV += 1;
    if (perigee->momentum().perp()>700)  Ntracks_07GeV += 1;
    if (perigee->momentum().perp()>1000) Ntracks_1GeV += 1;
    if (perigee->momentum().perp()>2000) Ntracks_2GeV += 1;

 }
  
  float vtxmass = sumpp*sumpp-sumpx*sumpx-sumpy*sumpy-sumpz*sumpz;
  if (vtxmass>0.0)
    vtxmass = std::sqrt(vtxmass);
  else
    vtxmass = 0.0;

  Ntracks = p.vxTrackAtVertex()->size();
  float meanPt = (Ntracks > 0 ? sumpt*1./Ntracks:0);
  float ptmiss = vecSumMom.perp();

  *m_px           = sumpx;
  *m_py           = sumpy;
  *m_pz           = sumpz;
  *m_E            = sumpp;
  *m_m            = vtxmass;
  *m_nTracks      = p.vxTrackAtVertex()->size();
  *m_sumPt        = sumpt;
  *m_sumPt2       = sumpt2;
  *m_type         = (int)p.vertexType();
  *m_ptmiss       = ptmiss;
  *m_meanpt       = meanPt;
  *m_nTracks05GeV = Ntracks_05GeV;
  *m_nTracks07GeV = Ntracks_05GeV;
  *m_nTracks1GeV  = Ntracks_1GeV;
  *m_nTracks2GeV  = Ntracks_2GeV;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
