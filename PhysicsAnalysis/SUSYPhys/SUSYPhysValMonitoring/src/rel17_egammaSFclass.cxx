/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Efficiency scale factors macro
//Date: 11/01/2010
//Author: Olivier Arnaez <olivier.arnaez@cern.ch>
//
//Returns the combined (W/Z) scale factor and uncertainty vs eta and ET (in MeV) for the egamma
//standard sets of cuts Loose (set=0), Medium (set=1), Tight (set=2)
//in release 15 (rel=0) / release 16 (rel=1) measured with 
//probes in the 20-50 GeV range (range=0) or 30-50 GeV (range=1) and corrected 
//(etcorrection=1) for the ET-dependence
//

#include "SUSYPhysValMonitoring/rel17_egammaSFclass.h"
#include <cmath>

egammaSFclass::egammaSFclass()
{
  //Definition of the eta binning
  m_nEtabins=10;
  m_Etabins.push_back(-2.47);
  m_Etabins.push_back(-2.01); 
  m_Etabins.push_back(-1.52); 
  m_Etabins.push_back(-1.37); 
  m_Etabins.push_back(-0.8); 
  m_Etabins.push_back(0); 
  m_Etabins.push_back(0.8); 
  m_Etabins.push_back(1.37); 
  m_Etabins.push_back(1.52); 
  m_Etabins.push_back(2.01); 
  m_Etabins.push_back(2.47);
  //Definition of the ET binning
  m_nETbins=10;
  m_ETbins.push_back(0.);
  m_ETbins.push_back(20000.); 
  m_ETbins.push_back(25000.); 
  m_ETbins.push_back(30000.); 
  m_ETbins.push_back(35000.); 
  m_ETbins.push_back(40000.); 
  m_ETbins.push_back(45000.); 
  m_ETbins.push_back(500000000.); 


  //For the scale factors of the standard egamma cuts 

  //Release 15
  //Probes between 30 and 50 GeV (plateau region)
  //Loose
  efficienciesRel15Loose3050.push_back(98.1); 
  efficienciesRel15Loose3050.push_back(99.0); 
  efficienciesRel15Loose3050.push_back(0.); 
  efficienciesRel15Loose3050.push_back(98.6); 
  efficienciesRel15Loose3050.push_back(99.5); 
  efficienciesRel15Loose3050.push_back(99.1); 
  efficienciesRel15Loose3050.push_back(98.8); 
  efficienciesRel15Loose3050.push_back(0.); 
  efficienciesRel15Loose3050.push_back(99.9); 
  efficienciesRel15Loose3050.push_back(98.2);
  uncertaintiesRel15Loose3050.push_back(1.6); 
  uncertaintiesRel15Loose3050.push_back( 1.5); 
  uncertaintiesRel15Loose3050.push_back(0.); 
  uncertaintiesRel15Loose3050.push_back( 1.5); 
  uncertaintiesRel15Loose3050.push_back( 1.5); 
  uncertaintiesRel15Loose3050.push_back( 1.5); 
  uncertaintiesRel15Loose3050.push_back( 1.5); 
  uncertaintiesRel15Loose3050.push_back(0.); 
  uncertaintiesRel15Loose3050.push_back( 1.5); 
  uncertaintiesRel15Loose3050.push_back( 1.6);
  //Medium
  efficienciesRel15Medium3050.push_back(95.4); 
  efficienciesRel15Medium3050.push_back(98.7);
  efficienciesRel15Medium3050.push_back(0.); 
  efficienciesRel15Medium3050.push_back(97.9);
  efficienciesRel15Medium3050.push_back(98.1);
  efficienciesRel15Medium3050.push_back(97.7); 
  efficienciesRel15Medium3050.push_back(97.9); 
  efficienciesRel15Medium3050.push_back(0.); 
  efficienciesRel15Medium3050.push_back(99.9); 
  efficienciesRel15Medium3050.push_back(97.4);
  uncertaintiesRel15Medium3050.push_back(1.7);
  uncertaintiesRel15Medium3050.push_back( 1.6);
  uncertaintiesRel15Medium3050.push_back(0.); 
  uncertaintiesRel15Medium3050.push_back( 1.6);
  uncertaintiesRel15Medium3050.push_back( 1.5); 
  uncertaintiesRel15Medium3050.push_back( 1.5); 
  uncertaintiesRel15Medium3050.push_back( 1.5); 
  uncertaintiesRel15Medium3050.push_back(0.); 
  uncertaintiesRel15Medium3050.push_back( 1.6);
  uncertaintiesRel15Medium3050.push_back( 1.7);
  //Tight
  efficienciesRel15Tight3050.push_back(92.3); 
  efficienciesRel15Tight3050.push_back(99.2); 
  efficienciesRel15Tight3050.push_back(0.);
  efficienciesRel15Tight3050.push_back(101.5); 
  efficienciesRel15Tight3050.push_back(98.9); 
  efficienciesRel15Tight3050.push_back(99.9);
  efficienciesRel15Tight3050.push_back(104.2); 
  efficienciesRel15Tight3050.push_back(0.);
  efficienciesRel15Tight3050.push_back(102.6); 
  efficienciesRel15Tight3050.push_back(95.5);
  uncertaintiesRel15Tight3050.push_back(3.3);
  uncertaintiesRel15Tight3050.push_back( 2.3); 
  uncertaintiesRel15Tight3050.push_back(0.);
  uncertaintiesRel15Tight3050.push_back( 2.0); 
  uncertaintiesRel15Tight3050.push_back( 1.8); 
  uncertaintiesRel15Tight3050.push_back( 1.8);
  uncertaintiesRel15Tight3050.push_back( 2.5); 
  uncertaintiesRel15Tight3050.push_back(0.); 
  uncertaintiesRel15Tight3050.push_back( 5.0); 
  uncertaintiesRel15Tight3050.push_back( 3.2);

  //Probes between 20 and 50 GeV
  //Loose
  efficienciesRel15Loose2050.push_back(97.6); 
  efficienciesRel15Loose2050.push_back(99.0); 
  efficienciesRel15Loose2050.push_back(0.); 
  efficienciesRel15Loose2050.push_back(98.2); 
  efficienciesRel15Loose2050.push_back(99.1); 
  efficienciesRel15Loose2050.push_back(98.8); 
  efficienciesRel15Loose2050.push_back(98.2); 
  efficienciesRel15Loose2050.push_back(0.); 
  efficienciesRel15Loose2050.push_back(99.6); 
  efficienciesRel15Loose2050.push_back(97.4);
  uncertaintiesRel15Loose2050.push_back(1.6); 
  uncertaintiesRel15Loose2050.push_back(1.5); 
  uncertaintiesRel15Loose2050.push_back(0.); 
  uncertaintiesRel15Loose2050.push_back( 1.5); 
  uncertaintiesRel15Loose2050.push_back( 1.5); 
  uncertaintiesRel15Loose2050.push_back( 1.5); 
  uncertaintiesRel15Loose2050.push_back( 1.5); 
  uncertaintiesRel15Loose2050.push_back(0.); 
  uncertaintiesRel15Loose2050.push_back( 1.5); 
  uncertaintiesRel15Loose2050.push_back( 1.6);
  //Medium
  efficienciesRel15Medium2050.push_back(94.5); 
  efficienciesRel15Medium2050.push_back(98.8);
  efficienciesRel15Medium2050.push_back(0.); 
  efficienciesRel15Medium2050.push_back(97.2);
  efficienciesRel15Medium2050.push_back(97.4);
  efficienciesRel15Medium2050.push_back(97.2); 
  efficienciesRel15Medium2050.push_back(96.7); 
  efficienciesRel15Medium2050.push_back(0.); 
  efficienciesRel15Medium2050.push_back(99.5); 
  efficienciesRel15Medium2050.push_back(96.1);
  uncertaintiesRel15Medium2050.push_back(1.7);
  uncertaintiesRel15Medium2050.push_back( 1.6);
  uncertaintiesRel15Medium2050.push_back(0.); 
  uncertaintiesRel15Medium2050.push_back( 1.6);
  uncertaintiesRel15Medium2050.push_back( 1.5); 
  uncertaintiesRel15Medium2050.push_back( 1.5); 
  uncertaintiesRel15Medium2050.push_back( 1.5); 
  uncertaintiesRel15Medium2050.push_back(0.); 
  uncertaintiesRel15Medium2050.push_back( 2.9);
  uncertaintiesRel15Medium2050.push_back( 1.7);
  //Tight
  efficienciesRel15Tight2050.push_back(92.5); 
  efficienciesRel15Tight2050.push_back(99.5); 
  efficienciesRel15Tight2050.push_back(0.);
  efficienciesRel15Tight2050.push_back(100.6); 
  efficienciesRel15Tight2050.push_back(98.2); 
  efficienciesRel15Tight2050.push_back(98.7);
  efficienciesRel15Tight2050.push_back(103.3); 
  efficienciesRel15Tight2050.push_back(0.);
  efficienciesRel15Tight2050.push_back(102.8); 
  efficienciesRel15Tight2050.push_back(93.6);
  uncertaintiesRel15Tight2050.push_back(3.4);
  uncertaintiesRel15Tight2050.push_back( 2.4); 
  uncertaintiesRel15Tight2050.push_back(0.);
  uncertaintiesRel15Tight2050.push_back( 2.1); 
  uncertaintiesRel15Tight2050.push_back( 1.8); 
  uncertaintiesRel15Tight2050.push_back( 1.8);
  uncertaintiesRel15Tight2050.push_back( 2.5); 
  uncertaintiesRel15Tight2050.push_back(0.); 
  uncertaintiesRel15Tight2050.push_back( 4.5); 
  uncertaintiesRel15Tight2050.push_back( 3.4);


  //Release 16
  //Probes between 30 and 50 GeV (plateau region)
  //Medium
  efficienciesRel16Medium3050.push_back(98.3); 
  efficienciesRel16Medium3050.push_back(98.1);
  efficienciesRel16Medium3050.push_back(0.); 
  efficienciesRel16Medium3050.push_back(97.9);
  efficienciesRel16Medium3050.push_back(97.6);
  efficienciesRel16Medium3050.push_back(97.9); 
  efficienciesRel16Medium3050.push_back(98.0); 
  efficienciesRel16Medium3050.push_back(0.); 
  efficienciesRel16Medium3050.push_back(98.7); 
  efficienciesRel16Medium3050.push_back(97.9);
  uncertaintiesRel16Medium3050.push_back(0.8);
  uncertaintiesRel16Medium3050.push_back(0.8);
  uncertaintiesRel16Medium3050.push_back(0.); 
  uncertaintiesRel16Medium3050.push_back(0.7);
  uncertaintiesRel16Medium3050.push_back(0.6); 
  uncertaintiesRel16Medium3050.push_back(0.6); 
  uncertaintiesRel16Medium3050.push_back(0.8); 
  uncertaintiesRel16Medium3050.push_back(0.); 
  uncertaintiesRel16Medium3050.push_back(0.7);
  uncertaintiesRel16Medium3050.push_back(0.9);
  //Tight
  efficienciesRel16Tight3050.push_back(101.2); 
  efficienciesRel16Tight3050.push_back(103.5); 
  efficienciesRel16Tight3050.push_back(0.);
  efficienciesRel16Tight3050.push_back(106.6); 
  efficienciesRel16Tight3050.push_back( 99.5); 
  efficienciesRel16Tight3050.push_back(100.0);
  efficienciesRel16Tight3050.push_back(105.7); 
  efficienciesRel16Tight3050.push_back(0.);
  efficienciesRel16Tight3050.push_back(104.3); 
  efficienciesRel16Tight3050.push_back(102.7);
  uncertaintiesRel16Tight3050.push_back(2.6);
  uncertaintiesRel16Tight3050.push_back(1.6); 
  uncertaintiesRel16Tight3050.push_back(0.);
  uncertaintiesRel16Tight3050.push_back(1.1); 
  uncertaintiesRel16Tight3050.push_back(0.7); 
  uncertaintiesRel16Tight3050.push_back(0.7);
  uncertaintiesRel16Tight3050.push_back(1.6); 
  uncertaintiesRel16Tight3050.push_back(0.); 
  uncertaintiesRel16Tight3050.push_back(2.8); 
  uncertaintiesRel16Tight3050.push_back(2.0);

  //Probes between 20 and 50 GeV
  //Medium
  efficienciesRel16Medium2050.push_back(97.3); 
  efficienciesRel16Medium2050.push_back(97.2);
  efficienciesRel16Medium2050.push_back(0.); 
  efficienciesRel16Medium2050.push_back(97.6);
  efficienciesRel16Medium2050.push_back(97.1);
  efficienciesRel16Medium2050.push_back(97.8); 
  efficienciesRel16Medium2050.push_back(97.6); 
  efficienciesRel16Medium2050.push_back(0.); 
  efficienciesRel16Medium2050.push_back(98.6); 
  efficienciesRel16Medium2050.push_back(97.4);
  uncertaintiesRel16Medium2050.push_back(1.0);
  uncertaintiesRel16Medium2050.push_back(1.0);
  uncertaintiesRel16Medium2050.push_back(0.); 
  uncertaintiesRel16Medium2050.push_back(1.1);
  uncertaintiesRel16Medium2050.push_back(0.7); 
  uncertaintiesRel16Medium2050.push_back(0.8); 
  uncertaintiesRel16Medium2050.push_back(0.9); 
  uncertaintiesRel16Medium2050.push_back(0.); 
  uncertaintiesRel16Medium2050.push_back(1.0);
  uncertaintiesRel16Medium2050.push_back(2.5);
  //Tight
  efficienciesRel16Tight2050.push_back(100.2); 
  efficienciesRel16Tight2050.push_back(103.2); 
  efficienciesRel16Tight2050.push_back(0.);
  efficienciesRel16Tight2050.push_back(106.5); 
  efficienciesRel16Tight2050.push_back( 98.9); 
  efficienciesRel16Tight2050.push_back( 99.6);
  efficienciesRel16Tight2050.push_back(105.9); 
  efficienciesRel16Tight2050.push_back(0.);
  efficienciesRel16Tight2050.push_back(104.8); 
  efficienciesRel16Tight2050.push_back(103.7);
  uncertaintiesRel16Tight2050.push_back(1.2);
  uncertaintiesRel16Tight2050.push_back(1.9); 
  uncertaintiesRel16Tight2050.push_back(0.);
  uncertaintiesRel16Tight2050.push_back(1.4); 
  uncertaintiesRel16Tight2050.push_back(0.9); 
  uncertaintiesRel16Tight2050.push_back(0.9);
  uncertaintiesRel16Tight2050.push_back(1.1); 
  uncertaintiesRel16Tight2050.push_back(0.); 
  uncertaintiesRel16Tight2050.push_back(2.8); 
  uncertaintiesRel16Tight2050.push_back(2.6);



  //For the ET-corrections of the scale factors
  //Medium
  ETCorrectionsMedium.push_back(85.2);
  ETCorrectionsMedium.push_back(92.8);
  ETCorrectionsMedium.push_back(97.2);
  ETCorrectionsMedium.push_back(99.8);
  ETCorrectionsMedium.push_back(100.2);
  ETCorrectionsMedium.push_back(100.3);
  ETCorrectionsMedium.push_back(101.3);
  uncertaintiesETCorrectionsMedium.push_back(10.2);
  uncertaintiesETCorrectionsMedium.push_back( 4.4);
  uncertaintiesETCorrectionsMedium.push_back( 1.5);
  uncertaintiesETCorrectionsMedium.push_back( 0.7);
  uncertaintiesETCorrectionsMedium.push_back( 0.6);
  uncertaintiesETCorrectionsMedium.push_back( 0.7);
  uncertaintiesETCorrectionsMedium.push_back( 1.3);
  //Medium
  ETCorrectionsTight.push_back(83.5);
  ETCorrectionsTight.push_back(91.9);
  ETCorrectionsTight.push_back(96.2);
  ETCorrectionsTight.push_back(100.0);
  ETCorrectionsTight.push_back( 99.8);
  ETCorrectionsTight.push_back(100.0);
  ETCorrectionsTight.push_back(100.9);
  uncertaintiesETCorrectionsTight.push_back(16.9);
  uncertaintiesETCorrectionsTight.push_back( 7.9);
  uncertaintiesETCorrectionsTight.push_back( 1.6);
  uncertaintiesETCorrectionsTight.push_back( 1.3);
  uncertaintiesETCorrectionsTight.push_back( 0.7);
  uncertaintiesETCorrectionsTight.push_back( 0.9);
  uncertaintiesETCorrectionsTight.push_back( 2.4);


}

