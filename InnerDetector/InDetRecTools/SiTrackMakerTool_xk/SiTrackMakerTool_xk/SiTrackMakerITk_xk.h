/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiTrackMakerITk_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for  Trk::Track production in SCT and Pixels
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 22/03/2005 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiTrackMakerITk_xk_H
#define SiTrackMakerITk_xk_H

#include <list>
#include <map>

#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ISiTrackMaker.h"
#include "InDetRecToolInterfaces/ISiDetElementsRoadMaker.h" 
#include "InDetRecToolInterfaces/ISiCombinatorialTrackFinder.h"
#include "InDetRecToolInterfaces/ISeedToTrackConversionTool.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "HepMC/GenParticle.h"

class MsgStream;

namespace InDet{

  /**
  @class SiTrackMakerITk_xk 
  
  InDet::SiTrackMakerITk_xk is algorithm which produce track-finding started
  from 3 space points information
  in the road of InDetDD::SiDetectorElement* sorted in propagation order.
  @author Igor.Gavrilenko@cern.ch     
  */

  class SiTrackMakerITk_xk : 
    virtual public ISiTrackMaker, public AthAlgTool
    {

      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      SiTrackMakerITk_xk
	(const std::string&,const std::string&,const IInterface*);
      virtual ~SiTrackMakerITk_xk();
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      ///////////////////////////////////////////////////////////////////
      // Main methods for local track finding
      ///////////////////////////////////////////////////////////////////
      
      virtual const std::list<Trk::Track*>&
	getTracks(const std::list<const Trk::SpacePoint*>&);

      virtual const std::list<Trk::Track*>& 
	getTracks(const Trk::TrackParameters&,const std::list<Amg::Vector3D>&);

      virtual void newEvent    (bool,bool);
      virtual void newTrigEvent(bool,bool);

      virtual void endEvent();

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump(MsgStream&    out) const;
      std::ostream& dump(std::ostream& out) const;

    protected:
      
      SiTrackMakerITk_xk() = delete;
      SiTrackMakerITk_xk(const SiTrackMakerITk_xk&) =delete;
      SiTrackMakerITk_xk &operator=(const SiTrackMakerITk_xk&) = delete;
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle  ;
      MagField::IMagFieldSvc*                m_fieldService        ;
      ToolHandle<InDet::ISiDetElementsRoadMaker>     m_roadmaker   ;
      ToolHandle<InDet::ISiCombinatorialTrackFinder> m_tracksfinder;
      ToolHandle<InDet::ISeedToTrackConversionTool>  m_seedtrack   ;

      int                            m_nprint        ;  // Kind output information
      int                            m_inputseeds [5];  // Number input seeds
      int                            m_ntrackpar  [5];  // Wrong track parameters
      int                            m_goodseeds  [5];  // Number good  seeds
      int                            m_findtracks [5];  // Number found tracks
      int                            m_bremseeds  [5];  // Number brem  seeds
      int                            m_findtracksb[5];  // Number found tracks with brem
      int                            m_twoclusters[5];
      int                            m_wrongroad  [5];
      int                            m_wronginit  [5];
      int                            m_notrack    [5];
      int                            m_notnewtrack[5];
      int                            m_bremattempt[5];
      int                            m_multytracks[5];
      mutable double                 m_desize     [5];

      int                            m_truthseeds [5];
      int                            m_inseeds [5][8];
      int                            m_okseeds [5][8];
      int                            m_trseeds [5][8];
      int                            m_trseedsT   [5];
      int                            m_truthseedsG[5];
      int                            m_inseedsG[5][8];
      int                            m_okseedsG[5][8];

      int                            m_seedsfilter   ;  // Level of seeds filer
      unsigned int                   m_wrongcluster  ;  // Max lentgh of thtrack
      std::string                    m_fieldmode     ;  // Mode of magnetic field
      std::string                    m_patternName   ;  // Name of the pattern recognition

      SG::ReadHandle<CaloClusterROI_Collection> m_caloCluster;
      SG::ReadHandle<CaloClusterROI_Collection> m_caloHad;

      Trk::TrackInfo                 m_trackinfo     ;
      bool                           m_pix           ;
      bool                           m_sct           ;
      bool                           m_usePix        ; //flags to set whether to use pixel/sct cluster, irrespective of what is in event
      bool                           m_useSct        ;
      bool                           m_useassoTool   ; // Use prd-track association tool
      bool                           m_cosmicTrack   ; // Is it cosmic track
      bool                           m_simpleTrack   ; // Is it simple output track
      bool                           m_multitracks   ;
      bool                           m_useBremModel  ;
      bool                           m_useCaloSeeds  ;
      bool                           m_useSSSfilter  ;
      bool                           m_useHClusSeed  ; // Hadronic Calorimeter Seeds 
      bool                           m_seedsegmentsWrite; // Call seed to track conversion
      bool                           m_heavyion      ; // Is it heavy ion events
      bool                           m_usetruth      ;
      Trk::MagneticFieldProperties   m_fieldprop     ; // Magnetic field properties
      double                         m_xi2max        ; // max Xi2 for updators
      double                         m_xi2maxNoAdd   ; // max Xi2 for clusters
      double                         m_xi2maxlink    ; // max Xi2 for clusters
      double                         m_pTmin         ; // min pT
      double                         m_pTminBrem     ; // min pT for Brem mode
      double                         m_pTminSSS      ; // min pT for SSS filtering
      double                         m_distmax       ; // 
      double                         m_xi2multitracks; // max Xi2 for multi tracks
      int                            m_nholesmax     ; // Max number holes
      int                            m_dholesmax     ; // Max holes gap
      int                            m_nclusmin      ; // Min number clusters
      int                            m_nwclusmin     ; // Min umber weighted clusters(pix=2 sct=1)
      InDet::TrackQualityCuts        m_trackquality  ;
      std::list<Trk::Track*>         m_tracks        ; // List found tracks
      std::multimap<const Trk::PrepRawData*,const Trk::Track*> m_clusterTrack;
      std::vector<double>            m_caloFRZ       ;
      double m_phiWidth                              ;
      double m_etaWidth                              ;
      double m_p[9]                                  ;

      const PRD_MultiTruthCollection* m_truthPIX; 
      const PRD_MultiTruthCollection* m_truthSCT; 

      std::vector < double >         m_etabins       ;
      std::vector < double >         m_ptbins        ;
      

      ///////////////////////////////////////////////////////////////////
      // Methods 
      ///////////////////////////////////////////////////////////////////

 
      const Trk::TrackParameters* getAtaPlane
	(const std::list<const Trk::SpacePoint*>&);

      bool globalPositions(const Trk::SpacePoint*,const Trk::SpacePoint*,const Trk::SpacePoint*,
			   double*,double*,double*);

      bool globalPosition(const Trk::SpacePoint*,double*,double*);
      void globalDirections(double*,double*,double*,double*,double*,double*);
      void setTrackQualityCuts();
      void detectorElementsSelection(std::vector<const InDetDD::SiDetectorElement*>&);
      bool newSeed    (const std::list<const Trk::SpacePoint*>&);
      int  kindSeed   (const std::list<const Trk::SpacePoint*>&);
      bool isCaloCompatible   ();
      void clusterTrackMap(Trk::Track*);
      
      double pTmin(double T);
      
      void       magneticFieldInit();

      int  rapidity(const std::list<const Trk::SpacePoint*>&);
      bool purity  (const std::list<const Trk::SpacePoint*>&);
      void truth   (const Trk::PrepRawData*,std::set<const HepMC::GenParticle*>&);
 
      StatusCode magneticFieldInit(IOVSVC_CALLBACK_ARGS);

      MsgStream&    dumpconditions(MsgStream&    out) const;
      MsgStream&    dumpevent     (MsgStream&    out) const;
      
    };

    MsgStream&    operator << (MsgStream&   ,const SiTrackMakerITk_xk&);
    std::ostream& operator << (std::ostream&,const SiTrackMakerITk_xk&); 
    
    inline double SiTrackMakerITk_xk::pTmin(double eta)
    {
      if (m_ptbins.size() == 0) return m_pTmin;
      double aeta = fabs(eta);
      for(int n = int(m_ptbins.size()-1); n>=0; --n) {
        if(aeta > m_etabins.at(n)) return m_ptbins.at(n);
      }
      
      return m_pTmin;
    }
} // end of name space

#endif // SiTrackMakerITk_xk_H

