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

      enum class AlgorithmType { SORTING, DECISION, MULTIPLICITY, INIT };

      /** Constructor */
      L1TopoAlgorithm();

      L1TopoAlgorithm(const L1TopoAlgorithm &) = delete;
      L1TopoAlgorithm& operator=(const L1TopoAlgorithm&) = delete;
      L1TopoAlgorithm(L1TopoAlgorithm&&) = default;

      /** Constructor initialized with configuration data 
       * @param data The data containing the L1Topo menu 
       */
      L1TopoAlgorithm(const std::string & algoName, AlgorithmType algoType, const ptree & data);

      /** Destructor */
      ~L1TopoAlgorithm();

      virtual std::string className() const override;

      /** Accessor to algorithm index */
      unsigned int algId() const;

      AlgorithmType type() const;

      /** Accessor to algorithm class type */
      const std::string & klass() const;

      /** Accessor to list of input data collections */
      std::vector<DataStructure> inputs() const;

      /** Accessor to output collection */
      const std::vector<std::string> & outputs() const;

      /** Accessor to generic parameters */
      std::vector<DataStructure> generics() const;

      /** Accessor to register parameters which can change for each algorithm instance */
      std::vector<DataStructure> parameters() const;

   private:

      /** Update the internal data after modification of the data object */
      virtual void update() override;

      AlgorithmType m_type{ AlgorithmType::INIT };

      std::vector<std::string> m_outputs{};

   };

}

#endif
