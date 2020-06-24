/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTrackingGeometryBuilderCond.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONTRACKINGGEOMETRY_MUONTRACKINGGEOMETRYBUILDERCOND_H
#define MUONTRACKINGGEOMETRY_MUONTRACKINGGEOMETRYBUILDERCOND_H

//Trk
#include "TrkDetDescrInterfaces/IGeometryBuilderCond.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeArrayCreator.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeHelper.h"
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "MuonTrackingGeometry/MuonStationBuilder.h"
#include "MuonTrackingGeometry/MuonInertMaterialBuilder.h"
// EnvelopeDefinitionService
#include "SubDetectorEnvelopes/RZPair.h"
#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h"

namespace Trk {
 class TrackingGeometry;
 class Material; 
 class VolumeBounds;
 class ITrackingVolumeBuilder;

 typedef std::pair< SharedObject<const TrackingVolume>, Amg::Vector3D> TrackingVolumeOrderPosition;
 typedef std::pair< SharedObject<const TrackingVolume>, const Amg::Transform3D*> TrackingVolumeNavOrder;

}

namespace Muon {

  typedef std::vector<double> Span;
     
  /** @class MuonTrackingGeometryBuilderCond
  
      The Muon::MuonTrackingGeometryBuilderCond retrieves MuonStationBuilder and MuonInertMaterialBuilder
      for the Muon Detector sub detectors and combines the given Volumes to a full Trk::TrackingGeometry.
      
      Inheriting directly from IGeometryBuilderCond it can use the protected member functions of the IGeometryBuilderCond
      to glue Volumes together and exchange BoundarySurfaces
      
      @author Sarka.Todorova@cern.ch
    */
    
