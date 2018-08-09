/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBEVENTSTREAMER_H
#define TBREC_TBEVENTSTREAMER_H
/*! \brief event streamer algorithm
 *
 * The \c TBEventStreamer algorithm invokes a sequence of \c TBEventStreamer 
 * tools. This sequence is interrupted if a tool returns 
 * \c StatusCode::FAILURE. The algorithm keeps scores of success and failures.
 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <vector>
#include <map>

class TBEventStreamerTool;
class StoreGateSvc;

class TBEventStreamer : public AthAlgorithm
{
 public:
  /*! \brief Algorithm constructor */
  TBEventStreamer(const std::string& name, ISvcLocator* pService);
  virtual ~TBEventStreamer();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 protected:

//  std::vector<std::string> m_toolNames;
//  std::vector<TBEventStreamerTool*> m_tools;
  ToolHandleArray<TBEventStreamerTool> m_tools;

  std::map<ToolHandle<TBEventStreamerTool>, unsigned int> m_acceptCounter;
  std::map<ToolHandle<TBEventStreamerTool>, unsigned int> m_rejectCounter;
  std::map<ToolHandle<TBEventStreamerTool>, unsigned int> m_invokeCounter;

};
#endif
