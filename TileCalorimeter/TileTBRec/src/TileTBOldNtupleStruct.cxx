/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileTBRec/TileTBOldNtupleStruct.h"

TileTBOldNtupleStruct::TileTBOldNtupleStruct(TTree *tree, bool bigain, bool adderFit, std::vector<int>& shift)
  // initialize all these variables here, otherwise Coverity will complain (UNINIT_CTOR warnings)
  : Evtime(0)
  , Run(0)
  , Evt(0)
  , Trig(0)
  , L1id()
  , Evbcid()
  , L1type()
  , Evtype()
  , Frbcid()
  , Mubackhit(0)
  , Mubacksum(0)
  , Muback()
  , Mucalib()
  , Ecal()
  , Lasflag(0)
  , Las0(0)
  , Las1(0)
  , Las2(0)
  , Las3(0)
  , Lasextra()
  , adder()
  , Eneadd()
  , Timeadd()
  , Efitadd()
  , Tfitadd()
  , Pedfitadd()
  , Chi2add()
  , Negstat()
  , Posstat()
  , status()
  , cispar()
  , S1cou(0)
  , S2cou(0)
  , S3cou(0)
  , S4cou(0)
  , Cher1(0)
  , Cher2(0)
  , Beamextra()
  , Xcha1(0)
  , Ycha1(0)
  , btdc1()
  , Xcha2(0)
  , Ycha2(0)
  , btdc2()
  , Ximp(0)
  , Yimp(0)
  , Evtn0lo(0)
  , Bcidn0lo(0)
  , Sizen0lo(0)
  , Dmumaskn0lo()
  , Slinkcrcn0lo()
  , Gainn0lo()
  , Errn0lo()
  , Samplen0lo()
  , Enen0lo()
  , Timen0lo()
  , Efitn0lo()
  , Tfitn0lo()
  , Pedfitn0lo()
  , Chi2n0lo()
  , Evtn1lo(0)
  , Bcidn1lo(0)
  , Sizen1lo(0)
  , Dmumaskn1lo()
  , Slinkcrcn1lo()
  , Gainn1lo()
  , Errn1lo()
  , Samplen1lo()
  , Enen1lo()
  , Timen1lo()
  , Efitn1lo()
  , Tfitn1lo()
  , Pedfitn1lo()
  , Chi2n1lo()
  , Evtn2lo(0)
  , Bcidn2lo(0)
  , Sizen2lo(0)
  , Dmumaskn2lo()
  , Slinkcrcn2lo()
  , Gainn2lo()
  , Errn2lo()
  , Samplen2lo()
  , Enen2lo()
  , Timen2lo()
  , Efitn2lo()
  , Tfitn2lo()
  , Pedfitn2lo()
  , Chi2n2lo()
  , Evtp0lo(0)
  , Bcidp0lo(0)
  , Sizep0lo(0)
  , Dmumaskp0lo()
  , Slinkcrcp0lo()
  , Gainp0lo()
  , Errp0lo()
  , Samplep0lo()
  , Enep0lo()
  , Timep0lo()
  , Efitp0lo()
  , Tfitp0lo()
  , Pedfitp0lo()
  , Chi2p0lo()
  , Evtp1lo(0)
  , Bcidp1lo(0)
  , Sizep1lo(0)
  , Dmumaskp1lo()
  , Slinkcrcp1lo()
  , Gainp1lo()
  , Errp1lo()
  , Samplep1lo()
  , Enep1lo()
  , Timep1lo()
  , Efitp1lo()
  , Tfitp1lo()
  , Pedfitp1lo()
  , Chi2p1lo()
  , Evtp2lo(0)
  , Bcidp2lo(0)
  , Sizep2lo(0)
  , Dmumaskp2lo()
  , Slinkcrcp2lo()
  , Gainp2lo()
  , Errp2lo()
  , Samplep2lo()
  , Enep2lo()
  , Timep2lo()
  , Efitp2lo()
  , Tfitp2lo()
  , Pedfitp2lo()
  , Chi2p2lo()
  , Evtn0hi(0)
  , Bcidn0hi(0)
  , Sizen0hi(0)
  , Dmumaskn0hi()
  , Slinkcrcn0hi()
  , Gainn0hi()
  , Errn0hi()
  , Samplen0hi()
  , Enen0hi()
  , Timen0hi()
  , Efitn0hi()
  , Tfitn0hi()
  , Pedfitn0hi()
  , Chi2n0hi()
  , Evtn1hi(0)
  , Bcidn1hi(0)
  , Sizen1hi(0)
  , Dmumaskn1hi()
  , Slinkcrcn1hi()
  , Gainn1hi()
  , Errn1hi()
  , Samplen1hi()
  , Enen1hi()
  , Timen1hi()
  , Efitn1hi()
  , Tfitn1hi()
  , Pedfitn1hi()
  , Chi2n1hi()
  , Evtn2hi(0)
  , Bcidn2hi(0)
  , Sizen2hi(0)
  , Dmumaskn2hi()
  , Slinkcrcn2hi()
  , Gainn2hi()
  , Errn2hi()
  , Samplen2hi()
  , Enen2hi()
  , Timen2hi()
  , Efitn2hi()
  , Tfitn2hi()
  , Pedfitn2hi()
  , Chi2n2hi()
  , Evtp0hi(0)
  , Bcidp0hi(0)
  , Sizep0hi(0)
  , Dmumaskp0hi()
  , Slinkcrcp0hi()
  , Gainp0hi()
  , Errp0hi()
  , Samplep0hi()
  , Enep0hi()
  , Timep0hi()
  , Efitp0hi()
  , Tfitp0hi()
  , Pedfitp0hi()
  , Chi2p0hi()
  , Evtp1hi(0)
  , Bcidp1hi(0)
  , Sizep1hi(0)
  , Dmumaskp1hi()
  , Slinkcrcp1hi()
  , Gainp1hi()
  , Errp1hi()
  , Samplep1hi()
  , Enep1hi()
  , Timep1hi()
  , Efitp1hi()
  , Tfitp1hi()
  , Pedfitp1hi()
  , Chi2p1hi()
  , Evtp2hi(0)
  , Bcidp2hi(0)
  , Sizep2hi(0)
  , Dmumaskp2hi()
  , Slinkcrcp2hi()
  , Gainp2hi()
  , Errp2hi()
  , Samplep2hi()
  , Enep2hi()
  , Timep2hi()
  , Efitp2hi()
  , Tfitp2hi()
  , Pedfitp2hi()
  , Chi2p2hi()
  ,
  b_Evtime(0), b_Run(0), b_Evt1(0), b_Trig(0), b_L1id(0), b_Evbcid(0), b_L1type(0), b_Evtype(0), b_Frbcid(0)
  , b_Mubackhit(0), b_Mubacksum(0), b_Muback(0), b_Mucalib(0), b_Ecal(0), b_Lasflag(0), b_Las0(0), b_Las1(0)
  , b_Las2(0), b_Las3(0), b_Lasextra(0), b_adder(0), b_Eneadd(0), b_Timeadd(0), b_Efitadd(0), b_Tfitadd(0)
  , b_Pedfitadd(0), b_Chi2add(0), b_Negstat(0), b_Posstat(0), b_status(0), b_cispar(0), b_S1cou(0), b_S2cou(0)
  , b_S3cou(0), b_S4cou(0), b_Cher1(0), b_Cher2(0), b_Beamextra(0), b_Xcha1(0), b_Ycha1(0), b_btdc1(0)
  , b_Xcha2(0), b_Ycha2(0), b_btdc2(0), b_Ximp(0), b_Yimp(0), m_fChain(0)
  , m_adderFit(false), m_nGains(1) 
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normaly not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   m_fChain = tree;
   m_fChain->SetMakeClass(1);

   m_adderFit = adderFit;
   m_nGains = (bigain) ? 2 : 1;
   m_eventShift = shift;
   m_eventShift.resize(7,0); // 7th element for beamROD itself
   int brs = 1; // set status for all branches to 1 for the moment

   m_fChain->SetBranchAddress("Evtime", &Evtime, &b_Evtime);          m_fChain->SetBranchStatus("Evtime",brs);   
   m_fChain->SetBranchAddress("Run", &Run, &b_Run);                   m_fChain->SetBranchStatus("Run",brs);   
   m_fChain->SetBranchAddress("Evt", &Evt, &b_Evt1);                  m_fChain->SetBranchStatus("Evt",brs);   
   m_fChain->SetBranchAddress("Trig", &Trig, &b_Trig);                m_fChain->SetBranchStatus("Trig",brs);  
   m_fChain->SetBranchAddress("L1id", L1id, &b_L1id);                 m_fChain->SetBranchStatus("L1id",brs);  
   m_fChain->SetBranchAddress("Evbcid", Evbcid, &b_Evbcid);           m_fChain->SetBranchStatus("Evbcid",brs);
   m_fChain->SetBranchAddress("L1type", L1type, &b_L1type);           m_fChain->SetBranchStatus("L1type",brs);
   m_fChain->SetBranchAddress("Evtype", Evtype, &b_Evtype);           m_fChain->SetBranchStatus("Evtype",brs);
   m_fChain->SetBranchAddress("Frbcid", Frbcid, &b_Frbcid);           m_fChain->SetBranchStatus("Frbcid",brs);
   m_fChain->SetBranchAddress("Mubackhit", &Mubackhit, &b_Mubackhit); m_fChain->SetBranchStatus("Mubackhit",brs);
   m_fChain->SetBranchAddress("Mubacksum", &Mubacksum, &b_Mubacksum); m_fChain->SetBranchStatus("Mubacksum",brs);
   m_fChain->SetBranchAddress("Muback", Muback, &b_Muback);           m_fChain->SetBranchStatus("Muback",brs);
   m_fChain->SetBranchAddress("Mucalib", Mucalib, &b_Mucalib);        m_fChain->SetBranchStatus("Mucalib",brs);
   m_fChain->SetBranchAddress("Ecal", Ecal, &b_Ecal);                 m_fChain->SetBranchStatus("Ecal",brs);  
   m_fChain->SetBranchAddress("Lasflag", &Lasflag, &b_Lasflag);       m_fChain->SetBranchStatus("Lasflag",brs);  
   m_fChain->SetBranchAddress("Las0", &Las0, &b_Las0);                m_fChain->SetBranchStatus("Las0",brs);  
   m_fChain->SetBranchAddress("Las1", &Las1, &b_Las1);                m_fChain->SetBranchStatus("Las1",brs);  
   m_fChain->SetBranchAddress("Las2", &Las2, &b_Las2);                m_fChain->SetBranchStatus("Las2",brs);  
   m_fChain->SetBranchAddress("Las3", &Las3, &b_Las3);                m_fChain->SetBranchStatus("Las3",brs);  
   m_fChain->SetBranchAddress("Lasextra", Lasextra, &b_Lasextra);     m_fChain->SetBranchStatus("Lasextra",brs); 
   m_fChain->SetBranchAddress("adder", adder, &b_adder);              m_fChain->SetBranchStatus("adder",brs); 
   m_fChain->SetBranchAddress("Eneadd", Eneadd, &b_Eneadd);           m_fChain->SetBranchStatus("Eneadd",brs);
   m_fChain->SetBranchAddress("Timeadd", Timeadd, &b_Timeadd);        m_fChain->SetBranchStatus("Timeadd",brs);  
   if (adderFit) {
     m_fChain->SetBranchAddress("Efitadd", Efitadd, &b_Efitadd);        m_fChain->SetBranchStatus("Efitadd",brs);  
     m_fChain->SetBranchAddress("Tfitadd", Tfitadd, &b_Tfitadd);        m_fChain->SetBranchStatus("Tfitadd",brs);  
     m_fChain->SetBranchAddress("Pedfitadd", Pedfitadd, &b_Pedfitadd);  m_fChain->SetBranchStatus("Pedfitadd",brs);
     m_fChain->SetBranchAddress("Chi2add", Chi2add, &b_Chi2add);        m_fChain->SetBranchStatus("Chi2add",brs); 
     m_fChain->SetBranchAddress("Negstat", Negstat, &b_Negstat);        m_fChain->SetBranchStatus("Negstat",brs);  
     m_fChain->SetBranchAddress("Posstat", Posstat, &b_Posstat);        m_fChain->SetBranchStatus("Posstat",brs); 
   } else {
     m_fChain->SetBranchAddress("status", status, &b_status);           m_fChain->SetBranchStatus("status",brs);
   }
   m_fChain->SetBranchAddress("cispar", cispar, &b_cispar);           m_fChain->SetBranchStatus("cispar",brs);
   m_fChain->SetBranchAddress("S1cou", &S1cou, &b_S1cou);             m_fChain->SetBranchStatus("S1cou",brs); 
   m_fChain->SetBranchAddress("S2cou", &S2cou, &b_S2cou);             m_fChain->SetBranchStatus("S2cou",brs); 
   m_fChain->SetBranchAddress("S3cou", &S3cou, &b_S3cou);             m_fChain->SetBranchStatus("S3cou",brs); 
   m_fChain->SetBranchAddress("S4cou", &S4cou, &b_S4cou);             m_fChain->SetBranchStatus("S4cou",brs); 
   m_fChain->SetBranchAddress("Cher1", &Cher1, &b_Cher1);             m_fChain->SetBranchStatus("Cher1",brs); 
   m_fChain->SetBranchAddress("Cher2", &Cher2, &b_Cher2);             m_fChain->SetBranchStatus("Cher2",brs); 
   m_fChain->SetBranchAddress("Beamextra", Beamextra, &b_Beamextra);  m_fChain->SetBranchStatus("Beamextra",brs);
   m_fChain->SetBranchAddress("Xcha1", &Xcha1, &b_Xcha1);             m_fChain->SetBranchStatus("Xcha1",brs); 
   m_fChain->SetBranchAddress("Ycha1", &Ycha1, &b_Ycha1);             m_fChain->SetBranchStatus("Ycha1",brs); 
   m_fChain->SetBranchAddress("btdc1", btdc1, &b_btdc1);              m_fChain->SetBranchStatus("btdc1",brs); 
   m_fChain->SetBranchAddress("Xcha2", &Xcha2, &b_Xcha2);             m_fChain->SetBranchStatus("Xcha2",brs); 
   m_fChain->SetBranchAddress("Ycha2", &Ycha2, &b_Ycha2);             m_fChain->SetBranchStatus("Ycha2",brs); 
   m_fChain->SetBranchAddress("btdc2", btdc2, &b_btdc2);              m_fChain->SetBranchStatus("btdc2",brs); 
   m_fChain->SetBranchAddress("Ximp", &Ximp, &b_Ximp);                m_fChain->SetBranchStatus("Ximp",brs);  
   m_fChain->SetBranchAddress("Yimp", &Yimp, &b_Yimp);                m_fChain->SetBranchStatus("Yimp",brs);  

   if (bigain) { // bigain run

     m_fChain->SetBranchAddress("Evtn0lo", &Evtn0lo, &b_Evt[0][0]);               m_fChain->SetBranchStatus("Evtn0lo",brs);     
     m_fChain->SetBranchAddress("Bcidn0lo", &Bcidn0lo, &b_Bcid[0][0]);            m_fChain->SetBranchStatus("Bcidn0lo",brs);    
     m_fChain->SetBranchAddress("Sizen0lo", &Sizen0lo, &b_Size[0][0]);            m_fChain->SetBranchStatus("Sizen0lo",brs);    
     m_fChain->SetBranchAddress("Dmumaskn0lo", Dmumaskn0lo, &b_Dmumask[0][0]);    m_fChain->SetBranchStatus("Dmumaskn0lo",brs); 
     m_fChain->SetBranchAddress("Slinkcrcn0lo", Slinkcrcn0lo, &b_Slinkcrc[0][0]); m_fChain->SetBranchStatus("Slinkcrcn0lo",brs);
     m_fChain->SetBranchAddress("Gainn0lo", Gainn0lo, &b_Gain[0][0]);             m_fChain->SetBranchStatus("Gainn0lo",brs);    
     m_fChain->SetBranchAddress("Errn0lo", Errn0lo, &b_Err[0][0]);                m_fChain->SetBranchStatus("Errn0lo",brs);     
     m_fChain->SetBranchAddress("Samplen0lo", Samplen0lo, &b_Sample[0][0]);       m_fChain->SetBranchStatus("Samplen0lo",brs);  
     m_fChain->SetBranchAddress("Enen0lo", Enen0lo, &b_Ene[0][0]);                m_fChain->SetBranchStatus("Enen0lo",brs);     
     m_fChain->SetBranchAddress("Timen0lo", Timen0lo, &b_Time[0][0]);             m_fChain->SetBranchStatus("Timen0lo",brs);    
     m_fChain->SetBranchAddress("Efitn0lo", Efitn0lo, &b_Efit[0][0]);             m_fChain->SetBranchStatus("Efitn0lo",brs);    
     m_fChain->SetBranchAddress("Tfitn0lo", Tfitn0lo, &b_Tfit[0][0]);             m_fChain->SetBranchStatus("Tfitn0lo",brs);    
     m_fChain->SetBranchAddress("Pedfitn0lo", Pedfitn0lo, &b_Pedfit[0][0]);       m_fChain->SetBranchStatus("Pedfitn0lo",brs);  
     m_fChain->SetBranchAddress("Chi2n0lo", Chi2n0lo, &b_Chi2[0][0]);             m_fChain->SetBranchStatus("Chi2n0lo",brs);    
                                                                                                                      
     m_fChain->SetBranchAddress("Evtn1lo", &Evtn1lo, &b_Evt[0][1]);               m_fChain->SetBranchStatus("Evtn1lo",brs);     
     m_fChain->SetBranchAddress("Bcidn1lo", &Bcidn1lo, &b_Bcid[0][1]);            m_fChain->SetBranchStatus("Bcidn1lo",brs);    
     m_fChain->SetBranchAddress("Sizen1lo", &Sizen1lo, &b_Size[0][1]);            m_fChain->SetBranchStatus("Sizen1lo",brs);    
     m_fChain->SetBranchAddress("Dmumaskn1lo", Dmumaskn1lo, &b_Dmumask[0][1]);    m_fChain->SetBranchStatus("Dmumaskn1lo",brs); 
     m_fChain->SetBranchAddress("Slinkcrcn1lo", Slinkcrcn1lo, &b_Slinkcrc[0][1]); m_fChain->SetBranchStatus("Slinkcrcn1lo",brs);
     m_fChain->SetBranchAddress("Gainn1lo", Gainn1lo, &b_Gain[0][1]);             m_fChain->SetBranchStatus("Gainn1lo",brs);    
     m_fChain->SetBranchAddress("Errn1lo", Errn1lo, &b_Err[0][1]);                m_fChain->SetBranchStatus("Errn1lo",brs);     
     m_fChain->SetBranchAddress("Samplen1lo", Samplen1lo, &b_Sample[0][1]);       m_fChain->SetBranchStatus("Samplen1lo",brs);  
     m_fChain->SetBranchAddress("Enen1lo", Enen1lo, &b_Ene[0][1]);                m_fChain->SetBranchStatus("Enen1lo",brs);     
     m_fChain->SetBranchAddress("Timen1lo", Timen1lo, &b_Time[0][1]);             m_fChain->SetBranchStatus("Timen1lo",brs);    
     m_fChain->SetBranchAddress("Efitn1lo", Efitn1lo, &b_Efit[0][1]);             m_fChain->SetBranchStatus("Efitn1lo",brs);    
     m_fChain->SetBranchAddress("Tfitn1lo", Tfitn1lo, &b_Tfit[0][1]);             m_fChain->SetBranchStatus("Tfitn1lo",brs);    
     m_fChain->SetBranchAddress("Pedfitn1lo", Pedfitn1lo, &b_Pedfit[0][1]);       m_fChain->SetBranchStatus("Pedfitn1lo",brs);  
     m_fChain->SetBranchAddress("Chi2n1lo", Chi2n1lo, &b_Chi2[0][1]);             m_fChain->SetBranchStatus("Chi2n1lo",brs);    
                                                                                                                        
     m_fChain->SetBranchAddress("Evtn2lo", &Evtn2lo, &b_Evt[0][2]);               m_fChain->SetBranchStatus("Evtn2lo",brs);     
     m_fChain->SetBranchAddress("Bcidn2lo", &Bcidn2lo, &b_Bcid[0][2]);            m_fChain->SetBranchStatus("Bcidn2lo",brs);    
     m_fChain->SetBranchAddress("Sizen2lo", &Sizen2lo, &b_Size[0][2]);            m_fChain->SetBranchStatus("Sizen2lo",brs);    
     m_fChain->SetBranchAddress("Dmumaskn2lo", Dmumaskn2lo, &b_Dmumask[0][2]);    m_fChain->SetBranchStatus("Dmumaskn2lo",brs); 
     m_fChain->SetBranchAddress("Slinkcrcn2lo", Slinkcrcn2lo, &b_Slinkcrc[0][2]); m_fChain->SetBranchStatus("Slinkcrcn2lo",brs);
     m_fChain->SetBranchAddress("Gainn2lo", Gainn2lo, &b_Gain[0][2]);             m_fChain->SetBranchStatus("Gainn2lo",brs);    
     m_fChain->SetBranchAddress("Errn2lo", Errn2lo, &b_Err[0][2]);                m_fChain->SetBranchStatus("Errn2lo",brs);     
     m_fChain->SetBranchAddress("Samplen2lo", Samplen2lo, &b_Sample[0][2]);       m_fChain->SetBranchStatus("Samplen2lo",brs);  
     m_fChain->SetBranchAddress("Enen2lo", Enen2lo, &b_Ene[0][2]);                m_fChain->SetBranchStatus("Enen2lo",brs);     
     m_fChain->SetBranchAddress("Timen2lo", Timen2lo, &b_Time[0][2]);             m_fChain->SetBranchStatus("Timen2lo",brs);    
     m_fChain->SetBranchAddress("Efitn2lo", Efitn2lo, &b_Efit[0][2]);             m_fChain->SetBranchStatus("Efitn2lo",brs);    
     m_fChain->SetBranchAddress("Tfitn2lo", Tfitn2lo, &b_Tfit[0][2]);             m_fChain->SetBranchStatus("Tfitn2lo",brs);    
     m_fChain->SetBranchAddress("Pedfitn2lo", Pedfitn2lo, &b_Pedfit[0][2]);       m_fChain->SetBranchStatus("Pedfitn2lo",brs);  
     m_fChain->SetBranchAddress("Chi2n2lo", Chi2n2lo, &b_Chi2[0][2]);             m_fChain->SetBranchStatus("Chi2n2lo",brs);    
                                                                                                                        
     m_fChain->SetBranchAddress("Evtp0lo", &Evtp0lo, &b_Evt[0][3]);               m_fChain->SetBranchStatus("Evtp0lo",brs);     
     m_fChain->SetBranchAddress("Bcidp0lo", &Bcidp0lo, &b_Bcid[0][3]);            m_fChain->SetBranchStatus("Bcidp0lo",brs);    
     m_fChain->SetBranchAddress("Sizep0lo", &Sizep0lo, &b_Size[0][3]);            m_fChain->SetBranchStatus("Sizep0lo",brs);    
     m_fChain->SetBranchAddress("Dmumaskp0lo", Dmumaskp0lo, &b_Dmumask[0][3]);    m_fChain->SetBranchStatus("Dmumaskp0lo",brs); 
     m_fChain->SetBranchAddress("Slinkcrcp0lo", Slinkcrcp0lo, &b_Slinkcrc[0][3]); m_fChain->SetBranchStatus("Slinkcrcp0lo",brs);
     m_fChain->SetBranchAddress("Gainp0lo", Gainp0lo, &b_Gain[0][3]);             m_fChain->SetBranchStatus("Gainp0lo",brs);    
     m_fChain->SetBranchAddress("Errp0lo", Errp0lo, &b_Err[0][3]);                m_fChain->SetBranchStatus("Errp0lo",brs);     
     m_fChain->SetBranchAddress("Samplep0lo", Samplep0lo, &b_Sample[0][3]);       m_fChain->SetBranchStatus("Samplep0lo",brs);  
     m_fChain->SetBranchAddress("Enep0lo", Enep0lo, &b_Ene[0][3]);                m_fChain->SetBranchStatus("Enep0lo",brs);     
     m_fChain->SetBranchAddress("Timep0lo", Timep0lo, &b_Time[0][3]);             m_fChain->SetBranchStatus("Timep0lo",brs);    
     m_fChain->SetBranchAddress("Efitp0lo", Efitp0lo, &b_Efit[0][3]);             m_fChain->SetBranchStatus("Efitp0lo",brs);    
     m_fChain->SetBranchAddress("Tfitp0lo", Tfitp0lo, &b_Tfit[0][3]);             m_fChain->SetBranchStatus("Tfitp0lo",brs);    
     m_fChain->SetBranchAddress("Pedfitp0lo", Pedfitp0lo, &b_Pedfit[0][3]);       m_fChain->SetBranchStatus("Pedfitp0lo",brs);  
     m_fChain->SetBranchAddress("Chi2p0lo", Chi2p0lo, &b_Chi2[0][3]);             m_fChain->SetBranchStatus("Chi2p0lo",brs);    
                                                                                                                        
     m_fChain->SetBranchAddress("Evtp1lo", &Evtp1lo, &b_Evt[0][4]);               m_fChain->SetBranchStatus("Evtp1lo",brs);     
     m_fChain->SetBranchAddress("Bcidp1lo", &Bcidp1lo, &b_Bcid[0][4]);            m_fChain->SetBranchStatus("Bcidp1lo",brs);    
     m_fChain->SetBranchAddress("Sizep1lo", &Sizep1lo, &b_Size[0][4]);            m_fChain->SetBranchStatus("Sizep1lo",brs);    
     m_fChain->SetBranchAddress("Dmumaskp1lo", Dmumaskp1lo, &b_Dmumask[0][4]);    m_fChain->SetBranchStatus("Dmumaskp1lo",brs); 
     m_fChain->SetBranchAddress("Slinkcrcp1lo", Slinkcrcp1lo, &b_Slinkcrc[0][4]); m_fChain->SetBranchStatus("Slinkcrcp1lo",brs);
     m_fChain->SetBranchAddress("Gainp1lo", Gainp1lo, &b_Gain[0][4]);             m_fChain->SetBranchStatus("Gainp1lo",brs);    
     m_fChain->SetBranchAddress("Errp1lo", Errp1lo, &b_Err[0][4]);                m_fChain->SetBranchStatus("Errp1lo",brs);     
     m_fChain->SetBranchAddress("Samplep1lo", Samplep1lo, &b_Sample[0][4]);       m_fChain->SetBranchStatus("Samplep1lo",brs);  
     m_fChain->SetBranchAddress("Enep1lo", Enep1lo, &b_Ene[0][4]);                m_fChain->SetBranchStatus("Enep1lo",brs);     
     m_fChain->SetBranchAddress("Timep1lo", Timep1lo, &b_Time[0][4]);             m_fChain->SetBranchStatus("Timep1lo",brs);    
     m_fChain->SetBranchAddress("Efitp1lo", Efitp1lo, &b_Efit[0][4]);             m_fChain->SetBranchStatus("Efitp1lo",brs);    
     m_fChain->SetBranchAddress("Tfitp1lo", Tfitp1lo, &b_Tfit[0][4]);             m_fChain->SetBranchStatus("Tfitp1lo",brs);    
     m_fChain->SetBranchAddress("Pedfitp1lo", Pedfitp1lo, &b_Pedfit[0][4]);       m_fChain->SetBranchStatus("Pedfitp1lo",brs);  
     m_fChain->SetBranchAddress("Chi2p1lo", Chi2p1lo, &b_Chi2[0][4]);             m_fChain->SetBranchStatus("Chi2p1lo",brs);    
                                                                                                                        
     m_fChain->SetBranchAddress("Evtp2lo", &Evtp2lo, &b_Evt[0][5]);               m_fChain->SetBranchStatus("Evtp2lo",brs);     
     m_fChain->SetBranchAddress("Bcidp2lo", &Bcidp2lo, &b_Bcid[0][5]);            m_fChain->SetBranchStatus("Bcidp2lo",brs);    
     m_fChain->SetBranchAddress("Sizep2lo", &Sizep2lo, &b_Size[0][5]);            m_fChain->SetBranchStatus("Sizep2lo",brs);    
     m_fChain->SetBranchAddress("Dmumaskp2lo", Dmumaskp2lo, &b_Dmumask[0][5]);    m_fChain->SetBranchStatus("Dmumaskp2lo",brs); 
     m_fChain->SetBranchAddress("Slinkcrcp2lo", Slinkcrcp2lo, &b_Slinkcrc[0][5]); m_fChain->SetBranchStatus("Slinkcrcp2lo",brs);
     m_fChain->SetBranchAddress("Gainp2lo", Gainp2lo, &b_Gain[0][5]);             m_fChain->SetBranchStatus("Gainp2lo",brs);    
     m_fChain->SetBranchAddress("Errp2lo", Errp2lo, &b_Err[0][5]);                m_fChain->SetBranchStatus("Errp2lo",brs);     
     m_fChain->SetBranchAddress("Samplep2lo", Samplep2lo, &b_Sample[0][5]);       m_fChain->SetBranchStatus("Samplep2lo",brs);  
     m_fChain->SetBranchAddress("Enep2lo", Enep2lo, &b_Ene[0][5]);                m_fChain->SetBranchStatus("Enep2lo",brs);     
     m_fChain->SetBranchAddress("Timep2lo", Timep2lo, &b_Time[0][5]);             m_fChain->SetBranchStatus("Timep2lo",brs);    
     m_fChain->SetBranchAddress("Efitp2lo", Efitp2lo, &b_Efit[0][5]);             m_fChain->SetBranchStatus("Efitp2lo",brs);    
     m_fChain->SetBranchAddress("Tfitp2lo", Tfitp2lo, &b_Tfit[0][5]);             m_fChain->SetBranchStatus("Tfitp2lo",brs);    
     m_fChain->SetBranchAddress("Pedfitp2lo", Pedfitp2lo, &b_Pedfit[0][5]);       m_fChain->SetBranchStatus("Pedfitp2lo",brs);  
     m_fChain->SetBranchAddress("Chi2p2lo", Chi2p2lo, &b_Chi2[0][5]);             m_fChain->SetBranchStatus("Chi2p2lo",brs);    
                                                                                                                        
     m_fChain->SetBranchAddress("Evtn0hi", &Evtn0hi, &b_Evt[1][0]);               m_fChain->SetBranchStatus("Evtn0hi",brs);     
     m_fChain->SetBranchAddress("Bcidn0hi", &Bcidn0hi, &b_Bcid[1][0]);            m_fChain->SetBranchStatus("Bcidn0hi",brs);    
     m_fChain->SetBranchAddress("Sizen0hi", &Sizen0hi, &b_Size[1][0]);            m_fChain->SetBranchStatus("Sizen0hi",brs);    
     m_fChain->SetBranchAddress("Dmumaskn0hi", Dmumaskn0hi, &b_Dmumask[1][0]);    m_fChain->SetBranchStatus("Dmumaskn0hi",brs); 
     m_fChain->SetBranchAddress("Slinkcrcn0hi", Slinkcrcn0hi, &b_Slinkcrc[1][0]); m_fChain->SetBranchStatus("Slinkcrcn0hi",brs);
     m_fChain->SetBranchAddress("Gainn0hi", Gainn0hi, &b_Gain[1][0]);             m_fChain->SetBranchStatus("Gainn0hi",brs);    
     m_fChain->SetBranchAddress("Errn0hi", Errn0hi, &b_Err[1][0]);                m_fChain->SetBranchStatus("Errn0hi",brs);     
     m_fChain->SetBranchAddress("Samplen0hi", Samplen0hi, &b_Sample[1][0]);       m_fChain->SetBranchStatus("Samplen0hi",brs);  
     m_fChain->SetBranchAddress("Enen0hi", Enen0hi, &b_Ene[1][0]);                m_fChain->SetBranchStatus("Enen0hi",brs);     
     m_fChain->SetBranchAddress("Timen0hi", Timen0hi, &b_Time[1][0]);             m_fChain->SetBranchStatus("Timen0hi",brs);    
     m_fChain->SetBranchAddress("Efitn0hi", Efitn0hi, &b_Efit[1][0]);             m_fChain->SetBranchStatus("Efitn0hi",brs);    
     m_fChain->SetBranchAddress("Tfitn0hi", Tfitn0hi, &b_Tfit[1][0]);             m_fChain->SetBranchStatus("Tfitn0hi",brs);    
     m_fChain->SetBranchAddress("Pedfitn0hi", Pedfitn0hi, &b_Pedfit[1][0]);       m_fChain->SetBranchStatus("Pedfitn0hi",brs);  
     m_fChain->SetBranchAddress("Chi2n0hi", Chi2n0hi, &b_Chi2[1][0]);             m_fChain->SetBranchStatus("Chi2n0hi",brs);    
                                                                                                                        
     m_fChain->SetBranchAddress("Evtn1hi", &Evtn1hi, &b_Evt[1][1]);               m_fChain->SetBranchStatus("Evtn1hi",brs);     
     m_fChain->SetBranchAddress("Bcidn1hi", &Bcidn1hi, &b_Bcid[1][1]);            m_fChain->SetBranchStatus("Bcidn1hi",brs);    
     m_fChain->SetBranchAddress("Sizen1hi", &Sizen1hi, &b_Size[1][1]);            m_fChain->SetBranchStatus("Sizen1hi",brs);    
     m_fChain->SetBranchAddress("Dmumaskn1hi", Dmumaskn1hi, &b_Dmumask[1][1]);    m_fChain->SetBranchStatus("Dmumaskn1hi",brs); 
     m_fChain->SetBranchAddress("Slinkcrcn1hi", Slinkcrcn1hi, &b_Slinkcrc[1][1]); m_fChain->SetBranchStatus("Slinkcrcn1hi",brs);
     m_fChain->SetBranchAddress("Gainn1hi", Gainn1hi, &b_Gain[1][1]);             m_fChain->SetBranchStatus("Gainn1hi",brs);    
     m_fChain->SetBranchAddress("Errn1hi", Errn1hi, &b_Err[1][1]);                m_fChain->SetBranchStatus("Errn1hi",brs);     
     m_fChain->SetBranchAddress("Samplen1hi", Samplen1hi, &b_Sample[1][1]);       m_fChain->SetBranchStatus("Samplen1hi",brs);  
     m_fChain->SetBranchAddress("Enen1hi", Enen1hi, &b_Ene[1][1]);                m_fChain->SetBranchStatus("Enen1hi",brs);     
     m_fChain->SetBranchAddress("Timen1hi", Timen1hi, &b_Time[1][1]);             m_fChain->SetBranchStatus("Timen1hi",brs);    
     m_fChain->SetBranchAddress("Efitn1hi", Efitn1hi, &b_Efit[1][1]);             m_fChain->SetBranchStatus("Efitn1hi",brs);    
     m_fChain->SetBranchAddress("Tfitn1hi", Tfitn1hi, &b_Tfit[1][1]);             m_fChain->SetBranchStatus("Tfitn1hi",brs);    
     m_fChain->SetBranchAddress("Pedfitn1hi", Pedfitn1hi, &b_Pedfit[1][1]);       m_fChain->SetBranchStatus("Pedfitn1hi",brs);  
     m_fChain->SetBranchAddress("Chi2n1hi", Chi2n1hi, &b_Chi2[1][1]);             m_fChain->SetBranchStatus("Chi2n1hi",brs);    
                                                                                                                        
     m_fChain->SetBranchAddress("Evtn2hi", &Evtn2hi, &b_Evt[1][2]);               m_fChain->SetBranchStatus("Evtn2hi",brs);     
     m_fChain->SetBranchAddress("Bcidn2hi", &Bcidn2hi, &b_Bcid[1][2]);            m_fChain->SetBranchStatus("Bcidn2hi",brs);    
     m_fChain->SetBranchAddress("Sizen2hi", &Sizen2hi, &b_Size[1][2]);            m_fChain->SetBranchStatus("Sizen2hi",brs);    
     m_fChain->SetBranchAddress("Dmumaskn2hi", Dmumaskn2hi, &b_Dmumask[1][2]);    m_fChain->SetBranchStatus("Dmumaskn2hi",brs); 
     m_fChain->SetBranchAddress("Slinkcrcn2hi", Slinkcrcn2hi, &b_Slinkcrc[1][2]); m_fChain->SetBranchStatus("Slinkcrcn2hi",brs);
     m_fChain->SetBranchAddress("Gainn2hi", Gainn2hi, &b_Gain[1][2]);             m_fChain->SetBranchStatus("Gainn2hi",brs);    
     m_fChain->SetBranchAddress("Errn2hi", Errn2hi, &b_Err[1][2]);                m_fChain->SetBranchStatus("Errn2hi",brs);     
     m_fChain->SetBranchAddress("Samplen2hi", Samplen2hi, &b_Sample[1][2]);       m_fChain->SetBranchStatus("Samplen2hi",brs);  
     m_fChain->SetBranchAddress("Enen2hi", Enen2hi, &b_Ene[1][2]);                m_fChain->SetBranchStatus("Enen2hi",brs);     
     m_fChain->SetBranchAddress("Timen2hi", Timen2hi, &b_Time[1][2]);             m_fChain->SetBranchStatus("Timen2hi",brs);    
     m_fChain->SetBranchAddress("Efitn2hi", Efitn2hi, &b_Efit[1][2]);             m_fChain->SetBranchStatus("Efitn2hi",brs);    
     m_fChain->SetBranchAddress("Tfitn2hi", Tfitn2hi, &b_Tfit[1][2]);             m_fChain->SetBranchStatus("Tfitn2hi",brs);    
     m_fChain->SetBranchAddress("Pedfitn2hi", Pedfitn2hi, &b_Pedfit[1][2]);       m_fChain->SetBranchStatus("Pedfitn2hi",brs);  
     m_fChain->SetBranchAddress("Chi2n2hi", Chi2n2hi, &b_Chi2[1][2]);             m_fChain->SetBranchStatus("Chi2n2hi",brs);    
                                                                                                                        
     m_fChain->SetBranchAddress("Evtp0hi", &Evtp0hi, &b_Evt[1][3]);               m_fChain->SetBranchStatus("Evtp0hi",brs);     
     m_fChain->SetBranchAddress("Bcidp0hi", &Bcidp0hi, &b_Bcid[1][3]);            m_fChain->SetBranchStatus("Bcidp0hi",brs);    
     m_fChain->SetBranchAddress("Sizep0hi", &Sizep0hi, &b_Size[1][3]);            m_fChain->SetBranchStatus("Sizep0hi",brs);    
     m_fChain->SetBranchAddress("Dmumaskp0hi", Dmumaskp0hi, &b_Dmumask[1][3]);    m_fChain->SetBranchStatus("Dmumaskp0hi",brs); 
     m_fChain->SetBranchAddress("Slinkcrcp0hi", Slinkcrcp0hi, &b_Slinkcrc[1][3]); m_fChain->SetBranchStatus("Slinkcrcp0hi",brs);
     m_fChain->SetBranchAddress("Gainp0hi", Gainp0hi, &b_Gain[1][3]);             m_fChain->SetBranchStatus("Gainp0hi",brs);    
     m_fChain->SetBranchAddress("Errp0hi", Errp0hi, &b_Err[1][3]);                m_fChain->SetBranchStatus("Errp0hi",brs);     
     m_fChain->SetBranchAddress("Samplep0hi", Samplep0hi, &b_Sample[1][3]);       m_fChain->SetBranchStatus("Samplep0hi",brs);  
     m_fChain->SetBranchAddress("Enep0hi", Enep0hi, &b_Ene[1][3]);                m_fChain->SetBranchStatus("Enep0hi",brs);     
     m_fChain->SetBranchAddress("Timep0hi", Timep0hi, &b_Time[1][3]);             m_fChain->SetBranchStatus("Timep0hi",brs);    
     m_fChain->SetBranchAddress("Efitp0hi", Efitp0hi, &b_Efit[1][3]);             m_fChain->SetBranchStatus("Efitp0hi",brs);    
     m_fChain->SetBranchAddress("Tfitp0hi", Tfitp0hi, &b_Tfit[1][3]);             m_fChain->SetBranchStatus("Tfitp0hi",brs);    
     m_fChain->SetBranchAddress("Pedfitp0hi", Pedfitp0hi, &b_Pedfit[1][3]);       m_fChain->SetBranchStatus("Pedfitp0hi",brs);  
     m_fChain->SetBranchAddress("Chi2p0hi", Chi2p0hi, &b_Chi2[1][3]);             m_fChain->SetBranchStatus("Chi2p0hi",brs);    
                                                                                                                        
     m_fChain->SetBranchAddress("Evtp1hi", &Evtp1hi, &b_Evt[1][4]);               m_fChain->SetBranchStatus("Evtp1hi",brs);     
     m_fChain->SetBranchAddress("Bcidp1hi", &Bcidp1hi, &b_Bcid[1][4]);            m_fChain->SetBranchStatus("Bcidp1hi",brs);    
     m_fChain->SetBranchAddress("Sizep1hi", &Sizep1hi, &b_Size[1][4]);            m_fChain->SetBranchStatus("Sizep1hi",brs);    
     m_fChain->SetBranchAddress("Dmumaskp1hi", Dmumaskp1hi, &b_Dmumask[1][4]);    m_fChain->SetBranchStatus("Dmumaskp1hi",brs); 
     m_fChain->SetBranchAddress("Slinkcrcp1hi", Slinkcrcp1hi, &b_Slinkcrc[1][4]); m_fChain->SetBranchStatus("Slinkcrcp1hi",brs);
     m_fChain->SetBranchAddress("Gainp1hi", Gainp1hi, &b_Gain[1][4]);             m_fChain->SetBranchStatus("Gainp1hi",brs);    
     m_fChain->SetBranchAddress("Errp1hi", Errp1hi, &b_Err[1][4]);                m_fChain->SetBranchStatus("Errp1hi",brs);     
     m_fChain->SetBranchAddress("Samplep1hi", Samplep1hi, &b_Sample[1][4]);       m_fChain->SetBranchStatus("Samplep1hi",brs);  
     m_fChain->SetBranchAddress("Enep1hi", Enep1hi, &b_Ene[1][4]);                m_fChain->SetBranchStatus("Enep1hi",brs);     
     m_fChain->SetBranchAddress("Timep1hi", Timep1hi, &b_Time[1][4]);             m_fChain->SetBranchStatus("Timep1hi",brs);    
     m_fChain->SetBranchAddress("Efitp1hi", Efitp1hi, &b_Efit[1][4]);             m_fChain->SetBranchStatus("Efitp1hi",brs);    
     m_fChain->SetBranchAddress("Tfitp1hi", Tfitp1hi, &b_Tfit[1][4]);             m_fChain->SetBranchStatus("Tfitp1hi",brs);    
     m_fChain->SetBranchAddress("Pedfitp1hi", Pedfitp1hi, &b_Pedfit[1][4]);       m_fChain->SetBranchStatus("Pedfitp1hi",brs);  
     m_fChain->SetBranchAddress("Chi2p1hi", Chi2p1hi, &b_Chi2[1][4]);             m_fChain->SetBranchStatus("Chi2p1hi",brs);    
                                                                                                                        
     m_fChain->SetBranchAddress("Evtp2hi", &Evtp2hi, &b_Evt[1][5]);               m_fChain->SetBranchStatus("Evtp2hi",brs);     
     m_fChain->SetBranchAddress("Bcidp2hi", &Bcidp2hi, &b_Bcid[1][5]);            m_fChain->SetBranchStatus("Bcidp2hi",brs);    
     m_fChain->SetBranchAddress("Sizep2hi", &Sizep2hi, &b_Size[1][5]);            m_fChain->SetBranchStatus("Sizep2hi",brs);    
     m_fChain->SetBranchAddress("Dmumaskp2hi", Dmumaskp2hi, &b_Dmumask[1][5]);    m_fChain->SetBranchStatus("Dmumaskp2hi",brs); 
     m_fChain->SetBranchAddress("Slinkcrcp2hi", Slinkcrcp2hi, &b_Slinkcrc[1][5]); m_fChain->SetBranchStatus("Slinkcrcp2hi",brs);
     m_fChain->SetBranchAddress("Gainp2hi", Gainp2hi, &b_Gain[1][5]);             m_fChain->SetBranchStatus("Gainp2hi",brs);    
     m_fChain->SetBranchAddress("Errp2hi", Errp2hi, &b_Err[1][5]);                m_fChain->SetBranchStatus("Errp2hi",brs);     
     m_fChain->SetBranchAddress("Samplep2hi", Samplep2hi, &b_Sample[1][5]);       m_fChain->SetBranchStatus("Samplep2hi",brs);  
     m_fChain->SetBranchAddress("Enep2hi", Enep2hi, &b_Ene[1][5]);                m_fChain->SetBranchStatus("Enep2hi",brs);     
     m_fChain->SetBranchAddress("Timep2hi", Timep2hi, &b_Time[1][5]);             m_fChain->SetBranchStatus("Timep2hi",brs);    
     m_fChain->SetBranchAddress("Efitp2hi", Efitp2hi, &b_Efit[1][5]);             m_fChain->SetBranchStatus("Efitp2hi",brs);    
     m_fChain->SetBranchAddress("Tfitp2hi", Tfitp2hi, &b_Tfit[1][5]);             m_fChain->SetBranchStatus("Tfitp2hi",brs);    
     m_fChain->SetBranchAddress("Pedfitp2hi", Pedfitp2hi, &b_Pedfit[1][5]);       m_fChain->SetBranchStatus("Pedfitp2hi",brs);  
     m_fChain->SetBranchAddress("Chi2p2hi", Chi2p2hi, &b_Chi2[1][5]);             m_fChain->SetBranchStatus("Chi2p2hi",brs);    

   } else { //monogain run

     m_fChain->SetBranchAddress("Evtn0", &Evtn0lo, &b_Evt[0][0]);               m_fChain->SetBranchStatus("Evtn0",brs);     
     m_fChain->SetBranchAddress("Bcidn0", &Bcidn0lo, &b_Bcid[0][0]);            m_fChain->SetBranchStatus("Bcidn0",brs);    
     m_fChain->SetBranchAddress("Sizen0", &Sizen0lo, &b_Size[0][0]);            m_fChain->SetBranchStatus("Sizen0",brs);    
     m_fChain->SetBranchAddress("Dmumaskn0", Dmumaskn0lo, &b_Dmumask[0][0]);    m_fChain->SetBranchStatus("Dmumaskn0",brs); 
     m_fChain->SetBranchAddress("Slinkcrcn0", Slinkcrcn0lo, &b_Slinkcrc[0][0]); m_fChain->SetBranchStatus("Slinkcrcn0",brs);
     m_fChain->SetBranchAddress("Gainn0", Gainn0lo, &b_Gain[0][0]);             m_fChain->SetBranchStatus("Gainn0",brs);    
     m_fChain->SetBranchAddress("Errn0", Errn0lo, &b_Err[0][0]);                m_fChain->SetBranchStatus("Errn0",brs);     
     m_fChain->SetBranchAddress("Samplen0", Samplen0lo, &b_Sample[0][0]);       m_fChain->SetBranchStatus("Samplen0",brs);  
     m_fChain->SetBranchAddress("Enen0", Enen0lo, &b_Ene[0][0]);                m_fChain->SetBranchStatus("Enen0",brs);     
     m_fChain->SetBranchAddress("Timen0", Timen0lo, &b_Time[0][0]);             m_fChain->SetBranchStatus("Timen0",brs);    
     m_fChain->SetBranchAddress("Efitn0", Efitn0lo, &b_Efit[0][0]);             m_fChain->SetBranchStatus("Efitn0",brs);    
     m_fChain->SetBranchAddress("Tfitn0", Tfitn0lo, &b_Tfit[0][0]);             m_fChain->SetBranchStatus("Tfitn0",brs);    
     m_fChain->SetBranchAddress("Pedfitn0", Pedfitn0lo, &b_Pedfit[0][0]);       m_fChain->SetBranchStatus("Pedfitn0",brs);  
     m_fChain->SetBranchAddress("Chi2n0", Chi2n0lo, &b_Chi2[0][0]);             m_fChain->SetBranchStatus("Chi2n0",brs);    
                                                                                                                
     m_fChain->SetBranchAddress("Evtn1", &Evtn1lo, &b_Evt[0][1]);               m_fChain->SetBranchStatus("Evtn1",brs);     
     m_fChain->SetBranchAddress("Bcidn1", &Bcidn1lo, &b_Bcid[0][1]);            m_fChain->SetBranchStatus("Bcidn1",brs);    
     m_fChain->SetBranchAddress("Sizen1", &Sizen1lo, &b_Size[0][1]);            m_fChain->SetBranchStatus("Sizen1",brs);    
     m_fChain->SetBranchAddress("Dmumaskn1", Dmumaskn1lo, &b_Dmumask[0][1]);    m_fChain->SetBranchStatus("Dmumaskn1",brs); 
     m_fChain->SetBranchAddress("Slinkcrcn1", Slinkcrcn1lo, &b_Slinkcrc[0][1]); m_fChain->SetBranchStatus("Slinkcrcn1",brs);
     m_fChain->SetBranchAddress("Gainn1", Gainn1lo, &b_Gain[0][1]);             m_fChain->SetBranchStatus("Gainn1",brs);    
     m_fChain->SetBranchAddress("Errn1", Errn1lo, &b_Err[0][1]);                m_fChain->SetBranchStatus("Errn1",brs);     
     m_fChain->SetBranchAddress("Samplen1", Samplen1lo, &b_Sample[0][1]);       m_fChain->SetBranchStatus("Samplen1",brs);  
     m_fChain->SetBranchAddress("Enen1", Enen1lo, &b_Ene[0][1]);                m_fChain->SetBranchStatus("Enen1",brs);     
     m_fChain->SetBranchAddress("Timen1", Timen1lo, &b_Time[0][1]);             m_fChain->SetBranchStatus("Timen1",brs);    
     m_fChain->SetBranchAddress("Efitn1", Efitn1lo, &b_Efit[0][1]);             m_fChain->SetBranchStatus("Efitn1",brs);    
     m_fChain->SetBranchAddress("Tfitn1", Tfitn1lo, &b_Tfit[0][1]);             m_fChain->SetBranchStatus("Tfitn1",brs);    
     m_fChain->SetBranchAddress("Pedfitn1", Pedfitn1lo, &b_Pedfit[0][1]);       m_fChain->SetBranchStatus("Pedfitn1",brs);  
     m_fChain->SetBranchAddress("Chi2n1", Chi2n1lo, &b_Chi2[0][1]);             m_fChain->SetBranchStatus("Chi2n1",brs);    
                                                                                                                
     m_fChain->SetBranchAddress("Evtn2", &Evtn2lo, &b_Evt[0][2]);               m_fChain->SetBranchStatus("Evtn2",brs);     
     m_fChain->SetBranchAddress("Bcidn2", &Bcidn2lo, &b_Bcid[0][2]);            m_fChain->SetBranchStatus("Bcidn2",brs);    
     m_fChain->SetBranchAddress("Sizen2", &Sizen2lo, &b_Size[0][2]);            m_fChain->SetBranchStatus("Sizen2",brs);    
     m_fChain->SetBranchAddress("Dmumaskn2", Dmumaskn2lo, &b_Dmumask[0][2]);    m_fChain->SetBranchStatus("Dmumaskn2",brs); 
     m_fChain->SetBranchAddress("Slinkcrcn2", Slinkcrcn2lo, &b_Slinkcrc[0][2]); m_fChain->SetBranchStatus("Slinkcrcn2",brs);
     m_fChain->SetBranchAddress("Gainn2", Gainn2lo, &b_Gain[0][2]);             m_fChain->SetBranchStatus("Gainn2",brs);    
     m_fChain->SetBranchAddress("Errn2", Errn2lo, &b_Err[0][2]);                m_fChain->SetBranchStatus("Errn2",brs);     
     m_fChain->SetBranchAddress("Samplen2", Samplen2lo, &b_Sample[0][2]);       m_fChain->SetBranchStatus("Samplen2",brs);  
     m_fChain->SetBranchAddress("Enen2", Enen2lo, &b_Ene[0][2]);                m_fChain->SetBranchStatus("Enen2",brs);     
     m_fChain->SetBranchAddress("Timen2", Timen2lo, &b_Time[0][2]);             m_fChain->SetBranchStatus("Timen2",brs);    
     m_fChain->SetBranchAddress("Efitn2", Efitn2lo, &b_Efit[0][2]);             m_fChain->SetBranchStatus("Efitn2",brs);    
     m_fChain->SetBranchAddress("Tfitn2", Tfitn2lo, &b_Tfit[0][2]);             m_fChain->SetBranchStatus("Tfitn2",brs);    
     m_fChain->SetBranchAddress("Pedfitn2", Pedfitn2lo, &b_Pedfit[0][2]);       m_fChain->SetBranchStatus("Pedfitn2",brs);  
     m_fChain->SetBranchAddress("Chi2n2", Chi2n2lo, &b_Chi2[0][2]);             m_fChain->SetBranchStatus("Chi2n2",brs);    
                                                                                                                
     m_fChain->SetBranchAddress("Evtp0", &Evtp0lo, &b_Evt[0][3]);               m_fChain->SetBranchStatus("Evtp0",brs);     
     m_fChain->SetBranchAddress("Bcidp0", &Bcidp0lo, &b_Bcid[0][3]);            m_fChain->SetBranchStatus("Bcidp0",brs);    
     m_fChain->SetBranchAddress("Sizep0", &Sizep0lo, &b_Size[0][3]);            m_fChain->SetBranchStatus("Sizep0",brs);    
     m_fChain->SetBranchAddress("Dmumaskp0", Dmumaskp0lo, &b_Dmumask[0][3]);    m_fChain->SetBranchStatus("Dmumaskp0",brs); 
     m_fChain->SetBranchAddress("Slinkcrcp0", Slinkcrcp0lo, &b_Slinkcrc[0][3]); m_fChain->SetBranchStatus("Slinkcrcp0",brs);
     m_fChain->SetBranchAddress("Gainp0", Gainp0lo, &b_Gain[0][3]);             m_fChain->SetBranchStatus("Gainp0",brs);    
     m_fChain->SetBranchAddress("Errp0", Errp0lo, &b_Err[0][3]);                m_fChain->SetBranchStatus("Errp0",brs);     
     m_fChain->SetBranchAddress("Samplep0", Samplep0lo, &b_Sample[0][3]);       m_fChain->SetBranchStatus("Samplep0",brs);  
     m_fChain->SetBranchAddress("Enep0", Enep0lo, &b_Ene[0][3]);                m_fChain->SetBranchStatus("Enep0",brs);     
     m_fChain->SetBranchAddress("Timep0", Timep0lo, &b_Time[0][3]);             m_fChain->SetBranchStatus("Timep0",brs);    
     m_fChain->SetBranchAddress("Efitp0", Efitp0lo, &b_Efit[0][3]);             m_fChain->SetBranchStatus("Efitp0",brs);    
     m_fChain->SetBranchAddress("Tfitp0", Tfitp0lo, &b_Tfit[0][3]);             m_fChain->SetBranchStatus("Tfitp0",brs);    
     m_fChain->SetBranchAddress("Pedfitp0", Pedfitp0lo, &b_Pedfit[0][3]);       m_fChain->SetBranchStatus("Pedfitp0",brs);  
     m_fChain->SetBranchAddress("Chi2p0", Chi2p0lo, &b_Chi2[0][3]);             m_fChain->SetBranchStatus("Chi2p0",brs);    
                                                                                                                
     m_fChain->SetBranchAddress("Evtp1", &Evtp1lo, &b_Evt[0][4]);               m_fChain->SetBranchStatus("Evtp1",brs);     
     m_fChain->SetBranchAddress("Bcidp1", &Bcidp1lo, &b_Bcid[0][4]);            m_fChain->SetBranchStatus("Bcidp1",brs);    
     m_fChain->SetBranchAddress("Sizep1", &Sizep1lo, &b_Size[0][4]);            m_fChain->SetBranchStatus("Sizep1",brs);    
     m_fChain->SetBranchAddress("Dmumaskp1", Dmumaskp1lo, &b_Dmumask[0][4]);    m_fChain->SetBranchStatus("Dmumaskp1",brs); 
     m_fChain->SetBranchAddress("Slinkcrcp1", Slinkcrcp1lo, &b_Slinkcrc[0][4]); m_fChain->SetBranchStatus("Slinkcrcp1",brs);
     m_fChain->SetBranchAddress("Gainp1", Gainp1lo, &b_Gain[0][4]);             m_fChain->SetBranchStatus("Gainp1",brs);    
     m_fChain->SetBranchAddress("Errp1", Errp1lo, &b_Err[0][4]);                m_fChain->SetBranchStatus("Errp1",brs);     
     m_fChain->SetBranchAddress("Samplep1", Samplep1lo, &b_Sample[0][4]);       m_fChain->SetBranchStatus("Samplep1",brs);  
     m_fChain->SetBranchAddress("Enep1", Enep1lo, &b_Ene[0][4]);                m_fChain->SetBranchStatus("Enep1",brs);     
     m_fChain->SetBranchAddress("Timep1", Timep1lo, &b_Time[0][4]);             m_fChain->SetBranchStatus("Timep1",brs);    
     m_fChain->SetBranchAddress("Efitp1", Efitp1lo, &b_Efit[0][4]);             m_fChain->SetBranchStatus("Efitp1",brs);    
     m_fChain->SetBranchAddress("Tfitp1", Tfitp1lo, &b_Tfit[0][4]);             m_fChain->SetBranchStatus("Tfitp1",brs);    
     m_fChain->SetBranchAddress("Pedfitp1", Pedfitp1lo, &b_Pedfit[0][4]);       m_fChain->SetBranchStatus("Pedfitp1",brs);  
     m_fChain->SetBranchAddress("Chi2p1", Chi2p1lo, &b_Chi2[0][4]);             m_fChain->SetBranchStatus("Chi2p1",brs);    
                                                                                                                
     m_fChain->SetBranchAddress("Evtp2", &Evtp2lo, &b_Evt[0][5]);               m_fChain->SetBranchStatus("Evtp2",brs);     
     m_fChain->SetBranchAddress("Bcidp2", &Bcidp2lo, &b_Bcid[0][5]);            m_fChain->SetBranchStatus("Bcidp2",brs);    
     m_fChain->SetBranchAddress("Sizep2", &Sizep2lo, &b_Size[0][5]);            m_fChain->SetBranchStatus("Sizep2",brs);    
     m_fChain->SetBranchAddress("Dmumaskp2", Dmumaskp2lo, &b_Dmumask[0][5]);    m_fChain->SetBranchStatus("Dmumaskp2",brs); 
     m_fChain->SetBranchAddress("Slinkcrcp2", Slinkcrcp2lo, &b_Slinkcrc[0][5]); m_fChain->SetBranchStatus("Slinkcrcp2",brs);
     m_fChain->SetBranchAddress("Gainp2", Gainp2lo, &b_Gain[0][5]);             m_fChain->SetBranchStatus("Gainp2",brs);    
     m_fChain->SetBranchAddress("Errp2", Errp2lo, &b_Err[0][5]);                m_fChain->SetBranchStatus("Errp2",brs);     
     m_fChain->SetBranchAddress("Samplep2", Samplep2lo, &b_Sample[0][5]);       m_fChain->SetBranchStatus("Samplep2",brs);  
     m_fChain->SetBranchAddress("Enep2", Enep2lo, &b_Ene[0][5]);                m_fChain->SetBranchStatus("Enep2",brs);     
     m_fChain->SetBranchAddress("Timep2", Timep2lo, &b_Time[0][5]);             m_fChain->SetBranchStatus("Timep2",brs);    
     m_fChain->SetBranchAddress("Efitp2", Efitp2lo, &b_Efit[0][5]);             m_fChain->SetBranchStatus("Efitp2",brs);    
     m_fChain->SetBranchAddress("Tfitp2", Tfitp2lo, &b_Tfit[0][5]);             m_fChain->SetBranchStatus("Tfitp2",brs);    
     m_fChain->SetBranchAddress("Pedfitp2", Pedfitp2lo, &b_Pedfit[0][5]);       m_fChain->SetBranchStatus("Pedfitp2",brs);  
     m_fChain->SetBranchAddress("Chi2p2", Chi2p2lo, &b_Chi2[0][5]);             m_fChain->SetBranchStatus("Chi2p2",brs);    
   }
}

