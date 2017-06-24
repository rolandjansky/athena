/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TrackClusterAssValidation_H
#define TrackClusterAssValidation_H

#include <string>
#include <map>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h" 
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "HepMC/GenParticle.h"
#include "HepPDT/ParticleDataTable.hh"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "InDetRecToolInterfaces/IInDetDynamicCutsTool.h"

namespace InDet {

  /////////////////////////////////////////////////////////////////////////////////
  //  Header file for class Barcode
  /////////////////////////////////////////////////////////////////////////////////

  class Barcode
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////    
    public:

      Barcode()  {};
      Barcode(const Barcode&);
      Barcode(int,int);
      Barcode(int,int,const HepMC::GenParticle*);
      ~Barcode() {};
      Barcode& operator = (const Barcode&);
      int barcode () const {return abs(m_barcharge);}
      int charge  () const {if(m_barcharge<0) return -1; if(m_barcharge>0) return 1; return 0;}
      int rapidity() const {return m_rapidity;}
      const HepMC::GenParticle* particle() const {return m_particle;}

    protected:

      int m_barcharge;
      int m_rapidity;
      const HepMC::GenParticle* m_particle;
    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////
  
  inline Barcode::Barcode(const Barcode& BC)
    {
      *this = BC;
    }
  
  inline Barcode& Barcode::operator = (const Barcode& BC) 
    {
      if(&BC!=this) {
	m_barcharge = BC.m_barcharge;
	m_rapidity  = BC.m_rapidity ;
	m_particle  = BC.m_particle ;
      }
      return(*this);
    }
  inline Barcode::Barcode (int bc,int rap)
    {
      m_barcharge = bc ;
      m_rapidity  = rap;
      m_particle  = 0  ;
    }
  inline Barcode::Barcode (int bc,int rap,const HepMC::GenParticle* pa)
    {
      m_barcharge = bc ;
      m_rapidity  = rap;
      m_particle  = pa ;
    }

  // Class-algorithm for track cluster association validation
  //
  class TrackClusterAssValidation : public AthAlgorithm 
    {
    
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard Algotithm methods
      ///////////////////////////////////////////////////////////////////

