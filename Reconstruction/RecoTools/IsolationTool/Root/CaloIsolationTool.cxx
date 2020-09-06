/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// CaloIsolationTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "IsolationTool/CaloIsolationTool.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "AsgDataHandles/ReadHandle.h"

#ifndef XAOD_ANALYSIS
#include "CaloEvent/CaloCell.h"
#include "TrackToCalo/CaloCellSelectorLayerdR.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellList.h"
#include "GaudiKernel/ThreadLocalContext.h"
#endif // XAOD_ANALYSIS

// #include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "xAODPrimitives/IsolationConeSize.h"
#include "xAODPrimitives/IsolationFlavour.h"
#include "xAODPrimitives/IsolationCorrection.h"
#include "xAODPrimitives/IsolationHelpers.h"
#include "xAODPrimitives/IsolationCorrectionHelper.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaDefs.h"
#include "xAODMuon/Muon.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "FourMomUtils/xAODP4Helpers.h"

#include "boost/format.hpp"
#include <cmath>
#include <map>


namespace {
#if defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS)
size_t cluster_size (const xAOD::CaloCluster* ) { return 0; }
#else
size_t cluster_size (const xAOD::CaloCluster* cl) { return cl->size(); }
#endif
}


namespace xAOD {

  CaloIsolationTool::CaloIsolationTool (const std::string& name):
        asg::AsgTool(name)
  {
#ifndef XAOD_ANALYSIS
    declareInterface<ICaloCellIsolationTool>(this);
    declareInterface<ICaloTopoClusterIsolationTool>(this);
    declareInterface<INeutralEFlowIsolationTool>(this);
#endif // XAOD_ANALYSIS
  }

  CaloIsolationTool::~CaloIsolationTool() { }

  StatusCode CaloIsolationTool::initialize() {
#ifndef XAOD_ANALYSIS
    if (!m_assoTool.empty())
      ATH_CHECK(m_assoTool.retrieve());

    if (!m_caloExtTool.empty())
      ATH_CHECK(m_caloExtTool.retrieve());

    if (!m_clustersInConeTool.empty())
      ATH_CHECK(m_clustersInConeTool.retrieve());

    if (!m_pflowObjectsInConeTool.empty())
      ATH_CHECK(m_pflowObjectsInConeTool.retrieve());

    if (!m_caloFillRectangularTool.empty()) {
      ATH_CHECK( m_caloFillRectangularTool.retrieve());
    }

    //check calo number specified for EM Calos
    unsigned int nSubCalo=static_cast<int>(CaloCell_ID::NSUBCALO);
    if (m_EMCaloNums.size()>nSubCalo || m_HadCaloNums.size()>nSubCalo) {
      ATH_MSG_ERROR(" More than " << nSubCalo << " calo specified. Required for EM =" << m_EMCaloNums.size()<< ", and for HCAL = " << m_HadCaloNums.size() << ". Must be wrong. Stop.");
      return StatusCode::FAILURE;
    }

    for (unsigned int index=0; index < m_EMCaloNums.size() ; ++index)  {
      if (m_EMCaloNums[index]<0 || m_EMCaloNums[index]>=(int)nSubCalo ) {
        ATH_MSG_ERROR("Invalid calo specification:" << m_EMCaloNums[index] << "Stop.");
        return StatusCode::FAILURE;
      } else
      	ATH_MSG_DEBUG("Select calorimeter " << m_EMCaloNums[index]);
    }

    for (unsigned int index=0; index < m_HadCaloNums.size() ; ++index) {
      if (m_HadCaloNums[index]<0 || m_HadCaloNums[index]>=(int)nSubCalo) {
        ATH_MSG_ERROR("Invalid calo specification:" << m_HadCaloNums[index] << "Stop.");
        return StatusCode::FAILURE;
      } else
      	ATH_MSG_DEBUG("Select calorimeter " << m_HadCaloNums[index]);
    }
#endif // XAOD_ANALYSIS

    if (!m_IsoLeakCorrectionTool.empty())
      ATH_CHECK(m_IsoLeakCorrectionTool.retrieve());

    // initialize the read handles (for now do all of them in all cases)
    ATH_CHECK(m_tpEDCentral.initialize());
    ATH_CHECK(m_tpEDForward.initialize());
    ATH_CHECK(m_tpEDveryForward.initialize());
    ATH_CHECK(m_efEDCentral.initialize());
    ATH_CHECK(m_efEDForward.initialize());

    // Exit function
    return StatusCode::SUCCESS;
  }


  StatusCode CaloIsolationTool::finalize() {
    return StatusCode::SUCCESS;
  }


  // IParticle interface for cell-based isolation (etcone)
 bool CaloIsolationTool::caloCellIsolation(CaloIsolation& result, const IParticle& particle,
            const std::vector<Iso::IsolationType>& cones,
            CaloCorrection corrlist,
            const CaloCellContainer* container) const {
#ifdef XAOD_ANALYSIS
   (void) result;
   (void) particle;
   (void) cones;
   (void) corrlist;
   (void) container;
    return false;
#else // XAOD_ANALYSIS
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
#endif // not XAOD_ANALYSIS
  }

  // IParticle interface for cluster-based isolation (topoetcone)
  bool CaloIsolationTool::caloTopoClusterIsolation(CaloIsolation& result, const IParticle& particle,
						   const std::vector<Iso::IsolationType>& cones,
						   CaloCorrection corrlist,
						   const CaloClusterContainer* container  ) const {

    if ( cones.empty() ) {
      ATH_MSG_DEBUG("No isolation required");
      return false;
    }

    double coneCoreSize = m_coneCoreSizeEg;
    if (particle.type() == Type::Muon)
      coneCoreSize = m_coneCoreSizeMu;

    derefMap_t derefMap;
    /// get track particle
    const IParticle* ip = getReferenceParticle(particle);
    if( !ip ){
      ATH_MSG_WARNING("Failed to obtain reference particle");
      return false;
    }
    derefMap[ip] = &particle;

    // muon topoetcone isolation
    const TrackParticle* trkp = dynamic_cast<const TrackParticle*>(ip);
    if( trkp ) return caloTopoClusterIsolation(result,*trkp,cones,corrlist,container,nullptr,nullptr,coneCoreSize,derefMap);

    // egamma topoetcone isolation
    const Egamma* egam = dynamic_cast<const Egamma*>(ip);
    if( egam ) return caloTopoClusterIsolation(result,*egam,cones,corrlist,container, coneCoreSize);

    ATH_MSG_WARNING("CaloTopoClusterIsolation only supported for TrackParticles and Egamma");

    return true;
  }


