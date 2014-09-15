/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Old egamma Includes:
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/egammaParamDefs.h"
#include "egammaInterfaces/IegammaBaseTool.h"
#include "egammaInterfaces/IEMClusterTool.h"

//New egamma
#include "xAODEgamma/ElectronAuxContainer.h"

//Misc
#include "FourMom/EigenP4JacobianEEtaPhiM2PtEtaPhiM.h"

// Local include(s):
#include "ElectronCnvTool.h"


namespace xAODMaker {

  ElectronCnvTool::ElectronCnvTool(const std::string& type, 
				  const std::string& name,
				  const IInterface* parent )
    : AthAlgTool( type, name, parent ) {

    // Declare the interface(s) provided by the tool:
    declareInterface< IElectronCnvTool >(this);

    declareProperty( "RunPID", m_runPID=true );
    declareProperty("PIDBuilder", m_PIDBuilder, "The PID Builder tool configured for electrons");

    declareProperty( "xAODElectronTrackContainerName", m_inDetTrackParticlesGSF = "GSFTrackParticles" );
    declareProperty( "xAODElectronOrigTrackContainerName", m_inDetTrackParticles = "InDetTrackParticles" );
    declareProperty( "xAODCaloClusterContainerNamer", m_caloClusters = "egClusterCollection");
    declareProperty( "xAODCaloClusterSofteContainerName", m_caloClustersSofte = "LArClusterEMSofte");
    declareProperty( "xAODCaloClusterFrwdContainerName", m_caloClustersFrwd = "LArClusterEMFrwd");
    declareProperty( "xAODCaloClusterOtherContainerName", m_caloClustersOther = "egClusterCollection", 
		     "Most likely used for trigger objects");
		declareProperty( "EMClusterTool", m_EMClusterTool, "EMClusterTool" );

  }



  
  StatusCode ElectronCnvTool::initialize() {

    ATH_MSG_DEBUG( "Initializing - Package version: " << PACKAGE_VERSION );

    if (m_runPID) {
      CHECK(m_PIDBuilder.retrieve());
    }

    CHECK(m_EMClusterTool.retrieve());
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode ElectronCnvTool::convert( const DataVector<egamma>* aod,
				       xAOD::ElectronContainer* xaod,
				       xAOD::ElectronContainer* xaodFrwd) const
  {  
    if (!aod) { 
      ATH_MSG_WARNING("No input Electron Collection passed"); 
      return StatusCode::SUCCESS; 
    } 

    // Create the xAOD objects:
    const auto end = aod->end();
    for(auto itr = aod->begin(); itr != end; ++itr ) {

      // ATH_MSG_DEBUG("Electron author = " << (*itr)->author() 
      // 		    << ", container = " << (*itr)->clusterElementLink().proxy()->name());

      //special treatment for forward electrons     
      if((*itr)->author(egammaParameters::AuthorFrwd)) {
	if (xaodFrwd) {
	  xAOD::Electron* electron = new xAOD::Electron();
	  xaodFrwd->push_back( electron );
	  // p4
	  electron->setP4((*itr)->pt(),(*itr)->eta(),(*itr)->phi(), (*itr)->m());
	  // author(s)
	  electron->setAuthor( (*itr)->author() );
	  //OQ
	  electron->setOQ( (*itr)->isgoodoq() );
	  //set Links
	  setLinks(**itr,*electron);
	} else {
	  ATH_MSG_WARNING("Found a foward electron, but xaodFrwd == NULL");
	}
      }//end if forward electron
      else{
	if (xaod) {
	  xAOD::Electron* electron = new xAOD::Electron();
	  // Create the xAOD object:
	  xaod->push_back( electron );
	  // p4
	  electron->setP4((*itr)->pt(),(*itr)->eta(),(*itr)->phi(), (*itr)->m());
	  // author(s)
	  electron->setAuthor( (*itr)->author() );
	  //OQ
	  electron->setOQ( (*itr)->isgoodoq() );
	  // charge
	  electron->setCharge( (*itr)->charge() );
	  
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
	      Eigen::Matrix<double,4,4> jacobian (EigenP4JacobianEEtaPhiM2PtEtaPhiM((*itr)->e(),(*itr)->eta(), (*itr)->m()));
	      Eigen::Matrix<double,4,4> covMatrix= jacobian*matrix*jacobian.transpose();
	      electron->setCovMatrix(covMatrix.cast<float>());
	    }
	  }
	  
	  //setParameters
	  setParameters(**itr,*electron);
	  //setIsolations
	  setIsolations(**itr,*electron);
	  //setTrackMatch
	  setTrackMatch(**itr,*electron);	
	  // set Links
	  setLinks(**itr,*electron); 
	  
	  if (m_runPID) {
	    CHECK(m_PIDBuilder->execute(electron));
	  }
	} else {
	  ATH_MSG_WARNING("Found a regular electron, but xaod == NULL");
	}
	  
      }//end If not forward
    }
    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  
  void ElectronCnvTool::setParameters(const egamma& aodel, xAOD::Electron& xaodel) const {
    // We're not doing all AOD parameters here because some are dropped, and some are moved elsewhere.
    checkAndSetParameter(egammaParameters::e011        ,   xAOD::EgammaParameters::e011        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::e033        ,   xAOD::EgammaParameters::e033        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::e132        ,   xAOD::EgammaParameters::e132        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::e1152       ,   xAOD::EgammaParameters::e1152       ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::ethad       ,   xAOD::EgammaParameters::ethad       ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::ethad1      ,   xAOD::EgammaParameters::ethad1      ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::ehad1       ,   xAOD::EgammaParameters::ehad1       ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::f1          ,   xAOD::EgammaParameters::f1          ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::f3          ,   xAOD::EgammaParameters::f3          ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::f1core      ,   xAOD::EgammaParameters::f1core      ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::f3core      ,   xAOD::EgammaParameters::f3core      ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::e233        ,   xAOD::EgammaParameters::e233        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::e235        ,   xAOD::EgammaParameters::e235        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::e255        ,   xAOD::EgammaParameters::e255        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::e237        ,   xAOD::EgammaParameters::e237        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::e277        ,   xAOD::EgammaParameters::e277        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::e333        ,   xAOD::EgammaParameters::e333        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::e335        ,   xAOD::EgammaParameters::e335        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::e337        ,   xAOD::EgammaParameters::e337        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::e377        ,   xAOD::EgammaParameters::e377        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::weta1       ,   xAOD::EgammaParameters::weta1       ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::weta2       ,   xAOD::EgammaParameters::weta2       ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::e2ts1       ,   xAOD::EgammaParameters::e2ts1       ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::e2tsts1     ,   xAOD::EgammaParameters::e2tsts1     ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::fracs1      ,   xAOD::EgammaParameters::fracs1      ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::widths1     ,   xAOD::EgammaParameters::widths1     ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::widths2     ,   xAOD::EgammaParameters::widths2     ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::poscs1      ,   xAOD::EgammaParameters::poscs1      ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::poscs2      ,   xAOD::EgammaParameters::poscs2      ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::asy1        ,   xAOD::EgammaParameters::asy1        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::pos         ,   xAOD::EgammaParameters::pos         ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::pos7        ,   xAOD::EgammaParameters::pos7        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::barys1      ,   xAOD::EgammaParameters::barys1      ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::wtots1      ,   xAOD::EgammaParameters::wtots1      ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::emins1      ,   xAOD::EgammaParameters::emins1      ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::emaxs1      ,   xAOD::EgammaParameters::emaxs1      ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::r33over37allcalo ,   xAOD::EgammaParameters::r33over37allcalo,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::ecore       ,   xAOD::EgammaParameters::ecore       ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::zvertex     ,   xAOD::EgammaParameters::zvertex     ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::errz        ,   xAOD::EgammaParameters::errz        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::etap        ,   xAOD::EgammaParameters::etap        ,  aodel,   xaodel);
    checkAndSetParameter(egammaParameters::depth       ,   xAOD::EgammaParameters::depth       ,  aodel,   xaodel);
  }
  
