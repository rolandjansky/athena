/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Nov 12 08:17:38 2007 by ROOT version 5.04/00
// from TTree h1000/Tile
// found on file: r0330076.root
//////////////////////////////////////////////////////////

#ifndef TileTBOldNtupleStruct_h
#define TileTBOldNtupleStruct_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include <vector>

class TileTBOldNtupleStruct {
public :

  TileTBOldNtupleStruct(TTree *tree, bool bigain, bool adderFit, std::vector<int> & shift);
  void SetEventShift(std::vector<int> shift) {m_eventShift = shift;}
  void SetEventShift(int dr, int shift)  {m_eventShift[dr] = shift;}

  virtual ~TileTBOldNtupleStruct();
  virtual Int_t    GetEntry(Long64_t entry, int branch=-1);

   // Declaration of leave types
   Int_t           Evtime;
   Int_t           Run;
   Int_t           Evt;
   Int_t           Trig;
   Int_t           L1id[3];
   Int_t           Evbcid[3];
   Int_t           L1type[3];
   Int_t           Evtype[3];
   Int_t           Frbcid[6];
   Float_t         Mubackhit;
   Float_t         Mubacksum;
   Float_t         Muback[14];
   Float_t         Mucalib[2];
   Float_t         Ecal[8];
   Int_t           Lasflag;
   Float_t         Las0;
   Float_t         Las1;
   Float_t         Las2;
   Float_t         Las3;
   Float_t         Lasextra[4];
   UChar_t         adder[16][16];
   Float_t         Eneadd[16];
   Float_t         Timeadd[16];
   Float_t         Efitadd[16];
   Float_t         Tfitadd[16];
   Float_t         Pedfitadd[16];
   Float_t         Chi2add[16];
   Float_t         Negstat[3];
   Float_t         Posstat[3];
   Float_t         status[2][3];
   Int_t           cispar[12];
   Float_t         S1cou;
   Float_t         S2cou;
   Float_t         S3cou;
   Float_t         S4cou;
   Float_t         Cher1;
   Float_t         Cher2;
   Float_t         Beamextra[2];
   Float_t         Xcha1;
   Float_t         Ycha1;
   UShort_t        btdc1[8];
   Float_t         Xcha2;
   Float_t         Ycha2;
   UShort_t        btdc2[8];
   Float_t         Ximp;
   Float_t         Yimp;
   Int_t           Evtn0lo;
   UShort_t        Bcidn0lo;
   UShort_t        Sizen0lo;
   UShort_t        Dmumaskn0lo[2];
   UShort_t        Slinkcrcn0lo[2];
   UChar_t         Gainn0lo[48];
   UShort_t        Errn0lo[48];
   UShort_t        Samplen0lo[48][9];
   Float_t         Enen0lo[48];
   Float_t         Timen0lo[48];
   Float_t         Efitn0lo[48];
   Float_t         Tfitn0lo[48];
   Float_t         Pedfitn0lo[48];
   Float_t         Chi2n0lo[48];
   Int_t           Evtn1lo;
   UShort_t        Bcidn1lo;
   UShort_t        Sizen1lo;
   UShort_t        Dmumaskn1lo[2];
   UShort_t        Slinkcrcn1lo[2];
   UChar_t         Gainn1lo[48];
   UShort_t        Errn1lo[48];
   UShort_t        Samplen1lo[48][9];
   Float_t         Enen1lo[48];
   Float_t         Timen1lo[48];
   Float_t         Efitn1lo[48];
   Float_t         Tfitn1lo[48];
   Float_t         Pedfitn1lo[48];
   Float_t         Chi2n1lo[48];
   Int_t           Evtn2lo;
   UShort_t        Bcidn2lo;
   UShort_t        Sizen2lo;
   UShort_t        Dmumaskn2lo[2];
   UShort_t        Slinkcrcn2lo[2];
   UChar_t         Gainn2lo[48];
   UShort_t        Errn2lo[48];
   UShort_t        Samplen2lo[48][9];
   Float_t         Enen2lo[48];
   Float_t         Timen2lo[48];
   Float_t         Efitn2lo[48];
   Float_t         Tfitn2lo[48];
   Float_t         Pedfitn2lo[48];
   Float_t         Chi2n2lo[48];
   Int_t           Evtp0lo;
   UShort_t        Bcidp0lo;
   UShort_t        Sizep0lo;
   UShort_t        Dmumaskp0lo[2];
   UShort_t        Slinkcrcp0lo[2];
   UChar_t         Gainp0lo[48];
   UShort_t        Errp0lo[48];
   UShort_t        Samplep0lo[48][9];
   Float_t         Enep0lo[48];
   Float_t         Timep0lo[48];
   Float_t         Efitp0lo[48];
   Float_t         Tfitp0lo[48];
   Float_t         Pedfitp0lo[48];
   Float_t         Chi2p0lo[48];
   Int_t           Evtp1lo;
   UShort_t        Bcidp1lo;
   UShort_t        Sizep1lo;
   UShort_t        Dmumaskp1lo[2];
   UShort_t        Slinkcrcp1lo[2];
   UChar_t         Gainp1lo[48];
   UShort_t        Errp1lo[48];
   UShort_t        Samplep1lo[48][9];
   Float_t         Enep1lo[48];
   Float_t         Timep1lo[48];
   Float_t         Efitp1lo[48];
   Float_t         Tfitp1lo[48];
   Float_t         Pedfitp1lo[48];
   Float_t         Chi2p1lo[48];
   Int_t           Evtp2lo;
   UShort_t        Bcidp2lo;
   UShort_t        Sizep2lo;
   UShort_t        Dmumaskp2lo[2];
   UShort_t        Slinkcrcp2lo[2];
   UChar_t         Gainp2lo[48];
   UShort_t        Errp2lo[48];
   UShort_t        Samplep2lo[48][9];
   Float_t         Enep2lo[48];
   Float_t         Timep2lo[48];
   Float_t         Efitp2lo[48];
   Float_t         Tfitp2lo[48];
   Float_t         Pedfitp2lo[48];
   Float_t         Chi2p2lo[48];
   Int_t           Evtn0hi;
   UShort_t        Bcidn0hi;
   UShort_t        Sizen0hi;
   UShort_t        Dmumaskn0hi[2];
   UShort_t        Slinkcrcn0hi[2];
   UChar_t         Gainn0hi[48];
   UShort_t        Errn0hi[48];
   UShort_t        Samplen0hi[48][9];
   Float_t         Enen0hi[48];
   Float_t         Timen0hi[48];
   Float_t         Efitn0hi[48];
   Float_t         Tfitn0hi[48];
   Float_t         Pedfitn0hi[48];
   Float_t         Chi2n0hi[48];
   Int_t           Evtn1hi;
   UShort_t        Bcidn1hi;
   UShort_t        Sizen1hi;
   UShort_t        Dmumaskn1hi[2];
   UShort_t        Slinkcrcn1hi[2];
   UChar_t         Gainn1hi[48];
   UShort_t        Errn1hi[48];
   UShort_t        Samplen1hi[48][9];
   Float_t         Enen1hi[48];
   Float_t         Timen1hi[48];
   Float_t         Efitn1hi[48];
   Float_t         Tfitn1hi[48];
   Float_t         Pedfitn1hi[48];
   Float_t         Chi2n1hi[48];
   Int_t           Evtn2hi;
   UShort_t        Bcidn2hi;
   UShort_t        Sizen2hi;
   UShort_t        Dmumaskn2hi[2];
   UShort_t        Slinkcrcn2hi[2];
   UChar_t         Gainn2hi[48];
   UShort_t        Errn2hi[48];
   UShort_t        Samplen2hi[48][9];
   Float_t         Enen2hi[48];
   Float_t         Timen2hi[48];
   Float_t         Efitn2hi[48];
   Float_t         Tfitn2hi[48];
   Float_t         Pedfitn2hi[48];
   Float_t         Chi2n2hi[48];
   Int_t           Evtp0hi;
   UShort_t        Bcidp0hi;
   UShort_t        Sizep0hi;
   UShort_t        Dmumaskp0hi[2];
   UShort_t        Slinkcrcp0hi[2];
   UChar_t         Gainp0hi[48];
   UShort_t        Errp0hi[48];
   UShort_t        Samplep0hi[48][9];
   Float_t         Enep0hi[48];
   Float_t         Timep0hi[48];
   Float_t         Efitp0hi[48];
   Float_t         Tfitp0hi[48];
   Float_t         Pedfitp0hi[48];
   Float_t         Chi2p0hi[48];
   Int_t           Evtp1hi;
   UShort_t        Bcidp1hi;
   UShort_t        Sizep1hi;
   UShort_t        Dmumaskp1hi[2];
   UShort_t        Slinkcrcp1hi[2];
   UChar_t         Gainp1hi[48];
   UShort_t        Errp1hi[48];
   UShort_t        Samplep1hi[48][9];
   Float_t         Enep1hi[48];
   Float_t         Timep1hi[48];
   Float_t         Efitp1hi[48];
   Float_t         Tfitp1hi[48];
   Float_t         Pedfitp1hi[48];
   Float_t         Chi2p1hi[48];
   Int_t           Evtp2hi;
   UShort_t        Bcidp2hi;
   UShort_t        Sizep2hi;
   UShort_t        Dmumaskp2hi[2];
   UShort_t        Slinkcrcp2hi[2];
   UChar_t         Gainp2hi[48];
   UShort_t        Errp2hi[48];
   UShort_t        Samplep2hi[48][9];
   Float_t         Enep2hi[48];
   Float_t         Timep2hi[48];
   Float_t         Efitp2hi[48];
   Float_t         Tfitp2hi[48];
   Float_t         Pedfitp2hi[48];
   Float_t         Chi2p2hi[48];