  // interface for pflow based isolation
  bool CaloIsolationTool::neutralEflowIsolation(CaloIsolation& result, const IParticle& particle,
						const std::vector<Iso::IsolationType>& cones,
						CaloCorrection corrlist) const{

    //
    double coneCoreSize = m_coneCoreSizeEg;
    if (particle.type() == Type::Muon)
      coneCoreSize = m_coneCoreSizeMu;

    derefMap_t derefMap;
    /// get track particle
    const IParticle* ip = getReferenceParticle(particle);
    if( !ip ){
      ATH_MSG_WARNING("Failed to obtain reference particle");
      return false;
    }
    derefMap[ip] = &particle;

    // muon pflowetcone isolation
    const TrackParticle* trkp = dynamic_cast<const TrackParticle*>(ip);
    if ( trkp ) return neutralEflowIsolation(result,*trkp,cones,corrlist,coneCoreSize,derefMap);

    // egamma pflowetcone isolation
    const Egamma* egam = dynamic_cast<const Egamma*>(ip);
    if ( egam ) return neutralEflowIsolation(result,*egam,cones,corrlist,coneCoreSize);

    ATH_MSG_WARNING("PFlowObjectIsolation only supported for Egamma and TrackParticle");

    return true;
  }


  // casted interface for TrackParticle cell-based isolation (etcone)
  bool CaloIsolationTool::caloCellIsolation( CaloIsolation& result,
#ifndef XAOD_ANALYSIS
  const TrackParticle& tp,
#endif
	const std::vector<Iso::IsolationType>& isoTypes, CaloCorrection corrlist
#ifndef XAOD_ANALYSIS
	, const CaloCellContainer* container
        , double coneCoreSize
        , const derefMap_t& derefMap
#endif
	) const {
    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }

    unsigned int typesize = isoTypes.size();
    initresult(result, corrlist, typesize);

    // get cones
    Iso::IsolationFlavour theFlavour = Iso::isolationFlavour(isoTypes.front());

    if (theFlavour == Iso::etcone)
#ifndef XAOD_ANALYSIS
      return etConeIsolation(result, tp, isoTypes, container, coneCoreSize, derefMap);
#else
      return true;
#endif

    ATH_MSG_WARNING("Unsupported isolation flavour passed, cannot calculate isolation " <<
		    static_cast<int>(theFlavour));

