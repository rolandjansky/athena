/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

    virtual StatusCode initialize() override;
    
    virtual StatusCode finalize() override;

    typedef std::map<std::string, float> Values_t;
    
    /** Get intersection point of two track helices.
     * Based on pure geometric arguments.
     * Return error flag if problems.
     */
    Amg::Vector3D getCirclesIntersectionPoint(const Trk::Perigee* per1,
                                              const Trk::Perigee* per2,
                                              unsigned int flag,
                                              int& errorcode) const;


    /** Get intersection point of two track helices.
     * Based on pure geometric arguments.
     * Return error flag if problems.
     * Also return dictionary of values for decorations. 
     */
    Amg::Vector3D getCirclesIntersectionPoint(const Trk::Perigee* per1,
                                              const Trk::Perigee* per2,
                                              unsigned int flag,
                                              int& errorcode,
                                              Values_t& decors) const;

    /** Return list of keys used for decorations. */
    static std::vector<std::string> decorKeys() ;


  private:
    Amg::Vector3D intersectionImpl (const Trk::Perigee *per1,
                                    const Trk::Perigee *per2,
                                    unsigned int flag,
                                    int& errorcode,
                                    float& deltaPhi,
                                    float& deltaR) const;


    double areaVar(double, double, double, double, double, double, double&) const;
    double areaVar(double, double, double, double, double, double, double&, double&, double&) const;
    bool   circleIntersection(double, double, double, 
			      double, double, double, 
			      double&, double&, 
			      double&, double&) const;
    static bool   secondDegree(double, double, double, double&, double&) ;
    static double areaTriangle(double, double, double, double, double, double) ;
    
    static const double s_bmagnt;
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
  };
  
}
#endif // INDETCONVERSIONFINDERTOOLS_VERTEXPOINTESTIMATOR_H 

 
