/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

/////////////////////////////////////////////////////////
#include "MboyAthTool/MuonDynamicLayerCreatorTester.h"

#include "TrkSurfaces/CylinderSurface.h"

MuonDynamicLayerCreatorTester::MuonDynamicLayerCreatorTester(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
 p_MboyCollectMatterTool ( "MboyCollectMatterTool" ),
 m_TestMboyCollectMatterToolDone(0),
 p_IDynamicLayerCreator ( "Trk::MuonDynamicLayerCreator/MuonDynamicLayerCreator" ),
 m_TestMuonDynamicLayerCreatorDone(0)
{

  declareProperty("MboyCollectMatterTool" , p_MboyCollectMatterTool    ) ;

  declareProperty("IDynamicLayerCreator" , p_IDynamicLayerCreator    ) ;

  declareProperty("InDegThePhiMin"   , m_InDegThePhiMin  =    0. ) ;
  declareProperty("InDegThePhiMax"   , m_InDegThePhiMax  =   90. ) ;
  declareProperty("NberOfPhiPoints"  , m_NberOfPhiPoints =   91  ) ;

  declareProperty("MomentumTest"  , m_MomentumTest = 1000000. ) ;
  declareProperty("ChargeTest"    , m_ChargeTest   =       1. ) ;


  declareProperty("X_Starting"  , m_X_Starting =  1000. ) ;
  declareProperty("Y_Starting"  , m_Y_Starting =  8000. ) ;
  declareProperty("Z_Starting"  , m_Z_Starting = 14500. ) ;

  declareProperty("X_Pointed"   , m_X_Pointed  =  1000. ) ;
  declareProperty("Y_Pointed"   , m_Y_Pointed  = -5500. ) ;
  declareProperty("Z_Pointed"   , m_Z_Pointed  =  7000. ) ;

  declareProperty("PX_Starting"   , m_PX_Starting  =       0.  ) ;
  declareProperty("PY_Starting"   , m_PY_Starting  = -874157.3 ) ;
  declareProperty("PZ_Starting"   , m_PZ_Starting  = -485642.9 ) ;

  m_MuSpectro_Radius = 0 ;
  m_MuSpectro_Zcyli  = 0 ;
  m_RadiusProtection = 0 ;

}
 
// Initialize
StatusCode MuonDynamicLayerCreatorTester::initialize(){

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  ATH_MSG_INFO( "================================" ) ;
  ATH_MSG_INFO( "=Proprieties are     " ) ;
  ATH_MSG_INFO( "= InDegThePhiMin                   " << m_InDegThePhiMin   ) ;
  ATH_MSG_INFO( "= InDegThePhiMax                   " << m_InDegThePhiMax   ) ;
  ATH_MSG_INFO( "= NberOfPhiPoints                  " << m_NberOfPhiPoints  ) ;
  ATH_MSG_INFO( "= MomentumTest                     " << m_MomentumTest     ) ;
  ATH_MSG_INFO( "= ChargeTest                       " << m_ChargeTest       ) ;
  ATH_MSG_INFO( "= X_Starting                       " << m_X_Starting       ) ;
  ATH_MSG_INFO( "= Y_Starting                       " << m_Y_Starting       ) ;
  ATH_MSG_INFO( "= Z_Starting                       " << m_Z_Starting       ) ;
  ATH_MSG_INFO( "= X_Pointed                        " << m_X_Pointed        ) ;
  ATH_MSG_INFO( "= Y_Pointed                        " << m_Y_Pointed        ) ;
  ATH_MSG_INFO( "= Z_Pointed                        " << m_Z_Pointed        ) ;
  ATH_MSG_INFO( "= PX_Starting                      " << m_PX_Starting      ) ;
  ATH_MSG_INFO( "= PY_Starting                      " << m_PY_Starting      ) ;
  ATH_MSG_INFO( "= PZ_Starting                      " << m_PZ_Starting      ) ;
  ATH_MSG_INFO( "================================" ) ;

//Retrieve p_MboyCollectMatterTool
  if ( p_MboyCollectMatterTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_MboyCollectMatterTool ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_MboyCollectMatterTool ) ;
  p_MboyCollectMatterTool->GiveSurfaceParameters(
                            m_MuSpectro_Radius ,
                            m_MuSpectro_Zcyli  ,
                            m_RadiusProtection 
                           );

  ATH_MSG_INFO( "Initialisation ended     " ) ;

//Retrieve p_IDynamicLayerCreator
  if ( p_IDynamicLayerCreator.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IDynamicLayerCreator ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IDynamicLayerCreator ) ;

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}
 
// Execute
StatusCode MuonDynamicLayerCreatorTester::execute(){

  
  if ( m_TestMboyCollectMatterToolDone == 0 ) {
    m_TestMboyCollectMatterToolDone = 1 ;
    TestMboyCollectMatterTool() ;
  }

  if ( m_TestMuonDynamicLayerCreatorDone == 0 ) {
    m_TestMuonDynamicLayerCreatorDone = 1 ;
    TestMuonDynamicLayerCreator() ;
  }

  return StatusCode::SUCCESS;

}
 
void MuonDynamicLayerCreatorTester::TestMboyCollectMatterTool(){

 ATH_MSG_INFO(  
        " Starting TestMboyCollectMatterTool "
      ) ;

 double InDegTheTheta ; 

//MuonDynamicLayerCreatorTesterMboyCo
 InDegTheTheta  = 75. ; 
 std::ofstream  OutCurrent_75 ;
 OutCurrent_75.open("Out.MuonDynamicLayerCreatorTesterMboyCo75");
 OutCurrent_75 << setiosflags(std::ios::fixed);
 DumpTestMboyCollectMatterTool(OutCurrent_75,InDegTheTheta);

 InDegTheTheta  = 25. ; 
 std::ofstream  OutCurrent_25 ;
 OutCurrent_25.open("Out.MuonDynamicLayerCreatorTesterMboyCo25");
 OutCurrent_25 << setiosflags(std::ios::fixed);
 DumpTestMboyCollectMatterTool(OutCurrent_25,InDegTheTheta);

 InDegTheTheta  = 10. ; 
 std::ofstream  OutCurrent_10 ;
 OutCurrent_10.open("Out.MuonDynamicLayerCreatorTesterMboyCo10");
 OutCurrent_10 << setiosflags(std::ios::fixed);
 DumpTestMboyCollectMatterTool(OutCurrent_10,InDegTheTheta);

 InDegTheTheta  = 105. ; 
 std::ofstream  OutCurrent_105 ;
 OutCurrent_105.open("Out.MuonDynamicLayerCreatorTesterMboyCo105");
 OutCurrent_105 << setiosflags(std::ios::fixed);
 DumpTestMboyCollectMatterTool(OutCurrent_105,InDegTheTheta);

 InDegTheTheta  = 155. ; 
 std::ofstream  OutCurrent_155 ;
 OutCurrent_155.open("Out.MuonDynamicLayerCreatorTesterMboyCo155");
 OutCurrent_155 << setiosflags(std::ios::fixed);
 DumpTestMboyCollectMatterTool(OutCurrent_155,InDegTheTheta);

 InDegTheTheta  = 170. ; 
 std::ofstream  OutCurrent_170 ;
 OutCurrent_170.open("Out.MuonDynamicLayerCreatorTesterMboyCo170");
 OutCurrent_170 << setiosflags(std::ios::fixed);
 DumpTestMboyCollectMatterTool(OutCurrent_170,InDegTheTheta);


 double Zconstant ; 

//MuonDynamicLayerCreatorTesterMboyCoZ
 Zconstant  = 10000. ; 
 std::ofstream  OutCurrent_Zfixed ;
 OutCurrent_Zfixed.open("Out.MuonDynamicLayerCreatorTesterMboyCoZ");
 OutCurrent_Zfixed << setiosflags(std::ios::fixed);
 DumpTest2MboyCollectMatterTool(OutCurrent_Zfixed,Zconstant);

 Zconstant  = -10000. ; 
 std::ofstream  OutCurrent_nZfixed ;
 OutCurrent_nZfixed.open("Out.MuonDynamicLayerCreatorTesterMboyConZ");
 OutCurrent_nZfixed << setiosflags(std::ios::fixed);
 DumpTest2MboyCollectMatterTool(OutCurrent_nZfixed,Zconstant);


 ATH_MSG_INFO(  
        " Ending TestMboyCollectMatterTool "
      ) ;


}

void MuonDynamicLayerCreatorTester::TestMuonDynamicLayerCreator(){

 ATH_MSG_INFO(  
        " Starting TestMuonDynamicLayerCreator "
      ) ;

 double InDegTheTheta ; 

//MuonDynamicLayerCreatorTesterMuonDyA
 InDegTheTheta  = 75. ; 
 std::ofstream  OutCurrentA_75 ;
 OutCurrentA_75.open("Out.MuonDynamicLayerCreatorTesterMuonDyA75");
 OutCurrentA_75 << setiosflags(std::ios::fixed);
 DumpTestMuonDynamicLayerCreatorA(OutCurrentA_75,InDegTheTheta);

 InDegTheTheta  = 25. ; 
 std::ofstream  OutCurrentA_25 ;
 OutCurrentA_25.open("Out.MuonDynamicLayerCreatorTesterMuonDyA25");
 OutCurrentA_25 << setiosflags(std::ios::fixed);
 DumpTestMuonDynamicLayerCreatorA(OutCurrentA_25,InDegTheTheta);

 InDegTheTheta  = 10. ; 
 std::ofstream  OutCurrentA_10 ;
 OutCurrentA_10.open("Out.MuonDynamicLayerCreatorTesterMuonDyA10");
 OutCurrentA_10 << setiosflags(std::ios::fixed);
 DumpTestMuonDynamicLayerCreatorA(OutCurrentA_10,InDegTheTheta);

 InDegTheTheta  = 105. ; 
 std::ofstream  OutCurrentA_105 ;
 OutCurrentA_105.open("Out.MuonDynamicLayerCreatorTesterMuonDyA105");
 OutCurrentA_105 << setiosflags(std::ios::fixed);
 DumpTestMuonDynamicLayerCreatorA(OutCurrentA_105,InDegTheTheta);

 InDegTheTheta  = 155. ; 
 std::ofstream  OutCurrentA_155 ;
 OutCurrentA_155.open("Out.MuonDynamicLayerCreatorTesterMuonDyA155");
 OutCurrentA_155 << setiosflags(std::ios::fixed);
 DumpTestMuonDynamicLayerCreatorA(OutCurrentA_155,InDegTheTheta);

 InDegTheTheta  = 170. ; 
 std::ofstream  OutCurrentA_170 ;
 OutCurrentA_170.open("Out.MuonDynamicLayerCreatorTesterMuonDyA170");
 OutCurrentA_170 << setiosflags(std::ios::fixed);
 DumpTestMuonDynamicLayerCreatorA(OutCurrentA_170,InDegTheTheta);


//MuonDynamicLayerCreatorTesterMuonDyB
 InDegTheTheta  = 75. ; 
 std::ofstream  OutCurrentB_75 ;
 OutCurrentB_75.open("Out.MuonDynamicLayerCreatorTesterMuonDyB75");
 OutCurrentB_75 << setiosflags(std::ios::fixed);
 DumpTestMuonDynamicLayerCreatorB(OutCurrentB_75,InDegTheTheta);

 InDegTheTheta  = 25. ; 
 std::ofstream  OutCurrentB_25 ;
 OutCurrentB_25.open("Out.MuonDynamicLayerCreatorTesterMuonDyB25");
 OutCurrentB_25 << setiosflags(std::ios::fixed);
 DumpTestMuonDynamicLayerCreatorB(OutCurrentB_25,InDegTheTheta);

 InDegTheTheta  = 10. ; 
 std::ofstream  OutCurrentB_10 ;
 OutCurrentB_10.open("Out.MuonDynamicLayerCreatorTesterMuonDyB10");
 OutCurrentB_10 << setiosflags(std::ios::fixed);
 DumpTestMuonDynamicLayerCreatorB(OutCurrentB_10,InDegTheTheta);

 InDegTheTheta  = 105. ; 
 std::ofstream  OutCurrentB_105 ;
 OutCurrentB_105.open("Out.MuonDynamicLayerCreatorTesterMuonDyB105");
 OutCurrentB_105 << setiosflags(std::ios::fixed);
 DumpTestMuonDynamicLayerCreatorB(OutCurrentB_105,InDegTheTheta);

 InDegTheTheta  = 155. ; 
 std::ofstream  OutCurrentB_155 ;
 OutCurrentB_155.open("Out.MuonDynamicLayerCreatorTesterMuonDyB155");
 OutCurrentB_155 << setiosflags(std::ios::fixed);
 DumpTestMuonDynamicLayerCreatorB(OutCurrentB_155,InDegTheTheta);

 InDegTheTheta  = 170. ; 
 std::ofstream  OutCurrentB_170 ;
 OutCurrentB_170.open("Out.MuonDynamicLayerCreatorTesterMuonDyB170");
 OutCurrentB_170 << setiosflags(std::ios::fixed);
 DumpTestMuonDynamicLayerCreatorB(OutCurrentB_170,InDegTheTheta);


 double Zconstant ; 

//MuonDynamicLayerCreatorTesterMboyDyZ
 Zconstant  = 10000. ; 
 std::ofstream  OutCurrent_Zfixed ;
 OutCurrent_Zfixed.open("Out.MuonDynamicLayerCreatorTesterMboyDyZ");
 OutCurrent_Zfixed << setiosflags(std::ios::fixed);
 DumpTest2MuonDynamicLayerCreator(OutCurrent_Zfixed,Zconstant);

 Zconstant  =-10000. ; 
 std::ofstream  OutCurrent_nZfixed ;
 OutCurrent_nZfixed.open("Out.MuonDynamicLayerCreatorTesterMboyDynZ");
 OutCurrent_nZfixed << setiosflags(std::ios::fixed);
 DumpTest2MuonDynamicLayerCreator(OutCurrent_nZfixed,Zconstant);


//MuonDynamicLayerCreatorTesterDyT375
 InDegTheTheta  = 75. ; 
 std::ofstream  OutCurrent_DyT375 ;
 OutCurrent_DyT375.open("Out.MuonDynamicLayerCreatorTesterDyT375");
 OutCurrent_DyT375 << setiosflags(std::ios::fixed);
 DumpTest3MuonDynamicLayerCreator(OutCurrent_DyT375,InDegTheTheta);


//MuonDynamicLayerCreatorTesterFromStarting2Pointed
 double X_Starting = m_X_Starting ;
 double Y_Starting = m_Y_Starting ;
 double Z_Starting = m_Z_Starting ;
 double X_Pointed  = m_X_Pointed  ;
 double Y_Pointed  = m_Y_Pointed  ;
 double Z_Pointed  = m_Z_Pointed  ;
 
 std::ofstream  OutCurrent_FromStarting2Pointed ;
 OutCurrent_FromStarting2Pointed.open("Out.MuonDynamicLayerCreatorTesterFromStarting2Pointed");
 OutCurrent_FromStarting2Pointed << setiosflags(std::ios::fixed);
 DumpTestMuonDynamicLayerCreatorFromStarting2Pointed(
                                  OutCurrent_FromStarting2Pointed ,
                                  X_Starting ,
                                  Y_Starting ,
                                  Z_Starting ,
                                  X_Pointed  ,
                                  Y_Pointed  ,
                                  Z_Pointed
                                 );

//MuonDynamicLayerCreatorTesterFromStartingPointAndMom
 double PX_Starting = m_PX_Starting ;
 double PY_Starting = m_PY_Starting ;
 double PZ_Starting = m_PZ_Starting ;
 
 std::ofstream  OutCurrent_FromStartingPointAndMom ;
 OutCurrent_FromStartingPointAndMom.open("Out.MuonDynamicLayerCreatorTesterFromStartingPointAndMom");
 OutCurrent_FromStartingPointAndMom << setiosflags(std::ios::fixed);
 DumpTestMuonDynamicLayerCreatorFromStartingPointAndMom(
                                  OutCurrent_FromStartingPointAndMom ,
                                  X_Starting  ,
                                  Y_Starting  ,
                                  Z_Starting  ,
                                  PX_Starting ,
                                  PY_Starting ,
                                  PZ_Starting   
                                 );


 ATH_MSG_INFO(  
        " Ending TestMuonDynamicLayerCreator "
      ) ;


}

void MuonDynamicLayerCreatorTester::DumpTestMuonDynamicLayerCreatorA(
                                      std::ofstream&  OutCurrent,
                                      double          InDegTheTheta
){

 int    NberOfPoints   = m_NberOfPhiPoints ;
 double InDegThePhiMin = m_InDegThePhiMin  ; 
 double InDegThePhiMax = m_InDegThePhiMax  ; 
 
 double Mom       = m_MomentumTest ;
 double TheCharge = m_ChargeTest   ;
 
 OutCurrent 
      << "------------------------------------------------------"
      << std::endl;
 OutCurrent 
      << "-Format "
      << std::endl;
 OutCurrent 
      << "- first line: Theta in deg "
      << std::endl;
 OutCurrent 
      << "- Conditions line: Momentum Theta Phi SumOfX0 NberOfX0points "
      << std::endl;
 OutCurrent 
      << "                   followed by the list of NberOfX0points X0 at each of the points"
      << std::endl;
 OutCurrent 
      << " InDegTheTheta " << InDegTheTheta
      << std::endl;


 double aRadius = 1.;
 double aZClynd = 1.;
 Trk::CylinderSurface aDummySurface(new Amg::Transform3D, aRadius , aZClynd );
 const Trk::IPropagator* pDummyIPropagator = 0;


 Amg::Vector3D aGlobalPositionDummy(0.,0.,0.);
 double InDegThePhi    =  0. ;  
 double DeltaPhi = InDegThePhiMax - InDegThePhiMin      ;
 DeltaPhi = DeltaPhi / ( ( (double) NberOfPoints ) - 1. ) ;
 for (int iPhi=0; iPhi<NberOfPoints ; iPhi++){
   InDegThePhi = InDegThePhiMin + DeltaPhi * ( (double) iPhi ) ;
   Trk::AtaPlane* pAtaPlane = AtaPlaneOnMuSpectroSurfAnglInDeg(
                                                             InDegThePhi    , 
                                                             InDegTheTheta  , 
                                                             Mom            , 
                                                             TheCharge  
                                                            );
   const std::vector< std::pair<const Trk::TrackParameters*,const Trk::Layer* > >* pPairVector =
   p_IDynamicLayerCreator->materialLayers(
      (*pAtaPlane),
      aDummySurface,
      pDummyIPropagator
      );
      
   if ( pPairVector ){
   
     double thicknessInX0 = 0.;
     std::vector<double> VectorOfX0;
     std::vector<double> VectorOfTheX;
     std::vector<double> VectorOfTheY;
     std::vector<double> VectorOfTheZ;
     int SizeOfpPairVector = pPairVector->size();
     for (int iPlaneLayer= 0 ; iPlaneLayer <SizeOfpPairVector ; iPlaneLayer++){

       double TheX0 =
       (
        ( ((*pPairVector)[iPlaneLayer].second)->layerMaterialProperties() )
            ->fullMaterial(aGlobalPositionDummy) )
            ->thicknessInX0 ();
       thicknessInX0 = thicknessInX0 + TheX0 ;
       VectorOfX0.push_back(TheX0);
                 
       double TheX = 0. ;
       double TheY = 0. ;
       double TheZ = 0. ;
       const Trk::PlaneLayer* pPlaneLayer
         = dynamic_cast<const Trk::PlaneLayer*> ((*pPairVector)[iPlaneLayer].second);
       if (pPlaneLayer){
         TheX = (pPlaneLayer->center()).x();
         TheY = (pPlaneLayer->center()).y();
         TheZ = (pPlaneLayer->center()).z();
       }
       VectorOfTheX.push_back(TheX);          
       VectorOfTheY.push_back(TheY);          
       VectorOfTheZ.push_back(TheZ);          

     }

     int SizeOfVectorOfX0 = VectorOfX0.size();
     OutCurrent 
          << "Conditions "
          << std::setw(10)<<std::setprecision(1)<< Mom
          << std::setw(12)<<std::setprecision(5)<< InDegTheTheta
          << std::setw(12)<<std::setprecision(5)<< InDegThePhi
          << std::setw(12)<<std::setprecision(2)<< thicknessInX0 
          << std::setw(12)                      << SizeOfVectorOfX0 
          << std::endl;
     for (int iX0= 0 ; iX0 <SizeOfVectorOfX0 ; iX0++){
       double vtemp = sqrt(
                             std::pow ( VectorOfTheX[iX0] , 2)
                           + std::pow ( VectorOfTheY[iX0] , 2)
                           + std::pow ( VectorOfTheZ[iX0] , 2) 
                          );
       OutCurrent 
          << std::setw(12)<<std::setprecision(5)<< VectorOfX0[iX0]
          << std::setw(12)<<std::setprecision(3)<< VectorOfTheX[iX0]
          << std::setw(12)<<std::setprecision(3)<< VectorOfTheY[iX0]
          << std::setw(12)<<std::setprecision(3)<< VectorOfTheZ[iX0]
          << std::setw(12)<<std::setprecision(3)<< vtemp
          << std::endl;
     }

     for (int iPlaneLayer= 0 ; iPlaneLayer <SizeOfpPairVector ; iPlaneLayer++){
       delete ((*pPairVector)[iPlaneLayer].first);
       delete ((*pPairVector)[iPlaneLayer].second);
     }
     delete pPairVector ;

   }
   
 }

}

void MuonDynamicLayerCreatorTester::DumpTestMuonDynamicLayerCreatorB(
                                      std::ofstream&  OutCurrent,
                                      double          InDegTheTheta
){

 int    NberOfPoints   = m_NberOfPhiPoints ;
 double InDegThePhiMin = m_InDegThePhiMin  ; 
 double InDegThePhiMax = m_InDegThePhiMax  ; 
 
 double Mom       = m_MomentumTest ;
 double TheCharge = m_ChargeTest   ;
 
 OutCurrent 
      << "------------------------------------------------------"
      << std::endl;
 OutCurrent 
      << "-Format "
      << std::endl;
 OutCurrent 
      << "- first line: Theta in deg "
      << std::endl;
 OutCurrent 
      << "- Conditions line: Momentum Theta Phi SumOfX0 NberOfX0points "
      << std::endl;
 OutCurrent 
      << "                   followed by the list of NberOfX0points X0 at each of the points"
      << std::endl;
 OutCurrent 
      << " InDegTheTheta " << InDegTheTheta
      << std::endl;


 double aRadius = 1.;
 double aZClynd = 1.;
 Trk::CylinderSurface aDummySurface(new Amg::Transform3D, aRadius , aZClynd );
 const Trk::IPropagator* pDummyIPropagator = 0;


 Amg::Vector3D aGlobalPositionDummy(0.,0.,0.);
 double InDegThePhi    =  0. ;  
 double DeltaPhi = InDegThePhiMax - InDegThePhiMin      ;
 DeltaPhi = DeltaPhi / ( ( (double) NberOfPoints ) - 1. ) ;
 for (int iPhi=0; iPhi<NberOfPoints ; iPhi++){
   InDegThePhi = InDegThePhiMin + DeltaPhi * ( (double) iPhi ) ;
   Trk::AtaPlane* pAtaPlane = AtaPlaneOnIPAnglInDeg(
                                                             InDegThePhi    , 
                                                             InDegTheTheta  , 
                                                             Mom            , 
                                                             TheCharge  
                                                            );
   const std::vector< std::pair<const Trk::TrackParameters*,const Trk::Layer* > >* pPairVector =
   p_IDynamicLayerCreator->materialLayers(
      (*pAtaPlane),
      aDummySurface,
      pDummyIPropagator
      );
      
   if ( pPairVector ){

     double thicknessInX0 = 0.;
     std::vector<double> VectorOfX0;
     std::vector<double> VectorOfTheX;
     std::vector<double> VectorOfTheY;
     std::vector<double> VectorOfTheZ;
     int SizeOfpPairVector = pPairVector->size();
     for (int iPlaneLayer= 0 ; iPlaneLayer <SizeOfpPairVector ; iPlaneLayer++){

       double TheX0 =
       (
        ( ((*pPairVector)[iPlaneLayer].second)->layerMaterialProperties() )
            ->fullMaterial(aGlobalPositionDummy) )
            ->thicknessInX0 ();
       thicknessInX0 = thicknessInX0 + TheX0 ;
       VectorOfX0.push_back(TheX0);          
                 
       double TheX = 0. ;
       double TheY = 0. ;
       double TheZ = 0. ;
       const Trk::PlaneLayer* pPlaneLayer
         = dynamic_cast<const Trk::PlaneLayer*> ((*pPairVector)[iPlaneLayer].second);
       if (pPlaneLayer){
         TheX = (pPlaneLayer->center()).x();
         TheY = (pPlaneLayer->center()).y();
         TheZ = (pPlaneLayer->center()).z();
       }
       VectorOfTheX.push_back(TheX);          
       VectorOfTheY.push_back(TheY);          
       VectorOfTheZ.push_back(TheZ);          

     }

     int SizeOfVectorOfX0 = VectorOfX0.size();
     OutCurrent 
          << "Conditions "
          << std::setw(10)<<std::setprecision(1)<< Mom
          << std::setw(12)<<std::setprecision(5)<< InDegTheTheta
          << std::setw(12)<<std::setprecision(5)<< InDegThePhi
          << std::setw(12)<<std::setprecision(2)<< thicknessInX0 
          << std::setw(12)                      << SizeOfVectorOfX0 
          << std::endl;
     for (int iX0= 0 ; iX0 <SizeOfVectorOfX0 ; iX0++){
       double vtemp = sqrt(
                             std::pow ( VectorOfTheX[iX0] , 2)
                           + std::pow ( VectorOfTheY[iX0] , 2)
                           + std::pow ( VectorOfTheZ[iX0] , 2) 
                          );
       OutCurrent 
          << std::setw(12)<<std::setprecision(5)<< VectorOfX0[iX0]
          << std::setw(12)<<std::setprecision(3)<< VectorOfTheX[iX0]
          << std::setw(12)<<std::setprecision(3)<< VectorOfTheY[iX0]
          << std::setw(12)<<std::setprecision(3)<< VectorOfTheZ[iX0]
          << std::setw(12)<<std::setprecision(3)<< vtemp
          << std::endl;
     }

     for (int iPlaneLayer= 0 ; iPlaneLayer <SizeOfpPairVector ; iPlaneLayer++){
       delete ((*pPairVector)[iPlaneLayer].first);
       delete ((*pPairVector)[iPlaneLayer].second);
     }
     delete pPairVector ;

   }
   
 }

}

void MuonDynamicLayerCreatorTester::DumpTest2MuonDynamicLayerCreator(
                                      std::ofstream&  OutCurrent,
                                      double          Zconstant
){

 int    NberOfPoints   = m_NberOfPhiPoints ;
 double InDegThePhiMin = m_InDegThePhiMin  ; 
 double InDegThePhiMax = m_InDegThePhiMax  ; 
 
 double Mom       = m_MomentumTest ;
 double TheCharge = m_ChargeTest   ;
 
 OutCurrent 
      << "------------------------------------------------------"
      << std::endl;
 OutCurrent 
      << "-Format "
      << std::endl;
 OutCurrent 
      << "- first line: Z cte "
      << std::endl;
 OutCurrent 
      << "- Conditions line: Momentum Zconstant Phi SumOfX0 NberOfX0points "
      << std::endl;
 OutCurrent 
      << "                   followed by the list of NberOfX0points X0 at each of the points"
      << std::endl;
 OutCurrent 
      << " Zconstant " << Zconstant
      << std::endl;


 double aRadius = 1.;
 double aZClynd = 1.;
 Trk::CylinderSurface aDummySurface(new Amg::Transform3D, aRadius , aZClynd );
 const Trk::IPropagator* pDummyIPropagator = 0;


 Amg::Vector3D aGlobalPositionDummy(0.,0.,0.);
 double InDegThePhi    =  0. ;  
 double DeltaPhi = InDegThePhiMax - InDegThePhiMin      ;
 DeltaPhi = DeltaPhi / ( ( (double) NberOfPoints ) - 1. ) ;
 for (int iPhi=0; iPhi<NberOfPoints ; iPhi++){
   InDegThePhi = InDegThePhiMin + DeltaPhi * ( (double) iPhi ) ;
   Trk::AtaPlane* pAtaPlane = AtaPlaneTransAtZfixedAnglInDeg(
                                                             InDegThePhi    , 
                                                             Zconstant      , 
                                                             Mom            , 
                                                             TheCharge  
                                                            );
   const std::vector< std::pair<const Trk::TrackParameters*,const Trk::Layer* > >* pPairVector =
   p_IDynamicLayerCreator->materialLayers(
      (*pAtaPlane),
      aDummySurface,
      pDummyIPropagator
      );
      
   if ( pPairVector ){

     double thicknessInX0 = 0.;
     std::vector<double> VectorOfX0;
     std::vector<double> VectorOfTheX;
     std::vector<double> VectorOfTheY;
     std::vector<double> VectorOfTheZ;
     int SizeOfpPairVector = pPairVector->size();
     for (int iPlaneLayer= 0 ; iPlaneLayer <SizeOfpPairVector ; iPlaneLayer++){

       double TheX0 =
       (
        ( ((*pPairVector)[iPlaneLayer].second)->layerMaterialProperties() )
            ->fullMaterial(aGlobalPositionDummy) )
            ->thicknessInX0 ();
       thicknessInX0 = thicknessInX0 + TheX0 ;
       VectorOfX0.push_back(TheX0);          
                 
       double TheX = 0. ;
       double TheY = 0. ;
       double TheZ = 0. ;
       const Trk::PlaneLayer* pPlaneLayer
         = dynamic_cast<const Trk::PlaneLayer*> ((*pPairVector)[iPlaneLayer].second);
       if (pPlaneLayer){
         TheX = (pPlaneLayer->center()).x();
         TheY = (pPlaneLayer->center()).y();
         TheZ = (pPlaneLayer->center()).z();
       }
       VectorOfTheX.push_back(TheX);          
       VectorOfTheY.push_back(TheY);          
       VectorOfTheZ.push_back(TheZ);          

     }

     int SizeOfVectorOfX0 = VectorOfX0.size();
     OutCurrent 
          << "Conditions "
          << std::setw(10)<<std::setprecision(1)<< Mom
          << std::setw(12)<<std::setprecision(5)<< Zconstant
          << std::setw(12)<<std::setprecision(5)<< InDegThePhi
          << std::setw(12)<<std::setprecision(2)<< thicknessInX0 
          << std::setw(12)                      << SizeOfVectorOfX0 
          << std::endl;
     for (int iX0= 0 ; iX0 <SizeOfVectorOfX0 ; iX0++){
       double vtemp = sqrt(
                             std::pow ( VectorOfTheX[iX0] , 2)
                           + std::pow ( VectorOfTheY[iX0] , 2)
                           + std::pow ( VectorOfTheZ[iX0] , 2) 
                          );
       OutCurrent 
          << std::setw(12)<<std::setprecision(5)<< VectorOfX0[iX0]
          << std::setw(12)<<std::setprecision(3)<< VectorOfTheX[iX0]
          << std::setw(12)<<std::setprecision(3)<< VectorOfTheY[iX0]
          << std::setw(12)<<std::setprecision(3)<< VectorOfTheZ[iX0]
          << std::setw(12)<<std::setprecision(3)<< vtemp
          << std::endl;
     }

     for (int iPlaneLayer= 0 ; iPlaneLayer <SizeOfpPairVector ; iPlaneLayer++){
       delete ((*pPairVector)[iPlaneLayer].first);
       delete ((*pPairVector)[iPlaneLayer].second);
     }
     delete pPairVector ;

   }
   
 }

}

void MuonDynamicLayerCreatorTester::DumpTest3MuonDynamicLayerCreator(
                                      std::ofstream&  OutCurrent,
                                      double          InDegTheTheta
){

 double Mom       = m_MomentumTest ;
 double TheCharge = m_ChargeTest   ;
 
 OutCurrent 
      << "------------------------------------------------------"
      << std::endl;
 OutCurrent 
      << "-Format "
      << std::endl;
 OutCurrent 
      << "- first line: Theta in deg "
      << std::endl;
 OutCurrent 
      << "- Conditions line: Momentum Theta Phi SumOfX0 NberOfX0points "
      << std::endl;
 OutCurrent 
      << "                   followed by the list of NberOfX0points X0 at each of the points"
      << std::endl;
 OutCurrent 
      << " InDegTheTheta " << InDegTheTheta
      << std::endl;


 double aRadius = 1.;
 double aZClynd = 1.;
 Trk::CylinderSurface aDummySurface(new Amg::Transform3D, aRadius , aZClynd );
 const Trk::IPropagator* pDummyIPropagator = 0;


 Amg::Vector3D aGlobalPositionDummy(0.,0.,0.);
 
 int iTurnMax = 20 ;
 int iTry = 1 ;
 Amg::Vector3D  aGlobalPositionNewStart(0.,0.,0.);
 Amg::Vector3D  aGlobalDirectionNewStart(0.,0.,0.);
 for (int iTurn= 0 ; iTurn <iTurnMax ; iTurn++){
   if (iTry == 1 ){
     Trk::AtaPlane* pAtaPlane = 0 ;
     
     if (iTurn==0){
       double InDegThePhi = 0. ;
       pAtaPlane = AtaPlaneOnIPAnglInDeg(
                                         InDegThePhi    , 
                                         InDegTheTheta  , 
                                         Mom            , 
                                         TheCharge  
                                        );
     }else{
       double Posx = aGlobalPositionNewStart.x() ;
       double Posy = aGlobalPositionNewStart.y() ;
       double Posz = aGlobalPositionNewStart.z() ;                                 
       double MomX = aGlobalDirectionNewStart.x() * Mom ;
       double MomY = aGlobalDirectionNewStart.y() * Mom ;
       double MomZ = aGlobalDirectionNewStart.z() * Mom ;                              
       pAtaPlane = AtaPlaneUltraBase(
                                     TheCharge ,
                                     Posx      ,
                                     Posy      ,
                                     Posz      ,                                  
                                     MomX      ,
                                     MomY      ,
                                     MomZ                                         
                                    );
     }
     
     const std::vector< std::pair<const Trk::TrackParameters*,const Trk::Layer* > >* pPairVector =
     p_IDynamicLayerCreator->materialLayers(
        (*pAtaPlane),
        aDummySurface,
        pDummyIPropagator
        );

     if ( pPairVector ){

       int SizeOfpPairVector = pPairVector->size();
       if (SizeOfpPairVector==0){
         iTry = 0 ;
       }else{
         double thicknessInX0 = 0.;
         std::vector<double> VectorOfX0;
         std::vector<double> VectorOfTheX;
         std::vector<double> VectorOfTheY;
         std::vector<double> VectorOfTheZ;
         std::vector<double> VectorOfTheUX;
         std::vector<double> VectorOfTheUY;
         std::vector<double> VectorOfTheUZ;
         for (int iPlaneLayer= 0 ; iPlaneLayer <SizeOfpPairVector ; iPlaneLayer++){

           double TheX0 =
           (
            ( ((*pPairVector)[iPlaneLayer].second)->layerMaterialProperties() )
                ->fullMaterial(aGlobalPositionDummy) )
                ->thicknessInX0 ();
           thicknessInX0 = thicknessInX0 + TheX0 ;
           VectorOfX0.push_back(TheX0);          
                     
           double TheX   = 0. ;
           double TheY   = 0. ;
           double TheZ   = 0. ;
           double TheUX  = 0. ;
           double TheUY  = 0. ;
           double TheUZ  = 0. ;
           const Trk::PlaneLayer* pPlaneLayer
             = dynamic_cast<const Trk::PlaneLayer*> ((*pPairVector)[iPlaneLayer].second);
           if (pPlaneLayer){
             TheX   = (pPlaneLayer->center()).x();
             TheY   = (pPlaneLayer->center()).y();
             TheZ   = (pPlaneLayer->center()).z();
             TheUX  = ((pPlaneLayer->normal()).unit()).x();
             TheUY  = ((pPlaneLayer->normal()).unit()).y();
             TheUZ  = ((pPlaneLayer->normal()).unit()).z();
             if (iPlaneLayer == 0) {
               aGlobalPositionNewStart  = pPlaneLayer->center() ;
               aGlobalDirectionNewStart = (pPlaneLayer->normal()).normalized() ;
             }
           }else{
            iTry = 0 ;
           }
           VectorOfTheX.push_back(TheX);          
           VectorOfTheY.push_back(TheY);          
           VectorOfTheZ.push_back(TheZ);          
           VectorOfTheUX.push_back(TheUX);          
           VectorOfTheUY.push_back(TheUY);          
           VectorOfTheUZ.push_back(TheUZ);          

         }

         int SizeOfVectorOfX0 = VectorOfX0.size();
         OutCurrent 
              << "Conditions "
              << std::setw(10)<<std::setprecision(1)<< (pAtaPlane->momentum()).mag()
              << std::setw(12)<<std::setprecision(5)<< (pAtaPlane->position()).x()
              << std::setw(12)<<std::setprecision(5)<< (pAtaPlane->position()).y()
              << std::setw(12)<<std::setprecision(5)<< (pAtaPlane->position()).z()
              << std::setw(12)<<std::setprecision(5)<< (pAtaPlane->momentum()).unit().x()
              << std::setw(12)<<std::setprecision(5)<< (pAtaPlane->momentum()).unit().y()
              << std::setw(12)<<std::setprecision(5)<< (pAtaPlane->momentum()).unit().z()
              << std::setw(12)<<std::setprecision(2)<< thicknessInX0 
              << std::setw(12)                      << SizeOfVectorOfX0 
              << std::endl;
         for (int iX0= 0 ; iX0 <SizeOfVectorOfX0 ; iX0++){
           double vtemp = sqrt(
                                 std::pow ( VectorOfTheX[iX0] , 2)
                               + std::pow ( VectorOfTheY[iX0] , 2)
                               + std::pow ( VectorOfTheZ[iX0] , 2) 
                              );
           OutCurrent 
              << std::setw(12)<<std::setprecision(5)<< VectorOfX0[iX0]
              << std::setw(12)<<std::setprecision(3)<< VectorOfTheX[iX0]
              << std::setw(12)<<std::setprecision(3)<< VectorOfTheY[iX0]
              << std::setw(12)<<std::setprecision(3)<< VectorOfTheZ[iX0]
              << std::setw(12)<<std::setprecision(3)<< vtemp
              << std::setw(12)<<std::setprecision(3)<< VectorOfTheUX[iX0]
              << std::setw(12)<<std::setprecision(3)<< VectorOfTheUY[iX0]
              << std::setw(12)<<std::setprecision(3)<< VectorOfTheUZ[iX0]
              << std::endl;
         }

         for (int iPlaneLayer= 0 ; iPlaneLayer <SizeOfpPairVector ; iPlaneLayer++){
           delete ((*pPairVector)[iPlaneLayer].first);
           delete ((*pPairVector)[iPlaneLayer].second);
         }
       }
       
       delete pPairVector ;
       
     }
     
   }
   
 }

}

void MuonDynamicLayerCreatorTester::DumpTestMuonDynamicLayerCreatorFromStarting2Pointed(
                                      std::ofstream&  OutCurrent,
                                      double X_Starting ,
                                      double Y_Starting ,
                                      double Z_Starting ,
                                      double X_Pointed  ,
                                      double Y_Pointed  ,
                                      double Z_Pointed    
){

 double Mom       = m_MomentumTest ;
 double TheCharge = m_ChargeTest   ;
 
 OutCurrent 
      << "------------------------------------------------------"
      << std::endl;
 OutCurrent 
      << "-Format "
      << std::endl;
 OutCurrent 
      << "- first  line: Start   position "
      << "- second line: Pointed position "
      << "- third  line: Start   position Charge Momentum "
      << std::endl;
 OutCurrent 
      << "- Conditions line: Momentum SumOfX0 NberOfX0points "
      << std::endl;
 OutCurrent 
      << "                   followed by the list of NberOfX0points X0 at each of the points"
      << std::endl;
 OutCurrent 
      << "  " << X_Starting
      << "  " << Y_Starting
      << "  " << Z_Starting
      << std::endl;
 OutCurrent 
      << "  " << X_Pointed
      << "  " << Y_Pointed
      << "  " << Z_Pointed
      << std::endl;


 double MomX = 1. ;
 double MomY = 1. ;
 double MomZ = 1. ;

 GiveMomentumForFromStarting2Pointed(
                                     X_Starting ,
                                     Y_Starting ,
                                     Z_Starting ,
                                     X_Pointed  ,
                                     Y_Pointed  ,
                                     Z_Pointed  ,
                                     Mom        , 
                                     MomX       ,
                                     MomY       ,
                                     MomZ   
                                    );    
 OutCurrent 
      << std::setw(16)<<std::setprecision(4)<< X_Starting
      << std::setw(16)<<std::setprecision(4)<< Y_Starting
      << std::setw(16)<<std::setprecision(4)<< Z_Starting
      << std::setw(16)<<std::setprecision(1)<< TheCharge
      << std::setw(16)<<std::setprecision(1)<< MomX
      << std::setw(16)<<std::setprecision(1)<< MomY
      << std::setw(16)<<std::setprecision(1)<< MomZ
      << std::endl;
 Trk::AtaPlane* pAtaPlane = AtaPlaneUltraBase(
                                              TheCharge ,
                                              X_Starting   ,
                                              Y_Starting   ,
                                              Z_Starting   ,                                  
                                              MomX      ,
                                              MomY      ,
                                              MomZ                                         
                                             );

 double aRadius = 1.;
 double aZClynd = 1.;
 Trk::CylinderSurface aDummySurface(new Amg::Transform3D, aRadius , aZClynd );
 const Trk::IPropagator* pDummyIPropagator = 0;

 const std::vector< std::pair<const Trk::TrackParameters*,const Trk::Layer* > >* pPairVector =
 p_IDynamicLayerCreator->materialLayers(
    (*pAtaPlane),
    aDummySurface,
    pDummyIPropagator
    );

 if ( pPairVector ){

   Amg::Vector3D aGlobalPositionDummy(0.,0.,0.);
   double thicknessInX0 = 0.;
   std::vector<double> VectorOfX0;
   std::vector<double> VectorOfTheX;
   std::vector<double> VectorOfTheY;
   std::vector<double> VectorOfTheZ;
   int SizeOfpPairVector = pPairVector->size();
   for (int iPlaneLayer= 0 ; iPlaneLayer <SizeOfpPairVector ; iPlaneLayer++){

     double TheX0 =
     (
      ( ((*pPairVector)[iPlaneLayer].second)->layerMaterialProperties() )
          ->fullMaterial(aGlobalPositionDummy) )
          ->thicknessInX0 ();
     thicknessInX0 = thicknessInX0 + TheX0 ;
     VectorOfX0.push_back(TheX0);          
               
     double TheX = 0. ;
     double TheY = 0. ;
     double TheZ = 0. ;
     const Trk::PlaneLayer* pPlaneLayer
       = dynamic_cast<const Trk::PlaneLayer*> ((*pPairVector)[iPlaneLayer].second);
     if (pPlaneLayer){
       TheX = (pPlaneLayer->center()).x();
       TheY = (pPlaneLayer->center()).y();
       TheZ = (pPlaneLayer->center()).z();
     }
     VectorOfTheX.push_back(TheX);          
     VectorOfTheY.push_back(TheY);          
     VectorOfTheZ.push_back(TheZ);          

   }


   int SizeOfVectorOfX0 = VectorOfX0.size();
   OutCurrent 
        << "Conditions "
        << std::setw(10)<<std::setprecision(1)<< Mom
        << std::setw(12)<<std::setprecision(2)<< thicknessInX0 
        << std::setw(12)                      << SizeOfVectorOfX0 
        << std::endl;
   for (int iX0= 0 ; iX0 <SizeOfVectorOfX0 ; iX0++){
     double vtemp = sqrt(
                           std::pow ( VectorOfTheX[iX0] , 2)
                         + std::pow ( VectorOfTheY[iX0] , 2)
                         + std::pow ( VectorOfTheZ[iX0] , 2) 
                        );
     OutCurrent 
        << std::setw(12)<<std::setprecision(5)<< VectorOfX0[iX0]
        << std::setw(12)<<std::setprecision(3)<< VectorOfTheX[iX0]
        << std::setw(12)<<std::setprecision(3)<< VectorOfTheY[iX0]
        << std::setw(12)<<std::setprecision(3)<< VectorOfTheZ[iX0]
        << std::setw(12)<<std::setprecision(3)<< vtemp
        << std::endl;
   }

   for (int iPlaneLayer= 0 ; iPlaneLayer <SizeOfpPairVector ; iPlaneLayer++){
     delete ((*pPairVector)[iPlaneLayer].first);
     delete ((*pPairVector)[iPlaneLayer].second);
   }
   delete pPairVector ;

 }

}

void MuonDynamicLayerCreatorTester::DumpTestMuonDynamicLayerCreatorFromStartingPointAndMom(
                                      std::ofstream&  OutCurrent,
                                      double X_Starting  ,
                                      double Y_Starting  ,
                                      double Z_Starting  ,
                                      double PX_Starting ,
                                      double PY_Starting ,
                                      double PZ_Starting   
){

 double TheCharge = m_ChargeTest   ;
 
 OutCurrent 
      << "------------------------------------------------------"
      << std::endl;
 OutCurrent 
      << "-Format "
      << std::endl;
 OutCurrent 
      << "- first  line: Start   position Charge Momentum "
      << std::endl;
 OutCurrent 
      << "- Conditions line: SumOfX0 NberOfX0points "
      << std::endl;
 OutCurrent 
      << "                   followed by the list of NberOfX0points X0 at each of the points"
      << std::endl;


 OutCurrent 
      << std::setw(16)<<std::setprecision(4)<< X_Starting
      << std::setw(16)<<std::setprecision(4)<< Y_Starting
      << std::setw(16)<<std::setprecision(4)<< Z_Starting
      << std::setw(16)<<std::setprecision(1)<< TheCharge
      << std::setw(16)<<std::setprecision(1)<< PX_Starting
      << std::setw(16)<<std::setprecision(1)<< PY_Starting
      << std::setw(16)<<std::setprecision(1)<< PZ_Starting
      << std::endl;
 Trk::AtaPlane* pAtaPlane = AtaPlaneUltraBase(
                                              TheCharge ,
                                              X_Starting   ,
                                              Y_Starting   ,
                                              Z_Starting   ,                                  
                                              PX_Starting  ,
                                              PY_Starting  ,
                                              PZ_Starting                                    
                                             );

 double aRadius = 1.;
 double aZClynd = 1.;
 Trk::CylinderSurface aDummySurface(new Amg::Transform3D, aRadius , aZClynd );
 const Trk::IPropagator* pDummyIPropagator = 0;

 const std::vector< std::pair<const Trk::TrackParameters*,const Trk::Layer* > >* pPairVector =
 p_IDynamicLayerCreator->materialLayers(
    (*pAtaPlane),
    aDummySurface,
    pDummyIPropagator
    );
 
 if ( pPairVector ){

   Amg::Vector3D aGlobalPositionDummy(0.,0.,0.);
   double thicknessInX0 = 0.;
   std::vector<double> VectorOfX0;
   std::vector<double> VectorOfTheX;
   std::vector<double> VectorOfTheY;
   std::vector<double> VectorOfTheZ;
   int SizeOfpPairVector = pPairVector->size();
   for (int iPlaneLayer= 0 ; iPlaneLayer <SizeOfpPairVector ; iPlaneLayer++){

     double TheX0 =
     (
      ( ((*pPairVector)[iPlaneLayer].second)->layerMaterialProperties() )
          ->fullMaterial(aGlobalPositionDummy) )
          ->thicknessInX0 ();
     thicknessInX0 = thicknessInX0 + TheX0 ;
     VectorOfX0.push_back(TheX0);          
               
     double TheX = 0. ;
     double TheY = 0. ;
     double TheZ = 0. ;
     const Trk::PlaneLayer* pPlaneLayer
       = dynamic_cast<const Trk::PlaneLayer*> ((*pPairVector)[iPlaneLayer].second);
     if (pPlaneLayer){
       TheX = (pPlaneLayer->center()).x();
       TheY = (pPlaneLayer->center()).y();
       TheZ = (pPlaneLayer->center()).z();
     }
     VectorOfTheX.push_back(TheX);          
     VectorOfTheY.push_back(TheY);          
     VectorOfTheZ.push_back(TheZ);          

   }

   int SizeOfVectorOfX0 = VectorOfX0.size();
   OutCurrent 
        << "Conditions "
        << std::setw(12)<<std::setprecision(2)<< thicknessInX0 
        << std::setw(12)                      << SizeOfVectorOfX0 
        << std::endl;
   for (int iX0= 0 ; iX0 <SizeOfVectorOfX0 ; iX0++){
     double vtemp = sqrt(
                           std::pow ( VectorOfTheX[iX0] , 2)
                         + std::pow ( VectorOfTheY[iX0] , 2)
                         + std::pow ( VectorOfTheZ[iX0] , 2) 
                        );
     OutCurrent 
        << std::setw(12)<<std::setprecision(5)<< VectorOfX0[iX0]
        << std::setw(12)<<std::setprecision(3)<< VectorOfTheX[iX0]
        << std::setw(12)<<std::setprecision(3)<< VectorOfTheY[iX0]
        << std::setw(12)<<std::setprecision(3)<< VectorOfTheZ[iX0]
        << std::setw(12)<<std::setprecision(3)<< vtemp
        << std::endl;
   }

   for (int iPlaneLayer= 0 ; iPlaneLayer <SizeOfpPairVector ; iPlaneLayer++){
     delete ((*pPairVector)[iPlaneLayer].first);
     delete ((*pPairVector)[iPlaneLayer].second);
   }
   delete pPairVector ;

 }
 
}

void MuonDynamicLayerCreatorTester::DumpTestMboyCollectMatterTool(
                                      std::ofstream&  OutCurrent,
                                      double          InDegTheTheta
){

 int    NberOfPoints   = m_NberOfPhiPoints ;
 double InDegThePhiMin = m_InDegThePhiMin  ; 
 double InDegThePhiMax = m_InDegThePhiMax  ; 
 
 double Mom       = m_MomentumTest ;
 double TheCharge = m_ChargeTest   ;
 
 OutCurrent 
      << "------------------------------------------------------"
      << std::endl;
 OutCurrent 
      << "-Format "
      << std::endl;
 OutCurrent 
      << "- first line: Theta in deg "
      << std::endl;
 OutCurrent 
      << "- Conditions line: Momentum Theta Phi SumOfX0 NberOfX0points "
      << std::endl;
 OutCurrent 
      << "                   followed by the list of NberOfX0points X0 at each of the points"
      << std::endl;
 OutCurrent 
      << " InDegTheTheta " << InDegTheTheta
      << std::endl;

 std::vector<std::pair<const Trk::TrackParameters*,const Trk::Layer* > > VectorOfPlaneLayer;
 Amg::Vector3D aGlobalPositionDummy(0.,0.,0.);
 double InDegThePhi    =  0. ;  
 double DeltaPhi = InDegThePhiMax - InDegThePhiMin      ;
 DeltaPhi = DeltaPhi / ( ( (double) NberOfPoints ) - 1. ) ;
 for (int iPhi=0; iPhi<NberOfPoints ; iPhi++){
   VectorOfPlaneLayer.clear();
   InDegThePhi = InDegThePhiMin + DeltaPhi * ( (double) iPhi ) ;
   Trk::AtaPlane* pAtaPlane = AtaPlaneOnMuSpectroSurfAnglInDeg(
                                                             InDegThePhi    , 
                                                             InDegTheTheta  , 
                                                             Mom            , 
                                                             TheCharge  
                                                            );
   StatusCode sc = 
   p_MboyCollectMatterTool->DoCollectMatter(
                                                             pAtaPlane,
                                                             &VectorOfPlaneLayer
                                                            );
   if ( sc.isFailure() ) OutCurrent  << " DoCollectMatter failed " << std::endl;
                                                            
   double thicknessInX0 = 0.;
   std::vector<double> VectorOfX0;
   std::vector<double> VectorOfTheX;
   std::vector<double> VectorOfTheY;
   std::vector<double> VectorOfTheZ;
   int SizeOfVectorOfPlaneLayer = VectorOfPlaneLayer.size();
   for (int iPlaneLayer= 0 ; iPlaneLayer <SizeOfVectorOfPlaneLayer ; iPlaneLayer++){
     const Trk::PlaneLayer *planelayer=dynamic_cast<const Trk::PlaneLayer *>(VectorOfPlaneLayer[iPlaneLayer].second);
     if (planelayer){
     double TheX0 =
     (
      ( planelayer->layerMaterialProperties() )
          ->fullMaterial(aGlobalPositionDummy) )
          ->thicknessInX0 ();
     thicknessInX0 = thicknessInX0 + TheX0 ;
     VectorOfX0.push_back(TheX0);          
               
     double TheX = 0. ;
     double TheY = 0. ;
     double TheZ = 0. ;
     
     TheX = (planelayer->center()).x();
     TheY = (planelayer->center()).y();
     TheZ = (planelayer->center()).z();
     VectorOfTheX.push_back(TheX);          
     VectorOfTheY.push_back(TheY);          
     VectorOfTheZ.push_back(TheZ);          

   }
   }

   int SizeOfVectorOfX0 = VectorOfX0.size();
   OutCurrent 
        << "Conditions "
        << std::setw(10)<<std::setprecision(1)<< Mom
        << std::setw(12)<<std::setprecision(5)<< InDegTheTheta
        << std::setw(12)<<std::setprecision(5)<< InDegThePhi
        << std::setw(12)<<std::setprecision(2)<< thicknessInX0 
        << std::setw(12)                      << SizeOfVectorOfX0 
        << std::endl;
   for (int iX0= 0 ; iX0 <SizeOfVectorOfX0 ; iX0++){
     double vtemp = sqrt(
                           std::pow ( VectorOfTheX[iX0] , 2)
                         + std::pow ( VectorOfTheY[iX0] , 2)
                         + std::pow ( VectorOfTheZ[iX0] , 2) 
                        );
     OutCurrent 
        << std::setw(12)<<std::setprecision(5)<< VectorOfX0[iX0]
        << std::setw(12)<<std::setprecision(3)<< VectorOfTheX[iX0]
        << std::setw(12)<<std::setprecision(3)<< VectorOfTheY[iX0]
        << std::setw(12)<<std::setprecision(3)<< VectorOfTheZ[iX0]
        << std::setw(12)<<std::setprecision(3)<< vtemp
        << std::endl;
   }
 }

}

void MuonDynamicLayerCreatorTester::DumpTest2MboyCollectMatterTool(
                                      std::ofstream&  OutCurrent,
                                      double          Zconstant
){

 int    NberOfPoints   = m_NberOfPhiPoints ;
 double InDegThePhiMin = m_InDegThePhiMin  ; 
 double InDegThePhiMax = m_InDegThePhiMax  ; 
 
 double Mom       = m_MomentumTest ;
 double TheCharge = m_ChargeTest   ;
 
 OutCurrent 
      << "------------------------------------------------------"
      << std::endl;
 OutCurrent 
      << "-Format "
      << std::endl;
 OutCurrent 
      << "- first line: Z cte "
      << std::endl;
 OutCurrent 
      << "- Conditions line: Momentum Zconstant Phi SumOfX0 NberOfX0points "
      << std::endl;
 OutCurrent 
      << "                   followed by the list of NberOfX0points X0 at each of the points"
      << std::endl;
 OutCurrent 
      << " Zconstant " << Zconstant
      << std::endl;

 std::vector<std::pair<const Trk::TrackParameters*,const Trk::Layer* > > VectorOfPlaneLayer;
 Amg::Vector3D aGlobalPositionDummy(0.,0.,0.);
 double InDegThePhi    =  0. ;  
 double DeltaPhi = InDegThePhiMax - InDegThePhiMin      ;
 DeltaPhi = DeltaPhi / ( ( (double) NberOfPoints ) - 1. ) ;
 for (int iPhi=0; iPhi<NberOfPoints ; iPhi++){
   VectorOfPlaneLayer.clear();
   InDegThePhi = InDegThePhiMin + DeltaPhi * ( (double) iPhi ) ;
   Trk::AtaPlane* pAtaPlane = AtaPlaneRadProtectedTransAtZfixedAnglInDeg(
                                                             InDegThePhi    , 
                                                             Zconstant      , 
                                                             Mom            , 
                                                             TheCharge  
                                                            );
   StatusCode sc = 
   p_MboyCollectMatterTool->DoCollectMatter(
                                                             pAtaPlane,
                                                             &VectorOfPlaneLayer
                                                            );
   if ( sc.isFailure() ) OutCurrent  << " DoCollectMatter failed " << std::endl;
                                                            
   double thicknessInX0 = 0.;
   std::vector<double> VectorOfX0;
   std::vector<double> VectorOfTheX;
   std::vector<double> VectorOfTheY;
   std::vector<double> VectorOfTheZ;
   int SizeOfVectorOfPlaneLayer = VectorOfPlaneLayer.size();
   for (int iPlaneLayer= 0 ; iPlaneLayer <SizeOfVectorOfPlaneLayer ; iPlaneLayer++){
     const Trk::PlaneLayer *planelayer=dynamic_cast<const Trk::PlaneLayer *>(VectorOfPlaneLayer[iPlaneLayer].second);
     if (planelayer){
     double TheX0 =
     (
      ( planelayer->layerMaterialProperties() )
          ->fullMaterial(aGlobalPositionDummy) )
          ->thicknessInX0 ();
     thicknessInX0 = thicknessInX0 + TheX0 ;
     VectorOfX0.push_back(TheX0);          
               
     double TheX = 0. ;
     double TheY = 0. ;
     double TheZ = 0. ;
     TheX = (planelayer->center()).x();
     TheY = (planelayer->center()).y();
     TheZ = (planelayer->center()).z();
     VectorOfTheX.push_back(TheX);          
     VectorOfTheY.push_back(TheY);          
     VectorOfTheZ.push_back(TheZ);          

   }
   }

   int SizeOfVectorOfX0 = VectorOfX0.size();
   OutCurrent 
        << "Conditions "
        << std::setw(10)<<std::setprecision(1)<< Mom
        << std::setw(12)<<std::setprecision(5)<< Zconstant
        << std::setw(12)<<std::setprecision(5)<< InDegThePhi
        << std::setw(12)<<std::setprecision(2)<< thicknessInX0 
        << std::setw(12)                      << SizeOfVectorOfX0 
        << std::endl;
   for (int iX0= 0 ; iX0 <SizeOfVectorOfX0 ; iX0++){
     double vtemp = sqrt(
                           std::pow ( VectorOfTheX[iX0] , 2)
                         + std::pow ( VectorOfTheY[iX0] , 2)
                         + std::pow ( VectorOfTheZ[iX0] , 2) 
                        );
     OutCurrent 
        << std::setw(12)<<std::setprecision(5)<< VectorOfX0[iX0]
        << std::setw(12)<<std::setprecision(3)<< VectorOfTheX[iX0]
        << std::setw(12)<<std::setprecision(3)<< VectorOfTheY[iX0]
        << std::setw(12)<<std::setprecision(3)<< VectorOfTheZ[iX0]
        << std::setw(12)<<std::setprecision(3)<< vtemp
        << std::endl;
   }
 }

}

Trk::AtaPlane* MuonDynamicLayerCreatorTester::AtaPlaneOnMuSpectroSurfAnglInDeg(
                                                    double InDegThePhi    , 
                                                    double InDegTheTheta  , 
                                                    double Mom            , 
                                                    double TheCharge  
){
    double ThePhi   = InDegThePhi   * M_PI/180. ;
    double TheTheta = InDegTheTheta * M_PI/180. ; 
    
    return AtaPlaneOnMuSpectroSurf(
     ThePhi    , 
     TheTheta  , 
     Mom       , 
     TheCharge
     );

}
Trk::AtaPlane* MuonDynamicLayerCreatorTester::AtaPlaneOnMuSpectroSurf(
                                           double ThePhi    , 
                                           double TheTheta  , 
                                           double Mom       , 
                                           double TheCharge
){


// Do the GlobalPosition
    double Posx = fabs(m_MuSpectro_Radius) * cos( ThePhi )   ;
    double Posy = fabs(m_MuSpectro_Radius) * sin( ThePhi )   ;
    double Posz = fabs(m_MuSpectro_Radius) / tan( TheTheta ) ;
    if ( fabs(Posz) > fabs(m_MuSpectro_Zcyli) ) {
      if ( Posz > 0 ){
        Posz =  fabs(m_MuSpectro_Zcyli) ;
      }else{
        Posz = -fabs(m_MuSpectro_Zcyli) ;
      }
      Posx = Posz * tan( TheTheta ) * cos( ThePhi ) ;
      Posy = Posz * tan( TheTheta ) * sin( ThePhi ) ;
      
    }
    return AtaPlaneBase(
                        ThePhi    , 
                        TheTheta  , 
                        Mom       , 
                        TheCharge ,
                        Posx      ,
                        Posy      ,
                        Posz                                         
                       );
}
Trk::AtaPlane* MuonDynamicLayerCreatorTester::AtaPlaneOnIPAnglInDeg(
                                         double InDegThePhi    , 
                                         double InDegTheTheta  , 
                                         double Mom            , 
                                         double TheCharge  
){
    double ThePhi   = InDegThePhi   * M_PI/180. ;
    double TheTheta = InDegTheTheta * M_PI/180. ; 
    
    return AtaPlaneOnIP(
     ThePhi    , 
     TheTheta  , 
     Mom       , 
     TheCharge
     );

}
Trk::AtaPlane* MuonDynamicLayerCreatorTester::AtaPlaneOnIP(
                                double ThePhi    , 
                                double TheTheta  , 
                                double Mom       , 
                                double TheCharge
){


// Do the GlobalPosition
    double Posx = 0. ;
    double Posy = 0. ;
    double Posz = 0. ;
    return AtaPlaneBase(
                        ThePhi    , 
                        TheTheta  , 
                        Mom       , 
                        TheCharge ,
                        Posx      ,
                        Posy      ,
                        Posz                                         
                       );
}

Trk::AtaPlane* MuonDynamicLayerCreatorTester::AtaPlaneRadProtectedTransAtZfixedAnglInDeg(
                                             double InDegThePhi    , 
                                             double Zconstant      , 
                                             double Mom            , 
                                             double TheCharge  
){
    double ThePhi   = InDegThePhi   * M_PI/180. ;
    
    return AtaPlaneRadProtectedTransAtZfixed(
     ThePhi    , 
     Zconstant  , 
     Mom       , 
     TheCharge
     );

}
Trk::AtaPlane* MuonDynamicLayerCreatorTester::AtaPlaneRadProtectedTransAtZfixed(
                                    double ThePhi    , 
                                    double Zconstant  , 
                                    double Mom       , 
                                    double TheCharge
){

    double TheTheta = M_PI/2. ;

// Do the GlobalPosition
    double Posx = m_RadiusProtection * cos( ThePhi )   ;
    double Posy = m_RadiusProtection * sin( ThePhi )   ;
    double Posz = Zconstant ;
    return AtaPlaneBase(
                        ThePhi    , 
                        TheTheta  , 
                        Mom       , 
                        TheCharge ,
                        Posx      ,
                        Posy      ,
                        Posz                                         
                       );
}

Trk::AtaPlane* MuonDynamicLayerCreatorTester::AtaPlaneTransAtZfixedAnglInDeg(
                                             double InDegThePhi    , 
                                             double Zconstant      , 
                                             double Mom            , 
                                             double TheCharge  
){
    double ThePhi   = InDegThePhi   * M_PI/180. ;
    
    return AtaPlaneTransAtZfixed(
     ThePhi    , 
     Zconstant  , 
     Mom       , 
     TheCharge
     );

}
Trk::AtaPlane* MuonDynamicLayerCreatorTester::AtaPlaneTransAtZfixed(
                                    double ThePhi    , 
                                    double Zconstant  , 
                                    double Mom       , 
                                    double TheCharge
){

    double TheTheta = M_PI/2. ;

// Do the GlobalPosition
    double Posx = 0. ;
    double Posy = 0. ;
    double Posz = Zconstant ;
    return AtaPlaneBase(
                        ThePhi    , 
                        TheTheta  , 
                        Mom       , 
                        TheCharge ,
                        Posx      ,
                        Posy      ,
                        Posz                                         
                       );
}

Trk::AtaPlane* MuonDynamicLayerCreatorTester::AtaPlaneBase(
                                double ThePhi    , 
                                double TheTheta  , 
                                double Mom       , 
                                double TheCharge ,
                                double Posx      ,
                                double Posy      ,
                                double Posz                                         
){

// Do the GlobalMomentum
    double MomX = Mom * sin( TheTheta ) * cos ( ThePhi ) ;
    double MomY = Mom * sin( TheTheta ) * sin ( ThePhi ) ;
    double MomZ = Mom * cos( TheTheta )                  ;

    return AtaPlaneUltraBase(
                             TheCharge ,
                             Posx      ,
                             Posy      ,
                             Posz      ,                                  
                             MomX      ,
                             MomY      ,
                             MomZ                                         
                            );
                             
}

Trk::AtaPlane* MuonDynamicLayerCreatorTester::AtaPlaneUltraBase(
                                     double TheCharge ,
                                     double Posx      ,
                                     double Posy      ,
                                     double Posz      ,                                  
                                     double MomX      ,
                                     double MomY      ,
                                     double MomZ                                         
){


// Do the GlobalPosition
    Amg::Vector3D aGlobalPosition(
     Posx, 
     Posy, 
     Posz 
     );
    
// Do the GlobalMomentum
    Amg::Vector3D aGlobalMomentum(
     MomX, 
     MomY, 
     MomZ 
     );
    
// Do the PlaneSurface
    Amg::Translation3D PointOnSurf(
     Posx,
     Posy,
     Posz
     );
    Amg::Vector3D dZ(
     MomX, 
     MomY, 
     MomZ 
     );
    dZ.normalize();  
    Amg::Vector3D dMostPerp ( 0., 0. , 0.);
    double LowestSca = 100000.;
    double  ScaCur = 0. ;
    for (int Ico=0; Ico<3; Ico++){
      if (Ico == 0 ) ScaCur = fabs(dZ.x()) ;
      if (Ico == 1 ) ScaCur = fabs(dZ.y()) ;
      if (Ico == 2 ) ScaCur = fabs(dZ.z()) ;
      if ( ScaCur < LowestSca ){
        LowestSca = ScaCur ;
        dMostPerp.x() = 0.;
        dMostPerp.y() = 0.;
        dMostPerp.z() = 0.;
        if (Ico == 0 ) dMostPerp.x() = 1.;
        if (Ico == 1 ) dMostPerp.y() = 1.;
        if (Ico == 2 ) dMostPerp.z() = 1.;
      }      
    }
    Amg::Vector3D dX = dZ.cross(dMostPerp);
    dX.normalize();
    Amg::Vector3D dY = dZ.cross(dX);
    dY.normalize();
    Amg::RotationMatrix3D aHepRotation;
    aHepRotation.col(0) = dX;
    aHepRotation.col(1) = dY;
    aHepRotation.col(2) = dZ;
    Amg::Transform3D pTransform3D = PointOnSurf * Amg::RotationMatrix3D::Identity();
    pTransform3D *= aHepRotation;
    double surfaceDimX = 5000. ;
    double surfaceDimY = 5000. ;
    Trk::PlaneSurface aPlaneSurface(&pTransform3D, surfaceDimX, surfaceDimY);

    return GetSecuredAtaPlane (aGlobalPosition, aGlobalMomentum , TheCharge , aPlaneSurface);

}
Trk::AtaPlane* MuonDynamicLayerCreatorTester::GetSecuredAtaPlane(
     const Amg::Vector3D &      aGlobalPosition ,
     const Amg::Vector3D &      aGlobalMomentum ,
     double                           TheCharge       ,
     const Trk::PlaneSurface   &      aPlaneSurface
){

//  return ( new Trk::AtaPlane (aGlobalPosition, aGlobalMomentum , TheCharge , aPlaneSurface) );

    double locx = 0. ;
    double locy = 0. ;
    
    Amg::Vector2D* pLocalPosition = new Amg::Vector2D(0., 0.);
    aPlaneSurface.globalToLocal( aGlobalPosition, aGlobalPosition, *pLocalPosition );
    if (pLocalPosition) {
      locx = (*pLocalPosition)[Trk::locX] ;
      locy = (*pLocalPosition)[Trk::locY] ;
    }else{
      ATH_MSG_DEBUG(  " globalToLocal failed in  GetSecuredAtaPlane  " ) ;
      Amg::Vector3D aLocalPosition = aPlaneSurface.transform().inverse()*aGlobalPosition ;
      locx = aLocalPosition[Trk::locX] ;
      locy = aLocalPosition[Trk::locY] ;
    }
    delete pLocalPosition;
    double qoverp = TheCharge/fabs(aGlobalMomentum.mag());
    return ( new Trk::AtaPlane(locx,locy,aGlobalMomentum.phi(),aGlobalMomentum.theta(),qoverp, aPlaneSurface) );

}

void MuonDynamicLayerCreatorTester::GiveMomentumForFromStarting2Pointed(
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
){

// Do the GlobalMomentum
    double DistanceStartToEnd = sqrt(
                                       std::pow ( ( X_Pointed - X_Starting ) , 2)
                                     + std::pow ( ( Y_Pointed - Y_Starting ) , 2)
                                     + std::pow ( ( Z_Pointed - Z_Starting ) , 2) 
                                    );
    MomX = Mom * ( X_Pointed - X_Starting )/DistanceStartToEnd ;
    MomY = Mom * ( Y_Pointed - Y_Starting )/DistanceStartToEnd ;
    MomZ = Mom * ( Z_Pointed - Z_Starting )/DistanceStartToEnd ;

}
 
// Finalize
StatusCode MuonDynamicLayerCreatorTester::finalize(){return StatusCode::SUCCESS;}
