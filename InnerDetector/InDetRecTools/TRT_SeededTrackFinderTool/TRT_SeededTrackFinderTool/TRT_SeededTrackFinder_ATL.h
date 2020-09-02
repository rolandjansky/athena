/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "InDetRecToolInterfaces/ITRT_SeededSpacePointFinder.h"
#include "TrkEventUtils/EventDataBase.h"

//Magnetic field
//
#include "TrkGeometry/MagneticFieldProperties.h"

// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

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
}

namespace InDet{
  class ISiDetElementsRoadMaker;
  class SiCombinatorialTrackFinderData_xk;

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
      virtual StatusCode initialize() override;
      virtual StatusCode finalize  () override;

      ///////////////////////////////////////////////////////////////////
      /** Main methods for local track finding                         */
      ///////////////////////////////////////////////////////////////////

      /** Main method. Calls private methods and returns a list of Si tracks */
      virtual std::list<Trk::Track*> getTrack (const EventContext& ctx, InDet::ITRT_SeededTrackFinder::IEventData &event_data,
                                               const Trk::TrackSegment&) const override;
      /** New event initialization */
      virtual std::unique_ptr<InDet::ITRT_SeededTrackFinder::IEventData>
         newEvent(const EventContext& ctx, SiCombinatorialTrackFinderData_xk& combinatorialData) const override;
      /** New region intialization */
      virtual std::unique_ptr<InDet::ITRT_SeededTrackFinder::IEventData>
         newRegion(const EventContext& ctx, SiCombinatorialTrackFinderData_xk& combinatorialData,
                   const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&) const override;
      /** End of event tasks       */
      virtual void endEvent(InDet::ITRT_SeededTrackFinder::IEventData &event_data) const override;

      ///////////////////////////////////////////////////////////////////
      /** Print internal tool parameters and status                    */
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump(MsgStream&    out) const override;
      std::ostream& dump(std::ostream& out) const override;

    protected:

       class EventData;
       class EventData : public Trk::EventDataBase<EventData,InDet::ITRT_SeededTrackFinder::IEventData> {
       public:
          friend class TRT_SeededTrackFinder_ATL;
          EventData(SiCombinatorialTrackFinderData_xk& combinatorialData,
                    std::unique_ptr<InDet::ITRT_SeededSpacePointFinder::IEventData> &&spacePointFinderEventData)
             : m_combinaatorialData(&combinatorialData),
               m_spacePointFinderEventData(std::move(spacePointFinderEventData) )
          {}
          virtual InDet::SiCombinatorialTrackFinderData_xk &combinatorialData() override             { return *m_combinaatorialData; }
          virtual const InDet::SiCombinatorialTrackFinderData_xk &combinatorialData() const override { return *m_combinaatorialData; }

          InDet::ITRT_SeededSpacePointFinder::IEventData &spacePointFinderEventData(){ return *m_spacePointFinderEventData; }
          std::multimap<const Trk::PrepRawData*,const Trk::Track*> &clusterTrack() { return m_clusterTrack; }
          std::vector<double>&                                      caloF()        { return m_caloF; }
          std::vector<double>&                                      caloE()        { return m_caloE; }
          const std::vector<double>&                                caloF()  const { return m_caloF; }
          const std::vector<double>&                                caloE()  const { return m_caloE; }
          InDet::SiNoise_bt&                                        noise()        { return m_noise; }
          const InDet::SiNoise_bt&                                  noise()  const { return m_noise; }
       protected:
          SiCombinatorialTrackFinderData_xk                              *m_combinaatorialData;
          std::unique_ptr<InDet::ITRT_SeededSpacePointFinder::IEventData> m_spacePointFinderEventData;
          std::multimap<const Trk::PrepRawData*,const Trk::Track*> m_clusterTrack  ; /** Multimap of tracks and associated PRDs  */
          std::vector<double>              m_caloF         ;
          std::vector<double>              m_caloE         ;

          /** Needed for adding material related noise   */
          InDet::SiNoise_bt                    m_noise        ;
       };

      ///////////////////////////////////////////////////////////////////
      /** Protected Data                                               */
      ///////////////////////////////////////////////////////////////////

      std::string                         m_fieldmode     ;  /** Magnetic field mode       */

      Trk::MagneticFieldProperties        m_fieldprop     ;  /** Magnetic field properties */

