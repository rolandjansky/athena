/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_VERSIONS_RINGSETCONF_V1_H
#define XAODCALORINGS_VERSIONS_RINGSETCONF_V1_H

// std includes:
#include <vector>
#include <iosfwd>

// Athena framework includes:
#include "CaloGeoHelpers/CaloSampling.h"
#include "AthContainers/DataVector.h"

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "xAODCore/AuxStoreAccessorMacros.h"

// local includes:
#include "xAODCaloRings/CaloRingsDefs.h"

namespace xAOD {

// Forward declare the container type:
class RingSetConf_v1;
typedef DataVector< xAOD::RingSetConf_v1 > RingSetConfContainer_v1;

/**
 * @class xAOD::RingSetConf_v1
 * @brief Class holding the RingSet configuration used for the Reconstruction.
 *
 * @author Werner Spolidoro Freund <wsfreund@cern.ch>
 *
 **/
class RingSetConf_v1 : public SG::AuxElement {

  public:

    /// @name RingSetConf_v1 nested types:
    /// @{ 
    /**
     * @class xAOD::RingSet_v1::RawConf
     * @brief Declare a configuration param struct
     *
     * This simple structure may be used to hold the raw
     * information within the RingSetConf, or either be used to
     * set the RingSetConf information.
     **/
    struct RawConf {
      /// Ringset number of Rings
      unsigned nRings;    
      /// Ringset layers
      std::vector<CaloSampling::CaloSample> layers;
      /// Ring Width in eta
      float etaWidth;   
      /// Ring Width in phi
      float phiWidth;   
      /// Maximum distance cell in eta to seed
      float cellMaxDEtaDist;   
      /// Maximum distance cell in phi to seed
      float cellMaxDPhiDist;   
      /// The Calorimeter Joint Layer of this RingSet (see CaloRingsDefs.h)
      Ringer::CalJointLayer calJointLayer;
      /// The Calorimeter Joint Section (see CaloRingsDefs.h)
      Ringer::CalJointSection calJointSection;
      /// Hold the option of eta axes division.
      bool doEtaAxesDivision;
      /// Hold the option of phi axes division.
      bool doPhiAxesDivision;
      /// Hold the start of this layer on a non-segmented representation:
      unsigned layerStartIdx;
      /// Hold the end of this layer on a non-segmented representation:
      unsigned layerEndIdx;
      /// Hold the start of this section on a non-segmented representation:
      unsigned sectionStartIdx;
      /// Hold the end of this section on a non-segmented representation:
      unsigned sectionEndIdx;

      /// Configuration Struct ctors
      /// @{
      /**
       * @brief Empty ctor
       **/
      RawConf();
      /**
       * @brief Build referencing core variables
       *
       *  This constructor may be used in conjuction to
       *  CaloRingsConf_v1::addRawConfColBounderies to obtain a complete
       *  RawConf struct.
       **/
      RawConf(const unsigned nRings, 
          const std::vector<CaloSampling::CaloSample> &layers,
          const float etaWidth, const float phiWidth,
          const float cellMaxDEtaDist, const float cellMaxDPhiDist,
          const Ringer::CalJointLayer calJointLayer,
          const Ringer::CalJointSection calJointSection,
          const bool doEtaAxesDivision = false,
          const bool doPhiAxesDivision = false);
      /**
       * @brief Build referencing each holden variable
       **/
      RawConf(const unsigned nRings, 
          const std::vector<CaloSampling::CaloSample> &layers,
          const float etaWidth, const float phiWidth,
          const float cellMaxDEtaDist, const float cellMaxDPhiDist,
          const Ringer::CalJointLayer calJointLayer,
          const Ringer::CalJointSection calJointSection,
          const bool doEtaAxesDivision,
          const bool doPhiAxesDivision,
          const unsigned layerStartIdx,
          const unsigned layerEndIdx,
          const unsigned sectionStartIdx,
          const unsigned sectionEndIdx);
      /**
       * @brief Build from the detailed class
       **/
      RawConf(const RingSetConf_v1 &rsConf);
      /// @}

    };

    /// typedef The raw configuration structure data holder 
    typedef std::vector<RawConf> RawConfCollection;
    /// @}