    return false;
  }

  // casted interface for EGamma cell-based isolation (etcone)
  bool CaloIsolationTool::caloCellIsolation( CaloIsolation& result, const Egamma& eg,
					     const std::vector<Iso::IsolationType>& isoTypes,
					     CaloCorrection corrlist
#ifndef XAOD_ANALYSIS
					     , const CaloCellContainer* container
#endif
                        ) const {
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
#ifndef XAOD_ANALYSIS
      etConeIsolation(result, eg, isoTypes, container);
#else
      return true;
#endif
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

  // casted interface for EGamma cluster-based isolation (topoetcone)
  bool CaloIsolationTool::caloTopoClusterIsolation(CaloIsolation& result,
						   const Egamma& eg,
						   const std::vector<Iso::IsolationType>& isoTypes,
						   CaloCorrection corrlist,
						   const CaloClusterContainer* container,
                                                   double coneCoreSize) const
  {

    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }

    unsigned int typesize = isoTypes.size();
    initresult(result, corrlist, typesize);

    std::vector<float> coneSizes;
    for( auto isoType : isoTypes ){
      coneSizes.push_back(Iso::coneSize(isoType));
    }

    float phi = eg.caloCluster()->phi();
    float eta = eg.caloCluster()->eta();

    // JB
    const CaloCluster* fwdClus = eg.author(xAOD::EgammaParameters::AuthorFwdElectron) ? eg.caloCluster() : nullptr;

    if (!topoConeIsolation(result, eta, phi, coneSizes, true, container, fwdClus, &eg, coneCoreSize)) {
      ATH_MSG_WARNING("topoConeIsolation failed for egamma");
      return false;
    }

    // 5x7 : no meaning for fwd electron
    if (fwdClus == nullptr && (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::core57cells)))) {
      // Apply core energy subtraction
      if (!correctIsolationEnergy_Eeg57(result,isoTypes,&eg))
	ATH_MSG_WARNING("Could not compute core cell energy for egamma in topoetcone");
    }

    // leakage : at least for the time being, no meaning for fwd electron
    if ((!m_saveOnlyRequestedCorrections && fwdClus == nullptr) || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::ptCorrection))) {
      // do pt correction
      if (!PtCorrection(result, eg, isoTypes))
        ATH_MSG_WARNING("Could not apply pt correction to topoetcone isolation");
    }
    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::pileupCorrection))) {
      // do pile-up correction
      if (!EDCorrection(result,isoTypes,eta,"topo",fwdClus))
	ATH_MSG_WARNING("Could not apply ED correction to topo isolation");
    }

    return true;

  }

  // casted interface for EGamma pflow-based isolation
  bool CaloIsolationTool::neutralEflowIsolation(CaloIsolation& result,
						const Egamma& eg,
						const std::vector<Iso::IsolationType>& isoTypes,
						CaloCorrection corrlist,
            double coneCoreSize) const
  {

    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }

    unsigned int typesize = isoTypes.size();
    initresult(result, corrlist, typesize);

    std::vector<float> coneSizes;
    for( auto isoType : isoTypes ){
      coneSizes.push_back(Iso::coneSize(isoType));
    }

    float phi = eg.caloCluster()->phi();
    float eta = eg.caloCluster()->eta();

    if (!pflowConeIsolation(result, eta, phi, coneSizes, true, nullptr, coneCoreSize)) {
      ATH_MSG_WARNING("pflowConeIsolation failed");
      return false;
    }

    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::core57cells))) {
      // Apply core energy subtraction
      if (!correctIsolationEnergy_Eeg57(result,isoTypes,&eg))
	ATH_MSG_WARNING("Could not compute core cell energy for egamma in neflowisol");
    }

    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::ptCorrection))) {
      // do pt correction
      if (!PtCorrection(result, eg, isoTypes))
	ATH_MSG_WARNING("Could not apply pt correction to isolation");
    }

    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::pileupCorrection))) {
      // do pile-up correction
      std::string type = "PFlow";
      if (!EDCorrection(result,isoTypes,eta,type,nullptr))
	ATH_MSG_WARNING("Could not apply ED correction to topo isolation");
    }

    return true;
  }

  // casted interface for TP pflow-based isolation
  bool
  CaloIsolationTool::neutralEflowIsolation(
    CaloIsolation& result,
    const TrackParticle& tp,
    const std::vector<Iso::IsolationType>& isoTypes,
    CaloCorrection corrlist,
    double coneCoreSize,
    derefMap_t& derefMap) const
  {

    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }

    unsigned int typesize = isoTypes.size();
    initresult(result, corrlist, typesize);

    std::vector<float> coneSizes;
    for( auto isoType : isoTypes ){
      coneSizes.push_back(Iso::coneSize(isoType));
    }

    float phi = tp.phi();
    float eta = tp.eta();
    if(!GetExtrapEtaPhi(&tp,eta,phi,derefMap)) ATH_MSG_WARNING("TrackParticle eta = " << tp.eta() << ", phi = " << tp.phi() << " not updated from extraplation!");
    ATH_MSG_DEBUG("TrackParticle eta = " << tp.eta() << ", phi = " << tp.phi() << ", extrap eta = " << eta << ", phi = " << phi);

    // The core subtraction with pflow removal is done in the method below
    if (!pflowConeIsolation(result, eta, phi, coneSizes, false, nullptr, coneCoreSize)) {
      ATH_MSG_WARNING("pflowConeIsolation failed for muon");
      return false;
    }

    // core energy subtraction
    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreMuon))) {
      if (!correctIsolationEnergy_MuonCore(result, tp, derefMap))
	ATH_MSG_WARNING("Could not compute muon core energy (cells) from neflowisol");
    }

    // do pile-up correction
    std::string type = "PFlow";
    if (!EDCorrection(result,isoTypes,eta,type,nullptr))
      ATH_MSG_WARNING("Could not apply ED correction to eflow isolation for muon");

    return true;
  }

  bool CaloIsolationTool::GetExtrapEtaPhi(const TrackParticle* tp, float& eta, float& phi,
                                          derefMap_t& derefMap) const
  {
    /// get it from decoration
    static const SG::AuxElement::ConstAccessor< char > Decorated("caloExt_Decorated");
    static const SG::AuxElement::ConstAccessor< float > Eta("caloExt_eta");
    static const SG::AuxElement::ConstAccessor< float > Phi("caloExt_phi");
    if(Decorated.isAvailable(*tp) && Decorated(*tp)){
      eta = Eta(*tp);
      phi = Phi(*tp);
      return true;
    }

    /// get it from calo-cluster if it's muon
    const Muon* mu = dynamic_cast<const Muon*>(derefMap[tp]);
    if(mu){
      auto cluster = mu->cluster();
      if(cluster){
        float etaT = 0, phiT = 0, dphiT = 0.;
        int nSample = 0;
        for(unsigned int i=0; i<CaloSampling::Unknown; i++) // dangerous?
        {
          auto s = static_cast<CaloSampling::CaloSample>(i);
          if(!cluster->hasSampling(s)) continue;
          ATH_MSG_DEBUG("Sampling: " << i << "eta-phi (" << cluster->etaSample(s) << ", " << cluster->phiSample(s) << ")");
          etaT += cluster->etaSample(s);
          if( nSample == 0 )
            phiT = cluster->phiSample(s);
          else
            dphiT += xAOD::P4Helpers::deltaPhi( cluster->phiSample(s), phiT ) ;
          nSample++;
        }
        if(nSample>0){
          eta = etaT/nSample;
          phi = phiT + dphiT/nSample;

          if(m_addCaloDeco && !Decorated.isAvailable(*tp)) decorateTrackCaloPosition(*tp, eta, phi);
          return true;
        }else{
          ATH_MSG_WARNING("Muon calo cluster is empty????");
        }
      }else{
        ATH_MSG_DEBUG("Muon calo cluster not found.");
      }
    }

#ifndef XAOD_ANALYSIS
    /// try the extention in athena if it's not obtained from muon yet.
    ATH_MSG_DEBUG("Geting calo extension caloExtension tool.");
    std::unique_ptr<Trk::CaloExtension> caloExtension = m_caloExtTool->caloExtension(*tp);
    if(!caloExtension){
      ATH_MSG_WARNING("Can not get caloExtension.");
      return false;
    };

    const std::vector<const Trk::CurvilinearParameters*>& intersections = caloExtension->caloLayerIntersections();
    if(!intersections.empty()){
      Amg::Vector3D avePoint(0,0,0);
      for (unsigned int i = 0; i < intersections.size(); ++i){
        const Amg::Vector3D& point = intersections[i]->position();
        ATH_MSG_DEBUG("Intersection: " << i << " ID: " << m_parsIdHelper.caloSample(intersections[i]->cIdentifier())
                      <<  " eta-phi (" << point.eta() << ", " << point.phi() << ")");
        avePoint += point;
      }
      avePoint = (1./intersections.size())*avePoint;
      eta = avePoint.eta();
      phi = avePoint.phi();

      //JBdVtestif(m_addCaloDeco) decorateTrackCaloPosition(*tp, eta, phi);
      if(m_addCaloDeco && !Decorated.isAvailable(*tp)) decorateTrackCaloPosition(*tp, eta, phi);
      return true;
    }else{
        ATH_MSG_WARNING("Muon Calo extension got no intersection!!!");
    }
#endif // xAOD

    /// if still not got the updated eta & phi
    ATH_MSG_WARNING("Calo extension can not be obtained!!!");
    return false;
  }

  // casted interface for TrackParticle cluster-based isolation (topoetcone)
  bool CaloIsolationTool::caloTopoClusterIsolation(CaloIsolation& result,
						   const TrackParticle& tp,
						   const std::vector<Iso::IsolationType>& isoTypes,
						   CaloCorrection corrlist,
						   const CaloClusterContainer* container,
                                                   const CaloCluster* fwdClus,
                                                   const Egamma* egObj,
                                                   double coneCoreSize,
                                                   derefMap_t& derefMap) const {
    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }

    unsigned int typesize = isoTypes.size();
    initresult(result, corrlist, typesize);
    std::vector<float> coneSizes;
    for( auto isoType : isoTypes ){
      coneSizes.push_back(Iso::coneSize(isoType));
    }

    float phi = tp.phi();
    float eta = tp.eta();
    if(!GetExtrapEtaPhi(&tp,eta,phi,derefMap)) ATH_MSG_WARNING("TrackParticle eta = " << tp.eta() << ", phi = " << tp.phi() << " not updated from extraplation!");
    ATH_MSG_DEBUG("TrackParticle eta = " << tp.eta() << ", phi = " << tp.phi() << ", extrap eta = " << eta << ", phi = " << phi);

    // get energy in cones
    if (!topoConeIsolation(result, eta, phi, coneSizes, false, container, fwdClus, egObj, coneCoreSize)) {
      ATH_MSG_WARNING("Could not compute topo isolation for muons");
      return false;
    }

    // core energy subtraction
    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreMuon))) {
      if (!correctIsolationEnergy_MuonCore(result, tp, derefMap))
	ATH_MSG_WARNING("Could not compute muon core energy (cells) from topoetcone");
    }

    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::pileupCorrection))) {
      if (!EDCorrection(result,isoTypes,eta,"topo",fwdClus))
	ATH_MSG_WARNING("Could not apply ED correction to topo isolation");
    }

    return true;
  }

