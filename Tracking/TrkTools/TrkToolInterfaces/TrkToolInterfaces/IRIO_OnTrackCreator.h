/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IRIO_OnTrackCreator.h
//   Header file for base class IRIO_OnTrackCreator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig@cern.ch, Andreas.Salzburger@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRKTOOLS_IROTCREATOR_H
#define TRKTOOLS_IROTCREATOR_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h" // typedef

namespace Trk {
  class PrepRawData;
  class RIO_OnTrack;

  static const InterfaceID IID_IRIO_OnTrackCreator("IRIO_OnTrackCreator",1,0);

    /** @class Trk::IRIO_OnTrackCreator
        @brief Interface class for transforming Trk::PrepRawData to
               Trk::RIO_OnTrack using a local track hypothesis.
     
        @author Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
     */

  class IRIO_OnTrackCreator : virtual public IAlgTool {

  public:
    /* if the AlgTool direct inheritance had been used, a cxx file
       would be necessary, otherwise C++ will take default
       constructor -> expect unresolved symbol
    */

    virtual ~IRIO_OnTrackCreator() {}
    static const InterfaceID& interfaceID()
    { return IID_IRIO_OnTrackCreator; }    //!< The AlgTool InterfaceID

    /** abstract base method for the creation of RIO_OnTrack
        it takes a RIO (PrepRawData) and the given Track Parameter
     */
    virtual const RIO_OnTrack*
      correct (const PrepRawData& hit, const TrackParameters& trk)
    const =0;   // pure virtual
  };
} // end of namespace

#endif // TRKTOOLS_IROTCREATOR_H

