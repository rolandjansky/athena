/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1TOPOALGORITHM_H
#define TRIGCONFDATA_L1TOPOALGORITHM_H

#include "TrigConfData/DataStructure.h"
#include <vector>
#include <optional>

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
      public:
         VariableParameter(const std::string & name, int value, std::optional<unsigned int> selection = std::nullopt)
            : m_name(name), m_value(value), m_selection(selection) {}
         const std::string & name() const { return m_name; }
         int value() const { return m_value; }
         unsigned int selection() const { return m_selection.value_or(0); }
         std::optional<unsigned int> selection_optional() const { return m_selection; }
      private:
         std::string m_name{""};
         int m_value{0};
         std::optional<unsigned int> m_selection{};
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

   protected:

      virtual void update() override { load(); }

   private:

      /** Update the internal data after modification of the data object */
      void load();

      AlgorithmType m_type{ AlgorithmType::UNKNOWN };

      std::string m_category{};

      std::vector<std::string> m_inputs{};
      std::vector<std::string> m_outputs{};

      std::vector<VariableParameter> m_parameters;

   };

   class XEFlavour {
   public:
      /** DO NOT CHANGE! Need to match with firmware values */
      enum class FLAV { 
          jXE     = 0,
          jXEC    = 1, 
          jTE     = 2, 
          jTEC    = 3, 
          jTEFWD  = 4, 
          jTEFWDA = 5, 
          jTEFWDC = 6, 
          gXEJWOJ = 7, 
          gXERHO  = 8, 
          gXENC   = 9, 
          gTE     = 10,
          gMHT    = 11,
          jXEPerf = 12, // Only for simulation studies!
      };

      static std::string flavourIntToStr(const unsigned int flavInt){
          if(flavInt==int(FLAV::jXE))     return "jXE";
          if(flavInt==int(FLAV::jXEC))    return "jXEC";
          if(flavInt==int(FLAV::jTE))     return "jTE";
          if(flavInt==int(FLAV::jTEC))    return "jTEC";
          if(flavInt==int(FLAV::jTEFWD))  return "jTEFWD";
          if(flavInt==int(FLAV::jTEFWDA)) return "jTEFWDA";
          if(flavInt==int(FLAV::jTEFWDC)) return "jTEFWDC";
          if(flavInt==int(FLAV::gXEJWOJ)) return "gXEJWOJ";
          if(flavInt==int(FLAV::gXERHO))  return "gXERHO";
          if(flavInt==int(FLAV::gXENC))   return "gXENC";
          if(flavInt==int(FLAV::gTE))     return "gTE";
          if(flavInt==int(FLAV::gMHT))    return "gMHT";
          if(flavInt==int(FLAV::jXEPerf))     return "jXEPerf";
          throw std::runtime_error("Flavour " + std::to_string(flavInt) + " for EnergyThreshold algorithm not recongnised!");
      };
      static unsigned int flavourStrToInt(const std::string & flavStr){
          if(flavStr=="jXE")     return int(FLAV::jXE);
          if(flavStr=="jXEC")    return int(FLAV::jXEC);
          if(flavStr=="jTE")     return int(FLAV::jTE);
          if(flavStr=="jTEC")    return int(FLAV::jTEC);
          if(flavStr=="jTEFWD")  return int(FLAV::jTEFWD);
          if(flavStr=="jTEFWDA") return int(FLAV::jTEFWDA);
          if(flavStr=="jTEFWDC") return int(FLAV::jTEFWDC);
          if(flavStr=="gXE")     return int(FLAV::gXEJWOJ); // for backward compatibility
          if(flavStr=="gXEJWOJ") return int(FLAV::gXEJWOJ);
          if(flavStr=="gXERHO")  return int(FLAV::gXERHO);
          if(flavStr=="gXEPUFIT") return int(FLAV::gXENC); // for backward compatibility
          if(flavStr=="gXENC")   return int(FLAV::gXENC);
          if(flavStr=="gTE")     return int(FLAV::gTE);
          if(flavStr=="gMHT")    return int(FLAV::gMHT);
          if(flavStr=="jXEPerf")     return int(FLAV::jXEPerf);
          throw std::runtime_error("Flavour " + flavStr + " for EnergyThreshold algorithm not recongnised!");
      };

   };

}

#endif
