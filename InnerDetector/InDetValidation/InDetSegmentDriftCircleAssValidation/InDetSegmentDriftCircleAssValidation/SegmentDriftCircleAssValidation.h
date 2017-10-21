/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SegmentDriftCircleAssValidation_H
#define SegmentDriftCircleAssValidation_H

#include <string>
#include <map>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "HepMC/GenParticle.h"
#include "HepPDT/ParticleDataTable.hh"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

///Needed for the TRT track segments
#include "TrkSegment/SegmentCollection.h"

//Drift circle 
#include "InDetPrepRawData/TRT_DriftCircle.h"

#include "StoreGate/ReadHandleKey.h"

namespace InDet {


  // Class-algorithm for track cluster association validation
  //
  class SegmentDriftCircleAssValidation : public AthAlgorithm 
    {
    
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard Algotithm methods
      ///////////////////////////////////////////////////////////////////

      SegmentDriftCircleAssValidation(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~SegmentDriftCircleAssValidation() {}
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump     (MsgStream&    out) const;
      std::ostream& dump     (std::ostream& out) const;

    protected:

      ///////////////////////////////////////////////////////////////////
      // Protected data 
      ///////////////////////////////////////////////////////////////////

      int                                           m_outputlevel           ;
      int                                           m_nprint                ;
      SG::ReadHandleKey<Trk::SegmentCollection>     m_origtrackKey{ this, "OrigTracksLocation", "TRTSegments", "TRT Segments collection name" };
      const HepPDT::ParticleDataTable*              m_particleDataTable     ;
      SG::ReadHandleKey<PRD_MultiTruthCollection>   m_PRDTruthTRTKey{ this, "TruthNameTRT", "PRD_MultiTruthTRT", "PRD Multitruth collection name" };
      SG::ReadHandleKey<TRT_DriftCircleContainer>   m_circlesTRTKey{ this, "TRT_DriftCirclesName", "TRT_DriftCircles", "TRT Driftcircle container name" };
      double                                        m_pTmin                 ;
      double                                        m_tcut                  ;
      double                                        m_rapcut                ;
      int                                           m_dccut                 ;
      double                                        m_rmin                  ;
      double                                        m_rmax                  ;

      int                                           m_events                ;
      int                                           m_ncircles              ;
      std::multimap<int,const Trk::PrepRawData*>    m_kinecircle            ;
      std::list<int>                                m_allBarcodes           ;
      int                                           m_nqsegments            ;
      std::list<int>                                m_particles             ;
      std::multimap<int,int>                        m_allParticles          ;
      std::multimap<int,int>                        m_tracks                ;
      std::multimap<const HepMC::GenParticle*, int> m_genPars               ;
      int                                           m_efficiency[5]         ;

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      void newCirclesEvent         ( const PRD_MultiTruthCollection* );
      void tracksComparison        ( const Trk::SegmentCollection*, const PRD_MultiTruthCollection* );
      void efficiencyReconstruction();
      int  QualityTracksSelection  ();

      std::list<int> kine(const InDet::TRT_DriftCircle*, const PRD_MultiTruthCollection* prdCollection );
      std::list<PRD_MultiTruthCollection::const_iterator> 
         kinpart(const InDet::TRT_DriftCircle*, const PRD_MultiTruthCollection* );
      
      bool isTheSameStrawElement(int,const Trk::PrepRawData*);

      std::list<PRD_MultiTruthCollection::const_iterator>                    
         findTruth (const InDet::TRT_DriftCircle* ,bool&, const PRD_MultiTruthCollection* );

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;

    };
  MsgStream&    operator << (MsgStream&   ,const SegmentDriftCircleAssValidation&);
  std::ostream& operator << (std::ostream&,const SegmentDriftCircleAssValidation&); 
}
#endif // SegmentDriftCircleAssValidation_H
