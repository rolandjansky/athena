/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tool to compute track isolation variable for photons
// some care is taken to remove conversion tracks from the SumPt of the tracks in the cone
//  G Unal  7 june 2007

#include "PhotonAnalysisUtils/PAUphotonIdentificationTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/Photon.h"
#include "egammaEvent/EMShower.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"

#include <algorithm>
#include <math.h>
#include <functional>

// constructor
PAUphotonIdentificationTool::PAUphotonIdentificationTool(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t,n,p) {
  declareInterface<IPAUphotonIdentificationTool>(this);
  declareProperty("EgDetailContainerName", m_egDetailContainer = "egDetailAOD");
}

// destructor
PAUphotonIdentificationTool::~PAUphotonIdentificationTool()
{}

// Athena standard methods

// initialize
StatusCode PAUphotonIdentificationTool::initialize()
{
  return StatusCode::SUCCESS;
}

// finalize
StatusCode PAUphotonIdentificationTool::finalize()
{
  MsgStream log(msgSvc(), name());
  log <<  MSG::VERBOSE  << "... in finalize() ..." << endreq ;
  return StatusCode::SUCCESS;
}

int PAUphotonIdentificationTool::photonCuts(const Analysis::Photon* phot, int itune, int iconv)
{
 const CaloCluster* clus; 
 clus = phot->cluster();
 if (clus) 
   m_pt = clus->energy()/cosh(std::fabs(clus->etaBE(2)));
 if (m_pt<100.) return 0;
 const EMShower* shower; 
 shower = phot->detail<EMShower>(m_egDetailContainer);
 if (shower && clus){
   m_eta2 = std::fabs((clus)->etaBE(2));
   m_rhad = (shower)->parameter(egammaParameters::ethad1)/m_pt;
   m_e277 = (shower)->parameter(egammaParameters::e277);
   m_e237 = (shower)->parameter(egammaParameters::e237);
   m_e233 = (shower)->parameter(egammaParameters::e233);
   m_weta2 = (shower)->parameter(egammaParameters::weta2);
   m_f1 = (shower)->parameter(egammaParameters::f1);
   m_emax2 = (shower)->parameter(egammaParameters::e2tsts1);
   m_emin2 = (shower)->parameter(egammaParameters::emins1);
   m_fracm = (shower)->parameter(egammaParameters::fracs1);
   m_wtot  = (shower)->parameter(egammaParameters::wtots1);
   m_w1    = (shower)->parameter(egammaParameters::weta1);
   m_emax2r = m_emax2/(1.+0.009*(m_pt/1000));
   m_deltae = m_emax2-m_emin2;
 }
 else{
   m_eta2   = -999.;
   m_rhad   = -999.;
   m_e277   = -999.;
   m_e237   = -999.;
   m_e233   = -999.;
   m_weta2  = -999.;
   m_f1     = -999.;
   m_emax2  = -999.;
   m_emin2  = -999.;
   m_fracm  = -999.;
   m_wtot   = -999.;
   m_w1     = -999.;
   m_emax2r = -999.;
   m_deltae = -999.;
 }
 m_conv = iconv;
 int status = this->cuts(itune);
 return status;
}

int PAUphotonIdentificationTool::photonCuts(const Analysis::Electron* elec, int itune, int iconv)
{
 const CaloCluster* clus; 
 clus = elec->cluster();
 m_pt = 0;
 if (clus){
   float eta = std::fabs(clus->etaBE(2));
   if(eta<5)
     m_pt = clus->energy()/cosh(eta);
 }
 if (m_pt<100.) return 0;
 const EMShower* shower; 
 shower = elec->detail<EMShower>(m_egDetailContainer);
 if (shower && clus){
   m_eta2 = std::fabs((clus)->etaBE(2));
   m_rhad = (shower)->parameter(egammaParameters::ethad1)/m_pt;//ethad1())/m_pt;
   m_e277 = (shower)->parameter(egammaParameters::e277);//e277();
   m_e237 = (shower)->parameter(egammaParameters::e237);//e237();
   m_e233 = (shower)->parameter(egammaParameters::e233);//e233();
   m_weta2 = (shower)->parameter(egammaParameters::weta2);//weta2();
   m_f1 = (shower)->parameter(egammaParameters::f1);//f1();
   m_emax2 = (shower)->parameter(egammaParameters::e2tsts1);//e2tsts1();
   m_emin2 = (shower)->parameter(egammaParameters::emins1);//emins1();
   m_fracm = (shower)->parameter(egammaParameters::fracs1);//fracs1();
   m_wtot  = (shower)->parameter(egammaParameters::wtots1);//wtots1();
   m_w1    = (shower)->parameter(egammaParameters::weta1);//weta1();
   m_emax2r = m_emax2/(1.+0.009*(m_pt/1000));
   m_deltae = m_emax2-m_emin2;
 }
 else{
   m_eta2   = -999.;
   m_rhad   = -999.;
   m_e277   = -999.;
   m_e237   = -999.;
   m_e233   = -999.;
   m_weta2  = -999.;
   m_f1     = -999.;
   m_emax2  = -999.;
   m_emin2  = -999.;
   m_fracm  = -999.;
   m_wtot   = -999.;
   m_w1     = -999.;
   m_emax2r = -999.;
   m_deltae = -999.;
 }
 m_conv = iconv;
 int status = this->cuts(itune);
 return status;
}

