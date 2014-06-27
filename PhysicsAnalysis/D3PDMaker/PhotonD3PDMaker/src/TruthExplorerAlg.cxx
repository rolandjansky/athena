/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthExplorerAlg.cxx 502492 2012-05-25 13:19:11Z jmarchan $
/**
 * @file PhotonD3PDMaker/src/TruthExplorerAlg.cxx
 * @author Mike Hance <mike.hance@cern.ch>
 * @date June 2011
 * @brief Calculate some different isolation values
 */


#include "TruthExplorerAlg.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "egammaEvent/Photon.h"
#include "egammaEvent/Electron.h"
#include "FourMomUtils/P4Helpers.h"
#include "AthenaKernel/errorcheck.h"
#include "boost/foreach.hpp"
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>

#include "TMath.h"

#include "HepMC/GenParticle.h"

namespace D3PD {

TruthExplorerAlg::TruthExplorerAlg (const std::string& name,
						ISvcLocator* svcloc)
  : UDAlgBase (name, svcloc),
    m_photonGetter(this),
    m_electronGetter(this),
    m_truthParticleGetter(this)
{
  declareProperty ("PAUtruthTool", m_PAUtruthTool,
                   "PAU truth tool.");

  declareProperty ("PAUCaloIsolationTool", m_PAUTruthCaloIsolationTool,
		   "PAU calorimeter isolation correction tool.");

  declareProperty ("CalculateTruthCaloIsolation", m_CalculateTruthCaloIsolation=true,
		   "Compute the jet-area corrections for the particle-level isolation variables");

  declareProperty ("RemoveNHardestJets", m_RemoveNHardestJets=0,
		   "Ignore the N hardest jets when computing the jet-area corrections.");
  
  declareProperty ("PhotonGetter", m_photonGetter,
                   "Getter instance for the photons.");

  declareProperty ("ElectronGetter", m_electronGetter,
                   "Getter instance for the electrons.");

  declareProperty ("TruthParticleGetter", m_truthParticleGetter,
                   "Getter instance for the truth particles.");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode TruthExplorerAlg::initialize()
{
  CHECK( UDAlgBase::initialize() );
  CHECK( m_PAUtruthTool.retrieve() );
  CHECK( m_PAUTruthCaloIsolationTool.retrieve() );
  CHECK( m_photonGetter.retrieve() );
  CHECK( m_electronGetter.retrieve() );
  CHECK( m_truthParticleGetter.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode TruthExplorerAlg::DecorateTruthParticle(const TruthParticle* truePart){

  if(m_DecoratedTruthParticles.find(truePart) != m_DecoratedTruthParticles.end()) return StatusCode::SUCCESS;
  
  // conversion info 
  m_isTruthConv = (int)m_PAUtruthTool->isConvMC( truePart, 
						   m_XTruthConv, 
						   m_YTruthConv, 
						   m_RTruthConv, 
						   m_ZTruthConv );

  CHECK( deco( *truePart, "TruthConv",  m_isTruthConv) );
  CHECK( deco( *truePart, "XTruthConv", m_XTruthConv ) );
  CHECK( deco( *truePart, "YTruthConv", m_YTruthConv ) );
  CHECK( deco( *truePart, "ZTruthConv", m_ZTruthConv ) );
  CHECK( deco( *truePart, "RTruthConv", m_RTruthConv ) );

  m_isHardProc     = (int)m_PAUtruthTool->isDirectPhotonMC(truePart) ;
  m_isBrem         = (int)(!(m_isHardProc) && m_PAUtruthTool->isQuarkBremMC(truePart)) ;
  m_isPromptPhoton = (int)m_PAUtruthTool->isPromptPhotonMC(truePart);

  CHECK( deco( *truePart, "isHardProcPhoton", m_isHardProc     ) ); 
  CHECK( deco( *truePart, "isBremPhoton"    , m_isBrem         ) );
  CHECK( deco( *truePart, "isPromptPhoton"  , m_isPromptPhoton ) );
  
  //all partons in cone
  m_partonIsolation15_UE =  m_PAUtruthTool->getPartonIsolation(truePart, 0.15, false )  ; 
  m_partonIsolation20_UE =  m_PAUtruthTool->getPartonIsolation(truePart, 0.20, false )  ; 
  m_partonIsolation25_UE =  m_PAUtruthTool->getPartonIsolation(truePart, 0.25, false )  ;
  m_partonIsolation30_UE =  m_PAUtruthTool->getPartonIsolation(truePart, 0.30, false )  ;
  m_partonIsolation35_UE =  m_PAUtruthTool->getPartonIsolation(truePart, 0.35, false )  ; 
  m_partonIsolation40_UE =  m_PAUtruthTool->getPartonIsolation(truePart, 0.40, false )  ; 
  m_partonIsolation45_UE =  m_PAUtruthTool->getPartonIsolation(truePart, 0.45, false )  ; 
  m_partonIsolation60_UE =  m_PAUtruthTool->getPartonIsolation(truePart, 0.60, false )  ;

  CHECK( deco( *truePart, "PartonIsolation15_UE",  m_partonIsolation15_UE ) );
  CHECK( deco( *truePart, "PartonIsolation20_UE",  m_partonIsolation20_UE ) );
  CHECK( deco( *truePart, "PartonIsolation25_UE",  m_partonIsolation25_UE ) );
  CHECK( deco( *truePart, "PartonIsolation30_UE",  m_partonIsolation30_UE ) );
  CHECK( deco( *truePart, "PartonIsolation35_UE",  m_partonIsolation35_UE ) );
  CHECK( deco( *truePart, "PartonIsolation40_UE",  m_partonIsolation40_UE ) );
  CHECK( deco( *truePart, "PartonIsolation45_UE",  m_partonIsolation45_UE ) );
  CHECK( deco( *truePart, "PartonIsolation60_UE",  m_partonIsolation60_UE ) );

  //just partons from the same mother in cone      
  m_partonIsolation15    =  m_PAUtruthTool->getPartonIsolation(truePart, 0.15, true )  ; 
  m_partonIsolation20    =  m_PAUtruthTool->getPartonIsolation(truePart, 0.20, true )  ; 
  m_partonIsolation30    =  m_PAUtruthTool->getPartonIsolation(truePart, 0.30, true )  ;
  m_partonIsolation25    =  m_PAUtruthTool->getPartonIsolation(truePart, 0.25, true )  ;
  m_partonIsolation35    =  m_PAUtruthTool->getPartonIsolation(truePart, 0.35, true )  ; 
  m_partonIsolation40    =  m_PAUtruthTool->getPartonIsolation(truePart, 0.40, true )  ; 
  m_partonIsolation45    =  m_PAUtruthTool->getPartonIsolation(truePart, 0.45, true )  ; 
  m_partonIsolation60    =  m_PAUtruthTool->getPartonIsolation(truePart, 0.60, true )  ;

  CHECK( deco( *truePart, "PartonIsolation15", m_partonIsolation15 ) );
  CHECK( deco( *truePart, "PartonIsolation20", m_partonIsolation20 ) );
  CHECK( deco( *truePart, "PartonIsolation30", m_partonIsolation30 ) );
  CHECK( deco( *truePart, "PartonIsolation25", m_partonIsolation25 ) );
  CHECK( deco( *truePart, "PartonIsolation35", m_partonIsolation35 ) );
  CHECK( deco( *truePart, "PartonIsolation40", m_partonIsolation40 ) );
  CHECK( deco( *truePart, "PartonIsolation45", m_partonIsolation45 ) );
  CHECK( deco( *truePart, "PartonIsolation60", m_partonIsolation60 ) );

  //compute particle level isolation          
  m_particleIsolation40  = m_PAUtruthTool->getParticleIsolation(truePart, 0.40, false)  ;
  m_particleIsolation30  = m_PAUtruthTool->getParticleIsolation(truePart, 0.30, false)  ;
  m_particleIsolation20  = m_PAUtruthTool->getParticleIsolation(truePart, 0.20, false)  ;

  CHECK( deco( *truePart, "ParticleIsolation40",  m_particleIsolation40 ) );
  CHECK( deco( *truePart, "ParticleIsolation30",  m_particleIsolation30 ) );
  CHECK( deco( *truePart, "ParticleIsolation20",  m_particleIsolation20 ) );

  //to get ED corrections right for particle isolation
  float area_cone40 = TMath::Pi()*0.4*0.4;
  float area_cone30 = TMath::Pi()*0.3*0.3;
  float area_cone20 = TMath::Pi()*0.2*0.2;
  int neta = 5, nphi = 7;
  float deta = 0.0125*neta*2;
  float dphi = M_PI/256.*nphi*2;
  float area_5x7 = deta*dphi;

  float fPutWBack40  = area_cone40 / (area_cone40 - area_5x7);
  float fPutWBack30  = area_cone30 / (area_cone30 - area_5x7);
  float fPutWBack20  = area_cone20 / (area_cone20 - area_5x7);

  if(m_CalculateTruthCaloIsolation){
    m_particleIsolation40_ED_corrected      = m_particleIsolation40 - m_PAUTruthCaloIsolationTool->EtConeCorrectionJetAreas(truePart, .40, m_RemoveNHardestJets) * fPutWBack40 ;
    m_particleIsolation30_ED_corrected      = m_particleIsolation30 - m_PAUTruthCaloIsolationTool->EtConeCorrectionJetAreas(truePart, .30, m_RemoveNHardestJets) * fPutWBack30 ;
    m_particleIsolation20_ED_corrected      = m_particleIsolation20 - m_PAUTruthCaloIsolationTool->EtConeCorrectionJetAreas(truePart, .20, m_RemoveNHardestJets) * fPutWBack20 ;
  }
  else{
    m_particleIsolation40_ED_corrected      = PAU::UNKNOWN;
    m_particleIsolation30_ED_corrected      = PAU::UNKNOWN;
    m_particleIsolation20_ED_corrected      = PAU::UNKNOWN;
  }

  CHECK( deco( *truePart, "ParticleIsolation40_ED_corrected", m_particleIsolation40_ED_corrected ) );
  CHECK( deco( *truePart, "ParticleIsolation30_ED_corrected", m_particleIsolation30_ED_corrected ) );
  CHECK( deco( *truePart, "ParticleIsolation20_ED_corrected", m_particleIsolation20_ED_corrected ) );

  if ( truePart->nParents()>0 && truePart->mother(0) ) 
    m_mother_pdgId = truePart->mother(0)->pdgId();
  else 
    m_mother_pdgId = int(PAU::UNKNOWN);
  CHECK( deco( *truePart, "mother_pdgId", m_mother_pdgId ) );

  m_DecoratedTruthParticles.insert(truePart);

  return StatusCode::SUCCESS;
}


StatusCode TruthExplorerAlg::DecorateTruthParticleWithZeroes(const TruthParticle* truePart){

  if(m_DecoratedTruthParticles.find(truePart) != m_DecoratedTruthParticles.end()) return StatusCode::SUCCESS;
  
  int unknownint = PAU::UNKNOWN;
  float unknownfloat = PAU::UNKNOWN;

  CHECK( deco( *truePart, "TruthConv" , unknownint   ) );
  CHECK( deco( *truePart, "XTruthConv", unknownfloat ) );
  CHECK( deco( *truePart, "YTruthConv", unknownfloat ) );
  CHECK( deco( *truePart, "ZTruthConv", unknownfloat ) );
  CHECK( deco( *truePart, "RTruthConv", unknownfloat ) );

  CHECK( deco( *truePart, "isHardProcPhoton", unknownint ) );
  CHECK( deco( *truePart, "isBremPhoton"    , unknownint ) );
  CHECK( deco( *truePart, "isPromptPhoton"  , unknownint ) );
  
  CHECK( deco( *truePart, "PartonIsolation15_UE", unknownfloat ) );
  CHECK( deco( *truePart, "PartonIsolation20_UE", unknownfloat ) );
  CHECK( deco( *truePart, "PartonIsolation25_UE", unknownfloat ) );
  CHECK( deco( *truePart, "PartonIsolation30_UE", unknownfloat ) );
  CHECK( deco( *truePart, "PartonIsolation35_UE", unknownfloat ) );
  CHECK( deco( *truePart, "PartonIsolation40_UE", unknownfloat ) );
  CHECK( deco( *truePart, "PartonIsolation45_UE", unknownfloat ) );
  CHECK( deco( *truePart, "PartonIsolation60_UE", unknownfloat ) );

  CHECK( deco( *truePart, "PartonIsolation15", unknownfloat ) );
  CHECK( deco( *truePart, "PartonIsolation20", unknownfloat ) );
  CHECK( deco( *truePart, "PartonIsolation30", unknownfloat ) );
  CHECK( deco( *truePart, "PartonIsolation25", unknownfloat ) );
  CHECK( deco( *truePart, "PartonIsolation35", unknownfloat ) );
  CHECK( deco( *truePart, "PartonIsolation40", unknownfloat ) );
  CHECK( deco( *truePart, "PartonIsolation45", unknownfloat ) );
  CHECK( deco( *truePart, "PartonIsolation60", unknownfloat ) );

  CHECK( deco( *truePart, "ParticleIsolation40", unknownfloat ) );
  CHECK( deco( *truePart, "ParticleIsolation30", unknownfloat ) );
  CHECK( deco( *truePart, "ParticleIsolation20", unknownfloat ) );

  CHECK( deco( *truePart, "ParticleIsolation40_ED_corrected", unknownfloat ) );
  CHECK( deco( *truePart, "ParticleIsolation30_ED_corrected", unknownfloat ) );
  CHECK( deco( *truePart, "ParticleIsolation20_ED_corrected", unknownfloat ) );

  CHECK( deco( *truePart, "mother_pdgId", unknownint ) );

  m_DecoratedTruthParticles.insert(truePart);

  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode TruthExplorerAlg::execute()
{

  m_DecoratedTruthParticles.clear();

  for ( unsigned u=0 ; u<m_PAUtruthTool->getFinalStateMC().size() ; ++u ) {
    
    const TruthParticle* truePart = m_PAUtruthTool->getFinalStateMC()[u] ;
    
    // I think this filter needs to be changed, so we keep a wider range
    // of potential photon candidates
    if ( truePart == 0 || 
	 truePart->genParticle() == 0 ||
	 truePart->pdgId() != 22 || 
	 !(m_PAUtruthTool->isFinalStatePhotonMC(truePart)))  
      continue;

    CHECK( DecorateTruthParticle(truePart) );
  }

  
  // okay, this part is ugly.  we need to make sure that we have MC particles corresponding
  // to all of the reconstructed electrons and photons.  Furthermore, they all need to be
  // decorated.  So, go through all of the electrons and photons, find their truth matches,
  // and decorate those too.
  CHECK( m_photonGetter->reset() );
  while (const Analysis::Photon* g = m_photonGetter->next<Analysis::Photon>()){
    const TruthParticle* tp = m_PAUtruthTool->getTruthMatch(g);
    if(tp){
      CHECK( DecorateTruthParticle(tp) );
    }
  }

  CHECK( m_electronGetter->reset() );
  while (const Analysis::Electron* e = m_electronGetter->next<Analysis::Electron>()){
    const TruthParticle* tp = m_PAUtruthTool->getTruthMatch(e);
    if(tp) {
      CHECK( DecorateTruthParticle(tp) );
    }
  }

  // now, for the rest of the truth particles, fill with dummy values
  CHECK( m_truthParticleGetter->reset() );
  while (const TruthParticle* tp = m_truthParticleGetter->next<TruthParticle>()){
    CHECK( DecorateTruthParticleWithZeroes(tp) );
  }


  return StatusCode::SUCCESS;
}


} // namespace D3PD
