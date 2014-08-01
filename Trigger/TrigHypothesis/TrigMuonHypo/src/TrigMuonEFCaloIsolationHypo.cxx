/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonHypo/TrigMuonEFCaloIsolationHypo.h"

#include "TrigMuonEvent/TrigMuonEFIsolationContainer.h"
#include "TrigMuonEvent/TrigMuonEFIsolation.h"

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

#include "TrigSteeringEvent/TrigPassBits.h"

class ISvcLocator;

/**
 * Constructor.
 * Declare variables to get from job options
 * the monitored variables.
 */
TrigMuonEFCaloIsolationHypo::TrigMuonEFCaloIsolationHypo(const std::string &name, ISvcLocator *pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator) {

    declareProperty("AcceptAll", m_acceptAll = false);
    declareProperty("DoAbsCut", m_abscut = true); //true for absolute cuts, false for sumpt/pt
    // declareProperty("PtCone02Cut",m_ptcone02_cut=-1.0); //convention is < 0 means don't cut
    // declareProperty("PtCone03Cut",m_ptcone03_cut=-1.0); //convention is < 0 means don't cut

    // declareMonitoredStdContainer("PtCone02", m_fex_ptcone02,  IMonitoredAlgo::AutoClear);
    // declareMonitoredStdContainer("PtCone03", m_fex_ptcone03,  IMonitoredAlgo::AutoClear);

    //Std isolation
    declareProperty("CaloConeSize",         m_CaloConeSize = 2); //Calorimetric cone size (1: DR=0.1, 2: DR=0.2, 3: DR=0.3: 4: DR=0.4)
    m_CaloConeSize.verifier().setBounds(1, 4);
    // declareProperty("IDConeSize",           m_IDConeSize = 2);   //ID cone size (1: DR=0.1, 2: DR=0.2, 3: DR=0.3: 4: DR=0.4)
    // m_IDConeSize.verifier().setBounds(1,4);

    // declareProperty("UseID",                m_UseID        = true );  //Iso based on ID
    // declareProperty("UseAbsID",             m_UseAbsID     = false ); //Absolute ID iso
    declareProperty("UseCalo",              m_UseCalo      = true );  //Iso based on Calo
    declareProperty("UseAbsCalo",           m_UseAbsCalo   = true );  //Absolute Calo iso
    declareProperty("UseIDptForMu",         m_UseIDptForMu = false ); //muon pt is IDmeasured pt

    declareProperty("MaxCaloIso_1",         m_MaxCaloIso_1 = 4000.0);
    declareProperty("MaxCaloIso_2",         m_MaxCaloIso_2 = 4000.0);
    declareProperty("MaxCaloIso_3",         m_MaxCaloIso_3 = 3500.0);

    // declareProperty("MaxIDIso_1",           m_MaxIDIso_1=0.03);
    // declareProperty("MaxIDIso_2",           m_MaxIDIso_2=0.03);
    // declareProperty("MaxIDIso_3",           m_MaxIDIso_3=0.04);

    declareProperty("EtaRegion_1",          m_EtaRegion_1 = 1.0);
    declareProperty("EtaRegion_2",          m_EtaRegion_2 = 1.4);
    declareProperty("EtaRegion_3",          m_EtaRegion_3 = 9.9);

    declareMonitoredVariable("CutCounter",  m_cutCounter);
    declareMonitoredVariable("SumEtCone",   m_SumEtCone);
    declareMonitoredVariable("SumPtCone",   m_SumPtCone);

}

/**
 * Destructor. Nothing to do for now.
 */
TrigMuonEFCaloIsolationHypo::~TrigMuonEFCaloIsolationHypo() {

}

/**
 * Initialize the algorithm.
 * Here we print out the cut values.
 */
