/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_MUONTRACKSUMMARY_H
#define TRK_MUONTRACKSUMMARY_H

#include "Identifier/Identifier.h"

class MsgStream;


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
        Projection() : nhits(0),nholes(0),noutliers(0),ndeltas(0), ncloseHits(0){}
        int nhits;      //<! number of hits on track in the projection
        int nholes;     //<! number of holes in the projection
        int noutliers;  //<! number of outliers in the projection
        int ndeltas;    //<! number of delta electrons in the projection (only filled for MDT chambers)
        int ncloseHits; //<! number of hits within a road around the track in the projection
      };

      ChamberHitSummary(  ) {}

      /** constructor taking the Identifier of the chamber and a boolean whether we are dealing with a MDT chamber or not
	  (the boolean is passed as the ChamberHitSummary cannot unpack the identifier and it needs to know whether it is dealing 
	  with a MDT chamber) */
      ChamberHitSummary( Identifier chID, bool isMdtTube ) : m_chId(chID),m_isMdt(isMdtTube) {}

      /** returns the chamber identifier */
      const Identifier& chamberId() const { return m_chId; }

      /** returns whether this is a MDT chamber */
      bool isMdt() const { return m_isMdt; }

      /** returns the total number of hits on track in the chamber */
      int nhits() const    { return first.nhits + second.nhits; }

      /** returns the total number of holes in the chamber */
      int nholes() const   { return first.nholes + second.nholes; }

      /** returns the total number of outliers in the chamber */
      int noutliers() const { return first.noutliers + second.noutliers; }

      /** returns the total number of delta electrons in the chamber */
      int ndeltas() const  { return first.ndeltas + second.ndeltas; }

      /** returns the total number of close hits in the chamber */
      int ncloseHits() const { return first.ncloseHits + second.ncloseHits; }

      /** returns the total number of eta hits on track  in the chamber */
      int netaHits() const { return isMdt() ? nhits() : first.nhits; }

      /** returns the total number of phi hits on track  in the chamber */
      int nphiHits() const { return isMdt() ? 0       : second.nhits; }

      /** returns the total number of MDT hits on track  in the first multi layer */
      int nMdtHitsMl1() const { return isMdt() ? first.nhits  : 0; }

      /** returns the total number of MDT hits on track  in the second multi layer */
      int nMdtHitsMl2() const { return isMdt() ? second.nhits : 0; }

      /** access to the data of the first MDT multi layer, users have to check whether this is a MDT chamber first!! */
      const Projection& mdtMl1() const { return first; }

      /** access to the data of the second MDT multi layer, users have to check whether this is a MDT chamber first!! */
      const Projection& mdtMl2() const { return second; }

      /** access to the data of the eta projection, users have to check whether this is NOT a MDT chamber first!! */
      const Projection& etaProjection() const { return first; }

      /** access to the data of the phi projection, users have to check whether this is NOT a MDT chamber first!! */
      const Projection& phiProjection() const { return second; }
      

      private:
      Identifier m_chId; //<! chamber identifier
      bool m_isMdt;      //<! is this a MDT chamber
      
      Projection first;  //<! eta projections for cluster chambers, first multi layer for mdt chambers
      Projection second; //<! phi projections for cluster chambers, first multi layer for mdt chambers
      
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
