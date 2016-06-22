// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_VERSIONS_MISSINGETCOMPOSITIONBASE_H
#define XAODMISSINGET_VERSIONS_MISSINGETCOMPOSITIONBASE_H

#include "xAODMissingET/versions/MissingETBase.h"

#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"

#include "AthContainers/ConstDataVector.h"
#include "AthLinks/ElementLink.h"

#include "xAODMissingET/MissingETContainer.h"

#include "xAODJet/JetContainer.h"

#include <vector>
#include <iterator>

/*! @brief General namespace for MET EDM software */
namespace MissingETBase
{
  namespace Types
  {
    /*! @name Link data types */
    /*!@{*/
    /*! @brief Link to @link MissingET MissingET @endlink object 
     *
     *  This is the link to the MET object the signal or physics objects in a @link MissingETContribution MissingETContribution @endlink contributes to.
     */
    typedef ElementLink<xAOD::MissingETContainer> metlink_t;
    /*! @brief Link to @link Jet Jet @endlink object 
     *
     *  This is the link to the jet used to build overlap removal decisions in a @link MissingETAssociation MissingETAssociation @endlink object.
     */
    typedef ElementLink<xAOD::JetContainer> jetlink_t;
    /*! @brief Link to contributing object
     *
     *  This is the link to an object contributing to a specific @link MissingET  MissingET @endlink object in the @link MissingETContribution MissingETContribution @endlink.
     */
    typedef ElementLink<xAOD::IParticleContainer> objlink_t;
    /*@}*/

    typedef ConstDataVector<xAOD::IParticleContainer> const_signal_vector_t;
    
    /*! @names Vector of links  and object types */
    /*!@{*/
    /*! @brief Vector of object links type
     *
     *  @todo Check behaviour of ElementLinkVector<xAOD::IParticleContainer> if incomplete ElementLink<xAOD::IParticleContainer> may exist!
     */
    typedef std::vector<objlink_t>        objlink_vector_t;
    typedef std::vector<const xAOD::IParticle*> object_vector_t;     /*!< @brief Vector of object type */
    /*!@}*/
  } // namespace MissingETBase::Types

  /*! @brief Namespace for status word tags
   *
   *  This namespace collects bit patterns for three different categories characterizing the status of a contribution to a given @link MissingET  MissingET @endlink object.
   *  MissingETBase::Status::Reco has bit patterns tagging the reconstruction status:
   *  <ul>
   *  MissingETBase::Status::Total indcates 
   */
  struct Status
  {
    /*! @brief General indicator of MET reconstruction status */
    enum Reco { 
      Total           = 0x01000000, /*!< @brief Total MET for given composition */
      ContributedTerm = 0x02000000, /*!< @brief Contribution to total MET */
      CorrectedTerm   = 0x04000000, /*!< @brief Corrected term (replaces default contributed term */
      AlternativeTerm = 0x08000000, /*!< @brief Alternative term */
      StandAlone      = 0x00000000  /*!< @brief Unknown/standalone reconstruction */ };
    /*! @brief Indicator for vertex choice in MET reconstruction */
    enum Vertex {
      Primary         = 0x00100000, /*!< @brief MET (term) is calculated with respect to primary vertex */
      Secondary       = 0x00200000, /*!< @brief MET (term) is calculated with respect to secondary vertex */
      Event           = 0x00010000, /*!< @brief Vertex recconstructed in event */
      Nominal         = 0x00020000, /*!< @brief Primary vertex is nominal vertex */
      Specific        = 0x00040000, /*!< @brief MET term is calculated with respect to object vertices */
      NoVertex        = 0x00000000  /*!< @brief Unknown vertex */ };
    /*! @brief Indicator for correction
     *
     *  This is mainly applied for the MET SoftTerm and MET from hard jets, which may be subjected to a pile-up filter. These indicators are not meant to tag
     *  the fact that a contributing object has corrected kinematics.       
     */
    enum Correction {
      Corrected       = 0x00001000, /*!< @brief Indicator for corrected MET */
      Pileup          = 0x00001100, /*!< @brief MET term is pile-up corrected*/
      PileupCalo      = 0x00001110, /*!< @brief Calorimeter signal based pile-up correction */ 
      PileupTrack     = 0x00001120, /*!< @brief Reconstructed track based pile-up correction */
      PileupSTVF      = 0x00001121, /*!< @brief STVF correction is applied */
      PileupJetVertex = 0x00001122, /*!< @brief JVF based filter is applied */
      PileupJetArea   = 0x00001140, /*!< @brief Jet area based pileup correction */
      NotCorrected    = 0x00000000  /*!< @brief No MET specific correction applied to term */ };

