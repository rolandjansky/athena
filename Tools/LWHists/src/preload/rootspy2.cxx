/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//A little symbol needed by preload/rootspy.cxx for better diagnostics.

#include "TProfile.h"
#include "TProfile2D.h"
#include "TProfile3D.h"
#include "TH1K.h"
#include <iostream>

unsigned roothiststatGetSize(TH1* h);//Declaration to be 100% sure to get a symbol
unsigned roothiststatGetSize(TH1* h)
{
  static bool first=true;
  if (first) {
    first=false;
    std::cout<<"ROOTSPY Using proper roothiststatGetSize. GOOD!"<<std::endl;
  }
  if (!h)
    return 0;
  //We are just looking at the per/bin contributions (ignoring axis labels, etc.)
  unsigned e = h->GetSumw2N()*sizeof(Double_t);
  unsigned c = 0;
  if (dynamic_cast<TProfile*>(h)) c=static_cast<TProfile*>(h)->GetSize()*sizeof(double)*2;
  else if (dynamic_cast<TProfile2D*>(h)) c=static_cast<TProfile2D*>(h)->GetSize()*sizeof(double)*2;
  else if (dynamic_cast<TProfile3D*>(h)) c=static_cast<TProfile3D*>(h)->GetSize()*sizeof(double)*2;
  else if (dynamic_cast<TH1F*>(h)) c=static_cast<TH1F*>(h)->GetSize()*sizeof(static_cast<TH1F*>(h)->fArray[0]);
  else if (dynamic_cast<TH1D*>(h)) c=static_cast<TH1D*>(h)->GetSize()*sizeof(static_cast<TH1D*>(h)->fArray[0]);
  else if (dynamic_cast<TH1I*>(h)) c=static_cast<TH1I*>(h)->GetSize()*sizeof(static_cast<TH1I*>(h)->fArray[0]);
  else if (dynamic_cast<TH1S*>(h)) c=static_cast<TH1S*>(h)->GetSize()*sizeof(static_cast<TH1S*>(h)->fArray[0]);
  else if (dynamic_cast<TH1C*>(h)) c=static_cast<TH1C*>(h)->GetSize()*sizeof(static_cast<TH1C*>(h)->fArray[0]);
  else if (dynamic_cast<TH1K*>(h)) c=static_cast<TH1K*>(h)->GetSize()*sizeof(static_cast<TH1K*>(h)->fArray[0]);
  else if (dynamic_cast<TH2F*>(h)) c=static_cast<TH2F*>(h)->GetSize()*sizeof(static_cast<TH2F*>(h)->fArray[0]);
  else if (dynamic_cast<TH2D*>(h)) c=static_cast<TH2D*>(h)->GetSize()*sizeof(static_cast<TH2D*>(h)->fArray[0]);
  else if (dynamic_cast<TH2I*>(h)) c=static_cast<TH2I*>(h)->GetSize()*sizeof(static_cast<TH2I*>(h)->fArray[0]);
  else if (dynamic_cast<TH2S*>(h)) c=static_cast<TH2S*>(h)->GetSize()*sizeof(static_cast<TH2S*>(h)->fArray[0]);
  else if (dynamic_cast<TH2C*>(h)) c=static_cast<TH2C*>(h)->GetSize()*sizeof(static_cast<TH2C*>(h)->fArray[0]);
  else if (dynamic_cast<TH3F*>(h)) c=static_cast<TH3F*>(h)->GetSize()*sizeof(static_cast<TH3F*>(h)->fArray[0]);
  else if (dynamic_cast<TH3D*>(h)) c=static_cast<TH3D*>(h)->GetSize()*sizeof(static_cast<TH3D*>(h)->fArray[0]);
  else if (dynamic_cast<TH3I*>(h)) c=static_cast<TH3I*>(h)->GetSize()*sizeof(static_cast<TH3I*>(h)->fArray[0]);
  else if (dynamic_cast<TH3S*>(h)) c=static_cast<TH3S*>(h)->GetSize()*sizeof(static_cast<TH3S*>(h)->fArray[0]);
  else if (dynamic_cast<TH3C*>(h)) c=static_cast<TH3C*>(h)->GetSize()*sizeof(static_cast<TH3C*>(h)->fArray[0]);

  return c ? e+c : 0;//0 to indicate that we don't know this type of ROOT hist
}
