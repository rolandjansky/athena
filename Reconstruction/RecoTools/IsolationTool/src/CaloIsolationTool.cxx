/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// CaloIsolationTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "CaloIsolationTool.h"
#include "xAODEgamma/Egamma.h"
#include "FourMomUtils/P4Helpers.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloClusterCorrection/CaloFillRectangularCluster.h"

#include "boost/foreach.hpp"
#include "boost/format.hpp"
#include <cmath> 


namespace xAOD {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  CaloIsolationTool::CaloIsolationTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
        m_toCalo("TrackInCaloTools/extrapolMuonInCaloTool"),
	m_caloFillRectangularTool("")
  {
    declareInterface<ICaloIsolationTool>(this);
    declareProperty("ExtrapolTrackToCaloTool", m_toCalo);

    // Topo Isolation parameters
    declareProperty("CaloClusterLocation", m_CaloCalTopoCluster = "CaloCalTopoCluster");
    declareProperty("UseEMScale", m_useEMScale = true, "Use TopoClusters at the EM scale.");
    declareProperty("ConeSizes", m_coneSizes, "Cone sizes for which to calculate isolation.");
    declareProperty("CaloFillRectangularClusterTool", m_caloFillRectangularTool, "Handle of the CaloFillRectangularClusterTool");

    for (int i=2; i<5; i++){
      m_coneSizes.push_back (i / 10.);
      m_coneSizes2.push_back((i / 10.)* (i / 10.)); 
    } 
  }

  CaloIsolationTool::~CaloIsolationTool()
  {
  
  }

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode CaloIsolationTool::initialize() {

    ATH_CHECK(m_toCalo.retrieve());

    if(!m_caloFillRectangularTool.empty()){
      ATH_CHECK( m_caloFillRectangularTool.retrieve());
      m_caloFillRectangularToolPtr=dynamic_cast<CaloFillRectangularCluster*>(&(*m_caloFillRectangularTool));
      if (!m_caloFillRectangularToolPtr) {
	ATH_MSG_WARNING("Could not retrieve CaloFillRectangularCluster");      
      }
    }

   return StatusCode::SUCCESS;
  }

  StatusCode CaloIsolationTool::finalize() {
    return StatusCode::SUCCESS;
  }



  bool CaloIsolationTool::caloIsolation( CaloIsolation& result, const IParticle& tp, const std::vector<Iso::IsolationType>& cones, 
                                         CaloIsolationTool::SubtractionStrategy strategy ) {

    // muon isolation
    const TrackParticle* trkp = dynamic_cast<const TrackParticle*>(&tp);
    if( trkp ) return caloIsolation(result,*trkp,cones,strategy);

    // egamma topo isolation
    const xAOD::Egamma* egam = dynamic_cast<const xAOD::Egamma*>(&tp);
    if( egam ) return topoConeIsolationEG(result, egam, cones, strategy);

    ATH_MSG_WARNING("Isolation only supported for TrackParticles and Egamma");

    return true;
  }




  bool CaloIsolationTool::caloIsolation( CaloIsolation& result, const TrackParticle& tp, const std::vector<Iso::IsolationType>& isoTypes, 
                                         CaloIsolationTool::SubtractionStrategy strategy ) {
    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }
    result.etcones.clear();

    // get cones
    Iso::IsolationFlavour theFlavour = Iso::isolationFlavour(isoTypes.front());

    if (theFlavour == Iso::etcone)     return etConeIsolationTP  (result, tp, isoTypes, strategy);

    ATH_MSG_WARNING("Unsupported isolation flavour passed, cannot calculate isolation " << static_cast<int>(theFlavour));

