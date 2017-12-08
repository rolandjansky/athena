/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2CaloTau.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloTau
// 
// AUTHOR:   M.P. Casado
//           S.R. Armstrong
// updates: 3/3/11 ccuenca, added new vars for monitoring
// 
// - Add variables for job option controlled region limits, set defaults
//   to most likely values.
// - Add function EtaPhiRange to return the maximum and minimum eta or phi
//   values to use when calculating energy sums over a region  - R. Soluk
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"                               

#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEvent/TrigTauClusterDetails.h"
#include "TrigCaloEvent/TrigTauClusterDetailsContainer.h"

#include "TrigT2CaloCommon/T2CaloBase.h"
#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "TrigT2CaloCommon/phiutils.h"
#include "TrigT2CaloTau/T2CaloTau.h"

//#include <TH1F.h>
#include "AthenaKernel/errorcheck.h"

#define NEG_ENERGY_CLUSTER HLT::Reason::USERDEF_1
#define NULL_ENERGY_SAMPLING HLT::Reason::USERDEF_2

class ISvcLocator;

T2CaloTau::T2CaloTau(const std::string & name, ISvcLocator* pSvcLocator) : T2CaloBase(name, pSvcLocator),
    m_storeCells(false),
    m_updateRoiDescriptor(false),
    m_phiWidthEM(0.4), 
    m_etaWidthEM(0.4)
{

    //properties
    declareProperty("TrigTauClusterKey",   m_trigTauClusterKey = "T2CaloTrigTauCluster");

    declareProperty("StoreCells",          m_storeCells,"store cells in container attached to RoI");
    declareProperty("updateRoiDescriptor", m_updateRoiDescriptor, "option to update RoiDescriptor after execution");
    declareProperty("PhiWidthEM",          m_phiWidthEM, "phi width EM calo");
    declareProperty("EtaWidthEM",          m_etaWidthEM, "eta width EM calo");

    //Monitored variables
    declareMonitoredVariable("EMRadius", m_EMRadius);
    declareMonitoredVariable("EMRadius3S", m_EMRadius3S);
    declareMonitoredVariable("CaloRadius", m_CaloRadius);
    declareMonitoredVariable("HadRad", m_HadRad);
    
    declareMonitoredVariable("IsoFrac", m_IsoFrac);
    declareMonitoredVariable("StripWidth", m_StripWidth);
    
    declareMonitoredVariable("EMFraction", m_EMFraction);
    
    //Wide, cone 0.3 (Wide in TrigTauClusterDetails)
    declareMonitoredVariable("EtRawWide", m_EtRawWide);
    
    //Medium, cone 0.2 (Medium in TrigTauClusterDetails)
    declareMonitoredVariable("EMEnMedium", m_EMEnMedium);
    declareMonitoredVariable("HADEnMedium", m_HADEnMedium);
    
    //Narrow, cone 0.1 (Narrow in TrigTauClusterDetails)
    declareMonitoredVariable("EMEnNarrow", m_EMEnNarrow);
    declareMonitoredVariable("HADEnNarrow", m_HADEnNarrow);
    
    declareMonitoredVariable("EtRawMedium",     m_EtRawMedium);
    declareMonitoredVariable("EtRawMediumEM0",  m_EtRawMediumEM0);
    declareMonitoredVariable("EtRawMediumEM1",  m_EtRawMediumEM1);
    declareMonitoredVariable("EtRawMediumEM2",  m_EtRawMediumEM2);
    declareMonitoredVariable("EtRawMediumEM3",  m_EtRawMediumEM3);
    declareMonitoredVariable("EtRawMediumHad0", m_EtRawMediumHad0);
    declareMonitoredVariable("EtRawMediumHad1", m_EtRawMediumHad1);
    declareMonitoredVariable("EtRawMediumHad2", m_EtRawMediumHad2);


    //EtNarrow/EtWide ratio
    declareMonitoredVariable("CoreFraction",     m_CoreFraction);

    declareMonitoredVariable("EtaL1",            m_EtaL1);
    declareMonitoredVariable("PhiL1",            m_PhiL1);
    declareMonitoredVariable("Eta",              m_Eta);
    declareMonitoredVariable("Phi",              m_Phi);
    declareMonitoredVariable("dEtaL2Tau_RoI",    m_dEtaL2Tau_RoI);
    declareMonitoredVariable("dPhiL2Tau_RoI",    m_dPhiL2Tau_RoI);

    declareMonitoredVariable("ConversionErrors", m_conversionError);
    declareMonitoredVariable("AlgorithmErrors",  m_algorithmError);

    declareMonitoredStdContainer("Quality", m_quality);

    m_Container = 0;

}

