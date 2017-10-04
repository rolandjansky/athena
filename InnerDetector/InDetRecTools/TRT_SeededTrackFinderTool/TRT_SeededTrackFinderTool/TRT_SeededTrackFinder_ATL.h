/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
   Header file for class TRT_SeededTrackFinder_ATL
  (c) ATLAS Detector software
  Class for Trk::Track production in SCT and Pixels
  Version 1.0: 04/12/2006
  Authors    : Thomas Koffas
  email      : Thomas.Koffas@cern.ch
**********************************************************************************/

#ifndef TRT_SeededTrackFinder_ATL_H
#define TRT_SeededTrackFinder_ATL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include <list>
#include <vector>
#include <map>

//Tool Handler
//
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"

//Tool Interface
//
#include "InDetRecToolInterfaces/ITRT_SeededTrackFinder.h"

//Magnetic field
//
#include "TrkGeometry/MagneticFieldProperties.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

//Si Tools
//
#include "TRT_SeededTrackFinderTool/SiNoise_bt.h"

//Combinatorial Track Finder Tool
//
#include "InDetRecToolInterfaces/ISiCombinatorialTrackFinder.h"

//ReadHandle key
//
#include "StoreGate/ReadHandleKey.h"

//ClusterCaloROI Collection
//
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"
class MsgStream;
class TRT_ID   ;

namespace Trk{
  class IUpdator;
  class IPropagator;
  class IRIO_OnTrackCreator;
  class IMagFieldSvc;
}

namespace InDet{
  class ISiDetElementsRoadMaker;
  class ITRT_SeededSpacePointFinder;

  /**
  @class TRT_SeededTrackFinder_ATL 
  
  InDet::TRT_SeededTrackFinderATL is an algorithm which produces tracks
  along the road of InDetDD::SiDetectorElement* sorted in propagation order.
  @author Thomas.Koffas@cern.ch     
  */

  class TRT_SeededTrackFinder_ATL : 

    virtual public ITRT_SeededTrackFinder, public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////

    public:
            
      ///////////////////////////////////////////////////////////////////
      /** Standard tool methods                                        */
      ///////////////////////////////////////////////////////////////////

      TRT_SeededTrackFinder_ATL
	(const std::string&,const std::string&,const IInterface*);
      virtual ~TRT_SeededTrackFinder_ATL();
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      ///////////////////////////////////////////////////////////////////
      /** Main methods for local track finding                         */
      ///////////////////////////////////////////////////////////////////

      /** Main method. Calls private methods and returns a list of Si tracks */
      virtual std::list<Trk::Track*> getTrack (const Trk::TrackSegment&);
      /** New event initialization */
      virtual void newEvent();
      /** New region intialization */
      void newRegion
        (const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&);
      /** End of event tasks       */
      virtual void endEvent();

      ///////////////////////////////////////////////////////////////////
      /** Print internal tool parameters and status                    */
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump(MsgStream&    out) const;
      std::ostream& dump(std::ostream& out) const;

    protected:
      
      ///////////////////////////////////////////////////////////////////
      /** Protected Data                                               */
      ///////////////////////////////////////////////////////////////////
      
      int                                 m_nprint        ;
      std::string                         m_fieldmode     ;  /** Magnetic field mode       */

      Trk::MagneticFieldProperties        m_fieldprop     ;  /** Magnetic field properties */

      /** Tools used  */
      ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle  ;  /** Magnetic field tool */
      
      MagField::IMagFieldSvc*                m_fieldService;


      ToolHandle<InDet::ISiDetElementsRoadMaker>     m_roadmaker  ;  /** Road maker tool     */
      ToolHandle<InDet::ITRT_SeededSpacePointFinder> m_seedmaker  ;  /** Seed maker tool     */
      ToolHandle<Trk::IPropagator>                   m_proptool   ;  /** Propagator tool     */
      ToolHandle<Trk::IUpdator>                      m_updatorTool;  /** Updator tool        */
      ToolHandle<InDet::ISiCombinatorialTrackFinder> m_tracksfinder; /** Combinatorial track finder tool */

      /** Needed for adding material related noise   */
      InDet::SiNoise_bt                    m_noise        ;

      /**ID TRT helper*/
      const TRT_ID* m_trtId;

