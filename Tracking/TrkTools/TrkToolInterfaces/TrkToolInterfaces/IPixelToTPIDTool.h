/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPixelToTPIDTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef TRK_IPIXELTOTPIDTOOL_H
#define TRK_IPIXELTOTPIDTOOL_H

#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
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
    virtual float dEdx(const EventContext& ctx,
                       const Trk::Track& track,
                       int& nUsedHits,
                       int& nUsedIBLOverflowHits) const = 0;

    float dEdx(const Trk::Track& track,
               int& nUsedHits,
               int& nUsedIBLOverflowHits) const;

    virtual std::vector<float> getLikelihoods(const EventContext& ctx,
                                              double dedx,
                                              double p,
                                              int nGoodPixels) const = 0;

    std::vector<float> getLikelihoods(double dedx,
                                      double p,
                                      int nGoodPixels) const;

    virtual float getMass(const EventContext& ctx,
                          double dedx,
                          double p,
                          int nGoodPixels) const = 0;

    float getMass(double dedx, double p, int nGoodPixels) const;
  };

  inline const InterfaceID& Trk::IPixelToTPIDTool::interfaceID()
    { 
      return IID_IPixelToTPIDTool; 
    }

} // end of namespace
#include "TrkToolInterfaces/IPixelToTPIDTool.icc"

#endif 
