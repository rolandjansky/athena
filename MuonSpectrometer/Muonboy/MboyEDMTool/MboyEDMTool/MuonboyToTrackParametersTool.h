/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyToTrackParametersTool_H
#define MuonboyToTrackParametersTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {
  class  Surface;
}

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"

#include "MboyEDMToolInterfaces/IMuonboyToTrackParametersTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

class IMboyEDMHelper;

class MbPOT;

namespace Trk {
  class  IMuonboyToSurfaceTool;
}

namespace Trk {

  /**
   @class MuonboyToTrackParametersTool

    This tool returns a TrackParameters for a MbPOT object
   
  @author samusog@cern.ch
  
  */

class MuonboyToTrackParametersTool: public AthAlgTool, virtual public IMuonboyToTrackParametersTool{
public:
    MuonboyToTrackParametersTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MuonboyToTrackParametersTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /** compute the TrackParameters associated to a MbPOT, correct for small distance to the PrepRawData surface */
   virtual void DoTheTrackParametersCorrected(
                             int DoCorrectPosition ,
                             int DoDbg ,
                             const MbPOT* pMbPOT                 ,
                             const Trk::Surface*& pSurface,
                             const Trk::TrackParameters*& pTrackParameters 
                            ) ;
                            
   /** compute the TrackParameters associated to a MbPOT, correct for small distance to the PrepRawData surface */
   void DoTheTrackParametersCorrectedSurface(
                             const Trk::Surface* pSurface,
                             double XdcaL  ,
                             double YdcaL  ,
                             double ZdcaL  ,
                             double XdirL  ,
                             double YdirL  ,
                             double ZdirL  ,
                             double TheMom ,  
                             double TheCharge  ,
                             const TrackParameters*& pTrackParameters 
                            ) ;

private:
///////////////////////////////////

   /** compute the TrackParameters associated to a MbPOT, correct for small distance to the PrepRawData surface for line/MDT */
   void DoTheTrackParametersCorrectedLine(
                             const Trk::Surface* pSurface,
                             double XdcaL  ,
                             double YdcaL  ,
                             double ZdcaL  ,
                             double XdirL  ,
                             double YdirL  ,
                             double ZdirL  ,
                             double TheMom ,  
                             double TheCharge  ,
                             const TrackParameters*& pTrackParameters 
                            ) ;

   /** Return parameters associated to MbPOT on the track */
   void GetParametersofTheTrack(
                             const   MbPOT* pMbPOT                 ,
                             double& XdcaL  ,
                             double& YdcaL  ,
                             double& ZdcaL  ,
                             double& XdirL  ,
                             double& YdirL  ,
                             double& ZdirL  ,
                             double& TheMom ,  
                             double& TheCharge 
                            ) ;

   /** correct for small distance to the PrepRawData surface */
   void CorrectParametersofTheTrackSurface(
                             const Trk::Surface* pSurface,
                             double& XdcaL  ,
                             double& YdcaL  ,
                             double& ZdcaL  ,
                             double  XdirL  ,
                             double  YdirL  ,
                             double  ZdirL  ,
                             double& TheMom ,  
                             double& TheCharge 
                            ) ;

   /** correct for small distance to the PrepRawData surface for line/MDT */
   void CorrectParametersofTheTrackLine(
                             const Trk::Surface* pSurface,
                             double& XdcaL  ,
                             double& YdcaL  ,
                             double& ZdcaL  ,
                             double  XdirL  ,
                             double  YdirL  ,
                             double  ZdirL  ,
                             double& TheMom ,  
                             double& TheCharge 
                            ) ;

   /** compute the TrackParameters */
   void DoTheTrackParametersBase(
                             const Trk::Surface* pSurface,
                             double XdcaL  ,
                             double YdcaL  ,
                             double ZdcaL  ,
                             double XTRAdir  ,
                             double YTRAdir  ,
                             double ZTRAdir  ,
                             double TheMom ,  
                             double TheCharge ,
                             const  TrackParameters*& pTrackParameters 
                            ) ;

   /** Reversed momenta */

   void FlipAChargedMomentum(Amg::Vector3D& aGlobalMomentum, double& TheCharge);

//Data

   std::string m_ToolName ;

   int m_NullSurface ; //!< Control surface treatement

   int m_DoFlip ; //!< Control momenta reversal              

   int m_DoCorrectPosition ; //!< Control correction for small distance to PreRawData
   
   int m_DoDbg ; //!< Control special debug
   
   ToolHandle< IMboyEDMHelper >        p_OwnEDMHelper          ; //!< Pointer On IMboyEDMHelper
   ToolHandle< Trk::IMuonboyToSurfaceTool > p_IMuonboyToSurfaceTool ; //!< Pointer On IMuonboyToSurfaceTool

};

}

#endif

