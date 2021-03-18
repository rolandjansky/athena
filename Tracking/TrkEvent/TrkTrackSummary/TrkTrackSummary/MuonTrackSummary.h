/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_MUONTRACKSUMMARY_H
#define TRK_MUONTRACKSUMMARY_H

#include "Identifier/Identifier.h"

class MsgStream;
class MuonTrackSummaryCnv_p1;
class MuonTrackSummaryCnv_p2;
class TrackSummaryCnv_p2;


namespace Muon {
  class MuonTrackSummaryHelperTool;
}

namespace Trk {
 
  /** 
      Detailed track summary for the muon system 
      Give access to hit counts per chamber.
  */
  class MuonTrackSummary {
  public:
    friend class Muon::MuonTrackSummaryHelperTool;

    /** structure to hold information per chamber in the muon system */
    struct ChamberHitSummary {
      friend class Muon::MuonTrackSummaryHelperTool;
      
      /** structure to hold the information for the eta/phi projection of RPC, TGC and CSC
      chambers and per multilayer for MDT chambers */
      struct Projection {
      Projection() {}
        int nhits{0};      //<! number of hits on track in the projection
        int nholes{0};     //<! number of holes in the projection
        int noutliers{0};  //<! number of outliers in the projection
        int ndeltas{0};    //<! number of delta electrons in the projection (only filled for MDT chambers)
        int ncloseHits{0}; //<! number of hits within a road around the track in the projection
        int ngoodHits{0};  //<! number of hits that matter for the track
        int noutBounds{0}; //<! number of out-of-bounds hits
      };

  
      /** constructor taking the Identifier of the chamber and a boolean whether we are dealing with a MDT chamber or not
      (the boolean is passed as the ChamberHitSummary cannot unpack the identifier and it needs to know whether it is dealing 
      with a MDT chamber) */
      ChamberHitSummary( Identifier chID, bool isMdtTube ) : m_chId(chID),m_isMdt(isMdtTube) {}

      /** returns the chamber identifier */
      const Identifier& chamberId() const { return m_chId; }

      /** returns whether this is a MDT chamber */
      bool isMdt() const { return m_isMdt; }

      /** returns the total number of hits on track in the chamber */
      int nhits() const    { return m_first.nhits + m_second.nhits; }

      /** returns the total number of holes in the chamber */
      int nholes() const   { return m_first.nholes + m_second.nholes; }

      /** returns the total number of outliers in the chamber */
      int noutliers() const { return m_first.noutliers + m_second.noutliers; }

      /** returns the total number of delta electrons in the chamber */
      int ndeltas() const  { return m_first.ndeltas + m_second.ndeltas; }

      /** returns the total number of close hits in the chamber */
      int ncloseHits() const { return m_first.ncloseHits + m_second.ncloseHits; }

      /** returns the number of non-deweighted hits in the chamber */
      int ngoodHits() const { return m_first.ngoodHits + m_second.ngoodHits; }

      //returns the number of out of bounds hits
      int noutBoundsHits() const { return m_first.noutBounds + m_second.noutBounds; }

      /** returns the total number of eta hits on track  in the chamber */
      int netaHits() const { return isMdt() ? nhits() : m_first.nhits; }

      /** returns the total number of phi hits on track  in the chamber */
      int nphiHits() const { return isMdt() ? 0       : m_second.nhits; }

      /** returns the total number of MDT hits on track  in the first multi layer */
      int nMdtHitsMl1() const { return isMdt() ? m_first.nhits  : 0; }

      /** returns the total number of MDT hits on track  in the second multi layer */
      int nMdtHitsMl2() const { return isMdt() ? m_second.nhits : 0; }

      /** access to the data of the first MDT multi layer, users have to check whether this is a MDT chamber first!! */
      const Projection& mdtMl1() const { return m_first; }

      /** access to the data of the second MDT multi layer, users have to check whether this is a MDT chamber first!! */
      const Projection& mdtMl2() const { return m_second; }

      /** access to the data of the eta projection, users have to check whether this is NOT a MDT chamber first!! */
      const Projection& etaProjection() const { return m_first; }

      /** access to the data of the phi projection, users have to check whether this is NOT a MDT chamber first!! */
      const Projection& phiProjection() const { return m_second; }
      

      private:
      friend class ::MuonTrackSummaryCnv_p1;
      friend class ::MuonTrackSummaryCnv_p2;
      friend class ::TrackSummaryCnv_p2;

      Identifier m_chId{0}; //<! chamber identifier
      bool m_isMdt{false};      //<! is this a MDT chamber
      
      Projection m_first;  //<! eta projections for cluster chambers, first multi layer for mdt chambers
      Projection m_second; //<! phi projections for cluster chambers, first multi layer for mdt chambers
      
    };

    /** default constructor */
    MuonTrackSummary();

    /** destructor */
    ~MuonTrackSummary();
   
    /** number of eta hits on the track */
    unsigned int netaHits() const;

    /** number of phi hits on the track */
    unsigned int nphiHits() const;

    /** number of eta trigger hits on the track */
    unsigned int noutliers() const;

    /** number of holes on the track */
    unsigned int nholes() const;

    /** number of holes on the track */
    unsigned int ncloseHits() const;

    /** number of scattering centres on the track */
    unsigned int nscatterers() const { return m_nscatterers; }

    /** number of pseudo measurements on the track */
    unsigned int npseudoMeasurements() const { return m_npseudoMeasurements; }

    /** access to the vector of chamber hit summaries on the track */
    const std::vector<ChamberHitSummary>& chamberHitSummary() const { return m_chamberHitSummary; }

  private:
    friend class ::MuonTrackSummaryCnv_p1;
    friend class ::MuonTrackSummaryCnv_p2;
    friend class ::TrackSummaryCnv_p2;

    unsigned int m_nscatterers; //<! number of scattering centres on the track
    unsigned int m_npseudoMeasurements; //<! number of pseudomeasurements on the track
    std::vector<ChamberHitSummary> m_chamberHitSummary; //<! vector with chamber hit summaries

  };

/**output. This dumps the values of each of the possible summary enums*/
MsgStream& operator<<(MsgStream& out, const MuonTrackSummary& trackSum);

/**output. This dumps the values of each of the possible summary enums*/
std::ostream& operator<<(std::ostream& out, const MuonTrackSummary& trackSum);

}
#endif
