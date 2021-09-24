/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MODANTIKTJETMAKER_H
#define MODANTIKTJETMAKER_H

#include "TrigL0GepPerf/IJetMaker.h"

#include "TrigL0GepPerf/CustomJet.h"
#include "TrigL0GepPerf/CustomTopoCluster.h"

namespace Gep{
  class ModAntikTJetMaker: public IJetMaker
  {

  public:

    ModAntikTJetMaker(std::string alg = "ModAntikT", int nIter = 10000, float jetR = 0.4) :
      m_jetAlg{alg},
      m_nIter{nIter},
      m_jetR{jetR} {}

    std::vector<CustomJet> makeJet( const std::vector<CustomTopoCluster> &clusters);
    std::string getName() const {return m_jetAlg;}

    void setName(std::string jetAlg) { m_jetAlg = jetAlg;}
    //For running modified algorithm
    void setNIter(int nIter) { m_nIter = nIter;}
    void setJetR(float jetR) { m_jetR = jetR;}

  private:

    std::string m_jetAlg;

    // For modified anti-kT
    int m_nIter;
    float m_jetR;

  };
}

#endif

      
