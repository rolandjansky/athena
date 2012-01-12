/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUFLAGS_H
#define TRIGTAUFLAGS_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauFlags.h
/// Package : offline/PhysicsAnalysis/AnalysisCommon/UserAnalysis
/// Author  : Olya Igonkina
/// Created : January 2006
///
/// DESCRIPTION:
///
///
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TString.h"
#include "TH1F.h"
#include <vector>

class TrigTauFlags : public TObject
{
 public:
  enum TRIGGERMENU{ SECONDATTEMPT       = 0, 
		    TAUNOCUT            = 1,
		    TAUNOCUT_CALO       = 319, //tauNoCut_cells
		    TAUNOCUT_SITRK      = 327,
		    TAUNOCUT_TRT        = 328,
		    TAUNOCUT_COSMICS    = 115,
		    TAUNOCUT_SITRK_COSMICS =116,
		    TAUNOCUT_HASTRK     = 320,
                    
		    L1_TAU8             = 317,
		    L1_TAU30            = 318,
		    TAU16_SITRK         = 112,
                    
                    TAU50_SITRK         = 229,
                    
		    TRK9                 = 2,
		    TRK9_ID              = 3,
		    TRK16_ID             = 325,
		    TRK_CALIB           = 5,
                    
		    TAU12     = 4,
		    TAU16     = 114, // not in 13.0.30 
		    TAU20     = 202,
		    TAU29     = 131,
 		    TAU38     = 118,
		    TAU50     = 120,
		    TAU84     = 121,
                    TAU100    = 331,
		    TAU125    = 300,
		    
		    
		    TAU16_MEDIUM   = 302,
		    TAU20_MEDIUM   = 303,
		    TAU29_MEDIUM   = 304,
 		    TAU38_MEDIUM   = 305,
		    TAU50_MEDIUM   = 306,
		    TAU84_MEDIUM   = 307,
                    TAU100_MEDIUM  = 301,
		    TAU125_MEDIUM  = 308,
                    
 		    TAU16_MEDIUM1  = 309,
		    TAU20_MEDIUM1  = 7,
		    TAU29_MEDIUM1  = 6,
		    TAU100_MEDIUM1 = 217,

		    TAU16_TIGHT   = 310,
		    TAU20_TIGHT   = 311,
		    TAU29_TIGHT   = 312,
 		    TAU38_TIGHT   = 313,
		    TAU50_TIGHT   = 314,
		    TAU84_TIGHT   = 315,
		    TAU125_TIGHT  = 316,
                    
                    
		    // MET decision is set for L1 only
		    MET10    = 111, 
		    MET15    = 110, 
		    MET20    = 10,  
		    MET25    = 124, 
		    MET30    = 11,  
		    MET35    = 14, 
		    MET40    = 12,  
		    MET45    = 15, 
		    MET50    = 13,
                                        
		    ISELECTRON     = 210,
		    E5_MEDIUM      = 211,
		    E10_LOOSE      = 16,
		    E10_MEDIUM     = 215,
 		    E12_MEDIUM     = 216,
 		    E15_MEDIUM     = 17, 
		    E15I_MEDIUM    = 18, 
                    E20_LOOSE      = 117,
                    E20I_LOOSE     = 122, 
		    E25_LOOSE      = 20,  
                    
		    G10      = 22, 
		    G15      = 23,  
		    G15I     = 24, 
		    G25I     = 25, 
                    
		    ISMUON   = 125,
		    MU4      = 123, 
		    MU6      = 26, 
		    MU6L     = 27,
		    MU8      = 28, 
		    MU10     = 29, 
		    MU15     = 30, 
		    MU20     = 31,
                    
		    // next set
		    ISJET     = 213,
                    J5        = 203, // FOR ALL JETS we use L1 threshold!!
		    J10       = 100,
		    J20       = 126,
		    J40       = 127,
		    J70       = 128,
		    J100      = 129,
		    J130      = 214,
		    J150      = 130,
                    
                    
		    BJ20      = 200, 
		    BJ40      = 201, 
		    BJ70      = 102, 
                    
                    
		    L1_BPTX0               = 228,
		    L1_BPTX1               = 212,
		    L1_RD0_FILLED          = 209,
                    
