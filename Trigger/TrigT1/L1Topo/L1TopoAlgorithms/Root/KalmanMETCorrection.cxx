/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * KalmanMETCorrection.cpp
 * Created by Joerg Stelzer on 11/16/12.
 *
 * @brief algorithm calculates the KF correction per jet , get new XE and apply cut 
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/KalmanMETCorrection.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
#include "L1TopoSimulationUtils/KFLUT.h"
#include "L1TopoSimulationUtils/Kinematics.h"
// Bitwise implementation utils
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"
#include "L1TopoSimulationUtils/Trigo.h"
#include "L1TopoSimulationUtils/Hyperbolic.h"
//

REGISTER_ALG_TCS(KalmanMETCorrection)

TCS::KalmanMETCorrection::KalmanMETCorrection(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 9);
   defineParameter("NumResultBits", 6);
   defineParameter("MinET", 0);
   defineParameter("KFXE",0,0);
   defineParameter("KFXE",0,1);
   defineParameter("KFXE",0,2);
   defineParameter("KFXE",0,3);
   defineParameter("KFXE",0,4);
   defineParameter("KFXE",0,5);
   setNumberOutputBits(6);
}

TCS::KalmanMETCorrection::~KalmanMETCorrection(){}


TCS::StatusCode
TCS::KalmanMETCorrection::initialize() {
   
   p_NumberLeading2 = parameter("InputWidth").value();

   p_MinEt = parameter("MinET").value();

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_XE[i] = parameter("KFXE",i).value();

   }
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);  
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
    TRG_MSG_INFO("KFXE   : " << p_XE[i]);
   }
   TRG_MSG_INFO("MinET          : " << p_MinEt);

   TRG_MSG_INFO("number output : " << numberOutputBits());
   
   
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::KalmanMETCorrection::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                      const std::vector<TCS::TOBArray *> & output,
                      Decision & decision )

{

   if(input.size()!=2) {
      TCS_EXCEPTION("KalmanMETCorrection alg must have exactly two input list (jets and MET list), but got " << input.size());
   }


   const TCS::GenericTOB & met = (*input[0])[0];

   double  KFmet = 0;
   double KFmetphi = 0;
//   double summetx = met.Et()*cos(met.phiDouble());
//   double summety = met.Et()*sin(met.phiDouble());
   double summetx = met.Ex();
   double summety = met.Ey();
   double corrfactor = 0;
  
   TRG_MSG_DEBUG("metsumx " << summetx << " metsumy " << summety );
 
   KFLUT  LUTobj;

   // loop over  jets
   for( TOBArray::const_iterator tob = input[1]->begin(); 
           tob != input[1]->end() && distance( input[1]->begin(), tob) < p_NumberLeading2;
           ++tob) 
         {

       if( (*tob)->Et() <= p_MinEt ) continue; // E_T cut
       int ipt = LUTobj.getetbin((*tob)->Et());
       int jeta = LUTobj.getetabin(abs((*tob)->etaDouble()));   //etaDouble = 0.1 * eta

       corrfactor = LUTobj.getcorrKF(ipt,jeta);   

       // This part of the code has to be reviewed again for phase1 BW simulation
       // Algorithm crashing when jJet inputs are enabled in the menu
       float cosphi;
       float sinphi;
       if (isLegacyTopo()){
	 cosphi = TSU::Kinematics::calcCosLegacy((*tob)->phi());
	 sinphi = TSU::Kinematics::calcSinLegacy((*tob)->phi());
       }
       else {
	 cosphi = TSU::Kinematics::calcCos((*tob)->phi());
	 sinphi = TSU::Kinematics::calcSin((*tob)->phi());
       }

       summetx += (-1.)*(*tob)->Et()*cosphi*corrfactor ;
       summety += (-1.)*(*tob)->Et()*sinphi*corrfactor ;
            

        TRG_MSG_DEBUG("corr  " << corrfactor);
	TRG_MSG_DEBUG("metsumx " << summetx << " metsumy " << summety );

       corrfactor = 0;
  }
   
   
   KFmet = sqrt(summetx*summetx + summety*summety);

   if (KFmet > 0 ) KFmetphi=isLegacyTopo()?TSU::Trigo::atan2leg(summetx,summety):TSU::Trigo::atan2(summetx,summety);

   for(unsigned int i=0; i<numberOutputBits(); ++i) {

      bool accept = KFmet > p_XE[i];

      decision.setBit( i, accept );

      if(accept)
         output[i]->push_back( CompositeTOB( GenericTOB::createOnHeap( GenericTOB(KFmet,0,KFmetphi) ) ));
      
      TRG_MSG_DEBUG("Old met " << sqrt(met.Ex()*met.Ex()+met.Ey()*met.Ey()) << ". Comparing new MET (phi)" << KFmet << "(" << KFmetphi << ")" << " with cut " << p_XE[i] << ". " << (KFmet > p_XE[i]?"Pass":"Fail"));

   }


   return TCS::StatusCode::SUCCESS;

}

TCS::StatusCode
TCS::KalmanMETCorrection::process( const std::vector<TCS::TOBArray const *> & input,
                      const std::vector<TCS::TOBArray *> & output,
                      Decision & decision )

{

   if(input.size()!=2) {
      TCS_EXCEPTION("KalmanMETCorrection alg must have exactly two input list (jets and MET list), but got " << input.size());
   }


   const TCS::GenericTOB & met = (*input[0])[0];

   double  KFmet = 0;
   double KFmetphi = 0;
//   double summetx = met.Et()*cos(met.phiDouble());
//   double summety = met.Et()*sin(met.phiDouble());
   double summetx = met.Ex();
   double summety = met.Ey();
   double corrfactor = 0;
  
   TRG_MSG_DEBUG("metsumx " << summetx << " metsumy " << summety );
 
   KFLUT  LUTobj;

   // loop over  jets
   for( TOBArray::const_iterator tob = input[1]->begin(); 
           tob != input[1]->end() && distance( input[1]->begin(), tob) < p_NumberLeading2;
           ++tob) 
         {

       if( (*tob)->Et() <= p_MinEt ) continue; // E_T cut
       int ipt = LUTobj.getetbin((*tob)->Et());
       int jeta = LUTobj.getetabin(abs((*tob)->etaDouble()));

       corrfactor = LUTobj.getcorrKF(ipt,jeta);   
       summetx += (-1.)*(*tob)->Et()*cos((*tob)->phiDouble())*corrfactor ;
       summety += (-1.)*(*tob)->Et()*sin((*tob)->phiDouble())*corrfactor ;

        TRG_MSG_DEBUG("corr  " << corrfactor);
         TRG_MSG_DEBUG("metsumx " << summetx << " metsumy " << summety );

       corrfactor = 0;
  }
   
   
   KFmet = sqrt(summetx*summetx + summety*summety);
   if (KFmet > 0 ) KFmetphi= 10*atan2(summety,summetx);

   for(unsigned int i=0; i<numberOutputBits(); ++i) {

      bool accept = KFmet > p_XE[i];

      decision.setBit( i, accept );

      if(accept)
         output[i]->push_back( CompositeTOB( GenericTOB::createOnHeap( GenericTOB(KFmet,0,KFmetphi) ) ));
      
      TRG_MSG_DEBUG("Old met " << met << ". Comparing new MET (phi)" << KFmet << "(" << KFmetphi << ")" << " with cut " << p_XE[i] << ". " << (KFmet > p_XE[i]?"Pass":"Fail"));

   }


   return TCS::StatusCode::SUCCESS;
}
