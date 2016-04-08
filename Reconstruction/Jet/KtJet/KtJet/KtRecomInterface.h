/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef KTJET_KTRECOMINTERFACE_H
#define KTJET_KTRECOMINTERFACE_H

#include <string>
#include "KtJet/KtUtil.h"


namespace KtJet {
  class KtLorentzVector;
  /**
   *  Interface class to combine 4-momenta
   *  @author J.Butterworth J.Couchman B.Cox B.Waugh
  */
  class KtRecom {
  public:
    /** virtual destructor needed */
    virtual ~KtRecom() {}
    /** Return merged 4-momentum */
    virtual CLHEPNAMESPACE HepLorentzVector operator()(const CLHEPNAMESPACE HepLorentzVector &, const CLHEPNAMESPACE HepLorentzVector &) const = 0;
    /** Process input 4-momentum */
    virtual KtLorentzVector operator()(const KtLorentzVector &) const = 0;
    /** Name of scheme */
    virtual std::string name() const = 0;
  };
  
}

#endif //end of namespace
