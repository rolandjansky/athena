/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 #ifndef TrkVertexWeightCalculator_TrueVertexDistanceWeightCalculator_H
 #define TrkVertexWeightCalculator_TrueVertexDistanceWeightCalculator_H
 
 #include "AthenaBaseComps/AthAlgTool.h"
 #include "GaudiKernel/ToolHandle.h"
 #include "TrkVertexFitterInterfaces/IVertexWeightCalculator.h"
 #include "GeoPrimitives/GeoPrimitives.h"
 
//xAOD include
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleFwd.h"

 /**
  * @class Trk::TrueVertexDistanceWeightCalculator 
  *
  * @author Giacinto Piacquadio, August 2010
  *
  * ---------------------------------------------------
  * Changes:
  *
  * David Shope <david.richard.shope@cern.ch> (2016-06-01)
  *
  *   EDM Migration to xAOD - remove method using VxCandidate
  *
  */
 
class McEventCollection;
 
 namespace Trk
 {
  
  class TrueVertexDistanceWeightCalculator : public AthAlgTool, virtual public IVertexWeightCalculator
  {
   public:
     StatusCode initialize();
     StatusCode finalize();
 
     /**
      * constructor
      */
     
     TrueVertexDistanceWeightCalculator(const std::string& t, const std::string& n, const IInterface*  p);
 
     /**
      * destructor
      */
     
     virtual ~TrueVertexDistanceWeightCalculator();
     
    /**
     * WeightCalculator
     */
    
    virtual double estimateSignalCompatibility(const xAOD::Vertex& vertex);
    
         
  private:


    double mEstimateSignalCompatibility(const Amg::Vector3D& vtxPosition);

    std::string m_McEventCollectionName;

    

 
  }; //end of class description
 }//end of namespace definition
 
 #endif
