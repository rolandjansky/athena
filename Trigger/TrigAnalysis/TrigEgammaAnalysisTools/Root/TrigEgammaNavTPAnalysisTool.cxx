/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************
 * AsgTool: TrigEgammaNavTPAnalysisTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Contributors:
 * Date: Feb 2015
 * Description:
 *      Derived class for measuring the trigger efficiency.
 *      Produces trigger efficiency distributions at each trigger level
 **********************************************************************/
#include "TrigEgammaAnalysisTools/TrigEgammaNavTPAnalysisTool.h"



TrigEgammaNavTPAnalysisTool::
TrigEgammaNavTPAnalysisTool( const std::string& myname )
: TrigEgammaNavTPBaseTool(myname) 
{}


StatusCode TrigEgammaNavTPAnalysisTool::childInitialize(){

    TrigEgammaNavTPBaseTool::childInitialize();
    ATH_MSG_INFO("Now configuring chains for analysis: " << name() );
    m_trigList = m_trigInputList;
    for(const auto trigName:m_trigInputList){
        m_trigList.push_back(trigName);
        setTrigInfo(trigName);
    }
    return StatusCode::SUCCESS;
}


StatusCode TrigEgammaNavTPAnalysisTool::childFinalize(){
    TrigEgammaNavTPBaseTool::childFinalize();
    return StatusCode::SUCCESS;
}


StatusCode TrigEgammaNavTPAnalysisTool::childExecute( const EventContext& ctx ) const  {

   
    //auto mon = Monitored::GroupAccumulator();
    //auto monGroup = mon.findGroup(m_dir+"/Expert/Event");


    //mon.fill(monGroup , m_anatype+"_CutCounter", "Events", 1)
    // Event Wise Selection (independent of the required signatures)
    //mon.fill(monGroup, m_anatype+"_CutCounter", "EventWise",1);
    
   
    std::vector<const xAOD::Electron*> probes;

    // Select TP Pairs
    ATH_MSG_DEBUG("Execute TP selection");
    if( !TrigEgammaNavTPBaseTool::executeTandP(ctx, probes) ){
        ATH_MSG_WARNING("Tag and Probe event failed.");
        return StatusCode::SUCCESS;
    }


    // Check HLTResult
    if(tdt()->ExperimentalAndExpertMethods()->isHLTTruncated()){
        ATH_MSG_WARNING("HLTResult truncated, skip trigger analysis");
        return StatusCode::SUCCESS;
    }



    for(unsigned int ilist = 0; ilist != m_trigList.size(); ilist++) {
        
        //auto monTrig = Monitored::GroupAccumulator();

        std::string probeTrigger = m_trigList.at(ilist);
        //const char * cprobeTrigger = m_trigList.at(ilist).c_str();
        /** Pair objects used in executeTool **/
        std::vector<std::pair<const xAOD::Egamma*, const TrigEgammaMatchingUtils::Element>> pairObjs;

        ATH_MSG_DEBUG("Start Chain Analysis ============================= " << probeTrigger);
        
        //if(tdt()->isPassed(probeTrigger))  
        //  monTrig.fill(monGroup, m_anatype+"_trigger_counts", cprobeTrigger,1);

        const TrigInfo info = getTrigInfo(probeTrigger);
        std::string trigName=probeTrigger;

        ATH_MSG_DEBUG("Trigger " << probeTrigger << " pidword " << info.trigPidDecorator << " threshold " << info.trigThrHLT);
        TrigEgammaNavTPBaseTool::matchObjects(trigName, probes, pairObjs);

        // Just for counting
        ATH_MSG_DEBUG("Probes " << probes.size() << " Pairs " << pairObjs.size() );

        
 
        for( auto& tool : m_tools) {
            if(tool->toolExecute(ctx, m_dir+"/Expert", info, pairObjs).isFailure())
                ATH_MSG_DEBUG("TE Tool Fails");// Requires offline match
        }

        if(isPrescaled(probeTrigger)){
            ATH_MSG_DEBUG(probeTrigger << " prescaled, skipping efficiency");
            continue; //Account for L1 and HLT prescale, discard event
        }

        for(unsigned int i=0;i<pairObjs.size();i++){

            //auto monPairObj = Monitored::GroupAccumulator();
            const xAOD::Electron* offEl = static_cast<const xAOD::Electron *> (pairObjs[i].first);
            float et = getEt(offEl)/1e3;
            if(et < info.trigThrHLT-5.0) continue; 
            if(!offEl->auxdecor<bool>(info.trigPidDecorator)) continue; 
            //asg::AcceptData acceptData = setAccept(pairObjs[i].second,info); //Sets the trigger accepts
            //if(et > info.trigThrHLT + 1.0)
            //    monPairObj.fill( monGroup, m_anatype+"_nProbes", cprobeTrigger);
            
            /*
            if ( pairObj[i].second.isValid() ) 
            {

                if(et > info.trigThrHLT + 1.0){
                    monPairObj.fill( monGroup, m_anatype+"_EffL1"    , cprobeTrigger,acceptData.getCutResult("L1Calo"));
                    monPairObj.fill( monGroup, m_anatype+"_EffL2Calo", cprobeTrigger,acceptData.getCutResult("L2Calo"));
                    monPairObj.fill( monGroup, m_anatype+"_EffL2"    , cprobeTrigger,acceptData.getCutResult("L2"));
                    monPairObj.fill( monGroup, m_anatype+"_EffEFCalo", cprobeTrigger,acceptData.getCutResult("EFCalo"));
                    monPairObj.fill( monGroup, m_anatype+"_EffHLT"   , cprobeTrigger,acceptData.getCutResult("HLT"));
                    
                    if( acceptData.getCutResult("L1Calo")){
                        monPairObj.fill( monGroup, m_anatype+"_nProbesL1", cprobeTrigger);
                    }
                    if( acceptData.getCutResult("L2Calo") ){
                        monPairObj.fill( monGroup, m_anatype+"_nProbesL2Calo", cprobeTrigger);
                    }
                    if( acceptData.getCutResult("L2") ){
                        monPairObj.fill( monGroup, m_anatype+"_nProbesL2", cprobeTrigger);
                    }
                    if( acceptData.getCutResult("EFCalo") ){
                        monPairObj.fill( monGroup, m_anatype+"_nProbesEFCalo", cprobeTrigger);
                    }
                    if( acceptData.getCutResult("HLT") ){
                        monPairObj.fill( monGroup, m_anatype+"_nProbesHLT", cprobeTrigger);
                    }
                }
            } // Features
            // Fill TProfile for no feature found (means no match)
            else {
                monPairObj.fill( monGroup, m_anatype+"_EffL1", cprobeTrigger, false);
                monPairObj.fill( monGroup, m_anatype+"_EffL2Calo",cprobeTrigger,false);
                monPairObj.fill( monGroup, m_anatype+"_EffL2",cprobeTrigger,false);
                monPairObj.fill( monGroup, m_anatype+"_EffEFCalo",cprobeTrigger,false);
                monPairObj.fill( monGroup, m_anatype+"_EffHLT",cprobeTrigger,false);
            }*/
            

        } // End loop over electrons
        
    } // End loop over trigger list
    
    //mon.fill( monGroup, m_anatype+"_CutCounter", "Success");
    
    return StatusCode::SUCCESS;
}

