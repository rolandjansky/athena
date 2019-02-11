// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_TRACKPARTICLE_V1_H
#define XAODTRACKING_VERSIONS_TRACKPARTICLE_V1_H

// System include(s):
#include <bitset>
extern "C" {
#   include <stdint.h>
}

// Core include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/DataVector.h"
#include "CxxUtils/CachedValue.h"

// xAOD include(s):
#include "xAODBase/IParticle.h"
#include "xAODTracking/TrackingPrimitives.h" 

#ifndef XAOD_STANDALONE
#ifndef XAOD_MANACORE
// Athena includes
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackCollection.h"
#endif // not XAOD_MANACORE
#endif // not XAOD_STANDALONE

// ROOT include(s):
#include "Math/Vector4D.h"

namespace xAOD {

  /// Class describing a TrackParticle.
  ///
  /// @author Edward Moyse <Edward.Moyse@cern.ch>
  /// @author Markus Elsing <Markus.Elsing@cern.ch>
  /// @nosubgrouping
  class TrackParticle_v1 : public IParticle {

  public:

      /// Default constructor
    TrackParticle_v1();
    /// Destructor
    ~TrackParticle_v1();
    /// Copy ctor. This involves copying the entire Auxilary store, and is a slow operation which should be used sparingly.
    TrackParticle_v1(const TrackParticle_v1& o );
    /// Assignment operator. This can involve creating and copying an Auxilary store, and so should be used sparingly.
    TrackParticle_v1& operator=(const TrackParticle_v1& tp );
    
      /// @name IParticle functions
      /// @{
        /// The transverse momentum (\f$p_T\f$) of the particle.
        virtual double           pt() const;
        /// The pseudorapidity (\f$\eta\f$) of the particle.
        virtual double           eta() const;
        /// The azimuthal angle (\f$\phi\f$) of the particle (has range \f$-\pi\f$ to \f$+\pi\f$.)
        virtual double           phi() const;
        /// The invariant mass of the particle..
        virtual double           m() const;
        /// The total energy of the particle.
        virtual double           e() const;
        /// The true rapidity (y) of the particle.
        virtual double           rapidity() const;

        /// Definition of the 4-momentum type.
        typedef IParticle::FourMom_t FourMom_t;

        /// The full 4-momentum of the particle.
        virtual FourMom_t p4() const;

        /// Base 4 Momentum type for TrackParticle
        typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzM4D<double> > GenVecFourMom_t;

        /// The full 4-momentum of the particle : GenVector form
        GenVecFourMom_t genvecP4() const;

        /// The type of the object as a simple enumeration
        virtual Type::ObjectType type() const;
      /// @}