int PAUphotonIdentificationTool::cuts(int itune)
{
 int status=0;
 if (m_eta2<1.37 || (m_eta2>1.52 && m_eta2<2.37)) status+=1;

// default tuning = 2
 if (itune==0) itune=2;

 if (itune==1) {
   if (this->CutHad_tune1()) status+=2;
   if (this->CutS2_tune1()) status+=4;
   if (this->CutS1_tune1()) status+=8;
 }
 if (itune==2) {
   if (this->CutHad_tune2()) status+=2;
   if (this->CutS2_tune2()) status+=4;
   if (this->CutS1_tune2()) status+=8;
 }
 if (itune==3) {
   if (this->CutHad_tune3()) status+=2;
   if (this->CutS2_tune3()) status+=4;
   if (this->CutS1_tune3()) status+=8;
 }
 return status;
}

//
// =====================================================================================================
// tune1 = cuts from January 2007

bool PAUphotonIdentificationTool::CutHad_tune1() {
  static double rcut[5]={0.010,0.0030,0.0120,0.011,0.025};

  int ibin;
  if (m_eta2<0.80) ibin=0;
  else if (m_eta2<1.5) ibin=1;
  else if (m_eta2<1.8) ibin=2;
  else if (m_eta2<2.0) ibin=3;
  else  ibin=4;
   
  if (m_rhad>rcut[ibin]) return false;
  return true;
}  

bool PAUphotonIdentificationTool::CutS2_tune1() {
// new cuts
    static double   ratio1_cut[4][5]={{0.91,0.925,0.90,0.92,0.925},
                                      {0.935,0.933,0.912,0.925,0.91},
                                      {0.943,0.942,0.92,0.940,0.93},
                                      {0.953,0.947,0.925,0.945,0.93}};
    static double   ratio2_cut[4][5]={{0.65,0.65,0.76,0.80,0.75},
                                      {0.75,0.75,0.77,0.80,0.84},
                                      {0.83,0.83,0.85,0.88,0.88},
                                      {0.89,0.89,0.91,0.91,0.92}};
    static double   weta2_cut[4][5]={{0.0109,0.0115,0.0130,0.0117,0.0120},
                                     {0.0105,0.0110,0.0125,0.0112,0.0120},
                                     {0.0101,0.0107,0.0121,0.0109,0.0118},
                                     {0.0100,0.0104,0.0115,0.0105,0.0115}};


   if (m_e277<0.1) return false;
   if (m_e237<0.1) return false;

   double ratio1=m_e237/m_e277;
   double ratio2=m_e233/m_e237;
   double weta2 = m_weta2;

   int ibine;
   if (m_pt<30000.) ibine=0;
   else if (m_pt<40000.) ibine=1;
   else if (m_pt<50000.) ibine=2;
   else ibine=3;

   int ibin;
   if (m_eta2<0.80) ibin=0;
   else if (m_eta2<1.50) ibin=1;
   else if (m_eta2<1.80) ibin=2;
   else if (m_eta2<2.0)  ibin=3;
   else ibin=4;

   if (ratio1 < ratio1_cut[ibine][ibin]) return false;
   if (ratio2 < ratio2_cut[ibine][ibin]) return false;
   if (weta2 > weta2_cut[ibine][ibin]) return false;
   return true;

}

