/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
    @class AcerMC_acset 
     AcerMC acset common block access
     @author Borut Paul Kersevan (February 2003)
*/
#ifndef AcerMC_acset_h
#define  AcerMC_acset_h
extern "C" {
  void* acermc_acset_address_(void);
}
class AcerMC_acset {
 public:
  AcerMC_acset();
  ~AcerMC_acset();
  int& acset12();
  inline void init(); 
 private:
  struct ACERMC_ACSET;
  friend struct ACERMC_ACSET;
  struct ACERMC_ACSET {
    int acset12;
  };
  static ACERMC_ACSET* s_acermc_acset;
};
#include "AcerMC_i/AcerMC_acset.icc"
#endif


