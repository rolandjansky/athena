///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IMuonMeanMDTdADCFiller.h, // Header file for class IMuonMeanMDTdADCFiller
/////////////////////////////////////////////////////////////////// 
#ifndef MUONCOMBINEDTOOLINTERFACES_IMUONMEANMDTDADCFILLER_H
#define MUONCOMBINEDTOOLINTERFACES_IMUONMEANMDTDADCFILLER_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODMuon/Muon.h"

namespace Trk {class Track; }

namespace Rec 
{


  static const InterfaceID IID_IMuonMeanMDTdADCFiller("Rec::IMuonMeanMDTdADCFiller", 1, 0);

  /** @class IMuonMeanMDTdADCFiller
      @brief interface providing mean Number of ADC counts for MDT tubes on the track
      @author Oleg.Bulekov@cern.ch
   */

class IMuonMeanMDTdADCFiller : virtual public IAlgTool {
 public: 
    static const InterfaceID& interfaceID( ) ;

    /** @brief return mean Number of ADC counts for MDT tubes on the track, starting from the full muon object
      */
    virtual double
      meanMDTdADCFiller (const xAOD::Muon& muon) const = 0;

    /** @brief return mean Number of ADC counts for MDT tubes on the track, starting from the track object
      */
    virtual double
      meanMDTdADCFiller(const Trk::Track& track) const = 0;
 
}; 
  inline const InterfaceID& Rec::IMuonMeanMDTdADCFiller::interfaceID()
    { 
      return IID_IMuonMeanMDTdADCFiller; 
    }

} // end of namespace

#endif //> !MUONCOMBINEDTOOLINTERFACES_IMUONMEANMDTDADCFILLER_H
