/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::Residual
   @brief storage of a pulse shape residual set
 */

#ifndef LArSamples_Residual_H
#define LArSamples_Residual_H

#include "LArSamplesMon/IndexRange.h"
#include "LArCafJobs/Definitions.h"
#include "LArSamplesMon/Averager.h"
#include "TVectorD.h"
#include <vector>
#include <map>

class TH1D;

namespace LArSamples
{  
  class ShapeErrorData;
  
  
  class Residual : public IndexRange
  {
    public:
    
      Residual(const TVectorD& deltas = TVectorD(), 
	       int run = 0, int event = 0, 
	       double adcMax = -1, double time = 0);

      virtual ~Residual();
        
      const TVectorD& deltas() const { return m_deltas; }
      int run() const { return m_run; }
      int event() const { return m_event; }
      double adcMax() const { return m_adcMax; }
      double time() const { return m_time; }
      double scaledDelta(short i) const;
      
      TVectorD scaledDeltas() const;
      TVectorD scaledDeltasAndTime() const;
      
      int lwb() const { return m_deltas.GetLwb(); }
      int upb() const { return m_deltas.GetUpb(); }

      static short comparisonSample;

      static bool test();
    
    private:
      
      TVectorD m_deltas;
      int m_run, m_event;
      double m_adcMax, m_time;
  };
  
  class ResidualCompare
  {
    public:
      ResidualCompare(int sampling) : m_sampling(sampling) { }
      bool operator() (const Residual& r1, const Residual& r2) const;
    private:
      int m_sampling;
  };
  
  class ResidualCalculator;
  
  class Residuals : public IndexRange
  {
    public:
      Residuals() { }
      Residuals(const std::vector<Residual>& r) 
          : m_residuals(r) { }
          
      virtual ~Residuals() { }
      
      unsigned int size() const { return m_residuals.size(); }
      const Residual* residual(unsigned int i) const { return (i < size() ? &m_residuals[i] : 0); } 

      int lwb() const { return (size() > 0 ? residual(0)->lwb() : 0); }
      int upb() const { return (size() > 0 ? residual(0)->upb() : 0); }

      bool add(const Residual& residual) { m_residuals.push_back(residual); return true; }

      bool medianVars(TVectorD& medians, TVectorD& widths) const;
      Residuals* truncate(double nWidthsRes, double nWidthsTime = -1, unsigned int nMax = 0) const;

      ResidualCalculator* calculator(bool weigh = false) const;
      
      TH1D* histogram(short sample, const TString& name, int nBins, double xMin, double xMax) const;
      
    private:
       std::vector<Residual> m_residuals;
  };

  
  class ResidualCalculator : public IndexRange
  {
    public:
      
      ResidualCalculator() : m_weigh(false) { }
      
      ResidualCalculator(unsigned int l, unsigned int u, bool weigh = false)
        : m_regresser(l, u + 1), m_weigh(weigh) { }
      
      ResidualCalculator(const Averager& regresser, bool weigh = false)
        : m_regresser(regresser), m_weigh(weigh) { }
      
      virtual ~ResidualCalculator() { }
      
      int lwb() const { return regresser()->lwb(); }
      int upb() const { return regresser()->upb() - 1; }

      unsigned int size() const { return m_events.size(); }

      const Averager* regresser() const { return &m_regresser; }
      bool weigh() const { return m_weigh; }
      double weight(const Residual& residual) const;

      bool fill(const Residual& residual);
      bool remove(const Residual& residual);

      bool append(const ResidualCalculator& other);

      ShapeErrorData* shapeErrorData() const;
      TString description() const;
      
      int run(unsigned int i) const { return m_runs[i]; }
      int event(unsigned int i) const { return m_events[i]; }
      
      int find(int run, int event) const;
      bool add(int run, int event);
      
    private:
      
      bool fill_with_weight(const Residual& residual, double w);
      
      Averager m_regresser;
      //std::map< std::pair<int, int >, bool > m_events;
      std::vector<int> m_runs, m_events;
      bool m_weigh;
  };
      
      
}

#endif