#ifndef XAOD_ANALYSIS
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
    std::unique_ptr<const Rec::ParticleCellAssociation> association=m_assoTool->particleCellAssociation(tp,maxConeSize,container);
    if( !association) {
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
#endif

#ifndef XAOD_ANALYSIS
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
    for (unsigned int n=0; n < m_EMCaloNums.size(); ++n) {
      Vec_EMCaloEnums.push_back(static_cast<CaloCell_ID::SUBCALO>( m_EMCaloNums[n] ));
    }
    CaloCellList EMccl(container, Vec_EMCaloEnums);

    std::vector<CaloCell_ID::SUBCALO> Vec_HadCaloEnums;
    for (unsigned int n=0; n < m_HadCaloNums.size(); ++n) {
      Vec_HadCaloEnums.push_back(static_cast<CaloCell_ID::SUBCALO>( m_HadCaloNums[n] ));
    }
    CaloCellList HADccl(container, Vec_HadCaloEnums);

    // Let's determine some values based on the input specs
    // Search for largest radius
    double Rmax = 0.0;
    for (unsigned int n=0; n< coneSizes.size(); n++)
      if (coneSizes[n] > Rmax) Rmax = coneSizes[n];

    // get the cells for the first one; by convention, it must be bigger than all the other cones.
    EMccl.select(eta,phi,Rmax);

    for (auto it: EMccl) {
      double etacel=it->eta();
      double phicel=it->phi();

      double deleta = eta-etacel;
      float delphi = Phi_mpi_pi(phi-phicel);
      double drcel2 = (deleta*deleta) + (delphi*delphi);

      for (unsigned int i = 0; i < coneSizes.size(); i++) {
	if (drcel2 < coneSizesSquared[i])
	  result.etcones[i] += it->et();
      }
    }//end loop over cell-list


    // get the cells for the first one; by convention, it must be bigger than all the other cones.
    HADccl.select(eta, phi, Rmax);

    for (auto it: HADccl) {
      // Optionally remove TileGap cells
      if (m_ExcludeTG3 && CaloCell_ID::TileGap3 == it->caloDDE()->getSampling()) {
	ATH_MSG_DEBUG("Excluding cell with Et = " << it->et());
	continue;
      }

      // if no TileGap cells excluded, log energy of all cells
      double etacel = it->eta();
      double phicel = it->phi();

      double deleta = eta-etacel;
      float delphi = Phi_mpi_pi(phi-phicel);
      double drcel2 = (deleta*deleta) + (delphi*delphi);

      for (unsigned int i = 0; i < coneSizes.size(); i++) {
	if (drcel2 < coneSizesSquared[i])
	  result.etcones[i] += it->et();
      }
    }//end loop over cell-list

    return true;
  }
