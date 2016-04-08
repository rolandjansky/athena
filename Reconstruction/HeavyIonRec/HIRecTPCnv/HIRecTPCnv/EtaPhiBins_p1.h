/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef RECTPCNV_ETAPHIBINS_P1_H
#define RECTPCNV_ETAPHIBINS_P1_H

/********************************************************************

NAME:     EtaPhiBins_p1
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Persistent rapresentation of EtaPhiBins class
********************************************************************/

// forward declarations
class EtaPhiBinsCnv_p1;

#include <vector>

class EtaPhiBins_p1
{
 friend class EtaPhiBinsCnv_p1;
 
 public:

  /**  default constructor */
  EtaPhiBins_p1()   {};

  /** destructor */
  ~EtaPhiBins_p1();

 protected:
  
  /// basic components of EtaPhiBins_p1
  float m_EtaMin;
  float m_EtaMax;
  int m_NEtaBins;
  int m_NPhiBins;
  std::vector<unsigned char> m_h2vec;

};


#endif
