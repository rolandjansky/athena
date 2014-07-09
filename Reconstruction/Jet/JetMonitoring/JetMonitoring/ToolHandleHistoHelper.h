//  -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_TOOLHANDLEHISTOHELPER_H
#define JETMONITORING_TOOLHANDLEHISTOHELPER_H
///////////////////////////////////////////
///
/// \class HistoRetriever
/// A helper class to instantiate TH1 objects as defined in a ToolHandleArray<HistoDefinitionTool>
///
/// The HistoRetriever is instantiated with a ToolHandleArray<HistoDefinitionTool>.
/// Then it can be asked for a histogram by a name.
///  If the name corresponds to a HistoDefinitionTool it holds, it uses this tool to build the requested TH1
///  if no name corresponds, returns NULL
///////////////////////////////////////////
#include "GaudiKernel/ToolHandle.h"
#include "JetMonitoring/HistoDefinitionTool.h"

namespace ToolHandleHistoHelper {



  struct HistoRetriever {

    std::string afterlastdot(const std::string s){
      size_t i = s.find_last_of(".");
      return s.substr( i+1, s.size() );
    }
    
    HistoRetriever(ToolHandleArray<HistoDefinitionTool>& h) : histos(h) {}

    HistoDefinitionTool* retrieveTool( const std::string & n){
      for( auto toolH : histos){
        //std::cout << "   xxxx compare   "<< afterlastdot( toolH->name() ) << " to "<< n <<std::endl; 
        if( afterlastdot(toolH->name()) == n ) return toolH.operator->();
      }
      return NULL;
    }

    template<typename T >
    T* build(const std::string & n){
      std::string name = "hdef_"; name+=n;
      HistoDefinitionTool* tool = retrieveTool(name);
      if (!tool ) return NULL;
      return tool->build<T>();
  }
    

    ToolHandleArray<HistoDefinitionTool>& histos;
  };


}


#endif
