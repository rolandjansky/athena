/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
using namespace std;
#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TROOT.h"
#include "TDirectory.h"

ClassImp(TrigTauFlags)

void TrigTauFlags::print() const
{
  cout << " L1 " << hex << m_L1_flags <<" "<< m_secL1_flags<<" "<< m_trdL1_flags<<" "<< m_fourL1_flags<< endl;
  cout << " L2 " << hex << m_L2_flags <<" "<< m_secL2_flags<<" "<< m_trdL2_flags<<" "<< m_fourL2_flags<< endl;
  cout << " EF " << hex << m_EF_flags <<" "<< m_secEF_flags<<" "<< m_trdEF_flags<<" "<< m_fourEF_flags<< endl;
  cout << dec;
  return;
}

TString 
TrigTauFlags::menuName(TRIGGERMENU menu) 
{

  switch (menu) {
  case TAUNOCUT:               return "tauNoCut";
  case TAUNOCUT_CALO:          return "tauNoCut_cells";
  case TAUNOCUT_SITRK:         return "tauNoCut_SiTrk";
  case TAUNOCUT_TRT:           return "tauNoCut_TRT";
  case TAUNOCUT_COSMICS:       return "tauNoCut_cosmic";
  case TAUNOCUT_SITRK_COSMICS: return "tauNoCut_SiTrk_cosmic";
  case TAUNOCUT_HASTRK:        return "tauNoCut_hasTrk";
    
  case L1_TAU8 :               return "L1_TAU8";
  case L1_TAU30 :              return "L1_TAU30";

  case TAU16_SITRK :         return "tau16_SiTrk";
  case TAU50_SITRK :         return "tau50_SiTrk";

  case TRK9:          return "trk9_loose";
  case TRK9_ID :      return "trk9_loose";
  case TRK16_ID :     return "trk16_loose";
  case TRK_CALIB:     return ".*_IDCalib";

  case TAU12:      return "tau12_loose";
  case TAU16:      return "tau16_loose";
  case TAU20:      return "tau20_loose";
  case TAU29:      return "tau29_loose";
  case TAU38:      return "tau38_loose";
  case TAU50:      return "tau50_loose";
  case TAU84:      return "tau84_loose";
  case TAU100:     return "tau100_loose";
  case TAU125:     return "tau125_loose";

  case TAU16_MEDIUM:     return "tau16_medium";
  case TAU16_MEDIUM1:    return "tau16_medium1";
  case TAU20_MEDIUM:     return "tau20_medium";
  case TAU20_MEDIUM1:    return "tau20_medium1";
  case TAU29_MEDIUM:     return "tau29_medium";
  case TAU29_MEDIUM1:    return "tau29_medium1";
  case TAU38_MEDIUM:     return "tau38_medium";
  case TAU50_MEDIUM:     return "tau50_medium";
  case TAU84_MEDIUM:     return "tau84_medium";
  case TAU100_MEDIUM:    return "tau100_medium";
  case TAU100_MEDIUM1:   return "tau100_medium1";
  case TAU125_MEDIUM:    return "tau125_medium";

  case TAU16_TIGHT:     return "tau16_tight";
  case TAU20_TIGHT:     return "tau20_tight";
  case TAU29_TIGHT:     return "tau29_tight";
  case TAU38_TIGHT:     return "tau38_tight";
  case TAU50_TIGHT:     return "tau50_tight";
  case TAU84_TIGHT:     return "tau84_tight";
  case TAU125_TIGHT:    return "tau125_tight";
    
    // di-tau  chains
  case TAU2_12_LOOSE:   return "2tau12_loose";
  case TAU2_16_LOOSE:   return "2tau16_loose";
  case TAU2_20_LOOSE:   return "2tau20_loose";
  case TAU2_29_LOOSE:   return "2tau29_loose";
  case TAU2_38_LOOSE:   return "2tau38_loose";
  case TAU2_29_MEDIUM1: return "2tau29_medium1";

    // tau + met/mu/e
  case TAU12_LOOSE_EFXE12_NOMU:  return "tau12_loose_EFxe12_noMu";
  case TAU12_LOOSE_XE15_NOMU:    return "tau12_loose_xe15_noMu";
  case TAU12_LOOSE_XE20_NOMU:    return "tau12_loose_xe20_noMu";
  case TAU12_LOOSE_E10_LOOSE:    return "tau12_loose_e10_loose";
  case TAU12_LOOSE_MU10:         return "tau12_loose_mu10";
  case TAU16_LOOSE_E10_LOOSE:    return "tau16_loose_e10_loose";
  case TAU16_LOOSE_E15_LOOSE:    return "tau16_loose_e15_loose";
  case TAU16_LOOSE_MU10:         return "tau16_loose_mu10";
  case TAU16_LOOSE_MU15:         return "tau16_loose_mu15";
  case TAU16_LOOSE_XE20_NOMU:    return "tau16_loose_xe20_noMu";
  case TAU16_LOOSE_XE25_NOMU:    return "tau16_loose_xe25_noMu";
  case TAU16_LOOSE_XE30_NOMU:    return "tau16_loose_xe30_noMu";
  case TAU20_LOOSE_XE25_NOMU:    return "tau20_loose_xe25_noMu";
    
  case TAU16_LOOSE_E15_TIGHT:       return "tau16_loose_e15_tight";
  case TAU29_MEDIUM1_TAU20_MEDIUM1: return "tau29_medium1_tau20_medium1";
  case TAUNOCUT_L1TAU50:            return "tauNoCut_L1TAU50";   
  case TAU16_IDTRKNOCUT:            return "tau16_IDTrkNoCut";   
                    
  case TAU29v2:  return "tau29_loose1";
    
  case ISELECTRON:   return "EF_e.*";
  case E5_MEDIUM:    return "e5_medium";
  case E10_LOOSE:    return "e10_loose";
  case E10_MEDIUM:   return "e10_medium";
  case E12_MEDIUM:   return "e12_medium";
  case E15_MEDIUM:   return "e15_medium";
  case E15I_MEDIUM:  return "e15i_medium";
  case E20_LOOSE:    return "e20_loose";
  case E20I_LOOSE:   return "e20i_loose";
  case E25_LOOSE:    return "e25_loose";
  
  case G10  : return "g10";
  case G15  : return "g15"; 
  case G15I : return "g15i"; 
  case G25I : return "g25i";
    
  case ISMUON: return "EF_mu.*";
  case MU4:    return "mu4";
  case MU6:    return "mu6";
  case MU6L:   return "mu6l";
  case MU8:    return "mu8";
  case MU10:   return "mu10";
  case MU15:   return "mu15";
  case MU20:   return "mu20";

		    // next set
  case ISJET:   return "EF_j.*";
  case J5:      return "L1_J5";  
  case J10:     return "L1_J10"; 
  case J20:     return "L1_J20"; 
  case J40:     return "L1_J40"; 
  case J70:     return "L1_J70"; 
  case J100:    return "L1_J100";
  case J130:    return "L1_J130"; 
  case J150:    return "L1_J150"; 
   
  case BJ20 : return "b20";
  case BJ40 : return "b40";
  case BJ70 : return "b70";

  case MET10: return "XE10";
  case MET15: return "XE15";
  case MET20: return "XE20";
  case MET25: return "XE25";
  case MET30: return "XE30";
  case MET35: return "XE35";
  case MET40: return "XE40";
  case MET45: return "XE45";
  case MET50: return "XE50";

  case MBTS : return "MBTS";
  case MBTS_1_1 : return "L1_MBTS_1_1";
  case MBTS_1_1_COL : return "L1_MBTS_1_1_Col";

  case L1_BPTX0            : return "L1_BPTX0_BGRP0";
  case L1_BPTX1            : return "L1_BPTX1_BGRP0";
  case L1_RD0_FILLED       : return "L1_RD0_FILLED";

  case RD0_FILLED_NOALG: return "rd0_filled_NoAlg";
    
  default:      return "Unknown";
  }
}

