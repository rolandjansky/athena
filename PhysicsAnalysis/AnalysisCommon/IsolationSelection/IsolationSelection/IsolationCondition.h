// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONSELECTION_ISOLATIONCONDITION_H
#define ISOLATIONSELECTION_ISOLATIONCONDITION_H

#include <memory>
#include <vector>
#include <map>
#include "xAODPrimitives/IsolationType.h"
#include <xAODPrimitives/tools/getIsolationAccessor.h>
#include "AthContainers/AuxElement.h"
#include "xAODBase/IParticle.h"

// Forward Declaration(s)
class TF1;
class TH3F;
  
struct strObj{
  float pt;
  float eta;
  std::vector< float > isolationValues;
};

namespace CP
{
  class IsolationCondition{
    public:
      IsolationCondition(std::string name, xAOD::Iso::IsolationType isoType):m_name(name),m_isolationType(isoType),m_cutValue(-999.){
        m_acc = xAOD::getIsolationAccessor(m_isolationType);
      };
      virtual ~IsolationCondition(){}
      
//       IsolationCondition() = delete;
      IsolationCondition():m_name(""),m_isolationType(xAOD::Iso::numIsolationTypes),m_acc(0),m_cutValue(-999){};
      void setName(std::string name){m_name = name;}
      void setCut(xAOD::Iso::IsolationType isoType){
        m_isolationType = isoType;
        m_acc = xAOD::getIsolationAccessor(m_isolationType);
      };

      IsolationCondition(const IsolationCondition& rhs) = delete;
      IsolationCondition& operator=(const IsolationCondition& rhs) = delete;
      
      std::string name(){return m_name;}
      virtual bool accept(const xAOD::IParticle& x, std::map< xAOD::Iso::IsolationType, float >* cutValues=0) = 0;
      virtual bool accept(const strObj& x, std::map< xAOD::Iso::IsolationType, float >* cutValues=0) = 0;

    protected:
      std::string m_name;
      xAOD::Iso::IsolationType m_isolationType;
      SG::AuxElement::Accessor< float >* m_acc; 
      float m_cutValue;
  };
}
#endif
