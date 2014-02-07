/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackingVolumeHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_ITRACKINGVOLUMEHELPER_H
#define TRKDETDESCRINTERFACES_ITRACKINGVOLUMEHELPER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk - template classes & enums
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/BinningType.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
#include "TrkVolumes/BoundarySurface.h"
#include "GeoPrimitives/GeoPrimitives.h"
//STL
#include <string>

namespace Trk {

  class VolumeBounds;
  class CylinderVolumeBounds;
  class Material;
  class Layer;
  class TrackingVolume;
  
  /** Interface ID for ITrackingVolumeHelpers*/  
  static const InterfaceID IID_ITrackingVolumeHelper("ITrackingVolumeHelper", 1, 0);
  
  /** @class ITrackingVolumeHelper
    Interface class ITrackingVolumeHelpers
    It inherits from IAlgTool. 

     Gives an interface to Helper Tool implemntations
    
     @author Andreas.Salzburger@cern.ch
    */
  class ITrackingVolumeHelper : virtual public IAlgTool {
    
    public:
      /**Virtual destructor*/
      virtual ~ITrackingVolumeHelper(){}
      
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_ITrackingVolumeHelper; }

      /** TrackingVolumeBuilder interface method - returns vector of Volumes */
      virtual TrackingVolume* trackingVolumeCylinderLayers(
                                                  Amg::Transform3D* trans,
                                                  CylinderVolumeBounds* bounds,
                                                  Material& matprop,
                                                  std::vector<double> radii,
                                                  std::vector<double> envelopeCovers,
                                                  const std::string& volumeName="UndefinedVolume" ,
                                                  int materialBinsRZ = 10, int materialBinsPhi = 1,
                                                  BinningType btype = arbitrary,
                                                  bool redoNavigation = false) const = 0;

      /** TrackingVolumeBuilder interface method - returns vector of Volumes */
      virtual TrackingVolume* trackingVolumeDiscLayers(
                                                  Amg::Transform3D* trans,
                                                  CylinderVolumeBounds* bounds,
                                                  Material& matprop,
                                                  std::vector<double> zPos,
                                                  std::vector<double> envelopeInnerCovers,
                                                  std::vector<double> envelopeOuterCovers,
                                                  const std::string& volumeName="UndefinedVolume",
                                                  int materialBinsRZ = 10, int materialBinsPhi = 10,
                                                  BinningType btype = arbitrary,
                                                  bool redoNavigation = false) const = 0;

      /** TrackingVolumeHelper interface method - creation of  Gap volume with a single layer */
      virtual TrackingVolume* createCylindricalGapVolume(double rMin, double rMax, double zMin, double zMax,
                                                         Material& matprop,
                                                         const std::string& volumeName ="UndefinedVolume",
                                                         bool cylinderLayer = true,
                                                         double coverOne = 25.,
                                                         double coverTwo = 25.,
                                                         int materialBinsRZ = 10, int materialBinsPhi = 10,
                                                         bool redoNavigation = false) const = 0;

      /** Create a one level higher TrackingVolue */
      virtual TrackingVolume* createCylindricalTopLevelVolume(
                                                   const std::vector<const TrackingVolume*>& volumes,
                                                   Material& matprop,
                                                   const std::string& volumeName ="UndefinedVolume",
                                                   bool glue=false) const =0;

      /** Reshape a Tracking Volume 
        - be careful with that as the layerarray is only copied */
      virtual const TrackingVolume* resizeCylindricalTrackingVolume(
                                                        const TrackingVolume* tvol,
                                                        double rMin, double rMax,
                                                        double zMin, double zMax) const = 0;


      /** Glue Volume method: One to one
           --- Neccessary as friendship cannot be inherited: your father's friend isn't necessary yours ---
          */ 
      virtual void glueTrackingVolumes(const TrackingVolume& firstVol,
                                       BoundarySurfaceFace firstFace,
                                       const TrackingVolume& secondVol,
                                       BoundarySurfaceFace secondFace) const = 0;

      /** Glue Volume method: One to many
           --- Neccessary as friendship cannot be inherited: your father's friend isn't necessary yours ---
          */ 
      virtual void glueTrackingVolumes(const TrackingVolume& firstVol,
                                       BoundarySurfaceFace firstFace,
                                       const std::vector<const TrackingVolume*>& secondVolumes,
                                       BoundarySurfaceFace secondFace) const = 0;

      /** Glue Volume method: many to many plus enveloping volume
           --- Neccessary as friendship cannot be inherited: your father's friend isn't necessary yours ---
          */ 
      virtual Trk::TrackingVolume* glueTrackingVolumeArrays(const TrackingVolume& firstVol,
                                                            BoundarySurfaceFace firstFace,
                                                            const TrackingVolume& secondVol,
                                                            BoundarySurfaceFace secondFace,
                                                            std::string name) const = 0;

    
      /**  Glue Volume method: set inside Volume 
         --- Neccessary as friendship cannot be inherited: your father's friend isn't necessary yours ---
      */
      virtual void setInsideTrackingVolume(const TrackingVolume& tvol,
                                           BoundarySurfaceFace face,
                                           const TrackingVolume* insidevol) const = 0;

      /**  Glue Volume mehtod : set inside Volume Array
          --- Neccessary as friendship cannot be inherited: your father's friend isn't necessary yours ---
      */
      virtual void setInsideTrackingVolumeArray(const TrackingVolume& tvol,
                                                BoundarySurfaceFace face,
                                                BinnedArray<TrackingVolume>* insidevolarray) const = 0;
      
      virtual void setInsideTrackingVolumeArray(const TrackingVolume& tvol,
                                                BoundarySurfaceFace face,
                                                SharedObject<BinnedArray<TrackingVolume> > insidevolarray) const = 0;
      
      /**  Glue Volume method: set outside Volume
           --- Neccessary as friendship cannot be inherited: your father's friend isn't necessary yours ---
      */
      virtual void setOutsideTrackingVolume(const TrackingVolume& tvol,
                                            BoundarySurfaceFace face,
                                            const TrackingVolume* outsidevol) const = 0;

      /**  Glue Volume method: set outside Volume array
           --- Neccessary as friendship cannot be inherited: your father's friend isn't necessary yours ---
       */
      virtual void setOutsideTrackingVolumeArray(const TrackingVolume& tvol,
                                                 BoundarySurfaceFace face,
                                                 BinnedArray<TrackingVolume>* outsidevolarray) const = 0;

      virtual void setOutsideTrackingVolumeArray(const TrackingVolume& tvol,
                                                 BoundarySurfaceFace face,
                                                 SharedObject<BinnedArray<TrackingVolume> > outsidevolarray) const = 0;

      /** Validation Action:
          Can be implemented optionally, outside access to internal validation steps */
      virtual void validationAction() const {}
                         
  };

} // end of namespace

#endif // TRKDETDESCRINTERFACES_IITRACKINGVOLUMEHELPER_H
