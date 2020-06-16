// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_VERSIONS_CALORINGS_V1_H
#define XAODCALORINGS_VERSIONS_CALORINGS_V1_H

// STL include(s):                          
#include <vector>
#include <iosfwd>

// EDM include(s):
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODCaloRings/RingSetContainer.h"
#include "xAODCaloRings/RingSetConf.h"

namespace xAOD {

/**
 * @class xAOD::CaloRings_v1
 * @brief Class summarizing the particle interaction throughout the
 * Calorimeter (its shower shape).
 *
 * @name xAOD::CaloRings holds the shower shape information let by
 * the particle interation with the calorimeter. This information is kept in
 * the form of concentric rings (currently the rings are formed as squares)
 * centered at the cluster energy barycenter for every layer (for now, it is
 * being used the cluster center for every layer, but this may be changed to
 * the energy barycenter on each layer).
 *
 * It holds a vector of ElementLink to its stored object (@name
 * xAOD::RingSet).
 *
 * Only the held @name xAOD::RingSet itself contains the rings energy, but it
 * may be directly accessed from @name CaloRings by using the method @name
 * CaloRings::ringAt.
 *
 * Also, it may be used to export the ringsE in a vectorized representation by
 * using the method exportRingsTo.
 * 
 * @author Werner Spolidoro Freund <wsfreund@cern.ch>
 *             (old and new EDM implementation)
 * @author Danilo Enoque Ferreira de Lima
 *             (first implementation for old EDM)
 * 
 * This package was based on xAODEgamma implementation.
 *
 **/
class CaloRings_v1 : public SG::AuxElement {

  public:

    /// @name CaloRings_v1 ctors
    /// @{
    /** 
     * @brief Default empty ctor
     **/
    CaloRings_v1(){;}
    /** 
     * @brief Default copy ctor
     **/
    CaloRings_v1(const CaloRings_v1& cr);
    /// @}
 
    /// @name CaloRings_v1 dtors
    /// @{
    ~CaloRings_v1(){;}
    /// @}

    /// @name RingSet Collection direct interation methods:
    /// @{
    /** 
     * @brief Number of RingSets ElementLinks available
     **/
    unsigned nRingSets() const;
    /** 
     * @brief Number of RingSets ElementLinks available
     **/
    unsigned size() const;
    /** 
     * @brief Add ElementLink to holden vector
     **/
    void addRingSetEL(const ElementLink<RingSetContainer_v1> &rsEL);
    /** 
     * @brief Clear RingSet EL Collection
     **/
    void clear(); 
    /** 
     * @brief returns iterator to the beginning of RingSet Collection.
     **/
    RingSetLinks::iterator begin();
    /** 
     * @brief returns iterator to the ending of RingSet EL Collection.
     **/
    RingSetLinks::iterator end();
    /** 
     * @brief returns const_iterator to the beginning of RingSet Collection.
     **/
    RingSetLinks::const_iterator begin() const;
    /** 
     * @brief returns const_iterator to the ending of RingSet EL Collection.
     **/
    RingSetLinks::const_iterator end() const;
    /** 
     * @brief Return ith RingSet
     **/
    RingSet *at(const unsigned index);
    /** 
     * @brief Return ith RingSet
     **/
    RingSet *operator [](const unsigned index);
    /** 
     * @brief Return ith RingSet
     **/
    const RingSet *at(const unsigned index) const;
    /** 
     * @brief Return ith RingSet
     **/
    const RingSet *operator [](const unsigned index) const;
    /** 
     * @brief Returns constant ringsets collection.
     *
     * Set via AUXSTORE_OBJECT_SETTER_AND_GETTER marcro.
     **/
    const RingSetLinks &ringSetLinks() const;
    /** 
     * @brief Set this CaloRings RingSet Collection.
     *
     * Set via AUXSTORE_OBJECT_SETTER_AND_GETTER marcro.
     **/
    void setRingSetLinks(const RingSetLinks &ringsets);
    /// @}