  void ElectronCnvTool::checkAndSetParameter(egammaParameters::ParamDef aodParameter,
					    xAOD::EgammaParameters::ShowerShapeType xaodParameter, 
					    const egamma& aodel, xAOD::Electron& xaodel) const {
    double result = aodel.detailValue(aodParameter);
    float parameter = static_cast<float>(result);
    xaodel.setShowerShapeValue(parameter, xaodParameter);
  }
  
  void ElectronCnvTool::setIsolations(const egamma& aodel, xAOD::Electron& xaodel) const {
    checkAndSetIsolation(egammaParameters::etcone15     ,   xAOD::EgammaParameters::etcone15     ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::etcone20     ,   xAOD::EgammaParameters::etcone20     ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::etcone25     ,   xAOD::EgammaParameters::etcone25     ,  aodel,   xaodel);    
    checkAndSetIsolation(egammaParameters::etcone30     ,   xAOD::EgammaParameters::etcone30     ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::etcone35     ,   xAOD::EgammaParameters::etcone35     ,  aodel,   xaodel);  
    checkAndSetIsolation(egammaParameters::etcone40     ,   xAOD::EgammaParameters::etcone40     ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::nucone20  ,   xAOD::EgammaParameters::nucone20     ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::nucone30  ,   xAOD::EgammaParameters::nucone30     ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::nucone40  ,   xAOD::EgammaParameters::nucone40     ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::ptcone20     ,   xAOD::EgammaParameters::ptcone20     ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::ptcone30     ,   xAOD::EgammaParameters::ptcone30     ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::ptcone40     ,   xAOD::EgammaParameters::ptcone40     ,  aodel,   xaodel);   
    checkAndSetIsolation(egammaParameters::etcone15_ptcorrected     ,   xAOD::EgammaParameters::etcone15_ptcorrected     ,  aodel,   xaodel);  
    checkAndSetIsolation(egammaParameters::etcone20_ptcorrected     ,   xAOD::EgammaParameters::etcone20_ptcorrected     ,  aodel,   xaodel);  
    checkAndSetIsolation(egammaParameters::etcone25_ptcorrected     ,   xAOD::EgammaParameters::etcone25_ptcorrected     ,  aodel,   xaodel);    
    checkAndSetIsolation(egammaParameters::etcone30_ptcorrected     ,   xAOD::EgammaParameters::etcone30_ptcorrected     ,  aodel,   xaodel);  
    checkAndSetIsolation(egammaParameters::etcone35_ptcorrected     ,   xAOD::EgammaParameters::etcone35_ptcorrected     ,  aodel,   xaodel);  
    checkAndSetIsolation(egammaParameters::etcone40_ptcorrected     ,   xAOD::EgammaParameters::etcone40_ptcorrected     ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::etcone20_corrected       ,   xAOD::EgammaParameters::etcone20_corrected       ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::etcone30_corrected       ,   xAOD::EgammaParameters::etcone30_corrected       ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::etcone40_corrected       ,   xAOD::EgammaParameters::etcone40_corrected       ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::topoetcone20     ,   xAOD::EgammaParameters::topoetcone20     ,  aodel,   xaodel);  
    checkAndSetIsolation(egammaParameters::topoetcone30     ,   xAOD::EgammaParameters::topoetcone30     ,  aodel,   xaodel);  
    checkAndSetIsolation(egammaParameters::topoetcone40     ,   xAOD::EgammaParameters::topoetcone40     ,  aodel,   xaodel); 
    checkAndSetIsolation(egammaParameters::topoetcone40_ptcorrected   ,   xAOD::EgammaParameters::topoetcone40_ptcorrected   ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::topoetcone40_corrected     ,   xAOD::EgammaParameters::topoetcone40_corrected     ,  aodel,   xaodel);
  }
  
