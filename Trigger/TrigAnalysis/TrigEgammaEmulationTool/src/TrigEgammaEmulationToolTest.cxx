
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// TrigEgammaMatchingToolTest.cxx 

#include "TrigEgammaEmulationToolTest.h"



//**********************************************************************

TrigEgammaEmulationToolTest::TrigEgammaEmulationToolTest(const std::string& name, ISvcLocator* pSvcLocator )
    : AthMonitorAlgorithm( name, pSvcLocator ),
    m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool")
{
    declareProperty("MatchTool"    , m_matchTool     );
    declareProperty("EmulatorTool" , m_emulatorTool );
}

//**********************************************************************

TrigEgammaEmulationToolTest::~TrigEgammaEmulationToolTest() { }

//**********************************************************************

StatusCode TrigEgammaEmulationToolTest::initialize() {

  ATH_MSG_DEBUG("Initializing " << name() << "...");
  ATH_CHECK(AthMonitorAlgorithm::initialize());
  ATH_MSG_DEBUG("Retrieving tools...");

  ATH_CHECK(m_trigdec.retrieve());
  ATH_CHECK(m_offElectronKey.initialize());
  ATH_CHECK(m_offPhotonKey.initialize());
  ATH_CHECK(m_matchTool.retrieve());
  ATH_CHECK(m_emulatorTool.retrieve());

  m_accept.addCut("L1Calo","");
  m_accept.addCut("L2Calo","");
  m_accept.addCut("L2","");
  m_accept.addCut("EFCalo","");
  m_accept.addCut("EFTrack","");
  m_accept.addCut("HLT","");


  return StatusCode::SUCCESS;
}


//**********************************************************************