T2CaloTau::~T2CaloTau()
{
}

HLT::ErrorCode T2CaloTau::hltInitialize()
{
    //  MsgStream log(msgSvc(), name());

    //  IToolSvc* toolSvc;
    //  service("ToolSvc",toolSvc);

    ToolHandleArray<IAlgToolCalo>::iterator it = m_emAlgTools.begin();
    for(; it != m_emAlgTools.end(); ++it)
    {
        StatusCode sc = it->retrieve();
        if(sc.isFailure())
        {
            msg() << MSG::ERROR << "Unable to initialize tool " << *it << endmsg;
            return HLT::BAD_ALGO_CONFIG ;
        }
        if(msgLvl() <= MSG::DEBUG)
        {
            msg() << MSG::DEBUG << "REGTEST: Created " << *it << " AlgTool" << endmsg;
        }
        (*it)->setCellContainerPointer(&m_Container);
    }

    if(msgLvl() <= MSG::DEBUG)
    {
        if(m_updateRoiDescriptor)
        {
            msg() << MSG::DEBUG << "REGTEST: TrigRoiDescriptor will be updated " << endmsg;
        }
        else
        {
            msg() << MSG::DEBUG << "REGTEST: TrigRoiDescriptor will NOT be updated " << endmsg;
        }
    }

    if(m_storeCells && msgLvl() <= MSG::DEBUG)
    {
        msg() << MSG::DEBUG << "REGTEST: will store cells in output " << endmsg;
    }

    return HLT::OK;
}

HLT::ErrorCode T2CaloTau::hltFinalize()
{
    if(msgLvl() <= MSG::DEBUG)
    {
        msg() << MSG::DEBUG << " hltFinalize is done" << endmsg;
    }
    return HLT::OK;
}