      /** Tools used  */

      ToolHandle<InDet::ISiDetElementsRoadMaker>     m_roadmaker  ;  /** Road maker tool     */
      ToolHandle<InDet::ITRT_SeededSpacePointFinder> m_seedmaker  ;  /** Seed maker tool     */
      ToolHandle<Trk::IPropagator>                   m_proptool   ;  /** Propagator tool     */
      ToolHandle<Trk::IUpdator>                      m_updatorTool;  /** Updator tool        */
      ToolHandle<InDet::ISiCombinatorialTrackFinder> m_tracksfinder; /** Combinatorial track finder tool */

      SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj",
        "fieldCondObj", "Name of the Magnetic Field conditions object key"};

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
      double                                                   m_outlierCut    ; /** Outlier chi2 cut when propagating through the seed */
      bool                                                     m_searchInCaloROI; /** Outlier chi2 cut when propagating through the seed */
      SG::ReadHandleKey<CaloClusterROI_Collection> m_inputClusterContainerName {this,"InputClusterContainerName","InDetCaloClusterROIs", "RHK for CaloClusterROI_Collection"};

      ///////////////////////////////////////////////////////////////////
      /** Private Methods                                              */
      ///////////////////////////////////////////////////////////////////

      /** Get Magnetic field properties  */
      void magneticFieldInit();

      /** Update track parameters through space point propagation  */
       const Trk::TrackParameters*                            getTP(MagField::AtlasFieldCache& fieldCache, const Trk::SpacePoint*,
                                                                    const Trk::TrackParameters*,
                                                                    bool&,
                                                                    InDet::TRT_SeededTrackFinder_ATL::EventData &event_data) const;

      /** Find the corresponding list of Si tracks  */
      std::list<Trk::Track*>                                 findTrack(const EventContext& ctx, MagField::AtlasFieldCache& fieldCache,
                                                                       InDet::TRT_SeededTrackFinder_ATL::EventData &event_data,
                                                                       const Trk::TrackParameters*,const Trk::TrackSegment&) const;

      /** Add material effects   */
      const Trk::TrackParameters*                            addNoise(double,double,double,double,const Trk::TrackParameters*,int) const;

      /** Get better track theta initial estimate using the SPs from the seed */
      double                                                 getNewTheta(std::vector<const Trk::SpacePoint*>&) const;

      /** Check consistency of seed and TRT track segment */
      bool                                                   checkSeed(std::vector<const Trk::SpacePoint*>&,const Trk::TrackSegment&,const Trk::TrackParameters*) const;

      /** Modify track parameters if brem correction  */
      const Trk::TrackParameters*                            modifyTrackParameters(const Trk::TrackParameters&,int) const;

      /** Set the track quality cuts for combinatorial track finding   */
      void                                                   setTrackQualityCuts();

      /** Map PRDs-tracks */
      void                                                   clusterTrackMap(Trk::Track*,
                                                                             InDet::TRT_SeededTrackFinder_ATL::EventData &event_data)
                                                                            const;

      /** Seed used by another track?  */
      bool                                                   newClusters(const std::vector<const Trk::SpacePoint*>&,
                                                                         InDet::TRT_SeededTrackFinder_ATL::EventData &event_data) const;

      /** Seed SPs used by other high quality tracks? */
      bool                                                   newSeed(const std::vector<const Trk::SpacePoint*>&,
                                                                     InDet::TRT_SeededTrackFinder_ATL::EventData &event_data) const;

      /** Clean-up duplicate tracks  */
      bool                                                   isNewTrack(Trk::Track*,
                                                                        InDet::TRT_SeededTrackFinder_ATL::EventData &event_data) const;

      /** Eliminate spurious Pixel clusters in track  */
      std::list<Trk::Track*>                                 cleanTrack(std::list<Trk::Track*>) const;

      /** aalonso: Only propagete to the Si if the TRT segment is compatible with a calo measurement */
      bool isCaloCompatible(const Trk::TrackParameters&, const InDet::TRT_SeededTrackFinder_ATL::EventData &event_data) const;
      double m_phiWidth                              ;
      double m_etaWidth                              ;
      double m_ClusterEt				                     ;

      MsgStream&    dumpconditions(MsgStream&    out) const;

    };

}  // end of namespace

#endif // TRT_SeededTrackFinder_ATL
