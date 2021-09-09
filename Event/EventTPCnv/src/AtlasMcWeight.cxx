/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AtlasMcWeight.cxx
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov>, Borut Kersevan <borut.kersevan@cern.ch>
 *
 * $Id: AtlasMcWeight.cxx 792215 2017-01-15 02:20:57Z ssnyder $
 */
#include <iostream>
#include <limits>

#include <cmath>

#include "EventTPCnv/AtlasMcWeight.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/GaudiException.h"

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


AtlasMcWeight::number_type
AtlasMcWeight::encode(double weight) { 
    int exponent = int(floor(log10(fabs(weight)))+1);
    if ( abs(exponent) > 9 ) { 
        throw GaudiException( "The MC weight abs value is bigger than 1E9,encoding failed","AtlasMcWeight", StatusCode::FAILURE ); 
    }

    number_type wt_prec = int(fabs(weight)*pow(10,(NPLACES-1-exponent)));
    number_type wt_exp = abs(exponent);
    int d_sign = ( weight > 0. ? 0 : 1 );
    int e_sign = ( exponent > 0. ? 0 : 1 );
    number_type wt_pref = d_sign+2*e_sign;

    char senc[13];
    snprintf(senc,13,"%u%u%u",wt_pref, wt_exp, wt_prec);
    number_type wt_encode=atoll(senc);

#ifdef AMC_DEBUG
    std::cout << " AtlasMcWeight::encode weight = " << weight << std::endl;
    printf("encode parts =%d %d %d \n", wt_pref, wt_exp, wt_prec);
    printf("encoded string=%s \n", senc);
    std::cout << " weight=" << weight << std::endl;
    std::cout << " str weight=" << senc << std::endl;
    std::cout << " exp weight=" << atoll(senc) << std::endl;
    std::cout << " encoded weight=" << wt_encode << std::endl;
#endif
    return wt_encode;
} 
