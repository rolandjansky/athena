//Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef FTAGVALIDATION_PHYSICSVARIABLEPLOTS_H
#define FTAGVALIDATION_PHYSICSVARIABLEPLOTS_H 1

#include "FlavourTaggingTests/FTAGValidationAlgorithm.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include "TH2.h"
#include "map"
#include "tuple"

namespace FTAGValidation {

  class PhysicsVariablePlots :
    public FTAGValidationAlgorithm {

  public:
    PhysicsVariablePlots( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~PhysicsVariablePlots(); 
    
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  protected:
    PhysicsVariablePlots();

    StatusCode initializeHistograms();

    StatusCode analyseTrackParticleObjects( const xAOD::Jet*,
					    const xAOD::Vertex*,
					    const std::string&,
                                            std::string = "" );

    StatusCode computeAndStoreTrackVariables( const std::string&, const xAOD::Jet*,
					      const std::string&, const xAOD::TrackParticle*,
					      const xAOD::Vertex*, const std::string = "" );

    StatusCode analyseBTaggingObject( const xAOD::Jet*,
				      const std::string&,
                                      std::string = "" );

    template < typename T >
      StatusCode defineHistogram( const std::string&,
				  const std::string&,
				  int,double,double,
				  const std::string& path = "" );
    template < typename T >
      StatusCode defineHistogram( const std::string&,
				  const std::string&,
				  int,double,double,
				  int,double,double,
				  const std::string& path = "" );

    template < typename T >
      StatusCode fillHistogram( const std::string&,T );
    template < typename T,typename U >
      StatusCode fillHistogram( const std::string&,T,U );

  private:
    ServiceHandle< ITHistSvc > m_histSvc { this, "THistSvc", "", "Histogramming svc" };

    SG::ReadHandleKey< xAOD::JetContainer >  m_jetKey  { this, "JetCollectionKey", "",   
	"Input Jet collection name" };
    SG::ReadHandleKey< xAOD::VertexContainer > m_vertexKey { this, "VertexCollectionKey", "",
	"Input Vertex collection name" };
    SG::ReadHandleKey< xAOD::TrackParticleContainer > m_trackKey { this, "TrackParticleCollectionKey", "",
	"Input Track collection name" };

  private:
    enum HISTO { TYPE=0,NAME=1,TITLE=2,PATH=3,XBINS=4,XMIN=5,XMAX=6,YBINS=7,YMIN=8,YMAX=9 };

    Gaudi::Property< std::vector< std::vector< std::string > > > m_Histograms1Ddefinitions { this,"Histograms1D",{},
    	"Map with 1D histograms" };
    Gaudi::Property< std::vector< std::vector< std::string > > > m_Histograms2Ddefinitions { this,"Histograms2D",{},
    	"Map with 1D histograms" };

    std::map< const std::string,TH1* > m_Histograms1D;
    std::map< const std::string,TH2* > m_Histograms2D;

  };

  #include "FlavourTaggingTests/PhysicsVariablePlots.icc"

} //> end namespace FTAGValidation

#endif

