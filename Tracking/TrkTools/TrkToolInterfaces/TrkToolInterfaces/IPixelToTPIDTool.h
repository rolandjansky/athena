/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPixelToTPIDTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef TRK_IPIXELTOTPIDTOOL_H
#define TRK_IPIXELTOTPIDTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>

namespace Trk {
  class Track;

  static const InterfaceID IID_IPixelToTPIDTool("Trk::IPixelToTPIDTool", 1, 0);

  /** @brief abstract interface for identification of particles based on

      @author Thijs Cornelissen <thijs.cornelissen -at- cern.ch>
   */

  class IPixelToTPIDTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

   /** @brief particle identification function returning a probability.
       @param[in] track the track to be identified
       @returns   probability
     */
    virtual float dEdx(const Trk::Track& track) = 0;
    virtual int numberOfUsedHitsdEdx() = 0;
    virtual int numberOfUsedIBLOverflowHits() = 0;
    virtual std::vector<float> getLikelihoods(double dedx, double p, int nGoodPixels) = 0;
    virtual float getMass(double dedx, double p, int nGoodPixels) = 0;
  };

  inline const InterfaceID& Trk::IPixelToTPIDTool::interfaceID()
    { 
      return IID_IPixelToTPIDTool; 
    }

} // end of namespace

#endif 
