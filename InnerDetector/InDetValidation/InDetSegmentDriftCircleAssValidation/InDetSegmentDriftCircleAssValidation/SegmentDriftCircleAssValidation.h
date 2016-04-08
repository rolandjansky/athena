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
      const Trk::SegmentCollection                 *m_origColTracks         ;
      std::string                                   m_origtracklocation     ;
      std::vector<const PRD_MultiTruthCollection*>  m_prdCollectionVector   ;
      const HepPDT::ParticleDataTable*              m_particleDataTable     ;
      const TRT_DriftCircleContainer               *m_trtcontainer          ;
      std::string                                   m_PRDTruthNameTRT       ;
      std::string                                   m_circlesTRTname        ;
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

      void newCirclesEvent         ();
      void tracksComparison        ();
      void efficiencyReconstruction();
      int  QualityTracksSelection  ();

      std::list<int> kine(const InDet::TRT_DriftCircle*);
      std::list<PRD_MultiTruthCollection::const_iterator> 
         kinpart(const InDet::TRT_DriftCircle*);
      
      bool isTheSameStrawElement(int,const Trk::PrepRawData*);

      std::list<PRD_MultiTruthCollection::const_iterator>                    
         findTruth (const InDet::TRT_DriftCircle* ,bool& );

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;

    };
  MsgStream&    operator << (MsgStream&   ,const SegmentDriftCircleAssValidation&);
  std::ostream& operator << (std::ostream&,const SegmentDriftCircleAssValidation&); 
}
#endif // SegmentDriftCircleAssValidation_H
