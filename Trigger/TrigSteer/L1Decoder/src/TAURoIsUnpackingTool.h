/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_TAUROISUNPACKINGTOOL_H
#define L1DECODER_TAUROISUNPACKINGTOOL_H 1

#include "RoIsUnpackingToolBase.h"

#include <string>


class TAURoIsUnpackingTool : public RoIsUnpackingToolBase { 
public: 

  TAURoIsUnpackingTool( const std::string& type,
                        const std::string& name, 
                        const IInterface* parent );

  virtual StatusCode updateConfiguration() override { return StatusCode::SUCCESS; }

  virtual StatusCode initialize() override;

private: 

}; 


#endif //> !L1DECODER_JROISUNPACKINGTOOL_H