TString 
TrigTauFlags::menuNameRel13(TRIGGERMENU /*menu*/) 
{
  cout << " This is not maintained anymore. Please switch to MENU1400 right away\n";
  return "Unknown";
}




bool TrigTauFlags::isJet() const
{
  if( passedEF(TrigTauFlags::ISJET) || passedEF(TrigTauFlags::J5) ||
      passedEF(TrigTauFlags::J10) || passedEF(TrigTauFlags::J20) ||
      passedEF(TrigTauFlags::J40) || passedEF(TrigTauFlags::J70) ||
      passedEF(TrigTauFlags::J100) || passedEF(TrigTauFlags::J130) || 
      passedEF(TrigTauFlags::J150))
      return true;
  return false;
}

  bool TrigTauFlags::isElectron() const
{
  if(  passedEF(TrigTauFlags::ISELECTRON)  ||
       passedEF(TrigTauFlags::E5_MEDIUM)   ||
       passedEF(TrigTauFlags::E10_LOOSE)   ||
       passedEF(TrigTauFlags::E10_MEDIUM)  ||
       passedEF(TrigTauFlags::E12_MEDIUM)  ||
       passedEF(TrigTauFlags::E15_MEDIUM)  ||
       passedEF(TrigTauFlags::E15I_MEDIUM) ||
       passedEF(TrigTauFlags::E20_LOOSE)   ||
       passedEF(TrigTauFlags::E20I_LOOSE)  ||
       passedEF(TrigTauFlags::E25_LOOSE) )
    return true;
  return false;
}
bool TrigTauFlags::isMuon() const
{
  if(  passedEF(TrigTauFlags::ISMUON) ||
       passedEF(TrigTauFlags::MU4)  ||  passedEF(TrigTauFlags::MU6) ||
       passedEF(TrigTauFlags::MU6L) ||  passedEF(TrigTauFlags::MU8) ||
       passedEF(TrigTauFlags::MU10) ||  passedEF(TrigTauFlags::MU15) ||
       passedEF(TrigTauFlags::MU20))
      return true;
  return false;
}

  bool TrigTauFlags::isTau_CaloOnly() const
{
  if( passedEF(TrigTauFlags::TAUNOCUT_CALO) ||
      passedEF(TrigTauFlags::TAU16_IDTRKNOCUT))
      return true;
  return false;
}

  bool TrigTauFlags::isTau_L2CaloOnly() const
{
  if( passedL2(TrigTauFlags::TAUNOCUT_CALO) ||
      passedL2(TrigTauFlags::TAU16_IDTRKNOCUT))
      return true;
  return false;
}


  bool TrigTauFlags::isTau_Cosmics() const
{
  if( passedEF(TrigTauFlags::TAUNOCUT_COSMICS) ||
      passedEF(TrigTauFlags::TAUNOCUT_SITRK_COSMICS))
      return true;
  return false;
}

  bool TrigTauFlags::isTau_L2Cosmics() const
{
  if( passedL2(TrigTauFlags::TAUNOCUT_COSMICS) ||
      passedL2(TrigTauFlags::TAUNOCUT_SITRK_COSMICS))
      return true;
  return false;
}


  bool TrigTauFlags::isTau_IDOnly() const
{

  if( ( passedEF(TrigTauFlags::TRK9_ID) ||
        passedEF(TrigTauFlags::TRK16_ID) ||
        passedEF(TrigTauFlags::TAU16_IDTRKNOCUT)) &&
      !isTau_Main())
      return true;
  return false;
}

  bool TrigTauFlags::isTau_L2IDOnly() const
{

  if( ( passedL2(TrigTauFlags::TRK9_ID) ||
        passedL2(TrigTauFlags::TRK16_ID) ||
        passedL2(TrigTauFlags::TAU16_IDTRKNOCUT)) &&
      !isTau_L2Main())
      return true;
  return false;
}

