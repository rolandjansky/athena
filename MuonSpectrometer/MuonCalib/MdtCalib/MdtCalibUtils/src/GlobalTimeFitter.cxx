/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibUtils/GlobalTimeFitter.h"

#include <string>
#include <sstream>

namespace MuonCalib {
  
double GlobalTimeFitter::GTFit( MuonCalibSegment * seg ) {

  double timeShift ;
  float tdcmin=1000000;
  double timeoffset=0.;
  int stepf,stepff[13],stepl,stepr,dtt,dttbuono(0);
  double chi2r,chi2l;
  double chi2min  = 100000;
  double bestchi2 = 100000;

   stepff[0]=512;stepff[1]=256;stepff[2]=128;stepff[3]=64;stepff[4]=32;
   stepff[5]=16;stepff[6]=8;stepff[7]=4;stepff[8]=2;stepff[9]=1 ;
   stepff[10]=1;stepff[11]=1;stepff[12]=1;



  for (std::vector<MdtCalibHitBase*>::iterator it = seg->mdtHOTBegin() ;
              it != seg->mdtHOTEnd(); ++it) {
      if ((*it)->tdcCount()<tdcmin) tdcmin = (*it)->tdcCount();
  }

  stepl=512 ; stepr=512 ; stepf=512;
  int outofrange  ;
  for (int jj=0;jj<11;jj++) {
      outofrange = 0 ;
      for(std::vector<MdtCalibHitBase*>::iterator it = seg->mdtHOTBegin() ;
         it != seg->mdtHOTEnd(); ++it) {
         double newtime = (double)((*it)->tdcCount()-tdcmin+stepl)*25./32.;
      
         if ( newtime<0. || newtime>750.) outofrange = 1 ;
         (*it)->setDriftTime(newtime);
         double r=m_rtRel->radius(newtime);
         double dr=getDefaultResolution(r); //  get H8 resolution
         (*it)->setDriftRadius(r,dr);
       }
       if (!outofrange) {
          m_fitter->fit(*seg);
          chi2l = seg->chi2() ;
       } else {
          chi2l = 100000. ;
       }
       outofrange = 0 ;
      for(std::vector<MdtCalibHitBase*>::iterator it = seg->mdtHOTBegin() ;
         it != seg->mdtHOTEnd(); ++it) {
         double newtime = (double)((*it)->tdcCount()-tdcmin+stepr)*25./32.;
      
         if ( newtime<0. || newtime>750.) outofrange = 1 ;
         (*it)->setDriftTime(newtime);
         double r=m_rtRel->radius(newtime);
         double dr=getDefaultResolution(r); //  get H8 resolution
         (*it)->setDriftRadius(r,dr);
       }
       if (!outofrange) {
          m_fitter->fit(*seg);
          chi2r = seg->chi2() ;
       } else {
          chi2r = 100000. ;
       }
       chi2min = chi2l ;
       stepf = stepl ;
       dtt   = stepl;
       if(chi2r<chi2l) {
          chi2min = chi2r ;
          stepf = stepr ;
          dtt   = stepr;
       }
       if (chi2min<bestchi2) {
          bestchi2 = chi2min ;
          dttbuono     = dtt;
           stepl = stepf-stepff[jj];
           stepr = stepff[jj]+stepf;
           if(stepl<0)stepl=0;
           if(stepr<0)stepr=0;
       } else {
           stepl = stepf-stepff[jj];
           stepr = stepff[jj]+stepf;
           if(stepl<0)stepl=0;
           if(stepr<0)stepr=0;
       }
       timeoffset = dttbuono;
   }

   for(std::vector<MdtCalibHitBase*>::iterator it = seg->mdtHOTBegin() ;
         it != seg->mdtHOTEnd(); ++it) {
      double newtime = (double)((*it)->tdcCount()-tdcmin+timeoffset)*25./32.;
      (*it)->setDriftTime(newtime);
      double r=m_rtRel->radius(newtime);
      double dr=getDefaultResolution(r); //  get H8 resolution
      (*it)->setDriftRadius(r,dr);
   }
   m_fitter->fit(*seg);

   timeShift = -(timeoffset-tdcmin)*25./32. ;
   return timeShift ;
}

double GlobalTimeFitter::GTFit2( MuonCalibSegment * seg ) {

  double timeShift = 999.;
  for (std::vector<MdtCalibHitBase*>::iterator it = seg->mdtHOTBegin() ;
              it != seg->mdtHOTEnd(); ++it) {
      (*it)->setDriftTime(400.);
      double t=(*it)->driftTime();
      double r=m_rtRel->radius(t);
      double dr=(*it)->sigmaDriftRadius();
      (*it)->setDriftRadius(r,dr);

      timeShift = t ;
   }
  
  return timeShift ;
}

IRtRelation * GlobalTimeFitter::getDefaultRtRelation() {
   MuonCalib::CalibFunc::ParVec rtPars;
   rtPars.push_back( -50. );   // t_Start 
   rtPars.push_back( 8. );     // t_binSize

   rtPars.push_back(0.2) ;        // r(-50)
   rtPars.push_back(0.2) ;          // r(-50+8)
   rtPars.push_back(0.2) ;          // ...
   rtPars.push_back(0.25) ;
   rtPars.push_back(0.294403) ;
   rtPars.push_back(0.311703) ;
   rtPars.push_back(0.485873) ;
   rtPars.push_back(0.804235) ;
   rtPars.push_back(1.19624) ;
   rtPars.push_back(1.5562) ;
   rtPars.push_back(1.91466) ;
   rtPars.push_back(2.3147) ;
   rtPars.push_back(2.66996) ;
   rtPars.push_back(3.05436) ;
   rtPars.push_back(3.40371) ;
   rtPars.push_back(3.80574) ;
   rtPars.push_back(4.13506) ;
   rtPars.push_back(4.45473) ;
   rtPars.push_back(4.77338) ;
   rtPars.push_back(5.05294) ;
   rtPars.push_back(5.33755) ;
   rtPars.push_back(5.58459) ;
   rtPars.push_back(5.8718) ;
   rtPars.push_back(6.09389) ;
   rtPars.push_back(6.37129) ;
   rtPars.push_back(6.56824) ;
   rtPars.push_back(6.73359) ;
   rtPars.push_back(6.96923) ;
   rtPars.push_back(7.16944) ;
   rtPars.push_back(7.38872) ;
   rtPars.push_back(7.55627) ;
   rtPars.push_back(7.73885) ;
   rtPars.push_back(7.86403) ;
   rtPars.push_back(8.07223) ;
   rtPars.push_back(8.26818) ;
   rtPars.push_back(8.39531) ;
   rtPars.push_back(8.54069) ;
   rtPars.push_back(8.69377) ;
   rtPars.push_back(8.86487) ;
   rtPars.push_back(9.01724) ;
   rtPars.push_back(9.16516) ;
   rtPars.push_back(9.24791) ;
   rtPars.push_back(9.45596) ;
   rtPars.push_back(9.56237) ;
   rtPars.push_back(9.69553) ;
   rtPars.push_back(9.80647) ;
   rtPars.push_back(9.9432) ;
   rtPars.push_back(10.039) ;
   rtPars.push_back(10.207) ;
   rtPars.push_back(10.2884) ;
   rtPars.push_back(10.3977) ;
   rtPars.push_back(10.6045) ;
   rtPars.push_back(10.6181) ;
   rtPars.push_back(10.7883) ;
   rtPars.push_back(10.9057) ;
   rtPars.push_back(10.9908) ;
   rtPars.push_back(11.1615) ;
   rtPars.push_back(11.2359) ;
   rtPars.push_back(11.3409) ;
   rtPars.push_back(11.4451) ;
   rtPars.push_back(11.5412) ;
   rtPars.push_back(11.6896) ;
   rtPars.push_back(11.7487) ;
   rtPars.push_back(11.8853) ;
   rtPars.push_back(11.9571) ;
   rtPars.push_back(12.0381) ;
   rtPars.push_back(12.1739) ;
   rtPars.push_back(12.2601) ;
   rtPars.push_back(12.3892) ;
   rtPars.push_back(12.4406) ;
   rtPars.push_back(12.5809) ;
   rtPars.push_back(12.6297) ;
   rtPars.push_back(12.7495) ;
   rtPars.push_back(12.8314) ;
   rtPars.push_back(12.9463) ;
   rtPars.push_back(13.0487) ;
   rtPars.push_back(13.1337) ;
   rtPars.push_back(13.2417) ;
   rtPars.push_back(13.2706) ;
   rtPars.push_back(13.3917) ;
   rtPars.push_back(13.5107) ;
   rtPars.push_back(13.60) ;
   rtPars.push_back(13.69) ;
   rtPars.push_back(13.71) ;
   rtPars.push_back(13.8088) ;
   rtPars.push_back(13.8577) ;
   rtPars.push_back(14.0212) ;
   rtPars.push_back(14.0671) ;
   rtPars.push_back(14.1225) ;
   rtPars.push_back(14.2112) ;
   rtPars.push_back(14.3113) ;
   rtPars.push_back(14.3747) ;
   rtPars.push_back(14.3531) ;
   rtPars.push_back(14.4799) ;
   rtPars.push_back(14.458) ;
   rtPars.push_back(14.516) ;
   rtPars.push_back(14.8312) ;
   rtPars.push_back(14.85) ;
   rtPars.push_back(14.86) ;
   rtPars.push_back(14.87) ;

   IRtRelation * rt = MuonCalib::MdtCalibrationFactory::createRtRelation( "RtRelationLookUp", rtPars );

   return rt ;

}





} // End namespace

