/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
////////////////////////////////////////////////////////////////////////////////
// IdScan: TrigZFinder
// ( see header-file for details )
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <vector>

#include "TrigZFinder.h"
#include "ZFinderConstants.h"

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
  //   NB: This should go into the InitializeInternal !!!!!!!!!
  //       NO internal settings should be changed in here, this should just 
  //       be an athena wrapper !!!

  //   phiBinSize is expected in degrees (~0.2); make sure it is > ZFinder_MinPhiSliceSize
  //

  /// NB: These only have to go here, because they need to write to the msgsvc, and because 
  ///     is rubbish, we can't pass in a pointer to a (non-athena) sub algorithm.
  if ( m_phiBinSize < ZFinder_MinPhiSliceSize ){
    ATH_MSG_WARNING("Requested PhiBinSize of "  << m_phiBinSize
                    << " degrees is smaller than the minimum allowed (" << ZFinder_MinPhiSliceSize
                    << " degrees). Set to the minimum value.");
    //  m_phiBinSize = ZFinder_MinPhiSliceSize;
  }
 
  // NB: This should go into the InitializeInternal !!!!
  if ( m_dphideta > 0 ){
    ATH_MSG_WARNING("Requested DPhiDEta of "  << m_dphideta
                    << " is positive.  Setting to its negative!");
    // m_dphideta *= -1.;
  }

  ATH_CHECK(m_numberingTool.retrieve());

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

  ATH_MSG_INFO("TrigZFinder constructed:     name()  "    << name()            );
  ATH_MSG_INFO("TrigZFinder::PixOnly        set to   "    << m_pixOnly         );
  ATH_MSG_INFO("TrigZFinder::FullScanMode            "    << m_fullScanMode    );
  ATH_MSG_INFO("TrigZFinder::PhiBinSize     set to   "    << m_phiBinSize      );
  ATH_MSG_INFO("TrigZFinder::# of peaks to consider: "    << m_numberOfPeaks   );
  ATH_MSG_INFO("TrigZFinder::z bin size              "    << m_minZBinSize     );
  ATH_MSG_INFO("TrigZFinder::eta coeff               "    << m_zBinSizeEtaCoeff);
  
  ATH_MSG_INFO("TrigZFinder::m_nFirstLayers     = " << m_nFirstLayers    );
  ATH_MSG_INFO("TrigZFinder::m_invPhiSliceSize  = " << m_invPhiSliceSize );
  ATH_MSG_INFO("TrigZFinder::m_phiBinSize       = " << m_phiBinSize      );
  ATH_MSG_INFO("TrigZFinder::m_dphideta         = " << m_dphideta        );
  ATH_MSG_INFO("TrigZFinder::m_neighborMultiplier = " << m_neighborMultiplier);
  ATH_MSG_INFO("TrigZFinder::m_minZBinSize      = " << m_minZBinSize     );
  ATH_MSG_INFO("TrigZFinder::m_zBinSizeEtaCoeff = " << m_zBinSizeEtaCoeff);
  ATH_MSG_INFO("TrigZFinder::m_chargeAware      = " << m_chargeAware     );
  ATH_MSG_INFO("TrigZFinder::m_zHistoPerPhi     = " << m_zHistoPerPhi    );

  ATH_MSG_INFO("TrigZFinder::m_nvrtxSeparation  = " << m_nvrtxSeparation );
  ATH_MSG_INFO("TrigZFinder::m_vrtxDistCut      = " << m_vrtxDistCut     );
  ATH_MSG_INFO("TrigZFinder::m_vrtxMixing       = " << m_vrtxMixing      );
  ATH_MSG_INFO("TrigZFinder::m_preferCentralZ   = " << m_preferCentralZ  );

  ATH_MSG_INFO("TrigZFinder::m_trustSPprovider  = " << m_trustSPprovider );

  ATH_MSG_INFO("TrigZFinder::m_tripletMode      = " << m_tripletMode     );

  ATH_MSG_INFO("TrigZFinder::m_maxLayer         = " << m_maxLayer       );

  ATH_MSG_INFO("TrigZFinder::m_minVtxSignificance = " << m_minVtxSignificance );

  if ( m_minVtxSignificance>0 ) { 
    ATH_MSG_INFO("TrigZFinder::m_percentile     = " << m_percentile );
  }

  ATH_MSG_INFO("TrigZFinder::m_weigthThreshold  = " << m_weightThreshold );

  return StatusCode::SUCCESS;
}




TrigVertexCollection* TrigZFinder::findZ( const std::vector<TrigSiSpacePointBase>& spVec, const IRoiDescriptor& roi) const
{

  TrigVertexCollection* output = new TrigVertexCollection;

  std::vector<vertex>* vertices = findZInternal( spVec, roi );

  ATH_MSG_DEBUG( "findZ() roi: " << roi );
 
  for ( unsigned int i=0 ; i<vertices->size() ; i++ ) { 
    auto vtx = new TrigVertex((*vertices)[i]._z, (*vertices)[i]._weight, TrigVertex::NULLID);
    output->push_back(vtx);
  }

  delete vertices;

  return output;
}

