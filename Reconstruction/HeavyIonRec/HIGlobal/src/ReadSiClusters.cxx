/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIGlobal/ReadSiClusters.h"

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "StoreGate/DataHandle.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/SiClusterCollection.h"
#include "InDetPrepRawData/SiCluster.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include "InDetPrepRawData/SiWidth.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"


ReadSiClusters::ReadSiClusters(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),   
  m_nDet(1),
  m_eventNumber(0),
  m_numberOfEvents(0),
  m_ntuple(nullptr)
{  
  // Declare the properties
  declareProperty("SiClusterContainerName", 
                  m_SiClustersName="PixelClusters");
  declareProperty("NtupleDirName",
                  m_ntupleDirName="PixelClusters");
  declareProperty("ReadSiClusters",m_readSiClusters=true);
}

StatusCode ReadSiClusters::initialize() {
  ATH_MSG_DEBUG( "In initialize()" );
  if(m_readSiClusters) ATH_CHECK( BookSiNtuple() );
  return StatusCode::SUCCESS;
}

StatusCode ReadSiClusters::BookSiNtuple(){

  NTuple::Directory *p_directory=0;
  std::string dirName="/NTUPLES/FILE1";
  NTupleFilePtr ntupleFile(ntupleSvc(),dirName);
  dirName+="/"+m_ntupleDirName;
  if (0!=ntupleFile) {
    p_directory=ntupleSvc()->createDirectory(dirName);
    if (!p_directory) {
      ATH_MSG_FATAL( "Cannot create ntuple directory !" );
      return StatusCode::FAILURE;
    } else {}
  } else {
    ATH_MSG_FATAL( "Cannot create ntuple file !" );
    return StatusCode::FAILURE;
  }

  NTuplePtr ntuple(ntupleSvc(),dirName+"/50");
  if (0!=ntuple) {
    ATH_MSG_FATAL( "Ntuple already booked !" );
    return StatusCode::FAILURE;
  } else {}

  m_ntuple=ntupleSvc()->book(p_directory,50,
			     CLID_RowWiseTuple,m_ntupleDirName);
  if (!m_ntuple) {
    ATH_MSG_FATAL( "Cannot book ntuple !" );
    return StatusCode::FAILURE;
  } else {}
  
  m_ntuple->addItem("r",m_r);
  m_r=0;
  m_ntuple->addItem("phi",m_phi);
  m_phi=0;
  m_ntuple->addItem("z",m_z);
  m_z=0;
  m_ntuple->addItem("RowWidth",m_rowWidth);
  m_rowWidth=0;
  m_ntuple->addItem("ColWidth",m_colWidth);
  m_colWidth=0;
  m_ntuple->addItem("EtaWidth",m_etaWidth);
  m_etaWidth=0;
  m_ntuple->addItem("PhiWidth",m_phiWidth);
  m_phiWidth=0;
  m_ntuple->addItem("ErrPhi",m_errPhi);
  m_errPhi=0;
  m_ntuple->addItem("ErrZ",m_errZ);
  m_errZ=0;

  return StatusCode::SUCCESS;
}

StatusCode ReadSiClusters::finalize() {
  ATH_MSG_DEBUG( "In finalize()" );
  return StatusCode::SUCCESS;
}

StatusCode ReadSiClusters::execute() {

  if(m_readSiClusters){
    ATH_CHECK( FillSiNtuple() );
  }
  return StatusCode::SUCCESS;
}

StatusCode ReadSiClusters::FillSiNtuple(){

  const DataHandle<EventInfo> eventInfo;
  ATH_CHECK( evtStore()->retrieve(eventInfo) );

  m_eventNumber= eventInfo->event_ID()->event_number();
  ATH_MSG_DEBUG( "event number " << m_eventNumber);
  m_numberOfEvents++;

  // get space points from TDS
  const DataHandle<InDet::SiClusterContainer> clusters;
  ATH_CHECK( evtStore()->retrieve(clusters, m_SiClustersName) );

  InDet::SiClusterContainer::const_iterator colNext = clusters->begin();
  InDet::SiClusterContainer::const_iterator colEnd = clusters->end();

  int collectionCount(0);
  int clusterCount(0);
  for (; colNext!= colEnd; ++colNext)
    {

      InDet::SiClusterCollection::const_iterator nextCluster = (*colNext)->begin();
      InDet::SiClusterCollection::const_iterator lastCluster = (*colNext)->end();
  
      for (; nextCluster!=lastCluster; nextCluster++)
	{
	  const InDet::SiCluster& cluster = **nextCluster;
	  m_r = cluster.globalPosition().perp();
	  m_phi = cluster.globalPosition().phi();
	  m_z = cluster.globalPosition().z();
	  const InDet::SiWidth siWidth = cluster.width();
	  //            const CLHEP::Hep2Vector PhiRZwidth = siWidth.widthPhiRZ();
	  const Amg::Vector2D& ColRowWidth = siWidth.colRow();
	  const Amg::MatrixX& cov = cluster.localCovariance();
	  m_phiWidth = siWidth.phiR();
	  m_etaWidth = siWidth.z();
	  m_rowWidth = ColRowWidth.x();
	  m_colWidth = ColRowWidth.y();
	  m_errPhi = Amg::error(cov, Trk::locRPhi);
	  m_errZ = Amg::error(cov, Trk::locZ);
	  ATH_CHECK( ntupleSvc()->writeRecord(m_ntuple) );
	  /*
	  if (nsc==StatusCode::FAILURE) 
	    {
              ATH_MSG_ERROR( "Cannot record ntuple !" );
	      sc = StatusCode::FAILURE; 
	    }
	  */
	  clusterCount++;
	}
      collectionCount++;
    }
  return StatusCode::SUCCESS;
}

