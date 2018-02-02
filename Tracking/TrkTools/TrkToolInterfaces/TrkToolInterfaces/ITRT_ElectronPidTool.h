/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITRT_ElectronPidTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef TRK_ITRT_ELECTRONPIDTOOL_H
#define TRK_ITRT_ELECTRONPIDTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include <vector>

namespace Trk {
  class Track;

  static const InterfaceID IID_ITRT_ElectronPidTool("Trk::ITRT_ElectronPidTool", 1, 0);

  /** @brief abstract interface for identification of electrons based on
      information from the inner detector track (mainly TRT)

      This interface is part of the tracking realm tracking to
      allow it being called from e.g. the track summary tool.
      Its implementation is found in the inner detector.

      @author Troels Petersen <troels.petersen -at- cern.ch>
   */

  class ITRT_ElectronPidTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

   /** @brief particle identification function returning a vector of
       probabilities.
       @param[in] track the track to be identified
       @returns   vector of probabilities
     */
    virtual std::vector<float> electronProbability(const Trk::Track& track) const = 0;
    
    /** @brief return high threshold probability 
        @returns double of probability
    */
    virtual double probHT( const double p, const Trk::ParticleHypothesis hypothesis, const int HitPart, const int Layer, const int Strawlayer) const = 0;
    virtual double probHTRun2( float pTrk, Trk::ParticleHypothesis hypothesis, int TrtPart, int GasType, int StrawLayer, float ZR, float rTrkWire, float Occupancy ) const = 0;



  };

  inline const InterfaceID& Trk::ITRT_ElectronPidTool::interfaceID()
    { 
      return IID_ITRT_ElectronPidTool; 
    }

} // end of namespace

#endif 
