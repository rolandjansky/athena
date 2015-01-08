/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTMERGER_H
#define MUON_MUONSEGMENTMERGER_H

#include "MuonSegmentMakerToolInterfaces/IMuonSegmentMerger.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkEventPrimitives/FitQuality.h"


namespace Muon {
  
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
  class MuonIdHelperTool;
  class IMuonSegmentTriggerHitAssociator;
  /**
     @brief tool to remove overlaps between segments

  */
  class MuonSegmentMerger : virtual public IMuonSegmentMerger, public AthAlgTool {
  public:
    /** @brief constructor */
    MuonSegmentMerger(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    virtual ~MuonSegmentMerger ();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
 
     /** @brief find segments sharing all MDT hits but different phi hits The caller should NOT take ownership of the segments */
    SegVec findDuplicates( const SegVec& segments ) const;

    /** @brief merges duplicates in a vector of segments. The callers should take ownership of the new Segments.*/
    const MuonSegment* merge( const SegVec& segments ) const;
   
    /** @brief group all segments that have at least one overlapping cluster */
    std::vector< SegVec > resolveClusterOverlaps( const SegVec& segments ) const ;

  private:

    ToolHandle<Muon::MuonIdHelperTool>                  m_idHelperTool;     //!< IdHelper tool
    ToolHandle<Muon::MuonEDMHelperTool>                 m_helperTool;       //!< EDM Helper tool
    ToolHandle<Muon::MuonEDMPrinterTool>                m_printer;          //!< EDM printer tool
    ToolHandle<Muon::IMuonSegmentTriggerHitAssociator>  m_triggerHitsAssociator; //!< trigger hit associator

  };

  class LocalSortSegmentsByChi2 {
  public:
    bool operator()( const MuonSegment* seg1, const MuonSegment* seg2){
	
      // prefer segments with fit quality (always expected)
      const Trk::FitQuality* fq1 = seg1->fitQuality();
      const Trk::FitQuality* fq2 = seg2->fitQuality();
      if( !fq1 && fq2 ) return false;
      if( fq1  && !fq2 ) return true;
      if(!fq1 && !fq2) return false;
	
      if( fq1->numberDoF() > fq2->numberDoF() ) return true;
      if( fq1->numberDoF() < fq2->numberDoF() ) return false;
      
      // select candidate with smallest chi2
      double chi2Ndof1 = fq1->chiSquared()/fq1->numberDoF();
      double chi2Ndof2 = fq2->chiSquared()/fq2->numberDoF();
      return chi2Ndof1 < chi2Ndof2;
    }
  };

}

#endif
