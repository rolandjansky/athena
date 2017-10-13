/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// IdScan: TrigZFinder
// ( see header-file for details )
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <vector>

#include "GaudiKernel/MsgStream.h"

#include "IDScanZFinder/TrigZFinder.h"
#include "IDScanZFinder/ZFinderConstants.h"

#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "IRegionSelector/IRoiDescriptor.h"


////////////////////////////////////////////////////////////////////////////////
////    IDScanZFinder
////////////////////////////////////////////////////////////////////////////////

TrigZFinder::TrigZFinder( const std::string& type, const std::string& name, const IInterface* parent) : 
  TrigZFinder::TrigZFinderInternal(type, name), AthAlgTool( type, name, parent ),
  m_numberingTool("TrigL2LayerNumberTool")
{
  declareInterface< ITrigZFinder >( this );

  //  std::cout << "ZFinder::ZFinder() " << name << " " << type << "\tm_nFirstLayers  " << m_nFirstLayers  << std::endl;
  //  std::cout << "ZFinder::ZFinder() version " << name << " " << type << "\tTrigZFinder-00-00-62"<< std::endl;

  declareProperty( "PhiBinSize",         m_phiBinSize       = 0.2   );
  declareProperty( "UseOnlyPixels",      m_pixOnly          = false );
  declareProperty( "MinZBinSize",        m_minZBinSize      = 0.2   );
  declareProperty( "nFirstLayers",       m_nFirstLayers     = 3     );
  declareProperty( "ZBinSizeEtaCoeff",   m_zBinSizeEtaCoeff = 0.1   );
  declareProperty( "DPhiDEta",           m_dphideta         = -0.02 );
  declareProperty( "NeighborMultiplier", m_neighborMultiplier = 1.);
  declareProperty( "NumberOfPeaks",      m_numberOfPeaks    = 1     );
  declareProperty( "ChargeAware",        m_chargeAware      = false );
  declareProperty( "ZHistoPerPhi",       m_zHistoPerPhi     = false );
  declareProperty( "VrtxDistCut",        m_vrtxDistCut      = 0.    );
  declareProperty( "nVrtxSeparation",    m_nvrtxSeparation  = 0     );
  declareProperty( "VrtxMixing",         m_vrtxMixing       = 0.    );
  declareProperty( "PreferCentralZ",     m_preferCentralZ   = true  );
  declareProperty( "TrustSPProvider",    m_trustSPprovider  = true  );
  declareProperty( "FullScanMode",       m_fullScanMode     = false );
  declareProperty( "TripletMode",        m_tripletMode      = 0     ); 
  declareProperty( "TripletDZ",          m_tripletDZ        = 25.   );
  declareProperty( "TripletDK",          m_tripletDK        = 0.005 );
  declareProperty( "TripletDP",          m_tripletDP        = 0.05  );
  declareProperty( "WeightThreshold",    m_weightThreshold  = 0     );
  declareProperty( "MaxLayer",           m_maxLayer        = 32    );
  declareProperty( "MinVtxSignificance", m_minVtxSignificance  = 0  );
  declareProperty( "Percentile",         m_percentile          = 1  );


  declareProperty( "LayerNumberTool", m_numberingTool);

  //  std::cout << "ZFinder::ZFinder() " << name << " " << type << "\tm_nFirstLayers  " << m_nFirstLayers  << std::endl;

}


