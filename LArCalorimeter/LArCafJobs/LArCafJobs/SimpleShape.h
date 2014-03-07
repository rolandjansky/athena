/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
   @class LArSimpleShape
   @brief Liquid Argon class for standalone storage of cell shape information
*/

#ifndef LArSamples_SimpleShape_H
#define LArSamples_SimpleShape_H

#include "LArCafJobs/AbsShape.h"
#include <vector>

class TH1D;
class TGraph;

namespace LArSamples {

  class ShapeInfo;
  
  class SimpleShape : public AbsShape {
  
   public:
   
    /** @brief Constructor  */
    SimpleShape(const std::vector<double>& values, const std::vector<double>& errors, 
                double timeInterval = 25, double startTime = 0);
  
    SimpleShape(unsigned int nPoints, double timeInterval = 25, double startTime = 0);
  
    SimpleShape(const SimpleShape& other) :
      AbsShape(),
      m_values(other.m_values), m_errors(other.m_errors), 
      m_timeInterval(other.m_timeInterval), m_startTime(other.m_startTime) { }
    
    SimpleShape(const ShapeInfo& shapeInfo, double scale = 1, double shift = 0, bool samplingTimeOnly = false);
    SimpleShape(const AbsShape& other, double scale = 1, double shift = 0);

    virtual ~SimpleShape() { }
    
    double timeInterval() const { return m_timeInterval; } 
    double startTime() const { return m_startTime; }

    unsigned int nPoints()  const { return m_values.size(); }  
    double value(unsigned int i) const { return (i < m_values.size() ? m_values[i] : -1E99); }
    double time(unsigned int i) const;
    double covariance(unsigned int i, unsigned int j) const { return (i == j ? m_errors[i]*m_errors[i] : 0); }
    
    void set(unsigned int i, double value, double error = -1) { m_values[i] = value; if (error > 0) m_errors[i] = error; }
    void setError(unsigned int i, double error) { m_errors[i] = error; }

    SimpleShape* diff() const;
        
    bool add(unsigned int k, double value, double error);
    SimpleShape* add(const AbsShape& other, double scale = 1, double shift = 0);
    SimpleShape* createEmpty() const;
    TH1D* histogram(const char* name = "shape", const char* title = "", bool timeInUnitOfSamples = false) const;

    static bool add(SimpleShape*& s1, const AbsShape* s2);
    static bool scaleAndShift(SimpleShape*& s1, double scale, double shift = 0);

  private:
      
    std::vector<double> m_values, m_errors;
    double m_timeInterval, m_startTime;

    SimpleShape& operator= (const SimpleShape&);
  };
}

#endif

