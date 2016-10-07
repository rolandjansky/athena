/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPREWEIGHTINGPROVIDER
#define PILEUPREWEIGHTINGPROVIDER

#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODEventInfo/EventInfoAuxContainer.h"

#include "PATInterfaces/SystematicRegistry.h"

namespace CP {

class PileupReweightingProvider : public AthAlgorithm {

     public:
      PileupReweightingProvider( const std::string& name, ISvcLocator* svcloc) : AthAlgorithm(name,svcloc),m_tool("CP::PileupReweightingTool/auto") {
         declareProperty("Tool",m_tool,"The configured PileupReweightingTool");
         declareProperty("Input",m_inputKey="","Specify a specific EventInfo object");
         declareProperty("Output",m_outputKey="","Specify an output EventInfo object. If differs from input, will create a clone of EventInfo and decorate that");
         declareProperty("ConfigOutputStream",m_configStream="","Specify the stream to output config file to");
      }

      ~PileupReweightingProvider() { }

      virtual StatusCode initialize() { 
         CHECK( m_tool.retrieve() ); 

         IProperty* myTool = dynamic_cast<IProperty*>(&*m_tool);
         if (!myTool)
           return StatusCode::FAILURE;
         CHECK( myTool->setProperty("ConfigOutputStream",m_configStream) );

         //get the list of systematics 
         CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
         m_allSysts = registry.recommendedSystematics();

         if(m_configStream!="") ATH_MSG_INFO("Now running config file making .... please be patient! ... ");

         return StatusCode::SUCCESS; 
      }
      virtual StatusCode execute() { 
            const xAOD::EventInfo* evtInfo =0;

            if(m_inputKey.length()>0) ATH_CHECK(evtStore()->retrieve(evtInfo,m_inputKey));
            else {
               #ifdef XAOD_STANDALONE
                  ATH_CHECK( evtStore()->retrieve(evtInfo,"") ); //apparently TEvent can't do keyless retrieves!!??
               #else
                  ATH_CHECK(evtStore()->retrieve(evtInfo));
               #endif
            }

            //do we need to make a copy??
            if(m_inputKey!=m_outputKey && m_outputKey!="") {
               xAOD::EventInfo* evtInfoCopy = new xAOD::EventInfo( *evtInfo );
               xAOD::EventInfoAuxContainer* aux = new xAOD::EventInfoAuxContainer;
               evtInfoCopy->setStore(aux);
               ATH_CHECK( evtStore()->record( evtInfoCopy , m_outputKey ) );
               ATH_CHECK( evtStore()->record( aux , m_outputKey+"Aux." ) );
               evtInfo = evtInfoCopy;
            }

            CHECK( m_tool->apply(*evtInfo) );


            //here's an example of systematic variations 
            for(auto& syst : m_allSysts) {
               ATH_MSG_VERBOSE("Doing systematic : " << syst.name());
               if(! m_tool->isAffectedBySystematic( syst )) continue;
               CP::SystematicSet tmp; tmp.insert( syst );
               if( m_tool->applySystematicVariation( tmp ) != CP::SystematicCode::Ok ) continue;
               CHECK( m_tool->apply(*evtInfo) );
            }
            //make sure we leave the tool in the nominal mode 
            if( m_tool->applySystematicVariation( CP::SystematicSet() ) != CP::SystematicCode::Ok ) return StatusCode::FAILURE; 

            return StatusCode::SUCCESS; 
      }

   private:
      ToolHandle<IPileupReweightingTool> m_tool;

      std::string m_inputKey,m_outputKey,m_configStream;

      CP::SystematicSet m_allSysts;

}; 

} //end of CP namespace

#endif