    /// @name CaloRings_v1 operators overload:
    /// @{
    /** 
     * @brief Assignment Operator. Using the assignment of SG::AuxElement.
     **/
    CaloRings_v1& operator=(const CaloRings_v1& clrings);
    /// @}

    /// Navigate through eT rings methods
    /// @{
    /** 
     * @brief Returns ring eT at RingSet rsIdx and its ringIdx value.
     **/
    float ringAt(const unsigned int rsIdx, const unsigned int ringIdx) const;
    /** 
     * @brief Returns the ringIdx'th ring eT value from the vectorized representation.
     **/
    float ringAt(const unsigned int ringIdx) const;
    /// @}

    /// Export rings eT in vectorized representation
    /// @{
    /** 
     * @brief Returns the eT rings for all RingSets.
     **/
    void exportRingsTo(std::vector<float> &ringStrip) const;
    /** 
     * @brief Returns the eT rings for RingSet index @name rsIdx.
     **/
    void exportRingsTo(std::vector<float> &ringStrip,
        const unsigned rsIdx) const;
    /** 
     * @brief Returns the eT rings from RingSet index @name rsIdxStart to @name rsIdxEnd.
     **/
    void exportRingsTo(std::vector<float> &ringStrip,
        const unsigned rsIdxStart, 
        const unsigned rsIdxEnd) const;
    /** 
     * @brief Returns the eT rings from Ringer::CalJointLayer @name layerTypeIdx to @name layerEnd.
     **/
    void exportRingsTo(std::vector<float> &ringStrip,
        const RingSetConf_v1::RawConfCollection &clRingsRawConfCol,
        const Ringer::CalJointLayer layerTypeIdx) const;
    /** 
     * @brief Returns the eT rings from Ringer::CalJointLayer @name layerTypeIdx to @name layerEnd.
     **/
    void exportRingsTo(std::vector<float> &ringStrip,
        const RingSetConf_v1::RawConfCollection &clRingsRawConfCol,
        const Ringer::CalJointLayer layerTypeStart, 
        const Ringer::CalJointLayer layerTypeEnd) const;
    /** 
     * @brief Returns the eT rings from Ringer::CalJointSection @name sectionIdx
     **/
    void exportRingsTo(std::vector<float> &ringStrip,
        const RingSetConf_v1::RawConfCollection &clRingsRawConfCol,
        const Ringer::CalJointSection sectionType) const;
    /// @}

    
    /// Print @name CaloRings_v1 methods:
    /// @{
    /**
     * @brief Print @name CaloRings_v1 using ostream:
     **/
    void print( std::ostream &stream ) const;
    /// @}
    
  private:

    /// Check if index is within range, otherwise throws overflow_error:
    void checkRingSetIndexWithinRange( unsigned index ) const;

};

/// @name CaloRings_v1 ctors
/// @{
//==============================================================================
inline
CaloRings_v1::CaloRings_v1(const CaloRings_v1& clrings) : 
  SG::AuxElement(clrings)
{
  this->makePrivateStore(clrings);
}
/// @}

//==============================================================================
inline
unsigned CaloRings_v1::size() const {
  return nRingSets();
}

//==============================================================================
inline
void CaloRings_v1::exportRingsTo(std::vector<float> &ringStrip) const 
{
  exportRingsTo(ringStrip, 0, size()-1);
}

//==============================================================================
inline
void CaloRings_v1::exportRingsTo(
    std::vector<float> &ringStrip,
    const unsigned rsIdx) const 
{
  exportRingsTo(ringStrip, rsIdx, rsIdx);
}

//==============================================================================
inline
void CaloRings_v1::exportRingsTo(
    std::vector<float> &ringStrip,
    const RingSetConf_v1::RawConfCollection &clRingsRawConfCol,
    const Ringer::CalJointLayer layerTypeIdx) const 
{
  exportRingsTo(ringStrip, clRingsRawConfCol, layerTypeIdx, layerTypeIdx);
}
/// @}

} // End of namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE ( xAOD::CaloRings_v1, SG::AuxElement );

#endif // XAODCALORINGS_VERSIONS_CALORINGS_V1_H
