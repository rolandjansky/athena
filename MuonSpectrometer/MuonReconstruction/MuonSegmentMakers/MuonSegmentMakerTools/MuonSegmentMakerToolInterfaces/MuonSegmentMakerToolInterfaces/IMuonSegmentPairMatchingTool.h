/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTPAIRMATCHINGTOOL_H
#define MUON_IMUONSEGMENTPAIRMATCHINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"

static const InterfaceID IID_IMuonSegmentPairMatchingTool("Muon::IMuonSegmentPairMatchingTool",1,0);


namespace Muon {
  
  class MuonSegment;

  /**
     @brief tool to match segments

  */
  class IMuonSegmentPairMatchingTool : virtual public IAlgTool {
  public:

    struct SegmentMatchResult {
      SegmentMatchResult() : chid_a(), chid_b(), phiSector_a(-99999),phiSector_b(-99999),
			     deltaTheta_a(-99999.), deltaTheta_b(-99999.), deltaTheta(-99999.), 
			     angleAC(-99999.), angleBC(-99999.), angleAB(-99999.),deltaPhipos(-99999.), deltaPhidir(-99999.), phiposerr_a(-99999.), phidirerr_a(-99999.), phiposerr_b(-99999.), phidirerr_b(-99999.), shorttube_a(-999999.), shorttube_b(-999999.), matchFlag(false) {}
      Identifier chid_a;
      Identifier chid_b;
      int phiSector_a;
      int phiSector_b;
      double deltaTheta_a;
      double deltaTheta_b;
      double deltaTheta;
      double angleAC;
      double angleBC;
      double angleAB;
      double deltaPhipos;
      double deltaPhidir;
      double phiposerr_a;
      double phidirerr_a;
      double phiposerr_b;
      double phidirerr_b;
      double shorttube_a;
      double shorttube_b;
      bool matchFlag; // false if anything went wrong with the calculation
    };
  
  public:
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_IMuonSegmentPairMatchingTool; }

    /** @brief performance match and return result */
    virtual SegmentMatchResult matchResult( const MuonSegment& seg1, const MuonSegment& seg2 ) const = 0; 

  };

}

#endif