bool TrigTauFlags::isTau_SITRK() const
{
  return isTau_Main();
}

bool TrigTauFlags::isTau_L2SITRK() const
{
  return isTau_L2Main();
}

  bool TrigTauFlags::isTau_TRTONLY() const
{

  if( passedEF(TrigTauFlags::TAUNOCUT_TRT)  ) return true;
  else return false;
}
  bool TrigTauFlags::isTau_L2TRTONLY() const
{

  if( passedL2(TrigTauFlags::TAUNOCUT_TRT)  ) return true;
  else return false;
}

bool TrigTauFlags::isTau_Main() const
{
  bool ret = (
      passedEF(TrigTauFlags::TAUNOCUT) ||
      passedEF(TrigTauFlags::L1_TAU8)  ||
      passedEF(TrigTauFlags::L1_TAU30) ||
      passedEF(TrigTauFlags::TAUNOCUT_HASTRK) ||
      
      passedEF(TrigTauFlags::TAU12)  ||
      passedEF(TrigTauFlags::TAU16)  ||
      passedEF(TrigTauFlags::TAU20)  ||
      passedEF(TrigTauFlags::TAU29)  ||
      passedEF(TrigTauFlags::TAU38)  ||
      passedEF(TrigTauFlags::TAU50)  ||
      passedEF(TrigTauFlags::TAU84)  ||
      passedEF(TrigTauFlags::TAU125) ||
      
      passedEF(TrigTauFlags::TAU16_MEDIUM)  ||
      passedEF(TrigTauFlags::TAU20_MEDIUM)  ||
      passedEF(TrigTauFlags::TAU29_MEDIUM)  ||
      passedEF(TrigTauFlags::TAU38_MEDIUM)  ||
      passedEF(TrigTauFlags::TAU50_MEDIUM)  ||
      passedEF(TrigTauFlags::TAU84_MEDIUM)  ||
      passedEF(TrigTauFlags::TAU125_MEDIUM) || 
      
      passedEF(TrigTauFlags::TAU16_TIGHT)  ||
      passedEF(TrigTauFlags::TAU20_TIGHT)  ||
      passedEF(TrigTauFlags::TAU29_TIGHT)  ||
      passedEF(TrigTauFlags::TAU38_TIGHT)  ||
      passedEF(TrigTauFlags::TAU50_TIGHT)  ||
      passedEF(TrigTauFlags::TAU84_TIGHT)  ||
      passedEF(TrigTauFlags::TAU125_TIGHT) ||
  
      passedEF(TAU100) ||
      passedEF(TAU100_MEDIUM) ||
      passedEF(TAU100_MEDIUM1) ||
      
      passedEF(TAU16_MEDIUM1)  ||
      passedEF(TAU20_MEDIUM1)  ||
      passedEF(TAU29_MEDIUM1)  ||
      
      // di-tau  chains
      passedEF(TAU2_12_LOOSE) ||
      passedEF(TAU2_16_LOOSE) ||
      passedEF(TAU2_20_LOOSE) ||
      passedEF(TAU2_29_LOOSE) ||
      passedEF(TAU2_38_LOOSE) ||
      passedEF(TAU2_29_MEDIUM1) ||
      
      // tau + met/mu/e
      passedEF(TAU12_LOOSE_EFXE12_NOMU) ||
      passedEF(TAU12_LOOSE_XE15_NOMU) ||
      passedEF(TAU12_LOOSE_XE20_NOMU) ||
      passedEF(TAU12_LOOSE_E10_LOOSE) ||
      passedEF(TAU12_LOOSE_MU10) ||
      passedEF(TAU16_LOOSE_E10_LOOSE) ||
      passedEF(TAU16_LOOSE_E15_LOOSE) ||
      passedEF(TAU16_LOOSE_MU10) ||
      passedEF(TAU16_LOOSE_MU15) ||
      passedEF(TAU16_LOOSE_XE20_NOMU) ||
      passedEF(TAU16_LOOSE_XE25_NOMU) ||
      passedEF(TAU16_LOOSE_XE30_NOMU) ||
      passedEF(TAU20_LOOSE_XE25_NOMU) ||
      
      passedEF(TAU16_LOOSE_E15_TIGHT) ||
      passedEF(TAU29_MEDIUM1_TAU20_MEDIUM1) ||
      passedEF(TAUNOCUT_L1TAU50) ||
      
      passedEF(TAU29v2));
  
  return ret;
}

