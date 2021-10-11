

#include "GaudiKernel/StatusCode.h"
#include "AthLinks/ElementLink.h"
#include "T2CaloEgammaForwardReFastAlgo.h"
#include "TrigT2CaloCommon/IReAlgToolCalo.h"
#include "CxxUtils/phihelper.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"

class ISvcLocator;

T2CaloEgammaForwardReFastAlgo::T2CaloEgammaForwardReFastAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
    AthReentrantAlgorithm(name, pSvcLocator)
{}




StatusCode T2CaloEgammaForwardReFastAlgo::initialize()
{
  m_emAlgTools.retrieve().ignore();
  ATH_CHECK(m_clusterContainerKey.initialize());
  ATH_CHECK(m_roiCollectionKey.initialize());
  ATH_CHECK( m_bcidAvgKey.initialize() );
  if (! m_monTool.empty() ) ATH_CHECK( m_monTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode T2CaloEgammaForwardReFastAlgo::execute(const EventContext& context) const
{



  SG::WriteHandle<xAOD::TrigEMClusterContainer> trigEmClusterCollection(m_clusterContainerKey, context);
  ATH_CHECK( trigEmClusterCollection.record(std::make_unique<xAOD::TrigEMClusterContainer>(),
                                            std::make_unique<xAOD::TrigEMClusterAuxContainer>()) );


  auto roisHandle = SG::makeHandle(m_roiCollectionKey, context);

  if (!roisHandle.isValid()) {
    ATH_MSG_DEBUG("no RoI");
    return StatusCode::SUCCESS;
  }

  trigEmClusterCollection->reserve(roisHandle->size());
  

  ATH_MSG_DEBUG("RoI descriptor size is " << roisHandle->size() );

  for (const TrigRoiDescriptor* roiDescriptor : *roisHandle) 
  {
    float etaL1, phiL1;
    double etamin, etamax, phimin, phimax;

    ATH_MSG_INFO( "RoI eta = " << roiDescriptor->eta() << " RoI phi = " << roiDescriptor->phi());

      
    etamin = std::max( -3.2 , roiDescriptor->eta() - m_etaWidth);
    etamax = std::min(  3.2 , roiDescriptor->eta() + m_etaWidth);

    phimin = CxxUtils::wrapToPi(roiDescriptor->phi() - m_phiWidth);
    phimax = CxxUtils::wrapToPi(roiDescriptor->phi() + m_phiWidth);

    etaL1 = roiDescriptor->eta();
    phiL1 = roiDescriptor->phi();

    TrigRoiDescriptor newroi(roiDescriptor->eta(), etamin, etamax,
                             roiDescriptor->phi(), phimin, phimax);


    ATH_MSG_DEBUG(" etamin = " << etamin << " etamax = " << etamax <<
                  " phimin = " << phimin << " phimax = " << phimax);

    xAOD::TrigEMCluster* ptrigEmCluster = new xAOD::TrigEMCluster();
    trigEmClusterCollection->push_back(ptrigEmCluster);
    ptrigEmCluster->setEnergy(0.0);
    ptrigEmCluster->setEt(0.0);
    ptrigEmCluster->setRawEnergy(0.0);
    ptrigEmCluster->setRawEt(0.0);
    ptrigEmCluster->setE277(0);
    ptrigEmCluster->setEmaxs1(0);
    ptrigEmCluster->setE2tsts1(0);
    ptrigEmCluster->setEhad1(-999);
    ptrigEmCluster->setWeta2(-999);
    ptrigEmCluster->setFracs1(-999);
    ptrigEmCluster->setE233(-999);
    ptrigEmCluster->setE237(-999);
    ptrigEmCluster->setWstot(-999);
    ptrigEmCluster->setEta1(-999);
    ptrigEmCluster->setNCells(0);
    ptrigEmCluster->setRawEta(-999);
    ptrigEmCluster->setRawPhi(-999);

    // It is a good idea to clear the energies
    for (int i = 0; i < CaloSampling::CaloSample::MINIFCAL0; i++) {
      ptrigEmCluster->setEnergy((CaloSampling::CaloSample)i, 0.);
      ptrigEmCluster->setRawEnergy((CaloSampling::CaloSample)i, 0.);
    }
    // Initial cluster position is the LVL1 position
    ptrigEmCluster->setEta(etaL1);
    ptrigEmCluster->setPhi(phiL1);


    ptrigEmCluster->setRoIword(roiDescriptor->roiWord());
    const CaloDetDescrElement* caloDDE = nullptr;

    //uint32_t error = 0;
    for (const auto& tool : m_emAlgTools) {
      ATH_CHECK( tool->execute(*ptrigEmCluster, newroi, caloDDE, context) );
    }
 
  
  

  } // end of roiCollection iterator

  return StatusCode::SUCCESS;
}

