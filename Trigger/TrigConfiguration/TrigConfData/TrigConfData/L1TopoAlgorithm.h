/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
   class L1TopoAlgorithm final : virtual public DataStructure {
   public:

      /** Constructor */
      L1TopoAlgorithm();

      /** Constructor initialized with configuration data 
       * @param data The data containing the L1Topo menu 
       */
      L1TopoAlgorithm(const ptree & data);

      /** Destructor */
      ~L1TopoAlgorithm();

      /** Accessor to algorithm index */
      unsigned int algId() const;

      /** Accessor to algorithm name */
      const std::string & name() const;

      /** Accessor to algorithm class type */
      const std::string & type() const;

      /** Accessor to list of input data collections */
      std::vector<DataStructure> inputs() const;

      /** Accessor to output collection */
      DataStructure outputs() const;

      /** Accessor to generic parameters */
      std::vector<DataStructure> generics() const;

      /** Accessor to register parameters which can change for each algorithm instance */
      std::vector<DataStructure> parameters() const;

   };

}

#endif
