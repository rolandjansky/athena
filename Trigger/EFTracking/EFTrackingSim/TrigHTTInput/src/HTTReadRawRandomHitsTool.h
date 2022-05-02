/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTREADRAWRANDOMHITSTOOL_H
#define HTTREADRAWRANDOMHITSTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHTTInput/IHTTEventInputHeaderTool.h"

#include "TFile.h"
#include "TTree.h"



class HTTReadRawRandomHitsTool : public extends<AthAlgTool, IHTTEventInputHeaderTool>
{
  public:

  HTTReadRawRandomHitsTool(const std::string&, const std::string&, const IInterface*);
  virtual ~HTTReadRawRandomHitsTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode readData(HTTEventInputHeader* header, bool &last) override;
  virtual StatusCode writeData(HTTEventInputHeader* header) override; 
  virtual StatusCode finalize() override;
  
  StatusCode readData(HTTEventInputHeader* header, bool &last, bool doReset);

  private:
  // JO configuration    
  StringProperty m_inpath {this, "InFileName", "httsim_smartwrapper.root", "input path"};

  // Internal pointers       
  unsigned int m_nEntries = 0U;
  unsigned m_entry = 0;
};

#endif // HTTREADRAWRANDOMHINPUTTOOL_H