      TrackClusterAssValidation(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~TrackClusterAssValidation() {}
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

      bool                               m_usePIX                 ;
      bool                               m_useSCT                 ;
      bool                               m_useTRT                 ;
      bool                               m_useOutliers            ;
      int                                m_pdg                    ;
      int                                m_outputlevel            ;
      int                                m_nprint                 ;
      int                                m_ncolection             ; 
      int                                m_nspacepoints           ;
      int                                m_nclusters              ;
      int                                m_nevents                ;
      int                                m_nclustersPTOT          ;
      int                                m_nclustersSTOT          ;
      int                                m_nspacepointsPTOT       ;
      int                                m_nspacepointsSTOT       ;
      int                                m_nspacepointsOTOT       ;
      int                                m_nclustersTRT           ;
      int                                m_nqtracks               ;
      int                                m_efficiency   [100][6]  ;
      int                                m_efficiencyN  [100][6][5];
      int                                m_efficiencyBTE[100][6][5][4];
      int                                m_efficiencyPOS[100][6]  ;
      int                                m_efficiencyNEG[100][6]  ;
      int                                m_ntracksPOSB  [100]     ;
      int                                m_ntracksPOSE  [100]     ;
      int                                m_ntracksPOSDBM[100];
      int                                m_ntracksNEGB  [100]     ;
      int                                m_ntracksNEGE  [100]     ;
      int                                m_ntracksNEGDBM[100];
      int                                m_total        [100][50] ;
      int                                m_fake         [100][50] ;
      int                                m_events                 ;
      int                                m_eventsPOS              ;
      int                                m_eventsNEG              ;
      int                                m_eventsBTE[4]           ;
      int                                m_particleClusters   [50];
      int                                m_particleSpacePoints[50];
      int                                m_particleClustersBTE   [50][4];
      int                                m_particleSpacePointsBTE[50][4];
      int                                m_nclustersPosBP         ;
      int                                m_nclustersPosBS         ;
      int                                m_nclustersPosEP         ;
      int                                m_nclustersPosES         ;
      int                                m_nclustersNegBP         ;
      int                                m_nclustersNegBS         ;
      int                                m_nclustersNegEP         ;
      int                                m_nclustersNegES         ;
      unsigned int                       m_clcut                  ;
      unsigned int                       m_clcutTRT               ;
      unsigned int                       m_spcut                  ;
      double                             m_ptcut                  ;
      double                             m_ptcutmax               ;
      double                             m_rapcut                 ;
      double                             m_tcut                   ;
      double                             m_rmin                   ;
      double                             m_rmax                   ;

      double                             m_rapTRAN                ;
      double                             m_rapENDS                ;
      double                             m_rapENDP                ;
   
      std::vector<std::string>           m_tracklocation          ; 
      std::string                        m_clustersSCTname        ;
      std::string                        m_clustersPixelname      ;
      std::string                        m_clustersTRTname        ;
      std::string                        m_truth_locationPixel    ;
      std::string                        m_truth_locationSCT      ;
      std::string                        m_truth_locationTRT      ;

      SG::ReadHandle<SpacePointContainer>         m_spacepointsSCT    ;
      SG::ReadHandle<SpacePointContainer>         m_spacepointsPixel  ;
      SG::ReadHandle<SpacePointOverlapCollection> m_spacepointsOverlap;

      const SiClusterContainer         * m_pixcontainer           ;
      const SiClusterContainer         * m_sctcontainer           ;
      const TRT_DriftCircleContainer   * m_trtcontainer           ;

      const PRD_MultiTruthCollection   * m_truthPIX               ;
      const PRD_MultiTruthCollection   * m_truthSCT               ;
      const PRD_MultiTruthCollection   * m_truthTRT               ;

      std::multimap<const HepMC::GenParticle*,const Trk::PrepRawData*> m_kineclusterN    ;
      std::multimap<const HepMC::GenParticle*,const Trk::PrepRawData*> m_kineclusterTRTN ;
      std::multimap<const HepMC::GenParticle*,const Trk::SpacePoint*>  m_kinespacepointN ;

      std::list<Barcode>                         m_particles[100] ;
      std::list<int>                             m_difference[100];
      std::multimap<int,int>                     m_tracks[100]    ;
      std::multimap<const HepMC::GenParticle*,int> m_tracksN[100] ;
      const HepPDT::ParticleDataTable*        m_particleDataTable ;
			
      /** tool to get cut values depending on different variable */   
      ToolHandle<IInDetDynamicCutsTool>     m_dynamicCutsTool;   
      bool m_useDynamicCuts;	// use InDetDynamicCutsTool to determine the cut value depending on characteristics of each track (default is false)   

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      void newSpacePointsEvent     ();
      void newClustersEvent        ();
      void tracksComparison        ();
      void efficiencyReconstruction();
      bool noReconstructedParticles();
      int  QualityTracksSelection  ();

      int kine (const Trk::PrepRawData*,const Trk::PrepRawData*,const HepMC::GenParticle**,int);
      int kine (const Trk::PrepRawData*,const HepMC::GenParticle**,int);
      int kine0(const Trk::PrepRawData*,const HepMC::GenParticle**,int);
      bool isTheSameDetElement(const HepMC::GenParticle*,const Trk::PrepRawData*);
      bool isTheSameDetElement(const HepMC::GenParticle*,const Trk::SpacePoint* );

      PRD_MultiTruthCollection::const_iterator findTruth
	(const Trk::PrepRawData*,PRD_MultiTruthCollection::const_iterator&);
      
      int charge(std::pair<int,const Trk::PrepRawData*>,int&);
      int charge(std::pair<int,const Trk::PrepRawData*>,int&, double&);

      int charge(std::pair<const HepMC::GenParticle*,const Trk::PrepRawData*>,int&);
      int charge(std::pair<const HepMC::GenParticle*,const Trk::PrepRawData*>,int&, double&);

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;

    };
  MsgStream&    operator << (MsgStream&   ,const TrackClusterAssValidation&);
  std::ostream& operator << (std::ostream&,const TrackClusterAssValidation&); 

}
#endif // TrackClusterAssValidation_H
