/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1TOPOALGORITHM_H
#define TRIGCONFDATA_L1TOPOALGORITHM_H

#include "TrigConfData/DataStructure.h"
#include <vector>

namespace TrigConf {

   /** 
    * @brief L1Topo algorithm configuration
    *
    * Provides access to the L1Topo algorithm name and type and the selection parameters,
    * input and output collections. Can hold sorting and decision algorithms
    */
   class L1TopoAlgorithm final : public DataStructure {
   public:

      enum class AlgorithmType { SORTING, DECISION, MULTIPLICITY, UNKNOWN };

      struct VariableParameter {
      VariableParameter(const std::string & _name, int _value, unsigned int _selection) 
      : name(_name), value(_value), selection(_selection) {}
         std::string name{""};
         int value{0};
         unsigned int selection{0};
      };

      /** Constructor */
      L1TopoAlgorithm() = default;

      L1TopoAlgorithm(const L1TopoAlgorithm &) = delete;
      L1TopoAlgorithm& operator=(const L1TopoAlgorithm&) = delete;
      L1TopoAlgorithm(L1TopoAlgorithm&&) = default;

      /** Constructor initialized with configuration data 
       * @param data The data containing the L1Topo menu 
       */
      L1TopoAlgorithm(const std::string & algoName, AlgorithmType algoType, const std::string & algoCategory, const ptree & data);

      /** Destructor */
      ~L1TopoAlgorithm();

      virtual std::string className() const override;

      /** Accessor to algorithm index */
      unsigned int algId() const;

      AlgorithmType type() const;

      const std::string & category() const;

      /** Accessor to algorithm class type */
      const std::string & klass() const;

      /** Accessor to input collections
       * Sorting and Multiplicity algorithms have only one input
       */
      const std::vector<std::string> & inputs() const;

      /** Accessor to output collections
       * Sorting and Multiplicity algorithms have only one output
       */
      const std::vector<std::string> & outputs() const;
      std::vector<std::string> fullOutputs() const;

      /** Accessors to generic parameters */
      DataStructure generics() const;

      std::string genericParameter(const std::string & parName) const;

      template<class T>
      T genericParameter(const std::string & parName) const {
         return getAttribute<T>("fixedParameters.generics." + parName + ".value");
      }

      /** Accessor to register parameters which can change for each algorithm instance */
      const std::vector<VariableParameter> & parameters() const;

      /** print main info */
      void print(std::ostream & os = std::cout) const override;

   private:

      /** Update the internal data after modification of the data object */
      virtual void update() override;

      AlgorithmType m_type{ AlgorithmType::UNKNOWN };

      std::string m_category{};

      std::vector<std::string> m_inputs{};
      std::vector<std::string> m_outputs{};

      std::vector<VariableParameter> m_parameters;

   };

}

#endif
