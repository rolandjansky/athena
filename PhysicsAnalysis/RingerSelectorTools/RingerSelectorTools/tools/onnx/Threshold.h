/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RingerSelectorTools_tools_onnx_Threshold_h
#define RingerSelectorTools_tools_onnx_Threshold_h

#include <algorithm>

namespace Ringer{

  namespace onnx{
    
    class Threshold{

      public:

        /*! Constructor */
        Threshold( float etmin , float etmax, float etamin, float etamax, float slope, float offset, float maxavgmu ) ;
        /*! Destructor */
        ~Threshold() = default; 

        /*! Get the lower et edge */
        float etMin() const { return m_etmin; };
        /*! Get the lower eta edge */
        float etaMin() const { return m_etamin; };
        /*! Get the high et edge */
        float etMax() const { return m_etmax; };
        /*! Get the high eta edge */
        float etaMax() const { return m_etamax; };
        /* Is pass with pileup linear correction */
        bool accept( float discriminant, float avgmu ) const;

      private:

        float m_etmin, m_etmax, m_etamin, m_etamax;

        float m_slope, m_offset, m_maxavgmu;

    };

    //==============================================================================
    inline bool Threshold::accept( float value, float avgmu) const
    {
	    avgmu = std::min(avgmu, m_maxavgmu);
      return value > (m_slope*avgmu + m_offset);
    }


  } // namespace onnx
} //namespace Ringer


#endif
