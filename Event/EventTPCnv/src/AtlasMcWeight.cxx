/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AtlasMcWeight.cxx
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov>, Borut Kersevan <borut.kersevan@cern.ch>
 *
 * $Id: AtlasMcWeight.cxx 566179 2013-10-20 09:27:11Z schaffer $
 */
#include <iostream>
#include <limits>

#include <cmath>

#include "EventTPCnv/AtlasMcWeight.h"

/* #define AMC_DEBUG 1 */

namespace {
    const int NPLACES = int(floor(log10(double(std::numeric_limits<AtlasMcWeight::number_type>::max()))));
    const int ONEBILLION = 1000000000;
    const int HUNDREDMIL = 100000000;
    enum { NUMP_EXPP, NUMNOTP_EXPP, NUMP_EXPNOTP, NUMNOTP_EXPNOTP };
}


double 
AtlasMcWeight::decode(number_type binnedWeight) {
    short d_pref = (short)(binnedWeight/ONEBILLION);
    int   d_rest = (int)(binnedWeight % ONEBILLION);
    short d_exp  = (short)(d_rest/HUNDREDMIL);
    int   d_prec = d_rest % HUNDREDMIL;

    switch(d_pref) {
        case NUMP_EXPP:
            break;
        case NUMNOTP_EXPP:
            d_prec = -d_prec;
            break;
        case NUMP_EXPNOTP:
            d_exp  = -d_exp;
            break;
        case NUMNOTP_EXPNOTP:
            d_prec = -d_prec;
            d_exp  = -d_exp;
            break;
    }
    double wt_decode=d_prec*pow(10,d_exp-NPLACES+1);
    //decoding
#ifdef AMC_DEBUG
    std::cout << "d_pref=" << d_pref << std::endl;
    std::cout << " d_exp=" << d_exp << std::endl;
    std::cout << "d_prec=" << d_prec << std::endl;
    printf("decodd=%.8g \n",wt_decode);
#endif
    return wt_decode;
} 

