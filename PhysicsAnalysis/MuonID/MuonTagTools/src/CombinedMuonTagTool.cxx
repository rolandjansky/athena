/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : CombinedMuonTagTool.cxx
Package : offline/PhysicsAnalysis/MuonID/CombinedMuonTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a CombinedMuonTag - word to encode muon specific information

*****************************************************************************/
/*
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "muonEvent/MuonContainer.h"
//#include "xAODMuon/MuonContainer.h"
#include "MuonTagTools/CombinedMuonTagTool.h"
#include "TagEvent/CombinedMuonAttributeNames.h"

#include "AnalysisUtils/AnalysisMisc.h"

//using namespace xAOD;
using namespace Analysis;
*/
/** the constructor */
/*
CombinedMuonTagTool::CombinedMuonTagTool (const std::string& type, const std::string& name, 
    const IInterface* parent) : 
    AlgTool( type, name, parent ), 
    m_storeGate("StoreGateSvc", name) 
{
*/
  /** Muon AOD Container Names */
/*
  declareProperty("MuonContainerNames",   m_containerNames);
*/
  /** selection cut of Pt */
/*
  declareProperty("MuonPtCut",       m_muonPtCut = 2.0*CLHEP::GeV);
*/
  /** select what the basis for encoding into the bits should be */
//  declareProperty("AuthorBased",       m_authorBased);
  
//  declareInterface<CombinedMuonTagTool>( this );
//}

/** initialization - called once at the begginning */
/*StatusCode  CombinedMuonTagTool::initialize() {
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in intialize()" << endreq;

  if (m_storeGate.retrieve().isFailure()) {
    mLog << MSG::ERROR << "Unable to retrieve pointer to StoreGateSvc"
         << endreq;
    return StatusCode::FAILURE;
  }

  m_userMuonContainer = new MuonContainer ( SG::VIEW_ELEMENTS );

  return StatusCode::SUCCESS;
}
*/
/** build the attribute list - called in initialize */
/*StatusCode CombinedMuonTagTool::attributeSpecification(
           std::map<std::string,AthenaAttributeType>& attrMap, const int max) {

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in attributeSpecification()" << endreq;
*/
  /** specifiy the combined Muon - Moore/Muonboy attributes */
/*
  attrMap[ CombinedMuonAttributeNames[0] ] = AthenaAttributeType("unsigned int", CombinedMuonAttributeUnitNames[0], CombinedMuonAttributeGroupNames[0]);
*/
  /** add more stuff if necessary */
/* for (int i=0; i<max; ++i) {}

  return StatusCode::SUCCESS;
}
*/
/** execute - called on every event */
/*StatusCode CombinedMuonTagTool::execute(TagFragmentCollection& combinedMuonTagCol, const int max) {

  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in execute()" << endreq;
*/
  /** build the bit map according to this prescription 
     With AuthorBased=False:
     Bits:
     - bit 0: at least one muon has been seen by any algorithm
     - bit 1: more than one muon has been seen by any algorithm
     - bit 2: at least one muon has been seen by any stand-alone algorithm
     - bit 3: more than one muon has been seen by any stand-alone algorithm
     - bit 4: at least one muon has been extrapolated by any stand-alone  
       algorithm
     - bit 5: more than one muon has been extrapolated by any stand-alone  
       algorithm
     - bit 6: at least one muon has been seen by any track-to-track  
       matching algorithm
     - bit 7: more than one muon has been seen by any track-to-track  
       matching algorithm
     - bit 8: at least one muon has been seen by any track-to-segments  
       matching algorithm
     - bit 9: more than one muon has been seen by any track-to-segments  
       matching algorithm
     - bit 10: at least one muon has been seen by any track-to-calo  
       matching algorithm
     - bit 11: more than one muon has been seen by any track-to-calo  
       matching algorithm
       
     With AuthorBased=True:
     Bits:
     - bit 0: at least one muon has been seen by Muonboy
     - bit 1: more than one muon has been seen by Muonboy
     - bit 2: at least one muon has been seen by Staco
     - bit 3: more than one muon has been seen by Staco
     - bit 4: at least one muon has been seen by MuTag
     - bit 5: more than one muon has been seen by MuTag
     - bit 6: at least one muon has been seen by MuidStandalone
     - bit 7: more than one muon has been seen by MuidStandalone
     - bit 8: at least one muon has been seen by MuidCombined
     - bit 9: more than one muon has been seen by MuidCombined
     - bit 10: at least one muon has been seen by MuGirl
     - bit 11: more than one muon has been seen by MuGirl
     - bit 12: at least one muon has been seen by MuTagIMO
     - bit 13: more than one muon has been seen by MuTagIMO
     - bit 14: at least one muon has been seen by any Calo alg
     - bit 15: more than one muon has been seen by any Calo alg
  */

  /** add more stuff if necessary */