   // List of branches
   TBranch        *b_Evtime;   //!
   TBranch        *b_Run;   //!
   TBranch        *b_Evt1;  //!
   TBranch        *b_Trig;   //!
   TBranch        *b_L1id;   //!
   TBranch        *b_Evbcid;   //!
   TBranch        *b_L1type;   //!
   TBranch        *b_Evtype;   //!
   TBranch        *b_Frbcid;   //!
   TBranch        *b_Mubackhit;   //!
   TBranch        *b_Mubacksum;   //!
   TBranch        *b_Muback;   //!
   TBranch        *b_Mucalib;   //!
   TBranch        *b_Ecal;   //!
   TBranch        *b_Lasflag;   //!
   TBranch        *b_Las0;   //!
   TBranch        *b_Las1;   //!
   TBranch        *b_Las2;   //!
   TBranch        *b_Las3;   //!
   TBranch        *b_Lasextra;   //!
   TBranch        *b_adder;   //!
   TBranch        *b_Eneadd;   //!
   TBranch        *b_Timeadd;   //!
   TBranch        *b_Efitadd;   //!
   TBranch        *b_Tfitadd;   //!
   TBranch        *b_Pedfitadd;   //!
   TBranch        *b_Chi2add;   //!
   TBranch        *b_Negstat;   //!
   TBranch        *b_Posstat;   //!
   TBranch        *b_status;   //!
   TBranch        *b_cispar;   //!
   TBranch        *b_S1cou;   //!
   TBranch        *b_S2cou;   //!
   TBranch        *b_S3cou;   //!
   TBranch        *b_S4cou;   //!
   TBranch        *b_Cher1;   //!
   TBranch        *b_Cher2;   //!
   TBranch        *b_Beamextra;   //!
   TBranch        *b_Xcha1;   //!
   TBranch        *b_Ycha1;   //!
   TBranch        *b_btdc1;   //!
   TBranch        *b_Xcha2;   //!
   TBranch        *b_Ycha2;   //!
   TBranch        *b_btdc2;   //!
   TBranch        *b_Ximp;   //!
   TBranch        *b_Yimp;   //!

   TBranch        *b_Evt[2][6];   //!
   TBranch        *b_Bcid[2][6];   //!
   TBranch        *b_Size[2][6];   //!
   TBranch        *b_Dmumask[2][6];   //!
   TBranch        *b_Slinkcrc[2][6];   //!
   TBranch        *b_Gain[2][6];   //!
   TBranch        *b_Err[2][6];   //!
   TBranch        *b_Sample[2][6];   //!
   TBranch        *b_Ene[2][6];   //!
   TBranch        *b_Time[2][6];   //!
   TBranch        *b_Efit[2][6];   //!
   TBranch        *b_Tfit[2][6];   //!
   TBranch        *b_Pedfit[2][6];   //!
   TBranch        *b_Chi2[2][6];   //!

private:

  TTree          *m_fChain;   //!pointer to the analyzed TTree or TChain

  bool            m_adderFit; // if ntuple has additional Efit for adder
  Int_t           m_nGains;  // monogain (=1) or bigain (=2) ntuple
  std::vector<int> m_eventShift; // additional shift for 6 drawers with respect to beam frag

};

#endif

