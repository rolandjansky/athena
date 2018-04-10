/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_HLTBYTESTREAMCREATOR_H
#define TRIGOUTPUTHANDLING_HLTBYTESTREAMCREATOR_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// OutputHandling includes
#include "TrigOutputHandling/IHLTOutputTool.h"



/**
 * @class $(klass)s
 * @brief 
 **/

class HLTByteStreamCreator
  : virtual public ::IHLTOutputTool,
            public ::AthAlgTool
{ 

 public: 

  HLTByteStreamCreator( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  virtual ~HLTByteStreamCreator(); 
  virtual StatusCode createOutput(const EventContext& context) const override;

  virtual StatusCode  initialize() override;
  virtual StatusCode  finalize() override;

 private: 

  HLTByteStreamCreator();

}; 


#endif //> !TRIGOUTPUTHANDLING_HLTBYTESTREAMCREATOR_H
