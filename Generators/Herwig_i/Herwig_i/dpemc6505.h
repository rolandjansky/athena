/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __DPEMC65_HH__
#define __DPEMC65_HH__
/* Common Block Declarations */
#include <complex>

//modifications for Dpemc
using std::string;

struct Prtype_t {
    char typepr[3];
    char typint[3];
};
extern Prtype_t* gPrtype;

struct Xsect_t {
    double gapspr, prospr, cdffac;
};
extern Xsect_t* gXsect;

struct Pdfnrm_t {
    double xpqnrm[13];
};
extern Pdfnrm_t* gPdfnrm; 


#endif