#endif

  bool CaloIsolationTool::topoConeIsolation(CaloIsolation& result, float eta, float phi,
					    std::vector<float>& coneSizes,
                                            bool coreEMonly,
					    const CaloClusterContainer* container,
                                            const CaloCluster* fwdClus,
                                            const Egamma* egObj,
                                            double coneCoreSize) const
  {

    // offline topocluster container is large: preselect only those in max cone size
    auto max_cone_iter=std::max_element(coneSizes.begin(), coneSizes.end());
    float max_cone= (*max_cone_iter);
    std::vector<const CaloCluster*> clusts;
    if(!container){
#ifndef XAOD_ANALYSIS
      if(m_clustersInConeTool){
	m_clustersInConeTool->particlesInCone(eta,phi,max_cone,clusts);
      }else{
	ATH_MSG_WARNING("No CaloClustersInConeTool available");
      }
#else
      if(!particlesInCone(eta,phi,max_cone,clusts))
        ATH_MSG_WARNING("No CaloClustersInConeTool available");
#endif
    }else{ // trigger container is small enough
      auto clItr  = container->begin();
      auto clItrE = container->end();
      for(; clItr != clItrE; ++clItr){
	clusts.push_back (*clItr);
      }
    }

    // Calculate isolation for topo cluster
    if (!topoClustCones (result,eta,phi, coneSizes, clusts)) {
      ATH_MSG_WARNING("Could not compute topo isolation");
      return false;
    }

    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone))) {
      // Subtract core (easier to do that here than outside like other core corrections)
      if (!correctIsolationEnergy_TopoCore(result, eta, phi, -1, -1, coneCoreSize*coneCoreSize, clusts, coreEMonly, fwdClus, egObj))
	ATH_MSG_WARNING("Could not compute topo core");
    }

    return true;
  }

  bool
  CaloIsolationTool::pflowConeIsolation(CaloIsolation& result,
                                        float eta,
                                        float phi,
                                        std::vector<float>& coneSizes,
                                        bool coreEMonly,
                                        const PFOContainer* container,
                                        double coneCoreSize) const
  {

    // container is large: preselect only those in max cone size
    auto max_cone_iter=std::max_element(coneSizes.begin(), coneSizes.end());
    float max_cone = (*max_cone_iter);
    std::vector<const PFO*> clusts;
    if (!container) {
#ifndef XAOD_ANALYSIS
      if (m_pflowObjectsInConeTool) {
	m_pflowObjectsInConeTool->particlesInCone(eta,phi,max_cone,clusts);
      } else {
	ATH_MSG_WARNING("No PFlowObjectsInConeTool available");
      }
#else
      if(!particlesInCone(eta,phi,max_cone,clusts)) ATH_MSG_WARNING("Failed to get particles in cone.");
#endif
    } else { // trigger container is small enough
      auto clItr  = container->begin();
      auto clItrE = container->end();
      for(; clItr != clItrE; ++clItr){
	clusts.push_back (*clItr);
      }
    }

    // Calculate isolation for pflow objects
    if (!pflowObjCones (result,eta,phi,coneSizes,clusts)) {
      ATH_MSG_WARNING("Could not compute pflow isolation");
      return false;
    }

    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone))) {
      // Core subtraction
      if (!correctIsolationEnergy_pflowCore(result, eta, phi, -1, -1, coneCoreSize*coneCoreSize, clusts, coreEMonly))  // be careful, require a certain tag of eflowRec, below which eSample are not always filled
	ATH_MSG_WARNING("Could not compure pflow core");
    }

    return true;
  }

  /**
   * @brief Calculate isolation cones in topo clusters around @a eg.
   */
  bool CaloIsolationTool::topoClustCones (CaloIsolation& result, float eta, float phi,
					  std::vector<float>& coneSizes,
					  const std::vector<const CaloCluster*>& clusts) const
  {

    ATH_MSG_DEBUG("In CaloIsolationTool::topoClustCones");

    for (const CaloCluster* cl : clusts) {
      float et = (m_useEMScale ? cl->p4(CaloCluster::State::UNCALIBRATED).Et() : cl->pt() );
      if(et <= 0 || fabs(cl->eta()) > 7.0) continue;

      float st = 1./cosh(cl->p4(CaloCluster::State::UNCALIBRATED).Eta());
      float tilegap3_et = cl->eSample(CaloSampling::TileGap3)*st;
      et -= tilegap3_et;

      float dPhi = Phi_mpi_pi(cl->phi()-phi);
      float dEta = cl->eta()-eta;
      float dr=sqrt(dPhi*dPhi+ dEta*dEta);

      for (unsigned int i = 0; i < coneSizes.size(); i++) {
	if (dr < coneSizes[i]) {
	  ATH_MSG_DEBUG("Adding topo " << cl << " dR = " << dr << " et of a topo clust et = " << et << " (calibrated " << cl->pt() << ", tilegap et = " << tilegap3_et << ")");
	  result.etcones[i] += et;
	}
      }
    }

    return true;
  }

  /**
   * @brief Calculate isolation cones in pflow objects around @a eg.
   */
  bool CaloIsolationTool::pflowObjCones (CaloIsolation& result, float eta, float phi,
					 std::vector<float>& coneSizes,
					 const std::vector<const PFO*>& clusts) const
  {

    ATH_MSG_DEBUG("In pflowObjCones obj eta = " << eta << " phi = " << phi);

    for (const PFO* cl : clusts) {
      float et = cl->pt();
      if (m_useEMScale)
	et = cl->ptEM();
      if (et <= 0 || fabs(cl->eta()) > 7.0) continue;

      float dPhi = Phi_mpi_pi(cl->phi()-phi);
      float dEta = cl->eta()-eta;
      float dR   = sqrt(dPhi*dPhi+ dEta*dEta);

      for (unsigned int i = 0; i < coneSizes.size(); i++) {
	if (dR < coneSizes[i]) {
	  result.etcones[i] += et;
	}
      }
    }

    return true;
  }


  bool CaloIsolationTool::correctIsolationEnergy_Eeg57 (CaloIsolation& result,
							const std::vector<Iso::IsolationType>& isoTypes,
							const Egamma* eg)  const
  {

    float coreV = 0;
    bool gotIso = eg->isolationCaloCorrection(coreV,Iso::etcone, Iso::core57cells, Iso::coreEnergy);
    if (gotIso)
      ATH_MSG_DEBUG("core57cells available = " << coreV);
    else
      ATH_MSG_DEBUG("core57cells not available");
    if ((gotIso && fabs(coreV) < 1e-3) || !gotIso) {
#ifndef XAOD_ANALYSIS
      const CaloCluster *cleg = eg->caloCluster();
      // now correct the isolation energy for the core cluster energy
      float eraw57=0., eta=0.;

      if(cleg && cleg->getCellLinks()){
	double seedEta = cleg->eta0(), seedPhi = cleg->phi0();
	CaloCluster* egcCloneFor57 = CaloClusterStoreHelper::makeCluster(cleg->getCellLinks()->getCellContainer(),
									 seedEta,seedPhi,
									 cleg->clusterSize());

	if (!m_caloFillRectangularTool->execute (Gaudi::Hive::currentContext(),
                                                 egcCloneFor57).isSuccess())
        {
          return false;
	}
	if(egcCloneFor57->size()==0){
	  ATH_MSG_WARNING("Size of Created Cluster is 0 aka no cells");
	  return false;
	}

	eraw57 = egcCloneFor57->e();
	eta    = cleg->eta(); //FillRectangularCluster doesn't recalculated the overall cluster eta (only per-sampling)
	coreV  = eraw57/cosh(eta);
	ATH_MSG_DEBUG("Number of cells in 5x7 " << egcCloneFor57->size()
		      << " seed eta,phi " << cleg->eta0() << " " << cleg->phi0()
		      << " eraw = " << eraw57 << " etraw = " << coreV
		      );
	delete egcCloneFor57;
     }
#else
     return false;
#endif // XAOD_ANALYSIS
    }

    std::map<Iso::IsolationCorrectionParameter,float> corecorr;
    corecorr[Iso::coreEnergy] = coreV;
    corecorr[Iso::coreArea]   = 5*0.025*7*TMath::Pi()/128;
    result.coreCorrections[Iso::core57cells] = corecorr;
    if (result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::core57cells))) {
      for (unsigned int i = 0; i < isoTypes.size(); i++){
	result.etcones[i] -= coreV;
      }
    }

    return true;
  }


  bool CaloIsolationTool::correctIsolationEnergy_TopoCore(CaloIsolation& result, float eta, float phi,
					  float dEtaMax_core, float dPhiMax_core, float dR2Max_core,
                                                          const std::vector<const CaloCluster*>& clusts, bool onlyEM,
                                                          const CaloCluster* fwdClus,
                                                          const Egamma* egObj) const
  {
    ATH_MSG_DEBUG("In CaloIsolationTool::correctIsolationEnergy_TopoCore");
    ATH_MSG_DEBUG("particle: eta " << eta << " phi " << phi);


    // I do not remember why I put areacore = 0 for fwdClus !!
    float areacore = -999.;
    if (fwdClus == nullptr) {
      if (dEtaMax_core>0 && dPhiMax_core>0) areacore = 4*dEtaMax_core*dPhiMax_core;
      else if (dR2Max_core>0) areacore = M_PI*dR2Max_core;
    } else
      areacore = 0;

    double topoCore(0.);
    if (fwdClus) {
      topoCore = fwdClus->p4(CaloCluster::State::UNCALIBRATED).Et();
      ATH_MSG_DEBUG("Including " << topoCore << " in the core transverse energy of the fwd electron");
    } else {
      for (const CaloCluster* cl : clusts) {
	ATH_MSG_DEBUG("cl: eta " << cl->eta() << " phi " << cl->phi()
		      << " E " << cl->p4(CaloCluster::State::UNCALIBRATED).E()
		      << " pt " << cl->p4(CaloCluster::State::UNCALIBRATED).Et()
		      << " cal E " << cl->e());
	/// check distance
	float dPhi = Phi_mpi_pi(cl->phi()-phi);
	if(dEtaMax_core>0 && fabs(dPhi) > dPhiMax_core) continue;
	float dEta = cl->eta()-eta;
	if(dPhiMax_core>0 && fabs(dEta) > dEtaMax_core) continue;
	if(dR2Max_core>0 && dPhi*dPhi+dEta*dEta > dR2Max_core) continue;
	ATH_MSG_DEBUG("dist: dPhi " << dPhi << " dEta " << dEta << " dR2 " << dPhi*dPhi+dEta*dEta);

	/// get enenrgy
	float et = (m_useEMScale ? cl->p4(CaloCluster::State::UNCALIBRATED).Et() : cl->pt() );
	if(et <= 0 || fabs(cl->eta()) > 7.0) continue;

	/// remove TileGap3
	double ettg3 = cl->eSample(CaloSampling::TileGap3)/cosh(cl->p4(CaloCluster::State::UNCALIBRATED).Eta());
	et -= ettg3;
	if (fabs(ettg3) > 1)
	  ATH_MSG_DEBUG("After TG3 removal, pt = " << et);

	/// if only EM
	double emfrac = 1.;
	if(onlyEM){
	  double eEM = cl->energyBE(0)+cl->energyBE(1)+cl->energyBE(2)+cl->energyBE(3);
	  emfrac     = std::min(1., eEM / cl->p4(CaloCluster::State::UNCALIBRATED).E());
	}
	et *= emfrac;

	/// add to the core
	topoCore += et;
	ATH_MSG_DEBUG("adding in core et: " << et << " (em frac = " << emfrac << " dR = " << sqrt(dPhi*dPhi+dEta*dEta) << ") total " << topoCore);
      }
    }

    double topoCoreSC(0.);
    double test         = 0;
    double topoCoreSCem = 0;
    if (egObj && egObj->caloCluster()->clusterSize() == CaloCluster::ClusterSize::SuperCluster) {
      const std::vector<const CaloCluster*> assocClus = EgammaHelpers::getAssociatedTopoClusters(egObj->caloCluster());
      for (unsigned int ic = 0; ic < assocClus.size(); ic++) {
	const CaloCluster* cl = assocClus.at(ic);
	test       += cl->pt();
	topoCoreSC += cl->p4(CaloCluster::State::UNCALIBRATED).Et();
	ATH_MSG_DEBUG("Adding topo " << ic << " ptr = " << cl << " contrib, pt = " << cl->pt()
		      << " uncal pt = " << cl->p4(CaloCluster::State::UNCALIBRATED).Et()
		      << " eta,phi = " << cl->eta() << " " << cl->phi()
		      << " uncal eta,phi = " << cl->p4(CaloCluster::State::UNCALIBRATED).Eta()
		      << " " << cl->p4(CaloCluster::State::UNCALIBRATED).Phi()
		      << " nCells = " << cluster_size(cl)
                      );

	/// remove TileGap3
	double ettg3 = cl->eSample(CaloSampling::TileGap3)/cosh(cl->p4(CaloCluster::State::UNCALIBRATED).Eta());
	topoCoreSCem -= ettg3;

	/// if only EM
	double emfrac = 1.;
	if(onlyEM){
	  double eEM = cl->energyBE(0)+cl->energyBE(1)+cl->energyBE(2)+cl->energyBE(3);
	  emfrac     = std::min(1., eEM / cl->p4(CaloCluster::State::UNCALIBRATED).E());
	}
	topoCoreSCem += emfrac*cl->p4(CaloCluster::State::UNCALIBRATED).Et();

	//auto itc = assocClus.at(ic)->begin();
	//for (; itc != assocClus.at(ic)->end(); itc++)
	//  std::cout << "A cell " << (*itc) << " eta = " << (*itc)->eta() << std::endl;
      }
      ATH_MSG_DEBUG("Including " << topoCore << " only em, no tg3 " << topoCoreSCem << " calibrated " << test << " in the core transverse energy of the super cluster egamma object made of "
		    << assocClus.size() << " clusters with corresponding pT = " << egObj->pt()
		    << " cluster pT = " << egObj->caloCluster()->pt() << " cluster un cal pT = "
		    << egObj->caloCluster()->p4(CaloCluster::State::UNCALIBRATED).Et()
		    << " nCells = " << cluster_size(egObj->caloCluster())
                    );
      //auto itc = egObj->caloCluster()->begin();
      //for (; itc != egObj->caloCluster()->end(); itc++)
      //std::cout << "A cell in the SC " << (*itc) << " eta = " << (*itc)->eta() << std::endl;

      std::map<Iso::IsolationCorrectionParameter,float> corecorrSC;
      corecorrSC[Iso::coreEnergy] = topoCoreSCem;
      corecorrSC[Iso::coreArea]   = areacore;
      result.coreCorrections[Iso::coreConeSC] = corecorrSC;

    }

    ATH_MSG_DEBUG("core energy, std = " << topoCore << " topo-assoc (full) = " << topoCoreSC << " topo-assoc (only em, no tg3) = " << topoCoreSCem);

    std::map<Iso::IsolationCorrectionParameter,float> corecorr;
    corecorr[Iso::coreEnergy] = topoCore;
    corecorr[Iso::coreArea]   = areacore;
    result.coreCorrections[Iso::coreCone] = corecorr;

    float toSub = 0;
    bool  doSub = false;
    if (result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone)) ||
	result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreConeSC))) {
      doSub = true;
      if (result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone)) &&
	  !result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreConeSC)))
	toSub = topoCore;
      else if (!result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone)) &&
	       result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreConeSC)))
	toSub = topoCoreSCem;
      else {
	ATH_MSG_WARNING("Cannot do two core subtraction ! Using coreCone");
	toSub = topoCore;
      }
    }

    if (doSub) {
      for (unsigned int i = 0; i < result.etcones.size(); i++){
	result.etcones[i] -= toSub;
	ATH_MSG_DEBUG("TopoCore correction i=" << i << " cone [before] " << result.etcones[i]+toSub << " cone [after] " << result.etcones[i]);
      }
    }

    return true;
  }