                    RD0_FILLED_NOALG = 205,
                    
		    MBTS = 230,
		    MBTS_1_1     = 207,
		    MBTS_1_1_COL = 208,
                    
                    // di-tau chains
                    TAU2_12_LOOSE = 9,
                    TAU2_16_LOOSE = 19,
                    TAU2_20_LOOSE = 225,
                    TAU2_29_LOOSE = 21,
                    TAU2_38_LOOSE = 226,
                    TAU2_29_MEDIUM1 = 329,
                    
                    // tau + met/mu/e
                    TAU12_LOOSE_EFXE12_NOMU = 101,
                    TAU12_LOOSE_XE15_NOMU   = 103,
                    TAU12_LOOSE_XE20_NOMU   = 104,
                    TAU12_LOOSE_E10_LOOSE   = 105,
                    TAU12_LOOSE_MU10        = 106,
                    TAU16_LOOSE_E10_LOOSE   = 107,
                    TAU16_LOOSE_E15_LOOSE   = 227,
                    TAU16_LOOSE_E15_TIGHT   = 119,
                    TAU16_LOOSE_MU10        = 108,
                    TAU16_LOOSE_MU15        = 231,
                    TAU16_LOOSE_XE20_NOMU   = 109,
                    TAU16_LOOSE_XE25_NOMU   = 113,
                    TAU16_LOOSE_XE30_NOMU   = 321,
                    TAU20_LOOSE_XE25_NOMU   = 322,

                    // added on Feb 28th 2011
                    TAU29_MEDIUM1_TAU20_MEDIUM1 = 206,
                    TAUNOCUT_L1TAU50            = 219,
                    TAU16_IDTRKNOCUT            = 8,

                    // tau29_loose1 with harder cut on nTracks
		    TAU29v2 = 326

                    // free bits
		    //TRK16I               = 323,
		    //TRK29I               = 324,
                    //TAUNOCUT_HASTRK6_EFXE15_NOMU         = 204,
                    //TAUNOCUT_HASTRK9_XE20_NOMU           = 218,
                    //TAUNOCUT_HASTRK6_IDSCAN_XE20_NOMU    = 220,
                    //TAUNOCUT_IDSCAN                      = 221,
                    //TAU12_LOOSE_IDSCAN_EFXE12_NOMU       = 222,
                    //TAU12_LOOSE_IDSCAN_XE15_NOMU         = 223,
                    //TAU50_LOOSE_IDSCAN                   = 224,
                    //   = 330

  };
  

  TrigTauFlags() : m_L1_flags(0),  m_L2_flags(0),  m_EF_flags(0),
		   m_secL1_flags(0),  m_secL2_flags(0),  m_secEF_flags(0),
		   m_trdL1_flags(0),  m_trdL2_flags(0),  m_trdEF_flags(0),
		   m_fourL1_flags(0),  m_fourL2_flags(0),  m_fourEF_flags(0),
                   m_PT_flags(0), m_secPT_flags(0), m_trdPT_flags(0), m_fourPT_flags(0)
  {};
  virtual ~TrigTauFlags() {};

  inline bool passedL1(TRIGGERMENU menu) const ;
  inline bool passedL2(TRIGGERMENU menu) const ;
  inline bool passedEF(TRIGGERMENU menu) const ;

  inline bool passThrough(TRIGGERMENU menu) const ;
  inline void setPT(TRIGGERMENU menu, bool passThrough);

  inline bool rerun(TRIGGERMENU menu) const ;
  inline void setRerun(TRIGGERMENU menu, bool rerun);

  bool passedTrigger(TRIGGERMENU menu) const {return passedL1(menu)&&passedL2(menu)&&passedEF(menu); } ; 
  inline void setL1(TRIGGERMENU menu, bool decision); 
  inline void setL2(TRIGGERMENU menu, bool decision);
  inline void setEF(TRIGGERMENU menu, bool decision);


  inline bool failedL1() const; // everything is 0. useful for debugging
  inline bool failedL2() const;
  inline bool failedEF() const;

  
  inline void reset();
  