    /// Static methods
    /// @{
    /// Check if this @name RingSetConf_v1 is at a specific calorimeter
    /// section/layer
    /// @{
    static bool isEMSection (const std::vector<CaloSampling::CaloSample> &layers);
    static bool isHADSection(const std::vector<CaloSampling::CaloSample> &layers);
    static bool isPSLayer   (const std::vector<CaloSampling::CaloSample> &layers);
    static bool isEM1Layer  (const std::vector<CaloSampling::CaloSample> &layers);
    static bool isEM2Layer  (const std::vector<CaloSampling::CaloSample> &layers);
    static bool isEM3Layer  (const std::vector<CaloSampling::CaloSample> &layers);
    static bool isHAD1Layer (const std::vector<CaloSampling::CaloSample> &layers);
    static bool isHAD2Layer (const std::vector<CaloSampling::CaloSample> &layers);
    static bool isHAD3Layer (const std::vector<CaloSampling::CaloSample> &layers);
    /// @}
    /// Returns the section/layer for this specific @name RingSetConf_v1
    /// @{
    static Ringer::CalJointSection whichSection(
        const std::vector<CaloSampling::CaloSample> &layers);
    static Ringer::CalJointLayer whichLayer(
        const std::vector<CaloSampling::CaloSample> &layers);
    /// @}
    /// Prints rawConf
    /// @{
    static void print( const RawConf &raw, std::ostream &stream);
    /// @}
    /**
     * Get the totalNumber of Rings contained in this configuration:
     **/
    static unsigned totalNumberOfRings( const RawConfCollection &clRingsConf );
    /**
     * Get CalJointSection start and end edges
     **/
    static void getEdges( const RawConfCollection &clRingsConf,
        const Ringer::CalJointSection sec,
        unsigned &startEdge,
        unsigned &endEdge);
    /**
     * Get CalJointLayer start and end edges
     **/
    static void getEdges( const RawConfCollection &clRingsConf,
        const Ringer::CalJointLayer layer,
        unsigned &startEdge,
        unsigned &endEdge);
    /**
     * Add to RawConfCollection its JointLayer/JointSection bounderies
     **/
    static void addRawConfColBounderies( RawConfCollection &clRingsConf );
    /// Prints RawConfCollection
    /// @{
    static void print( const RawConfCollection &raw, std::ostream &stream );
    /// @}
    /**
     * Creates RingSetConfContainer from RawConfCollection
     **/
    static void fillRingSetConfContainer(
        const RawConfCollection &rawConfCol,
        RingSetConfContainer_v1 *container);
    /**
     * Retrieve RawConfCollection from RingSetConf container.
     **/
    static void getRawConfCol(RawConfCollection &rawConfCol,
        const RingSetConfContainer_v1 * container);
    /// @}
    

    /// @name RingSetConf_v1 ctors
    /// @{
    /**
     * @brief Empty ctor:
     **/
    RingSetConf_v1();
    /**
     * @brief Copy constructor
     **/
    RingSetConf_v1(const RingSetConf_v1& ringset);
    /**
     * @brief Set each individual variable:
     **/
    RingSetConf_v1(const unsigned nRings, 
        const std::vector<CaloSampling::CaloSample> &rsLayers,
        const float etaWidth, const float phiWidth,
        const float cellMaxDEtaDist, const float cellMaxDPhiDist,
        const Ringer::CalJointLayer calJointLayer,
        const Ringer::CalJointSection calJointSection,
        const bool doEtaAxesDivision, const bool doPhiAxesDivision,
        const unsigned layerStartIdx, const unsigned layerEndIdx,
        const unsigned sectionStartIdx, const unsigned sectionEndIdx);
    /**
     * @brief Set each AuxVariable with @name confStruct value:
     **/
    RingSetConf_v1(const RawConf &confStruct);
    /// @}
 
    
    /// @name RingSetConf_v1 dtors
    /// @{
    /// @}

    /// @name RingSetConf_v1 nRings interation
    /// @{
    /**
     * @brief Get nRings
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    unsigned nRings() const;
    /**
     * @brief Set nRings
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    void setNRings(unsigned nRings);
    /// @}

    /// @name RingSetConf_v1 layer interation
    /// @{
    /**
     * @brief RingSet layers size:
     **/
    unsigned nLayers() const;
    /**
     * @brief get layers
     *
     * Defined via AUXSTORE_OBJECT_SETTER_AND_GETTER macro
     **/
    const std::vector< CaloSampling::CaloSample >& layers() const; 
    /**
     * @brief set layers
     *
     * Defined via AUXSTORE_OBJECT_SETTER_AND_GETTER macro
     **/
    void setLayers(const std::vector< CaloSampling::CaloSample > &layers); 
    /**
     * @brief Get ith layer
     **/
    CaloSampling::CaloSample layerAt(const unsigned idx) const; 
    /**
     * @brief Add layer
     **/
    void addLayer(const CaloSampling::CaloSample c);
    /**
     * @brief Clear layers
     **/
    void clearLayers();
    /// @}
 
