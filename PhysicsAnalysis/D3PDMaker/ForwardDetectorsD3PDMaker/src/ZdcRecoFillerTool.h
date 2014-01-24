// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ZdcRecoFillerTool.h 538888 2013-02-25 04:06:47Z leite $
/**
  * @file ForwardDetectorsD3PDMaker/FillerTool.h
  * @author Yujiao Chen
  * @author Marco Leite <leite@cern.ch>
  * @date August 2010
  * @brief Block filler tool for ZDC information.
  */
#ifndef FORWARDDETECTORSD3PDMAKER_ZDCRECOFILLERTOOL_H
#define FORWARDDETECTORSD3PDMAKER_ZDCRECOFILLERTOOL_H

// STL include(s):
#include <vector>
#include <string>

// EDM include(s):
#include "ZdcEvent/ZdcDigitsCollection.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

// Forward declaration(s):
class ZdcID;

namespace D3PD {

   // Convenience type definition:
   typedef D3PD::BlockFillerTool< ZdcDigitsCollection >
   ZdcDigitsCollectionFillerBase;

   class ZdcRecoFillerTool : public ZdcDigitsCollectionFillerBase {

   public:
      /**
       * @brief Standard Gaudi tool constructor.
       * @param type   The name of the tool type.
       * @param name   The tool name.
       * @param parent The tool's Gaudi parent.
       */
      ZdcRecoFillerTool( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

      virtual StatusCode initialize();
      virtual StatusCode book();
      virtual StatusCode fill( const ZdcDigitsCollection & zdcCol );
      virtual StatusCode finalize();

   private:

      void fillCalibrationConstants();
      void getEnergySum(const std::vector<std::vector<int> > & zdc_Lg,
   		   	   	   	   	const std::vector<std::vector<int> > & zdc_Hg, const int & side);
      float convertToLowGain(const int & v, const int & module, const int & side);

      const ZdcID* m_zdcID;

      // Sum of modules energy per side, per gain
      int            *m_energySumA_Lg;
      int            *m_energySumA_Hg;

      int            *m_energySumC_Lg;
      int            *m_energySumC_Hg;

      // Modules energy after electronic gain selection
      std::vector< float > *m_energyA;
      std::vector< float > *m_energyC;

      // Quality of the auto gain reconstruction
      std::vector< int > *m_qualityA;
      std::vector< int > *m_qualityC;

      // Sum of modules energy (auto gain)
      float *m_energySumA;
      float *m_energySumC;


      //Placeholder for Segmented information (Only EM Side C is segmented)
      /*
      float          *m_posXEmC;
      float          *m_posYEmC;

      float          *m_posXHadA;
      float          *m_posYHadA;

      float          *m_posXHadC;
      float          *m_posYHadC;
      */

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
      std::vector<float> m_calib_p1A;
      std::vector<float> m_calib_p0A;

      std::vector<float> m_calib_p1C;
      std::vector<float> m_calib_p0C;

      //Modules seems to saturate at different positions,
      //So for flexibility is better to keep a saturation limit for each one
      std::vector<int> m_saturationA;
      std::vector<int> m_saturationC;

      int m_pedMin;
      int m_pedMax;

      /**
       * Helpers
       */
      /*
      std::vector<int>   m_id ;
      std::vector<float> m_energy;
      std::vector<float> m_time;
      */

   }; // class ZdcRecoFillerTool

} // namespace D3PD

#endif // FORWARDDETECTORSD3PDMAKER_ZDCRECOFILLERTOOL_H
