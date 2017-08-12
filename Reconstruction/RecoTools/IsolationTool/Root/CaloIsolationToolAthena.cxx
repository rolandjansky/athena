/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#include "IsolationTool/CaloIsolationTool.h"

#include "xAODPrimitives/IsolationConeSize.h"
#include "xAODPrimitives/IsolationFlavour.h"
#include "xAODPrimitives/IsolationCorrection.h"
#include "xAODPrimitives/IsolationHelpers.h"
#include "xAODPrimitives/tools/getIsolationAccessor.h"

#include "CaloEvent/CaloCell.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellList.h"

#include "TrackToCalo/CaloCellSelectorLayerdR.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkParameters/TrackParameters.h"

namespace xAOD {

  // IParticle interface for cell-based isolation (etcone)
  bool CaloIsolationTool::caloCellIsolation(CaloIsolation& result, const IParticle& particle, 
					    const std::vector<Iso::IsolationType>& cones, 
					    CaloCorrection corrlist, 
					    const CaloCellContainer* container) const {
    derefMap_t derefMap;
    /// get track particle
    const IParticle* ip = getReferenceParticle(particle);
    if( !ip ){
      ATH_MSG_WARNING("Failed to obtain reference particle");
      return false;
    }
    derefMap[ip] = &particle;

    double coneCoreSize = m_coneCoreSizeEg;
    if (particle.type() == Type::Muon)
      coneCoreSize = m_coneCoreSizeMu;
    
    // muon etcone isolation
    const TrackParticle* trkp = dynamic_cast<const TrackParticle*>(ip);
    if( trkp ) return caloCellIsolation(result,*trkp,cones,corrlist,container,coneCoreSize, derefMap);
    
    // egamma etcone isolation
    const Egamma* egam = dynamic_cast<const Egamma*>(ip);
    if( egam ) return caloCellIsolation(result,*egam,cones,corrlist,container);
    
    ATH_MSG_WARNING("CaloCellIsolation only supported for TrackParticles and Egamma");
    
    return true;
  }

  bool CaloIsolationTool::caloCellIsolation( CaloIsolation& result, const TrackParticle& tp, 
					     const std::vector<Iso::IsolationType>& isoTypes, CaloCorrection corrlist, 
					     const CaloCellContainer* container, double coneCoreSize, const derefMap_t& derefMap) const {
    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }

    unsigned int typesize = isoTypes.size();
    initresult(result, corrlist, typesize);
    
    // get cones
    Iso::IsolationFlavour theFlavour = Iso::isolationFlavour(isoTypes.front());
    
    if (theFlavour == Iso::etcone){
      return etConeIsolation(result, tp, isoTypes, container, coneCoreSize, derefMap);
    }

    ATH_MSG_WARNING("Unsupported isolation flavour passed, cannot calculate isolation " << 
		    static_cast<int>(theFlavour));
    
