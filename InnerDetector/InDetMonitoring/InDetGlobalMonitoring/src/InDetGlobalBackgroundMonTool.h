/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef InDetGlobalBackgroundMonTool_H
#define InDetGlobalBackgroundMonTool_H

#include <string>
#include "GaudiKernel/StatusCode.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "InDetGlobalMotherMonTool.h"

class TH1I;
class TH1F;
class TH2I;
class TProfile;
class IInterface;
class StatusCode;

///Template monitoring tool derived from InDetGlobalMotherMonTool
class InDetGlobalBackgroundMonTool: public InDetGlobalMotherMonTool
{
public:
  ///Constructor
  InDetGlobalBackgroundMonTool(const std::string& type,
                               const std::string& name,
                               const IInterface* parent);

  ///Virtual destructor
  virtual ~InDetGlobalBackgroundMonTool() {}

  ///@name Book, fill and proc histograms
  ///@{

  ///@copydoc InDetGlobalMotherMonTool::bookHistograms()
  virtual StatusCode bookHistograms();

  ///@copydoc InDetGlobalMotherMonTool::fillHistograms()
  virtual StatusCode fillHistograms();

  ///@}
private:
  TProfile* m_EndCapDisks[2];
  ///The disks are grouped in pairs, with the disk 9 excluded
  TH1I* m_AsymDisks[4];
  TProfile* m_EndCapLB[2][4];
  TProfile* m_asymLB;
  TProfile* m_asymBCID;
  TProfile* m_hitBCID;
  ///Maps for the two beam directions for every disk
  TH2I* m_Mapping[2][18];
  TH1F* m_RadMap[2][18];
};

#endif // InDetGlobalBackgroundMonTool_H
