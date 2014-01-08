/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DealWithFit_H
#define DealWithFit_H

#include "MuonboyCore/MuonboyCoreMisc.h"

extern "C"
void resetdealwithfit_();

extern "C"
void resetcofito_();

extern "C"
void addtocofito_(        
        int& ISCI,int& KOUNTERDRA_i);

extern "C"
void resetcodrat_();

extern "C"
void addtocodrat_(        
        int& ISC,double& CCDRAT_i,double& FFDRAT_i,double& GGDRAT_i,double& EZDRAT_i,  
        double& SSDRAT_i1,double& ZZDRAT_i1,double& TTDRAT_i1,int& IWDRAT_i1,  
        double& SSDRAT_i2,double& ZZDRAT_i2,double& TTDRAT_i2,int& IWDRAT_i2, 
        int& JFDRAT_i,
        int& KOUNTERDRA_i);

extern "C"
void getkounterdrafix_(int& Icur,int& KOUNTERDRA_o);

extern "C"
void getnbfixe_(int& NBFIXE_o);

extern "C"
void dothefit_(int& MboyStatusInput,int& ISC0,int& ISC1,int& iok);


#endif
