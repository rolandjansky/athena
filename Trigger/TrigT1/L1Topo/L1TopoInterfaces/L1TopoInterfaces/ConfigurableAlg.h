//  BaseTOBoAlg.h
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef TCS_ConfigurableAlg
#define TCS_ConfigurableAlg

#include <vector>
#include <string>
#include "L1TopoInterfaces/Parameter.h"
#include "L1TopoInterfaces/ParameterSpace.h"
#include "L1TopoInterfaces/AlgFactory.h"
#include "L1TopoCommon/StatusCode.h"

namespace TCS {

   class ConfigurableAlg {
   protected:
      enum AlgType { NONE, SORT, DECISION };

   public:

      // constructor
      ConfigurableAlg(const std::string & name, AlgType algType);

      // destructor
      virtual ~ConfigurableAlg();

      // setting the class name
      void setClassName(const std::string & className) { m_className = className; }

      // accessors
      const std::string & name() const { return m_name; }
      const std::string & className() const { return m_className; }
      std::string fullname() const { return m_className + "/" + m_name; }

      // the index of the algorithm in the hardware
      unsigned int algoId() const { return m_algoId; }

      // reset
      virtual StatusCode initialize() { return StatusCode::SUCCESS;}

      StatusCode reset() { return doReset(); }

      bool isSortingAlg() const { return m_algType == SORT; }

      bool isDecisionAlg() const { return m_algType == DECISION; }

      // const access to parameter
      const Parameter & parameter(const std::string & parameterName) const;

      const Parameter & parameter(const std::string & parameterName, unsigned int selection) const;

      const ParameterSpace & parameters() const { return m_parameters; }

      // set parameters from configuration
      void setParameters(const ParameterSpace &);
      
      // setters
      void setAlgoId(unsigned int algoId) { m_algoId = algoId; }

   protected:

      // define parameters by developer
      void defineParameter(const std::string & name, TCS::parType_t value);

      void defineParameter(const std::string & name, TCS::parType_t value, unsigned int selection);
      
      virtual StatusCode doReset() = 0;
      
   private:

      void defineParameter(const Parameter &);

      std::string    m_name {""};
      std::string    m_className {""};

      // just for the hardware
      unsigned int m_algoId {0};

      ParameterSpace m_parameters {""};

      AlgType  m_algType; // stores type of alg (Sorting or Decision)


   };

} // end of namespace TCS

std::ostream & operator<<(std::ostream &, const TCS::ConfigurableAlg &);

#endif /* defined(__TopoCore__TopoAlg__) */
