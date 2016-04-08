/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrkSegmentConverter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKTRKSEGMENTCONVERTER_H
#define TRKTRKSEGMENTCONVERTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/ISegmentConverterTool.h"

class AtlasDetectorID;
class Identifier;

namespace Trk 
{

  class Track;
  class Segment;
  class ITrackFitter;
  class IExtrapolator;

  /** @class TrkSegmentConverter 

      Implementation of the ISegmentConverterTool interface
      
      @author  Christian Schmitt <Christian.Schmitt@cern.ch>
  */  

  class TrkSegmentConverter : virtual public ISegmentConverterTool, public AthAlgTool
    {
    public:
      TrkSegmentConverter(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~TrkSegmentConverter ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();

      Track* convert(const Segment&);
      
    private:
      
      /** Convert the segment using a TrackFitter */
      Track* convertWithFitter(const Segment&);

      /** Convert the segment by hand  */
      Track* convertBruteForce(const Segment&);

      /** member variables for algorithm properties: */
      bool m_useFitter;                         //!< Use a Fitter to do the conversion?
      std::string m_trackFitterName;            //!< Name of the TrackFitter
      std::string m_trackFitterInstance;        //!< Instance of the TrackFitter
      ToolHandle<ITrackFitter> m_trackFitter;   //!< The TrackFitter
      ToolHandle<Trk::IExtrapolator> m_extrapolator  ;  //!< Track extrapolator tool.

    }; 
} // end of namespace

#endif 
