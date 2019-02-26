// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTagging_v1.h 797330 2017-02-15 14:25:13Z guirriec $
#ifndef XAODBTAGGING_VERSIONS_BTAGGING_V1_H
#define XAODBTAGGING_VERSIONS_BTAGGING_V1_H

// Core include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODBTagging/BTaggingEnums.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODBTagging/BTagVertexContainer.h"

// STL include(s):
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include "CxxUtils/checker_macros.h"

namespace xAOD {

  /// @class xAOD::BTagging
  /// @brief Class for xAOD b-tagging information
  ///
  /// @author Aaron Armbruster
  /// @author Johannes Erdmann
  /// @author Magdalena Slawinska
  /// @author Paul Thompson
  ///
  class BTagging_v1 : public SG::AuxElement {

  public:

    /// A helper typedef
    typedef std::vector< ElementLink< xAOD::TrackParticleContainer > >
    TPELVec_t;
    typedef std::vector<ElementLink<xAOD::VertexContainer> > VxELVec_t;
    typedef std::vector<ElementLink<xAOD::BTagVertexContainer> > BTagVxELVec_t;

    /// @name xAOD::BTagging constructors
    /// @{

    /// Default constructor
    BTagging_v1();

    /// @}

    /// @name Dedicated SV0 accessor functions
    /// @{

    /// @brief SV0 3D significance
    double SV0_significance3D() const;
    /// @brief set SV0 3D significance
    void setSV0_significance3D(double value);
    /// @brief get vector of SV0 TrackParticle ElementLinks
    const TPELVec_t& SV0_TrackParticleLinks() const;
    /// @brief set vector of SV0 TrackParticle ElementLinks
    void setSV0_TrackParticleLinks( const TPELVec_t& value );
    /// @brief Get one particular SV0 TrackParticle as a bare pointer
    const xAOD::TrackParticle* SV0_TrackParticle( size_t i ) const;
    /// @brief Get the number of SV0 TrackParticles associated with the object
    size_t nSV0_TrackParticles() const;
    /// @brief add a TrackParticle ElementLink for SV0
    void addSV0_TrackParticle(const ElementLink< xAOD::TrackParticleContainer >& link);
    /// @brief clear vector of SV0 TrackParticle ElementLinks
    void clearSV0_TrackParticleLinks();

    /// @}

    /// @name Dedicated SV1 accessor functions
    /// @{

    /// @brief SV1 b probability
    double SV1_pb() const;
    /// @brief set SV1 b probability
    void setSV1_pb(double value);
    /// @brief SV1 c probability
    double SV1_pc() const;
    /// @brief set SV1 c probability
    void setSV1_pc(double value);
    /// @brief SV1 light probability
    double SV1_pu() const;
    /// @brief set SV1 light probability
    void setSV1_pu(double value);
    /// @brief get SV1 log likelihood ratio
    double SV1_loglikelihoodratio() const { return this->calcLLR(SV1_pb(),SV1_pu()); }
    /// @brief get vector of SV1 TrackParticle ElementLinks
    const TPELVec_t& SV1_TrackParticleLinks() const;
    /// @brief set vector of SV1 TrackParticle ElementLinks
    void setSV1_TrackParticleLinks( const TPELVec_t& value );
    /// @brief Get one particular SV1 TrackParticle as a bare pointer
    const xAOD::TrackParticle* SV1_TrackParticle( size_t i ) const;
    /// @brief Get the number of SV1 TrackParticles associated with the object
    size_t nSV1_TrackParticles() const;
    /// @brief add a TrackParticle ElementLink for SV1
    void addSV1_TrackParticle(const ElementLink< xAOD::TrackParticleContainer > & link);
    /// @brief clear vector of SV1 TrackParticle ElementLinks
    void clearSV1_TrackParticleLinks();

    /// @}

    /// @name Dedicated IP2D accessor functions
    /// @{

