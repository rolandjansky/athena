/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LWHISTVAL_H
#define LWHISTVAL_H

class LWHistVal {
  //Trick to avoid exposing methods we don't really want to support in
  //the public interface:
  public:
  template<class THnX_LW> static void clearKeptROOTHist(THnX_LW*h) { h->clearKeptROOTHist(); }
};

#endif
