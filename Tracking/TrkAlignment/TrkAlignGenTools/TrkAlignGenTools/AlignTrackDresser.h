/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

    StatusCode initialize();
    StatusCode finalize();
    
    /** dresses alignTrack with derivatives and other information */
    bool dressAlignTrack(AlignTrack * alignTrack) const;

    /** sets residuals for TSOS on alignTrack */
    void setResiduals(AlignTrack * alignTrack) const;

    /** sets the output stream for the logfile */
    virtual void setLogStream(std::ostream * os);

  private:

    // private variables
    ToolHandle <IDerivCalcTool>  m_derivCalcTool; //!< tool for calculating derivatives of residuals wrt. alignment parameters

    mutable unsigned int m_numBadCovWMatrix; //!< number of tracks with invalid weight matrix from the deriv.calc.tool

  }; // end class

} // end namespace

#endif // TRKALIGNGENDRESSERS_ALIGN_TRACK_DRESSER_H