    /// @brief IP2D b probability
    double IP2D_pb() const;
    /// @brief set IP2D b probability
    void setIP2D_pb(double value);
    /// @brief IP2D c probability
    double IP2D_pc() const;
    /// @brief set IP2D c probability
    void setIP2D_pc(double value);
    /// @brief IP2D light probability
    double IP2D_pu() const;
    /// @brief set IP2D light probability
    void setIP2D_pu(double value);
    /// @brief get IP2D log likelihood ratio
    double IP2D_loglikelihoodratio() const { return this->calcLLR(IP2D_pb(),IP2D_pu()); }
    /// @brief get vector of IP2D TrackParticle ElementLinks
    const TPELVec_t& IP2D_TrackParticleLinks() const;
    /// @brief set vector of IP2D TrackParticle ElementLinks
    void setIP2D_TrackParticleLinks( const TPELVec_t& value );
    /// @brief Get one particular IP2D TrackParticle as a bare pointer
    const xAOD::TrackParticle* IP2D_TrackParticle( size_t i ) const;
    /// @brief Get the number of IP2D TrackParticles associated with the object
    size_t nIP2D_TrackParticles() const;
    /// @brief add a TrackParticle ElementLink for IP2D
    void addIP2D_TrackParticle(const ElementLink< xAOD::TrackParticleContainer > & link);
    /// @brief clear vector of IP2D TrackParticle ElementLinks
    void clearIP2D_TrackParticleLinks();

    /// @}
    
    /// @name Dedicated IP3D accessor functions
    /// @{

    /// @brief IP3D b probability
    double IP3D_pb() const;
    /// @brief set IP3D b probability
    void setIP3D_pb(double value);
    /// @brief IP3D c probability
    double IP3D_pc() const;
    /// @brief set IP3D c probability
    void setIP3D_pc(double value);
    /// @brief IP3D light probability
    double IP3D_pu() const;
    /// @brief set IP3D light probability
    void setIP3D_pu(double value);
    /// @brief get IP3D log likelihood ratio
    double IP3D_loglikelihoodratio() const { return ( (IP3D_pb()==1.&&IP3D_pu()==1.e9) ? 0. : 
                                                      this->calcLLR(IP3D_pb(),IP3D_pu()) ); }
    /// @brief get vector of IP3D TrackParticle ElementLinks
    const TPELVec_t& IP3D_TrackParticleLinks() const;
    /// @brief set vector of IP3D TrackParticle ElementLinks
    void setIP3D_TrackParticleLinks( const TPELVec_t& value );
    /// @brief Get one particular IP3D TrackParticle as a bare pointer
    const xAOD::TrackParticle* IP3D_TrackParticle( size_t i ) const;
    /// @brief Get the number of IP3D TrackParticles associated with the object
    size_t nIP3D_TrackParticles() const;
    /// @brief add a TrackParticle ElementLink for IP3D
    void addIP3D_TrackParticle(const ElementLink< xAOD::TrackParticleContainer > & link);
    /// @brief clear vector of IP3D TrackParticle ElementLinks
    void clearIP3D_TrackParticleLinks();

    /// @}

    /// @name Dedicated SV1+IP3D accessor functions
    /// @{

    double SV1plusIP3D_discriminant() const {
      return SV1_loglikelihoodratio() + IP3D_loglikelihoodratio();
    }

    /// @}

    /// @name Dedicated JetFitter accessor functions
    /// @{

    /// @brief JetFitter b probability
    double JetFitter_pb() const;
    /// @brief set JetFitter b probability
    void setJetFitter_pb(double value);
    /// @brief JetFitter c probability
    double JetFitter_pc() const;
    /// @brief set JetFitter c probability
    void setJetFitter_pc(double value);
    /// @brief JetFitter light probability
    double JetFitter_pu() const;
    /// @brief set JetFitter light probability
    void setJetFitter_pu(double value);
    /// @brief get JetFitter log likelihood ratio
    double JetFitter_loglikelihoodratio() const { return this->calcLLR(JetFitter_pb(),JetFitter_pu()); }


    /// @}

    /// @name Dedicated JetFittercomb accessor functions
    /// @{

    /// @brief JetFitter b probability
    //double JetFitterCombNN_pb() const;
    /// @brief set JetFitter b probability
    //void setJetFitterCombNN_pb(double value);
    /// @brief JetFitter c probability
    //double JetFitterCombNN_pc() const;
    /// @brief set JetFitter c probability
    //void setJetFitterCombNN_pc(double value);
    /// @brief JetFitter light probability
    //double JetFitterCombNN_pu() const;
    /// @brief set JetFitter light probability
    //void setJetFitterCombNN_pu(double value);
    /// @brief get JetFitter log likelihood ratio
    //double JetFitterCombNN_loglikelihoodratio() const { return this->calcLLR(JetFitterCombNN_pb(),JetFitterCombNN_pu());
    //}