bool PAUphotonIdentificationTool::CutS1_tune1() {

// emax2r is  MeV
    static double   cut_emax2r[3][5]={{180.,270.,400.,350.,350.},
                                      {180.,270.,450.,300.,300.},
                                      {180.,210.,450.,300.,500.}};

// deltaE in MeV
    static double   cut_deltae[3][5]={{140.,120.,300.,210.,250.},
                                      {140.,120.,350.,230.,300.},
                                      {140.,120.,350.,230.,300.}};

// wtot (no unit)
    static double   wtot_cut[3][5]={{2.5,2.7,3.0,2.1,1.45},
                                    {2.4,2.7,2.9,2.1,1.45},
                                    {2.2,2.7,2.8,2.1,1.45}};

// fracs1 (no unit)
    static double   fracm_cut[3][5]={{0.33,0.45,0.47,0.30,0.19},
                                     {0.30,0.43,0.47,0.27,0.18},
                                     {0.25,0.35,0.45,0.22,0.18}};
// w1 (no unit)
    static double   w1_cut[3][5]={{0.67,0.70,0.75,0.67,0.65},
                                  {0.67,0.70,0.73,0.65,0.63},
                                  {0.65,0.69,0.73,0.65,0.63}};

   if (m_f1<0.005) return false;

   int ibin=-1;
   if (m_eta2<0.80) ibin=0;
   else if (m_eta2<1.37) ibin=1;
   else if (m_eta2>1.52 && m_eta2<1.8) ibin=2;
   else if (m_eta2>1.8  && m_eta2<2.0) ibin=3;
   else if (m_eta2>2.0  && m_eta2<2.37) ibin=4;
   if (ibin<0) return false;

   int ibine=0;
   if (m_pt>30000.) ibine=1;
   if (m_pt>40000.) ibine=2;

   if (m_emax2r > cut_emax2r[ibine][ibin]) return false;
   if (m_deltae > cut_deltae[ibine][ibin]) return false;
   if (m_wtot > wtot_cut[ibine][ibin]) return false;
   if (m_fracm > fracm_cut[ibine][ibin]) return false;
   if (m_w1 > w1_cut[ibine][ibin]) return false;
    
   return true;

}


// ===============================================================================================
// tune2 from August 2007 (R.Souche)

bool PAUphotonIdentificationTool::CutHad_tune2()
{
    static double rcut[5]={0.006,0.003,0.008,0.009,0.007}; //cut unconverted photon

    int ibin;
    if (m_eta2<0.70) ibin=0;
    else if (m_eta2<1.5) ibin=1;
    else if (m_eta2<1.8) ibin=2;
    else if (m_eta2<2.0) ibin=3;
    else  ibin=4;

    if (m_rhad>rcut[ibin]) return false;
    return true;
}

bool PAUphotonIdentificationTool::CutS2_tune2()
{
// all the variables below are without unit
// new cuts
    static double ratio1_cut[7][6]={{0.925,0.925,0.920,0.905,0.925,0.915},//1% H, 60% fake
                                      {0.935,0.925,0.925,0.905,0.930,0.920},
                                      {0.941,0.932,0.930,0.910,0.93,0.92},
                                      {0.943,0.937,0.932,0.910,0.93,0.922},
                                      {0.946,0.94,0.935,0.916,0.93,0.922},
                                      {0.946,0.94,0.937,0.91,0.94,0.928},
                                      {0.952,0.946,0.943,0.925,0.94,0.93}};

    static double ratio2_cut[7][6]={{0.6,0.62,0.6,0.68,0.74,0.8}, //1% exclusion phot de H
                                      {0.66,0.69,0.65,0.73,0.8,0.86},//50% exclusion jet
                                      {0.75,0.75,0.76,0.82,0.87,0.88},
                                      {0.82,0.82,0.82,0.86,0.9,0.9},
                                      {0.86,0.86,0.86,0.89,0.9,0.91},
                                      {0.89,0.88,0.88,0.9,0.91,0.925},
                                      {0.92,0.92,0.9,0.915,0.926,0.925}};

// new cuts 
    static double weta2_cut_sup[7][6]={{0.0108,0.0115,0.0115,0.0114,0.0114,0.0123}, //rejet 2% des phot de H
                                         {0.0105,0.011,0.0113,0.0129,0.0112,0.0122}, //40 - 60% exclusion jet
                                         {0.0105,0.0108,0.0114,0.0127,0.011,0.012},
                                         {0.01,0.0105,0.0108,0.0127,0.0108,0.0118},
                                         {0.0101,0.0104,0.0106,0.012,0.0107,0.0117},
                                         {0.0098,0.0104,0.0107,0.0123,0.0108,0.0119},
                                         {0.0097,0.0102,0.0105,0.0125,0.0108,0.0114}};
                                       
   if (m_e277<0.1) {
     return false;
   }

    double ratio1=m_e237/m_e277;
    double ratio2=m_e233/m_e237;
    double weta2 = m_weta2;

    int ibine;
    if (m_pt<30000.) ibine=0; //cf CutHad ??
    else if (m_pt<40000.) ibine=1;
    else if (m_pt<50000.) ibine=2;
    else if (m_pt<60000.) ibine=3;
    else if (m_pt<70000.) ibine=4;
    else if (m_pt<80000.) ibine=5;
    else ibine=6;

// new eta division
    int ibin;
    if (m_eta2<0.70) ibin=0;
    else if (m_eta2<1.0) ibin=1;
    else if (m_eta2<1.50) ibin=2;
    else if (m_eta2<1.80) ibin=3;
    else if (m_eta2<2.0)  ibin=4;
    else ibin=5;


    if (ratio1 < ratio1_cut[ibine][ibin]) return false;
    if (ratio2 < ratio2_cut[ibine][ibin]) return false; 
    if (weta2 > weta2_cut_sup[ibine][ibin] ) return false;
    return true;

}

