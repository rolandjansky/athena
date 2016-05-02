/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/egammaParamDefs.h"
#include "FourMom/EigenP4JacobianEEtaPhiM2PtEtaPhiM.h"

// Local include(s):
#include "PhotonCnvTool.h"


namespace xAODMaker {

  PhotonCnvTool::PhotonCnvTool(const std::string& type, 
			       const std::string& name,
			       const IInterface* parent )
    : AthAlgTool( type, name, parent )
  {  
    // Declare the interface(s) provided by the tool:
    declareInterface< IPhotonCnvTool >(this);

    declareProperty( "xAODCaloClusterContainerName", m_caloClusters = "egClusterCollection");
    declareProperty( "xAODCaloClusterTopoContainerName", m_caloClustersTopo = "EMCaloClusters");
    declareProperty( "xAODConversionContainerName",  m_vertexContainer  = "GSFConversionVertices");
    declareProperty( "xAODCaloClusterOtherContainerName", m_caloClustersOther = "egClusterCollection", 
		     "Most likely used for trigger objects");
   
    
  }

  StatusCode PhotonCnvTool::initialize() {

    ATH_MSG_DEBUG( "Initializing - Package version: " << PACKAGE_VERSION );

    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode PhotonCnvTool::convert( const egammaContainer* aod,
				     xAOD::PhotonContainer* xaod ) const
  {

    if (!aod) { 
      ATH_MSG_WARNING("No input Photon Collection passed"); 
      return StatusCode::SUCCESS; 
    }
    if (!xaod) { 
      ATH_MSG_WARNING("No output Photon Collection passed"); 
      return StatusCode::SUCCESS; 
    }
    // Create the xAOD objects:
    const auto end = aod->end();
    for(auto itr = aod->begin(); itr != end; ++itr ) {
      // Create the xAOD object:
      xAOD::Photon* photon = new xAOD::Photon();
      xaod->push_back( photon );
      
      // p4
      photon->setP4((*itr)->pt(),(*itr)->eta(),(*itr)->phi(),(*itr)->m());
      
      // author(s)
      photon->setAuthor( (*itr)->author() );
      
      //OQ
      photon->setOQ( (*itr)->isgoodoq() );
      
    
      // Error Matrix
      if((*itr)->errors()){
 
	const ErrorMatrixEEtaPhiM* oldMatrix = (*itr)->errors()->eEtaPhiMMatrix();
	if(oldMatrix){
	  Eigen::Matrix<double,4,4> matrix;
	  for(int i(0);i<4;++i){
	    for(int j(0);j<4;++j){
	      matrix(i,j) = (*oldMatrix)(i,j);
	    } 
	  }
	  Eigen::Matrix<double,4,4> jacobian (EigenP4JacobianEEtaPhiM2PtEtaPhiM((*itr)->e(),(*itr)->eta(),0));
	  Eigen::Matrix<double,4,4> covMatrix= jacobian*matrix*jacobian.transpose();
	  photon->setCovMatrix(covMatrix.cast<float>());
	}
      }
      			              			
      //setParameters
      setParameters(**itr,*photon);
      //setIsolations
      setIsolations(**itr,*photon);
      //setLinks  
      setLinks(**itr,*photon);

    }

    // Return gracefully - like a elephant on roller skates :
    return StatusCode::SUCCESS;
  }
  
  void PhotonCnvTool::setParameters(const egamma& aodph, xAOD::Photon& xaodph) const{
    // We're not doing all AOD parameters here because some are dropped, and some are moved elsewhere.
    checkAndSetParameter(egammaParameters::e011        ,   xAOD::EgammaParameters::e011        ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::e033        ,   xAOD::EgammaParameters::e033        ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::e132        ,   xAOD::EgammaParameters::e132        ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::e1152       ,   xAOD::EgammaParameters::e1152       ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::ethad       ,   xAOD::EgammaParameters::ethad       ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::ethad1      ,   xAOD::EgammaParameters::ethad1      ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::ehad1       ,   xAOD::EgammaParameters::ehad1       ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::f1          ,   xAOD::EgammaParameters::f1          ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::f3          ,   xAOD::EgammaParameters::f3          ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::f1core      ,   xAOD::EgammaParameters::f1core      ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::f3core      ,   xAOD::EgammaParameters::f3core      ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::e233        ,   xAOD::EgammaParameters::e233        ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::e235        ,   xAOD::EgammaParameters::e235        ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::e255        ,   xAOD::EgammaParameters::e255        ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::e237        ,   xAOD::EgammaParameters::e237        ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::e277        ,   xAOD::EgammaParameters::e277        ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::e333        ,   xAOD::EgammaParameters::e333        ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::e335        ,   xAOD::EgammaParameters::e335        ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::e337        ,   xAOD::EgammaParameters::e337        ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::e377        ,   xAOD::EgammaParameters::e377        ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::weta1       ,   xAOD::EgammaParameters::weta1       ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::weta2       ,   xAOD::EgammaParameters::weta2       ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::e2ts1       ,   xAOD::EgammaParameters::e2ts1       ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::e2tsts1     ,   xAOD::EgammaParameters::e2tsts1     ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::fracs1      ,   xAOD::EgammaParameters::fracs1      ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::widths1     ,   xAOD::EgammaParameters::widths1     ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::widths2     ,   xAOD::EgammaParameters::widths2     ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::poscs1      ,   xAOD::EgammaParameters::poscs1      ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::poscs2      ,   xAOD::EgammaParameters::poscs2      ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::asy1        ,   xAOD::EgammaParameters::asy1        ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::pos         ,   xAOD::EgammaParameters::pos         ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::pos7        ,   xAOD::EgammaParameters::pos7        ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::barys1      ,   xAOD::EgammaParameters::barys1      ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::wtots1      ,   xAOD::EgammaParameters::wtots1      ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::emins1      ,   xAOD::EgammaParameters::emins1      ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::emaxs1      ,   xAOD::EgammaParameters::emaxs1      ,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::r33over37allcalo ,   xAOD::EgammaParameters::r33over37allcalo,  aodph,   xaodph);
    checkAndSetParameter(egammaParameters::ecore       ,   xAOD::EgammaParameters::ecore       ,  aodph,   xaodph);
  }
  
  void PhotonCnvTool::checkAndSetParameter(egammaParameters::ParamDef aodParameter,xAOD::EgammaParameters::ShowerShapeType xaodParameter, const egamma& aodph, xAOD::Photon& xaodph) const {
    double result = aodph.detailValue(aodParameter);
    float parameter = static_cast<float>(result);
    xaodph.setShowerShapeValue(parameter, xaodParameter);
  }
  
  void PhotonCnvTool::setIsolations(const egamma& aodph, xAOD::Photon& xaodph) const {
    checkAndSetIsolation(egammaParameters::etcone20     ,   xAOD::Iso::etcone20     ,  aodph,   xaodph);
    checkAndSetIsolation(egammaParameters::etcone30     ,   xAOD::Iso::etcone30     ,  aodph,   xaodph);
    checkAndSetIsolation(egammaParameters::etcone40     ,   xAOD::Iso::etcone40     ,  aodph,   xaodph);
    checkAndSetIsolation(egammaParameters::ptcone20     ,   xAOD::Iso::ptcone20     ,  aodph,   xaodph);
    checkAndSetIsolation(egammaParameters::ptcone30     ,   xAOD::Iso::ptcone30     ,  aodph,   xaodph);
    checkAndSetIsolation(egammaParameters::ptcone40     ,   xAOD::Iso::ptcone40     ,  aodph,   xaodph);   
    checkAndSetIsolation(egammaParameters::topoetcone20     ,   xAOD::Iso::topoetcone20     ,  aodph,   xaodph);  
    checkAndSetIsolation(egammaParameters::topoetcone30     ,   xAOD::Iso::topoetcone30     ,  aodph,   xaodph);  
    checkAndSetIsolation(egammaParameters::topoetcone40     ,   xAOD::Iso::topoetcone40     ,  aodph,   xaodph); 
  }
  
  void PhotonCnvTool::checkAndSetIsolation(egammaParameters::ParamDef aodParameter,xAOD::Iso::IsolationType xaodParameter, 
					   const egamma& aodph, xAOD::Photon& xaodph) const {
    double result = aodph.detailValue(aodParameter);
    float isolation = static_cast<float>(result);
    xaodph.setIsolationValue(isolation, xaodParameter);
  }
  

  void PhotonCnvTool::setLinks(const egamma& aodph, xAOD::Photon& xaodph) const {
    // Need to reset links from old CaloCluster to xAOD::CaloCluster
    std::string clusterContainerName;
    
    //Change link depending on the photon author
    //Topo seeded photons
    if( aodph.author(egammaParameters::AuthorCaloTopo35)) {
      clusterContainerName = m_caloClustersTopo;
    }
    //Standard photons
    else if (aodph.author(egammaParameters::AuthorPhoton | egammaParameters::AuthorRConv)) { 
      clusterContainerName = m_caloClusters;
    } 
    // others (trigger)
    else { 
      clusterContainerName = m_caloClustersOther;
    } 

    // If EL name not set, use the original name.
    if (clusterContainerName.empty())
      clusterContainerName = aodph.clusterElementLink().dataID();
    ElementLink<xAOD::CaloClusterContainer> newclusterElementLink;
    newclusterElementLink.resetWithKeyAndIndex( clusterContainerName,
                                                aodph.clusterElementLink().index()  );

    std::vector< ElementLink< xAOD::CaloClusterContainer > > linksToClusters;
    linksToClusters.push_back(newclusterElementLink);
    xaodph.setCaloClusterLinks(linksToClusters);
    
    // Decorate cluster with position in calo
    if (newclusterElementLink.isValid()) ATH_MSG_WARNING("Invalid link to cluster");
		
    // Need to reset links from old VxVertex to xAOD::Vertex
    std::vector< ElementLink< xAOD::VertexContainer > > linksToVertices;
    for(unsigned int i(0); i<aodph.nConversions(); ++i){
      linksToVertices.push_back( getNewLink(aodph.conversionElementLink(i), m_vertexContainer) );
    }
    xaodph.setVertexLinks( linksToVertices );
  }
  
  ElementLink<xAOD::VertexContainer> PhotonCnvTool::getNewLink(const ElementLink<VxContainer>& oldLink, const std::string& name) const{
    std::string linkname = name;
    // If not set, use same name as in original link.
    if (linkname.empty())
      linkname = oldLink.dataID();
    ElementLink<xAOD::VertexContainer> newLink;
    newLink.resetWithKeyAndIndex( linkname, oldLink.index() );
    // std::cout<<"Old link is "<<(oldLink.isValid()?"VALID":"INVALID")
    //          <<" and new link (pointing to"<<name<<") is "<<(newLink.isValid()?"VALID":"INVALID")<<std::endl;
    return newLink;
  }



} // namespace xAODMaker


//  LocalWords:  Gaudi
