/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyEDMTool/MuonboyRTMdtDriftCircleOnTrackCreator.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

MuonboyRTMdtDriftCircleOnTrackCreator::MuonboyRTMdtDriftCircleOnTrackCreator():MuonboyRTAbstract(){
 p_IMdtDriftCircleOnTrackCreator = 0 ;
 p_MuonRecDigiStoreAth = 0 ;
}
 
MuonboyRTMdtDriftCircleOnTrackCreator::~MuonboyRTMdtDriftCircleOnTrackCreator(){}
 
void MuonboyRTMdtDriftCircleOnTrackCreator::RfT(int& TheRimRTSvc,
                         int& Ident,
                         double&  XXXDCA ,double& YYYDCA,double& ZZZDCA,
                         double&  XXXDCd ,double& YYYDCd,double& ZZZDCd,
                         double&  XWiCen   ,double&   YWiCen  ,double&   ZWiCen ,
                         double&  XWidir   ,double&   YWidir  ,double&   ZWidir ,
                         double& RADIUS,
                         int& MboyMdtStatus){

  RADIUS =  9999.;

  XWiCen = 0. ;
  YWiCen = 0. ;
  ZWiCen = 0. ;                                 
  XWidir = 0. ;
  YWidir = 0. ;
  ZWidir = 0. ;                              

  MboyMdtStatus =  900 ;
  
  double GposX = XXXDCA*10. ;
  double GposY = YYYDCA*10. ;
  double GposZ = ZZZDCA*10. ;
  
//   double Vtemp= sqrt( XXXDCd*XXXDCd + YYYDCd*YYYDCd + ZZZDCd*ZZZDCd );
//   if ( Vtemp <= 0.99999 ){
//     std::cout << "TAGADA*XXX/YYY/ZZZDCd " 
//               << " " << XXXDCd  
//               << " " << YYYDCd  
//               << " " << ZZZDCd  
//               << " " << Vtemp
//               << std::endl ;
//   }
  double GmomX = XXXDCd * 1000000. ;
  double GmomY = YYYDCd * 1000000. ;
  double GmomZ = ZZZDCd * 1000000. ;

  int Irank = Ilinear2MDTrank[Ident];

  (HepPoint3DSet[Irank]).x() = GposX;
  (HepPoint3DSet[Irank]).y() = GposY;
  (HepPoint3DSet[Irank]).z() = GposZ;
  (HepVector3DSet[Irank]).x() = GmomX;
  (HepVector3DSet[Irank]).y() = GmomY;
  (HepVector3DSet[Irank]).z() = GmomZ;

  Muon::MdtDriftCircleOnTrack* pMdtDriftCircleOnTrack =
  const_cast<Muon::MdtDriftCircleOnTrack*>(
                                           p_IMdtDriftCircleOnTrackCreator->createRIO_OnTrack(
                                                                                              *pMdtPrepDataSet[Irank]   , 
                                                                                              HepPoint3DSet[Irank], 
                                                                                             &(HepVector3DSet[Irank]) 
                                                                                             )
                                          );
  if (pMdtDriftCircleOnTrack) {
  
    RADIUS =  fabs( pMdtDriftCircleOnTrack->driftRadius()/10. ) ;
    
    Muon::MdtDriftCircleStatus aMdtDriftCircleStatus = p_IMdtDriftCircleOnTrackCreator->driftCircleStatus( (*pMdtDriftCircleOnTrack) );
    if ( aMdtDriftCircleStatus == Muon::MdtStatusDriftTime ){
      MboyMdtStatus = 1 ;
    }else{
      if ( aMdtDriftCircleStatus == Muon::MdtStatusBeforeSpectrum ){
        MboyMdtStatus = 2 ;
      }else{
        if ( aMdtDriftCircleStatus == Muon::MdtStatusAfterSpectrum ){
          MboyMdtStatus = 3 ;
        }else{
          if ( aMdtDriftCircleStatus == Muon::MdtStatusUnDefined ){
            MboyMdtStatus = 999 ;
          }else{
             MboyMdtStatus = 0 ;
          }
        }
      }
    }

    if (TheRimRTSvc == 1){
      const Trk::StraightLineSurface* pStraightLineSurface 
       = dynamic_cast<const Trk::StraightLineSurface*> (&(pMdtDriftCircleOnTrack->associatedSurface()));
      if (!pStraightLineSurface) return;
      Amg::Vector3D  aGlobalPosition   = pStraightLineSurface->center() ;
      Amg::Vector3D  aGlobalDirection = ((pStraightLineSurface->transform()).rotation().col(2)).normalized() ;

      XWiCen = aGlobalPosition.x() /10. ;
      YWiCen = aGlobalPosition.y() /10. ;
      ZWiCen = aGlobalPosition.z() /10. ;                                 
      XWidir = aGlobalDirection.x()     ;
      YWidir = aGlobalDirection.y()     ;
      ZWidir = aGlobalDirection.z()     ;                              
    }

  }

  delete pMdtDriftCircleOnTrack;

}