bool PAUphotonIdentificationTool::CutS1_tune2()
{
// wtot_cut is without unit
// new cuts
    static double wtot_cut[8][6]={{2.15,2.55,2.65,2.85,1.75,1.3},
                                    {2.05,2.3,2.45,2.65,1.6,1.3},
                                    {2.35,2.6,2.62,2.95,1.82,1.35},
                                    {2.4,2.6,2.8,3.0,1.9,1.4},
                                    {2.55,2.75,2.95,2.9,2.2,1.5},
                                    {2.15,2.8,2.75,2.8,2.4,1.55},
                                    {2.4,2.4,2.65,3.0,2.0,1.6},
                                    {2.3,2.6,2.8,2.9,2.2,1.6}};

// emax2r is  in MeV
    static double cut_emax2r[8][6]={{130.,162.,220.,357.,280.,300.},
                                      {135.,150.,240.,315.,240.,250.},
                                      {175.,265.,270.,400.,460.,270.},
                                      {220.,260.,280.,600.,500.,700.},
                                      {367.,550.,395.,700.,700.,700.},
                                      {300.,400.,400.,500.,500.,400.},
                                      {280.,280.,420.,500.,400.,370.},
                                      {460.,340.,400.,650.,500.,460.}};

// deltaE is in energy unit (->MeV)
// new cuts
    static double cut_deltae[8][6]={{100.,75.,90.,200.,130.,140.},
                                      {100.,75.,90.,150.,140.,155.},//penser que certaines coupures ne sont pas realistes
                                      {130.,130.,135.,150.,190.,145.},//cf 1000
                                      {140.,110.,185.,150.,320.,400.},
                                      {300.,260.,190.,460.,540.,550.},
                                      {280.,400.,190.,250.,500.,400.},
                                      {320.,280.,280.,360.,500.,460.},
                                      {200.,200.,200.,600.,500.,600.}};

// fracm_cut is without unit
    static double fracm_cut[8][6]={{0.262,0.342,0.402,0.462,0.231,0.18},
                                     {0.262,0.342,0.402,0.452,0.231,0.18},
                                     {0.255,0.340,0.385,0.45,0.230,0.18},
                                     {0.25,0.340,0.385,0.45,0.23,0.18},
                                     {0.245,0.34,0.38,0.45,0.23,0.18},
                                     {0.245,0.34,0.40,0.45,0.23,0.18},
                                     {0.25,0.35,0.40,0.44,0.25,0.19},
                                     {0.25,0.35,0.40,0.45,0.25,0.19}};

// w1_cut is without unit
// new cuts
    static double w1_cut[8][6]={{0.65,0.68,0.68,0.72,0.64,0.62},
                                  {0.65,0.68,0.68,0.68,0.64,0.62},
                                  {0.64,0.68,0.7,0.72,0.64,0.6},
                                  {0.68,0.7,0.72,0.74,0.68,0.62},
                                  {0.68,0.7,0.7,0.72,0.64,0.62},
                                  {0.64,0.655,0.7,0.7,0.7,0.64},
                                  {0.62,0.68,0.7,0.74,0.68,0.68},
                                  {0.66,0.66,0.68,0.74,0.655,0.62}};
    if (m_eta2>2.37) return false;
    if (m_f1<0.005)  return false;

    int ibin=-1; 
    if (m_eta2<0.70) ibin=0;
    else if (m_eta2<1.00) ibin=1;
    else if (m_eta2<1.37) ibin=2;
    else if (m_eta2>1.52 && m_eta2<1.8) ibin=3;
    else if (m_eta2>1.8  && m_eta2<2.0) ibin=4;
    else if (m_eta2>2.0  && m_eta2<2.37) ibin=5;

    int ibine;
    if (m_pt<25000.) ibine=0; //cf CutHad ??
    else if (m_pt<30000.) ibine=1;
    else if (m_pt<40000.) ibine=2;
    else if (m_pt<50000.) ibine=3;
    else if (m_pt<60000.) ibine=4;
    else if (m_pt<70000.) ibine=5;
    else if (m_pt<80000.) ibine=6;
    else ibine=7; 
    if (ibin==-1) return false;

    if (m_emax2r > cut_emax2r[ibine][ibin]) return false;
    if (m_deltae > cut_deltae[ibine][ibin]) return false;

    if (m_wtot > wtot_cut[ibine][ibin]) return false;
    if (m_fracm > fracm_cut[ibine][ibin]) return false;
    if (m_w1 > w1_cut[ibine][ibin]) return false;

    return true;
}


