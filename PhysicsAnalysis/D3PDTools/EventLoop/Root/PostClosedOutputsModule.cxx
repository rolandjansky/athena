/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Romain Bouquet


#include <EventLoop/PostClosedOutputsModule.h>
#include <EventLoop/Job.h>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/ModuleData.h>
#include <EventLoop/Worker.h>
#include <RootCoreUtils/ShellExec.h>


//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    ::StatusCode PostClosedOutputsModule ::
    postFileClose (ModuleData& data)
    { 
      using namespace msgEventLoop;
      
      // get executable if user provided one 
      std::string postCloseExec = data.m_worker->metaData()->castString (Job::optWorkerPostClosedOutputsExecutable, ""); 

      if (!postCloseExec.size()){
        // user did not provide an excutable to be called after all outputs are closed
        ANA_MSG_DEBUG ("PostClosedOutputsModule: no additionnal checks/actions performed after all outputs are closed");
        return ::StatusCode::SUCCESS;
      }

      // user provided an executable to be used 
      // loop over all outputs
      for (auto & output : data.m_outputs){
        std::string outputLabel = output.first; 
        std::string outputFileName = output.second.finalFileName();
        
        if (!outputFileName.size()){
          ANA_MSG_WARNING ("PostClosedOutputsModule: empty outputFileName skipping outputLabel = "+outputLabel);
          continue;
        }

        // for instance execute a script checking that root files are not corrupted
        std::string cmdLineToExecute = postCloseExec+" "+outputLabel+" "+outputFileName;
        
        ANA_MSG_INFO ("PostClosedOutputsModule: execute command line = '" << cmdLineToExecute << "'");
        RCU::Shell::exec (cmdLineToExecute.c_str());
      }
      return ::StatusCode::SUCCESS;
    }
  }
}