      /// @name Defining parameters functions
      /// The 'defining parameters' are key to the concept of a TrackParticle, and give the values for the IParticle interface
      /// ( pt(), phi(), eta() etc.).
      /// They use the Trk::Perigee coordinate system, and are defined as:
      ///  \f$( d_0, z_0, \phi, \theta, q/p )\f$.
      /// The parameters are expressed with respect to an origin (returned by vx(), vy() and vy() ), currently intended to be the 'beamspot'.
      /// This origin is expected to be the same for all track particles in a collection (and this may be be enforced).
      /// The \f$\phi\f$ parameter is returned by either the phi() or the phi0() methods, the difference just being whether it is returned as a float or a double (it is stored as a float)
      /// @{
        /// Returns the charge.
        float charge() const;
        /// Returns the \f$d_0\f$ parameter
        float d0() const;
        /// Returns the \f$z_0\f$ parameter
        float z0() const;
        /// Returns the \f$\phi\f$ parameter, which has range \f$-\pi\f$ to \f$+\pi\f$.
        float phi0() const;
        /// Returns the \f$\theta\f$  parameter, which has range 0 to \f$\pi\f$.
        float theta() const;
        /// Returns the \f$q/p\f$  parameter
        float qOverP() const;
        /// @brief Returns a SVector of the Perigee track parameters. 
        /// i.e. a vector of
        ///  \f$\left(\begin{array}{c}d_0\\z_0\\\phi_0\\\theta\\q/p\end{array}\right)\f$
        const DefiningParameters_t definingParameters() const;
        /// Returns the 5x5 symmetric matrix containing the defining parameters covariance matrix.
        const ParametersCovMatrix_t definingParametersCovMatrix() const;  
        /// Returns the length 6 vector containing the elements of defining parameters covariance matrix.
        const std::vector<float>& definingParametersCovMatrixVec() const;   
        /// Set the defining parameters.     
        void setDefiningParameters(float d0, float z0, float phi0, float theta, float qOverP);
        /// Set the defining parameters covariance matrix.
        void setDefiningParametersCovMatrix(const ParametersCovMatrix_t& cov);
        /// Set the defining parameters covariance matrix using a length 15 vector.
        void setDefiningParametersCovMatrixVec(const std::vector<float>& cov);
        /// The x origin for the parameters.
        float vx() const;
        /// The y origin for the parameters.
        float vy() const;
        /// The z origin for the parameters.
        float vz() const;
        /// Set the origin for the parameters.
        void setParametersOrigin(float x, float y, float z);

#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
        /// @brief Returns the Trk::MeasuredPerigee track parameters.
        ///
        /// These are defined as:
        ///  \f$\left(\begin{array}{c}d_0\\z_0\\\phi_0\\\theta\\q/p\\\end{array}\right)\f$
        /// @note This is only available in Athena. 
        const Trk::Perigee& perigeeParameters() const;
#endif // not XAOD_STANDALONE and not XAOD_MANACORE
      /// @}

      /// @name Curvilinear functions
      /// The set of functions which return other track parameters.
      /// The remaining track parameters (i.e. not the 'defining parameters') use the 'curvilinear' coordinate system, 
      /// and are represented by the parameters @f$(x,y,z,p_x,p_y,p_z)@f$.
      /// The parameters can have an associated local 5x5 error/covariance matrix. They are expressed at various points through the
      /// detector, which can be determined by the parameterPosition() method.      
      /// @code
      /// // Example code to use parameters
      /// unsigned int index=0;
      /// if (myTP.indexOfParameterAtPosition(index, xAOD::FirstMeasurement)){
      ///   CurvilinearParameters_t parameters = myTP.trackParameters(index);
      /// }
      /// @endcode
      /// @{
        /// Returns the number of additional parameters stored in the TrackParticle. 
        size_t numberOfParameters() const; 
        /// Returns the track parameter vector at 'index'.
        const CurvilinearParameters_t trackParameters(unsigned int index) const;
        /// Returns the parameter x position, for 'index'.
        float parameterX(unsigned int index) const;
        /// Returns the parameter y position, for 'index'.
        float parameterY(unsigned int index) const;
        /// Returns the parameter z position, for 'index'.
        float parameterZ(unsigned int index) const;
        /// Returns the parameter x momentum component, for 'index'.
        float parameterPX(unsigned int index) const;
        /// Returns the parameter y momentum component, for 'index'.
        float parameterPY(unsigned int index) const;
        /// Returns the parameter z momentum component, for 'index'.
        float parameterPZ(unsigned int index) const; 
        /// Set the parameters via the passed vector of vectors. 
        /// The vector<float> should be of size 6: x,y,z,px,py,pz (charge is stored elsewhere)
        void setTrackParameters(std::vector<std::vector<float> >& parameters);
        /// @brief Returns the TrackParticleCovMatrix_t (covariance matrix) at 'index', 
        /// which corresponds to the parameters at the same index.
        ParametersCovMatrix_t trackParameterCovarianceMatrix(unsigned int index) const;
        /// Set the cov matrix of the parameter at 'index', using a vector of floats.
        /// The vector @f$\mathrm{v}(a1,a2,a3 ... a_{15})@f$ represents the lower diagonal, i.e. it gives a matrix of 
        /// \f$\left(\begin{array}{ccccc} a_1  & a_2  & a_4  & a_7  & a_{11} \\ a_2  & a_3  & a_5  & a_8  & a_{12} \\ a_4  & a_5  & a_6  & a_9  & a_{13} \\ a_7  & a_8  & a_9  & a_{10}  & a_{14} \\ a_{11} & a_{12} & a_{13} & a_{14} & a_{15} \end{array}\right)\f$
        void setTrackParameterCovarianceMatrix(unsigned int index, std::vector<float>& cov);  
        /// @brief Return the ParameterPosition of the parameters at 'index'.
        xAOD::ParameterPosition parameterPosition(unsigned int index) const;
        /// @brief Function to determine if this TrackParticle contains track parameters at a certain position, and if so, what the 'index' is.
        /// @param[in] index Filled with the index of the track parameters at 'position' - untouched otherwise.
        /// @param[out] position The location in the detector of the required track parameters.
        /// @return Returns 'true' if the TrackParticle parameters at 'position', returns False otherwise.
        bool indexOfParameterAtPosition(unsigned int& index, ParameterPosition position) const;
        /// Set the 'position' (i.e. where it is in ATLAS) of the parameter at 'index', using the ParameterPosition enum. 
        void setParameterPosition(unsigned int index, ParameterPosition pos);
#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
        /// @brief Returns a curvilinear representation of the parameters at 'index'.
        /// @note This is only available in Athena. 
        const Trk::CurvilinearParameters curvilinearParameters(unsigned int index) const;          
#endif // not XAOD_STANDALONE and not XAOD_MANACORE

