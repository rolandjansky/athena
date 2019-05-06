// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "InDetRecToolInterfaces/ISiTrackMaker.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "BeamSpotConditionsData/BeamSpotData.h"
#include "InDetRecToolInterfaces/ISeedToTrackConversionTool.h"
#include "InDetRecToolInterfaces/ISiCombinatorialTrackFinder.h"
#include "InDetRecToolInterfaces/ISiDetElementsRoadMaker.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"
#include "TrkGeometry/MagneticFieldProperties.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <array>
#include <iosfwd>
#include <list>

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
      virtual ~SiTrackMaker_xk() = default;
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      ///////////////////////////////////////////////////////////////////
      // Main methods for local track finding
      ///////////////////////////////////////////////////////////////////
      
      virtual std::list<Trk::Track*>
      getTracks(const std::list<const Trk::SpacePoint*>&);

      virtual std::list<Trk::Track*>
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

      ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle{this, "MagFieldSvc", "AtlasFieldSvc"};
      ToolHandle<InDet::ISiDetElementsRoadMaker> m_roadmaker{this, "RoadTool", "InDet::SiDetElementsRoadMaker_xk"};
      PublicToolHandle<InDet::ISiCombinatorialTrackFinder> m_tracksfinder{this, "CombinatorialTrackFinder", "InDet::SiCombinatorialTrackFinder_xk"};
      ToolHandle<InDet::ISeedToTrackConversionTool> m_seedtrack{this, "SeedToTrackConversion", "InDet::SeedToTrackConversionTool"};

      SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};
      SG::ReadHandleKey<CaloClusterROI_Collection> m_caloCluster{this, "InputClusterContainerName", "InDetCaloClusterROIs"};
      SG::ReadHandleKey<CaloClusterROI_Collection> m_caloHad{this, "InputHadClusterContainerName", "InDetHadCaloClusterROIs"};

      IntegerProperty m_seedsfilter{this, "SeedsFilterLevel", 2, "Level of seeds filer"};
      UnsignedIntegerProperty m_wrongcluster{this, "GoodSeedClusterCount", 10, "Max lentgh of thtrack"};
      StringProperty m_fieldmode{this, "MagneticFieldMode", "MapSolenoid", "Mode of magnetic field"};
      StringProperty m_patternName{this, "TrackPatternRecoInfo", "SiSPSeededFinder", "Name of the pattern recognition"};
      BooleanProperty m_usePix{this, "usePixel", true, "flags to set whether to use pixel/sct cluster, irrespective of what is in event"};
      BooleanProperty m_useSct{this, "useSCT", true};
      BooleanProperty m_useassoTool{this, "UseAssociationTool", false, "Use prd-track association tool"};
      BooleanProperty m_cosmicTrack{this, "CosmicTrack", false, "Is it cosmic track"};
      BooleanProperty m_multitracks{this, "doMultiTracksProd", false};
      BooleanProperty m_useBremModel{this, "useBremModel", false};
      BooleanProperty m_useCaloSeeds{this, "doCaloSeededBrem", false};
      BooleanProperty m_useSSSfilter{this, "useSSSseedsFilter", true};
      BooleanProperty m_useHClusSeed{this, "doHadCaloSeedSSS", false, "Hadronic Calorimeter Seeds"};
      BooleanProperty m_ITKGeomtry{this, "ITKGeometry", false, "ITK geometry"};
      BooleanProperty m_seedsegmentsWrite{this, "SeedSegmentsWrite", false, "Call seed to track conversion"};
      DoubleProperty m_xi2max{this, "Xi2max", 15., "max Xi2 for updators"};
      DoubleProperty m_xi2maxNoAdd{this, "Xi2maxNoAdd", 35., "max Xi2 for clusters"};
      DoubleProperty m_xi2maxlink{this, "Xi2maxlink", 200., "max Xi2 for clusters"};
      DoubleProperty m_pTmin{this, "pTmin", 500., "min pT"};
      DoubleProperty m_pTminBrem{this, "pTminBrem", 1000., "min pT for Brem mode"};
      DoubleProperty m_pTminSSS{this, "pTminSSS", 1000., "min pT for SSS filtering"};
      DoubleProperty m_distmax{this, "MaxDistanceForSCTsp", 5.};
      DoubleProperty m_xi2multitracks{this, "Xi2maxMultiTracks", 3., "max Xi2 for multi tracks"};
      IntegerProperty m_nholesmax{this, "nHolesMax", 2, "Max number holes"};
      IntegerProperty m_dholesmax{this, "nHolesGapMax", 2, "Max holes gap"};
      IntegerProperty m_nclusmin{this, "nClustersMin", 6,  "Min number clusters"};
      IntegerProperty m_nwclusmin{this, "nWeightedClustersMin", 6, "Min umber weighted clusters(pix=2 sct=1)"};
      DoubleProperty m_phiWidth{this, "phiWidth", 0.3};
      DoubleProperty m_etaWidth{this, "etaWidth", 0.3};

      // Updated only in initialize method
      Trk::TrackInfo                 m_trackinfo     ;
      bool                           m_heavyion{false}; // Is it heavy ion events
      Trk::MagneticFieldMode         m_fieldModeEnum{Trk::FullField};

      // Counters
      int                            m_inputseeds{0}; // Number input seeds
      int                            m_goodseeds{0}; // Number good  seeds
      int                            m_findtracks{0};  // Numbe found tracks

      // Flag for dump method
      int                            m_nprint{}; // Kind output information

      // Updated by many methods
      std::multimap<const Trk::PrepRawData*, const Trk::Track*> m_clusterTrack;

      // Updated only by newEvent and newTrigEvent methods
      bool                           m_pix{false};
      bool                           m_sct{false};

      // Updated ony by newEvent method
      std::list<double>              m_caloF         ;
      std::list<double>              m_caloR         ;
      std::list<double>              m_caloZ         ;
      std::list<double>              m_hadF          ;
      std::list<double>              m_hadR          ;
      std::list<double>              m_hadZ          ;
      double m_xybeam[2]                             ;

      // Caches for the getTracks method with two arguments.
      // These have to be dropped.
      bool m_dbm{false};
      std::array<double, 9> m_par;

      ///////////////////////////////////////////////////////////////////
      // Methods 
      ///////////////////////////////////////////////////////////////////

 
      const Trk::TrackParameters* getAtaPlane(bool,
                                              const std::list<const Trk::SpacePoint*>&,
                                              std::array<double, 9>& par) const;
      const Trk::TrackParameters* getAtaPlaneDBM(const std::list<const Trk::SpacePoint*>&,
                                                 std::array<double, 9>& par) const;

      bool globalPositions(const Trk::SpacePoint*,const Trk::SpacePoint*,const Trk::SpacePoint*,
                           double*,double*,double*) const;

      bool globalPosition(const Trk::SpacePoint*,double*,double*) const;
      void globalDirections(double*,double*,double*,double*,double*,double*) const;
      InDet::TrackQualityCuts setTrackQualityCuts(bool simpleTrack);
       void detectorElementsSelection(std::list<const InDetDD::SiDetectorElement*>&, bool dbm) const;
      bool newSeed    (const std::list<const Trk::SpacePoint*>&);
      bool isNewTrack(Trk::Track*,
                      const std::multimap<const Trk::PrepRawData*,
                                          const Trk::Track*>& clusterTrack) const;
      bool isCaloCompatible(const std::array<double, 9>& par) const;
      bool isHadCaloCompatible(const std::array<double, 9>& par) const;
      bool isDBMSeeds(const Trk::SpacePoint*) const;
      void clusterTrackMap(Trk::Track*,
                           std::multimap<const Trk::PrepRawData*,
                                         const Trk::Track*>& clusterTrack) const;

      MsgStream& dumpconditions(MsgStream&    out) const;
      MsgStream& dumpevent     (MsgStream&    out) const;
    };

    MsgStream&    operator << (MsgStream&   ,const SiTrackMaker_xk&);
    std::ostream& operator << (std::ostream&,const SiTrackMaker_xk&); 

} // end of name space

#endif // SiTrackMaker_xk_H

