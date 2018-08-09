/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  BaseTOBoAlg.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/10/12.
//

#ifndef __TopoCore__ConfigurableAlg__
#define __TopoCore__ConfigurableAlg__

#include <vector>
#include <string>
#include "L1TopoCoreSimulation/ParameterArray.h"
#include "L1TopoCoreSimulation/AlgFactory.h"
#include "L1TopoCoreSimulation/StatusCode.h"

namespace TCS {

   class ConfigurableAlg {
   public:
      ConfigurableAlg(const std::string & name);
      virtual ~ConfigurableAlg(){};

      const std::string & name() const { return m_name; }

      // reset
      StatusCode reset() { return doReset(); }

      virtual bool isSortingAlg() const { return false; }
      virtual bool isDecisionAlg() const { return false; }

      // set parameters by user
      void setParameter(const Parameter &);
      void setParameter(const std::string & name, TCS::parType_t value);
      void setParameters(const ParameterArray &);
      
      // define parameters by developer
      void defineParameter(const Parameter &);
      void defineParameter(const std::string & name, TCS::parType_t value);
      
      // const access to parameter
      const Parameter & parameter(const std::string & parameterName) const;
      const ParameterArray & parameters() const { return m_parameters; }

   protected:

      virtual StatusCode doReset() = 0;
      
   private:
      std::string    m_name;
      ParameterArray m_parameters;
   };

} // end of namespace TCS

std::ostream & operator<<(std::ostream &, const TCS::ConfigurableAlg &);

#endif /* defined(__TopoCore__TopoAlg__) */
