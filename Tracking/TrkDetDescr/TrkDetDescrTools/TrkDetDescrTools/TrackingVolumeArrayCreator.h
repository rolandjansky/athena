/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingVolumeArrayCreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_TRACKINGVOLUMEARRAYCREATOR_H
#define TRKDETDESCRTOOLS_TRACKINGVOLUMEARRAYCREATOR_H

// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrInterfaces/ITrackingVolumeArrayCreator.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkGeometry/TrackingVolume.h"
// STL
#include <algorithm>

namespace Trk {

    class Layer;
    class PlaneLayer;

    /**@typedef LayerOrderPosition */
    typedef std::pair< SharedObject<const TrackingVolume>, Amg::Vector3D> TrackingVolumeOrderPosition;
    typedef std::pair< SharedObject<const TrackingVolume>, const Amg::Transform3D*> TrackingVolumeNavOrder;


    /** @class TrackingVolumeArrayCreator

      The TrackingVolumeArrayCreator is a simple Tool that helps to construct
      binned arrays of TrackingVolumes for both, confinement in another volume 
      and navigation issues.

      @author Andreas.Salzburger@cern.ch   
     */

    class TrackingVolumeArrayCreator : public AthAlgTool,
                               virtual public ITrackingVolumeArrayCreator {

      public:
        /** Constructor */
        TrackingVolumeArrayCreator(const std::string&,const std::string&,const IInterface*);

        /** Destructor */
        virtual ~TrackingVolumeArrayCreator();

        /** AlgTool and IAlgTool interface methods */
        static const InterfaceID& interfaceID() { return IID_ITrackingVolumeArrayCreator; }

        /** AlgTool initialize method */
        StatusCode initialize();

        /** AlgTool finalize method */
        StatusCode finalize();

        /** TrackingVolumeArrayCreator interface method -
            create a R-binned cylindrical volume array*/
        TrackingVolumeArray* cylinderVolumesArrayInR(const std::vector< const TrackingVolume* >& vols,
                                                     bool navigationtype=false) const;

        /** TrackingVolumeArrayCreator interface method -
            create a R-binned cylindrical volume array*/
        TrackingVolumeArray* cylinderVolumesArrayInZ(const std::vector< const TrackingVolume* >& vols,
                                                     bool navigationtype=false) const;

        /** TrackingVolumeArrayCreator interface method -
            create a R-binned cylindrical volume array*/
        TrackingVolumeArray* cylinderVolumesArrayInPhi(const std::vector< const TrackingVolume* >& vols,
                                                       bool navigationtype=false) const;

        /** TrackingVolumeArrayCreator interface method -
            create a 2dim cylindrical volume array*/
        TrackingVolumeArray* cylinderVolumesArrayInPhiR(const std::vector< const TrackingVolume* >& vols,
                                                        bool navigationtype=false) const;

        /** TrackingVolumeArrayCreator interface method -
            create a 2dim cylindrical volume array*/
        TrackingVolumeArray* cylinderVolumesArrayInPhiZ(const std::vector< const TrackingVolume* >& vols,
                                                        bool navigationtype=false) const;

        /** TrackingVolumeArrayCreator interface method -
            create a Z-binned cuboid volume array*/
        TrackingVolumeArray* cuboidVolumesArrayInZ(const std::vector< const TrackingVolume* >& vols,
                                                   bool navigationtype=false) const;

        /** TrackingVolumeArrayCreator interface method -
            create a cuboid volume array - linked to detached tracking volumes */
        TrackingVolumeArray* cuboidVolumesArrayNav(const std::vector< const TrackingVolume* >& vols,
                                                   Trk::BinUtility* binUtil,
                                                   bool navigationtype=false) const;

        /** TrackingVolumeArrayCreator interface method -
            create a trapezoid volume array - linked to detached tracking volumes */
        TrackingVolumeArray* trapezoidVolumesArrayNav(const std::vector< const TrackingVolume* >& vols,
                                                      Trk::BinUtility* binUtil,
                                                      bool navigationtype=false) const;

        /** TrackingVolumeArrayCreator interface method -
            create a doubleTrapezoid volume array - linked to detached tracking volumes */
        TrackingVolumeArray* doubleTrapezoidVolumesArrayNav(const std::vector< const TrackingVolume* >& vols,
                                                            Trk::BinUtility* binUtil,
                                                            bool navigationtype=false) const;

    };

}

#endif