    /// Returns the radius of the first hit.
    float radiusOfFirstHit() const;
    /// Set the radius of the first hit.
    void setRadiusOfFirstHit(float radius);
    
        /// Returns the offline identifier of the first hit.
    uint64_t identifierOfFirstHit() const;
    /// Set the offline identifier of the first hit.
    void setIdentifierOfFirstHit( uint64_t id);
    
    float beamlineTiltX() const;
    void  setBeamlineTiltX(float tiltX);
    
    float beamlineTiltY() const;
    void  setBeamlineTiltY(float tiltY);
    
    uint32_t hitPattern() const;
    void setHitPattern(uint32_t hitpattern);

    uint8_t numberOfUsedHitsdEdx() const;
    void setNumberOfUsedHitsdEdx(uint8_t numhits);

    uint8_t numberOfIBLOverflowsdEdx() const;
    void setNumberOfIBLOverflowsdEdx(uint8_t numoverflows);

      /// @}

      /// @name Fit quality functions
      /// Returns some information about quality of the track fit.
      /// @{
        /// Returns the @f$ \chi^2 @f$ of the overall track fit.
        float chiSquared() const;
        /// Returns the number of degrees of freedom of the overall track or vertex fit as float.
        float  numberDoF() const;   
        /// Set the 'Fit Quality' information.
        void setFitQuality(float chiSquared, float numberDoF);   
      /// @}

      /// @name TrackInfo functions
      /// Contains information about the 'fitter' of this Trk::Track / TrackParticle.
      /// Additionally there is some information about how the e.g. fit was configured. 
      /// Also the information on the properties of the  track fit is stored.
      /// @{
        /// Methods setting the TrackProperties.
        void setTrackProperties (const TrackProperties properties) ;
        /// Method setting the pattern recognition algorithm, using a bitset. 
        /// The bitset should be created using the TrackPatternRecoInfo enum as follows:
        /// @code
        /// const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco;
        /// patternReco.set(xAOD::Fatras);
        /// @endcode
        void setPatternRecognitionInfo(const std::bitset<xAOD::NumberOfTrackRecoInfo>& patternReco) ;
        /// Method setting the pattern recognition algorithm, using a 64-bit int (which is faster than using a bitset). 
        /// The bit set should be created using the TrackPatternRecoInfo enum as follows:
        /// @code
        /// uint64_t patternReco;
        /// patternReco	|= (1<<static_cast<uint64_t>(xAOD::Fatras))
        /// @endcode
        void setPatternRecognitionInfo(uint64_t patternReco) ;
        /// Method for setting the fitter, using the TrackFitter enum.
        void setTrackFitter(const TrackFitter fitter)  ;
        /// Method for setting the particle type, using the ParticleHypothesis enum.
        void setParticleHypothesis(const ParticleHypothesis hypo);
        ///Access methods for track properties, which returns 'true' 
        /// if a logical AND of the parameter 'proprty' and the stored properties returns true.
        /// i.e. you do: 
        /// @code 
        /// TrackProperties testProperty;
        /// testProperty.set(SOMEPROPERTY);
        /// if (trackParticle.trackProperties(testProperty)) doSomething();
        /// @endcode
        /// @todo - fix the above (or make something nicer)
        TrackProperties trackProperties() const;
        ///Access method for pattern recognition algorithm.
        std::bitset<NumberOfTrackRecoInfo>  patternRecoInfo() const;
      /// Returns the particle hypothesis used for Track fitting.
        ParticleHypothesis particleHypothesis() const;
        /// Returns the fitter.
        TrackFitter trackFitter() const;
      /// @}