// ==================================================================
// tune3 different cuts for converted / unconverted photons
bool PAUphotonIdentificationTool::CutHad_tune3()
{
// new cuts
    double rcut_nc[5]={0.006,0.002,0.008,0.009,0.007}; //cut unconverted photon
    double rcut_c[5]={0.02,0.01,0.04,0.04,0.05}; //environ 30% fake passe pour moins de 1%de H coupe

   
    int ibin;
    if (m_eta2<0.80) ibin=0;
    else if (m_eta2<1.5) ibin=1;
    else if (m_eta2<1.8) ibin=2;
    else if (m_eta2<2.0) ibin=3;
    else  ibin=4;
   
    if (m_conv==0)
        {if (m_rhad>rcut_nc[ibin]) return false;}
    else
        {if (m_rhad>rcut_c[ibin]) return false;}
    return true;
}

bool PAUphotonIdentificationTool::CutS2_tune3()
{
// all the variables below are without unit
// new cuts for unconverted photon
    static double ratio1_cut_nc[7][6]={{0.94,0.93,0.93,0.913,0.934,0.92},//1% H, 70-80% fake
                                         {0.943,0.934,0.934,0.915,0.938,0.928},
                                         {0.946,0.942,0.94,0.92,0.937,0.928},
                                         {0.95,0.94,0.938,0.922,0.94,0.928},
                                         {0.946,0.938,0.938,0.92,0.936,0.928},
                                         {0.948,0.944,0.948,0.928,0.938,0.93},
                                         {0.95,0.944,0.945,0.93,0.94,0.926}};

    static double ratio2_cut_nc[7][6]={{0.8,0.75,0.75,0.8,0.79,0.84}, 
                                         {0.9,0.86,0.86,0.86,0.88,0.9},//50% exclusion jet
                                         {0.9,0.88,0.88,0.87,0.88,0.93},
                                         {0.92,0.9,0.87,0.89,0.91,0.93},
                                         {0.93,0.926,0.915,0.925,0.93,0.93},
                                         {0.94,0.92,0.925,0.93,0.93,0.935},
                                         {0.95,0.94,0.935,0.935,0.94,0.938}};

    static double weta2_cut_inf_nc[7][6]={{0.0081,0.0085,0.0087,0.009,0.009,0.01}, //0
                                            {0.0076,0.0085,0.0087,0.009,0.009,0.01},//1
                                            {0.0078,0.0077,0.008,0.0082,0.0088,0.0095},//2
                                            {0.0075,0.0078,0.0079,0.008,0.0085,0.0094},//3
                                            {0.0075,0.0076,0.008,0.0083,0.0085,0.0094},//4
                                            {0.0075,0.0079,0.008,0.0082,0.0088,0.0095},//5
                                            {0.0072,0.0076,0.008,0.008,0.0087,0.009}};//6

    static double weta2_cut_sup_nc[7][6]={{0.0103,0.0106,0.011,0.0118,0.011,0.0118}, //rejet 2% des phot de H
                                            {0.0102,0.0105,0.011,0.012,0.0109,0.0118}, //40 - 60% exclusion jet
                                            {0.0099,0.0104,0.0107,0.0118,0.0107,0.0116},
                                            {0.0098,0.0102,0.0107,0.0116,0.0106,0.0118},
                                            {0.01,0.0104,0.0106,0.0116,0.0107,0.0115},
                                            {0.0099,0.0102,0.0104,0.012,0.0107,0.0115},
                                            {0.0097,0.0105,0.0106,0.0112,0.0106,0.0114}};

//new cuts for converted photon
    static double ratio1_cut_c[7][6]={{0.915,0.913,0.91,0.875,0.92,0.915},
                                        {0.915,0.915,0.92,0.885,0.926,0.91},
                                        {0.934,0.926,0.92,0.89,0.932,0.91},
                                        {0.938,0.932,0.925,0.89,0.928,0.915},
                                        {0.94,0.93,0.93,0.9,0.93,0.925},
                                        {0.945,0.93,0.93,0.9,0.935,0.92},
                                        {0.945,0.93,0.94,0.91,0.94,0.92}};

    static double weta2_cut_inf_c[7][6]={{0.0085,0.0086,0.0092,0.0096,0.0095,0.0105},
                                           {0.0083,0.0089,0.0092,0.0093,0.009,0.0102},
                                           {0.008,0.0085,0.0089,0.009,0.009,0.01},
                                           {0.008,0.0085,0.0087,0.009,0.009,0.0101},
                                           {0.0079,0.0085,0.0087,0.0089,0.009,0.01},
                                           {0.008,0.0087,0.0089,0.009,0.009,0.01},
                                           {0.008,0.0084,0.0084,0.0089,0.009,0.0095}};

    static double weta2_cut_sup_c[7][6]={{0.0111,0.0117,0.0116,0.0128,0.0114,0.0125},
                                           {0.0108,0.011,0.0114,0.0128,0.0113,0.0123},
                                           {0.0107,0.0109,0.0113,0.0127,0.0113,0.0121},
                                           {0.0104,0.0111,0.0112,0.0124,0.0111,0.012},
                                           {0.0103,0.0111,0.0113,0.0121,0.0112,0.012},
                                           {0.0105,0.0106,0.011,0.012,0.011,0.0117},
                                           {0.0102,0.0105,0.011,0.0115,0.0107,0.0116}};

    static double ratio2_cut_c[7][6]={{0.62,0.66,0.65,0.7,0.75,0.82},  // 2% de H exclue au min
                                        {0.64,0.69,0.65,0.78,0.84,0.86}, //20% de fake exclue au min
                                        {0.66,0.71,0.75,0.83,0.88,0.885}, // competition entre les 2
                                        {0.78,0.79,0.82,0.86,0.88,0.89}, //surtout pour les pt faibles
                                        {0.78,0.8,0.79,0.87,0.88,0.9}, //et les eta faible
                                        {0.84,0.83,0.85,0.87,0.91,0.91},
                                        {0.885,0.89,0.86,0.91,0.905,0.92}};



   if (m_e277<0.1 || m_e237<0.1)
   {
     return false;
   }

    double ratio1=m_e237/m_e277;
    double ratio2=m_e233/m_e237;
    double weta2 = m_weta2;

    int ibine;
    if (m_pt<30000.) ibine=0; //cf CutHad ??
    else if (m_pt<40000.) ibine=1;
    else if (m_pt<50000.) ibine=2;
    else if (m_pt<60000.) ibine=3;
    else if (m_pt<70000.) ibine=4;
    else if (m_pt<80000.) ibine=5;
    else ibine=6;

// new eta division
    int ibin;
    if (m_eta2<0.70) ibin=0;
    else if (m_eta2<1.0) ibin=1;
    else if (m_eta2<1.50) ibin=2;
    else if (m_eta2<1.80) ibin=3;
    else if (m_eta2<2.0)  ibin=4;
    else ibin=5;

    if (m_conv==0)
        {
        if (ratio1 < ratio1_cut_nc[ibine][ibin]) return false;
        if (weta2 < weta2_cut_inf_nc[ibine][ibin] || weta2 > weta2_cut_sup_nc[ibine][ibin]) return false;
        if (ratio2 < ratio2_cut_nc[ibine][ibin]) return false;
        }
    else
        {
        if (ratio1 < ratio1_cut_c[ibine][ibin]) return false;
        if (weta2 < weta2_cut_inf_c[ibine][ibin] || weta2 > weta2_cut_sup_c[ibine][ibin]) return false;
        if (ratio2 < ratio2_cut_c[ibine][ibin]) return false;
        }
   return true;
}