HLT::ErrorCode TrigMuonEFCaloIsolationHypo::hltInitialize() {


    //  if(m_acceptAll) {
    //    msg() << MSG::INFO
    //   << "Accepting all the events with not cut!"
    //   << endreq;
    //  } else {
    //    if(m_ptcone02_cut < 0.0 && m_ptcone03_cut < 0.0) {
    //      msg() << MSG::FATAL
    //     << "Configured to apply cuts, but not cut was specified" << endreq;
    //      return HLT::BAD_JOB_SETUP;
    //    }
    //    if(m_ptcone02_cut > 0.0) {
    //      if(m_abscut) {
    // msg() << MSG::INFO
    //       << "Requiring sum pT in 0.2 cone < " << m_ptcone02_cut.value() << " MeV" << endreq;
    //      } else {
    // msg() << MSG::INFO
    //       << "Requiring sum pT in 0.2 cone / muon pT < " << m_ptcone02_cut.value() << endreq;
    //      }//relative cut
    //    }
    //    if(m_ptcone03_cut > 0.0) {
    //      if(m_abscut) {
    // msg() << MSG::INFO
    //       << "Requiring sum pT in 0.3 cone < " << m_ptcone03_cut.value() << " MeV" << endreq;
    //      } else {
    //   msg() << MSG::INFO
    //  << "Requiring sum pT in 0.3 cone / muon pT < " << m_ptcone03_cut.value() << endreq;
    //      }//relative cut
    //    }//cut on 0.3 cone
    //  }


    if (m_acceptAll) {
        msg() << MSG::INFO
              << "Accepting all the events with not cut!"
              << endreq;
    } else {
        if (m_UseCalo) {
            msg() << MSG::INFO << "Calo based isolation activated" << endreq;
            if (m_UseAbsCalo) msg() << MSG::INFO << "Absolute isolation cut for calo selected" << endreq;
            msg() << MSG::INFO << "Calorimetric cone size selected: " << m_CaloConeSize << endreq;
        }
        // if (m_UseID) {
        //   msg() << MSG::INFO << "ID   based isolation activated" << endreq;
        //   if (m_UseAbsID) msg() << MSG::INFO << "Absolute isolation cut for ID selected" << endreq;
        //   msg() << MSG::INFO << "ID cone size selected: " << m_IDConeSize << endreq;
        // }

        // if (m_UseHollowConeCalo) {
        //   msg() << MSG::INFO << "Hollow Cone algorithm selected for calo isolation" << endreq;
        //   msg() << MSG::INFO << "Outer cone: " << m_HollowConeCaloOuter << endreq;
        //   msg() << MSG::INFO << "Inner cone: " << m_HollowConeCaloInner << endreq;
        // }
        // if (m_UseHollowConeID) {
        //   msg() << MSG::INFO << "Hollow Cone algorithm selected for ID isolation" << endreq;
        //   msg() << MSG::INFO << "Outer cone: " << m_HollowConeIDOuter << endreq;
        //   msg() << MSG::INFO << "Inner cone: " << m_HollowConeIDInner << endreq;
        // }

        //   if (m_RemoveMaxPtTrack)
        //     msg() << MSG::INFO << "Pt of max pt ID track (different from muon) subtracted from sumpt" << endreq;

        //   if (m_UseMaxPtTrackAlgo)
        //     msg() << MSG::INFO << "Max pt ID Algo selected (sumpt == pt of max pt ID track (different from the muon)" << endreq;

        //   if (m_DoNotSubtractMuonPt)
        //     msg() << MSG::INFO << "Muon pt is not subtracted from ptsum" << endreq;
        // }


        msg() << MSG::INFO
              << "Initialization completed successfully"
              << endreq;

    }

    return HLT::OK;
}

/**
 * Finalize the algorithm - nothing to do here yet.
 */
HLT::ErrorCode TrigMuonEFCaloIsolationHypo::hltFinalize() {
    return HLT::OK;
}

/**
 * Execute the algorithm.
 * Here we apply the cuts to the muon(s).
 * The hypo passes if at least one muon passes the cuts.
 */