  void ElectronCnvTool::checkAndSetIsolation(egammaParameters::ParamDef aodParameter,
					    xAOD::EgammaParameters::IsolationType xaodParameter, 
					    const egamma& aodel, xAOD::Electron& xaodel) const {
    double result = aodel.detailValue(aodParameter);
    float isolation = static_cast<float>(result);
    xaodel.setIsolationValue(isolation, xaodParameter);
  }
  

  void ElectronCnvTool::setTrackMatch(const egamma& aodel, xAOD::Electron& xaodel) const {
    checkAndSetTrackMatch(egammaParameters::deltaEta0     ,   xAOD::EgammaParameters::deltaEta0     ,  aodel,   xaodel);
    checkAndSetTrackMatch(egammaParameters::deltaEta1     ,   xAOD::EgammaParameters::deltaEta1     ,  aodel,   xaodel);
    checkAndSetTrackMatch(egammaParameters::deltaEta2     ,   xAOD::EgammaParameters::deltaEta2     ,  aodel,   xaodel);
    checkAndSetTrackMatch(egammaParameters::deltaEta3     ,   xAOD::EgammaParameters::deltaEta3     ,  aodel,   xaodel);
    checkAndSetTrackMatch(egammaParameters::deltaPhi0     ,   xAOD::EgammaParameters::deltaPhi0     ,  aodel,   xaodel);
    checkAndSetTrackMatch(egammaParameters::deltaPhi1     ,   xAOD::EgammaParameters::deltaPhi1     ,  aodel,   xaodel);
    checkAndSetTrackMatch(egammaParameters::deltaPhi2     ,   xAOD::EgammaParameters::deltaPhi2     ,  aodel,   xaodel);
    checkAndSetTrackMatch(egammaParameters::deltaPhi3     ,   xAOD::EgammaParameters::deltaPhi3     ,  aodel,   xaodel);
    checkAndSetTrackMatch(egammaParameters::deltaPhiFromLastMeasurement,   xAOD::EgammaParameters::deltaPhiFromLastMeasurement,  aodel,   xaodel);
    checkAndSetTrackMatch(egammaParameters::deltaPhiRescaled,   xAOD::EgammaParameters::deltaPhiRescaled2     ,  aodel,   xaodel);
  
  }