    /// @name RingSetConf_v1 etaWidth,phiWidth get and set:
    /// @{
    /** 
     * @short Returns the etaWidth configuration step used for this RingSet.
     *        This parameter is used to determine which Ring index the cell
     *        is within, for eta coordinate.
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    float etaWidth() const; 
    /**
     * @short Returns the phiWidth configuration step used for this RingSet.
     *        This parameter is used to determine which Ring index the cell
     *        is within, for phi coordinate.
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    float phiWidth() const;
    /** 
     * @brief Set etaWidth. For more details, see phiWidth()
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    void setEtaWidth(float etaWidth); 
    /** 
     * @brief Set phiWidth. For more details, see phiWidth()
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    void setPhiWidth(float phiWidth);
    /** 
     * @brief Return the maximun cell distance in eta w.r.t seed
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    float cellMaxDEtaDist() const; 
    /** 
     * @brief Return the maximun cell distance in phi w.r.t seed
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    float cellMaxDPhiDist() const; 
    /** 
     * @brief Set maxCellDEtaDist. For more details, see maxCellDEtaDist()
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    void setCellMaxDEtaDist(float cellMaxDEtaDist);
    /** 
     * @brief Set maxCellDPhiDist. For more details, see maxCellDPhiDist()
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    void setCellMaxDPhiDist(float cellMaxDPhiDist);
    /** 
     * @brief Return the etaAxesDivision configuration 
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    bool doEtaAxesDivision() const;
    /** 
     * @brief Set doEtaAxesDivision. For more details, see doEtaAxesDivision()
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
     void setdoEtaAxesDivision(bool doEtaAxesDivision);
    /** 
     * @brief Return the phiAxesDivision configuration 
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    bool doPhiAxesDivision() const;
    /** 
     * @brief Set doPhiAxesDivision. For more details, see doPhiAxesDivision()
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    void setdoPhiAxesDivision(bool doPhiAxesDivision);
    /// @}
 
    /// @name RingSetConf_v1 calJointLayer, calJoint
    /// @{
    /**
     *  @brief Retrieve this RingSet conf CalJointLayer
     **/
    Ringer::CalJointLayer calJointLayer() const;
    /**
     *  @brief Retrieve this RingSet conf CalJointSection
     **/
    Ringer::CalJointSection calJointSection() const;
    /**
     *  @brief Set this RingSet CalJointLayer
     **/
    void setCalJointLayer(Ringer::CalJointLayer calJointLayer);
    /**
     *  @brief Set this RingSet CalJointSection
     **/
    void setCalJointSection(Ringer::CalJointSection calJointSection);
    /// @}
  
    /// @name RingSetConf_v1 layerStartIdx,layerEndIdx,sectionEndIdx,sectionEndIdx
    /// interaction
    /// @{
    /** 
     * @short Returns start of layer index for this RingSet Configuration 
     *        when this configuration is concatened into a
     *        non segmented representation.
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    unsigned layerStartIdx() const;

    /** 
     * @short Returns start of section index for this RingSet Configuration 
     *        when this configuration is concatened into a
     *        non segmented representation.
     **/
    unsigned sectionStartIdx() const;

    /** 
     * @short Returns end of section index for this RingSet Configuration 
     *        when this configuration is concatened into a
     *        non segmented representation.
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    unsigned layerEndIdx() const;

    /** 
     * @short Returns end of section index for this RingSet Configuration 
     *        when this configuration is concatened into a
     *        non segmented representation.
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    unsigned sectionEndIdx() const;

    /** 
     * @brief Set start layer configuration index
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    void setLayerStartIdx(unsigned layerStartIdx);
    /** 
     * @brief Set start section configuration index
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    void setSectionStartIdx(unsigned sectionEndIdx);
    /** 
     * @brief Set end layer configuration index
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    void setLayerEndIdx(unsigned layerEndIdx);
    /**
     * @brief Set end section configuration index
     *
     * Defined via AUXSTORE_PRIMITIVE_SETTER_AND_GETTER macro
     **/
    void setSectionEndIdx(unsigned sectionEndIdx);
    /// @}
 
    /// @{ Print @name RingSetConf_v1 methods:
    /**
     * @brief Print @name RingSetConf_v1 using ostream:
     **/
    void print( std::ostream &stream ) const;
    /// @}
 
    /// @name CaloRingsConf_v1 utilities
    /// @{
    /**
     * @brief Retrieve raw  RingSet Configuration struct.
     **/
    const RingSetConf_v1::RawConf getRawConf() const;
    /// @}

    /// @name RingSetConf_v1 operators overload:
    /// @{
    /** 
     * @brief Assignment Operator. Using the assignment of SG::AuxElement.
     **/
    RingSetConf_v1& operator=(const RingSetConf_v1& clrings);
    /// @}

};

/// Ctors
/// @{
//==============================================================================
inline
RingSetConf_v1::RingSetConf_v1(){;}

//==============================================================================
inline
RingSetConf_v1::RingSetConf_v1(const RingSetConf_v1& ringsetconf) : 
  SG::AuxElement(ringsetconf)
{
  this->makePrivateStore(ringsetconf);
}

//==============================================================================
inline
RingSetConf_v1::RawConf::RawConf()
  : nRings(0),
    etaWidth(0),   
    phiWidth(0), 
    cellMaxDEtaDist(0),
    cellMaxDPhiDist(0),
    calJointLayer(Ringer::CalJointLayer::UnknownJointLayer),
    calJointSection(Ringer::CalJointSection::UnknownJointSection),
    doEtaAxesDivision(false),
    doPhiAxesDivision(false),
    layerStartIdx(0),
    layerEndIdx(0),
    sectionStartIdx(0),
    sectionEndIdx(0){;}
/// @}

} // End of namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE ( xAOD::RingSetConf_v1, SG::AuxElement );

#endif // XAODCALORINGS_VERSIONS_RINGSETCONF_V1_H

