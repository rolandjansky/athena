// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#if ! defined( _NEWHITFILTER_H )
#define _NEWHITFILTER_H

#include <vector>

//#ifndef M_2PI
//#define M_2PI   6.28318530717958647692
//#endif
	
//namespace TrigIdScan {
//  const long   IdScan_MaxNumLayers     = 19;
//  const double IdScan_maxEta = 3.0;
  const double maxEta = 3.0;
//} //namespace
///////////

//#include "IDScanHitFilter/HF_Constants.h"
const long   HashKeyCoeff  = 1000;

#include "TrigInDetPattRecoEvent/TrigL2TrackCandidate.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"

#include "TrigInDetPattRecoTools/TrigL2HoughTransformClass.h"
#include "TrigInDetPattRecoTools/TrigL2HashKeyFunction.h"
#include "TrigInDetPattRecoTools/TrigL2GroupMaker.h"
#include "TrigInDetPattRecoTools/TrigL2GroupCleanerCuts.h"
#include "TrigInDetPattRecoTools/TrigL2GroupCleaner.h"
#include "TrigInDetPattRecoTools/TrigL2Prefitter.h"
#include "TrigInDetPattRecoTools/TrigL2CloneRemoval.h"

class TrigRoiDescriptor;

////////////////////////////////////////////////////////////////////////////////
////    HitFilter
////////////////////////////////////////////////////////////////////////////////

class NewHitFilter
{
public:
  
  NewHitFilter() : m_fullScan(false),  
		   m_phiBinSize(2.4),
		   m_etaBinSize(0.005),
		   m_layerThreshold(3.5),
		   m_enhanceLayer0(true),
		   m_fieldOn(true),
		   m_removeOutliers(true),
		   m_cloneRemoval(true),
		   m_usePtSign(true),
		   m_cloneRemovalCut(true),
		   m_looseTight(20.5),
		   m_pTcutInMeV(1000.0),
		   m_printDiagnosticMessages(false),
		   m_maxBeamSpotShift(4.0),
		   m_loose(NULL), 
		   m_tight(NULL)  
	      //	 m_zVertex(0) 
  {

    m_pHoughForHF=NULL;m_pHoughForGC=NULL;m_pKeyGenForHF=NULL;
    m_pGroupMaker=NULL;
    m_pGroupCleaner=NULL;
    m_pPrefitter=NULL;
    m_pCloneRemoval=NULL;
  } 

  ~NewHitFilter();
  
  void initialise();

  double  getLayerThreshold() const { return m_layerThreshold; }

  void setLayerThresholdTmp(double d) { m_layerThresholdTmp = d; }

protected:                      // member functions

  TrigL2HoughTransformClass* m_pHoughForHF;
  TrigL2HoughTransformClass* m_pHoughForGC;
  TrigL2HashKeyFunction*     m_pKeyGenForHF;

  TrigL2GroupMaker*          m_pGroupMaker;
  TrigL2GroupCleaner*        m_pGroupCleaner;
  
  TrigL2Prefitter*           m_pPrefitter;
  TrigL2CloneRemoval*        m_pCloneRemoval;

  void execute(std::vector<TrigSiSpacePointBase*>&, const TrigRoiDescriptor&, double, std::vector<TrigL2TrackCandidate*>&); 

protected:                      // data members
  
  // to be read in from jobOptions
  bool   m_fullScan;


  long   m_maxPhiBins;
  double m_phiBinSize;
  double m_etaBinSize;
  double m_layerThreshold;    // This is the value that you set once.
  double m_layerThresholdTmp; // This value is set for each RoI, depending on number of missing layers
  bool   m_enhanceLayer0;
  bool   m_fieldOn;
  bool   m_removeOutliers;
  bool   m_cloneRemoval;
  bool   m_usePtSign;
  long   m_cloneRemovalCut;
  double m_invPhiBinSize;
  double m_invEtaBinSize;
  double m_looseTight;
  double m_pTcutInMeV;
  double m_dPhidRCut;
  double m_loose_deltadzdrCut;
  double m_loose_etaCutInner;
  double m_loose_etaCutOuter;
  double m_loose_d0Cut;
  double m_loose_dPhidRBinSize;
  long   m_loose_phi0Bins;
  double m_tight_deltadzdrCut;
  double m_tight_etaCutInner;
  double m_tight_etaCutOuter;
  double m_tight_d0Cut;
  double m_tight_dPhidRBinSize;
  long   m_tight_phi0Bins;
  bool   m_printDiagnosticMessages;
  double m_maxBeamSpotShift;

  TrigL2GroupCleanerCutsLoose* m_loose;         // loose set of parameters for group cleaner (look at GC_Constants.h)
  TrigL2GroupCleanerCutsTight* m_tight;         // tight set of parameters for group cleaner


};


#endif
