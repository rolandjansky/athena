/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyCollectMatterTool_H
#define MboyCollectMatterTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 

#include "TrkParameters/TrackParameters.h"

#include "TrkGeometry/PlaneLayer.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthTool/MboyAthToolMisc.h"
class MbMuSc;

class IMboyAthToolHelper;

class IMuonboyToLayer;

  /**
   @class MboyCollectMatterTool

   This class is providing access to Muonboy collector of material along a trajectory

  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IMboyCollectMatterTool("MboyCollectMatterTool", 1, 0);

class MboyCollectMatterTool:public AthAlgTool{
public:
    MboyCollectMatterTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyCollectMatterTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   StatusCode DoCollectMatter(
                              const Trk::TrackParameters* pTrackParameters,
                              std::vector<std::pair<const Trk::TrackParameters*,const Trk::Layer*> >* VectorOfPlaneLayer
                             );

   void GiveSurfaceParameters(
                              double& MuSpectro_Radius ,
                              double& MuSpectro_Zcyli  ,
                              double& RadiusProtection 
                             );

private:
///////////////////////////////////

   /** Convert MbMuSc in PlaneLayer **/
   std::pair<const Trk::TrackParameters*, const Trk::Layer*> DoConvert(
                             const MbMuSc* pMbMuSc,
                             double sign
                            );

   ToolHandle< IMboyAthToolHelper >   p_OwnEDMHelper          ; //!< Pointer On IMboyAthToolHelper

   int m_AtCosm ; // Flag beam conditions

   double m_MuSpectro_Radius ; // Definition of Muon Spectro Entrance
   double m_MuSpectro_Zcyli  ; // Definition of Muon Spectro Entrance
   double m_RadiusProtection ; // Small radii protection

   ToolHandle< IMuonboyToLayer > p_IMuonboyToLayer ; //!< Pointer On IMuonboyToLayer

};

inline const InterfaceID& MboyCollectMatterTool::interfaceID(){return IID_IMboyCollectMatterTool;}

#endif