void MuonboyRTMdtDriftCircleOnTrackCreator::TfTDC(int&  /*Ident*/,int& /*TDCcount*/,double& TimeFromTDC){

  TimeFromTDC = 0.;

}
 
void MuonboyRTMdtDriftCircleOnTrackCreator::SetLinks(){

  pMdtPrepDataSet.clear();
  Ilinear2MDTrank.clear();
  HepPoint3DSet.clear();
  HepVector3DSet.clear();

  int NberTot    = p_MuonRecDigiStoreAth->NberOfDigi();
  int NberTotMDT = p_MuonRecDigiStoreAth->NberOfDigiMDT();

  pMdtPrepDataSet.resize(NberTotMDT,0);
  Amg::Vector3D  aHepPoint3D;
  Amg::Vector3D  aHepVector3D;
  HepPoint3DSet.resize(NberTotMDT,aHepPoint3D);
  HepVector3DSet.resize(NberTotMDT,aHepVector3D);
  Ilinear2MDTrank.resize(NberTot,-1);

  const Muon::MdtPrepData* pMdtPrepData = 0 ;
  int KounterMDT = 0 ;
  for (int Ilinear=0; Ilinear<NberTot; Ilinear++){
    pMdtPrepData =  p_MuonRecDigiStoreAth->LiIdGetMdtPrepData(Ilinear);
    if ( pMdtPrepData != 0 ) {
      (pMdtPrepDataSet[KounterMDT])= pMdtPrepData;
      HepPoint3DSet[KounterMDT]=pMdtPrepData->globalPosition();
      Ilinear2MDTrank[Ilinear]=KounterMDT ;
      KounterMDT = KounterMDT + 1 ;
    }
  }

}
std::string  MuonboyRTMdtDriftCircleOnTrackCreator::PrintRsLine(int Ident, int WireSag, double AdMaxima){

  std::string ToBeReturned ="";
  std::stringstream Objectstringstream;
  Objectstringstream<< setiosflags(std::ios::fixed);

  const Muon::MdtPrepData* pMdtPrepData               = p_MuonRecDigiStoreAth->LiIdGetMdtPrepData(Ident);
  const MuonGM::MdtReadoutElement* pMdtReadoutElement = pMdtPrepData->detectorElement() ;
  const Amg::Vector3D ROpt                               = pMdtReadoutElement->ROPos(pMdtPrepData->identify()) ;
  const Amg::Vector3D TUpt                               = pMdtPrepData->globalPosition();
  const double TubeLength                             = pMdtReadoutElement->tubeLength(pMdtPrepData->identify()) ;
  Amg::Vector3D     TUdir (TUpt.x()-ROpt.x(),
                    TUpt.y()-ROpt.y(),
                    TUpt.z()-ROpt.z());
  TUdir.normalize();

  double dNstep = TubeLength/AdMaxima ;
  int NStep =(int) dNstep;
  NStep = NStep + 1 ;
  double Step = TubeLength/NStep ;

  Amg::Vector3D PosC ;
  int NStepTot = NStep + 1 ;
  double StepTotal = 0 ;
  
  double XXXDCA    = 0. ;
  double YYYDCA    = 0. ;
  double ZZZDCA    = 0. ;
  
  double xCoord_MostPerp = 1. ;
  double yCoord_MostPerp = 0. ;
  double zCoord_MostPerp = 0. ;
  double Vmin = fabs(TUdir.x()) ;
  if ( fabs(TUdir.y()) < Vmin ){
    xCoord_MostPerp = 0. ;
    yCoord_MostPerp = 1. ;
    zCoord_MostPerp = 0. ;
    Vmin = fabs(TUdir.y()) ;
  }
  if ( fabs(TUdir.z()) < Vmin ){
    xCoord_MostPerp = 0. ;
    yCoord_MostPerp = 0. ;
    zCoord_MostPerp = 1. ;
    Vmin = fabs(TUdir.z()) ;
  }
  Amg::Vector3D aHep3Vector_MostPerp(xCoord_MostPerp, yCoord_MostPerp, zCoord_MostPerp );
  Amg::Vector3D Perp_TUdir = TUdir.cross(aHep3Vector_MostPerp);
  Perp_TUdir.normalize();
  double XXXDCd = Perp_TUdir.x() ;
  double YYYDCd = Perp_TUdir.y() ;
  double ZZZDCd = Perp_TUdir.z() ;
    
  double XWiCen = 0. ;
  double YWiCen = 0. ;
  double ZWiCen = 0. ;
  double XWidir = 0. ;
  double YWidir = 0. ;
  double ZWidir = 0. ;
  double RADIUS=  0. ;
  int    MboyMdtStatus=  999 ;

  int TheRimRTSvc = 1 ;
  
  Objectstringstream
    << "RS3 "
    << " " <<std::setw(6)<<Ident  
    << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.x() 
    << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.y() 
    << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.z() 
    << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.x()
    << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.y()
    << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.z() 
    << " " <<std::setw(10)<< NStep 
    << " " <<std::setw(10)<<std::setprecision(3)<< Step ;
  
  int NStepMedium = NStepTot/2 ;
  int MboyMdtStatus_Keep = -1 ;
  for (int IStep=0; IStep<NStepTot; IStep++){
    StepTotal = IStep * Step;
    PosC = ROpt + StepTotal * TUdir ;
    XXXDCA  = PosC.x()   / 10. ;
    YYYDCA  = PosC.y()   / 10. ;
    ZZZDCA  = PosC.z()   / 10. ;
//    std::cout << "TAGADA=>>>From MuonboyRTMdtDriftCircleOnTrackCreator::PrintRsLine" <<std::endl ;
    RfT(TheRimRTSvc,Ident,
        XXXDCA, YYYDCA, ZZZDCA,
        XXXDCd, YYYDCd, ZZZDCd,
        XWiCen, YWiCen, ZWiCen,
        XWidir, YWidir, ZWidir,
        RADIUS,
        MboyMdtStatus);
    Objectstringstream
      << " " <<std::setw(10)<<std::setprecision(3)<< RADIUS*10. ;
    if ( IStep < NStepMedium ) MboyMdtStatus_Keep = MboyMdtStatus ;
  }  
    Objectstringstream
      << " " <<std::setw(10)<<std::setprecision(3)<< MboyMdtStatus_Keep ;

  if( WireSag != 0 ) {
    Objectstringstream
      << std::endl ;
    Objectstringstream
      << "SXP "
      << " " <<std::setw(6)<<Ident  
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.x() 
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.y() 
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.z() 
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.x()
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.y()
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.z() 
      << " " <<std::setw(10)<< NStep 
      << " " <<std::setw(10)<<std::setprecision(3)<< Step ;
    for (int IStep=0; IStep<NStepTot; IStep++){
      StepTotal = IStep * Step;
      PosC = ROpt + StepTotal * TUdir ;
      XXXDCA  = PosC.x()   / 10. ;
      YYYDCA  = PosC.y()   / 10. ;
      ZZZDCA  = PosC.z()   / 10. ;
      RfT(TheRimRTSvc,Ident,
          XXXDCA, YYYDCA, ZZZDCA,
          XXXDCd, YYYDCd, ZZZDCd,
          XWiCen, YWiCen, ZWiCen,
          XWidir, YWidir, ZWidir,
          RADIUS,
          MboyMdtStatus);
      Objectstringstream
        << " " <<std::setw(10)<<std::setprecision(3)<< XWiCen*10. ;
    }

    Objectstringstream
        << std::endl ;
    Objectstringstream
      << "SYP "
      << " " <<std::setw(6)<<Ident  
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.x() 
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.y() 
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.z() 
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.x()
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.y()
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.z() 
      << " " <<std::setw(10)<< NStep 
      << " " <<std::setw(10)<<std::setprecision(3)<< Step ;
    for (int IStep=0; IStep<NStepTot; IStep++){
      StepTotal = IStep * Step;
      PosC = ROpt + StepTotal * TUdir ;
      XXXDCA  = PosC.x()   / 10. ;
      YYYDCA  = PosC.y()   / 10. ;
      ZZZDCA  = PosC.z()   / 10. ;
      RfT(TheRimRTSvc,Ident,
          XXXDCA, YYYDCA, ZZZDCA,
          XXXDCd, YYYDCd, ZZZDCd,
          XWiCen, YWiCen, ZWiCen,
          XWidir, YWidir, ZWidir,
          RADIUS,
          MboyMdtStatus);
      Objectstringstream
        << " " <<std::setw(10)<<std::setprecision(3)<< YWiCen*10. ;
    }

    Objectstringstream
        << std::endl ;
    Objectstringstream
      << "SZP "
      << " " <<std::setw(6)<<Ident  
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.x() 
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.y() 
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.z() 
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.x()
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.y()
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.z() 
      << " " <<std::setw(10)<< NStep 
      << " " <<std::setw(10)<<std::setprecision(3)<< Step ;
    for (int IStep=0; IStep<NStepTot; IStep++){
      StepTotal = IStep * Step;
      PosC = ROpt + StepTotal * TUdir ;
      XXXDCA  = PosC.x()   / 10. ;
      YYYDCA  = PosC.y()   / 10. ;
      ZZZDCA  = PosC.z()   / 10. ;
      RfT(TheRimRTSvc,Ident,
          XXXDCA, YYYDCA, ZZZDCA,
          XXXDCd, YYYDCd, ZZZDCd,
          XWiCen, YWiCen, ZWiCen,
          XWidir, YWidir, ZWidir,
          RADIUS,
          MboyMdtStatus);
      Objectstringstream
        << " " <<std::setw(10)<<std::setprecision(3)<< ZWiCen*10. ;
    }

    Objectstringstream
        << std::endl ;
    Objectstringstream
      << "SXD "
      << " " <<std::setw(6)<<Ident  
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.x() 
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.y() 
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.z() 
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.x()
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.y()
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.z() 
      << " " <<std::setw(10)<< NStep 
      << " " <<std::setw(10)<<std::setprecision(3)<< Step ;
    for (int IStep=0; IStep<NStepTot; IStep++){
      StepTotal = IStep * Step;
      PosC = ROpt + StepTotal * TUdir ;
      XXXDCA  = PosC.x()   / 10. ;
      YYYDCA  = PosC.y()   / 10. ;
      ZZZDCA  = PosC.z()   / 10. ;
      RfT(TheRimRTSvc,Ident,
          XXXDCA, YYYDCA, ZZZDCA,
          XXXDCd, YYYDCd, ZZZDCd,
          XWiCen, YWiCen, ZWiCen,
          XWidir, YWidir, ZWidir,
          RADIUS,
          MboyMdtStatus);
      Objectstringstream
        << " " <<std::setw(10)<<std::setprecision(7)<< XWidir;
    }

    Objectstringstream
        << std::endl ;
    Objectstringstream
      << "SYD "
      << " " <<std::setw(6)<<Ident  
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.x() 
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.y() 
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.z() 
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.x()
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.y()
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.z() 
      << " " <<std::setw(10)<< NStep 
      << " " <<std::setw(10)<<std::setprecision(3)<< Step ;
    for (int IStep=0; IStep<NStepTot; IStep++){
      StepTotal = IStep * Step;
      PosC = ROpt + StepTotal * TUdir ;
      XXXDCA  = PosC.x()   / 10. ;
      YYYDCA  = PosC.y()   / 10. ;
      ZZZDCA  = PosC.z()   / 10. ;
      RfT(TheRimRTSvc,Ident,
          XXXDCA, YYYDCA, ZZZDCA,
          XXXDCd, YYYDCd, ZZZDCd,
          XWiCen, YWiCen, ZWiCen,
          XWidir, YWidir, ZWidir,
          RADIUS,
          MboyMdtStatus);
      Objectstringstream
        << " " <<std::setw(10)<<std::setprecision(7)<< YWidir;
    }

    Objectstringstream
        << std::endl ;
    Objectstringstream
      << "SZD "
      << " " <<std::setw(6)<<Ident  
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.x() 
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.y() 
      << " " <<std::setw(10)<<std::setprecision(3)<< ROpt.z() 
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.x()
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.y()
      << " " <<std::setw(10)<<std::setprecision(7)<< TUdir.z() 
      << " " <<std::setw(10)<< NStep 
      << " " <<std::setw(10)<<std::setprecision(3)<< Step ;
    for (int IStep=0; IStep<NStepTot; IStep++){
      StepTotal = IStep * Step;
      PosC = ROpt + StepTotal * TUdir ;
      XXXDCA  = PosC.x()   / 10. ;
      YYYDCA  = PosC.y()   / 10. ;
      ZZZDCA  = PosC.z()   / 10. ;
      RfT(TheRimRTSvc,Ident,
          XXXDCA, YYYDCA, ZZZDCA,
          XXXDCd, YYYDCd, ZZZDCd,
          XWiCen, YWiCen, ZWiCen,
          XWidir, YWidir, ZWidir,
          RADIUS,
          MboyMdtStatus);
      Objectstringstream
        << " " <<std::setw(10)<<std::setprecision(7)<< ZWidir;
    }
  }

  ToBeReturned=Objectstringstream.str();
  return ToBeReturned;

}
