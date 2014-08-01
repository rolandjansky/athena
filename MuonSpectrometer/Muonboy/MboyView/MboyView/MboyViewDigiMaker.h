/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include <map>
class IMboyPrepDataToDigiStoreTool;
class IMboyGetPrepDataFromEventTool;
class MuonRecDigiStoreAth;
class Identifier;

namespace Muon {
  class MdtPrepData;
}
namespace Trk {
  class PrepRawData;
}

  /**
   @class MboyViewDigiMaker

   This class is a small algorithm that reads the PrepRawData and sets
   in the transient data store a MuonRecDigiStoreTDS.

   Optionally it can also determine an approximate "RS-line" for the
   MDT hits, without the need for a properly configured RT service. It
   does this by determining if the MDT hit belongs to a segment, and
   if it is the case, the radius measurement is set to the one saved
   in the segment. If not found, radius is optionnaly set to 7.5 mm, or hit is
   discarded.

   This last feature is only useful to produce Persint dumps of data
   in an ESD file (for visualization, there is no need for a precise
   RS-line)

  @author samusog@cern.ch
  
  */

class MboyViewDigiMaker : public AthAlgorithm {
public:
  MboyViewDigiMaker(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MboyViewDigiMaker();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  StatusCode makeRadiusMap(std::multimap<Identifier,double>& radiusMap);
  StatusCode makeRadiusPrdMap (std::map<const Muon::MdtPrepData*, double>& radiusPrdMap,
      const std::vector<const Trk::PrepRawData*>& prepData,
      const std::multimap<Identifier,double>& radiusMap);
  StatusCode removeUnfoundMdtHits (std::vector<const Trk::PrepRawData*>& prepData,
      const std::map<const Muon::MdtPrepData*, double>& radiusPrdMap);
  StatusCode makeRsLine(MuonRecDigiStoreAth* pMuonRecDigiStoreAth,
      const std::map<const Muon::MdtPrepData*, double> radiusPrdMap);

  std::string m_DigiStoreLocation; //!< Location of the MuonRecDigiStoreTDS
  bool m_ApproximateRsLine; //!< Try to make an RS line from the segments without activating RT service
  bool m_SkipUnfoundHits; //!< Do not output an RS line for the hits which are not associated to a segment
  std::string m_segmentCollection; //!< The name of the segment collection from which to obtain approximate RT
  const MdtIdHelper *m_mdtId;

  ToolHandle<IMboyGetPrepDataFromEventTool> p_IMboyGetPrepDataFromEventTool;
  ToolHandle<IMboyPrepDataToDigiStoreTool> p_IMboyPrepDataToDigiStoreTool ;
};
