/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration 
*/

#ifndef HTTRAWTOLOGICALHITSTOOL_H
#define HTTRAWTOLOGICALHITSTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTConfTools/IHTTEventSelectionSvc.h"


// Forward declaration
class ITrigHTTMappingSvc;
class HTTEventInputHeader;
class HTTLogicalEventInputHeader;


class HTTRawToLogicalHitsTool : public AthAlgTool
{
 public:

  HTTRawToLogicalHitsTool(const std::string&, const std::string&, const IInterface*);
  virtual  ~HTTRawToLogicalHitsTool() = default;
  virtual StatusCode initialize() override;

  StatusCode convert(unsigned stage, const HTTEventInputHeader& header, 
                                      HTTLogicalEventInputHeader& logicheader);
  StatusCode getUnmapped(std::vector<HTTHit>& missing_hits);

  const HTTPlaneMap* getPlaneMap_1st();


private:
 
  // JO configuration
  ServiceHandle<ITrigHTTMappingSvc>     m_HTTMapping {this, "TrigHTTMappingSvc", "TrigHTTMappingSvc"};
  ServiceHandle<IHTTEventSelectionSvc>  m_EvtSel     {this, "HTTEventSelectionSvc", "HTTEventSelectionSvc"};
  IntegerProperty                       m_saveOptional {this, "SaveOptional", 2, "flag to enable the truth/offline tracking save =0 no optional saved, =1 saved in region, =2 save all "};
  IntegerArrayProperty                  m_towersToMap  {this, "TowersToMap", {}, "Which Towers to map, goes from 0 to 96!"};

  // internal members
  std::vector<int> m_towers;
  std::vector<HTTHit> m_missing_hits;// vector to save hits not mapped, debugging only
  std::vector<int> m_missing_hit_codes; // for histograms used in debugging

};

#endif // HTTRAWTOLOGICALHITSTOOL_H
