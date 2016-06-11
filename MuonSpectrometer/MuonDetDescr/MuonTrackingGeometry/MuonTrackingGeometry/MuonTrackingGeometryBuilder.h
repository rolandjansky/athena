/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTrackingGeometryBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONTRACKINGGEOMETRY_MUONTRACKINGGEOMETRYBUILDER_H
#define MUONTRACKINGGEOMETRY_MUONTRACKINGGEOMETRYBUILDER_H

//Trk
#include "TrkDetDescrInterfaces/IGeometryBuilder.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "MuonTrackingGeometry/MuonStationBuilder.h"
#include "MuonTrackingGeometry/MuonInertMaterialBuilder.h"

namespace Trk {
 class TrackingGeometry;
 class Material; 
 class VolumeBounds;
 class ITrackingVolumeBuilder;
 class ITrackingVolumeHelper;
 class ITrackingVolumeArrayCreator;

 typedef std::pair< SharedObject<const TrackingVolume>, Amg::Vector3D> TrackingVolumeOrderPosition;
 typedef std::pair< SharedObject<const TrackingVolume>, const Amg::Transform3D*> TrackingVolumeNavOrder;

}

class IEnvelopeDefSvc; 
 
namespace Muon {

  typedef std::vector<double> Span;
     
  /** @class MuonTrackingGeometryBuilder
  
      The Muon::MuonTrackingGeometryBuilder retrieves MuonStationBuilder and MuonInertMaterialBuilder
      for the Muon Detector sub detectors and combines the given Volumes to a full Trk::TrackingGeometry.
      
      Inheriting directly from IGeometryBuilder it can use the protected member functions of the IGeometryBuilder
      to glue Volumes together and exchange BoundarySurfaces
      
      @author Sarka.Todorova@cern.ch
    */
    
