/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETCONVERSIONFINDERTOOLS_VERTEXPOINTESTIMATOR_H
#define INDETCONVERSIONFINDERTOOLS_VERTEXPOINTESTIMATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkParameters/TrackParameters.h"

namespace InDet {
  
  /**
     @class VertexPointEstimator
     Some helper tools like:
     * hits counter
     @author Tatjana Lenz , Thomas Koffas
  */
  
  class VertexPointEstimator : public AthAlgTool {
    
  public:
    VertexPointEstimator (const std::string& type,const std::string& name, const IInterface* parent);
    
    virtual ~VertexPointEstimator();
    
    static const InterfaceID& interfaceID();
    
    virtual StatusCode initialize();
    
    virtual StatusCode finalize();
    
    /** Get intersection point of two track helices.Based on pure geometric arguments.Return error flag if problems. */
    Amg::Vector3D getCirclesIntersectionPoint(const Trk::Perigee*, const Trk::Perigee*, int, int&);
    /** Return a map with the values calculated for the last pair
      * to decorate the vertex once it is created **/
    std::map<std::string, float> getLastValues();
		
  private:
    
    double areaVar(double, double, double, double, double, double, double&);
    double areaVar(double, double, double, double, double, double, double&, double&, double&);
    bool   circleIntersection(double, double, double, 
			      double, double, double, 
			      double&, double&, 
			      double&, double&);
    bool   secondDegree(double, double, double, double&, double&);
    double areaTriangle(double, double, double, double, double, double);
    
    static double s_bmagnt;
    std::vector<double> m_maxDR;         /**maximum XY separation, non-intersecting circles*/
    std::vector<double> m_maxDZ;         /**maximum allowed track Z separation at the vertex */
    std::vector<double> m_maxR;          /** maximum allowed vertex radius */
    std::vector<double> m_minArcLength;  /**minimum permitted arc length, track to vertex, depends on the posion of the first measurement*/
    std::vector<double> m_maxArcLength;  /**maximum permitted arc length, track to vertex, depends on the posion of the first measurement*/
    std::vector<double> m_minDr;         /**minimum difference between helix centers*/
    std::vector<double> m_maxDr;         /**maximum difference between helix centers*/
    std::vector<double> m_maxHl;         /**maximum ratio H/l */
    std::vector<double> m_maxPhi;        /**maximum DPhi at the estimated vertex position */
    double m_maxChi2;                    /** max chi2 of the estimated vertex position*/
    float m_deltaPhi;                    /** deltaPhi between tracks **/
    float m_deltaR;                      /** Distance between helix centers minus the radii of each of them (D - R1 - R2) **/
  };
  
}
#endif // INDETCONVERSIONFINDERTOOLS_VERTEXPOINTESTIMATOR_H 

 