StatusCode TrigZFinder::initialize()
{
  StatusCode sc = AthAlgTool::initialize(); // sets properties

  MsgStream athenaLog( msgSvc(), name() );

  if ( sc.isFailure() ){
    athenaLog << MSG::ERROR << "Error in AthAlgTool::initialize()  " << endreq;
    // msg(MSG::ERROR) << "Error in AthAlgTool::initialize()  " << endreq;
    return sc;
  }

  //   NB: This should go into the InitializeInternal !!!!!!!!!
  //       NO internal settings should be changed in here, this should just 
  //       be an athena wrapper !!!

  //   phiBinSize is expected in degrees (~0.2); make sure it is > ZFinder_MinPhiSliceSize
  //

  /// NB: These only have to go here, because they need to write to the msgsvc, and because 
  ///     is rubbish, we can't pass in a pointer to a (non-athena) sub algorithm.
  if ( m_phiBinSize < ZFinder_MinPhiSliceSize ){
    athenaLog << MSG::WARNING << "TrigZFinder constructor: " << name()  << endreq;
    athenaLog << MSG::WARNING 
		<< "Requested PhiBinSize of "  << m_phiBinSize 
		<< " degrees is smaller than the minimum allowed (" << ZFinder_MinPhiSliceSize
		<< " degrees). Set to the minimum value." << endreq;
    //  m_phiBinSize = ZFinder_MinPhiSliceSize;
  }
 
  // NB: This should go into the InitializeInternal !!!!
  if ( m_dphideta > 0 ){
    athenaLog << MSG::WARNING << "TrigZFinder constructor: " << name()  << endreq;
    athenaLog << MSG::WARNING 
		<< "Requested DPhiDEta of "  << m_dphideta 
		<< " is positive.  Setting to its negative!" << endreq;
    // m_dphideta *= -1.;
  }

  if (m_numberingTool.retrieve().isFailure()){
    athenaLog << MSG::FATAL << "Tool " << m_numberingTool
	      << " not found " << endreq;
    return StatusCode::FAILURE;
  } 

  /// get first endcap layer, so we know how
  /// barrel layers there are 
  int offsetEndcapPixels = m_numberingTool->offsetEndcapPixels();
  int maxSiliconLayerNum = m_numberingTool->maxSiliconLayerNum(); 
  //  int offsetBarrelSCT    = m_numberingTool->offsetBarrelSCT(); 
 

  // std::cout << "ZFinder::initialise() offset pixels " << offsetEndcapPixels 
  // 	    << "\toffsetBarrelSCT " << offsetBarrelSCT 
  //	    << "\tmaxlayers " << maxSiliconLayerNum << std::endl;

  /// pass in the total number of layers, and the last barrel layer
  /// NB: decrement the endcap pixels, as we want the layer number 
  ///     of the last barrel layer, not the number of the first 
  ///     endcap pixel layer

  initializeInternal(maxSiliconLayerNum,offsetEndcapPixels-1);

  athenaLog << MSG::INFO << "TrigZFinder constructed:     name()  "    << name()             << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::PixOnly        set to   "    << m_pixOnly          << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::FullScanMode            "    << m_fullScanMode     << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::PhiBinSize     set to   "    << m_phiBinSize       << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::# of peaks to consider: "    << m_numberOfPeaks    << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::z bin size              "    << m_minZBinSize      << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::eta coeff               "    << m_zBinSizeEtaCoeff << endmsg;
  
  athenaLog << MSG::INFO << "TrigZFinder::m_nFirstLayers     = " << m_nFirstLayers     << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::m_invPhiSliceSize  = " << m_invPhiSliceSize  << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::m_phiBinSize       = " << m_phiBinSize       << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::m_dphideta         = " << m_dphideta         << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::m_neighborMultiplier = " << m_neighborMultiplier << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::m_minZBinSize      = " << m_minZBinSize      << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::m_zBinSizeEtaCoeff = " << m_zBinSizeEtaCoeff << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::m_chargeAware      = " << m_chargeAware      << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::m_zHistoPerPhi     = " << m_zHistoPerPhi     << endmsg;

  athenaLog << MSG::INFO << "TrigZFinder::m_nvrtxSeparation  = " << m_nvrtxSeparation  << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::m_vrtxDistCut      = " << m_vrtxDistCut      << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::m_vrtxMixing       = " << m_vrtxMixing       << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::m_preferCentralZ   = " << m_preferCentralZ   << endmsg;

  athenaLog << MSG::INFO << "TrigZFinder::m_trustSPprovider  = " << m_trustSPprovider  << endmsg;
  athenaLog << MSG::INFO << "TrigZFinder::m_tripletMode      = " << m_tripletMode      << endmsg;

  athenaLog << MSG::INFO << "TrigZFinder::m_maxLayer         = " << m_maxLayer        << endmsg;

  athenaLog << MSG::INFO << "TrigZFinder::m_minVtxSignificance = " << m_minVtxSignificance  << endmsg;

  if ( m_minVtxSignificance>0 ) { 
    athenaLog << MSG::INFO << "TrigZFinder::m_percentile     = " << m_percentile  << endmsg;
  }

  athenaLog << MSG::INFO << "TrigZFinder::m_weigthThreshold  = " << m_weightThreshold  << endmsg;

  return sc;
}


StatusCode TrigZFinder::finalize() { 
  return AthAlgTool::finalize(); 
}

TrigVertexCollection* TrigZFinder::findZ( const std::vector<TrigSiSpacePointBase>& spVec, const IRoiDescriptor& roi)
{

  TrigVertexCollection* output = new TrigVertexCollection;

  std::vector<vertex>* vertices = findZInternal( spVec, roi);

  ATH_MSG_DEBUG("roi: "    << roi);
  ATH_MSG_DEBUG("m_NumPhiSlices: " << m_NumPhiSlices);
 

  if ( GetInternalStatus()==-1 ) { 
    ATH_MSG_WARNING("phi of spacepoint out of range! phi=" << GetReturnValue());
    ATH_MSG_WARNING("Exiting ZFinder...");
  }

  for ( unsigned int i=0 ; i<vertices->size() ; i++ ) { 
    output->push_back( new TrigVertex( (*vertices)[i]._z, (*vertices)[i]._weight, TrigVertex::NULLID ) );
    //    std::cout << "SUTT vertex " << i << "\tz "<< (*vertices)[i]._z << "\tn " << (*vertices)[i]._weight << std::endl;
  }

  delete vertices;

  return output;
}

