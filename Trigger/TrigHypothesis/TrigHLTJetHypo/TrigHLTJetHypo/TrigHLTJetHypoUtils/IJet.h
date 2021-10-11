/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IJET_H
#define TRIGHLTJETHYPO_IJET_H

#include "xAODJet/Jet.h"
#include <string>
#include <ostream>
#include <optional>

class TLorentzVector;

namespace HypoJet{
  class IJet{
  public:
    virtual ~IJet (){};
    virtual double pt() const = 0;
    virtual double eta() const = 0;
    virtual double phi() const = 0;
    virtual double m() const = 0;
    virtual double e() const = 0;
    virtual double et() const = 0;
    virtual double rapidity() const = 0;
    virtual TLorentzVector p4() const = 0;
    virtual unsigned int position() const = 0;
    virtual std::optional<const xAOD::Jet*> xAODJet() const = 0;
      
    virtual bool getAttribute(const std::string &name, float&) const =0;
    virtual bool getAttribute(const std::string &name, bool&) const =0;
    virtual std::string toString() const = 0;
  };
}
#endif
