/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Old egamma Includes:
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/egammaParamDefs.h"

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
    : AthAlgTool( type, name, parent )
  {
    // Declare the interface(s) provided by the tool:
    declareInterface< IElectronCnvTool >(this);

    declareProperty( "xAODElectronTrackContainerName", m_inDetTrackParticlesGSF = "GSFTrackParticles" );
    declareProperty( "xAODElectronOrigTrackContainerName", m_inDetTrackParticles = "InDetTrackParticles" );
    declareProperty( "xAODCaloClusterContainerName", m_caloClusters = "egClusterCollection");
    declareProperty( "xAODCaloClusterSofteContainerName", m_caloClustersSofte = "LArClusterEMSofte");
    declareProperty( "xAODCaloClusterFrwdContainerName", m_caloClustersFrwd = "LArClusterEMFrwd");
    declareProperty( "xAODCaloClusterOtherContainerName", m_caloClustersOther = "egClusterCollection", 
		     "Most likely used for trigger objects");

  }
  
  StatusCode ElectronCnvTool::convert( const egammaContainer* aod,
          xAOD::ElectronContainer* xaod ) const
  {
      if (!aod) { 
          ATH_MSG_WARNING("No input Electron Collection passed"); 
          return StatusCode::SUCCESS; 
      } 
      //Create the container for forward electrons
      xAOD::ElectronContainer xaodFrwd;
     
      return convert(aod,xaod,&xaodFrwd);
  }

  StatusCode ElectronCnvTool::convert( const egammaContainer* aod,
				       xAOD::ElectronContainer* xaod,
				       xAOD::ElectronContainer* xaodFrwd) const
  {  
    if (!aod) { 
      ATH_MSG_WARNING("No input Electron Collection passed"); 
      return StatusCode::SUCCESS; 
    } 

    // Create the xAOD objects:
    for(const egamma* eg : *aod) {

      // ATH_MSG_DEBUG("Electron author = " << eg->author() 
      // 		    << ", container = " << eg->clusterElementLink().proxy()->name());

      //special treatment for forward electrons     
      if(eg->author(egammaParameters::AuthorFrwd)) {
	if (xaodFrwd) {
	  xAOD::Electron* electron = new xAOD::Electron();
	  xaodFrwd->push_back( electron );
	  // p4
	  electron->setP4(eg->pt(),eg->eta(),eg->phi(), eg->m());
	  // author(s)
	  electron->setAuthor( eg->author() );
	  //OQ
	  electron->setOQ( eg->isgoodoq() );
	  //set Links
	  setLinks(*eg,*electron);
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
          if (eg->trackParticle()) {
            double clE = eg->cluster()->e();
            double pt  =  sqrt(clE*clE - 0.511*0.511)/cosh(eg->trackParticle()->eta());
            double eta = eg->trackParticle()->eta();
            double phi = eg->trackParticle()->phi();
            electron->setP4(pt, eta, phi, 0.511);
          }
          else
            electron->setP4(eg->pt(),eg->eta(),eg->phi(), eg->m());

	  // author(s)
	  electron->setAuthor( eg->author() );
	  //OQ
	  electron->setOQ( eg->isgoodoq() );
	  // charge
	  electron->setCharge( eg->charge() );
	  
	  // Error Matrix
	  if(eg->errors()){
	    
	    const ErrorMatrixEEtaPhiM* oldMatrix = eg->errors()->eEtaPhiMMatrix();
	    if(oldMatrix){
	      Eigen::Matrix<double,4,4> matrix;
	      for(int i(0);i<4;++i){
		for(int j(0);j<4;++j){
		  matrix(i,j) = (*oldMatrix)(i,j);
		} 
	      }
	      Eigen::Matrix<double,4,4> jacobian (EigenP4JacobianEEtaPhiM2PtEtaPhiM(eg->e(),eg->eta(), eg->m()));
	      Eigen::Matrix<double,4,4> covMatrix= jacobian*matrix*jacobian.transpose();
	      electron->setCovMatrix(covMatrix.cast<float>());
	    }
	  }
	  
	  //setParameters
	  setParameters(*eg,*electron);
	  //setIsolations
	  setIsolations(*eg,*electron);
	  //setTrackMatch
	  setTrackMatch(*eg,*electron);	
	  // set Links
	  setLinks(*eg,*electron); 
	  // set derived parameters - should be done last
	  setDerivedParameters(*electron);
  
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
  }
  
  void ElectronCnvTool::checkAndSetParameter(egammaParameters::ParamDef aodParameter,
					    xAOD::EgammaParameters::ShowerShapeType xaodParameter, 
					    const egamma& aodel, xAOD::Electron& xaodel) const {
    double result = aodel.detailValue(aodParameter);
    float parameter = static_cast<float>(result);
    xaodel.setShowerShapeValue(parameter, xaodParameter);
  }
  
  void ElectronCnvTool::setIsolations(const egamma& aodel, xAOD::Electron& xaodel) const {
    checkAndSetIsolation(egammaParameters::etcone20     ,   xAOD::Iso::etcone20     ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::etcone30     ,   xAOD::Iso::etcone30     ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::etcone40     ,   xAOD::Iso::etcone40     ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::ptcone30     ,   xAOD::Iso::ptcone30     ,  aodel,   xaodel);
    checkAndSetIsolation(egammaParameters::ptcone40     ,   xAOD::Iso::ptcone40     ,  aodel,   xaodel);   
    checkAndSetIsolation(egammaParameters::topoetcone20     ,   xAOD::Iso::topoetcone20     ,  aodel,   xaodel);  
    checkAndSetIsolation(egammaParameters::topoetcone30     ,   xAOD::Iso::topoetcone30     ,  aodel,   xaodel);  
    checkAndSetIsolation(egammaParameters::topoetcone40     ,   xAOD::Iso::topoetcone40     ,  aodel,   xaodel); 
  }
  
  void ElectronCnvTool::checkAndSetIsolation(egammaParameters::ParamDef aodParameter,
					    xAOD::Iso::IsolationType xaodParameter, 
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
														   


  void ElectronCnvTool::setLinks(const egamma& aodel, xAOD::Electron& xaodel) const
  {
    std::string clusterContainerName;

    // Need to reset links from old CaloCluster to xAOD::CaloCluster
    ElementLink<xAOD::CaloClusterContainer> newclusterElementLink;
    if( aodel.author(egammaParameters::AuthorElectron) ){ 
      clusterContainerName = m_caloClusters;

      std::vector< ElementLink< xAOD::TrackParticleContainer > > linksToTracks;
      for(unsigned int i(0); i<aodel.nTrackParticles(); ++i){
	linksToTracks.push_back( getNewLink(aodel.trackParticleElementLink(i), m_inDetTrackParticlesGSF) );
      }
      xaodel.setTrackParticleLinks( linksToTracks );

    }
    else if ( aodel.author(egammaParameters::AuthorSofte) ) {
      clusterContainerName = m_caloClustersSofte;
    
      //softe do not use GSF Tracks
      std::vector< ElementLink< xAOD::TrackParticleContainer > > linksToTracks;
      for(unsigned int i(0); i<aodel.nTrackParticles(); ++i){
	linksToTracks.push_back( getNewLink(aodel.trackParticleElementLink(i), m_inDetTrackParticles) );
      }
      xaodel.setTrackParticleLinks( linksToTracks );
    
    }
    else if ( aodel.author(egammaParameters::AuthorFrwd) ) { 
      clusterContainerName = m_caloClustersFrwd;
    }
    else { // assume a trigger object
      clusterContainerName = m_caloClustersOther;
      
      // trigger does not use GSF
      std::vector< ElementLink< xAOD::TrackParticleContainer > > linksToTracks;
      for(unsigned int i(0); i<aodel.nTrackParticles(); ++i){
	linksToTracks.push_back( getNewLink(aodel.trackParticleElementLink(i), m_inDetTrackParticles) );
      }
      xaodel.setTrackParticleLinks( linksToTracks );
    }      

    // If EL name not set, use the original name.
    if (clusterContainerName.empty())
      clusterContainerName = aodel.clusterElementLink().dataID();

    newclusterElementLink.resetWithKeyAndIndex( clusterContainerName,
                                                aodel.clusterElementLink().index()  );

    std::vector< ElementLink< xAOD::CaloClusterContainer > > linksToClusters;
    linksToClusters.push_back(newclusterElementLink);
    xaodel.setCaloClusterLinks(linksToClusters);
    
    if (!newclusterElementLink.isValid()) ATH_MSG_WARNING("Invalid link to cluster");
    
  }
  
  ElementLink<xAOD::TrackParticleContainer> ElectronCnvTool::getNewLink(const ElementLink<Rec::TrackParticleContainer>& oldLink, 
								       const std::string& name) const{
    ElementLink<xAOD::TrackParticleContainer> newLink;
    std::string linkname = name;
    // If not set, use same name as in original link.
    if (linkname.empty())
      linkname = oldLink.dataID();
    newLink.resetWithKeyAndIndex( linkname, oldLink.index() );
    return newLink;
  }
 
  void ElectronCnvTool::setDerivedParameters(xAOD::Electron& xaodel) const{

    // Set some extra derived variables:
    /// e237/e277
    float e237 = 0;
    float e277 = 0;
    if (xaodel.showerShapeValue(e237, xAOD::EgammaParameters::e237) &&
	xaodel.showerShapeValue(e277, xAOD::EgammaParameters::e277)) {
      float reta = (e277 != 0) ? e237/e277 : 0;
      xaodel.setShowerShapeValue(reta, xAOD::EgammaParameters::Reta);
    }
    ///  e233/e237
    float e233 = 0;
    if (xaodel.showerShapeValue(e233, xAOD::EgammaParameters::e233)) {
      float rphi = (e237 != 0) ? e233/e237 : 0;
      xaodel.setShowerShapeValue(rphi, xAOD::EgammaParameters::Rphi);
     }
    ///  (emaxs1-e2tsts1)/(emaxs1+e2tsts1)
    // Eratio
    float emaxs1   = 0;
    float e2tsts1  = 0;
    if (xaodel.showerShapeValue(emaxs1, xAOD::EgammaParameters::emaxs1) &&
	xaodel.showerShapeValue(e2tsts1, xAOD::EgammaParameters::e2tsts1)) {
         float dEmaxs1  = 0;
         float esums1 = emaxs1 + e2tsts1;
         if (fabs(esums1) > 0.0) dEmaxs1   = (emaxs1 - e2tsts1)/esums1;
         xaodel.setShowerShapeValue(dEmaxs1, xAOD::EgammaParameters::Eratio);
    }
    
    /// e2tsts1-emins1
    float emins1   = 0;
    if (xaodel.showerShapeValue(emins1, xAOD::EgammaParameters::emins1)) {
      float deltaE   = e2tsts1 - emins1;
      xaodel.setShowerShapeValue(deltaE, xAOD::EgammaParameters::DeltaE);
     }
    
    /// ethad/et
    const xAOD::CaloCluster*   el_cl = xaodel.caloCluster();
    if(el_cl != nullptr) {
      float elEta    = fabs(el_cl->etaBE(2));
      float elEt     = el_cl->e()/cosh(elEta);
      float ethad    = 0;
      if (xaodel.showerShapeValue(ethad, xAOD::EgammaParameters::ethad)) {
	float raphad = fabs(elEt) != 0. ? ethad/elEt : 0.;
	xaodel.setShowerShapeValue(raphad, xAOD::EgammaParameters::Rhad);
      }
      
      ///  ethad1/et
      float ethad1   = 0;
      if (xaodel.showerShapeValue(ethad1, xAOD::EgammaParameters::ethad1)) {
	float raphad1    = fabs(elEt) != 0. ? ethad1/elEt : 0.;
	xaodel.setShowerShapeValue(raphad1, xAOD::EgammaParameters::Rhad1);
      }
    } else {
      ATH_MSG_WARNING("No xAOD::CaloCluster was found during the conversion egammaContainer -> xAOD::ElectronContainer");
    }
  }

} // namespace xAODMaker

