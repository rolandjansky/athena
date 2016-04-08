/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GAMMAFN_H
#define GAMMAFN_H
//#include <gsl_sf.h>
#include <gsl/gsl_sf.h>
extern "C" {
	double gammafn_(double * x);
        float fgammafn_(float * x);
        double igamma_(double * a, double * x);
}
#endif
