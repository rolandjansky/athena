/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTHITSUMMARYTOOL_H
#define MUON_IMUONSEGMENTHITSUMMARYTOOL_H

#include "GaudiKernel/IAlgTool.h"

#include "Identifier/Identifier.h"

static const InterfaceID IID_IMuonSegmentHitSummaryTool
    ("Muon::IMuonSegmentHitSummaryTool",1,0);

namespace Muon {
  class MuonSegment;

  /** Interface for tools providing segment hit summary information*/
  class IMuonSegmentHitSummaryTool : virtual public IAlgTool 
  {      
    public:
    // struct to allow us to count the number of trigger hits per gas gap in both projections
    struct EtaPhiHitCount{
      EtaPhiHitCount() : nphiHits(0),netaHits(0) {}
      int nphiHits;
      int netaHits;
    };
    typedef std::map<Identifier,EtaPhiHitCount> HitCountsPerLayer; // map with hits per gas gap

    struct MdtLayerIntersect {
      MdtLayerIntersect() : nhits(0),nholes(0),distFromTubeEnd(0.),tubeLength(1.) {}
      int nhits;
      int nholes;
      double distFromTubeEnd;
      double tubeLength;
    };
    typedef std::map<int,MdtLayerIntersect> MdtLayerIntersectMap;


    struct HitCounts {
      HitCounts() : nmdtHighADCHits(0),nmdtHitsMl1(0),nmdtHitsMl2(0),nmdtHoles(0),nmdtHolesInChamber(0),nmdtEnclosedHoles(0),
		    ncscHitsPhi(0),ncscHitsEta(0),nphiTrigHitLayers(0),netaTrigHitLayers(0),
		    npairedTrigHitLayers(0),nexpectedTrigHitLayers(0),closeToChamberEdge(false),segmentQuality(0.),adcMax(-999) {}
      unsigned int nmdtHighADCHits; // number of mdt hits with an ADC count above cut
      unsigned int nmdtHitsMl1; // number of mdt hits in the first multilayer
      unsigned int nmdtHitsMl2; // number of mdt hits in the second multilayer
      unsigned int nmdtHoles; // number of mdt holes
      unsigned int nmdtHolesInChamber; // number of mdt holes within the chamber bounds
      unsigned int nmdtEnclosedHoles; // number of mdt holes enclosed within hits
      unsigned int ncscHitsPhi; // number of csc phi hits
      unsigned int ncscHitsEta; // number of csc eta hits
      unsigned int nphiTrigHitLayers; // number of trigger hit layers with phi hits
      unsigned int netaTrigHitLayers; // number of trigger hit layers with eta hits
      unsigned int npairedTrigHitLayers; // number of trigger layers with hits in both projections
      unsigned int nexpectedTrigHitLayers; // number of expected trigger layers
      bool closeToChamberEdge;
      double segmentQuality; // segment quality number
      int adcMax; // maximum ADC on segment

      unsigned int nmdtHits() const { return nmdtHitsMl1 + nmdtHitsMl2; }
      unsigned int ncscHits() const { return ncscHitsPhi + ncscHitsEta; }
      
      double goodADCFraction() const { return (double)nmdtHighADCHits/(double)nmdtHits(); }
      
      // count number of hits and holes per layer
      MdtLayerIntersectMap mdtHitHolePerLayerCounts;    
      
      // hit counts per layer 
      HitCountsPerLayer hitCountsPerLayer;    

      /** dump content to a string */
      std::string print() const;
    };

    
    static const InterfaceID& interfaceID();

    /** @brief get hits counts for a segment
	@return provides pointer to fitter, ownsership not passed to client
    */
    virtual HitCounts getHitCounts( const MuonSegment& seg ) const = 0;


  };
  
  inline const InterfaceID& IMuonSegmentHitSummaryTool::interfaceID()
  {
    return IID_IMuonSegmentHitSummaryTool;
  }

  inline std::string IMuonSegmentHitSummaryTool::HitCounts::print() const {
    std::ostringstream sout;
    sout << "HitCounts: ";
    if( nmdtHits() != 0 ) sout << " MDT ml1 " << std::setw(3) << nmdtHitsMl1 << "  ml2 "  << std::setw(3) << nmdtHitsMl2;
    if( nmdtHoles != 0 ) sout << "  holes " << std::setw(3) << nmdtHoles << " in chamber " << std::setw(3) << nmdtHolesInChamber
			      << "  enclosed "  << std::setw(3) << nmdtEnclosedHoles << " quality " << segmentQuality;
    if( ncscHits() != 0 ) sout << "  CSC eta " << std::setw(3) << ncscHitsEta << "  phi "  << std::setw(3) << ncscHitsPhi;
    if( netaTrigHitLayers != 0 ) sout << "  Trig eta " << std::setw(3) << netaTrigHitLayers;
    if( nphiTrigHitLayers != 0 ) sout << "  Trig phi " << std::setw(3) << nphiTrigHitLayers;
    if( npairedTrigHitLayers != 0 ) sout << "  Trig pairs " << std::setw(3) << npairedTrigHitLayers;      
    if( closeToChamberEdge ) sout << " on edge";
    return sout.str();
  }

} // end of name space

#endif // IMuonSegmentHitSummaryTool