  void ElectronCnvTool::checkAndSetTrackMatch(egammaParameters::ParamDef aodParameter,
					     xAOD::EgammaParameters::TrackCaloMatchType xaodParameter, 
					     const egamma& aodel, xAOD::Electron& xaodel) const {
    double result = aodel.detailValue(aodParameter);
    float isolation = static_cast<float>(result);
    xaodel.setTrackCaloMatchValue(isolation, xaodParameter);
  }
														   


  void ElectronCnvTool::setLinks(const egamma& aodel, xAOD::Electron& xaodel) const {
    // Need to reset links from old CaloCluster to xAOD::CaloCluster
    ElementLink<xAOD::CaloClusterContainer> newclusterElementLink;
    if( aodel.author(egammaParameters::AuthorElectron) ){ 
      newclusterElementLink.resetWithKeyAndIndex( m_caloClusters, aodel.clusterElementLink().index()  );

      std::vector< ElementLink< xAOD::TrackParticleContainer > > linksToTracks;
      for(unsigned int i(0); i<aodel.nTrackParticles(); ++i){
	linksToTracks.push_back( getNewLink(aodel.trackParticleElementLink(i), m_inDetTrackParticlesGSF) );
      }
      xaodel.setTrackParticleLinks( linksToTracks );

    }
    else if ( aodel.author(egammaParameters::AuthorSofte) ) {
      newclusterElementLink.resetWithKeyAndIndex( m_caloClustersSofte, aodel.clusterElementLink().index()  );
    
      //softe do not use GSF Tracks
      std::vector< ElementLink< xAOD::TrackParticleContainer > > linksToTracks;
      for(unsigned int i(0); i<aodel.nTrackParticles(); ++i){
	linksToTracks.push_back( getNewLink(aodel.trackParticleElementLink(i), m_inDetTrackParticles) );
      }
      xaodel.setTrackParticleLinks( linksToTracks );
    
    }
    else if ( aodel.author(egammaParameters::AuthorFrwd) ) { 
      newclusterElementLink.resetWithKeyAndIndex( m_caloClustersFrwd, aodel.clusterElementLink().index()  );
    }
    else { // assume a trigger object
      newclusterElementLink.resetWithKeyAndIndex( m_caloClustersOther, aodel.clusterElementLink().index()  );
      
      // trigger does not use GSF
      std::vector< ElementLink< xAOD::TrackParticleContainer > > linksToTracks;
      for(unsigned int i(0); i<aodel.nTrackParticles(); ++i){
	linksToTracks.push_back( getNewLink(aodel.trackParticleElementLink(i), m_inDetTrackParticles) );
      }
      xaodel.setTrackParticleLinks( linksToTracks );
    }      

    std::vector< ElementLink< xAOD::CaloClusterContainer > > linksToClusters;
    linksToClusters.push_back(newclusterElementLink);
    xaodel.setCaloClusterLinks(linksToClusters);
    
    // Decorate cluster with position in calo
    if (newclusterElementLink.isValid())
    { 
      ATH_MSG_DEBUG("Decorating cluster");
      xAOD::CaloCluster *cluster = const_cast<xAOD::CaloCluster*>(*newclusterElementLink);
      if (cluster)
      {
        m_EMClusterTool->fillPositionsInCalo(*cluster);
      }
      else ATH_MSG_DEBUG("Could not dereference / cast link to cluster");
    }
    else ATH_MSG_WARNING("Invalid link to cluster");
    
  }
  
  ElementLink<xAOD::TrackParticleContainer> ElectronCnvTool::getNewLink(const ElementLink<Rec::TrackParticleContainer>& oldLink, 
								       const std::string& name) const{
    ElementLink<xAOD::TrackParticleContainer> newLink;
    newLink.resetWithKeyAndIndex( name, oldLink.index() );
    return newLink;
  }


} // namespace xAODMaker