bool TrigTauFlags::isTau_L2Main() const
{
  bool ret = (
      passedL2(TrigTauFlags::TAUNOCUT) ||
      passedL2(TrigTauFlags::L1_TAU8)  ||
      passedL2(TrigTauFlags::L1_TAU30) ||
      passedL2(TrigTauFlags::TAUNOCUT_HASTRK) ||
      
      passedL2(TrigTauFlags::TAU12)  ||
      passedL2(TrigTauFlags::TAU16)  ||
      passedL2(TrigTauFlags::TAU20)  ||
      passedL2(TrigTauFlags::TAU29)  ||
      passedL2(TrigTauFlags::TAU38)  ||
      passedL2(TrigTauFlags::TAU50)  ||
      passedL2(TrigTauFlags::TAU84)  ||
      passedL2(TrigTauFlags::TAU125) ||
      
      passedL2(TrigTauFlags::TAU16_MEDIUM)  ||
      passedL2(TrigTauFlags::TAU20_MEDIUM)  ||
      passedL2(TrigTauFlags::TAU29_MEDIUM)  ||
      passedL2(TrigTauFlags::TAU38_MEDIUM)  ||
      passedL2(TrigTauFlags::TAU50_MEDIUM)  ||
      passedL2(TrigTauFlags::TAU84_MEDIUM)  ||
      passedL2(TrigTauFlags::TAU125_MEDIUM) || 
      
      passedL2(TrigTauFlags::TAU16_TIGHT)  ||
      passedL2(TrigTauFlags::TAU20_TIGHT)  ||
      passedL2(TrigTauFlags::TAU29_TIGHT)  ||
      passedL2(TrigTauFlags::TAU38_TIGHT)  ||
      passedL2(TrigTauFlags::TAU50_TIGHT)  ||
      passedL2(TrigTauFlags::TAU84_TIGHT)  ||
      passedL2(TrigTauFlags::TAU125_TIGHT) ||
      
      passedL2(TAU100) ||
      passedL2(TAU100_MEDIUM) ||
      passedL2(TAU100_MEDIUM1) ||
      
      passedL2(TAU16_MEDIUM1)  ||
      passedL2(TAU20_MEDIUM1)  ||
      passedL2(TAU29_MEDIUM1)  ||
      
      // di-tau  chains
      passedL2(TAU2_12_LOOSE) ||
      passedL2(TAU2_16_LOOSE) ||
      passedL2(TAU2_20_LOOSE) ||
      passedL2(TAU2_29_LOOSE) ||
      passedL2(TAU2_38_LOOSE) ||
      passedL2(TAU2_29_MEDIUM1) ||
      
      // tau + met/mu/e
      passedL2(TAU12_LOOSE_EFXE12_NOMU) ||
      passedL2(TAU12_LOOSE_XE15_NOMU) ||
      passedL2(TAU12_LOOSE_XE20_NOMU) ||
      passedL2(TAU12_LOOSE_E10_LOOSE) ||
      passedL2(TAU12_LOOSE_MU10) ||
      passedL2(TAU16_LOOSE_E10_LOOSE) ||
      passedL2(TAU16_LOOSE_E15_LOOSE) ||
      passedL2(TAU16_LOOSE_MU10) ||
      passedL2(TAU16_LOOSE_MU15) ||
      passedL2(TAU16_LOOSE_XE20_NOMU) ||
      passedL2(TAU16_LOOSE_XE25_NOMU) ||
      passedL2(TAU16_LOOSE_XE30_NOMU) ||
      passedL2(TAU20_LOOSE_XE25_NOMU) ||
      
      passedL2(TAU16_LOOSE_E15_TIGHT) ||
      passedL2(TAU29_MEDIUM1_TAU20_MEDIUM1) ||
      passedL2(TAUNOCUT_L1TAU50) ||
      
      passedL2(TAU29v2));

  return ret;
}