StatusCode TrigEgammaEmulationToolTest::fillHistograms( const EventContext &ctx ) const
{
  ATH_MSG_DEBUG("=============== Start Event ===============");

  SG::ReadHandle<xAOD::EventInfo> eventInfo = GetEventInfo (ctx);
  if( !eventInfo.isValid() ){
    ATH_MSG_WARNING("Failed to retrieve EventInfo");
    return StatusCode::SUCCESS;
  }
  if (eventInfo->errorState(xAOD::EventInfo::LAr) == xAOD::EventInfo::Error) {
      ATH_MSG_WARNING("Event not passing LAr");
      return StatusCode::SUCCESS;
  }

  SG::ReadHandle<xAOD::ElectronContainer> offElectrons(m_offElectronKey, ctx);
  if(!offElectrons.isValid())
  {
    ATH_MSG_WARNING("Failed to retrieve offline Electrons ");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG( "Electron size is " << offElectrons->size() );

  for (const auto &trigger : m_electronTriggerList) 
  {
    auto info = getInfo(trigger);
    ATH_MSG_DEBUG(trigger);
    for (const auto el : *offElectrons) 
    {


      ATH_MSG_DEBUG( "Offline Electron cluster object is "<< el->caloCluster());
      const TrigCompositeUtils::Decision *dec=nullptr;
      //m_matchTool->match(el,trigger,dec, TrigDefs::includeFailedDecisions);
      m_matchTool->match(el,trigger,dec, TrigDefs::Physics);

      if (!dec) {
        ATH_MSG_DEBUG("e/g matching fail. skip...");
        continue;
      }

      auto accept = setAccept(dec, info);
      bool valid=false;
      auto emu_accept = m_emulatorTool->emulate( dec, info.trigger, valid );

      ATH_MSG_DEBUG( "trigger : " << info.trigger << " (tdt/emu)");
      ATH_MSG_DEBUG( "L1Calo  : " << accept.getCutResult(0) << " / " << emu_accept.getCutResult(0));
      ATH_MSG_DEBUG( "L2Calo  : " << accept.getCutResult(1) << " / " << emu_accept.getCutResult(1));
      ATH_MSG_DEBUG( "L2      : " << accept.getCutResult(2) << " / " << emu_accept.getCutResult(2));
      ATH_MSG_DEBUG( "EFCalo  : " << accept.getCutResult(3) << " / " << emu_accept.getCutResult(3));
      ATH_MSG_DEBUG( "EFTrack : " << accept.getCutResult(4) << " / " << emu_accept.getCutResult(4));
      ATH_MSG_DEBUG( "HLT     : " << accept.getCutResult(5) << " / " << emu_accept.getCutResult(5));

      auto monGroup = getGroup( trigger );

      
      // Fill trigger decision tool steps
      if (accept.getCutResult(0)){
        fillLabel(monGroup, "decisions", "L1Calo");
        if (accept.getCutResult(1)){
          fillLabel(monGroup, "decisions", "L2Calo");
          if(accept.getCutResult(2)){
            fillLabel(monGroup, "decisions", "L2");
            if(accept.getCutResult(3)){
              fillLabel(monGroup, "decisions", "EFCalo");
              if(accept.getCutResult(5)){
                fillLabel(monGroup, "decisions", "HLT");
              }
            }
          }
        }
      }

      // Fill emulation
      if (emu_accept.getCutResult(0)){
        fillLabel(monGroup, "emulations", "L1Calo");
        if (emu_accept.getCutResult(1)){
          fillLabel(monGroup, "emulations", "L2Calo");
          if(emu_accept.getCutResult(2)){
            fillLabel(monGroup, "emulations", "L2");
            if(emu_accept.getCutResult(3)){
              fillLabel(monGroup, "emulations", "EFCalo");
              if(emu_accept.getCutResult(5)){
                fillLabel(monGroup, "emulations", "HLT");
              }
            }
          }
        }
      }




    }
  }


  ATH_MSG_DEBUG("================= End Event =================");

  return StatusCode::SUCCESS;
}



//**********************************************************************



asg::AcceptData TrigEgammaEmulationToolTest::setAccept( const TrigCompositeUtils::Decision *dec, 
                                                        const TrigEgammaEmulationToolTest::TrigInfo info) const 
{
    
    ATH_MSG_DEBUG("setAccept");
    unsigned int condition=TrigDefs::includeFailedDecisions;

    asg::AcceptData acceptData (&m_accept);
    bool passedL1Calo=false;
    bool passedL2Calo=false;
    bool passedEFCalo=false;
    bool passedL2=false;
    bool passedEFTrk=false;
    bool passedEF=false;
    
    if (dec) {

        auto trigger = info.trigger; 
        // Step 1
        passedL1Calo = m_matchTool->ancestorPassed<TrigRoiDescriptorCollection>( dec , trigger , "initialRois", condition);

        if( passedL1Calo ){ // HLT item get full decision
            // Step 2
            passedL2Calo = m_matchTool->ancestorPassed<xAOD::TrigEMClusterContainer>(dec, trigger, m_matchTool->key("FastCalo"), condition);  
          
            if(passedL2Calo){

                // Step 3
                if(info.type == "electron"){
                    std::string key = m_matchTool->key("FastElectrons");
                    if(info.lrt)  key = m_matchTool->key("FastElectrons_LRT");
                    passedL2 = m_matchTool->ancestorPassed<xAOD::TrigElectronContainer>(dec, trigger, key, condition);
                }else if(info.type == "photon"){
                    passedL2 = m_matchTool->ancestorPassed<xAOD::TrigPhotonContainer>(dec, trigger, m_matchTool->key("FastPhotons"), condition);
                }

                if(passedL2){


                    // Step 4
                    std::string key = m_matchTool->key("PrecisionCalo");
                    if(info.lrt)  key = m_matchTool->key("PrecisionCalo_LRT");
                    passedEFCalo = m_matchTool->ancestorPassed<xAOD::CaloClusterContainer>(dec, trigger, key, condition);

                    if(passedEFCalo){

                        // Step 5
                        passedEFTrk=true;// Assume true for photons

                        // Step 6
                        if(info.type == "electron"){

                            if( info.etcut || info.idperf){// etcut or idperf
                                passedEF = true; // since we dont run the preciseElectron step
                            }else{
                                std::string key = m_matchTool->key("Electrons");
                                if(info.lrt)  key = m_matchTool->key("Electrons_LRT");
                                if(info.gsf)  key = m_matchTool->key("Electrons_GSF");
                                passedEF = m_matchTool->ancestorPassed<xAOD::ElectronContainer>(dec, trigger, key, condition);
                            }
   
                        }else if(info.type == "photon"){
                            if (info.etcut){
                                passedEF = true; // since we dont run the precisePhoton step
                            }else{
                                passedEF = m_matchTool->ancestorPassed<xAOD::PhotonContainer>(dec, trigger, m_matchTool->key("Photons"), condition);
                            }
                        }
                    } // EFCalo
                }// L2
            }// L2Calo
        }// L2Calo

    }

    acceptData.setCutResult("L1Calo",passedL1Calo);
    acceptData.setCutResult("L2Calo",passedL2Calo);
    acceptData.setCutResult("L2",passedL2);
    acceptData.setCutResult("EFCalo",passedEFCalo);
    acceptData.setCutResult("EFTrack",passedEFTrk);
    acceptData.setCutResult("HLT",passedEF);
    
    ATH_MSG_DEBUG("Accept results:");
    ATH_MSG_DEBUG("L1: "<< passedL1Calo);
    ATH_MSG_DEBUG("L2Calo: " << passedL2Calo);
    ATH_MSG_DEBUG("L2: "<< passedL2);
    ATH_MSG_DEBUG("EFCalo: "<< passedEFCalo);
    ATH_MSG_DEBUG("HLT: "<<passedEF);

    return acceptData;
}

//**********************************************************************


TrigEgammaEmulationToolTest::TrigInfo TrigEgammaEmulationToolTest::getInfo( std::string trigger ) const
{
    std::string type = "electron"; // default
    std::vector<std::string> strs;
    boost::split(strs,trigger,boost::is_any_of("_"));
    if(boost::contains(trigger,"HLT_e")) type = "electron";
    else if(boost::contains(trigger,"HLT_g")) type = "photon";
    bool idperf = boost::contains(trigger, "idperf"); 
    bool etcut = boost::contains(trigger, "etcut"); 
    bool lrt = boost::contains(trigger, "lrt"); 
    bool gsf = boost::contains(trigger, "gsf");
    std::string str_thr = strs.at(1);
    str_thr.erase(0, 1);
    float et = atof(str_thr.c_str());

    ATH_MSG_DEBUG("type   : " << type );
    ATH_MSG_DEBUG("idperf : " << (idperf?"Yes":"No"));
    ATH_MSG_DEBUG("etcut  : " << (etcut?"Yes":"No"));
    ATH_MSG_DEBUG("gsf    : " << (gsf?"Yes":"No"));
    ATH_MSG_DEBUG("lrt    : " << (lrt?"Yes":"No"));



    return TrigEgammaEmulationToolTest::TrigInfo{trigger,type,et,etcut,idperf,gsf,lrt};
}

//**********************************************************************


void TrigEgammaEmulationToolTest::fillLabel( const ToolHandle<GenericMonitoringTool>& groupHandle, 
                                                    const std::string &histname, 
                                                    const std::string &label ) const
{
  auto mon = Monitored::Scalar<std::string>( histname, label );
  fill( groupHandle, mon );
}


