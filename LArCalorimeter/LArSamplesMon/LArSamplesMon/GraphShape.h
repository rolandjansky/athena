/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArGraphShape
   @brief Liquid Argon class for standalone storage of cell shape information
*/

#ifndef LArSamples_GraphShape_H
#define LArSamples_GraphShape_H

#include "LArCafJobs/AbsShape.h"
#include <vector>

class TH1D;
class TGraph;

namespace LArSamples {

  struct GraphPoint
  {
    GraphPoint(double t, double v, double e) : time(t), value(v), error(e) { }
    bool operator<(const GraphPoint& other) const { return time < other.time; }
    GraphPoint(const GraphPoint& other) = default;
    GraphPoint(GraphPoint&& other) = default;
    GraphPoint& operator=(const GraphPoint& other) = default;
    GraphPoint& operator=(GraphPoint&& other) = default;
    double time, value, error;
  };    
  
  class GraphShape : public AbsShape {
  
   public:
   
    /** @brief Constructor  */
    GraphShape(const std::vector<double>& times, const std::vector<double>& values, const std::vector<double>& errors);
    
    GraphShape(const GraphShape& other) :
        AbsShape(),
        m_points(other.m_points) { }
        
    GraphShape(const AbsShape& other, double scale = 1, double shift = 0);
      
    virtual ~GraphShape() { }
    
    bool add(const AbsShape& other);
    
    /** @return size */
    unsigned int nPoints()  const { return m_points.size(); }  
    double value(unsigned int i) const { return m_points[i].value; }
    double time(unsigned int i) const { return m_points[i].time; }
    double covariance(unsigned int i, unsigned int j) const { return (i == j ? m_points[i].error*m_points[i].error : 0); }
    
  private:
      
    std::vector<GraphPoint> m_points;
  };
}

#endif

