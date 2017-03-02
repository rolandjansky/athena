// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTagging_v1.h
#ifndef XAODBTAGGING_VERSIONS_BTAGVERTEX_V1_H
#define XAODBTAGGING_VERSIONS_BTAGVERTEX_V1_H

// Core include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// xAOD include(s):
//#include "xAODBTagging/BTaggingEnums.h"
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
// STL include(s):
#include <map>
#include <vector>
#include <string>

namespace xAOD {

  /// @class xAOD::BTagVertex
  /// @brief Class for xAOD secondary vertex finding information (main client: JetFitter) that is too much overhead for the xAOD::Vertex
  //enum BTagInfo;

  class BTagVertex_v1 : public SG::AuxElement{

  public :
    /// A helper typedef 
    typedef std::vector< ElementLink< xAOD::TrackParticleContainer > >
    TPELinks_t; 
    //typedef std::vector< ElementLink< xAOD::IParticleContainer > >
    //IPELinks_t; 
      /// @name xAOD::BTagging constructors
      /// @{
    
      /// Default constructor
            
    BTagVertex_v1();  

   
    /// @name Per-vertex accessor functions                        

    /// get fit chi2
    float chi2() const;
    /// get fit NDF
    float NDF() const;
    /// compatibility with PV
    float pv_compatibility() const;
    /// compatibility with others SV
    const std::vector<float>& compToOtherSV() const;
    /// access the corresponding xAOD::BTagVertex
    //const std::vector< ElementLink<xAOD::BTagVertexContainer> >& reco_vertexLink() const;
    
    /// set fit chi2
    void setChi2(float value);
    /// set fit NDF
    void setNDF(float value);
    /// set compatibility with PV
    void setPV_compatibility(float value);
    /// set compatibility with others SV
    void setCompToOtherSV(const std::vector<float>& value);
    /// set the link to the corresponding xAOD::Vertex
    //void setReco_vertexLink(const std::vector< ElementLink<xAOD::BTagVertexContainer> >& link);
    

    /// @name Per-track accessor functions (all tracks simultaneously)
    /// @{
    
    /// get track chi2 values
    const std::vector<float>& track_chi2() const;
    /// get track NDF values
    const std::vector<float>& track_NDF() const;
    /// get track momentum x components
    const std::vector<float>& track_refPx() const;
    /// get track momentum y components
    const std::vector<float>& track_refPy() const;
    /// get track momentum z components
    const std::vector<float>& track_refPz() const;
    /// get track ElementLinks
    //const IPELinks_t& track_links() const;
    const TPELinks_t& track_links() const;
    
    /// set track chi2 values
    void setTrack_chi2(const std::vector<float>& values);
    /// set track NDF values
    void setTrack_NDF(const std::vector<float>& values);
    /// set track momentum x components
    void setTrack_refPx(const std::vector<float>& values);
    /// set track momentum y components
    void setTrack_refPy(const std::vector<float>& values);
    /// set track momentum z components
    void setTrack_refPz(const std::vector<float>& values);
    /// set track ElementLinks
    //void setTrack_links(const IPELinks_t& links) ;
    void setTrack_links(const TPELinks_t& links) ;
    
    /// @}

 
    void toPersistent();

  }; // class BTagVertex
} // namespace xAOD

#endif
