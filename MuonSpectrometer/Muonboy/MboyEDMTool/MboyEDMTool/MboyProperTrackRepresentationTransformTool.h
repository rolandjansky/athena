/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyProperTrackRepresentationTransformTool_H
#define MboyProperTrackRepresentationTransformTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

class MB_Track_MboyCyl ;
class MB_Track_Perigee ;
class MB_Track_SpecEntr ;

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "MboyEDMToolInterfaces/IMboyProperTrackRepresentationTransformTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

  /**
   @class MboyProperTrackRepresentationTransformTool

  @author samusog@cern.ch
  
  */

namespace Muon {

class MboyProperTrackRepresentationTransformTool: public AthAlgTool, virtual public IMboyProperTrackRepresentationTransformTool{
public:
    MboyProperTrackRepresentationTransformTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyProperTrackRepresentationTransformTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /** return the appropriate track measurements */
   virtual const Trk::TrackParameters* TrkParMboyToAtaCylinderOrAtaDisc(const MB_Track_MboyCyl* pMB_Track_MboyCyl) ; 

   virtual const Trk::TrackParameters* TrkParPerigee(const MB_Track_Perigee* pMB_Track_Perigee);

   virtual MB_Track_SpecEntr* AtaCylinderOrAtaDiscToTrkParMboy(const Trk::TrackParameters* pTrackParameters) ; 

private:
///////////////////////////////////

  std::string m_ToolName ;

   const Trk::TrackParameters* TrkParMboyToAtaCylinder(const MB_Track_MboyCyl* pMB_Track_MboyCyl);
   const Trk::TrackParameters* TrkParMboyToAtaDisc    (const MB_Track_MboyCyl* pMB_Track_MboyCyl);

   MB_Track_SpecEntr* AtaCylinderToTrkParMboy(const Trk::TrackParameters* pTrackParameters);
   MB_Track_SpecEntr* AtaDiscToTrkParMboy    (const Trk::TrackParameters* pTrackParameters);

   /** determine if the track is on the Spectro entrance cylinder */
   int IsOnTheCylinder(const MB_Track_MboyCyl* pMB_Track_MboyCyl) ;

   /** Reversed momenta */
   void FlipMeasuredPerigee(
                        double&  Thed0    ,
                        double&  Thez0    ,
                        double&  ThePhi0  ,
                        double&  TheTheta ,
                        double&  TheqOnp  ,
                        AmgSymMatrix(5)& TheHepSymMatrix
                       );
   void FlipMeasuredAtaDisc(
                        double&  TheLocR   ,
                        double&  TheLocPhi ,
                        double&  ThePhi    ,
                        double&  TheTheta  ,
                        double&  TheqOnp   ,
                        AmgSymMatrix(5)& TheHepSymMatrix
                       );
   void FlipMeasuredAtaCylinder(
                        double&  TheLocRPhi ,
                        double&  TheLocZ    ,
                        double&  ThePhi     ,
                        double&  TheTheta   ,
                        double&  TheqOnp    ,
                        AmgSymMatrix(5)& TheHepSymMatrix
                       );
      
   void Jacobian_MB_Track_MboyCyl_To_AtaCylinder(
                                                 AmgMatrix(5,5)* pTheJac  ,
                                                 double& RCrossing   ,
                                                 double& TheLocRPhi  ,
                                                 double& TheLocZ     ,
                                                 double& ThePhi      ,
                                                 double& TheTheta    ,
                                                 double& TheqOnp     ,
                                                 double& ThetaX      ,
                                                 double& PhiX        ,
                                                 double& ThetaV      ,
                                                 double& PhiV        ,
                                                 double& qOnp   
                                                );
   void Jacobian_AtaCylinder_To_MB_Track_MboyCyl(
                                                 AmgMatrix(5,5)* pTheJac  ,
                                                 double& RCrossing   ,
                                                 double& TheLocRPhi  ,
                                                 double& TheLocZ     ,
                                                 double& ThePhi      ,
                                                 double& TheTheta    ,
                                                 double& TheqOnp     ,
                                                 double& ThetaX      ,
                                                 double& PhiX        ,
                                                 double& ThetaV      ,
                                                 double& PhiV        ,
                                                 double& qOnp   
                                                );
   void Jacobian_MB_Track_MboyCyl_To_AtaDisc(
                                                 AmgMatrix(5,5)* pTheJac  ,
                                                 double& ZCrossing   ,
                                                 double& TheLocR     ,
                                                 double& TheLocPhi   ,
                                                 double& ThePhi      ,
                                                 double& TheTheta    ,
                                                 double& TheqOnp     ,
                                                 double& ThetaX      ,
                                                 double& PhiX        ,
                                                 double& ThetaV      ,
                                                 double& PhiV        ,
                                                 double& qOnp   
                                                );
   void Jacobian_AtaDisc_To_MB_Track_MboyCyl(
                                                 AmgMatrix(5,5)* pTheJac  ,
                                                 double& ZCrossing   ,
                                                 double& TheLocR     ,
                                                 double& TheLocPhi   ,
                                                 double& ThePhi      ,
                                                 double& TheTheta    ,
                                                 double& TheqOnp     ,
                                                 double& ThetaX      ,
                                                 double& PhiX        ,
                                                 double& ThetaV      ,
                                                 double& PhiV        ,
                                                 double& qOnp   
                                                );

};

}

#endif