HLT::ErrorCode T2CaloTau::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE)
{
    // Time total T2Calo execution time.
    if(timerSvc()) m_timer[0]->start();      

    m_conversionError = 0;
    m_algorithmError = 0;
    // reset quality vector for monitoring
    m_quality.clear();

    m_EMRadius = -99.0;
    m_EMRadius3S = -99.0;
    m_CaloRadius = -99.0;
    m_HadRad = -99.0;

    m_IsoFrac = -99.0;
    m_StripWidth = -99.0;

    m_EMFraction = -99.0;

    m_EtRawWide = -99.0;

    m_EMEnMedium = -99.0;
    m_HADEnMedium = -99.0;

    m_EMEnNarrow = -99.0;
    m_HADEnNarrow = -99.0;

    m_EtRawMedium = -99.0;
    m_EtRawMediumEM0 = -99.0;
    m_EtRawMediumEM1 = -99.0;
    m_EtRawMediumEM2 = -99.0;
    m_EtRawMediumEM3 = -99.0;
    m_EtRawMediumHad0 = -99.0;
    m_EtRawMediumHad1 = -99.0;
    m_EtRawMediumHad2 = -99.0;

    m_CoreFraction = -99.0;

    m_EtaL1 = -99.0;
    m_PhiL1 = -99.0;
    m_Eta = -99.0;
    m_Phi = -99.0;
    m_dEtaL2Tau_RoI = -99.0;
    m_dPhiL2Tau_RoI = -99.0;  


#ifndef NDEBUG
    if(msgLvl() <= MSG::DEBUG)
    {
        msg() << MSG::INFO << "in execute()" << endmsg;
    }
#endif

    // Some debug output:
#ifndef NDEBUG
    if(msgLvl() <= MSG::DEBUG)
    {
        msg() << MSG::DEBUG << "output TE ID: " << outputTE->getId() << endmsg;
    }
#endif

    // Some debug output:
#ifndef NDEBUG
    if(msgLvl() <= MSG::DEBUG)
    {
        msg() << MSG::DEBUG << "input TE ID: " << inputTE->getId() << endmsg;
    }
#endif
    const TrigRoiDescriptor* roiDescriptor = 0;
    HLT::ErrorCode st = getFeature(inputTE, roiDescriptor);  

    if(st == HLT::OK && roiDescriptor)
    {
#ifndef NDEBUG // Will not be executed in optimised build                               
        if(msgLvl() <= MSG::DEBUG)
        {        
            msg() << MSG::DEBUG  << " RoI id " << roiDescriptor->roiId() 
                  << " LVL1 id " << roiDescriptor->l1Id() 
		  << *roiDescriptor << endmsg;
	    //                  << " located at   phi = " <<  roiDescriptor->phi0() 
            //      << ", eta = " << roiDescriptor->eta0() << endmsg;
        }
#endif
    }
    else
    {
        msg() <<  MSG::WARNING << " Failed to find RoiDescriptor " << endmsg;
        return HLT::ERROR;
    }

    // Some debug output:

    if(msgLvl() <= MSG::DEBUG)
    {
        msg() << MSG::DEBUG  << "Message to count events. LVL1 phi="
              << roiDescriptor->phi()
              << " & LVL1 eta="
              << roiDescriptor->eta() << " " << roiDescriptor
              << endmsg;
    }


    // End LVL1 part
    //    double RoIeta = roiDescriptor->eta();
    //    double RoIphi = roiDescriptor->phi();

    const TrigTauClusterDetails * pDetails= new TrigTauClusterDetails();

    std::string key = "";  
    HLT::ErrorCode  hltstatusD = recordAndAttachFeature(outputTE, pDetails, key, "TrigT2CaloTauDetails");
    if(hltstatusD != HLT::OK)
    {
        if (msgLvl() <= MSG::DEBUG)
        {
            msg() << MSG::ERROR << "Write of TrigTauClusterDetails into outputTE failed" << endmsg;
        }
        return hltstatusD;
    }

    // retrieve TrigTauClusterDetails from the TE
    ElementLink< TrigTauClusterDetailsContainer > ELDetails;
    HLT::ErrorCode stat = getFeatureLink< TrigTauClusterDetailsContainer, TrigTauClusterDetails >( outputTE, ELDetails );

    if(stat == HLT::OK && ELDetails.isValid())
    {
        if(msgLvl() <= MSG::DEBUG)
        { 
            (*ELDetails)->print(msg()); 
        }
    }
    else
    {
        if(msgLvl() <= MSG::DEBUG)
        { 
            msg() << MSG::DEBUG << "Failed to get TrigTauClusterDetails" << endmsg; 
        }
        return HLT::MISSING_FEATURE;
    }

    TrigTauCluster* ptrigTauCluster = new TrigTauCluster(ELDetails.getStorableObjectPointer(), ELDetails.index(), 0.0, -10.0, -10.0, 0);
    // Energies, EMRadius and other variables are initialized at the 
    // TrigTauCluster creation time.

    // Add RoI word to TrigTauCluster
    ptrigTauCluster->setRoIword(roiDescriptor->roiWord());

    // zeros the container per RoI
    m_Container = 0;

    HLT::ErrorCode ToolStat = HLT::OK; // define flag to monitor problems with tools without stopping the sequence.


    /// generate the new roiDescriptor with the correct sizes

    const TrigRoiDescriptor* _roi = roiDescriptor; 

    TrigRoiDescriptor roi( _roi->eta(), _roi->eta()-m_etaWidth, _roi->eta()+m_etaWidth,
			   _roi->phi(), HLT::wrap_phi(_roi->phi()-m_phiWidth), HLT::wrap_phi(_roi->phi()+m_phiWidth) );

    /// this isn't needed 
    //  TrigRoiDescriptor* roiEM = new TrigRoiDescriptor( _roi->eta(), _roi->eta()-m_etaWidthEM, _roi->eta()+m_etaWidthE<,
    //	  					          _roi->phi(), HLT::wrap_phi(_roi->phi()-m_phiWidthEM), HLT::wrap_phi(_roi->phi()+m_phiWidthEM) );

    msg() << MSG::DEBUG << "Using RoIs " << roi << endmsg;

    ToolHandleArray<IAlgToolCalo>::iterator it = m_emAlgTools.begin();
    if(timerSvc()) m_timer[1]->start();
    uint32_t error = 0;
    for(; it < m_emAlgTools.end(); it++)
    {
        //        HLT::ErrorCode stat = (*it)->execute(*ptrigTauCluster, m_phiWidth, m_etaWidth, m_phiWidthEM, m_etaWidthEM, RoIeta, RoIphi);
        //       HLT::ErrorCode stat = (*it)->execute(*ptrigTauCluster, *roi, *roiEM );
        HLT::ErrorCode stat = (*it)->execute(*ptrigTauCluster, roi );
        if(stat.reason() == NEG_ENERGY_CLUSTER)
        {
            msg() << MSG::DEBUG << (*it)->name() << " Found a cluster with E~<=0. CONTINUE execution. " << endmsg;
            // do not forget to delete trigTauCluster of attach it to storegate if return
            // return  stat; // uncomment to avaid running on the remaining tools.
            ToolStat = stat;
        }
        if(stat.reason() == NULL_ENERGY_SAMPLING)
        {
            msg() << MSG::DEBUG << (*it)->name() << " Found E==0 in this sampling. CONTINUE execution. " << endmsg;
            // do not forget to delete trigTauCluster of attach it to storegate if return
            // return  stat; // uncomment to avaid running on the remaining tools.
            ToolStat = stat;
            // since userdefined errors are not monitored (reason=continue)
            // fill our own variable with T2CaloTau related errors
        }
        if(stat == HLT::TOOL_FAILURE)
        {
            msg() << MSG::WARNING << "T2CaloTau AlgTool " << (*it)->name() << "  returned Failure" << endmsg;
            // if tool has returned this failure, then loadingCollection has failed and it
            // does not make sense to keep this TrigTauCluster - we would not learn anything from it
            delete ptrigTauCluster;
            delete pDetails;	
            return  stat;
        }
        uint32_t in_error = (*it)->report_error();
        if(0x0FFF00FF & in_error) m_conversionError++;
        if(0xF0000000 & in_error) m_algorithmError++;

        error |= in_error;
    }

    // convert bits into fixed-size vector variable for error monitoring
    TAUCLUSTMON::FillErrorMonitoring(error, &m_quality);
    // Fill ALLCLUST bin for all clusters, so we can know the relative importance of each error with this normalization.
    m_quality.push_back(TAUCLUSTMON::ALLCLUST);
    // Fill GOODCLUST for clusters without errors
    if(!error) m_quality.push_back(TAUCLUSTMON::GOODCLUST);

    // OI  Now, ensure that phi is [-pi,pi] indepedent on what tools do...
    while(ptrigTauCluster->phi() < -M_PI) ptrigTauCluster->setPhi(ptrigTauCluster->phi() + 2.0 * M_PI);
    while(ptrigTauCluster->phi() >  M_PI) ptrigTauCluster->setPhi(ptrigTauCluster->phi() - 2.0 * M_PI);

    // 
    // Get L1 RoiDescriptor
    const TrigRoiDescriptor* roiL1Descriptor = 0;
    HLT::ErrorCode tmpStatus = getFeature( inputTE, roiL1Descriptor, "initialRoI" );

    if(tmpStatus == HLT::OK && roiDescriptor){
      if(msgLvl() <= MSG::DEBUG) {        
	msg() << MSG::DEBUG  << " RoI id " << roiL1Descriptor->roiId() 
	      << " LVL1 id " << roiL1Descriptor->l1Id() 
	      << *roiL1Descriptor << endmsg;
      }
    } else {
      msg() <<  MSG::WARNING << " Failed to find RoiDescriptor " << endmsg;
      return HLT::ERROR;
    }


    //Fill monitored variables
    const TrigTauClusterDetails* clusterDetails = ptrigTauCluster->clusterDetails();
    const double coshEta = cosh(ptrigTauCluster->eta());
    const double inv_coshEta = 1. / coshEta;

    m_EMRadius    = clusterDetails->EMRadius(2);
    m_EMRadius3S  = ptrigTauCluster->EMRadius3S();
    m_CaloRadius  = ptrigTauCluster->CaloRadius();
    m_HadRad      = ptrigTauCluster->HadRadius();
    m_IsoFrac     = ptrigTauCluster->IsoFrac();
    m_StripWidth  = ptrigTauCluster->stripWidth();
    m_EMFraction  = ptrigTauCluster->EMFrac();

    //wide: cone 0.3 -> wide at TrigTauClusterDetails
    m_EtRawWide = (clusterDetails->EMenergyWide(0)
                + clusterDetails->EMenergyWide(1)
                + clusterDetails->EMenergyWide(2)
                + clusterDetails->EMenergyWide(3)
                + clusterDetails->HADenergyWide(0)
                + clusterDetails->HADenergyWide(1)
                + clusterDetails->HADenergyWide(2)) * inv_coshEta;

    //medium: cone 0.2 -> medium at TrigTauClusterDetails. In previus version of the code, Wide instead of Medium
    m_EMEnMedium = clusterDetails->EMenergyMedium(0)
               + clusterDetails->EMenergyMedium(1)
               + clusterDetails->EMenergyMedium(2)
               + clusterDetails->EMenergyMedium(3); 
    m_HADEnMedium = clusterDetails->HADenergyMedium(0)
                + clusterDetails->HADenergyMedium(1)
                + clusterDetails->HADenergyMedium(2);

    //narrow: cone 0.1 -> narrow at TrigTauClusterDetails
    m_EMEnNarrow = clusterDetails->EMenergyNarrow(0)
                 + clusterDetails->EMenergyNarrow(1)
                 + clusterDetails->EMenergyNarrow(2)
                 + clusterDetails->EMenergyNarrow(3);
    m_HADEnNarrow = clusterDetails->HADenergyNarrow(0)
                  + clusterDetails->HADenergyNarrow(1)
                  + clusterDetails->HADenergyNarrow(2);

    m_EtRawMediumEM0 = clusterDetails->EMenergyMedium(0) * inv_coshEta;
    m_EtRawMediumEM1 = clusterDetails->EMenergyMedium(1) * inv_coshEta;
    m_EtRawMediumEM2 = clusterDetails->EMenergyMedium(2) * inv_coshEta;
    m_EtRawMediumEM3 = clusterDetails->EMenergyMedium(3) * inv_coshEta;
    m_EtRawMediumHad0 = clusterDetails->HADenergyMedium(0) * inv_coshEta;
    m_EtRawMediumHad1 = clusterDetails->HADenergyMedium(1) * inv_coshEta;
    m_EtRawMediumHad2 = clusterDetails->HADenergyMedium(2) * inv_coshEta;

    m_EtRawMedium = m_EtRawMediumEM0 + m_EtRawMediumEM1 + m_EtRawMediumEM2 + m_EtRawMediumEM3 + m_EtRawMediumHad0 + m_EtRawMediumHad1 + m_EtRawMediumHad2;

    m_CoreFraction = ptrigTauCluster->CoreFrac();

    m_Eta = ptrigTauCluster->eta();
    m_Phi = ptrigTauCluster->phi();
    if(roiL1Descriptor)
    {
        m_EtaL1 = roiL1Descriptor->eta();
        m_PhiL1 = roiL1Descriptor->phi();
    }

    m_dEtaL2Tau_RoI = m_Eta - m_EtaL1;
    m_dPhiL2Tau_RoI = m_Phi - m_PhiL1;
    //   m_dEtaL2Tau_RoI = ptrigTauCluster->eta() - roiL1Descriptor->eta0();
    //   m_dPhiL2Tau_RoI = ptrigTauCluster->phi() - roiL1Descriptor->phi0();

    if(m_dPhiL2Tau_RoI > M_PI)  m_dPhiL2Tau_RoI -= 2 * M_PI;
    if(m_dPhiL2Tau_RoI < -M_PI) m_dPhiL2Tau_RoI += 2 * M_PI;


    if(m_EtRawWide > 495000.) m_EtRawWide = 499000.;

    if(m_EMEnMedium > 495000.) m_EMEnMedium = 499000.;
    if(m_HADEnMedium > 495000.) m_HADEnMedium = 499000.;

    if(m_EMEnNarrow > 495000.) m_EMEnNarrow = 499000.;
    if(m_HADEnNarrow > 495000.) m_HADEnNarrow = 499000.;

    if(m_EtRawMedium > 495000.) m_EtRawMedium = 499000.;
    if(m_EtRawMediumEM0 > 495000.) m_EtRawMediumEM0 = 499000.;
    if(m_EtRawMediumEM1 > 495000.) m_EtRawMediumEM1 = 499000.;
    if(m_EtRawMediumEM2 > 495000.) m_EtRawMediumEM2 = 499000.;
    if(m_EtRawMediumEM3 > 495000.) m_EtRawMediumEM3 = 499000.;
    if(m_EtRawMediumHad0 > 495000.) m_EtRawMediumHad0 = 499000.;
    if(m_EtRawMediumHad1 > 495000.) m_EtRawMediumHad1 = 499000.;
    if(m_EtRawMediumHad2 > 495000.) m_EtRawMediumHad2 = 499000.;

    // Cluster quality is a collection of possible errors
    // No error => quality=0
    ptrigTauCluster->setquality(error);

    if(timerSvc()) m_timer[1]->stop();


    if(msgLvl() <= MSG::DEBUG)
    {
        // Print out Cluster produced
        //    msg() << MSG::DEBUG << "TEST-TEST-TEST-TEST" << endmsg;
        msg() << MSG::DEBUG << " REGTEST: eta/phi = "<< m_Eta << "/" << m_Phi << endmsg;
        //msg() << MSG::DEBUG << " REGTEST: etaL1/phiL1 = "<< m_EtaL1 << "/" << m_PhiL1 << endmsg;
        msg() << MSG::DEBUG << " REGTEST: EMenergy0Narrow/Medium/Wide = "<< (*ptrigTauCluster).clusterDetails()->EMenergyNarrow(0) 
              << "/" << (*ptrigTauCluster).clusterDetails()->EMenergyMedium(0) << "/" 
              << (*ptrigTauCluster).clusterDetails()->EMenergyWide(0) << endmsg;
        msg() << MSG::DEBUG << " REGTEST: EMenergy1Narrow/Medium/Wide = "
              << (*ptrigTauCluster).clusterDetails()->EMenergyNarrow(1) << "/" 
              << (*ptrigTauCluster).clusterDetails()->EMenergyMedium(1) << "/"
              << (*ptrigTauCluster).clusterDetails()->EMenergyWide(1) << endmsg;
        msg() << MSG::DEBUG << " REGTEST: EMenergy2Narrow/Medium/Wide = "
              << (*ptrigTauCluster).clusterDetails()->EMenergyNarrow(2) << "/"
              << (*ptrigTauCluster).clusterDetails()->EMenergyMedium(2) << "/" 
              << (*ptrigTauCluster).clusterDetails()->EMenergyWide(2) <<  endmsg;
        msg() << MSG::DEBUG << " REGTEST: EMenergy3Narrow/Medium/Wide = "
              << (*ptrigTauCluster).clusterDetails()->EMenergyNarrow(3) << "/"
              << (*ptrigTauCluster).clusterDetails()->EMenergyMedium(3) << "/" 
              << (*ptrigTauCluster).clusterDetails()->EMenergyWide(3) << endmsg;
        msg() << MSG::DEBUG << " REGTEST: HADenergy0Narrow/Medium/Wide = "
              << (*ptrigTauCluster).clusterDetails()->HADenergyNarrow(0) << "/" 
              << (*ptrigTauCluster).clusterDetails()->HADenergyMedium(0) << "/" 
              << (*ptrigTauCluster).clusterDetails()->HADenergyWide(0) << endmsg;
        msg() << MSG::DEBUG << " REGTEST: HADenergy1Narrow/Medium/Wide = "
              << (*ptrigTauCluster).clusterDetails()->HADenergyNarrow(1) << "/" 
              << (*ptrigTauCluster).clusterDetails()->HADenergyMedium(1) << "/" 
              << (*ptrigTauCluster).clusterDetails()->HADenergyWide(1) << endmsg;
        msg() << MSG::DEBUG << " REGTEST: HADenergy2Narrow/Medium/Wide = "
              << (*ptrigTauCluster).clusterDetails()->HADenergyNarrow(2) << "/" 
              << (*ptrigTauCluster).clusterDetails()->HADenergyMedium(2) << "/" 
              << (*ptrigTauCluster).clusterDetails()->HADenergyWide(2) << endmsg;

        msg() << MSG::DEBUG << " REGTEST: EMRadius0/1/2/3 = "
              << (*ptrigTauCluster).clusterDetails()->EMRadius(0) << "/" 
              << (*ptrigTauCluster).clusterDetails()->EMRadius(1) << "/" 
              << (*ptrigTauCluster).clusterDetails()->EMRadius(2) << "/" 
              << (*ptrigTauCluster).clusterDetails()->EMRadius(3) << endmsg;

        msg() << MSG::DEBUG << " REGTEST: HADRadius0/1/2 = "
              << (*ptrigTauCluster).clusterDetails()->HADRadius(0) << "/" 
              << (*ptrigTauCluster).clusterDetails()->HADRadius(1) << "/" 
              << (*ptrigTauCluster).clusterDetails()->HADRadius(2) << endmsg;

        msg() << MSG::DEBUG << " REGTEST: EMenergyWidth0/1/2/3 = "
              << (*ptrigTauCluster).clusterDetails()->EMenergyWidth(0) << "/" 
              << (*ptrigTauCluster).clusterDetails()->EMenergyWidth(1) << "/" 
              << (*ptrigTauCluster).clusterDetails()->EMenergyWidth(2) << "/" 
              << (*ptrigTauCluster).clusterDetails()->EMenergyWidth(3) << endmsg;

        msg() << MSG::DEBUG << " REGTEST: EMenergy/Hadenergy =  "
              << (*ptrigTauCluster).EMenergy() << "/" 
              << (*ptrigTauCluster).HADenergy() << "/" 
              <<  endmsg;
        msg() << MSG::DEBUG << " REGTEST: numStripCells/stripWidth/IsoFrac = "
              << (*ptrigTauCluster).numStripCells() << "/" 
              << (*ptrigTauCluster).stripWidth() << "/"
              << m_IsoFrac << endmsg;
        msg() << MSG::DEBUG << " REGTEST: RoIWord = "
              << (*ptrigTauCluster).RoIword() << endmsg;
    }  

    //// From egamma. This is already done inside the tools for tau! but maybe would like to change at some point...
    //  if ( error ) {
    //    // Clustering failed. Transmit ahead L1
    //    ptrigTauCluster->setEta(etaL1);
    //    ptrigTauCluster->setPhi(phiL1);
    //    ptrigTauCluster->setEnergy(0.0);
    //  }

    key = "";
    HLT::ErrorCode hltstatus = recordAndAttachFeature(outputTE, ptrigTauCluster, key, "TrigT2CaloTau");
    if(hltstatus != HLT::OK)
    {
        if (msgLvl() <= MSG::DEBUG)
        {
            msg() << MSG::ERROR << "Write of TrigTauCluster into outputTE failed" << endmsg;
        }
        return hltstatus;
    }

    if(m_storeCells)
    {
        if(m_Container != 0)
        {
            HLT::ErrorCode hltstatus = recordAndAttachFeature(outputTE, m_Container, key, "TrigT2CaloTauCells");
            if(msgLvl() <= MSG::DEBUG)
            {
                msg() << MSG::DEBUG << " recorded " <<  m_Container->size() << " cells "<< endmsg;
            }
            if(hltstatus != HLT::OK)
            {
#ifndef NDEBUG
                if(msgLvl() <= MSG::DEBUG)
                {
                    msg() << MSG::ERROR << "Write of TrigTauClusterCells into outputTE failed" << endmsg;
                }
#endif
                if(m_timersvc) m_timer[0]->stop();
                return hltstatus;
            }
        } // End of if to check whether there is a container
    } // End of if to check the option is ok

    // Create a new RoiDescriptor with updated eta and phi.
    // Note that the steering will propagate l1Id and roiId automatically
    // so no need to set them.

    if(m_updateRoiDescriptor)
    {

      /// what size should we create this roi with ??? 
      /// use some new parameters
      
      double _eta = ptrigTauCluster->eta();
      double _phi = ptrigTauCluster->phi();
      
      TrigRoiDescriptor* newRoiDescriptor   = new TrigRoiDescriptor( roiDescriptor->roiWord(), 
								     roiDescriptor->l1Id(), 
								     roiDescriptor->roiId(), 
								     _eta, _eta-m_etaWidthForID, _eta+m_etaWidthForID,
								     _phi, HLT::wrap_phi(_phi-m_phiWidthForID), HLT::wrap_phi(_phi+m_phiWidthForID) );
    
      /// obsolete constructor 
      //      TrigRoiDescriptor* newRoiDescriptor = new TrigRoiDescriptor(roiDescriptor->roiWord(), 
      //								  roiDescriptor->l1Id(), 
      //								  roiDescriptor->roiId(), 
      //								  ptrigTauCluster->eta(), 
      //								  ptrigTauCluster->phi());

        if(msgLvl() <= MSG::DEBUG)
        {
            msg() << MSG::DEBUG  << "Recorded an RoiDescriptor "
                  << " phi " << newRoiDescriptor->phi()
                  << " eta " << newRoiDescriptor->eta() << "  " << *newRoiDescriptor << endmsg;
        }

        hltstatus = attachFeature(outputTE, newRoiDescriptor, "TrigT2CaloTau"); 

        if(hltstatus != HLT::OK)
        {
            if(msgLvl() <= MSG::DEBUG)
            {
                msg() << MSG::ERROR << "Write of newRoiDescriptor into outputTE failed" << endmsg;
            }
            return hltstatus;
        }
    }
    // Some debug output:
#ifndef NDEBUG
    if(msgLvl() <= MSG::DEBUG)
    {
        msg() << MSG::DEBUG
              << "We assume success, set TE with id "
              << outputTE->getId()
              << " active to signal positive result."
              << endmsg;
    }
#endif

    // Time total T2CaloT2Calo execution time.
    if(timerSvc()) m_timer[0]->stop();      

    //return HLT::OK;
    return ToolStat;
}