        /// Accessor for TrackSummary values.
        /// If 'information' is stored in this TrackParticle and is of the correct templated type T, 
        /// then the function fills 'value' and returns 'true', otherwise it returns 'false', and does not touch 'value'. 
        /// See below for an example of how this is intended to be used.
        /// @code
        /// int numberOfBLayerHits=0;
        /// if( myParticle.summaryValue(numberOfBLayerHits,xAOD::numberOfBLayerHits) ){
        ///   ATH_MSG_INFO("Successfully retrieved the integer value, numberOfBLayerHits"); 
        /// }
        /// float numberOfCscPhiHits=0.0; //Wrong! This is actually an int too.
        /// if( !myParticle.summaryValue(numberOfCscPhiHits,xAOD::numberOfCscPhiHits) ){
        ///   ATH_MSG_INFO("Types must match!"); 
        /// }
        /// @endcode
        /// @param[in] information The information being requested. This is not guaranteed to be stored in all TrackParticles.
        /// @param[out] value  Only filled if this TrackParticle contains 'information', and the types match.
        /// @return Returns 'true' if the TrackParticle contains 'information', and its concrete type matches 'value' (templated type T).
        bool summaryValue(uint8_t& value, const SummaryType &information) const;
		///  @copydoc TrackParticle_v1::summaryValue(uint8_t& value, const SummaryType &information) const
        bool summaryValue(float& value, const SummaryType &information) const;
        /// Set method for TrackSummary values.
        void setSummaryValue(uint8_t& value, const SummaryType &information);
		///  @copydoc TrackParticle_v1::setSummaryValue(uint8_t& value, const SummaryType &information)
        void setSummaryValue(float& value, const SummaryType &information);
      /// @}
      
      /// @name Links
      /// @{
#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
        /// @brief Returns a link (which can be invalid) to the Trk::Track which was used to make this TrackParticle.
        /// @note This is only available in Athena. 
        const ElementLink< TrackCollection >& trackLink() const;
        /// @brief Set the link to the original track
        /// @note This is only available in Athena.
         void setTrackLink(const ElementLink< TrackCollection >& track);
        /// @brief Returns a pointer (which can be NULL) to the Trk::Track which was used to make this TrackParticle.
        /// @note This is only available in Athena. 
		 const Trk::Track* track() const; 
#endif // not XAOD_STANDALONE and not XAOD_MANACORE
        
      /// @}

      /// Reset the internal cache of the object
      void resetCache();
 
private:

#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) ) && ( ! defined(__GCCXML__) )  && !defined(__CLING__)
      /// @brief Cached MeasuredPerigee, built from this object.
      /// @note This is only available in Athena.
     CxxUtils::CachedValue<Trk::Perigee> m_perigeeParameters;
#endif // not XAOD_STANDALONE and not XAOD_MANACORE and not __GCCXML__

    }; // class Track Particle

  } // namespace xAOD


#endif // XAODTRACKING_VERSIONS_TrackParticle_v1_H
