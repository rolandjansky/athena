/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
   @class LArShapeInfo
   @brief Liquid Argon class for standalone storage of cell shape information
*/

#ifndef LArSamples_ShapeInfo_H
#define LArSamples_ShapeInfo_H

#include <vector>

class TH1D;
class TGraph;

namespace LArSamples {

  class SimpleShape;
  
  class ShapeInfo final {
  
   public:
   
    /** @brief Constructor  */
    ShapeInfo(unsigned char nSamples = 5, unsigned char binSize = 3, 
                 unsigned char nPhases = 8, float shift = 0);
  
    ShapeInfo(const ShapeInfo& other);

    ~ShapeInfo();
    
    /** @return size */
    unsigned int   nPoints()    const { return m_values.size() + 1; } // add one more point by extrapolation
    unsigned char  binSize()    const { return m_binSize; } // in ns. 3 ns for now, may move to 1 ns
    unsigned char nSamples()    const { return m_nSamples; }
    unsigned char  nPhases()    const { return m_nPhases; }
    unsigned int   nIntervals() const { return nBuckets/binSize(); }
    float          shift()      const { return m_shift + m_timeShift; }

    //double timeInterval() const { return (1.0*samplingTime()/nIntervals()); }
    //double startTime()    const { return -shift()*timeInterval(); }
    
    /** @return data points */
    double value(unsigned int i) const;
    //double covariance(unsigned int /*i*/, unsigned int /*j*/) const { return 0; }
    unsigned char phase(unsigned int i)  const;
    
    void set(unsigned char iSample, unsigned char iPhase, float value);
        
    bool isValid() const;
    double footprint() const { return sizeof(*this) + m_values.size()*sizeof(short); }
    
    // MIG: TBR
    //SimpleShape* simpleShape(double scale = 1, double shift = 0) const;
    //SimpleShape* sampleShape(double scale = 1, double shift = 0) const;
    //void Draw() const;

    static constexpr unsigned int nBuckets = 24;

    static double timeShift() { return m_timeShift; }
    static void setTimeShift(double shift = 0) { m_timeShift = shift; }
    
   private:

    std::vector<short> m_values;
    unsigned char m_nSamples, m_binSize, m_nPhases;
    float m_shift;
    static double m_timeShift; // specify a global time shift between first sample time and reported ofc time

    ShapeInfo& operator= (const ShapeInfo&);
  };
}

#endif

