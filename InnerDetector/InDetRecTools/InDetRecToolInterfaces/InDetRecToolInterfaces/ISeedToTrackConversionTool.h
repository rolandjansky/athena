/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISeedToTrackConversionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IINDETSEEDTOTRACKCONVERSIONTOOL_H
#define IINDETSEEDTOTRACKCONVERSIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkTrack/TrackInfo.h"

class MsgStream;

namespace InDet 
{


  static const InterfaceID IID_ISeedToTrackConversionTool("InDet::ISeedToTrackConversionTool", 1, 0);

  class ISeedToTrackConversionTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;
    virtual StatusCode initialize ()=0;
    virtual StatusCode finalize   ()=0;
    virtual void executeSiSPSeedSegments(const Trk::TrackParameters*,const int&, const std::list<const Trk::SpacePoint*>&)=0;
    virtual void newEvent(const Trk::TrackInfo&, const std::string&)=0;
    virtual void endEvent()=0;

    //////////////////////////////////////////////////////////////////
    // Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////
      
    virtual MsgStream&    dump(MsgStream&    out) const=0;
    virtual std::ostream& dump(std::ostream& out) const=0;
    // enter declaration of your interface-defining member functions here

  };

  // Overload of << operator for MsgStream and  std::ostream
  MsgStream&    operator << (MsgStream&   ,const ISeedToTrackConversionTool&);
  std::ostream& operator << (std::ostream&,const ISeedToTrackConversionTool&);
  inline const InterfaceID& ISeedToTrackConversionTool::interfaceID()
    { 
      return IID_ISeedToTrackConversionTool; 
    }

  // Overload of << operator MsgStream
  inline MsgStream& operator    <<
    (MsgStream& sl,const ISeedToTrackConversionTool& se)
  {
    return se.dump(sl); 
  }

  // Overload of << operator std::ostream
  inline std::ostream& operator << 
    (std::ostream& sl,const ISeedToTrackConversionTool& se)
  {
    return se.dump(sl); 
  }
  
} // end of namespace

#endif 