bool CaloIsolationTool::correctIsolationEnergy_pflowCore(CaloIsolation& result, float eta, float phi,
							 float detaMax, float dphiMax, float dR2Max,
							 const std::vector<const PFO*>& clusts, bool onlyEM) const
  {

    float pflowCore(0.);
    for (const PFO* cl : clusts) {
      ATH_MSG_DEBUG("pflo: eta " << cl->eta() << " phi " << cl->phi() << " pt " << cl->pt() << " ptEM = " << cl->ptEM()
		    << " charge " << cl->charge());
      float dphi = Phi_mpi_pi(cl->phi()-phi);
      if (detaMax > 0 && fabs(dphi) > dphiMax) continue;
      float deta = cl->eta()-eta;
      if (dphiMax > 0 && fabs(deta) > detaMax) continue;
      if (dR2Max > 0 && dphi*dphi+deta*deta > dR2Max) continue;

      /// get energy
      float et = cl->pt();
      if (m_useEMScale)
	et = cl->ptEM();
      if (et <= 0 || fabs(cl->eta()) > 7.0) continue;

      double emfrac = 1.;
      if (onlyEM) {
	const xAOD::CaloCluster *ocl = cl->cluster(0);
	if(ocl) {
	  double eEM = ocl->energyBE(0)+ocl->energyBE(1)+ocl->energyBE(2)+ocl->energyBE(3);
	  emfrac     = std::min(1.,eEM / cl->eEM());
	}
      }
      et *= emfrac;

      /// add to the core
      pflowCore += et;
      ATH_MSG_DEBUG("pflow to be added to core: et = " << et << " (em frac = " << emfrac << "), total = " << pflowCore);
    }

    /// set results
    float areacore = -999.;
    if      (detaMax > 0 && dphiMax > 0) areacore = 4*detaMax*dphiMax;
    else if (dR2Max > 0)                 areacore = M_PI*dR2Max;

    std::map<Iso::IsolationCorrectionParameter,float> corecorr;
    corecorr[Iso::coreEnergy] = pflowCore;
    corecorr[Iso::coreArea]   = areacore;
    result.coreCorrections[Iso::coreCone] = corecorr;

    if (result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone))) {
      for (unsigned int i = 0; i < result.etcones.size(); i++) {
	ATH_MSG_DEBUG("pflow, cone type " << i << ", in cone " << result.etcones[i] << ", subtracted " << result.etcones[i]-pflowCore);
	result.etcones[i] -= pflowCore;
      }
    }

    return true;
  }


  // etcone implementation for TrackParticle
  bool CaloIsolationTool::correctIsolationEnergy_MuonCore( CaloIsolation& result, const
                                                           TrackParticle& tp,
                                                           const derefMap_t& derefMap) const {

    ATH_MSG_DEBUG("in  CaloIsolationTool::correctIsolationEnergy_MuonCore ");

    // initialize varialbes
    double ecore = 0.;
    auto muI = derefMap.find(&tp);
    if(muI!=derefMap.end() && muI->second->isAvailable<float>("ET_Core")){
      ecore = muI->second->auxdataConst<float>("ET_Core");
    }else{
     ATH_MSG_WARNING("ET_Core of muon not found! coreMuon isolation correction will not be applied!!!");
     return false;
    }

    std::map<Iso::IsolationCorrectionParameter,float> corecorr;
    corecorr[Iso::coreEnergy] = ecore;
    corecorr[Iso::coreArea]   = 0;
    result.coreCorrections[Iso::coreMuon] = corecorr;

    if (result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreMuon))) {
      for( unsigned int i=0;i<result.etcones.size();++i ) {
	result.etcones[i] -= ecore;
	ATH_MSG_DEBUG("MuonCore i=" << i << " cone [before] " << result.etcones[i]+ecore << " cone [after] " << result.etcones[i]);
      }
    }

    return true;
  }

  bool CaloIsolationTool::PtCorrection (CaloIsolation& result,
					const Egamma& eg,
					const std::vector<Iso::IsolationType>& isoTypes) const
  {
// #ifndef XAOD_ANALYSIS
    if(m_IsoLeakCorrectionTool.empty()) return false;
// #endif // XAOD_ANALYSIS

    std::vector<float> corrvec;
    corrvec.resize(isoTypes.size(),0.);
    for (unsigned int i = 0; i < isoTypes.size(); i++) {

// #ifndef XAOD_ANALYSIS
      corrvec[i] = m_IsoLeakCorrectionTool->GetPtCorrection(eg, isoTypes[i]);
// #else
//       corrvec[i] = eg.isolationCaloCorrection (isoTypes[i], Iso::ptCorrection);
// #endif // XAOD_ANALYSIS
      if (result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::ptCorrection))) {
	result.etcones[i] -= corrvec[i];
	ATH_MSG_DEBUG("eta = " << eg.eta() << ", phi = " << eg.phi() << ", pt = " << eg.pt() << ", isoType = " << Iso::toCString(isoTypes[i])
		      << ", ptcorr = " << corrvec[i] << ", isol pt corrected = " << result.etcones[i] );
      }
    }
    result.noncoreCorrections[Iso::ptCorrection] = corrvec;
    /// FIX LATER
    // get the correction from xAOD file

    return true;
  }

  bool CaloIsolationTool::EDCorrection(CaloIsolation& result,
				       const std::vector<Iso::IsolationType>& isoTypes,
				       float eta, // In principle, could be (eta,phi)
				       const std::string& type,
                                       const CaloCluster* fwdClus) const

  {
    // assume two densities for the time being
    const SG::ReadHandleKey<EventShape>* esKey = (fabs(eta) < 1.5) ? &m_tpEDCentral : &m_tpEDForward;
    if (type == "PFlow") {
      esKey = (fabs(eta) < 1.5) ? &m_efEDCentral : &m_efEDForward;
    } else if (fwdClus != nullptr) {
      esKey = &m_tpEDveryForward;
    }

    SG::ReadHandle<EventShape> edShape(*esKey);
    // check is only used for serial running; remove when MT scheduler used
    if(!edShape.isValid()) {
      ATH_MSG_FATAL("Failed to retrieve "<< esKey->key());
      return false;
    }

    double rho = 0;
    bool gotDensity = edShape->getDensity(EventShape::Density,rho);
    if (!gotDensity) {
      ATH_MSG_WARNING("Cannot retrieve density " << esKey->key() << " for isolation correction. No ED correction");
      return false;
    }

    // Get the core size
    float areacore = 0;
    std::map<xAOD::Iso::IsolationCorrectionParameter,float> ecore;
    if(result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreMuon))){
      ecore = result.coreCorrections.find(Iso::coreMuon)->second;
    }else if(result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::core57cells))){
      ecore = result.coreCorrections.find(Iso::core57cells)->second;
    }else if(result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone))){
      ecore = result.coreCorrections.find(Iso::coreCone)->second;
    }

    auto iter = ecore.find(xAOD::Iso::coreArea);
    if (iter != ecore.end())
      areacore = ecore.find(xAOD::Iso::coreArea)->second;

    std::vector<float> corrvec;
    corrvec.resize(isoTypes.size(),0.);
    for (unsigned int i = 0; i < isoTypes.size(); i++) {
      float dR    = Iso::coneSize(isoTypes.at(i));
      float toSub = rho*(dR*dR*M_PI - areacore);
      corrvec[i] = toSub;
      if (result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::pileupCorrection))){
	result.etcones[i] -= toSub;
      }
      ATH_MSG_DEBUG("ED correction ("<< type << ")for size " << dR << " = " << toSub << " (areacore=" << areacore << ")");
    }

    result.noncoreCorrections[Iso::pileupCorrection] = corrvec;
    return true;
  }

  void CaloIsolationTool::initresult(CaloIsolation& result,
				     CaloCorrection corrlist,
				     unsigned int typesize) const {

    result.corrlist = corrlist;
    result.coreCorrections.clear();
    result.noncoreCorrections.clear();
    result.etcones.resize(typesize,0.);

  }

  const IParticle* CaloIsolationTool::getReferenceParticle(const IParticle& particle) const {
    const TrackParticle* tp = dynamic_cast<const TrackParticle*>(&particle);
    if( tp ) return tp;
    const Muon* muon = dynamic_cast<const Muon*>(&particle);
    if( muon ) {
      ATH_MSG_DEBUG("muon with author "<<muon->author()<<" and pT "<<muon->pt());
      const TrackParticle* tp = nullptr;
      //note: if STACO, the track particle has no Trk::Track associated, so use the ID track
      if(muon->primaryTrackParticleLink().isValid() && muon->author()!=2) tp = *muon->primaryTrackParticleLink();
      if( !tp) tp = *muon->inDetTrackParticleLink();
      if( !tp ) {
        ATH_MSG_WARNING(" No TrackParticle found for muon " );
        return nullptr;
      }
      return tp;
    }
    return &particle;
  }