    return false;
  }





  bool CaloIsolationTool::etConeIsolationTP( CaloIsolation& result, const TrackParticle& tp, const std::vector<Iso::IsolationType>& isoTypes, 
					     CaloIsolationTool::SubtractionStrategy strategy ) {
    // for now always use track, should move to extrapolation to calo entrance 
    const Trk::Track* track = *tp.trackLink();
    if( !track  ) {
      ATH_MSG_WARNING("Failed to access track");
      return false;
    }    
    bool onlyEM = false;
    std::vector<double> conesf;
    for( auto isoType : isoTypes ){
      Iso::IsolationFlavour flavour = Iso::isolationFlavour(isoType);
      if( flavour != Iso::etcone && flavour != Iso::etconeNoEm ) {
        ATH_MSG_WARNING("Unsupported isolation type passed, cannot calculate isolation " << static_cast<int>(isoType));
        return false;
      }
      if( flavour == Iso::etconeNoEm ) onlyEM = true;
      float coneSize = Iso::coneSize(isoType);
      conesf.push_back(coneSize);
    }
    // sort in decending order
    std::sort(conesf.begin(),conesf.end(),[](double i, double j) { return i>j; });
    std::vector<double> energies;

    // should decode onlyEM from IsolationType
    CaloLayer lastLayer = onlyEM ? em3 : hec3;

    // get energies in cone for track
    m_toCalo->getMeasuredEnergy(track, conesf, energies, ps, lastLayer);

    if( energies.size() != conesf.size() ) {
      ATH_MSG_WARNING("Inconsistent number of energies return by: " << m_toCalo << " requested " << conesf.size() << " but got " << energies.size() );
      return false;
    }

    // calculate etcore 
    double ecore = strategy == Ecore ? m_toCalo->getEcore(track,ps,lastLayer) : 0.;
    result.ecore = ecore;

    // update result
    for( unsigned int i=0;i<conesf.size();++i ) result.etcones.push_back(energies[i]-ecore);

    if( msgLvl(MSG::DEBUG) ){
      msg(MSG::DEBUG) << "Calculated calo isolation: ";
      if( strategy == Ecore ) msg(MSG::DEBUG) << "ecore " << ecore;
      for( unsigned int i = 0; i< result.etcones.size();++i ){
        msg(MSG::DEBUG) << " coneSize " << std::setw(3) << conesf[i] << " value " << result.etcones[i] << " energy " << energies[i];
      }
      msg(MSG::DEBUG) << endreq;
    }
    
    return true;
  }






  bool CaloIsolationTool::topoConeIsolationEG(CaloIsolation& result, const xAOD::Egamma* eg, const std::vector<Iso::IsolationType>& isotype, 
					    CaloIsolationTool::SubtractionStrategy strategy)
  {

    if ( isotype.size() == 0 ) {
      ATH_MSG_DEBUG("No isolation required");
      return false;
    }

    // Try and evaluate topo_container
    const CaloClusterContainer *topo_container = 0;
    if(!evtStore()->contains<xAOD::CaloClusterContainer>(m_CaloCalTopoCluster))
    {
      ATH_MSG_FATAL("Specified ClusterLocation " << m_CaloCalTopoCluster << " does not exist!");
      return 0;
    }

    if(evtStore()->retrieve(topo_container, m_CaloCalTopoCluster).isFailure()) 
    {
      ATH_MSG_FATAL( "Unable to retrieve " << m_CaloCalTopoCluster );
      return 0;
    }
    
    // Reset vectors
    m_coneSizes.clear();
    m_coneSizes2.clear();
    result.etcones.clear();
    
    for (std::vector<xAOD::Iso::IsolationType>::const_iterator it = isotype.begin(); it != isotype.end(); ++it) {
      switch (*it) 
	{

	case xAOD::Iso::IsolationType::topoetcone10_core57cells:
	  m_coneSizes.push_back(0.1);        
	  m_coneSizes2.push_back(0.1*0.1);                
	  result.etcones.push_back(0.);
	  break;
	case xAOD::Iso::IsolationType::topoetcone20_core57cells:
	  m_coneSizes.push_back(0.2);        
	  m_coneSizes2.push_back(0.2*0.2);                
	  result.etcones.push_back(0.);
	  break;
	case xAOD::Iso::IsolationType::topoetcone30_core57cells:
	  m_coneSizes.push_back(0.3);        
	  m_coneSizes2.push_back(0.3*0.3);                
	  result.etcones.push_back(0.);
	  break;
	case xAOD::Iso::IsolationType::topoetcone40_core57cells:
	  m_coneSizes.push_back(0.4);
	  m_coneSizes2.push_back(0.4*0.4);                        
	  result.etcones.push_back(0.);
	  break;
	default:
	  ATH_MSG_WARNING("Topo isolation cannot be calculated for "<< (*it));
	}
    }
   
    if (m_coneSizes.size() != isotype.size()) 
      ATH_MSG_WARNING("Not all requested cone sizes are considered;  requested: " << isotype.size()
		      << " sizes calculated: "<< m_coneSizes.size() );

    if (m_coneSizes.size() == 0)
    {
      ATH_MSG_ERROR("No cones to process!");
      return 0;
    }
    
    // Photon and topo cluster lists may be long.
    // So make a local copy of the cluster list.
    std::vector<const xAOD::CaloCluster*> clusts;
    auto clItr  = topo_container->begin();
    auto clItrE = topo_container->end();
    
    //We need just the clusters inside the dR of the largest cone used for isolation
    auto max_cone_iter=std::max_element(m_coneSizes2.begin(), m_coneSizes2.end());
    float max_cone= (*max_cone_iter);
    for(; clItr != clItrE; ++clItr){
      
      float dPhi = P4Helpers::deltaPhi( (*clItr)->phi(),  eg->caloCluster()->phi() );
      float dEta = (*clItr)->eta()-eg->caloCluster()->eta();
      float dr2=dPhi*dPhi+ dEta*dEta;
      
      if (dr2 < max_cone) {
	clusts.push_back (*clItr);
      }
    }
    
    // Calculate isolation for topo cluster 
    CHECK( topoClustCones (result,eg->caloCluster(), clusts) );
    // now correct the isolation energy for the core cluster energy
    if (strategy ==  CaloIsolationTool::SubtractionStrategy::Eeg57)
      CHECK( correctIsolationEnergy_Eeg57(result,eg->caloCluster())); 
    else
    {
      ATH_MSG_ERROR("Unrecognised energy subtraction strategy");
      return false;
    }
    
    
    return true;
  }
  




  /**
   * @brief Calculate isolation cones in topo clusters around @a eg.
   */
  bool CaloIsolationTool::topoClustCones (CaloIsolation& result, const xAOD::CaloCluster* eg, const std::vector<const xAOD::CaloCluster*>& clusts) 
  {
    
    ATH_MSG_DEBUG("In egammaTopoIso::topoClustCones");
    
    if (eg == 0 || eg->pt() <= 0 || fabs(eg->eta()) > 7.0) {
      ATH_MSG_DEBUG("Cluster potentially malformed");
      return true;
    }
    
    BOOST_FOREACH (const xAOD::CaloCluster* cl, clusts) {
      float et = (m_useEMScale ? cl->p4(xAOD::CaloCluster::State::UNCALIBRATED).Et() : cl->pt() );
      if(et <= 0 || fabs(cl->eta()) > 7.0) continue;

      float st = 1./cosh(cl->p4(xAOD::CaloCluster::State::UNCALIBRATED).Eta());
      float tilegap3_et = cl->eSample(CaloSampling::TileGap3)*st;
      et -= tilegap3_et;
      
      float dPhi = P4Helpers::deltaPhi( cl->phi(),  eg->phi() );
      float dEta = cl->eta()-eg->eta();
      float dr2=dPhi*dPhi+ dEta*dEta;
      
      for (unsigned int i = 0; i < m_coneSizes.size(); i++) {
	if (dr2 < m_coneSizes2[i]) {
	  result.etcones[i] += et;
	}
      }
    }
    
    return true;
}





  bool CaloIsolationTool::correctIsolationEnergy_Eeg57 (CaloIsolation& result, const xAOD::CaloCluster* cleg) 
  {
    // now correct the isolation energy for the core cluster energy
    float eraw57=0., eta=0.;
    if(cleg && cleg->getCellLinks()){
      xAOD::CaloCluster* egcCloneFor57 = CaloClusterStoreHelper::makeCluster(cleg->getCellLinks()->getCellContainer(),
									     cleg->eta0(),cleg->phi0(),
									     cleg->clusterSize()
									     );
      if (!m_caloFillRectangularToolPtr) {
	ATH_MSG_WARNING("CaloFillRectangularCluster is not available");
	return false;
      }
      m_caloFillRectangularToolPtr->makeCorrection(egcCloneFor57);
      eraw57   = egcCloneFor57->e();
      eta = cleg->eta(); //FillRectangularCluster doesn't recalculated the overall custer eta (only per-sampling)
      delete egcCloneFor57;
    }
    result.ecore = eraw57/cosh(eta);
    for (unsigned int i = 0; i < m_coneSizes.size(); i++){
      result.etcones[i] -= result.ecore;
    }
    return true;
  }

}	// end of namespace