bool PAUphotonIdentificationTool::CutS1_tune3()
{
//for unconverted photon
// wtot_cut is without unit
// new cuts
    static double wtot_cut_nc[8][6]={{2.15,2.55,2.65,2.85,1.75,1.3},
                                    {2.15,2.4,2.55,2.75,1.7,1.35},
                                    {2.3,2.6,2.6,2.9,1.8,1.35},
                                    {2.2,2.5,2.65,2.95,1.9,1.35},
                                    {2.45,2.65,2.65,2.9,2.1,1.4},
                                    {2.4,2.7,2.65,2.8,2.1,1.5},
                                    {2.4,2.4,2.65,3.0,2.1,1.6},
                                    {2.3,2.6,2.8,2.9,2.2,1.5}};

// emax2r is  in MeV
// new cuts
    static double cut_emax2r_nc[8][6]={{130.,162.,280.,400.,280.,300.},
                                      {135.,230.,270.,400.,242.5,200.},
                                      {175.,265.,270.,330.,460.,270.},
                                      {220.,260.,280.,600.,500.,700.},
                                      {370.,550.,395.,700.,700.,700.},
                                      {170.,230.,250.,600.,360.,400.},
                                      {280.,280.,420.,500.,400.,330.},
                                      {460.,340.,400.,650.,500.,460.}};

// deltaE is in energy unit (->MeV)
// new cuts
    static double cut_deltae_nc[8][6]={{105.,75.,90.,300.,140.,155.},
                                      {105.,75.,90.,100.,140.,155.},//penser que certaines coupures ne sont pas realistes
                                      {140.,150.,135.,175.,190.,145.},//cf 1000
                                      {150.,110.,185.,150.,320.,400.},
                                      {300.,260.,190.,460.,540.,550.},
                                      {280.,400.,190.,300.,500.,400.},
                                      {320.,280.,280.,360.,1000.,300.},
                                      {175.,170.,200.,560.,380.,560.}};
                 
// fracm_cut is without unit
    static double fracm_cut_nc[8][6]={{0.265,0.345,0.405,0.49,0.235,0.18},
                                     {0.265,0.39,0.405,0.49,0.235,0.18},
                                     {0.255,0.33,0.37,0.48,0.245,0.18},
                                     {0.25,0.325,0.37,0.45,0.23,0.17},
                                     {0.235,0.32,0.35,0.45,0.215,0.205},
                                     {0.24,0.32,0.37,0.43,0.22,0.205},
                                     {0.26,0.36,0.41,0.43,0.27,0.2},
                                     {0.245,0.325,0.355,0.485,0.225,0.19}};
// w1_cut is without unit
// new cuts
    static double w1_cut_nc[8][6]={{0.65,0.68,0.68,0.74,0.64,0.62},
                                  {0.65,0.68,0.68,0.72,0.64,0.62},
                                  {0.64,0.68,0.7,0.72,0.64,0.6},
                                  {0.68,0.7,0.72,0.74,0.68,0.62},
                                  {0.68,0.7,0.68,0.72,0.64,0.62},
                                  {0.64,0.655,0.68,0.72,0.7,0.64},
                                  {0.62,0.68,0.7,0.74,0.68,0.68},
                                  {0.66,0.66,0.68,0.74,0.655,0.62}};

    static double wtot_cut_c[8][6]={{2.15,2.45,2.65,2.95,1.7,1.25},
                                      {2.2,2.7,2.75,2.8,1.8,1.3},
                                      {2.3,2.5,2.65,3.0,1.85,1.4},
                                      {2.4,2.8,2.75,2.95,1.9,1.4},
                                      {2.25,2.65,2.75,3.05,2.2,1.4},
                                      {2.25,2.8,2.9,3.0,2.2,1.5},
                                      {2.4,2.8,3.0,3.0,1.9,1.5},
                                      {2.3,2.7,2.9,3.1,2.0,1.5}};

    static double cut_emax2r_c[8][6]={{100.,120.,220.,280.,220.,200.},
                                        {130.,130.,195.,250.,220.,240.},
                                        {175.,265.,250.,400.,260.,230.},
                                        {220.,280.,320.,400.,230.,290.},
                                        {300.,340.,320.,500.,500.,360.},
                                        {230.,300.,300.,500.,400.,290.},
                                        {210.,280.,290.,400.,270.,330.},
                                        {190.,300.,310.,500.,320.,250.}};

    static double cut_deltae_c[8][6]={{105.,75.,90.,300.,140.,155.},
                                      {105.,75.,90.,100.,140.,155.},//penser que certaines coupures ne sont pas realistes
                                      {140.,150.,135.,175.,190.,145.},//cf 1000
                                      {150.,110.,185.,150.,320.,400.},
                                      {300.,260.,190.,460.,540.,550.},
                                      {280.,400.,190.,300.,500.,400.},
                                      {320.,280.,280.,360.,1000.,300.},
                                      {175.,170.,200.,560.,380.,560.}};

    static double fracm_cut_c[8][6]={{0.27,0.35,0.39,0.45,0.23,0.2},
                                       {0.28,0.37,0.38,0.475,0.25,0.19},
                                       {0.3,0.37,0.4,0.49,0.25,0.2},
                                       {0.3,0.4,0.45,0.49,0.26,0.2},
                                       {0.32,0.45,0.45,0.48,0.26,0.2},
                                       {0.31,0.4,0.41,0.46,0.26,0.21},
                                       {0.3,0.38,0.41,0.48,0.26,0.2},
                                       {0.27,0.35,0.4,0.485,0.27,0.22}};
                                       
// w1_cut is without unit
// new cuts
    static double w1_cut_c[8][6]={{0.65,0.68,0.68,0.72,0.64,0.62},
                                  {0.65,0.68,0.68,0.68,0.64,0.62},
                                  {0.64,0.68,0.7,0.72,0.64,0.6},
                                  {0.68,0.7,0.72,0.74,0.68,0.62},
                                  {0.68,0.7,0.7,0.72,0.64,0.62},
                                  {0.64,0.655,0.7,0.72,0.7,0.64},
                                  {0.62,0.68,0.7,0.74,0.68,0.68},
                                  {0.66,0.66,0.68,0.74,0.655,0.62}};

    if (m_eta2>2.37) return false;
    if (m_f1<0.005)  return false;

    int ibin=-1;
    if (m_eta2<0.70) ibin=0;
    else if (m_eta2<1.00) ibin=1;
    else if (m_eta2<1.37) ibin=2;
    else if (m_eta2>1.52 && m_eta2<1.8) ibin=3;
    else if (m_eta2>1.8  && m_eta2<2.0) ibin=4;
    else if (m_eta2>2.0  && m_eta2<2.37) ibin=5;

    int ibine;
    if (m_pt<25000.) ibine=0; //cf CutHad ??
    else if (m_pt<30000.) ibine=1;
    else if (m_pt<40000.) ibine=2;
    else if (m_pt<50000.) ibine=3;
    else if (m_pt<60000.) ibine=4;
    else if (m_pt<70000.) ibine=5;
    else if (m_pt<80000.) ibine=6;
    else ibine=7; 
    if (ibin==-1) return false;


    if (m_conv == 0)
        {
        if (m_wtot > wtot_cut_nc[ibine][ibin]) return false;
        if (m_emax2r > cut_emax2r_nc[ibine][ibin]) return false;
        if(m_deltae > cut_deltae_nc[ibine][ibin]) return false;
        if (m_fracm > fracm_cut_nc[ibine][ibin]) return false;
        if (m_w1 > w1_cut_nc[ibine][ibin]) return false;
        }
    else
        {
        if (m_wtot > wtot_cut_c[ibine][ibin]) return false;
        if (m_emax2r > cut_emax2r_c[ibine][ibin]) return false;
        if (m_deltae > cut_deltae_c[ibine][ibin]) return false;
        if (m_fracm > fracm_cut_c[ibine][ibin]) return false;
        if (m_w1 > w1_cut_c[ibine][ibin]) return false;
        }

    return true;
}

