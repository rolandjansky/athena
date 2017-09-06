/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkTop/BoostedHadTopAndTopPairFilterTool.h"

namespace DerivationFramework{

//--------------------------------------------------------------------------
BoostedHadTopAndTopPairFilterTool::BoostedHadTopAndTopPairFilterTool(const std::string& t, const std::string& n, const IInterface* p)
  : AthAlgTool(t,n,p)
{

  declareInterface<DerivationFramework::BoostedHadTopAndTopPairFilterTool>(this);
  declareProperty("MCCollectionName",m_mcName     ="TruthEvents");
  declareProperty("cutPtOf",         m_cutPtOf    = 0);
  // use values directly in filterFlag to make tool more flexible for now...
  //declareProperty("tHadPtCut",       m_tHadPtCut  = 500000.0);
  //declareProperty("tPairPtCut",      m_tPairPtCut = 350000.0);

}

//--------------------------------------------------------------------------
BoostedHadTopAndTopPairFilterTool::~BoostedHadTopAndTopPairFilterTool(){}

//--------------------------------------------------------------------------
StatusCode BoostedHadTopAndTopPairFilterTool::initialize() {ATH_MSG_INFO("Initialize " ); return StatusCode::SUCCESS; }

//--------------------------------------------------------------------------
StatusCode BoostedHadTopAndTopPairFilterTool::finalize() {return StatusCode::SUCCESS;}

//--------------------------------------------------------------------------
  int BoostedHadTopAndTopPairFilterTool::filterFlag(double m_tHadPtCut, double m_tPairPtCut) const {


  // if true, the event pass the filter :
  int  filterCode  = 0;
  bool passTopHad  = false;
  bool passTopPair = false;

  double topPt=0.0;
  double topPx=0.0;
  double topPy=0.0;
  double topbarPt=0.0;
  double topbarPx=0.0;
  double topbarPy=0.0;

  double topChildrenPx=0.0;
  double topChildrenPy=0.0;
  double topbarChildrenPx=0.0;
  double topbarChildrenPy=0.0;

  double hadtopPt         = -1;
  double hadtopbarPt      = -1;
  double hadtopChildrenPt = -1;

  double bPx=0.0;
  double bPy=0.0;

  const xAOD::TruthEventContainer* xTruthEventContainer = 0;
  if (evtStore()->retrieve(xTruthEventContainer,m_mcName).isFailure()) {
    ATH_MSG_WARNING("could not retrieve TruthEventContainer " <<m_mcName);
    return -1;
  }
  for ( const auto* truthevent : *xTruthEventContainer ) {

    // Loop over all truth particles in the event
    for(unsigned int i = 0; i < truthevent->nTruthParticles(); i++){

      const xAOD::TruthParticle* part = truthevent->truthParticle(i);
      // In release 21 we'll have a thinned truth record in the AODs.
      // Specifically, geant particle are removed in most cases. The subsequent
      // nullptr check is supposed to catch these truth particles,
      // unfortunately however, there's no way to check whether this truth
      // particle would have had an impact on what we do further down.
      if (not part){
          // We could possibly also use break since the thinned truth particles
          // in principle should have barcode >= 200000.
          continue;
      }

      if(part->barcode() >= 200000) break;
      int pdgId = part->pdgId();

      // pdgId t quark = 6
      if ( pdgId == 6 && isFinalParticle(part) ){
        if ( part->pt() > topPt ){
          topPx = part->px();
          topPy = part->py();
          topPt = part->pt();}
      }

      if ( pdgId == -6 && isFinalParticle(part) ){
        if ( part->pt() > topbarPt ){
          topbarPx = part->px();
          topbarPy = part->py();
          topbarPt = part->pt();}
      }

     // pdgId W boson = 24
     if ( abs(pdgId) != 24 || !isFinalParticle(part) ) continue;

     // "part" is now a W boson
     bPx=PxBofW(part);
     bPy=PyBofW(part);
     if (isFromTop(part)){
        if (pdgId > 0) {
          topChildrenPx=( part->px() + bPx );
          topChildrenPy=( part->py() + bPy );
        }
        else {
          topbarChildrenPx=( part->px() + bPx );
          topbarChildrenPy=( part->py() + bPy );
        }

        if (isHadronic(part)){
          double pT = sqrt( pow( part->px() + bPx,2) + pow( part->py() + bPy,2));
          if (pT > hadtopChildrenPt){
            hadtopChildrenPt = pT;
            if (pdgId > 0) hadtopPt = topPt;
            else hadtopbarPt = topbarPt;
          }
        } // isHadronic
      } // isFromTop
    } // particle loop
  } // event loop


  double TTBarSysPt = sqrt( pow( topPx + topbarPx , 2 ) + pow( topPy + topbarPy , 2 ));
  double TTBarChildrenSysPt = sqrt( pow( topChildrenPx + topbarChildrenPx , 2 ) + pow( topChildrenPy + topbarChildrenPy , 2 ));

  if (m_cutPtOf == 0){ // cut on the pT of top on the truth list
    if (hadtopPt   >= m_tHadPtCut || hadtopbarPt >= m_tHadPtCut )   passTopHad  = true;
    if (TTBarSysPt >= m_tPairPtCut )  passTopPair = true;
  }
  else if( m_cutPtOf == 1){ // cut on the pT of top decay products (b, q, qbar') on the truth list
    if (hadtopChildrenPt   >= m_tHadPtCut  )  passTopHad  = true;
    if (TTBarChildrenSysPt >= m_tPairPtCut )  passTopPair = true;
  }


  //return which flags were passed
  if ( !passTopPair && !passTopHad)  filterCode = 0;
  if ( passTopPair && !passTopHad)   filterCode = 1;
  if ( !passTopPair && passTopHad )  filterCode = 2;
  if ( passTopPair && passTopHad )   filterCode = 3;
  return filterCode;
}

const xAOD::TruthParticle*  BoostedHadTopAndTopPairFilterTool::findInitial(const xAOD::TruthParticle* part) const{

  if(!part->nParents()) return part;

  for(unsigned int i=0; i<part->nParents(); ++i){
    const xAOD::TruthParticle* parent = part->parent(i);
    if( part->barcode() < parent->barcode()) continue; /// protection for sherpa
    if( part->pdgId() == parent->pdgId() ) return findInitial(parent);
  }

  return part;
}

//--------------------------------------------------------------------------
bool BoostedHadTopAndTopPairFilterTool::isFromTop(const xAOD::TruthParticle* part) const{

  if(!part->nParents()) return false;

  for(unsigned int i=0; i<part->nParents(); ++i){
    const xAOD::TruthParticle* parent = part->parent(i);
    if( part->barcode() < parent->barcode() ) continue; /// protection for sherpa
    if( abs( parent->pdgId() ) == 6 ) return true;
  }

  return false;
}

//--------------------------------------------------------------------------
bool BoostedHadTopAndTopPairFilterTool::isHadronic(const xAOD::TruthParticle* part) const{

  for(unsigned j = 0; j < part->nChildren(); j++){
    const xAOD::TruthParticle* child = part->child(j);
    if( part->barcode() > child->barcode() ) continue; /// protection for sherpa
    if( abs(child->pdgId()) <= 5 ) return true;
  }
  return false;
}

//--------------------------------------------------------------------------
bool BoostedHadTopAndTopPairFilterTool::isFinalParticle(const xAOD::TruthParticle* part) const{
  int type = part->pdgId();
  for(unsigned j = 0; j < part->nChildren(); j++){
    const xAOD::TruthParticle* child = part->child(j);
    if( part->barcode() > child->barcode() ) continue; /// protection for sherpa
    int childtype = child->pdgId();
    if( childtype == type ) return false;
  }

  return true;

}


//--------------------------------------------------------------------------
double BoostedHadTopAndTopPairFilterTool::PxBofW(const xAOD::TruthParticle* part) const{

  const xAOD::TruthParticle* initpart = findInitial(part);

  double px=0.0;

  for(unsigned j = 0; j < initpart->nChildren(); j++){
    const xAOD::TruthParticle* child = initpart->child(j);
    if( part->barcode() > child->barcode() ) continue; /// protection for sherpa
    if( abs( child->pdgId() ) == 5 ){
      px = child->px();
     }
  }
  return px;
}

//--------------------------------------------------------------------------
double BoostedHadTopAndTopPairFilterTool::PyBofW(const xAOD::TruthParticle* part) const{

  const xAOD::TruthParticle* initpart = findInitial(part);

  double py=0.0;

  for(unsigned j = 0; j < initpart->nChildren(); j++){
    const xAOD::TruthParticle* child = initpart->child(j);
    if( part->barcode() > child->barcode() ) continue; /// protection for sherpa
    if( abs( child->pdgId() ) == 5 ){
      py = child->py();
     }
  }
  return py;
}

} /// namespace