    return false;
  }

  // casted interface for EGamma cell-based isolation (etcone)
  bool CaloIsolationTool::caloCellIsolation( CaloIsolation& result, const Egamma& eg, 
					     const std::vector<Iso::IsolationType>& isoTypes, 
					     CaloCorrection corrlist, const CaloCellContainer* container) const {
    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }

    unsigned int typesize = isoTypes.size();
    initresult(result, corrlist, typesize);
    
    std::vector<float> coneSizes;
    coneSizes.resize(isoTypes.size());
    for (unsigned int is = 0; is < isoTypes.size(); is++)
      coneSizes[is] = Iso::coneSize(isoTypes[is]);

    // get cones
    Iso::IsolationFlavour theFlavour = Iso::isolationFlavour(isoTypes.front());
    
    if (theFlavour == Iso::etcone){
      etConeIsolation(result, eg, isoTypes, container);
    }else{
      ATH_MSG_WARNING("Unsupported isolation flavour passed, cannot calculate isolation " << static_cast<int>(theFlavour));
      return false;
    }
    
    // Apply corrections
    // core energy subtraction
    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::core57cells))) {
      if (!correctIsolationEnergy_Eeg57(result,isoTypes,&eg))
	ATH_MSG_WARNING("Could not compute core cell energy for egamma in etcone");
    }

    // leakage correction
    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::ptCorrection))) {
      if (!PtCorrection(result, eg, isoTypes)) 
	ATH_MSG_WARNING("Could not apply pt correction to etcone isolation");
    }
    
    return true;
  }

  // etcone implementation for TrackParticle
  bool CaloIsolationTool::etConeIsolation( CaloIsolation& result, const TrackParticle& tp,
					   const std::vector<Iso::IsolationType>& isoTypes, 
					   const CaloCellContainer* container,
					   double coneCoreSize,
					   const derefMap_t& derefMap) const {

    // for now always use track, should move to extrapolation to calo entrance 
    const Trk::Track* track = tp.track();
    if( !track  ) {
      ATH_MSG_WARNING("Failed to access track");
      return false;
    }

    std::vector<double> conesf;
    double maxConeSize = -1;
    for( auto isoType : isoTypes ){
      if( Iso::isolationFlavour(isoType) != Iso::etcone ) {
	ATH_MSG_WARNING("Unsupported isolation type passed, cannot calculate isolation " << static_cast<int>(isoType));
	return false;
      }
      double cone = Iso::coneSize(isoType);
      conesf.push_back(cone);
      if(cone>maxConeSize) maxConeSize = cone;
    }

    /// add coreCone if asked -- make sure it's the last one! or a better
    const double coreConeDR = coneCoreSize;
    bool doCoreCone = (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone)));
    if(doCoreCone && maxConeSize<coreConeDR) maxConeSize = coreConeDR;

    /// start the calculation 
    ATH_MSG_DEBUG("calculating etcone for # " << conesf.size() << " cones");
    const Rec::ParticleCellAssociation* association = 0;
    if( !m_assoTool->particleCellAssociation(tp,association,maxConeSize,container) ){
      ATH_MSG_DEBUG("failed to obtain the ParticleCellAssociation");
      return false;
    }

    Trk::CaloCellSelectorLayerdR selector(maxConeSize);
    selector.preSelectAction(association->caloExtension());
    ATH_MSG_DEBUG("looping over cells " << association->data().size());
    for(unsigned int i=0; i<conesf.size(); i++){
      double totE = 0.;
      selector.setConeSize(conesf[i]);
      for (auto aCell : association->data()){
	if( !selector.select(*aCell) ) continue;
	if (m_ExcludeTG3 && CaloCell_ID::TileGap3 == aCell->caloDDE()->getSampling()) continue;
	totE += aCell->et();
      }
      result.etcones[i] = totE;
      ATH_MSG_DEBUG("etcone raw: coneSize = " << conesf[i] << "; etcone = " << result.etcones[i]);
    }
    ATH_MSG_DEBUG("done looping over cells ");

    /// do coreCone
    if(doCoreCone){
      ATH_MSG_DEBUG("starting etcone, coreCone");
      double totE = 0.;
      selector.setConeSize(coreConeDR);
      for (auto aCell : association->data()){
	if( !selector.select(*aCell) ) continue;
	if (m_ExcludeTG3 && CaloCell_ID::TileGap3 == aCell->caloDDE()->getSampling()) continue;
	totE += aCell->et();
      }
      std::map<Iso::IsolationCorrectionParameter,float> corecorr;
      corecorr[Iso::coreEnergy] = totE;
      corecorr[Iso::coreArea]   = coreConeDR*coreConeDR*M_PI;
      result.coreCorrections[Iso::coreCone] = corecorr;
      ATH_MSG_DEBUG("done etcone, coreCone");

      /// apply the correction if required.
      if(result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone))){
	double ecore = totE;
	ATH_MSG_DEBUG("Applying coreCone correction for trackParticle etcone isolation.");
	for( unsigned int i=0;i<result.etcones.size();++i ) {
	  result.etcones[i] -= ecore;
	  ATH_MSG_DEBUG("i: " << i << " cone [before] " << result.etcones[i]+ecore << " cone [after] " << result.etcones[i]);
	}
      }
    }

    // calculate etcore 
    if(!m_saveOnlyRequestedCorrections ||
       result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreMuon))){
      correctIsolationEnergy_MuonCore(result, tp, derefMap);
    }
  
    return true;
  }

  // etcone implementation for Egamma
  bool CaloIsolationTool::etConeIsolation( CaloIsolation& result, const Egamma& eg,
					   const std::vector<Iso::IsolationType>& isoTypes, 
					   const CaloCellContainer* container )  const {
    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }
    
    std::vector<float> coneSizes;        coneSizes.resize(3);
    std::vector<float> coneSizesSquared; coneSizesSquared.resize(3);
    for (unsigned int i = 0; i < isoTypes.size(); i++) {
      float dR = Iso::coneSize(isoTypes.at(i));
      coneSizes[i] = dR;
      coneSizesSquared[i] = dR*dR;
    }

    float phi = eg.caloCluster()->phi();
    float eta = eg.caloCluster()->eta();
        
    // Define a new Calo Cell list corresponding to EM Calo
    std::vector<CaloCell_ID::SUBCALO> Vec_EMCaloEnums;
    for (unsigned int n=0; n < m_EMCaloNums.size(); ++n)
      Vec_EMCaloEnums.push_back(static_cast<CaloCell_ID::SUBCALO>( m_EMCaloNums[n] ));
    CaloCellList* EMccl = new CaloCellList(container, Vec_EMCaloEnums);
   
    std::vector<CaloCell_ID::SUBCALO> Vec_HadCaloEnums;
    for (unsigned int n=0; n < m_HadCaloNums.size(); ++n) 
      Vec_HadCaloEnums.push_back(static_cast<CaloCell_ID::SUBCALO>( m_HadCaloNums[n] ));
    CaloCellList* HADccl = new CaloCellList(container, Vec_HadCaloEnums);
    
    // Let's determine some values based on the input specs
    // Search for largest radius
    double Rmax = 0.0;
    for (unsigned int n=0; n< coneSizes.size(); n++)
      if (coneSizes[n] > Rmax) Rmax = coneSizes[n];
        
    if (EMccl) {
      // get the cells for the first one; by convention, it must be bigger than all the other cones.
      EMccl->select(eta,phi,Rmax);

      for (CaloCellList::list_iterator it = EMccl->begin(); it != EMccl->end(); ++it) {
	double etacel=(*it)->eta();
	double phicel=(*it)->phi();
        
	double deleta = eta-etacel;
	float delphi = Phi_mpi_pi(phi-phicel);
	double drcel2 = (deleta*deleta) + (delphi*delphi);
        
	for (unsigned int i = 0; i < coneSizes.size(); i++) {
	  if (drcel2 < coneSizesSquared[i])
	    result.etcones[i] += (*it)->et();
	}
      }
    }
    
    if (HADccl) {
      // get the cells for the first one; by convention, it must be bigger than all the other cones.
      HADccl->select(eta, phi, Rmax);

      for (CaloCellList::list_iterator it = HADccl->begin(); it != HADccl->end(); ++it) {
	// Optionally remove TileGap cells
	if (m_ExcludeTG3 && CaloCell_ID::TileGap3 == (*it)->caloDDE()->getSampling())
	  {
	    ATH_MSG_DEBUG("Excluding cell with Et = " << (*it)->et());
	    continue;
	  }
	
	// if no TileGap cells excluded, log energy of all cells
	double etacel = (*it)->eta();
	double phicel = (*it)->phi();
        
	double deleta = eta-etacel;
	float delphi = Phi_mpi_pi(phi-phicel);
	double drcel2 = (deleta*deleta) + (delphi*delphi);

	for (unsigned int i = 0; i < coneSizes.size(); i++) {
	  if (drcel2 < coneSizesSquared[i]) 
	    result.etcones[i] += (*it)->et();
	}
      }
    }

    delete EMccl;
    delete HADccl;
    return true;
  }

  bool CaloIsolationTool::decorateParticle_caloCellIso( IParticle& tp,
							const std::vector<Iso::IsolationType>& cones,
							CaloCorrection corrections,
							const CaloCellContainer* Cells){
    // calculate the isolation
    CaloIsolation result;
    if( !caloCellIsolation(result,tp,cones,corrections,Cells) ) {
      ATH_MSG_DEBUG("Calculation of caloCellIsolation failed");
      return false;
    }
    if( !decorateParticle(result, tp, cones, corrections) ){
      ATH_MSG_DEBUG("Decoration of caloCellIsolation failed");
      return false;
    }

    return true;
  }

  bool CaloIsolationTool::decorateParticle( IParticle& tp,
					    const std::vector<Iso::IsolationType>& cones,
					    CaloCorrection corrections,
					    const CaloCellContainer* Cells,
					    const CaloClusterContainer* TopClusters){
    ATH_MSG_DEBUG("just a test ");
    /// check Isolation flavour
    Iso::IsolationFlavour flavour = Iso::numIsolationFlavours;
    for( unsigned int i=0;i<cones.size();++i ){
      if(i==0) {flavour = isolationFlavour( cones[i] ); continue;}
      if(flavour != isolationFlavour( cones[i] )){
	ATH_MSG_FATAL("decorateParticle does not support mixture of the isolation flavour!!!");
	return false;
      }
    }
    
    bool suc(false);
    switch(flavour) {
    case Iso::etcone:
      suc = decorateParticle_caloCellIso(tp, cones, corrections, Cells); break;
    case Iso::topoetcone:
      suc = decorateParticle_topoClusterIso(tp, cones, corrections, TopClusters); break;
    case Iso::neflowisol:
      suc = decorateParticle_eflowIso(tp, cones, corrections); break;
    default:
      ATH_MSG_FATAL("Unsupported isolation flavour!!!"); return false;
    }
    
    return suc;    
  }
}

#endif //XAOD_ANALYSIS
