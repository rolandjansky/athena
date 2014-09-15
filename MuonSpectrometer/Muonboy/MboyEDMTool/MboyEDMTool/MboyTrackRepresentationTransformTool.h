/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyTrackRepresentationTransformTool_H
#define MboyTrackRepresentationTransformTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "TrkParameters/TrackParameters.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace Muon {
  class IMboyDetectorLimitsTool ;
}

#include "MboyEDMToolInterfaces/IMboyTrackRepresentationTransformTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

  /**
   @class MboyTrackRepresentationTransformTool

  @author samusog@cern.ch
  
  */

namespace Muon {

class MboyTrackRepresentationTransformTool: public AthAlgTool, virtual public IMboyTrackRepresentationTransformTool{
public:
    MboyTrackRepresentationTransformTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyTrackRepresentationTransformTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   virtual const Trk::TrackParameters* AtACOrAtAD2MeasuredPerigee(const Trk::TrackParameters* pTrackParameters) ;
   virtual const Trk::TrackParameters* MeasuredPerigee2AtACOrAtAD(const Trk::TrackParameters* pTrackParameters) ;
   
private:
///////////////////////////////////

  std::string m_ToolName ;

  const Trk::Perigee* MeasuredAtaCylinder2MeasuredPerigee(const Trk::AtaCylinder* pMeasuredAtaCylinder);
  void Jacobian_AtaCylinder_To_Perigee(
                                                 AmgMatrix(5,5)* pTheJac  ,
                                                 double& RCrossing   ,
                                                 double&  Peri_Thed0       ,
                                                 double&  Peri_Thez0     ,
                                                 double&  Peri_ThePhi0   ,
                                                 double&  Peri_TheTheta  ,
                                                 double&  Peri_TheqOnp   ,
                                                 double&  AtAC_TheLocRPhi    ,
                                                 double&  AtAC_TheLocZ     ,
                                                 double&  AtAC_ThePhi        ,
                                                 double&  AtAC_TheTheta      ,
                                                 double&  AtAC_TheqOnp      
                                       );
  const Trk::AtaCylinder* MeasuredPerigee2MeasuredAtaCylinder(const Trk::Perigee* pMeasuredPerigee);
  void Jacobian_Perigee_To_AtaCylinder(
                                                 AmgMatrix(5,5)* pTheJac  ,
                                                 double& RCrossing   ,
                                                 double&  Peri_Thed0       ,
                                                 double&  Peri_Thez0     ,
                                                 double&  Peri_ThePhi0   ,
                                                 double&  Peri_TheTheta  ,
                                                 double&  Peri_TheqOnp   ,
                                                 double&  AtAC_TheLocRPhi    ,
                                                 double&  AtAC_TheLocZ     ,
                                                 double&  AtAC_ThePhi        ,
                                                 double&  AtAC_TheTheta      ,
                                                 double&  AtAC_TheqOnp      
                                       );

  const Trk::Perigee* MeasuredAtaDisc2MeasuredPerigee(const Trk::AtaDisc* pMeasuredAtaDisc);
  void Jacobian_AtaDisc_To_Perigee(
                                                 AmgMatrix(5,5)* pTheJac  ,
                                                 double&  ZCrossing ,
                                                 double&  Peri_Thed0       ,
                                                 double&  Peri_Thez0     ,
                                                 double&  Peri_ThePhi0   ,
                                                 double&  Peri_TheTheta  ,
                                                 double&  Peri_TheqOnp   ,
                                                 double&  AtAD_TheLocR       ,
                                                 double&  AtAD_TheLocPhi     ,
                                                 double&  AtAD_ThePhi        ,
                                                 double&  AtAD_TheTheta      ,
                                                 double&  AtAD_TheqOnp     
                                       );
  const Trk::AtaDisc* MeasuredPerigee2MeasuredAtaDisc(const Trk::Perigee* pMeasuredPerigee);
  void Jacobian_Perigee_To_AtaDisc(
                                                 AmgMatrix(5,5)* pTheJac  ,
                                                 double&  ZCrossing ,
                                                 double&  Peri_Thed0       ,
                                                 double&  Peri_Thez0     ,
                                                 double&  Peri_ThePhi0   ,
                                                 double&  Peri_TheTheta  ,
                                                 double&  Peri_TheqOnp   ,
                                                 double&  AtAD_TheLocR       ,
                                                 double&  AtAD_TheLocPhi     ,
                                                 double&  AtAD_ThePhi        ,
                                                 double&  AtAD_TheTheta      ,
                                                 double&  AtAD_TheqOnp      
                                       );
                                       
  ToolHandle< Muon::IMboyDetectorLimitsTool > p_IMboyDetectorLimitsTool ; //!< Pointer On IMboyDetectorLimitsTool

};

}

#endif

