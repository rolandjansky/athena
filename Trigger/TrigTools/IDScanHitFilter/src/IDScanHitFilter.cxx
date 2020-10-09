/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// IDScanHitFilter
// ( see header-file for details )
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/checker_macros.h"

#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigSiSpacePoint.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "IRegionSelector/IRoiDescriptor.h"

#include "IDScanHitFilter.h"
#include "IdScanSpPoint.h"
#include "Group.h"


IDScanHitFilter::IDScanHitFilter(const std::string& t, 
		     const std::string& n,
		     const IInterface*  p ): 
  AthAlgTool(t,n,p),
  m_numberingTool("TrigL2LayerNumberTool")
{
  declareInterface< ITrigHitFilter >( this );

  declareProperty( "PrintDiagnosticMessages", m_printDiagnosticMessages = false );
  declareProperty( "phiHalfWidth",            m_ROIphiHalfWidth = 0.1     );
  declareProperty( "etaHalfWidth",            m_ROIetaHalfWidth = 0.1     );

  declareProperty( "PhiBinSize",              m_phiBinSize = 2.4          );
  declareProperty( "EtaBinSize",              m_etaBinSize = 0.005        );
  declareProperty( "LayerThreshold",          m_layerThreshold = 3.5      );
  declareProperty( "EnhanceLayer0",           m_enhanceLayer0 = true      );
  declareProperty( "RemoveDuplicates",        m_cloneRemoval = true       );
  declareProperty( "CloneRemovalCut",         m_cloneRemovalCut = 3       );
  declareProperty( "UsePtSign",               m_usePtSign = true          );

  declareProperty( "LooseTightBoundary",      m_looseTight = 20.5         );

  declareProperty( "deltadzdrCut_loose",      m_loose_deltadzdrCut = 0.04 );
  declareProperty( "deltadzdrCut_tight",      m_tight_deltadzdrCut = 0.01 );
  declareProperty( "etaCutInner_loose",       m_loose_etaCutInner = 0.008 );
  declareProperty( "etaCutInner_tight",       m_tight_etaCutInner = 0.004 );
  declareProperty( "etaCutOuter_loose",       m_loose_etaCutOuter = 0.02  );
  declareProperty( "etaCutOuter_tight",       m_tight_etaCutOuter = 0.01  );
  declareProperty( "d0Cut_loose",             m_loose_d0Cut = 4.0         );
  declareProperty( "d0Cut_tight",             m_tight_d0Cut = 1.0         );
  declareProperty( "dPhidRBinSize_loose",     m_loose_dPhidRBinSize = 0.0000030 );
  declareProperty( "dPhidRBinSize_tight",     m_tight_dPhidRBinSize = 0.0000024 );
  declareProperty( "pTcutInMeV",              m_pTcutInMeV = 1000.0       );
  declareProperty( "phi0Bins_loose",          m_loose_phi0Bins = 16000    );
  declareProperty( "phi0Bins_tight",          m_tight_phi0Bins = 20000    );

  declareProperty( "maxBeamSpotShift",        m_maxBeamSpotShift = 4.0    );
  declareProperty( "fullScan",                m_fullScan = false    );

}

StatusCode IDScanHitFilter::initialize()
{
  m_dPhidRCut = 0.3/m_pTcutInMeV;

  ATH_MSG_INFO( "IDScanHitFilter constructed "                        );
  ATH_MSG_INFO( "phiBinSize       set to " << m_phiBinSize      );
  ATH_MSG_INFO( "etaBinSize       set to " << m_etaBinSize      );
  ATH_MSG_INFO( "layerThreshold   set to " << m_layerThreshold  );
  ATH_MSG_INFO( "enhanceZeroLayer set to " << m_enhanceLayer0   );
  ATH_MSG_INFO( "Clone removal    set to " << m_cloneRemoval    );
  ATH_MSG_INFO( "dphidrcut    set to " << m_dPhidRCut    );

  ATH_CHECK( m_numberingTool.retrieve() );

  /// get first endcap layer, so we know how
  /// barrel layers there are 
  int offsetEndcapPixels = m_numberingTool->offsetEndcapPixels();
  int maxSiliconLayerNum = m_numberingTool->maxSiliconLayerNum(); 
  //  int offsetBarrelSCT    = m_numberingTool->offsetBarrelSCT(); 
 
  /// Set the layer variables for the FilterBin class
  /// NB: decrement the endcap pixels, as we want the layer number 
  ///     of the last barrel layer, not the number of the first 
  ///     endcap pixel layer
  /// initialise the HitFilter with the correct number of layers
  /// NB: DO NOT pass in the max eta here, but remember that it can be set here!!
  ///     the parameter initialisation is very messy for this stuff 
  iHitFilter::initialise( maxSiliconLayerNum, offsetEndcapPixels-1 ); /// , 3.0 ); 

  // NB: Now set the other way - ie, set m_fullScan=true, 
  //     then this will enforce m_ROIphiHalfWidth=M_PI, but in 
  //     iHitFilterinitialise() where it should have been in any 
  //     case 
  //  
  //  m_fullScan = (2*m_ROIphiHalfWidth > 6.28);

  return StatusCode::SUCCESS;
}


  //Luke: edit this to take recoTracks as an input pointer, passed by reference 
