/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
Class to write useful tag bits for cosmic ray commissioning
(like if there is an ID track in this event...)
Jamie Boyd 21 Jan 2008 (Jamie.Boyd@cern.ch)
*/


#ifndef COMMISSIONREC_RAWINFOSUMMARYFORTAGWRITER
#define COMMISSIONREC_RAWINFOSUMMARYFORTAGWRITER


#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

#include <string>
#include <vector>
#include <map>
#include "TrkSegment/SegmentCollection.h"

class TileTBID;
class StoreGateSvc;
class MdtIdHelper;
class IMagFieldAthenaSvc;

namespace Trk
{
class Track;
}

namespace coral
{
  class AttributeListSpecification;
}

namespace MuonCalib {
  class IIdToFixedIdTool;
}

namespace Muon {
    class MuonIdHelperTool;
}

class RawInfoSummaryForTagWriter : public AthAlgorithm
{

 public:

  RawInfoSummaryForTagWriter(const std::string& name, ISvcLocator* pSvcLocator);
  ~RawInfoSummaryForTagWriter();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  StatusCode magneticFieldInit(IOVSVC_CALLBACK_ARGS);

  StoreGateSvc* m_storeGate;

  /** member variables for algorithm properties: */
  std::string m_sgKeyIDtrack; //!< Name of the Track Collection to read in

  std::string m_sgKeyTileMuonFitterNew;  //!< name of the ComTime object in StoreGate 
  std::string m_TRT_DriftCircleName;  //!< name of the TRT DC key


  bool m_doTileMuonFitter; //!< do TileMuonFitter tag
  bool m_doClusterSums;//!< turn on/off the cluster sum in the tag (as it crashes if Tile is off)
  
  std::string m_mbtsContainerName;
  float m_mbts_threshold;


  std::string m_vertexCollectionName;

  const TileTBID* m_tileTBID;
       
  coral::AttributeListSpecification* m_attribListSpec;

  ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle;
};

#endif
