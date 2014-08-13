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
#include "TrkGeometry/MagneticFieldProperties.h"

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
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle  ;
      MagField::IMagFieldSvc*                m_fieldService        ;
      ToolHandle<InDet::ISiDetElementsRoadMaker>     m_roadmaker   ;
      ToolHandle<InDet::ISiCombinatorialTrackFinder> m_tracksfinder;

      int                            m_nprint        ;  // Kind output information
      int                            m_inputseeds    ;  // Number input seeds
      int                            m_goodseeds     ;  // Number good  seeds
      int                            m_findtracks    ;  // Numbe found tracks
      int                            m_seedsfilter   ;  // Level of seeds filer
      unsigned int                   m_wrongcluster  ;  // Max lentgh of thtrack
      std::string                    m_fieldmode     ;  // Mode of magnetic field
      std::string                    m_patternName   ;  // Name of the pattern recognition
      std::string         m_inputClusterContainerName;
      Trk::TrackInfo                 m_trackinfo     ;
      bool                           m_pix           ;
      bool                           m_sct           ;
      bool                           m_useassoTool   ; // Use prd-track association tool
      bool                           m_cosmicTrack   ; // Is it cosmic track
      bool                           m_simpleTrack   ; // Is it simple output track
      bool                           m_multitracks   ;
      bool                           m_useBremModel  ;
      bool                           m_useCaloSeeds  ;
      bool                           m_useSSSfilter  ;
      Trk::MagneticFieldProperties   m_fieldprop     ; // Magnetic field properties
      double                         m_xi2max        ; // max Xi2 for updators
      double                         m_xi2maxNoAdd   ; // max Xi2 for clusters
      double                         m_xi2maxlink    ; // max Xi2 for clusters
      double                         m_pTmin         ; // min pT
      double                         m_pTminBrem     ; // min pT for Brem mode
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
      std::list<double>              m_caloE         ;
      double m_phiWidth                              ;
      double m_etaWidth                              ;

      ///////////////////////////////////////////////////////////////////
      // Methods 
      ///////////////////////////////////////////////////////////////////

 
      const Trk::TrackParameters* getAtaPlane
	(const std::list<const Trk::SpacePoint*>&);

      bool globalPositions(const Trk::SpacePoint*,const Trk::SpacePoint*,const Trk::SpacePoint*,
			   double*,double*,double*);

      void globalPosition(const Trk::SpacePoint*,double*,double*);
      void globalPosition(const Trk::SpacePoint*,double*,double*,double*);
      void magneticFieldInit();
      void setTrackQualityCuts();
      void detectorElementsSelection(std::list<const InDetDD::SiDetectorElement*>&);
      bool newClusters(const std::list<const Trk::SpacePoint*>&);
      bool newSeed    (const std::list<const Trk::SpacePoint*>&);
      bool isNewTrack(Trk::Track*);
      bool isCaloCompatible(const Trk::TrackParameters&);
      void clusterTrackMap(Trk::Track*);

      MsgStream&    dumpconditions(MsgStream&    out) const;
      MsgStream&    dumpevent     (MsgStream&    out) const;

    };

  MsgStream&    operator << (MsgStream&   ,const SiTrackMaker_xk&);
  std::ostream& operator << (std::ostream&,const SiTrackMaker_xk&); 

} // end of name space

#endif // SiTrackMaker_xk_H

