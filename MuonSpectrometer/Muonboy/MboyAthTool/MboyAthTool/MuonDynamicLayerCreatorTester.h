/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonDynamicLayerCreatorTester_H
#define MuonDynamicLayerCreatorTester_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 

#include "TrkParameters/TrackParameters.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthTool/MboyAthToolMisc.h"

#include "MboyAthTool/MboyCollectMatterTool.h"
#include "TrkDetDescrInterfaces/IDynamicLayerCreator.h"

  /**
   @class MuonDynamicLayerCreatorTester

    This class is performing checks of MboyCollectMatterTool and  MuonDynamicLayerCreator tools 

  @author samusog@cern.ch
  
  */

class MuonDynamicLayerCreatorTester:public AthAlgorithm {
public:
  MuonDynamicLayerCreatorTester(const std::string& name, ISvcLocator* pSvcLocator);

public:
///////////////////////////////////

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
///////////////////////////////////

    Trk::AtaPlane* AtaPlaneOnMuSpectroSurfAnglInDeg(
                                                    double InDegThePhi    , 
                                                    double InDegTheTheta  , 
                                                    double Mom            , 
                                                    double TheCharge  
                                                   );
    Trk::AtaPlane* AtaPlaneOnMuSpectroSurf(
                                           double ThePhi    , 
                                           double TheTheta  , 
                                           double Mom       , 
                                           double TheCharge
                                          );

    Trk::AtaPlane* AtaPlaneOnIPAnglInDeg(
                                         double InDegThePhi    , 
                                         double InDegTheTheta  , 
                                         double Mom            , 
                                         double TheCharge  
                                        );
    Trk::AtaPlane* AtaPlaneOnIP(
                                double ThePhi    , 
                                double TheTheta  , 
                                double Mom       , 
                                double TheCharge
                               );

    Trk::AtaPlane* AtaPlaneRadProtectedTransAtZfixedAnglInDeg(
                                             double InDegThePhi    , 
                                             double InDegThZconstanteTheta  , 
                                             double Mom            , 
                                             double TheCharge  
                                            );
    Trk::AtaPlane* AtaPlaneRadProtectedTransAtZfixed(
                                    double ThePhi    , 
                                    double Zconstant  , 
                                    double Mom       , 
                                    double TheCharge
                                   );

    Trk::AtaPlane* AtaPlaneTransAtZfixedAnglInDeg(
                                             double InDegThePhi    , 
                                             double InDegThZconstanteTheta  , 
                                             double Mom            , 
                                             double TheCharge  
                                            );
    Trk::AtaPlane* AtaPlaneTransAtZfixed(
                                    double ThePhi    , 
                                    double Zconstant  , 
                                    double Mom       , 
                                    double TheCharge
                                   );

    Trk::AtaPlane* AtaPlaneBase(
                                double ThePhi    , 
                                double TheTheta  , 
                                double Mom       , 
                                double TheCharge ,
                                double Posx      ,
                                double Posy      ,
                                double Posz                                         
                               );

    Trk::AtaPlane* AtaPlaneUltraBase(
                                     double TheCharge ,
                                     double Posx      ,
                                     double Posy      ,
                                     double Posz      ,                                  
                                     double MomX      ,
                                     double MomY      ,
                                     double MomZ                                         
                                    );
    void GiveMomentumForFromStarting2Pointed(
                                      double  X_Starting ,
                                      double  Y_Starting ,
                                      double  Z_Starting ,
                                      double  X_Pointed  ,
                                      double  Y_Pointed  ,
                                      double  Z_Pointed  ,
                                      double  Mom        , 
                                      double& MomX       ,
                                      double& MomY       ,
                                      double& MomZ       
                                    );


   void TestMboyCollectMatterTool()    ; //!< Test MboyCollectMatterTool
   void DumpTestMboyCollectMatterTool(
                                      std::ofstream&  OutCurrent,
                                      double          InDegTheTheta
                                     );  
   void DumpTest2MboyCollectMatterTool(
                                      std::ofstream&  OutCurrent,
                                      double          Zconstant
                                     );  

   ToolHandle< MboyCollectMatterTool > p_MboyCollectMatterTool ; //!< Pointer On MboyCollectMatterTool

   int m_TestMboyCollectMatterToolDone ; //!< Flag to do Test only once



   void TestMuonDynamicLayerCreator()    ; //!< Test MuonDynamicLayerCreator
   void DumpTestMuonDynamicLayerCreatorA(
                                      std::ofstream&  OutCurrent,
                                      double          InDegTheTheta
                                     );  
   void DumpTestMuonDynamicLayerCreatorB(
                                      std::ofstream&  OutCurrent,
                                      double          InDegTheTheta
                                     );  
   void DumpTest2MuonDynamicLayerCreator(
                                      std::ofstream&  OutCurrent,
                                      double          Zconstant
                                     );  
   void DumpTest3MuonDynamicLayerCreator(
                                      std::ofstream&  OutCurrent,
                                      double          InDegTheTheta
                                     );  

   void DumpTestMuonDynamicLayerCreatorFromStarting2Pointed(
                                      std::ofstream&  OutCurrent,
                                      double X_Starting ,
                                      double Y_Starting ,
                                      double Z_Starting ,
                                      double X_Pointed  ,
                                      double Y_Pointed  ,
                                      double Z_Pointed    
                                     );  

   void DumpTestMuonDynamicLayerCreatorFromStartingPointAndMom(
                                      std::ofstream&  OutCurrent,
                                      double X_Starting  ,
                                      double Y_Starting  ,
                                      double Z_Starting  ,
                                      double PX_Starting ,
                                      double PY_Starting ,
                                      double PZ_Starting   
                                     );  

   ToolHandle< Trk::IDynamicLayerCreator > p_IDynamicLayerCreator ; //!< Pointer On IDynamicLayerCreator

   int m_TestMuonDynamicLayerCreatorDone ; //!< Flag to do Test only once



   double m_MuSpectro_Radius ; // Definition of Muon Spectro Entrance
   double m_MuSpectro_Zcyli  ; // Definition of Muon Spectro Entrance
   double m_RadiusProtection ; // Small radii protection
   
   double m_InDegThePhiMin   ; // Min of Phi range
   double m_InDegThePhiMax   ; // Max of Phi range
   int    m_NberOfPhiPoints  ; // Nber of point in Phi range
   
   double m_MomentumTest  ; // Momentum of tested muon
   double m_ChargeTest    ; // Charge of the tested muon

   double m_X_Starting  ; // Starting Point
   double m_Y_Starting  ; // Starting Point
   double m_Z_Starting  ; // Starting Point

   double m_X_Pointed   ; // Pointed  Point
   double m_Y_Pointed   ; // Pointed  Point
   double m_Z_Pointed   ; // Pointed  Point

   double m_PX_Starting ; // Starting  Point
   double m_PY_Starting ; // Starting  Point
   double m_PZ_Starting ; // Starting  Point


   /** return a secured AtAplane */
   Trk::AtaPlane* GetSecuredAtaPlane (
     const Amg::Vector3D       &      aGlobalPosition ,
     const Amg::Vector3D       &      aGlobalMomentum ,
     double                           TheCharge       ,
     const Trk::PlaneSurface   &      aPlaneSurface
   );

};

#endif