/*for (int i=0; i<max; ++i) {}

  unsigned int fragment = 0x0;

  unsigned int any_alg =0;
  unsigned int standalone_alg=0;
  unsigned int extrapolated_alg=0;
  unsigned int combined_alg=0;
  unsigned int lowPt_alg=0;
  unsigned int calo_alg=0;
  unsigned int muonboy  = 0;
  unsigned int stacocb  = 0;
  unsigned int mutag    = 0;
  unsigned int muidsa   = 0;
  unsigned int muidcb   = 0;
  unsigned int mugirl   = 0;
  unsigned int mutagimo = 0;
  unsigned int calo     = 0;

  for ( unsigned int j=0; j<m_containerNames.size(); ++j ) {
     const MuonContainer *muonContainer=0;
     StatusCode sc = m_storeGate->retrieve( muonContainer, m_containerNames[j] );
     if (sc.isFailure()) {
        mLog << MSG::WARNING << "No Muon container found in SG: " << m_containerNames[j] << endreq;
        continue;
     }
     mLog << MSG::DEBUG << "Muon container successfully retrieved: " << m_containerNames[j] << endreq;
     m_userMuonContainer->clear();
     *m_userMuonContainer = *muonContainer;
     AnalysisUtils::Sort::pT( m_userMuonContainer );
*/
     /** make the selection */
/*
     MuonContainer::const_iterator muonItr  = m_userMuonContainer->begin();
     MuonContainer::const_iterator muonItrE = m_userMuonContainer->end();
     for (; muonItr != muonItrE; ++muonItr) { 
      if ( (*muonItr)->pt()>m_muonPtCut ) {
        if (m_authorBased) { /// count the authors
          if (m_containerNames[j]=="MuidMuonCollection") {
            if ( (*muonItr)->isAuthor(12) ) ++muidcb;
            if ( (*muonItr)->isAuthor(11) ) ++muidsa;
            if ( (*muonItr)->isAuthor(13) ) ++mugirl;
            if ( (*muonItr)->isAuthor(18) ) ++mutagimo;
          } else if (m_containerNames[j]=="StacoMuonCollection") {
            if ( (*muonItr)->isAuthor(6) )  ++stacocb;
            if ( (*muonItr)->isAuthor(5) )  ++muonboy;
            if ( (*muonItr)->isAuthor(7) )  ++mutag;
          } else if (m_containerNames[j]=="CaloMuonCollection") {
            if ( (*muonItr)->isAuthor(14) || (*muonItr)->isAuthor(16) ||(*muonItr)->isAuthor(17) )  ++calo;
          }
        } else { /// count types of muons
          any_alg++;
          if ( (*muonItr)->isStandAloneMuon() ) standalone_alg++;
          if ( (*muonItr)->hasInnerExtrapolatedTrackParticle() ) extrapolated_alg++;
          if ( (*muonItr)->isCombinedMuon() ) combined_alg++;
          if ( (*muonItr)->isSegmentTaggedMuon() && !(*muonItr)->hasInnerExtrapolatedTrackParticle()) lowPt_alg++;
          if ( (*muonItr)->isCaloMuonId() ) calo_alg++; 
        }
      }
    }
  }
*/
  /** Now encode the Bits -
  */   
/* if (m_authorBased) { /// encode the authors
    if ( muonboy > 0 )          fragment = 0x1;
    if ( muonboy > 1 )          fragment = fragment | 2;
    if ( stacocb > 0 )          fragment = fragment | 4;
    if ( stacocb > 1 )          fragment = fragment | 8;
    if ( mutag > 0 )            fragment = fragment | 16;
    if ( mutag > 1 )            fragment = fragment | 32;
    if ( muidsa > 0 )           fragment = fragment | 64;
    if ( muidsa > 1 )           fragment = fragment | 128;
    if ( muidcb > 0 )           fragment = fragment | 256;
    if ( muidcb > 1 )           fragment = fragment | 512;
    if ( mugirl > 0 )           fragment = fragment | 1024;
    if ( mugirl > 1 )           fragment = fragment | 2048;
    if ( mutagimo > 0 )         fragment = fragment | 4096;
    if ( mutagimo > 1 )         fragment = fragment | 8192;
    if ( calo > 0 )             fragment = fragment | 16384;
    if ( calo > 1 )             fragment = fragment | 32768;
  } else { /// encode types of muons
    if ( any_alg > 0 )          fragment = 0x1;
    if ( any_alg > 1 )          fragment = fragment | 2;
    if ( standalone_alg > 0 )   fragment = fragment | 4;
    if ( standalone_alg > 1 )   fragment = fragment | 8;
    if ( extrapolated_alg > 0 ) fragment = fragment | 16;
    if ( extrapolated_alg > 1 ) fragment = fragment | 32;
    if ( combined_alg > 0 )     fragment = fragment | 64;
    if ( combined_alg > 1 )     fragment = fragment | 128;
    if ( lowPt_alg > 0 )        fragment = fragment | 256;
    if ( lowPt_alg > 1 )        fragment = fragment | 512;
    if ( calo_alg > 0 )         fragment = fragment | 1024;
    if ( calo_alg > 1 )         fragment = fragment | 2048;
  }

  combinedMuonTagCol.insert( CombinedMuonAttributeNames[0], fragment );

  return StatusCode::SUCCESS;
}
*/
/** finialize - called once at the end */
/*
StatusCode CombinedMuonTagTool::finalize() {
  MsgStream mLog(msgSvc(), name());
  mLog << MSG::DEBUG << "in finalize()" << endreq;
  m_userMuonContainer->clear();
  delete m_userMuonContainer;
  return StatusCode::SUCCESS;
}
*/
/** destructor */
//CombinedMuonTagTool::~CombinedMuonTagTool() {}


