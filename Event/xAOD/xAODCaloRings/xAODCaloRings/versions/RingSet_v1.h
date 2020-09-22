// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_VERSIONS_RINGSET_V1_H
#define XAODCALORINGS_VERSIONS_RINGSET_V1_H

// std includes:
#include <vector>
#include <iosfwd>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "xAODCaloRings/CaloRingsDefs.h"

namespace xAOD {

/**
 * @class xAOD::RingSet_v1
 * @brief Class holding a set of rings.
 *
 * @name xAOD::RingSet provides a container which holds a set of rings.
 * Usually this set holds the rings for a specific layer of the calorimeter.
 *
 * @author Werner Spolidoro Freund <wsfreund@cern.ch>
 *             (old and new EDM implementation)
 * @author Danilo Enoque Ferreira de Lima
 *             (first implementation)
 *
 **/
class RingSet_v1 : public SG::AuxElement {

  public:

    /// @{
    /// Iterator to rings Et
    typedef std::vector<float>::iterator iterator;
    /// const Iterator to rings Et
    typedef std::vector<float>::const_iterator const_iterator;
    /// @}


    /// @name RingSet_v1 ctors
    /// @{
    /**
     * @brief Default empty constructor
     **/
    RingSet_v1();
    /**
     * @brief Copy constructor
     **/
    RingSet_v1(const RingSet_v1& ringset);
    /**
     * @brief Initialize RingSet with rings std::vector assignment
     **/
    RingSet_v1(const size_t size, const float value = 0);
    /// @}

    /// @name RingSet_v1 dtors:
    /// @{
    /// @}

    /// @name RingSet_v1 accessors:
    /// @{
    /**
     * @brief Get/set ring Et at ith position
     **/
    float &at(const unsigned int i); 
    /**
     * @brief Get/set ring Et at ith position
     **/
    float &operator [](const unsigned int i); 
    /**
     * @brief Get/set ring Et at ith position
     **/
    float at(const unsigned int i) const; 
    /**
     * @brief Get/set ring Et at ith position
     **/
    float operator [](const unsigned int i) const; 
    /**
     * @brief Get rings holden by RingSet
     **/
    const std::vector<float>& ringsE() const;
    /**
     * @brief Get/set rings holden by RingSet
     **/
    void setRingsE(const std::vector<float>&);
    /// @}
    
    /// @name RingSet_v1 rings interation:
    /// @{
    /**
     * @brief Get rings size
     **/
    unsigned size() const; 
    /**
     * @brief clear rings
     **/
    void clear(); 
    /**
     * @brief add ring to RingSet
     **/
    void addRing(float r); 
    /**
     * @brief ring begin() iterator
     **/
    iterator begin();
    /**
     * @brief ring end() iterator
     **/
    iterator end();
    /**
     * @brief ring begin() const_iterator
     **/
    const_iterator begin() const;
    /**
     * @brief ring end() const_iterator
     **/
    const_iterator end() const;
    /**
     * @brief Copy ringset to std::vector end:
     **/ 
    void copyTo(std::vector<float> &vec) const;
    /// @}

    /// @name RingSet_v1 operators overload:
    /// @{
    /** 
     * @brief Assignment Operator. Using the assignment of SG::AuxElement.
     **/
    RingSet_v1& operator=(const RingSet_v1& clrings);
    /// @}

    /// @{ Print @name RingSet_v1 methods:
    /**
     * @brief Print @name RingSet_v1 using ostream:
     **/
    void print( std::ostream &stream ) const;
    /// @}

};

/// @name RingSet_v1 ctors:
/// @{
//==============================================================================
inline
RingSet_v1::RingSet_v1(){;}

//==============================================================================
inline
RingSet_v1::RingSet_v1(const RingSet_v1& ringset) : 
  SG::AuxElement(ringset)
{
  this->makePrivateStore(ringset);
}

//==============================================================================
inline
RingSet_v1::RingSet_v1(const size_t size, const float value)
{
  if (!this->container() && !this->hasStore() ) {
    makePrivateStore();
  }
  setRingsE(std::vector<float>(size,value));
}
/// @}



} // End of namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE ( xAOD::RingSet_v1, SG::AuxElement );

#endif // XAODCALORINGS_VERSIONS_RINGSET_V1_H
