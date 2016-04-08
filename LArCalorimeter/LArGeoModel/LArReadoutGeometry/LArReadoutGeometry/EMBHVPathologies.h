/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _EMBHVPathologies_h_
#define _EMBHVPathologies_h_
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/ConstLink.h"
// --------------------------------------------------------
// This class describes ugly things that can happen to an
// EMB Electrode. We foresee extending it as we learn of
// more badness.
//--------------------------------------------------------


class EMBHVPathologies: public RCBase {

 public:
  
  // Constructor:
  EMBHVPathologies(unsigned int electrodeNumber,
		  bool isShorted,
		  bool isDisconnected);

  // Destructor():
  virtual ~EMBHVPathologies();

  // The electrode number:
  const unsigned int & electrodeNumber() const;

  // Is shorted:
  const bool &isShorted() const;

  // Is disconnected();
  const bool & isDisconnected() const;

 private:

  // Illegal operations:
  EMBHVPathologies(const EMBHVPathologies & );
  EMBHVPathologies & operator=(const EMBHVPathologies & );
  
  // Internals:
  class Clockwork;
  Clockwork *m_c;

};


typedef ConstLink< EMBHVPathologies  > EMBHVPathologiesConstLink;



#endif