HLT::ErrorCode TrigMuonEFCaloIsolationHypo::hltExecute(const HLT::TriggerElement *outputTE, bool &pass) {

    msg() << MSG::INFO << "In execute() of EF calo iso hypo" << endreq;

    pass = false; // fail by default

    if (m_acceptAll) {
        pass = true;
        if (msgLvl() <= MSG::INFO) {
            msg() << MSG::INFO
                  << "Accept property is set: taking all the events" << endreq;
        }
        return HLT::OK;
    }//acceptAll

    const bool debug = msgLvl() <= MSG::INFO;

    // Some debug output:
    if (debug) msg() << MSG::INFO << "outputTE->ID(): " << outputTE->getId() << endreq;

    // Get vector of pointers of TrigMuonEFIsolation objects linked to the outputTE
    std::vector<const TrigMuonEFIsolationContainer *>  v_efisolation;
    if (getFeatures(outputTE, v_efisolation, "MuonEFCaloIsolation") != HLT::OK) {
        if (debug) msg() << MSG::INFO << "no TrigMuonEFIsolationContainer" << endreq;
        return HLT::MISSING_FEATURE;
    } else {
        if (debug) msg() << MSG::INFO << "vector of TrigMuonEFIsolation found with size " << v_efisolation.size() << endreq;
    }

    // check the size of the vector
    if (v_efisolation.size() != 1) {
        msg() << MSG::ERROR << "Got " << v_efisolation.size()
              << " TrigMuonEFIsolationContainers when 1 is expected" << endreq;
        return HLT::NAV_ERROR;
    }

    // get the container
    const TrigMuonEFIsolationContainer *isocont = v_efisolation[0];

    // make pass bits object to store the result per muon
    TrigPassBits *passBits = HLT::makeTrigPassBits(isocont);

    // result of the hypo
    bool result = true;


    // loop over objects (muons) in the container
    for (TrigMuonEFIsolationContainer::const_iterator isoit = isocont->begin();
            isoit != isocont->end(); ++isoit) {


        float etsum  = 0.0;
        float ptsum  = 0.0;
        float etsumI = 0.0;
        float ptsumI = 0.0;

        if      (m_CaloConeSize == 1) etsum  = (*isoit)->sumEtCone01();
        else if (m_CaloConeSize == 2) etsum  = (*isoit)->sumEtCone02();
        else if (m_CaloConeSize == 3) etsum  = (*isoit)->sumEtCone03();
        else if (m_CaloConeSize == 4) etsum  = (*isoit)->sumEtCone04();
        else                          etsum  = (*isoit)->sumEtCone02();

        // if      (m_IDConeSize == 1)   ptsum  = pMuonIso->getsumpt01();
        // else if (m_IDConeSize == 2)   ptsum  = pMuonIso->getsumpt02();
        // else if (m_IDConeSize == 3)   ptsum  = pMuonIso->getsumpt03();
        // else if (m_IDConeSize == 4)   ptsum  = pMuonIso->getsumpt04();
        // else                          ptsum  = pMuonIso->getsumpt02();

        // if (m_UseHollowConeCalo) {
        //   if      (m_HollowConeCaloOuter == 1) etsum  = pMuonIso->getsumet01();
        //   else if (m_HollowConeCaloOuter == 2) etsum  = pMuonIso->getsumet02();
        //   else if (m_HollowConeCaloOuter == 3) etsum  = pMuonIso->getsumet03();
        //   else if (m_HollowConeCaloOuter == 4) etsum  = pMuonIso->getsumet04();
        //   else                                 etsum  = pMuonIso->getsumet02();
        //   if      (m_HollowConeCaloInner == 1) etsumI = pMuonIso->getsumet01();
        //   else if (m_HollowConeCaloInner == 2) etsumI = pMuonIso->getsumet02();
        //   else if (m_HollowConeCaloInner == 3) etsumI = pMuonIso->getsumet03();
        //   else if (m_HollowConeCaloInner == 4) etsumI = pMuonIso->getsumet04();
        //   else                                 etsumI = pMuonIso->getsumet02();
        //   if (m_HollowConeCaloOuter <= m_HollowConeCaloInner) {
        //     msg() << MSG::ERROR << "CALO Outer cone radius smaller or equal to inner cone one, please check settings!" << endreq;
        //     etsum  = pMuonIso->getsumet02();
        //     etsumI = 0.0;
        //   }
        // }

        // if (m_UseHollowConeID) {
        //   if      (m_HollowConeIDOuter == 1) ptsum  = pMuonIso->getsumpt01();
        //   else if (m_HollowConeIDOuter == 2) ptsum  = pMuonIso->getsumpt02();
        //   else if (m_HollowConeIDOuter == 3) ptsum  = pMuonIso->getsumpt03();
        //   else if (m_HollowConeIDOuter == 4) ptsum  = pMuonIso->getsumpt04();
        //   else                               ptsum  = pMuonIso->getsumpt02();
        //   if      (m_HollowConeIDInner == 1) ptsumI = pMuonIso->getsumpt01();
        //   else if (m_HollowConeIDInner == 2) ptsumI = pMuonIso->getsumpt02();
        //   else if (m_HollowConeIDInner == 3) ptsumI = pMuonIso->getsumpt03();
        //   else if (m_HollowConeIDInner == 4) ptsumI = pMuonIso->getsumpt04();
        //   else                               ptsumI = pMuonIso->getsumpt02();
        //   if (m_HollowConeIDOuter <= m_HollowConeIDInner) {
        //     msg() << MSG::ERROR << "ID Outer cone radius smaller or equal to inner cone one, please check settings!" << endreq;
        //     ptsum  = pMuonIso->getsumpt02();
        //     ptsumI = 0.0;
        //   }
        // }

        // if (m_RemoveMaxPtTrack) {
        //   ptsum -= pMuonIso->getMaxPtID();
        // }

        // if (m_UseMaxPtTrackAlgo) {
        //   ptsum  = pMuonIso->getMaxPtID();
        //   ptsumI = 0.0;
        // }



        // monitoring
        // m_fex_ptcone02.push_back((*isoit)->sumTrkPtCone02()/1000.0);
        // m_fex_ptcone03.push_back((*isoit)->sumTrkPtCone03()/1000.0);

        // bool goodmu=true;




        // Pointer for the associated muon track
        const TrigMuonEFInfoTrack *infotrk = 0;

        // get the info object
        infotrk = (*isoit)->getEFMuonInfoTrack();

        if (!infotrk) {
            msg(MSG::WARNING) << "TrigMuonEFIsolation object has no attached TrigMuonEFInfoTrack"
                              << ". Ignoring this muon."
                              << "Please contact muon trigger experts." << endreq;
            continue;
        }//valid InfoTrack
        double mupt = -1.0;
        float etamu = -1000.;
        if (infotrk->hasCombinedTrack()) {
            mupt = infotrk->CombinedTrack()->pt();
            etamu = infotrk->CombinedTrack()->eta();
            if (debug) msg(MSG::INFO) << "Got combined muon, pt = " << mupt << endreq;
        }//combined
        //     else {
        // if(infotrk->hasExtrapolatedTrack()) {
        //   mupt = infotrk->ExtrapolatedTrack()->pt();
        //   if(debug) msg(MSG::INFO) << "Got extrapolated muon, pt = " << mupt << endreq;
        // }// extrapolated
        // else {
        //   if(infotrk->hasSpectrometerTrack()) {
        //     mupt = infotrk->SpectrometerTrack()->pt();
        //     if(debug) msg(MSG::INFO) << "Got spectrometer muon, pt = " << mupt << endreq;
        //   }
        // }
        //    }
        if (mupt < 0) {
            msg(MSG::WARNING) << "Invalid muon pT = " << mupt
                              << "Ignoring this muon" << endreq;
            continue;
        }




        // float ptmuid = ?? ? //fabs(pMuonIso->getPtMuID());
        // float ptmu   = ?? ? //fabs(pMuonIso->getPtMu());
        //  if (m_UseIDptForMu || usealgo != 1) ptmu = ptmuid;
        // if (m_DoNotSubtractMuonPt) ptmuid = 0.0;

        // float isoID = ptsum-ptsumI-ptmuid;
        // m_SumPtCone = isoID/1000.;
        // if (!m_UseAbsID) {
        //  if (ptmu > 0) isoID /= ptmu;
        // }
        // if (isoID < 0) {
        //   if (msgLvl() <= MSG::INFO) msg() << MSG::INFO << " isoID < 0 --> should never happen, set to zero " << endreq;
        //   isoID = 0.0;
        // }

        float isoCalo = etsum /* - etsumI*/;
        m_SumEtCone = isoCalo / 1000.;
        if (!m_UseAbsCalo) {
            if (ptsum > 0) isoCalo /= ptsum;
        }
        if (isoCalo < 0) {
            if (msgLvl() <= MSG::INFO) msg() << MSG::INFO << " isoCalo < 0 --> should never happen, set to zero " << endreq;
            isoCalo = 0.0;
        }


        bool isIsolatedCalo = true;
        bool isIsolatedID   = true;
        if (fabs(etamu) <= m_EtaRegion_1) {
            if (m_UseCalo) {
                if (isoCalo > m_MaxCaloIso_1.value()) isIsolatedCalo = false;
            }
            // if (m_UseID) {
            //   if (isoID > m_MaxIDIso_1.value())       isIsolatedID = false;
            // }
            if (isIsolatedCalo && isIsolatedID) m_cutCounter = 1;
        } else if (fabs(etamu) > m_EtaRegion_1 && fabs(etamu) <= m_EtaRegion_2 ) {
            if (m_UseCalo) {
                if (isoCalo > m_MaxCaloIso_2.value()) isIsolatedCalo = false;
            }
            // if (m_UseID) {
            //   if (isoID > m_MaxIDIso_2.value())       isIsolatedID = false;
            // }
            if (isIsolatedCalo && isIsolatedID) m_cutCounter = 2;
        } else if (fabs(etamu) > m_EtaRegion_2 && fabs(etamu) <= m_EtaRegion_3 ) {
            if (m_UseCalo) {
                if (isoCalo > m_MaxCaloIso_3.value()) isIsolatedCalo = false;
            }
            // if (m_UseID) {
            //   if (isoID > m_MaxIDIso_3.value())       isIsolatedID = false;
            // }
            if (isIsolatedCalo && isIsolatedID) m_cutCounter = 3;
        }

        if (isIsolatedCalo && isIsolatedID) {
            result = true;
        }
        
        //=============================
        // Temp output
        //=============================
        if (isIsolatedCalo && isIsolatedID) {
	  msg() << MSG::INFO << "Muon passed with isoCalo = " << isoCalo << endreq;
        }
        else {
          msg() << MSG::INFO << "Muon rejected with isoCalo = " << isoCalo << endreq;
        }
        //=============================

        if (msgLvl() <= MSG::INFO) {
            msg() << MSG::INFO << " REGTEST usealgo / eta / SumEtCone / SumPtCone: "
                  // << " / " << usealgo
                  << " / " << etamu
                  << " / " << m_SumEtCone
                  << " / " << m_SumPtCone
                  << " / Muon Isolation Hypotesis is " << (result ? "true" : "false")
                  << endreq;
        }

        if (isIsolatedCalo && isIsolatedID) {
            HLT::markPassing(passBits, (*isoit), isocont); // set bit for this muon in TrigPassBits mask
        }//muon passed
    }//loop over isolation objects

    //store result
    pass = result;

    // store TrigPassBits result
    if ( attachBits(outputTE, passBits) != HLT::OK ) {
        msg() << MSG::ERROR << "Could not store TrigPassBits! " << endreq;
    }


    //    //absolute cut
    //    if(m_abscut) {
    //      if(m_ptcone02_cut > 0.0) {
    // if( (*isoit)->sumTrkPtCone02() >= m_ptcone02_cut ) goodmu=false;
    //      }
    //      if(m_ptcone03_cut > 0.0) {
    // if( (*isoit)->sumTrkPtCone03() >= m_ptcone03_cut ) goodmu=false;
    //      }

    //      if(debug) {
    // msg(MSG::INFO) << "Muon with pT cone 0.2 = " << (*isoit)->sumTrkPtCone02()
    //      << ", pT cone 0.3 = " << (*isoit)->sumTrkPtCone03()
    //      << " so result for this muon is " << (goodmu?"true":"false") << endreq;
    //      }
    //    }//absolute cut
    //    else { //relative cut

    //      //pointer for the asociated muon track
    //      const TrigMuonEFInfoTrack* infotrk = 0;

    //      // get the info object
    //      infotrk = (*isoit)->getEFMuonInfoTrack();

    //      if(!infotrk) {
    // msg(MSG::WARNING) << "TrigMuonEFIsolation object has no attached TrigMuonEFInfoTrack"
    //        << ". Ignoring this muon."
    //        << "Please contact muon trigger experts." << endreq;
    // continue;
    //      }//valid InfoTrack
    //      double mupt=-1.0;
    //      if(infotrk->hasCombinedTrack()) {
    // mupt = infotrk->CombinedTrack()->pt();
    // if(debug) msg(MSG::INFO) << "Got combined muon, pt = " << mupt << endreq;
    //      }//combined
    //      else {
    // if(infotrk->hasExtrapolatedTrack()) {
    //   mupt = infotrk->ExtrapolatedTrack()->pt();
    //   if(debug) msg(MSG::INFO) << "Got extrapolated muon, pt = " << mupt << endreq;
    // }// extrapolated
    // else {
    //   if(infotrk->hasSpectrometerTrack()) {
    //     mupt = infotrk->SpectrometerTrack()->pt();
    //     if(debug) msg(MSG::INFO) << "Got spectrometer muon, pt = " << mupt << endreq;
    //   }
    // }
    //      }
    //      if(mupt < 0) {
    // msg(MSG::WARNING) << "Invalid muon pT = " << mupt
    //        << "Ignoring this muon" << endreq;
    // continue;
    //      }

    //      // now we can make the cut(s)
    //      if(m_ptcone02_cut > 0.0) {
    // if( (*isoit)->sumTrkPtCone02()/mupt >= m_ptcone02_cut ) goodmu=false;
    //      }
    //      if(m_ptcone03_cut > 0.0) {
    // if( (*isoit)->sumTrkPtCone03()/mupt >= m_ptcone03_cut ) goodmu=false;
    //      }

    //      if(debug) {
    // msg(MSG::INFO) << "Muon with pT cone 0.2 / pt = " << (*isoit)->sumTrkPtCone02()/mupt
    //      << ", pT cone 0.3 / pt = " << (*isoit)->sumTrkPtCone03()/mupt
    //      << " so result for this muon is " << (goodmu?"true":"false") << endreq;
    //      }

    //    }//relative cut


    //    if(goodmu) {
    //      HLT::markPassing(passBits, (*isoit), isocont); // set bit for this muon in TrigPassBits mask
    //      result=true;
    //    }//muon passed
    //  }//loop over isolation objects

    //  if(debug) {
    //    msg(MSG::INFO) << "Algo result = " << (result?"true":"false") << endreq;
    //  }

    //  pass = result;

    //  // store TrigPassBits result
    //  if ( attachBits(outputTE, passBits) != HLT::OK ) {
    //    msg() << MSG::ERROR << "Could not store TrigPassBits! " << endreq;
    //  }

    return HLT::OK;
}//hltExecute
