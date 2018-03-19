// Emacs -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVSTRUCTUE_STANDALONENAVIGATION_H
#define TRIGNAVSTRUCTUE_STANDALONENAVIGATION_H

#include "TrigNavStructure/TrigNavStructure.h"
#include <vector>

namespace HLT{
  class StandaloneNavigation : public HLT::TrigNavStructure {
  public:
    StandaloneNavigation();
    virtual ~StandaloneNavigation();

    /**
     * @brief method serializes the helper objects allowing use of FeatureAccessHelpers w/o CLID
     * This methods should be overwritten by the derived classes
     *
     * @param output vector to place the result
     *
     * @return if true then OK else some sort of failure happened (likely badly truncated structure)     
     */
    virtual bool serializeHolders( std::vector<uint32_t>& output ) const;
    virtual bool deserializeHolders(std::vector<uint32_t>::const_iterator& start,  const std::vector<uint32_t>::const_iterator& end);

    /**
     * @brief method serializes entire navigation
     *
     * @param output vector to place the result
     *
     * @return if true then OK else some sort of failure happened (likely badly truncated structure)
     */
    virtual bool serialize( std::vector<uint32_t>& output ) const;
    virtual bool deserialize( const std::vector<uint32_t>& input );

  };
}//end of HLT namespace
#endif