void IDScanHitFilter::findTracks( std::vector<const TrigSiSpacePoint* > spVec, TrigInDetTrackCollection& recoTracks, 
	                          const IRoiDescriptor* RoI, double zVertex,
				  double shiftx=0, double shifty=0,
				  const int missing_layers=0 )
{
  setLayerThresholdTmp(getLayerThreshold()-double(missing_layers));
  ATH_MSG_DEBUG( "m_layerThreshold="      << m_layerThreshold
                 << "\tm_layerThresholdTmp=" << m_layerThresholdTmp  );

  // 0. check if RoI is in [0,2pi] boundary (boundary for the space point definition!)
  //    and calculate phi offset <- now done in spacepoints themselves!!!

  // 1. make internal IDScan SPs
  if (RoI!=nullptr) {
		makeIDScanSPs( spVec, RoI->eta(), 0.5*fabs(RoI->etaPlus()-RoI->etaMinus()),
				RoI->phi(), 0.5*fabs(RoI->phiPlus()-RoI->phiMinus()), 
				zVertex );
	}
	else {
		makeIDScanSPs( spVec, 0, 3,
				0, M_PI, 
				zVertex );
	}

  ATH_MSG_DEBUG( "REGTEST / Made " << m_internalSPs->size() 
                 << " IdScanSpPoints. Proceeding to HitFilter."  );
  if (m_printDiagnosticMessages) std::cout << "IdScanMain DIAGNOSTIC " <<  " Made " << m_internalSPs->size() 
					   << " IdScanSpPoints. Proceeding to HitFilter." << std::endl;

  // 2. run HitFilter & GroupCleaner

  GroupList cleanGroups = execute();

  // 3. turn groups into TrigInDetTracks
  
  //Luke: delete this line
  //TrigInDetTrackCollection* recoTracks=NULL;
  if ( !cleanGroups.empty() ) {
    makeTracks( cleanGroups, recoTracks, zVertex, shiftx, shifty);
  }

  cleanInternalSPs();
  
  //return recoTracks;
}


//Luke: need to pass recoTracks by reference to this or have it as a member variable or something?
void IDScanHitFilter::makeTracks( GroupList& idScanGroups, TrigInDetTrackCollection& recoTracks,  double zPosition, 
				  double shiftx=0, double shifty=0)
{
    //  recoTracks->reserve( idScanGroups.size() );

  // Since the FilterMap is a hashmap and hash tables are not guaranteed
  // to be sorted, running the code twice sometimes ends up producing
  // differently ordered REGTEST output files. So sort them is debugging:

  if ( msgLvl(MSG::DEBUG) ) idScanGroups.sort();

  for ( GroupList::iterator gItr( idScanGroups.begin() ) ; gItr != idScanGroups.end(); ++gItr ) {
    if(gItr->groupHits().size()==0) continue;
    gItr->groupHits().sort(orderInRho());
    double phi0 = gItr->getPhi0();
    if(phi0<-M_PI) 
      { 
	phi0+=2.0*M_PI;
      }
    else if(phi0>M_PI) 
      {
	phi0-=2.0*M_PI;
      }
    ATH_MSG_DEBUG( "REGTEST / group phi0/ptInv/eta: " 
                   << phi0 << " / " 
                   << gItr->getPtInv() << " / " 
                   << gItr->getEta()  );

    if (m_printDiagnosticMessages) std::cout << "IdScanMain DIAGNOSTIC " << " group phi0/ptInv/eta: " 
					     << phi0 << " / " 
					     << gItr->getPtInv() << " / " 
					     << gItr->getEta() << std::endl;

    double ptInv=gItr->getPtInv();   
    if(fabs(ptInv)<1e-9) ptInv=1e-9; 

    double input_d0 = 0;
    if (shiftx || shifty){
      input_d0 = -shiftx*sin(phi0)+shifty*cos(phi0);
    }

    TrigInDetTrackFitPar* perigee = new TrigInDetTrackFitPar( input_d0, 
							      phi0, 
							      zPosition, 
							      gItr->getEta(), 
							      1.0/ptInv );

    std::vector<const TrigSiSpacePoint*> spacePointsOnTrack;
    for (IdScanSpPoint* sp : gItr->groupHits())
      {
	spacePointsOnTrack.push_back( sp->commonSP() );
      }
    std::vector<const TrigSiSpacePoint*>* spv = new std::vector<const TrigSiSpacePoint*>;
    spv->assign(spacePointsOnTrack.begin(),spacePointsOnTrack.end());
    TrigInDetTrack* newTrack = new TrigInDetTrack(spv, perigee, NULL, gItr->getChi2());
    newTrack->algorithmId(TrigInDetTrack::IDSCANID);
    recoTracks.push_back( newTrack );
  }

  //return recoTracks;

}