#ifdef XAOD_ANALYSIS // particlesInCone tool will not be avaible. Write our own...
  bool CaloIsolationTool::particlesInCone( float eta, float phi, float dr, std::vector<const CaloCluster*>& output ) const {
    /// retrieve container
    const CaloClusterContainer* caloClusters = 0;
    std::string m_caloClusterLocation = "CaloCalTopoClusters";
    if(evtStore()->retrieve(caloClusters, m_caloClusterLocation).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_caloClusterLocation );
      return false;
    }

    const float m_minPt = 1e-3;
    /// make selection
    float dr2 = dr*dr;
    for(auto trk: *caloClusters){
      if(trk->pt()<m_minPt) continue;

      float dEta = eta-trk->eta();
      float dPhi = Phi_mpi_pi(phi-trk->phi());
      if(dr2<(dEta*dEta+dPhi*dPhi)) continue;
      output.push_back(trk);
    }

    return true;
  }
  bool CaloIsolationTool::particlesInCone( float eta, float phi, float dr, std::vector<const PFO*>& output ){
    /// retrieve container
    const PFOContainer* Clusters = 0;
    std::string m_ClusterLocation = "JetETMissNeutralParticleFlowObjects";
    if(evtStore()->retrieve(Clusters,m_ClusterLocation).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_ClusterLocation);
      return false;
    }

    const float m_minPt = 1e-3;
    /// make selection
    float dr2 = dr*dr;
    for(auto trk: *Clusters){
      if(trk->pt()<m_minPt) continue;
      float dEta = eta-trk->eta();
      float dPhi = Phi_mpi_pi(phi-trk->phi());
      if(dr2<(dEta*dEta+dPhi*dPhi))continue;
      output.push_back(trk);
    }

    return true;
  }
#endif // XAOD_ANALYSIS
  // FIXME! This should be updated to use the standard caching of extrapolation to calo
  void CaloIsolationTool::decorateTrackCaloPosition(const IParticle& p, float eta, float phi) const{
    static const SG::AuxElement::Decorator< char > dec_Decorated("caloExt_Decorated");
    static const SG::AuxElement::Decorator< float > dec_Eta("caloExt_eta");
    static const SG::AuxElement::Decorator< float > dec_Phi("caloExt_phi");

    if(!dec_Decorated.isAvailable(p) || dec_Decorated.isAvailableWritable(p)){
      dec_Decorated(p) = 1;
      dec_Eta(p) = eta;
      dec_Phi(p) = phi;
    }else{
      ATH_MSG_WARNING("The writing of caloExt decoration was requested, but couldn't be accomplished");
    }
  }

}	// end of namespace