std::pair<float,float> egammaSFclass::scaleFactor(float eta, float ET, int set, int range, int rel, bool etcorrection) {

   int ietabin=-1;
   while (ietabin<m_nEtabins && eta>=m_Etabins[ietabin+1]) ietabin++;
   if (ietabin<0 || ietabin>=m_nEtabins) {
     std::cout << "egammaSFclass: ERROR : given eta value outside range of existing scale factors" << std::endl;
     return make_pair(-1.,-1.);
   }

   std::vector<float> * vectEff=0;
   std::vector<float> * vectUnc=0;

   if (rel==1) {
     if (range==0) { //20-50 GeV region
       if (set==0 || set>2) {//Loose
	 std::cout << "egammaSFclass: ERROR : only Medium and Tight scale factors exist" << std::endl;
	 return make_pair(-1.,-1.);
       }
       else if (set==1) {//Medium
	 vectEff = &efficienciesRel16Medium2050;
	 vectUnc = &uncertaintiesRel16Medium2050;
       }
       else if (set==2) {//Tight
	 vectEff = &efficienciesRel16Tight2050;
	 vectUnc = &uncertaintiesRel16Tight2050;
       }
     }//endif 20-50 GeV
     else if (range==1) { //30-50 GeV region
       if (set==0 || set>2) {//Loose
	 std::cout << "egammaSFclass: ERROR : only Medium and Tight scale factors exist" << std::endl;
	 return make_pair(-1.,-1.);
       }
       else if (set==1) {//Medium
	 vectEff = &efficienciesRel16Medium3050;
	 vectUnc = &uncertaintiesRel16Medium3050;
       }
       else if (set==2) {//Tight
	 vectEff = &efficienciesRel16Tight3050;
	 vectUnc = &uncertaintiesRel16Tight3050;
       }
     }//endif 30-50 GeV
     else {
	 std::cout << "egammaSFclass: ERROR : invalid range" << std::endl;
	 return make_pair(-1.,-1.);
     }
   }
   else { //release 15 numbers
     if (range==0) { //20-50 GeV region
       if (set==0) {//Loose
	 vectEff = &efficienciesRel15Loose2050;
	 vectUnc = &uncertaintiesRel15Loose2050;
       }
       else if (set==1) {//Medium
	 vectEff = &efficienciesRel15Medium2050;
	 vectUnc = &uncertaintiesRel15Medium2050;
       }
       else if (set==2) {//Tight
	 vectEff = &efficienciesRel15Tight2050;
	 vectUnc = &uncertaintiesRel15Tight2050;
       }
       else {
	 std::cout << "egammaSFclass: ERROR : invalid set of cuts" << std::endl;
	 return make_pair(-1.,-1.);
       }
     }//endif 20-50 GeV
     else if (range==1) { //30-50 GeV region
       if (set==0) {//Loose
	 vectEff = &efficienciesRel15Loose3050;
	 vectUnc = &uncertaintiesRel15Loose3050;
       }
       else if (set==1) {//Medium
	 vectEff = &efficienciesRel15Medium3050;
	 vectUnc = &uncertaintiesRel15Medium3050;
       }
       else if (set==2) {//Tight
	 vectEff = &efficienciesRel15Tight3050;
	 vectUnc = &uncertaintiesRel15Tight3050;
       }
       else {
	 std::cout << "egammaSFclass: ERROR : invalid set of cuts" << std::endl;
	 return make_pair(-1.,-1.);
       }
     }//endif 30-50 GeV
     else {
	 std::cout << "egammaSFclass: ERROR : invalid range" << std::endl;
	 return make_pair(-1.,-1.);
     }
   }//endif rel15

   float effvseta = vectEff->at(ietabin)/100.;
   float uncvseta = vectUnc->at(ietabin)/100.;

   float eff = effvseta;
   float unc = uncvseta;

   if (etcorrection) {
     std::pair<float,float> corr = etCorrection(ET, set);
     eff *= corr.first;
     unc = sqrt( unc*unc + corr.second*corr.second );
   }

   return make_pair(eff,unc);
}


