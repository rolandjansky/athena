/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiTrackMaker_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for  Trk::Track production in SCT and Pixels
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 22/03/2005 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiTrackMaker_xk_H
#define SiTrackMaker_xk_H

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
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"

class MsgStream;

namespace InDet{

  /**
  @class SiTrackMaker_xk 
  
  InDet::SiTrackMaker_xk is algorithm which produce track-finding started
  from 3 space points information
  in the road of InDetDD::SiDetectorElement* sorted in propagation order.
  @author Igor.Gavrilenko@cern.ch     
  */

  class SiTrackMaker_xk : 
    virtual public ISiTrackMaker, public AthAlgTool
    {

      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      SiTrackMaker_xk
	(const std::string&,const std::string&,const IInterface*);
      virtual ~SiTrackMaker_xk();
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
      
      SiTrackMaker_xk() = delete;
      SiTrackMaker_xk(const SiTrackMaker_xk&) =delete;
      SiTrackMaker_xk &operator=(const SiTrackMaker_xk&) = delete;
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle  ;
      MagField::IMagFieldSvc*                m_fieldService        ;
      ToolHandle<InDet::ISiDetElementsRoadMaker>     m_roadmaker   ;
      ToolHandle<InDet::ISiCombinatorialTrackFinder> m_tracksfinder;
      ToolHandle<InDet::ISeedToTrackConversionTool>  m_seedtrack   ;
      IBeamCondSvc*                                  m_beam        ;

      int                            m_nprint        ;  // Kind output information
      int                            m_inputseeds    ;  // Number input seeds
      int                            m_goodseeds     ;  // Number good  seeds
      int                            m_findtracks    ;  // Numbe found tracks
      int                            m_seedsfilter   ;  // Level of seeds filer
      unsigned int                   m_wrongcluster  ;  // Max lentgh of thtrack
      std::string                    m_fieldmode     ;  // Mode of magnetic field
      std::string                    m_patternName   ;  // Name of the pattern recognition
//      std::string         m_inputClusterContainerName;
//      std::string      m_inputHadClusterContainerName;
      SG::ReadHandle<CaloClusterROI_Collection> m_caloCluster;
      SG::ReadHandle<CaloClusterROI_Collection> m_caloHad;
      std::string                    m_beamconditions;
      Trk::TrackInfo                 m_trackinfo     ;
      bool                           m_pix           ;
      bool                           m_sct           ;
      bool                           m_dbm           ;
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
      bool                           m_sss           ; // True if SSS seed without filter 
      bool                           m_ITKGeomtry    ; // ITK geometry
      bool                        m_seedsegmentsWrite; // Call seed to track conversion
      bool                           m_heavyion      ; // Is it heavy ion events
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
      std::list<double>              m_caloF         ;
      std::list<double>              m_caloR         ;
      std::list<double>              m_caloZ         ;
      std::list<double>              m_hadF          ;
      std::list<double>              m_hadR          ;
      std::list<double>              m_hadZ          ;
      double m_phiWidth                              ;
      double m_etaWidth                              ;
      double m_p[9]                                  ;
      double m_xybeam[2]                             ;

      ///////////////////////////////////////////////////////////////////
      // Methods 
      ///////////////////////////////////////////////////////////////////

 
      const Trk::TrackParameters* getAtaPlane
	(bool,const std::list<const Trk::SpacePoint*>&);
      const Trk::TrackParameters* getAtaPlaneDBM
	(const std::list<const Trk::SpacePoint*>&);

      bool globalPositions(const Trk::SpacePoint*,const Trk::SpacePoint*,const Trk::SpacePoint*,
			   double*,double*,double*);

      bool globalPosition(const Trk::SpacePoint*,double*,double*);
      void globalDirections(double*,double*,double*,double*,double*,double*);
      void setTrackQualityCuts();
      void detectorElementsSelection(std::vector<const InDetDD::SiDetectorElement*>&);
      bool newSeed    (const std::list<const Trk::SpacePoint*>&);
      bool isNewTrack(Trk::Track*);
      bool isCaloCompatible   ();
      bool isHadCaloCompatible();
      bool isDBMSeeds(const Trk::SpacePoint*);
      void clusterTrackMap(Trk::Track*);
      void       magneticFieldInit();
      StatusCode magneticFieldInit(IOVSVC_CALLBACK_ARGS);

      MsgStream&    dumpconditions(MsgStream&    out) const;
      MsgStream&    dumpevent     (MsgStream&    out) const;
    };

    MsgStream&    operator << (MsgStream&   ,const SiTrackMaker_xk&);
    std::ostream& operator << (std::ostream&,const SiTrackMaker_xk&); 

} // end of name space

#endif // SiTrackMaker_xk_H

