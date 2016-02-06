/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SeedToTrackConversionTool.h, (c) ATLAS Detector software
// Class for converting Seeds to Track Collection 
// Version 0.0 01/02/2016 Weiming Yao(LBNL)  
///////////////////////////////////////////////////////////////////

#ifndef INDETSEEDTOTRACKCONVERSIONTOOL_H
#define INDETSEEDTOTRACKCONVERSIONTOOL_H

#include <list>
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ISeedToTrackConversionTool.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

class MsgStream;

namespace InDet 
{

  /** @class SeedToTrackConversionTool 

      InDet::SeedToTrackConversionTool is tool that converts the seeds
      into track collection. 
      
      @author  Weiming Yao <Weiming.Yao>
  */  

  class SeedToTrackConversionTool : virtual public ISeedToTrackConversionTool, public AthAlgTool
    {
    public:
      SeedToTrackConversionTool(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~SeedToTrackConversionTool ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();

      // Main methods for seeds conversion
      virtual void executeSiSPSeedSegments   (const Trk::TrackParameters*,const int&, const std::list<const Trk::SpacePoint*>&); 
      //!<seed trackparameters, number of tracks found:m_track.size(), list of spacepoints
      virtual void newEvent();
      virtual void newEvent(const Trk::TrackInfo&,const std::string&);
      virtual void endEvent();

      //////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
      
      virtual MsgStream&    dump(MsgStream&    out) const;
      virtual std::ostream& dump(std::ostream& out) const;
      // enter declaration of your interface-defining member functions here
      
    protected:
      ToolHandle<Trk::IExtrapolator>        m_extrapolator; //!< extrapolator
      ToolHandle<Trk::IRIO_OnTrackCreator > m_rotcreator; //!< Creator ROT
      mutable TrackCollection*              m_seedsegmentsCollection; //!< output collection for seed
      std::string                           m_seedsegmentsOutput; //!< SiSpSeedSegments Output Collection
      Trk::TrackInfo                        m_trackinfo     ; //!< TrackInfo for seeds 
      std::string                           m_patternName   ; //!< Name of the pattern recognition
      int                                   m_nprint        ; //!< Kind output information
      int                                   m_totseed       ; //!< number of total seeds in the pass
      int                                   m_survived      ; //!< number of survived seeds 
      
      /** member variables for algorithm properties: */
      // int/double/bool  m_propertyName;
      MsgStream&    dumpconditions(MsgStream&    out) const;
      MsgStream&    dumpevent     (MsgStream&    out) const;
      
    }; 
  MsgStream&    operator << (MsgStream&   ,const SeedToTrackConversionTool&);
  std::ostream& operator << (std::ostream&,const SeedToTrackConversionTool&); 
} // end of namespace

#endif 