  class MuonTrackingGeometryBuilderCond : public AthAlgTool,
                               virtual public Trk::IGeometryBuilderCond {
  public:
      /** Constructor */
      MuonTrackingGeometryBuilderCond(const std::string&,const std::string&,const IInterface*);
      /** Destructor */
      virtual ~MuonTrackingGeometryBuilderCond() = default;
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
      /** TrackingGeometry Interface method */
      std::pair<EventIDRange, const Trk::TrackingGeometry*> trackingGeometry(const EventContext& ctx, std::pair<EventIDRange, const Trk::TrackingVolume*> tVolPair ) const; 

      /** The unique signature */
      Trk::GeometrySignature geometrySignature() const { return Trk::MS; }
      
    private:
      /** Private struct to contain local variables we dont want to be global in this class*/
      struct LocalVariablesContainer {
	double m_innerBarrelRadius;
	double m_outerBarrelRadius;
	double m_innerEndcapZ;
	double m_outerEndcapZ;
	bool m_adjustStatic;
	bool m_static3d;
	std::vector<double> m_zPartitions;
	std::vector<int>       m_zPartitionsType;
	std::vector<float>    m_adjustedPhi;
	std::vector<int>       m_adjustedPhiType;
	std::vector<const Span*>                      m_spans;             // for clearing
	std::vector<std::vector<std::vector<std::vector<std::pair<int,float> > > > >  m_hPartitions;
	std::vector<double>                              m_shieldZPart;
	std::vector<std::vector<std::pair<int,float> > >   m_shieldHPart;
	std::map<const Trk::DetachedTrackingVolume*,std::vector<const Trk::TrackingVolume*>* > m_blendMap;
	std::vector<const Trk::DetachedTrackingVolume*> m_blendVols;
	const std::vector<std::vector<std::pair<const Trk::DetachedTrackingVolume*,const Span*> >* >* m_stationSpan; 
	const std::vector<std::vector<std::pair<const Trk::DetachedTrackingVolume*,const Span*> >* >* m_inertSpan; 
	RZPairVector     m_msCutoutsIn;
	RZPairVector     m_msCutoutsOut;
	Trk::Material  m_muonMaterial;               //!< the (empty) material
	Trk::TrackingVolume*        m_standaloneTrackingVolume;   // muon standalone tracking volume
      };

      /** Private method to find z/phi span of detached volumes */
      const Span* findVolumeSpan(const Trk::VolumeBounds* volBounds, Amg::Transform3D transf, double zTol, double phiTol, LocalVariablesContainer &aLVC) const;
      const std::vector<std::vector<std::pair<const Trk::DetachedTrackingVolume*,const Span*> >* >* findVolumesSpan(const std::vector<const Trk::DetachedTrackingVolume*>* objs, double zTol, double phiTol, LocalVariablesContainer &aLVC) const;
      /** Private methods to define subvolumes and fill them with detached volumes */
      const Trk::TrackingVolume* processVolume( const Trk::Volume*, int, int, std::string, LocalVariablesContainer &aLVC) const; 
      const Trk::TrackingVolume* processVolume( const Trk::Volume*, int, std::string, LocalVariablesContainer &aLVC) const; 
      const Trk::TrackingVolume* processShield( const Trk::Volume*, int, std::string, LocalVariablesContainer &aLVC) const; 
      /** Private method to check volume properties */
      void checkVolume(const Trk::TrackingVolume*) const;
      /** Private method to find detached volumes */
      std::vector<const Trk::DetachedTrackingVolume*>* getDetachedObjects(const Trk::Volume*, std::vector<const Trk::DetachedTrackingVolume*>&, LocalVariablesContainer &aLVC ,int mode=0) const;
      /** Private method to check if constituent enclosed */
      bool enclosed(const Trk::Volume*, const Muon::Span*, LocalVariablesContainer &aLVC) const;
      /** Private method to retrieve z partition */
      void getZParts(LocalVariablesContainer &aLVC) const;
      /** Private method to retrieve phi partition */
      void getPhiParts(int, LocalVariablesContainer &aLVC) const;
      /** Private method to retrieve h partition */
      void getHParts(LocalVariablesContainer &aLVC) const;
      /** Private method to retrieve shield partition */
      void getShieldParts(LocalVariablesContainer & aLVC) const; 
      /** Private method to calculate volume */
      double calculateVolume(const Trk::Volume*) const;
      /** Private method to blend the inert material */
      void blendMaterial(LocalVariablesContainer &aLVC) const;
     
      ToolHandle<Trk::IDetachedTrackingVolumeBuilder>      m_stationBuilder{this,"MuonStationBuilder","Muon::MuonStationBuilder/MuonStationBuilder"};                //!< A Tool for station type creation

      ToolHandle<Trk::IDetachedTrackingVolumeBuilder>      m_inertBuilder{this,"InertMaterialBuilder","Muon::MuonInertMaterialBuilder/MuonInertMaterialBuilder"};      //!< A Tool for inert object  creation
      
      ToolHandle<Trk::ITrackingVolumeArrayCreator>         m_trackingVolumeArrayCreator{this,"TrackingVolumeArrayCreator","Trk::TrackingVolumeArrayCreator/TrackingVolumeArrayCreator"};    //!< Helper Tool to create TrackingVolume Arrays

      ToolHandle<Trk::ITrackingVolumeHelper>               m_trackingVolumeHelper{this,"TrackingVolumeHelper","Trk::TrackingVolumeHelper/TrackingVolumeHelper"};          //!< Helper Tool to create TrackingVolumes

      ServiceHandle<IEnvelopeDefSvc>                       m_enclosingEnvelopeSvc{this,"EnvelopeDefinitionSvc","AtlasEnvelopeDefSvc","n"};   //!< service to provide input volume size
   
      Gaudi::Property<bool>                                m_muonSimple{this,"SimpleMuonGeometry",false};
      Gaudi::Property<bool>                                m_loadMSentry{this,"LoadMSEntry",false};
      Gaudi::Property<bool>                                m_muonActive{this,"BuildActiveMaterial",true};
      Gaudi::Property<bool>                                m_muonInert{this,"BuildInertMaterial",true};

      // Overall Dimensions
      Gaudi::Property<double>        m_innerBarrelRadius{this,"InnerBarrelRadius",4255.};    //!< minimal extend in radial dimension of the muon barrel
      Gaudi::Property<double>        m_outerBarrelRadius{this,"OuterBarrelRadius",13910.}; //!< maximal extend in radial dimension of the muon barrel
      Gaudi::Property<double>        m_barrelZ{this,"BarrelZ",6785.};                                              //!< maximal extend in z of the muon barrel
      Gaudi::Property<double>        m_innerEndcapZ{this,"InnerEndcapZ",12900.};              //!< maximal extend in z of the inner part of muon endcap 
      Gaudi::Property<double>        m_outerEndcapZ{this,"OuterEndcapZ",26046.};             //!< maximal extend in z of the outer part of muon endcap
      double                              m_bigWheel;                 //!< maximal extend in z of the big wheel
      double                              m_outerWheel;               //!< minimal extend in z of the outer wheel (EO)
      double                              m_ectZ;                     //!< minimal extent in z of the ECT
      double                              m_beamPipeRadius;
      double                              m_innerShieldRadius;
      double                              m_outerShieldRadius;
      double                              m_diskShieldZ;

      Gaudi::Property<int>                m_barrelEtaPartition{this,"EtaBarrelPartitions",9};
      Gaudi::Property<int>                m_innerEndcapEtaPartition{this,"EtaInnerEndcapPartitions",3};
      Gaudi::Property<int>                m_outerEndcapEtaPartition{this,"EtaOuterEndcapPartitions",3};
      Gaudi::Property<int>                m_phiPartition{this,"PhiPartitions",16};

      Gaudi::Property<bool>              m_adjustStatic{this,"AdjustStatic",true};
      Gaudi::Property<bool>              m_static3d{this,"StaticPartition3D",true};

      Gaudi::Property<bool>               m_blendInertMaterial{this,"BlendInertMaterial",false}; 
      Gaudi::Property<bool>               m_removeBlended{this,"RemoveBlendedMaterialObjects",false};
      mutable std::atomic_uint            m_inertPerm{0};                  // number of perm objects
      Gaudi::Property<double>           m_alignTolerance{this,"AlignmentPositionTolerance",0.};
      Gaudi::Property<int>                m_colorCode{this,"ColorCode",0};
      Gaudi::Property<int>                  m_activeAdjustLevel{this,"ActiveAdjustLevel",2};
      Gaudi::Property<int>                  m_inertAdjustLevel{this,"InertAdjustLevel",1};

      mutable std::atomic_uint            m_frameNum{0};      
      mutable std::atomic_uint            m_frameStat{0};      

      Gaudi::Property<std::string>        m_entryVolume{this,"EntryVolumeName","MuonSpectrometerEntrance"};
      Gaudi::Property<std::string>        m_exitVolume{this,"ExitVolumeName","All::Container::CompleteDetector"};

      
      const std::vector<const Trk::DetachedTrackingVolume*>*   m_stations;    // muon chambers 
      const std::vector<const Trk::DetachedTrackingVolume*>*   m_inertObjs;   // muon inert material 
      //mutable std::vector<std::pair<std::string,std::pair<double, unsigned int> > >   m_dilFact;
      //mutable std::vector<Trk::MaterialProperties>               m_matProp;
      typedef ServiceHandle<IChronoStatSvc> IChronoStatSvc_t;
      IChronoStatSvc_t m_chronoStatSvc{this,"ChronoStatSvc","ChronoStatSvc","n"};      
 };


} // end of namespace


#endif //MUONTRACKINGGEOMETRY_MUONTRACKINGGEOMETRYBUILDER_H


