/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*- c++ -*-
#ifndef XAODTRIGL1CALO_VERSIONS_JGTOWER_V1_H
#define XAODTRIGL1CALO_VERSIONS_JGTOWER_V1_H

// EDM include(s):
#include "xAODBase/IParticle.h"
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

//CxxUtils for override final
#include "CxxUtils/final.h"
#include "CxxUtils/override.h"

// System include(s):
#include <stdint.h>

namespace xAOD {

  /// Description of JGTower_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 652807 $
  /// $Date: 2015-03-09 22:52:07 +0100 (Mon, 09 Mar 2015) $  
  ///  
  /// Trigger towers are the inputs to all other parts of the calorimeter trigger.
  /// They are formed by analogue summation of cells (represented in simulation
  /// by LArTTL1 and TileTTL1 objects). The L1Calo PreProcessor digitizes these,
  /// performs Bunch-Crossing IDentification, final calibration and reduction
  /// to 8 bit words.
  ///   
  /// The TriggerTower class represents the output of the PreProcessor.
  ///
  /// For each tower, the TriggerTower contains the final ET (LUT output)
  /// as well as a vector of ADC data (at 25ns intervals) from which it was
  /// formed, plus error flags and raw BCID output. As the PreProcessor
  /// readout can read variable numbers of slices of all of these data, the
  /// internal storage is in std::vectors, and methods exist to return all
  /// slices as well as just the data corresponding to the triggered crossing.
  ///
  /// The JGTower_v1 class inherits from IParticle in order to assist
  /// The Level 1.5 jet HLT algorithm
  ///
  
  class JGTower_v1 : public IParticle{
    public:
      /// Default constructor
      JGTower_v1();
      /// Copy constructor
      JGTower_v1(const JGTower_v1& other);
      /// Assignment operator
      JGTower_v1& operator=(const JGTower_v1& other);
      /// Default desturctor
      virtual ~JGTower_v1(){}
      
      /// @name xAOD::IParticle functions
      /// These are already virtual due to IParticle
      /// @{

      /// @brief The transverse momentum (\f$p_T\f$) of the particle
      virtual double pt() const  ATH_FINAL;

      /// @brief The pseudorapidity (\f$\eta\f$) of the particle
      virtual double eta() const ATH_FINAL;
      void setEta(float);

      /// @brief The azimuthal angle (\f$\phi\f$) of the particle
      virtual double phi() const ATH_FINAL;
      void setPhi(float);

      /// @brief The pseudorapidity (\f$\eta\f$) of the particle
      virtual double deta() const ATH_FINAL;
      void setdEta(float);

      /// @brief The azimuthal angle (\f$\phi\f$) of the particle
      virtual double dphi() const ATH_FINAL;
      void setdPhi(float);


      virtual double et() const ATH_FINAL;
      void setEt(float);

      /// get coolId
      virtual int Id() const ATH_FINAL;
      void setId(int);
      virtual int sampling() const ATH_FINAL;
      void setSampling(int);




      ///get SCIndex
      const std::vector<int>& SCIndex() const;
      ///set SCIndex
      void setSCIndex(const std::vector<int>&);

      ///get TileIndex
      const std::vector<int>& TileIndex() const;
      ///set TileIndex
      void setTileIndex(const std::vector<int>&);

      
      /// @brief The invariant mass of the particle
      virtual double m() const ATH_FINAL;

      /// The total energy of the particle
      virtual double e() const  ATH_FINAL;

      /// @brief The true rapidity (y) of the particle
      virtual double rapidity() const ATH_FINAL;

      /// @brief Definition of the 4-momentum type
      typedef IParticle::FourMom_t FourMom_t;

      /// @brief The full 4-momentum of the particle as a TLoretzVector
      virtual const FourMom_t& p4() const ATH_FINAL;

      /// @brief The type of the object as a simple enumeration, remains pure virtual in e/gamma.
      virtual Type::ObjectType type() const ATH_FINAL; 
      
      
      virtual void initialize(const int Id, const float Et, const float Eta,const float Phi);
      virtual void initialize(const int Id, const float Eta,const float Phi);
    private:
      
      /// Cached 4-momentum object
      mutable FourMom_t m_p4;
      /// Cache state of the internal 4-momentum (reset from the streamer)
      mutable bool m_p4Cached;      
  
  }; // class JGTower_v1
} // namespace xAOD

#endif // XAODTRIGL1CALO_VERSIONS_JGTOWER_V1_H