    /// @}

    /// @name Dedicated MV1 accessor functions
    /// @{

    /// @brief MV1 discriminant
    double MV1_discriminant() const;
    /// @brief set MV1 discriminant
    void setMV1_discriminant(double value);

    /// @}


    /// @name generic tagger weight accessor functions
    /// @{
    bool loglikelihoodratio(const std::string& taggername, double &value, 
                            const std::string& signal="pb", const std::string& bckgd="pu") const;
    bool MVx_discriminant(const std::string& taggername, double &value) const;
    bool pu(const std::string& taggername, double &value) const;
    bool pb(const std::string& taggername, double &value) const;
    bool pc(const std::string& taggername, double &value) const;
    double calcLLR(double num, double den) const;


    /// @}

 
    /// @name Generic tagger info accessor functions
    /// @{

    /// @brief get taggers information (int)
    bool taggerInfo(int& value, BTagInfo info) const;
    /// @brief get taggers information (float)
    bool taggerInfo(float& value, BTagInfo info) const;
    /// @brief get taggers information (bool)
    bool taggerInfo(bool& value, BTagInfo info) const;
    /// @brief get taggers information (string)
    bool taggerInfo(std::string& value, BTagInfo info) const;
    /// @brief get taggers information (vector<int>)
    bool taggerInfo(std::vector<int>& value, BTagInfo info) const;
    /// @brief get taggers information (vector<float>)
    bool taggerInfo(std::vector<float>& value, BTagInfo info) const;
    /// @brief get taggers information (vector<bool>)
    bool taggerInfo(std::vector<bool>& value, BTagInfo info) const;

    /// @brief set taggers information (int)
    void setTaggerInfo(int value, BTagInfo info);
    /// @brief set taggers information (float)
    void setTaggerInfo(float value, BTagInfo info);
    /// @brief set taggers information (bool)
    void setTaggerInfo(bool value, BTagInfo info);
    /// @brief set taggers information (string)
    void setTaggerInfo(const std::string& value, BTagInfo info);
    /// @brief set taggers information (vector<int>)
    void setTaggerInfo(const std::vector<int>& value, BTagInfo info);
    /// @brief set taggers information (vector<float>)
    void setTaggerInfo(const std::vector<float>& value, BTagInfo info);
    /// @brief set taggers information (vector<bool>)
    void setTaggerInfo(const std::vector<bool>& value, BTagInfo info);

    /// @}

    /// @name Generic decoration accessor functions
    /// @{

    /// @brief get variables by string
    template< class T >
    bool variable(const std::string &taggername,
                  const std::string &variablename, T &value) const;

    /// @brief set variables by string
    template< class T >
    void setVariable(const std::string &taggername,
                     const std::string &variablename, const T &value);

    /// @}

    ///@name xAOD::BTagging helper for ElementLink Persistification
    void toPersistent  ATLAS_NOT_REENTRANT ();

    void setDynTPELName ATLAS_NOT_REENTRANT (const std::string &taggername,
			const std::string &variablename);
    void setDynVxELName ATLAS_NOT_REENTRANT (const std::string &taggername,
			const std::string &variablename);
    void setDynBTagVxELName ATLAS_NOT_REENTRANT (const std::string &taggername,
			    const std::string &variablename);


  }; // class BTagging

  template< class T >
  inline
  bool BTagging_v1::variable( const std::string &taggername,
                              const std::string &variablename,
                              T& value ) const {

    const std::string varname = taggername+ "_" + variablename;
    Accessor< T > acc( varname );
    if( ! acc.isAvailable( *this ) ) return false;
    value = acc( *this );
    return true;
  }

  template< class T >
  inline
  void BTagging_v1::setVariable( const std::string &taggername,
                                 const std::string &variablename,
                                 const T &value ) {

    const std::string varname = taggername+ "_" + variablename;
    Accessor< T > acc( varname );
    acc( *this ) = value;
    return;
  }

} // namespace xAOD

#endif // XAODBTAGGING_VERSIONS_BTAGGING_V1_H