  void print() const;
  static TString menuName(TRIGGERMENU menu) ; // Return string for menu name
  static TString menuNameRel13(TRIGGERMENU menu) ; // Return string as in release 13

  bool isJet() const;
  bool isMuon() const;
  bool isElectron() const;
  bool isTau() const {return isTau_EF(); };

  bool isTau_L1() const;
  bool isTau_L2() const {return isTau_L2AllDetectors() ||isTau_L2CaloOnly() || 
      isTau_L2IDOnly() || isTau_L2Cosmics(); };
  bool isTau_EF() const {return isTau_allDetectors() ||isTau_CaloOnly() || 
      isTau_IDOnly() || isTau_Cosmics(); };

  static std::vector<TrigTauFlags::TRIGGERMENU> allMuonChains() ;      // single only
  static std::vector<TrigTauFlags::TRIGGERMENU> allElectronChains() ;  // single only, includes photon
  static std::vector<TrigTauFlags::TRIGGERMENU> allTauChains() ;       // single only
  static std::vector<TrigTauFlags::TRIGGERMENU> allJetChains() ;       // single only


  // Specify tau author type selection
  enum HLTAUTHOR{ 
    ANYAUTHOR = 0,  // To specify no checking
    ALLDETECTORS,
    DEFAULT,
    CALOONLY,
    IDONLY,
    IDSCAN,
    SITRK,
    TRTONLY,
    COSMICS
  };

  // One-stop shopping for author type selections
  inline bool isTau_L2AuthorType(HLTAUTHOR type) const ;
  inline bool isTau_AuthorType(HLTAUTHOR type) const ;


  // Specific author functions (checks EF bits)
  bool isTau_allDetectors() const {return isTau_IDSCAN() || isTau_SITRK() || isTau_TRTONLY() ; } ;
  bool isTau_CaloOnly() const ;
  bool isTau_Cosmics() const ;
  bool isTau_IDOnly() const ;
  bool isTau_IDSCAN() const ; // basic tauNoCut selection
  bool isTau_SITRK() const ;
  bool isTau_TRTONLY() const ;
  bool isTau_Main() const;   // main chains without algo/detector specific suffixes
  
  bool isTau_L2AllDetectors() const {return isTau_L2IDSCAN() || isTau_L2SITRK() || isTau_L2TRTONLY() || isTau_L2Cosmics() ; } ;
  bool isTau_L2CaloOnly() const ;
  bool isTau_L2Cosmics() const ;
  bool isTau_L2IDOnly() const ;
  bool isTau_L2IDSCAN() const ;
  bool isTau_L2SITRK() const ;
  bool isTau_L2TRTONLY() const ;
  bool isTau_L2Main() const;   // main chains without algo/detector specific suffixes
  
  bool secondAttept() const {return passedEF(SECONDATTEMPT); }

  static TH1F * makeHist(const char *histName, const char * title ) ;
  int fillHist(int level, TH1 * hist ) const;
  int fillHist(int level,  const char * hdec ) const;

 private:
  unsigned int m_L1_flags;
  unsigned int m_L2_flags;
  unsigned int m_EF_flags;

  unsigned int m_secL1_flags;
  unsigned int m_secL2_flags;
  unsigned int m_secEF_flags;

  unsigned int m_trdL1_flags;
  unsigned int m_trdL2_flags;
  unsigned int m_trdEF_flags;

  unsigned int m_fourL1_flags;
  unsigned int m_fourL2_flags;
  unsigned int m_fourEF_flags;

  // bit fields for pass-through
  unsigned int m_PT_flags;
  unsigned int m_secPT_flags;
  unsigned int m_trdPT_flags;
  unsigned int m_fourPT_flags;
  
  // bit fields for pass-through
  unsigned int m_rerun_flags;
  unsigned int m_secRerun_flags;
  unsigned int m_trdRerun_flags;
  unsigned int m_fourRerun_flags;
  
  ClassDef(TrigTauFlags, 13);
};

