/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNGENDRESSERS_ALIGN_TRACK_DRESSER_H
#define TRKALIGNGENDRESSERS_ALIGN_TRACK_DRESSER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkAlignInterfaces/IAlignTrackDresser.h"

/**
   @file AlignTrackDresser.h
   @class AlignTrackDresser
   
   @brief Tool used to dress AlignTrack with residuals and other information needed for alignment.

   @author Robert Harrington <roberth@bu.edu>
   @date 1/5/08

   @author Daniel Kollar <daniel.kollar@cern.ch>
*/

namespace CLHEP {
  class HepSymMatrix;
}

namespace Trk {
  class IDerivCalcTool;
  class AlignTrack;

  class AlignTrackDresser : virtual public IAlignTrackDresser, public AthAlgTool {

  public:
    AlignTrackDresser(const std::string & type, const std::string & name, const IInterface * parent);
    virtual ~AlignTrackDresser();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    
    /** dresses alignTrack with derivatives and other information */
    virtual bool dressAlignTrack(AlignTrack * alignTrack) override;

    /** sets residuals for TSOS on alignTrack */
    void setResiduals(AlignTrack * alignTrack) const;

    /** sets the output stream for the logfile */
    virtual void setLogStream(std::ostream * os) override;

  private:

    // private variables
    ToolHandle <IDerivCalcTool>  m_derivCalcTool; //!< tool for calculating derivatives of residuals wrt. alignment parameters

    unsigned int m_numBadCovWMatrix; //!< number of tracks with invalid weight matrix from the deriv.calc.tool

  }; // end class

} // end namespace

#endif // TRKALIGNGENDRESSERS_ALIGN_TRACK_DRESSER_H
