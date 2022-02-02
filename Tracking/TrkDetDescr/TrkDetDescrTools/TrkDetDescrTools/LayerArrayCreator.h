/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerArrayCreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_LAYERARRAYCREATOR_H
#define TRKDETDESCRTOOLS_LAYERARRAYCREATOR_H

#ifndef TRKDETDESCR_TAKESMALLERBIGGER
#define TRKDETDESCR_TAKESMALLERBIGGER
#define takeSmaller(current,test) current = current < test ? current : test
#define takeBigger(current,test)  current = current > test ? current : test
#define takeSmallerBigger(cSmallest, cBiggest, test) takeSmaller(cSmallest, test); takeBigger(cBiggest, test)
#endif
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrInterfaces/ILayerArrayCreator.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkGeometry/PlaneLayer.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
// STL
#include <algorithm>

namespace Trk {

    class Layer;
    class PlaneLayer;

    /**@typedef LayerOrderPosition */
    typedef std::pair< SharedObject<const Layer>, Amg::Vector3D> LayerOrderPosition;


    /** @class LayerArrayCreator

      The LayerArrayCreator is a simple Tool that helps to construct
      LayerArrays from std::vector of Trk::CylinderLayer, Trk::DiscLayer, Trk::PlaneLayer.

      It fills the gaps automatically with Trk::NavigationLayer to be processed easily in the
      Navigation of the Extrapolation process.

      @author Andreas.Salzburger@cern.ch   
     */

    class LayerArrayCreator : public AthAlgTool, virtual public ILayerArrayCreator {

      public:
        /** Constructor */
        LayerArrayCreator(const std::string&,const std::string&,const IInterface*);
        
        /** Destructor */
        virtual ~LayerArrayCreator();

        /** AlgTool and IAlgTool interface methods */
        static const InterfaceID& interfaceID() { return IID_ILayerArrayCreator; }

        /** LayerArrayCreator interface method - for Barrel-like layers */
        LayerArray* cylinderLayerArray(const std::vector<CylinderLayer* >& layers,
                                       double rmin,
                                       double rmax,
                                       BinningType btype = arbitrary) const;

        /** LayerArrayCreator interface method - for Endcap-like layers */
        LayerArray* discLayerArray(const std::vector< DiscLayer* >& layers,
                                   double zmin,
                                   double zmax,
                                   BinningType btype = arbitrary) const;

        /** LayerArrayCreator interface method - for Planar-like layers */
        LayerArray* planeLayerArray(const std::vector< PlaneLayer* >& layers,
                                    double min,
                                    double max,
                                    BinningType btype = arbitrary,
                                    Trk::BinningValue bv = Trk::binX) const;
      private :
        Trk::Layer*             checkAndReplaceEmptyLayer(Trk::Layer* lay) const;
        int                     m_emptyLayerMode;       //!< 0 - do nothing, 1 - replace with navigation layer, 2 - delete
    };

} // end of namespace

#endif // TRKDETDESCRTOOLS_LAYERARRAYCREATOR_H