    /*! @name Bit pattern for overall contribution indicator */
    /*!@{*/
    static const Types::bitmask_t clearedStatusTag   = StandAlone      | NoVertex | NotCorrected;     /*!< @brief General cleared term tag */
    static const Types::bitmask_t generalSummedTag   = Total           | NoVertex | NotCorrected;     /*!< @brief General summed MET tag */
    static const Types::bitmask_t generalContribTag  = StandAlone      | NoVertex | NotCorrected;     /*!< @brief General contribution tag */
    static const Types::bitmask_t generalHardTermTag = ContributedTerm | Specific | Pileup;           /*!< @brief General hard term contribution tag */
    static const Types::bitmask_t generalSoftTermTag = ContributedTerm | NoVertex | NotCorrected;     /*!< @brief General soft term contribution tag */
    static Types::bitmask_t clearedStatus()       { return clearedStatusTag; }                        /*!< @brief Cleared term tag accessor */
    static Types::bitmask_t summedTerm()          { return generalSummedTag; }                        /*!< @brief General summed term tag accessor */
    static Types::bitmask_t contributedTerm()     { return generalContribTag; }                       /*!< @brief General contribution tag accesssor */
    static Types::bitmask_t contributedHardTerm() { return generalHardTermTag; }                      /*!< @brief General hard term contribution tag accessor */
    static Types::bitmask_t contributedSoftTerm() { return generalSoftTermTag; }                      /*!< @brief General soft term contribution tag accessor */
    /*!@}*/

    struct Tags 
    {
      static Types::bitmask_t setPattern(Types::bitmask_t rec,Types::bitmask_t vtx,Types::bitmask_t cor) { return ( rec | vtx ) | cor; }

      static Types::bitmask_t total(Vertex vtx=Nominal,Correction cor=NotCorrected)              { return setPattern(Total,vtx,cor); }
      static Types::bitmask_t contributedTerm(Vertex vtx=Nominal,Correction cor=NotCorrected)    { return setPattern(ContributedTerm,vtx,cor); }
      static Types::bitmask_t correctedTerm(Vertex vtx=Nominal,Correction cor=Corrected) { return setPattern(CorrectedTerm,vtx,cor); }
      static Types::bitmask_t alternativeTerm(Vertex vtx=Nominal,Correction cor=NotCorrected)     { return setPattern(AlternativeTerm,vtx,cor); }

      static bool hasPattern(Types::bitmask_t sw,Types::bitmask_t pat) { return ( sw & pat ) == pat; }

      static bool isTotal(Types::bitmask_t sw)           { return hasPattern(sw,Total); }
      static bool isContributedTerm(Types::bitmask_t sw) { return hasPattern(sw,ContributedTerm); }
      static bool isAlternativeTerm(Types::bitmask_t sw) { return hasPattern(sw,AlternativeTerm); }
      static bool isStandAlone(Types::bitmask_t sw)      { return hasPattern(sw,StandAlone); }
    }; // struct MissingETBase::Status::Tags
  }; // struct MissingETBase::Status

  namespace Container
  { 
    /*! @name Iterator algebra */
    /*!@{*/
    /*! @brief Difference (number of steps) between two iterators
     *
     *  @return Number of steps from first to last iterator.
     * 
     *  @tparam ITER iterator type.
     *
     *  @note ITER::operator-(const ITER&) must be defined with an integer return value >= 0. 
     *
     *  @param[in] first first iterator
     *  @param[in] last last iterator
     *
     *  @note @c first needs to be identical to last, or precede it. Otherwise the return value may be undefined.
     */
    template<class ITER>
    size_t iteratorDiff(ITER first,ITER last) { return (size_t)(std::distance(first,last)); }
    /*! @brief Advance an iterator by a number of steps
     *
     *  @return Advanced iterator. 
     * 
     *  @tparam ITER iterator type.
     *
     *  @note ITER::operator+(size_t) must be defined.
     *
     *  @param[in] iter starting iterator
     *  @param[in] step number of steps to advance
     *
     *  @warning There is no control on the number of steps. The resulting iterator maybe invalid, except if ITER has a protection build in.
     */
    template<class ITER>
    ITER iteratorAdvance(ITER iter,size_t step) { ITER fIter(iter); std::advance(iter,step); return fIter; }
    /*!@}*/
  } // namespace MissingETBase::Container

  /*! @brief Signal usage for ambiguity resolution control */
  namespace UsageHandler
  {
    /*! @brief Policies on usage checks */
    enum Policy {
      AllCalo       = 0x0010, /*!< @brief Inclusive except tracks */
      OnlyCluster   = 0x0011, /*!< @brief CaloCluster based only */
      OnlyTrack     = 0x0012, /*!< @brief Track based only */
      TrackCluster  = 0x0014, /*!< @brief Both cluster and track based */
      PhysicsObject = 0x0020, /*!< @brief Physics object based*/
      ParticleFlow  = 0x0040, /*!< @brief Particle Flow Object based */
      TruthParticle = 0x0100, /*!< @brief Truth particle based */
      UnknownPolicy = 0x0000  /*!< @brief Unknown policy */};
  } // namespace MissingETBase::OverlapHandler
} // namespace MissingETBase
#endif

