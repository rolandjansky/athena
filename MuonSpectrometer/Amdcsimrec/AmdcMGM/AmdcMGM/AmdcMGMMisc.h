/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#ifndef AmdcMGMMISC_H
#define AmdcMGMMISC_H

#ifdef HAVE_NEW_IOSTREAMS
//gcc3.2
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#else
//gcc 295
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#endif
#if HAVE_CXX_STDC_HS
# include <cstdio>
# include <cmath>
#else
# include <stdio.h>
# include <math.h>
#endif
#include <algorithm>
#include <iterator>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <utility>

#endif