TileTBOldNtupleStruct::~TileTBOldNtupleStruct()
{
   if (!m_fChain) return;
   delete m_fChain->GetCurrentFile();
}

Int_t TileTBOldNtupleStruct::GetEntry(Long64_t entry, int branch)
{
// Read contents of entry.
   if (!m_fChain) return 0;

   if (branch > 6) return m_fChain->GetEntry(entry);  // read all branches with non-zero status at once, ignoring shift

   // read either set of branches for one drawer or all branches N0,N1,N2,P0,P1,P2,BEAMROD
   int brmin = ( (branch < 0) ? 0: branch );
   int brmax = ( (branch < 0) ? 7: branch+1 );
   
   if (brmax > 6) { // read ancillary info
     brmax = 6;

     b_Evtime->GetEntry(entry);
     b_Run->GetEntry(entry);
     b_Evt1->GetEntry(entry);
     b_Trig->GetEntry(entry);
     b_L1id->GetEntry(entry);
     b_Evbcid->GetEntry(entry);
     b_L1type->GetEntry(entry);
     b_Evtype->GetEntry(entry);
     b_Frbcid->GetEntry(entry);
     b_Mubackhit->GetEntry(entry);
     b_Mubacksum->GetEntry(entry);
     b_Muback->GetEntry(entry);
     b_Mucalib->GetEntry(entry);
     b_Ecal->GetEntry(entry);
     b_Lasflag->GetEntry(entry);
     b_Las0->GetEntry(entry);
     b_Las1->GetEntry(entry);
     b_Las2->GetEntry(entry);
     b_Las3->GetEntry(entry);
     b_Lasextra->GetEntry(entry);
     b_adder->GetEntry(entry);
     b_Eneadd->GetEntry(entry);
     b_Timeadd->GetEntry(entry);
     if (m_adderFit) {
       b_Efitadd->GetEntry(entry);
       b_Tfitadd->GetEntry(entry);
       b_Pedfitadd->GetEntry(entry);
       b_Chi2add->GetEntry(entry);
       b_Negstat->GetEntry(entry);
       b_Posstat->GetEntry(entry);
     } else {
       b_status->GetEntry(entry);
     }
     b_cispar->GetEntry(entry);
     b_S1cou->GetEntry(entry);
     b_S2cou->GetEntry(entry);
     b_S3cou->GetEntry(entry);
     b_S4cou->GetEntry(entry);
     b_Cher1->GetEntry(entry);
     b_Cher2->GetEntry(entry);
     b_Beamextra->GetEntry(entry);
     b_Xcha1->GetEntry(entry);
     b_Ycha1->GetEntry(entry);
     b_btdc1->GetEntry(entry);
     b_Xcha2->GetEntry(entry);
     b_Ycha2->GetEntry(entry);
     b_btdc2->GetEntry(entry);
     b_Ximp->GetEntry(entry);
     b_Yimp->GetEntry(entry);

   }

   if (brmin < 6) {
     for (int gain = 0; gain < m_nGains; ++gain) {
       for (int br = brmin; br<brmax; ++br) {
         int ent = entry + m_eventShift[br];
         b_Evt[gain][br]->GetEntry(ent);      
         b_Bcid[gain][br]->GetEntry(ent);     
         b_Size[gain][br]->GetEntry(ent);     
         b_Dmumask[gain][br]->GetEntry(ent);  
         b_Slinkcrc[gain][br]->GetEntry(ent); 
         b_Gain[gain][br]->GetEntry(ent);     
         b_Err[gain][br]->GetEntry(ent);      
         b_Sample[gain][br]->GetEntry(ent);   
         b_Ene[gain][br]->GetEntry(ent);      
         b_Time[gain][br]->GetEntry(ent);     
         b_Efit[gain][br]->GetEntry(ent);     
         b_Tfit[gain][br]->GetEntry(ent);     
         b_Pedfit[gain][br]->GetEntry(ent);   
         b_Chi2[gain][br]->GetEntry(ent);     
       }
     }
   }
   
   return 1;
}

