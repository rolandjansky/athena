/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialLayerHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRGEOMODELCNV_MATERIALLAYERHELPER_H
#define TRKDETDESCRGEOMODELCNV_MATERIALLAYERHELPER_H

#include "TrkDetDescrGeoModelCnv/MaterialElement.h"
#include "TrkDetDescrGeoModelCnv/GeoMaterialConverter.h"

namespace Trk {

  class Layer;
  class TrackingVolume;
  class CylinderLayer;

  /**
    @class MaterialLayerHelper
    
    A helper class that builds binned material array for material layers pre-defined in the tracking geometry
    using an external material input (for example, the material extracted from the GeoModel tree).
    It provides access to the material budget for validation purpose.  
        
    @author sarka.todorova@cern.ch
    */
    
    class MaterialLayerHelper {
      
    public:
      /** Default constructor*/
      MaterialLayerHelper()
	{}
      
      /** Destructor*/
      virtual ~MaterialLayerHelper(){}
      
      /** main method to process the external input */
      void processMaterial(const Trk::TrackingVolume*, std::vector<MaterialElement>& , float beamPipeRadius, bool printInfo=false);
      
      Trk::CylinderLayer* buildCylinderLayer(std::vector< MaterialElement>& material, double, double, double, double) const;
      

      private:
        /** extract material layers from input tracking volume */
        void getMaterialLayers(const Trk::TrackingVolume* inputVol, std::vector< const Trk::Layer* >& matLayers);

        /** assign material objects to layer */
	void findClosestLayer(std::vector<MaterialElement>& matInput, std::vector< const Trk::Layer* >&  layers,
			      std::vector< std::vector < MaterialElement > >& mat4merge,
			      float inputVolRadius = 1148. , float inputVolZ=3500. ,  float beamPipeRadius = 30. );

        /** build and assign material arrays */
	void buildMaterial(std::vector< const Trk::Layer* >&  matLayers, 
			             std::vector< std::vector < MaterialElement > >& mat4merge,
			             float beamPipeRadius = 30.);

        /** material overview */
	void materialBudget(std::vector<MaterialElement>& inputMat) const; 

        double eta(double z, double R) const;
        double etaToZ(double eta, double R) const;
        double etaToR(double eta, double z) const;
	std::pair<double,std::pair<double,double> >  averageThickness(MaterialElement mat,bool cyl) const; 
        // best thickness approximation (input in eta)
	double binThickness( double binLow, double binUp, double thick, double e0, double e1, double e2, double e3) const; 

   };
 

} // end of namespace Trk

#endif

