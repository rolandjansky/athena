// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ZdcFillerTool.h 546900 2013-05-06 03:24:54Z ssnyder $
/**
  * @file ForwardDetectorsD3PDMaker/FillerTool.h
  * @author Yujiao Chen
  * @author Marco Leite <leite@cern.ch>
  * @date August 2010
  * @brief Block filler tool for ZDC information.
  */
#ifndef FORWARDDETECTORSD3PDMAKER_ZDCFILLERTOOL_H
#define FORWARDDETECTORSD3PDMAKER_ZDCFILLERTOOL_H

// STL include(s):
#include <vector>
#include <string>

// EDM include(s):
#include "ZdcEvent/ZdcRawChannel.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

// Forward declaration(s):
class ZdcID;

namespace D3PD {

   // Convenience type definition:
   typedef D3PD::BlockFillerTool< ZdcRawChannel >
   ZdcRawChannelFillerBase;

   class ZdcFillerTool : public ZdcRawChannelFillerBase {

   public:
      /**
       * @brief Standard Gaudi tool constructor.
       * @param type   The name of the tool type.
       * @param name   The tool name.
       * @param parent The tool's Gaudi parent.
       */
      ZdcFillerTool( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

      virtual StatusCode initialize();
      virtual StatusCode book();
      virtual StatusCode fill( const ZdcRawChannel& p );

  private:
      void clearData();
      void fillCalibrationConstants(void);

      const ZdcID* m_zdcID;

      /**
       * ZDC Ntuple variables
       * The postfix number corresponds to the longitudinal segmentation module number:
       *     ZDC A                                        ZDC C
       * [3][2][1][0] -------------> IP <------------ [0][1][2][3]
       *
       * 0 : EM Module        (also w/ transverse segmentation only in side A in 64 channels)
       * 1 : Hadronic Module  (also w/ transverse segmentation  in sides A, C in 24 channels)
       * 2 : Hadronic Module
       * 3 : Hadronic Module
       *
       *  Energy   is calibrated in GeV (with modules weights)
       *  Time     is the "preferred method" time determination
       *  Quality  is the "signal quality" (eg. saturation, no peak, etc) and ranges from -1
       *           and right now is either -1 (useless) or 1 (ok)
       *  Method   is the algorithm used in the reconstruction. Since the D3PD can be merged,
       *           it's possible to compare several methods with this entry running reconstruction
       *           jobs with different methods selection
       */

      // These hold all information: different gains, delays and methods
      std::vector< float >* m_energyVec;
      std::vector< float >* m_timeVec;
      int *m_vecSize;

      //These holds the simple calculation from sample
      //peak and time from sample ratio, High gain and low gain
      //undelayed only
      float          *m_energyLg;
      float          *m_timeLg;
      float          *m_energyHg;
      float          *m_timeHg;

     //float          *m_quality;

      float          *m_timeCalib;

      unsigned int  *m_id;

      int            *m_module;
      int            *m_side;
      int            *m_type;
      int            *m_channel;

      /**
       * m_ZdcRecMethod holds a number with the method used to reconstruct the ZDC information
       * 0 :Direct sample measurement
       *      - Energy -> maximum sample
       *      - Time   -> ratio of A1/A2 (second/third sample)
       *
       * 1 : Spline interpolation factory for the samples
       *      - Energy -> peak value of the interpolation function
       *      - Time   -> CFD calculation (0.3 fraction)
       *
       * 2 : Sin(x)/x bandwidth limited interpolation
       *      - Energy -> peak value of the interpolation function
       *      - Time   -> position of the peak
       *
       */
      unsigned int *m_recMethod;

      /**
       * Segmented information. This should calculate the position in the reconstruction,
       * but it's still not implemented. This will need a different container, where
       * the objects will be the position in each segmented module
       * TODO: Not implemented yet
       */

      // float          *m_x;
      // float          *m_y;

      /// Property: if TRUE, apply weigths and constants to get the full energy
      bool         m_calibrateEnergy;
      /// Property: if TRUE, calibrate the time information
      bool         m_calibrateTime;
      /// Property: below this number of ADC counts the signal is considered noise
      unsigned int m_threshold;
      /// Property: the name of the method information to be retrieved
      std::string  m_recoMethod;

      /**
       * Energy calibration vectors for weighting the longitudinal section of the calorimeter
       * This includes both the electronic gain equalization and the weigth for
       * photons/neutrons (different to account for lekeage)
       */
      std::vector<float> m_zdcHadronWeightA;
      std::vector<float> m_zdcHadronWeightC;

      std::vector<float> m_zdcPhotonWeightA;
      std::vector<float> m_zdcPhotonWeightC;

      /**
       * Polynomial coeficients for timing calibration
       * the first dimension corresponds to the section while the second
       * corresponds to the coefficients
       */
      std::vector<std::vector<float> > m_zdcTimeCoeff;

      /**
       * Helpers
       */
      /*
      std::vector<int>   m_id ;
      std::vector<float> m_energy;
      std::vector<float> m_time;
      */

   }; // class ZdcFillerTool

} // namespace D3PD

#endif // FORWARDDETECTORSD3PDMAKER_ZDCFILLERTOOL_H