  class MuonTrackingGeometryBuilder : public AthAlgTool,
                               virtual public Trk::IGeometryBuilder {
  public:
      /** Constructor */
      MuonTrackingGeometryBuilder(const std::string&,const std::string&,const IInterface*);
      /** Destructor */
      virtual ~MuonTrackingGeometryBuilder();
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
      /** TrackingGeometry Interface methode */
      const Trk::TrackingGeometry* trackingGeometry(const Trk::TrackingVolume* tvol = 0) const; 

      /** The unique signature */
      Trk::GeometrySignature geometrySignature() const { return Trk::MS; }
      
    private:
      /** Private method to find z/phi span of detached volumes */
      const Span* findVolumeSpan(const Trk::VolumeBounds* volBounds, Amg::Transform3D transf, double zTol, double phiTol) const;
      const std::vector<std::vector<std::pair<const Trk::DetachedTrackingVolume*,const Span*> >* >* findVolumesSpan(const std::vector<const Trk::DetachedTrackingVolume*>*& objs, double zTol, double phiTol) const;
      /** Private methods to define subvolumes and fill them with detached volumes */
      const Trk::TrackingVolume* processVolume( const Trk::Volume*, int, int, std::string) const; 
      const Trk::TrackingVolume* processVolume( const Trk::Volume*, int, std::string) const; 
      const Trk::TrackingVolume* processShield( const Trk::Volume*, int, std::string) const; 
      /** Private method to check volume properties */
      void checkVolume(const Trk::TrackingVolume*) const;
      /** Private method to find detached volumes */
      std::vector<const Trk::DetachedTrackingVolume*>* getDetachedObjects(const Trk::Volume*, std::vector<const Trk::DetachedTrackingVolume*>& ,int mode=0) const;
      /** Private method to check if constituent enclosed */
      bool enclosed(const Trk::Volume*, const Muon::Span*) const;
      /** Private method to retrieve z partition */
      void getZParts() const;
      /** Private method to retrieve phi partition */
      void getPhiParts(int) const;
      /** Private method to retrieve h partition */
      void getHParts() const;
      /** Private method to retrieve shield partition */
      void getShieldParts() const; 
      /** Private method to calculate volume */
      double calculateVolume(const Trk::Volume*) const;
      /** Private method to blend the inert material */
      void blendMaterial() const;
     
      unsigned int                                         m_magFieldMode;                  //!< switch the magnetic field mode

      ToolHandle<Trk::IDetachedTrackingVolumeBuilder>      m_stationBuilder;                //!< A Tool for station type creation

      ToolHandle<Trk::IDetachedTrackingVolumeBuilder>      m_inertBuilder;                  //!< A Tool for inert object  creation
      
      ToolHandle<Trk::ITrackingVolumeArrayCreator>         m_trackingVolumeArrayCreator;    //!< Helper Tool to create TrackingVolume Arrays

      ToolHandle<Trk::ITrackingVolumeHelper>               m_trackingVolumeHelper;          //!< Helper Tool to create TrackingVolumes

      ServiceHandle<IEnvelopeDefSvc>                       m_enclosingEnvelopeSvc;   //!< service to provide input volume size  
   
      bool                                m_muonSimple;
      bool                                m_loadMSentry;
      bool                                m_muonActive;
      bool                                m_muonInert;

      // Overall Dimensions
      mutable double                      m_innerBarrelRadius;             //!< minimal extend in radial dimension of the muon barrel
      mutable double                      m_outerBarrelRadius;             //!< maximal extend in radial dimension of the muon barrel
      mutable double                      m_barrelZ;                  //!< maximal extend in z of the muon barrel
      double                              m_innerEndcapZ;             //!< maximal extend in z of the inner part of muon endcap 
      double                              m_outerEndcapZ;             //!< maximal extend in z of the outer part of muon endcap
      double                              m_bigWheel;                 //!< maximal extend in z of the big wheel
      double                              m_outerWheel;               //!< minimal extend in z of the outer wheel (EO)
      double                              m_ectZ;                     //!< minimal extent in z of the ECT
      double                              m_beamPipeRadius;
      double                              m_innerShieldRadius;
      double                              m_outerShieldRadius;
      double                              m_diskShieldZ;

      mutable Trk::Material               m_muonMaterial;               //!< the (empty) material

      mutable Trk::TrackingVolume*        m_standaloneTrackingVolume;   // muon standalone tracking volume                 
      int                                 m_barrelEtaPartition;
      int                                 m_innerEndcapEtaPartition;
      int                                 m_outerEndcapEtaPartition;
      int                                 m_phiPartition;
      mutable bool                        m_adjustStatic;
      mutable bool                        m_static3d;
      bool                                m_blendInertMaterial; 
      bool                                m_removeBlended;
      mutable unsigned int                m_inertPerm;                  // number of perm objects
      mutable double                      m_alignTolerance;
      int                                 m_colorCode;
      mutable int                         m_activeAdjustLevel;
      mutable int                         m_inertAdjustLevel;

      mutable unsigned int                m_frameNum;      
      mutable unsigned int                m_frameStat;      

      std::string                         m_entryVolume;
      std::string                         m_exitVolume;
      
      mutable const std::vector<const Trk::DetachedTrackingVolume*>*    m_stations;    // muon chambers 
      mutable const std::vector<const Trk::DetachedTrackingVolume*>*    m_inertObjs;   // muon inert material 
      mutable const std::vector<std::vector<std::pair<const Trk::DetachedTrackingVolume*,const Span*> >* >* m_stationSpan; 
      mutable const std::vector<std::vector<std::pair<const Trk::DetachedTrackingVolume*,const Span*> >* >* m_inertSpan; 
      mutable std::vector<const Span*>                            m_spans;             // for clearing
      mutable std::vector<double>                                 m_zPartitions;
      mutable std::vector<int>                                    m_zPartitionsType;
      mutable std::vector<float>                                  m_adjustedPhi;
      mutable std::vector<int>                                    m_adjustedPhiType;
      mutable std::vector<std::vector<std::vector<std::vector<std::pair<int,float> > > > > m_hPartitions;
      mutable std::vector<double>                                 m_shieldZPart;
      mutable std::vector<std::vector<std::pair<int,float> > >   m_shieldHPart;
      //mutable std::vector<std::pair<std::string,std::pair<double, unsigned int> > >   m_dilFact;
      mutable std::vector<Trk::MaterialProperties>               m_matProp;
      mutable std::map<const Trk::DetachedTrackingVolume*,std::vector<const Trk::TrackingVolume*>* > m_blendMap;
      mutable std::vector<const Trk::DetachedTrackingVolume*>  m_blendVols;
      typedef ServiceHandle<IChronoStatSvc> IChronoStatSvc_t;
      IChronoStatSvc_t              m_chronoStatSvc;      
 };


} // end of namespace


#endif //MUONTRACKINGGEOMETRY_MUONTRACKINGGEOMETRYBUILDER_H