//Returns the ET-correction factor (and uncertainty) to the scale factor for the correspond ET bin and Medium (set=1) or Tight (set=2)
std::pair<float,float> egammaSFclass::etCorrection(float ET, int set) {
  int iETbin=-1;
  while (iETbin<m_nETbins && ET>=m_ETbins[iETbin+1]) iETbin++;
  if (iETbin<0 || iETbin>=m_nETbins) {
    std::cout << "egammaSFclass: ERROR : given ET value (" << ET << ") outside range of existing ET-correction factors" << std::endl;
    return make_pair(-1.,-1.);
  }
  
  
  std::vector<float> * vectCorr=0;
  std::vector<float> * vectUncCorr=0;
  
  if (set==0) {//Loose
    std::cout << "egammaSFclass: ERROR : ET-correction factors not implemented for Loose" << std::endl;
    return make_pair(-1.,-1.);
  }
  else if (set==1) {//Medium
    vectCorr = &ETCorrectionsMedium;
    vectUncCorr = &uncertaintiesETCorrectionsMedium;
  }
  else if (set==2) {//Tight
    vectCorr = &ETCorrectionsTight;
    vectUncCorr = &uncertaintiesETCorrectionsTight;
  }
  
  return make_pair(vectCorr->at(iETbin)/100., vectUncCorr->at(iETbin)/100.);
}