      /** Track quality cuts to be passed to the combinatorial track finder */
      double                                                   m_xi2max        ; /** max Xi2 for updators */
      double                                                   m_xi2maxNoAdd   ; /** max Xi2 for outliers */
      double                                                   m_xi2maxlink    ; /** max Xi2 for clusters */
      double                                                   m_pTmin         ; /** min pT  */
      int                                                      m_nholesmax     ; /** Max number holes  */
      int                                                      m_dholesmax     ; /** Max gap between holes */
      int                                                      m_nclusmin      ; /** Min number clusters */
      int                                                      m_nwclusmin     ; /** Min number weighted clusters */
      bool                                                     m_bremCorrect   ; /** Optional Brem correction */
      bool                                                     m_propR         ; /** Check seed-TRT segment consistency at large etas */ 
      bool                                                     m_useassoTool   ; /** Use prd-track association tool */
      InDet::TrackQualityCuts                                  m_trackquality  ; 
      std::vector<double>                                      m_errorScale    ; /** Optional error scaling of track parameters  */
      std::multimap<const Trk::PrepRawData*,const Trk::Track*> m_clusterTrack  ; /** Multimap of tracks and associated PRDs  */
      double                                                   m_outlierCut    ; /** Outlier chi2 cut when propagating through the seed */
      bool                                                     m_searchInCaloROI; /** Outlier chi2 cut when propagating through the seed */
      SG::ReadHandleKey<CaloClusterROI_Collection> m_inputClusterContainerName {this,"InputClusterContainerName","InDetCaloClusterROIs", "RHK for CaloClusterROI_Collection"}; 

      ///////////////////////////////////////////////////////////////////
      /** Private Methods                                              */
      ///////////////////////////////////////////////////////////////////

      /** Get Magnetic field properties  */
      void magneticFieldInit(); 

      /** Update track parameters through space point propagation  */
      const Trk::TrackParameters*                            getTP(const Trk::SpacePoint*, const Trk::TrackParameters*,bool&);

      /** Find the corresponding list of Si tracks  */
      std::list<Trk::Track*>                                 findTrack(const Trk::TrackParameters*,const Trk::TrackSegment&);

      /** Add material effects   */
      const Trk::TrackParameters*                            addNoise(double,double,double,double,const Trk::TrackParameters*,int);

      /** Get better track theta initial estimate using the SPs from the seed */
      double                                                 getNewTheta(std::list<const Trk::SpacePoint*>&);

      /** Check consistency of seed and TRT track segment */
      bool                                                   checkSeed(std::list<const Trk::SpacePoint*>&,const Trk::TrackSegment&,const Trk::TrackParameters*);

      /** Modify track parameters if brem correction  */
      const Trk::TrackParameters*                            modifyTrackParameters(const Trk::TrackParameters&,int);

      /** Set the track quality cuts for combinatorial track finding   */
      void                                                   setTrackQualityCuts();

      /** Map PRDs-tracks */
      void                                                   clusterTrackMap(Trk::Track*);

      /** Seed used by another track?  */
      bool                                                   newClusters(const std::list<const Trk::SpacePoint*>&);

      /** Seed SPs used by other high quality tracks? */
      bool                                                   newSeed(const std::list<const Trk::SpacePoint*>&);

      /** Clean-up duplicate tracks  */
      bool                                                   isNewTrack(Trk::Track*);

      /** Eliminate spurious Pixel clusters in track  */
      std::list<Trk::Track*>                                 cleanTrack(std::list<Trk::Track*>);
      
      /** aalonso: Only propagete to the Si if the TRT segment is compatible with a calo measurement */
      bool isCaloCompatible(const Trk::TrackParameters&);
      std::list<double>              m_caloF         ;
      std::list<double>              m_caloE         ;
      double m_phiWidth                              ;
      double m_etaWidth                              ;
      double m_ClusterE				     ;

      MsgStream&    dumpconditions(MsgStream&    out) const;
      MsgStream&    dumpevent     (MsgStream&    out) const;

    };

  MsgStream&    operator << (MsgStream&   ,const TRT_SeededTrackFinder_ATL&);
  std::ostream& operator << (std::ostream&,const TRT_SeededTrackFinder_ATL&); 

}  // end of namespace

#endif // TRT_SeededTrackFinder_ATL
