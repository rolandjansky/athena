/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXADAPTIVEFITTERTOOLS_CHI2TRACKCOMPATIBILITYESTIMATOR_H
#define TRKVERTEXADAPTIVEFITTERTOOLS_CHI2TRACKCOMPATIBILITYESTIMATOR_H

#include "TrkVertexFitterInterfaces/IVertexTrackCompatibilityEstimator.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "xAODTracking/Vertex.h"

/**
 *  @class Trk::Chi2TrackCompatibilityEstimator
 * 
 *  This object reweights the tracks according to a chi2 value constructed according to 
 *  the distance to the vertex (the error of the vertex is not considered in 
 *  constructing the chi squared, because this is an apriori probability which enters the fit)
 *  @author N. Giacinto Piacquadio (for the Freiburg Group)
 *
 *  -----------------------------------------------------------------------------------
 *  Changes:
 *
 *  David Shope <david.richard.shope@cern.ch> (2016-03-18)
 *  EDM Migration to xAOD - move Trk::Vertex to Amg::Vector3D
 */


namespace Trk
{
  class PerigeeLinearizedTrack;

  class Chi2TrackCompatibilityEstimator : public AthAlgTool,virtual public IVertexTrackCompatibilityEstimator
  {
   public:
    StatusCode initialize();
    StatusCode finalize();
    
   /**
    * default constructor due to Athena interface
    */
    Chi2TrackCompatibilityEstimator(const std::string& t, const std::string& n, const IInterface*  p);
    
   /**
    * destructor
    */
    ~Chi2TrackCompatibilityEstimator();
   
   /**
    * Estimate method changing the state of VxTrackAtVertex
    */
     void estimate(VxTrackAtVertex &,const Amg::Vector3D & vertex);
 
    /**
    * Compatibility method returning the compatibility value
    */  
     float compatibility(VxTrackAtVertex & vtxTrack,const Amg::Vector3D & vertex);
    
  private:  
  
    /**
    * Compatibility method returning the compatibility value
    */  
     template<class T> float _compatibility(T & plane,const Amg::Vector3D & vertex);

  };
}
#endif
