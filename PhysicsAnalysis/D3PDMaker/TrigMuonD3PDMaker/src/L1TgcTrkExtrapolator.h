/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef D3PD_L1TgcTrkExtrapolator_h
#define D3PD_L1TgcTrkExtrapolator_h

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <string>

namespace Rec { class TrackParticle; }
namespace Trk { class IExtrapolator; }
namespace Trk { class TrackStateOnSurface; }

namespace CLHEP { class Hep2Vector; }
namespace CLHEP { class Hep3Vector; }

static const InterfaceID IID_L1TgcTrkExtrapolator("L1TgcTrkExtrapolator", 1, 0);

namespace D3PD {
  class L1TgcTrkExtrapolator : public AthAlgTool
  {
  public:
    
    L1TgcTrkExtrapolator(const std::string& type,
			 const std::string& name,
			 const IInterface* parent);
    ~L1TgcTrkExtrapolator();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    enum TargetDetector { UNDEF, TGC, RPC };
    
    static const InterfaceID& interfaceID() { return IID_L1TgcTrkExtrapolator; };
    
    bool extrapolateFromMDT(const Rec::TrackParticle* trackParticle,
			    const Amg::Vector3D& pos,
			    const int detector,
			    Amg::Vector2D& eta,
			    Amg::Vector2D& phi,
			    Amg::Vector3D& mom) const;
    
    
  private:
    const Trk::TrackParameters*
      extrapolateToTGC(const Trk::TrackStateOnSurface* tsos,
                       const Amg::Vector3D& pos,
		       Amg::Vector2D& distance) const;
    const Trk::TrackParameters*
      extrapolateToRPC(const Trk::TrackStateOnSurface* tsos,
                       const Amg::Vector3D& pos,
		       Amg::Vector2D& distance) const;
    double getError(const std::vector<double>& inputVec) const;
    
    // tool handles
    ToolHandle<Trk::IExtrapolator> m_extrapolator;
    
    double m_endcapPivotPlaneMinimumRadius;
    double m_endcapPivotPlaneMaximumRadius;
    double m_barrelPivotPlaneHalfLength;

  };
}

#endif
/* eof */