bool TrigTauFlags::passedL1( TrigTauFlags::TRIGGERMENU menu) const 
{ 
  unsigned int bit  =menu;
  if( bit<100 )
    return ((m_L1_flags >> bit) &0x1) != 0; 
  else if( bit<200 )
    return ((m_secL1_flags >> (bit-100)) &0x1) != 0; 
  else if( bit<300 )
    return ((m_trdL1_flags >> (bit-200))&0x1) != 0;
  else 
    return ((m_fourL1_flags >> (bit-300))&0x1) != 0;
}

bool TrigTauFlags::TrigTauFlags::passedL2(TRIGGERMENU menu) const 
{ 
  unsigned int bit  =menu;
  if( bit<100 )
    return ((m_L2_flags >> bit) &0x1) != 0; 
  else if( bit<200 )
    return ((m_secL2_flags >> (bit-100))&0x1) != 0; 
  else  if( bit<300 )
    return ((m_trdL2_flags >> (bit-200))&0x1) != 0; 
  else 
    return ((m_fourL2_flags >> (bit-300))&0x1) != 0; 
}


bool  TrigTauFlags::passedEF(TRIGGERMENU menu) const   
{ 
  unsigned int bit  =menu;
  if( bit<100 )
    return ((m_EF_flags >> bit) &0x1) != 0; 
  else if( bit<200 )
    return ((m_secEF_flags >> (bit-100))&0x1) != 0; 
  else if( bit<300 )
    return ((m_trdEF_flags >> (bit-200))&0x1) != 0; 
  else 
    return ((m_fourEF_flags >> (bit-300))&0x1) != 0; 
}


void TrigTauFlags::setL1(TRIGGERMENU menu, bool decision) 
{
  unsigned int bit  =menu;
  unsigned int * var = &m_L1_flags;
  if( bit > 299 ) {      bit -= 300; var  = &m_fourL1_flags;}
  else if( bit > 199 ) { bit -= 200; var  = &m_trdL1_flags;}
  else if( bit > 99 ) {  bit -= 100; var  = &m_secL1_flags;}

  unsigned int newTag = 0x1 << bit;
  if( decision )   *var   |= newTag;
  else             *var   &= ~newTag;
}

void TrigTauFlags::setL2(TRIGGERMENU menu, bool decision) 
{
  unsigned int bit  =menu;
  unsigned int * var = &m_L2_flags;
  if( bit > 299 ) {     bit -= 300; var  = &m_fourL2_flags;}
  else if( bit > 199 ){ bit -= 200; var  = &m_trdL2_flags;}
  else if( bit > 99 ) { bit -= 100; var  = &m_secL2_flags;}

  unsigned int newTag = 0x1 << bit;
  if( decision )   *var   |= newTag;
  else             *var   &= ~newTag;
}

void TrigTauFlags::setEF(TRIGGERMENU menu, bool decision) 
{
  unsigned int bit  =menu;
  unsigned int * var = &m_EF_flags;
  if( bit > 299 ) {     bit -= 300; var  = &m_fourEF_flags;}
  else if( bit > 199 ){ bit -= 200; var  = &m_trdEF_flags;}
  else if( bit > 99 ) { bit -= 100; var  = &m_secEF_flags;}

  unsigned int newTag = 0x1 << bit;
  if( decision )   *var   |= newTag;
  else             *var   &= ~newTag;
}


bool TrigTauFlags::TrigTauFlags::passThrough(TRIGGERMENU menu) const 
{ 
  unsigned int bit  =menu;
  if( bit<100 )
    return ((m_PT_flags >> bit) &0x1) != 0; 
  else if( bit<200 )
    return ((m_secPT_flags >> (bit-100))&0x1) != 0; 
  else  if( bit<300 )
    return ((m_trdPT_flags >> (bit-200))&0x1) != 0; 
  else 
    return ((m_fourPT_flags >> (bit-300))&0x1) != 0; 
}

