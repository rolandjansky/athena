/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_ExtraTreeMuonFillerTool_h
#define MuonCalib_ExtraTreeMuonFillerTool_h

#include "MuonCalibExtraTreeAlg/ExtraTreeTrackFillerTool.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"

namespace Trk {
class IPropagator;
}

namespace MuonCalib {

class ExtraTreeMuonFillerTool: public ExtraTreeTrackFillerTool {
  public:
    ExtraTreeMuonFillerTool(const std::string &type, const std::string &name, const IInterface *parent);
    StatusCode initialize();
    StatusCode writeTracks(unsigned int &index);
		
  private:
    //name of the muon container - jo
    std::string m_muonContainer;
    //offset of the track author
    //	offset: SA track
    //	offsset+1: SA extrapolated
    //	offset+ 2: combined
    //	offset+3: statistically combined
    int m_authorOffset;
    //switches to control storing of hits
    bool m_hitsForSA, m_hitsForSAE, m_hitsForCombined, m_hitsForStatCombined;
    // edm helper tool
    ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelper {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };
    ToolHandle<Trk::IPropagator> m_propagator{this,"Propagator","Trk::StraightLinePropagator/MuonStraightLinePropagator"};
    inline StatusCode writeTrackParticle(const xAOD::TrackParticle *&part, bool /*isPrimaryAuthor*/, bool writeHits, unsigned int &index, int author);
    inline Trk::Track* createTaggedMuonTrack( const xAOD::Muon &muon ) const;
  };

}// namespace MuonCalib
#endif