bool TrigTauFlags::isTau_IDSCAN() const
{
  return false;
}

bool TrigTauFlags::isTau_L2IDSCAN() const
{
  return false;
}

TH1F * TrigTauFlags::makeHist(const char *histName, const char * title ) 
{
  const unsigned int nbins = 4*32+1;

  TH1F * h1 = new TH1F(histName,title, nbins, -0.5, nbins-0.5);

  TString labels[nbins];
  for(unsigned int bit = 0; bit < 32 ; ++bit)
    labels[bit] = menuName( TRIGGERMENU(bit));
  for(unsigned int bit = 100; bit < 132 ; ++bit)
    labels[bit-100+32] = menuName( TRIGGERMENU(bit));
  for(unsigned int bit = 200; bit < 232 ; ++bit)
    labels[bit-200+64] = menuName( TRIGGERMENU(bit));
  for(unsigned int bit = 300; bit < 332 ; ++bit)
    labels[bit-300+96] = menuName( TRIGGERMENU(bit));

  for(unsigned int i = 0; i < nbins; ++i)
    h1->GetXaxis()->SetBinLabel(i+1,labels[i]);

  return h1;
 
}

int TrigTauFlags::fillHist(int level, const char * hdec ) const
{
  if( gDirectory == 0 ) return 0;
  TH1 * h1 = (TH1*)gDirectory->Get(hdec);
  if( h1 == 0 ) return 0;
  return fillHist(level, h1);
}
int TrigTauFlags::fillHist(int level, TH1 * h1 ) const
{
  if( h1 == 0 ) return 0;
  if( h1->GetNbinsX() < 32*4+1 ) {
    static int count = 10;
    if( count > 0 ) 
      cout << " TrigTauFlags ERROR: given histogram " << h1->GetName() << " can not accomodate all bins. Do nothing.\n";
    count--;
    return 0;
  }

  for(unsigned int bit = 0; bit < 32 ; ++bit)
    {
      if( level == 1 ){
	h1->AddBinContent(bit+1,    float(((m_L1_flags     >> bit) &0x1) != 0 ));
	h1->AddBinContent(bit+32+1, float(((m_secL1_flags  >> bit) &0x1) != 0 ));
	h1->AddBinContent(bit+64+1, float(((m_trdL1_flags  >> bit) &0x1) != 0 ));
	h1->AddBinContent(bit+96+1, float(((m_fourL1_flags >> bit) &0x1) != 0 ));
      }else if( level == 2 ){
	h1->AddBinContent(bit+1,    float(((m_L2_flags     >> bit) &0x1) != 0 ));
	h1->AddBinContent(bit+32+1, float(((m_secL2_flags  >> bit) &0x1) != 0 ));
	h1->AddBinContent(bit+64+1, float(((m_trdL2_flags  >> bit) &0x1) != 0 ));
	h1->AddBinContent(bit+96+1, float(((m_fourL2_flags >> bit) &0x1) != 0 ));
      }else  if( level == 3 ){
	h1->AddBinContent(bit+1,    float(((m_EF_flags     >> bit) &0x1) != 0 ));
	h1->AddBinContent(bit+32+1, float(((m_secEF_flags  >> bit) &0x1) != 0 ));
	h1->AddBinContent(bit+64+1, float(((m_trdEF_flags  >> bit) &0x1) != 0 ));
	h1->AddBinContent(bit+96+1, float(((m_fourEF_flags >> bit) &0x1) != 0 ));
      }
    }
  return 1;
}



 std::vector<TrigTauFlags::TRIGGERMENU> TrigTauFlags::allMuonChains()       // single only
{
  std::vector<TrigTauFlags::TRIGGERMENU> chains;
      chains.push_back(TrigTauFlags::MU4);
      chains.push_back(TrigTauFlags::MU6);
      chains.push_back(TrigTauFlags::MU6L);
      chains.push_back(TrigTauFlags::MU8);
      chains.push_back(TrigTauFlags::MU10);
      chains.push_back(TrigTauFlags::MU15);
      chains.push_back(TrigTauFlags::MU20);

      //chains.push_back(TrigTauFlags::TWOMU6);
      //chains.push_back(TrigTauFlags::TWOMU20);
  return chains;
    
}
 std::vector<TrigTauFlags::TRIGGERMENU> TrigTauFlags::allElectronChains()   // single only
{
  std::vector<TrigTauFlags::TRIGGERMENU> chains;
      chains.push_back(TrigTauFlags::E5_MEDIUM);
      chains.push_back(TrigTauFlags::E10_LOOSE);
      chains.push_back(TrigTauFlags::E10_MEDIUM    );
      chains.push_back(TrigTauFlags::E12_MEDIUM    );
      chains.push_back(TrigTauFlags::E15_MEDIUM    );
      chains.push_back(TrigTauFlags::E15I_MEDIUM   );
      chains.push_back(TrigTauFlags::E20_LOOSE    );
      chains.push_back(TrigTauFlags::E20I_LOOSE   );
      chains.push_back(TrigTauFlags::E25_LOOSE   );
      chains.push_back(TrigTauFlags::G10    );
      chains.push_back(TrigTauFlags::G15    );
      chains.push_back(TrigTauFlags::G15I   );
      chains.push_back(TrigTauFlags::G25I   );
  return chains;
}
 std::vector<TrigTauFlags::TRIGGERMENU> TrigTauFlags::allTauChains()        // single only
{
  std::vector<TrigTauFlags::TRIGGERMENU> chains;
  chains.push_back(TrigTauFlags::TAUNOCUT                );
  chains.push_back(TrigTauFlags::TAUNOCUT_CALO           );
  chains.push_back(TrigTauFlags::TAUNOCUT_COSMICS        );
  chains.push_back(TrigTauFlags::TAUNOCUT_SITRK          );
  chains.push_back(TrigTauFlags::TAUNOCUT_TRT            );
  chains.push_back(TrigTauFlags::TAUNOCUT_SITRK_COSMICS        );
  chains.push_back(TrigTauFlags::TAUNOCUT_HASTRK          );

  chains.push_back(TrigTauFlags::L1_TAU8                 );
  chains.push_back(TrigTauFlags::L1_TAU30                );

  chains.push_back(TrigTauFlags::TAU50_SITRK          );

  chains.push_back(TrigTauFlags::TRK9);
  chains.push_back(TrigTauFlags::TRK9_ID);
  chains.push_back(TrigTauFlags::TRK16_ID);
  chains.push_back(TrigTauFlags::TRK_CALIB);
  chains.push_back(TrigTauFlags::TAU12);
  chains.push_back(TrigTauFlags::TAU16);
  chains.push_back(TrigTauFlags::TAU20);
  chains.push_back(TrigTauFlags::TAU29);
  chains.push_back(TrigTauFlags::TAU38);
  chains.push_back(TrigTauFlags::TAU50);
  chains.push_back(TrigTauFlags::TAU84);
  chains.push_back(TrigTauFlags::TAU125);

  chains.push_back(TrigTauFlags::TAU16_MEDIUM);
  chains.push_back(TrigTauFlags::TAU20_MEDIUM);
  chains.push_back(TrigTauFlags::TAU29_MEDIUM);
  chains.push_back(TrigTauFlags::TAU38_MEDIUM);
  chains.push_back(TrigTauFlags::TAU50_MEDIUM);
  chains.push_back(TrigTauFlags::TAU84_MEDIUM);
  chains.push_back(TrigTauFlags::TAU125_MEDIUM);

  chains.push_back(TrigTauFlags::TAU16_TIGHT);
  chains.push_back(TrigTauFlags::TAU20_TIGHT);
  chains.push_back(TrigTauFlags::TAU29_TIGHT);
  chains.push_back(TrigTauFlags::TAU38_TIGHT);
  chains.push_back(TrigTauFlags::TAU50_TIGHT);
  chains.push_back(TrigTauFlags::TAU84_TIGHT);
  chains.push_back(TrigTauFlags::TAU125_TIGHT);

  chains.push_back(TrigTauFlags::TAU29v2);
  chains.push_back(TrigTauFlags::TAU100);
  chains.push_back(TrigTauFlags::TAU100_MEDIUM);
  chains.push_back(TrigTauFlags::TAUNOCUT_L1TAU50);
  chains.push_back(TrigTauFlags::TAU16_IDTRKNOCUT);
  chains.push_back(TrigTauFlags::TAU16_MEDIUM1);
  chains.push_back(TrigTauFlags::TAU20_MEDIUM1);
  chains.push_back(TrigTauFlags::TAU29_MEDIUM1);
  chains.push_back(TrigTauFlags::TAU100_MEDIUM1);
  
  return chains;
}
 std::vector<TrigTauFlags::TRIGGERMENU> TrigTauFlags::allJetChains()        // single only
{
  std::vector<TrigTauFlags::TRIGGERMENU> chains;
      chains.push_back(TrigTauFlags::J5);
      chains.push_back(TrigTauFlags::J10);
      chains.push_back(TrigTauFlags::J20);
      chains.push_back(TrigTauFlags::J40);
      chains.push_back(TrigTauFlags::J70);
      chains.push_back(TrigTauFlags::J100);
      chains.push_back(TrigTauFlags::J130);
      chains.push_back(TrigTauFlags::J150);
  return chains;
}