void TrigTauFlags::setPT(TRIGGERMENU menu, bool passThrough) 
{
  unsigned int bit  =menu;
  unsigned int * var = &m_PT_flags;
  if( bit > 299 ) {     bit -= 300; var  = &m_fourPT_flags;}
  else if( bit > 199 ){ bit -= 200; var  = &m_trdPT_flags;}
  else if( bit > 99 ) { bit -= 100; var  = &m_secPT_flags;}

  unsigned int newTag = 0x1 << bit;
  if( passThrough )   *var   |= newTag;
  else             *var   &= ~newTag;
}

bool TrigTauFlags::TrigTauFlags::rerun(TRIGGERMENU menu) const 
{ 
  unsigned int bit  =menu;
  if( bit<100 )
    return ((m_rerun_flags >> bit) &0x1) != 0; 
  else if( bit<200 )
    return ((m_secRerun_flags >> (bit-100))&0x1) != 0; 
  else  if( bit<300 )
    return ((m_trdRerun_flags >> (bit-200))&0x1) != 0; 
  else 
    return ((m_fourRerun_flags >> (bit-300))&0x1) != 0; 
}

void TrigTauFlags::setRerun(TRIGGERMENU menu, bool rerun) 
{
  unsigned int bit  =menu;
  unsigned int * var = &m_rerun_flags;
  if( bit > 299 ) {     bit -= 300; var  = &m_fourRerun_flags;}
  else if( bit > 199 ){ bit -= 200; var  = &m_trdRerun_flags;}
  else if( bit > 99 ) { bit -= 100; var  = &m_secRerun_flags;}

  unsigned int newTag = 0x1 << bit;
  if( rerun )   *var   |= newTag;
  else             *var   &= ~newTag;
}

inline bool  TrigTauFlags::failedL1() const
{return m_L1_flags==0 || m_secL1_flags ==0 || m_trdL1_flags==0 || m_fourL1_flags ==0;}
inline bool  TrigTauFlags::failedL2() const
{return m_L2_flags==0 || m_secL2_flags ==0 || m_trdL2_flags==0 || m_fourL2_flags ==0;}
inline bool  TrigTauFlags::failedEF() const
{return m_EF_flags==0 || m_secEF_flags ==0 || m_trdEF_flags==0 || m_fourEF_flags ==0;}


bool TrigTauFlags::isTau_L2AuthorType(HLTAUTHOR type) const {
    switch (type) {
    case ANYAUTHOR:
      return true;
    case ALLDETECTORS:
      return isTau_L2AllDetectors();
    case CALOONLY:
      return isTau_L2CaloOnly();
    case COSMICS:
      return isTau_L2Cosmics();
    case IDONLY:
      return isTau_L2IDOnly();
    case IDSCAN:
      return isTau_L2IDSCAN();
    case SITRK:
      return isTau_L2SITRK();
    case TRTONLY:
      return isTau_L2TRTONLY();
    default:
      break;
    }
    return false;  // Invalid author type
  }

bool TrigTauFlags::isTau_AuthorType(HLTAUTHOR type) const {
    switch (type) {
    case ANYAUTHOR:
      return true;
    case ALLDETECTORS:
      return isTau_allDetectors();
    case COSMICS:
      return isTau_Cosmics();
    case CALOONLY:
      return isTau_CaloOnly();
    case IDONLY:
      return isTau_IDOnly();
    case IDSCAN:
      return isTau_IDSCAN();
    case SITRK:
      return isTau_SITRK();
    case TRTONLY:
      return isTau_TRTONLY();
    default:
      break;
    }
    return false;  // Invalid author type
  }

void TrigTauFlags::reset()
{ 
  m_L1_flags=0; m_L2_flags=0; m_EF_flags=0;
  m_PT_flags=0; m_rerun_flags=0;
  
  m_secL1_flags=0; m_secL2_flags=0; m_secEF_flags=0;
  m_secPT_flags=0; m_secRerun_flags=0;
  
  m_trdL1_flags=0; m_trdL2_flags=0; m_trdEF_flags=0;
  m_trdPT_flags=0; m_trdRerun_flags=0;
  
  m_fourL1_flags=0; m_fourL2_flags=0; m_fourEF_flags=0;
  m_fourPT_flags=0; m_fourRerun_flags=0;
}
  
#endif //TRIGTAUEVENTPOOL_H
