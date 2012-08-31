/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  @file TgcHpt.h
*
*    authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)             
*/
#ifndef __Tgc_Hpt_H__
#define __Tgc_Hpt_H__

class TgcHpt
{

public:
  TgcHpt () ;
  virtual ~TgcHpt ();

  //set functions

  //clear function
  void clear ();

public:
  bool isAside;
  bool isForward;
  int phi;
  int trackletId;
  int sub;
  int delta;



};
#endif