bool TrigTauFlags::isTau_L1() const
{
  bool ret = (
      passedL1(TrigTauFlags::TAUNOCUT) ||
      passedL1(TrigTauFlags::L1_TAU8)  ||
      passedL1(TrigTauFlags::L1_TAU30)  ||
      passedL1(TrigTauFlags::TAUNOCUT_HASTRK) ||
      
      passedL1(TrigTauFlags::TAU12) ||
      passedL1(TrigTauFlags::TAU16) ||
      passedL1(TrigTauFlags::TAU20) ||
      passedL1(TrigTauFlags::TAU29) ||
      passedL1(TrigTauFlags::TAU38) ||
      passedL1(TrigTauFlags::TAU50) ||
      passedL1(TrigTauFlags::TAU84) ||
      passedL1(TrigTauFlags::TAU125) ||
      
      passedL1(TrigTauFlags::TAU16_MEDIUM)  ||
      passedL1(TrigTauFlags::TAU20_MEDIUM)  ||
      passedL1(TrigTauFlags::TAU29_MEDIUM)  ||
      passedL1(TrigTauFlags::TAU38_MEDIUM)  ||
      passedL1(TrigTauFlags::TAU50_MEDIUM)  ||
      passedL1(TrigTauFlags::TAU84_MEDIUM)  ||
      passedL1(TrigTauFlags::TAU125_MEDIUM) ||
      
      passedL1(TrigTauFlags::TAU16_TIGHT)  ||
      passedL1(TrigTauFlags::TAU20_TIGHT)  ||
      passedL1(TrigTauFlags::TAU29_TIGHT)  ||
      passedL1(TrigTauFlags::TAU38_TIGHT)  ||
      passedL1(TrigTauFlags::TAU50_TIGHT)  ||
      passedL1(TrigTauFlags::TAU84_TIGHT)  ||
      passedL1(TrigTauFlags::TAU125_TIGHT) ||
      
      passedL1(TrigTauFlags::TAUNOCUT_TRT) ||
      passedL1(TrigTauFlags::TAUNOCUT_SITRK) ||
      passedL1(TrigTauFlags::TAU16_SITRK) ||
      passedL1(TrigTauFlags::TAU50_SITRK) ||
      passedL1(TrigTauFlags::TAUNOCUT_COSMICS) ||
      passedL1(TrigTauFlags::TAUNOCUT_SITRK_COSMICS) ||
      passedL1(TrigTauFlags::TAUNOCUT_CALO) ||
  
      passedL1(TAU29v2) ||
      passedL1(TAU100) ||
      passedL1(TAU100_MEDIUM) ||
      passedL1(TAU16_MEDIUM1) ||
      passedL1(TAU20_MEDIUM1) ||
      passedL1(TAU29_MEDIUM1) ||
      passedL1(TAU100_MEDIUM1) ||
      passedL1(TAUNOCUT_L1TAU50));
  
  return ret;
}



