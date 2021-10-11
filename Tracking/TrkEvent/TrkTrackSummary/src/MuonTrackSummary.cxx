/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackSummary/MuonTrackSummary.h"

#include "GaudiKernel/MsgStream.h"
#include <iostream>

Trk::MuonTrackSummary::MuonTrackSummary()
  : m_nscatterers(0)
  , m_npseudoMeasurements(0)
{}

Trk::MuonTrackSummary::~MuonTrackSummary() {}

unsigned int
Trk::MuonTrackSummary::netaHits() const
{

  unsigned int count = 0;
  std::vector<ChamberHitSummary>::const_iterator it =
    m_chamberHitSummary.begin();
  std::vector<ChamberHitSummary>::const_iterator it_end =
    m_chamberHitSummary.end();
  for (; it != it_end; ++it)
    count += it->netaHits();
  return count;
}

unsigned int
Trk::MuonTrackSummary::nphiHits() const
{
  unsigned int count = 0;
  std::vector<ChamberHitSummary>::const_iterator it =
    m_chamberHitSummary.begin();
  std::vector<ChamberHitSummary>::const_iterator it_end =
    m_chamberHitSummary.end();
  for (; it != it_end; ++it)
    count += it->nphiHits();
  return count;
}

unsigned int
Trk::MuonTrackSummary::nholes() const
{
  unsigned int count = 0;
  std::vector<ChamberHitSummary>::const_iterator it =
    m_chamberHitSummary.begin();
  std::vector<ChamberHitSummary>::const_iterator it_end =
    m_chamberHitSummary.end();
  for (; it != it_end; ++it)
    count += it->nholes();
  return count;
}

unsigned int
Trk::MuonTrackSummary::noutliers() const
{
  unsigned int count = 0;
  std::vector<ChamberHitSummary>::const_iterator it =
    m_chamberHitSummary.begin();
  std::vector<ChamberHitSummary>::const_iterator it_end =
    m_chamberHitSummary.end();
  for (; it != it_end; ++it)
    count += it->noutliers();
  return count;
}

unsigned int
Trk::MuonTrackSummary::ncloseHits() const
{
  unsigned int count = 0;
  std::vector<ChamberHitSummary>::const_iterator it =
    m_chamberHitSummary.begin();
  std::vector<ChamberHitSummary>::const_iterator it_end =
    m_chamberHitSummary.end();
  for (; it != it_end; ++it)
    count += it->ncloseHits();
  return count;
}

std::ostream&
Trk::operator<<(std::ostream& out, const MuonTrackSummary& trackSum)
{
  out << "MuonTrackSummary: npseudo " << trackSum.npseudoMeasurements()
      << " nscat " << trackSum.nscatterers() << " number of chambers "
      << trackSum.chamberHitSummary().size() << std::endl;
  std::vector<MuonTrackSummary::ChamberHitSummary>::const_iterator it =
    trackSum.chamberHitSummary().begin();
  std::vector<MuonTrackSummary::ChamberHitSummary>::const_iterator it_end =
    trackSum.chamberHitSummary().end();

  for (; it != it_end; ++it) {
    const MuonTrackSummary::ChamberHitSummary& chSum = *it;
    const MuonTrackSummary::ChamberHitSummary::Projection& etaP =
      chSum.etaProjection();
    const MuonTrackSummary::ChamberHitSummary::Projection& phiP =
      chSum.phiProjection();
    out << " Chamber " << chSum.chamberId() << " isMdt " << chSum.isMdt()
        << std::endl
        << " First projection: Hits " << etaP.nhits << " holes " << etaP.nholes
        << " outlier " << etaP.noutliers << " deltas " << etaP.ndeltas
        << " close Hits " << etaP.ncloseHits << std::endl
        << " Second projection: Hits " << phiP.nhits << " holes " << phiP.nholes
        << " outlier " << phiP.noutliers << " deltas " << phiP.ndeltas
        << " close Hits " << phiP.ncloseHits << std::endl;
  }
  return out;
}

MsgStream&
Trk::operator<<(MsgStream& out, const MuonTrackSummary& trackSum)
{
  out << "MuonTrackSummary: Hits: eta " << trackSum.netaHits() << " phi "
      << trackSum.nphiHits() << " holes " << trackSum.nholes() << " outliers "
      << trackSum.noutliers() << " close hits " << trackSum.ncloseHits()
      << endmsg;
  if (out.level() <= MSG::VERBOSE) {
    std::vector<MuonTrackSummary::ChamberHitSummary>::const_iterator it =
      trackSum.chamberHitSummary().begin();
    std::vector<MuonTrackSummary::ChamberHitSummary>::const_iterator it_end =
      trackSum.chamberHitSummary().end();
    for (; it != it_end; ++it) {
      const MuonTrackSummary::ChamberHitSummary& chSum = *it;
      const MuonTrackSummary::ChamberHitSummary::Projection& etaP =
        chSum.etaProjection();
      const MuonTrackSummary::ChamberHitSummary::Projection& phiP =
        chSum.phiProjection();
      out << " Chamber " << chSum.chamberId() << " isMdt " << chSum.isMdt()
          << std::endl
          << " First projection: Hits " << etaP.nhits << " holes "
          << etaP.nholes << " outlier " << etaP.noutliers << " deltas "
          << etaP.ndeltas << " close Hits " << etaP.ncloseHits << std::endl
          << " Second projection: Hits " << phiP.nhits << " holes "
          << phiP.nholes << " outlier " << phiP.noutliers << " deltas "
          << phiP.ndeltas << " close Hits " << phiP.ncloseHits << std::endl;
    }
  }
  return out;
}
