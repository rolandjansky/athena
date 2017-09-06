/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/PhotonTruthTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Helpers to categorize photon TruthParticle's.
 */


#include "PhotonTruthTool.h"
#include "xAODEgamma/Photon.h"
#include "AthenaKernel/errorcheck.h"
#include <vector>
#include <cmath>


using HepGeom::Point3D;


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PhotonTruthTool::PhotonTruthTool (const std::string& type,
                                  const std::string& name,
                                  const  IInterface* parent)
  : AthAlgTool (type, name, parent),
    m_classifier ("MCTruthClassifier")
{
  declareProperty ("Classifier", m_classifier, "Classifier tool instance.");
  declareProperty ("ZTruthConv",     m_zTruthConv = 50e3);
  declareProperty ("RTruthConv",     m_rTruthConv = 800);
  declareProperty ("UseG4Particles", m_useG4Particles = false);
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode PhotonTruthTool::initialize()
{
  CHECK( AthAlgTool::initialize() );
  CHECK( m_classifier.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c queryInterface method.
 */
StatusCode
PhotonTruthTool::queryInterface( const InterfaceID& riid, void** ppvIf )
{
  if ( riid == PhotonTruthTool::interfaceID() )  {
    *ppvIf = static_cast<PhotonTruthTool*> (this);
    addRef();
    return StatusCode::SUCCESS;
  }

  return AthAlgTool::queryInterface( riid, ppvIf );
}


/**
 * @brief Go from a photon to a matching @c TruthParticle.
 * @param g The input photon.
 * @return The matching @c TruthParticle, or null.
 */
// Not const due to particleTruthClassifier().
const xAOD::TruthParticle*
PhotonTruthTool::toTruthParticle (const xAOD::Photon& g) /*const*/
{
  IMCTruthClassifier::Info info;
  m_classifier->particleTruthClassifier (&g, &info);
  return info.genPart;
}


/**
 * @brief Check a truth particle for a conversion.
 * @param truePart The particle to check.
 * @param[out] RconvMC Radius of the conversion.
 * @param[out] ZconvMC Z of the conversion.
 * @return True if this is a conversion; otherwise false.
 */
bool
PhotonTruthTool::getMCConv (const xAOD::TruthParticle* truePart,
                            float& RconvMC,
                            float& ZconvMC) const
{
  RconvMC = +9.999e+10 ;
  ZconvMC = +9.999e+10 ;
  if (!truePart) return false;

  const xAOD::TruthVertex* v = truePart->decayVtx();
  if (!v || v->nOutgoingParticles() < 2)
    return false;

  int pdgId  = truePart->pdgId();

  RconvMC = v->perp();
  ZconvMC = v->z();

  bool OKint = ( RconvMC < m_rTruthConv ) && ( fabs(ZconvMC) < m_zTruthConv );

  if ( pdgId == 22 ) { // photon
    if ( v->nOutgoingParticles() == 2 ) {
      int pdgChild[2] = {0};
      for ( unsigned u=0; u<2 ; ++u) {
        const xAOD::TruthParticle* p = v->outgoingParticle(u);
        if (p)
          pdgChild[u] = p->pdgId();
      }
      if ( pdgChild[0]+pdgChild[1]==0 && pdgChild[0]*pdgChild[1]==-121 ) {
        // gamma -> e+e-
	return OKint ;
      }
    }
  }
  else if ( std::abs(pdgId) == 11 ) { // e+/e-
    v = truePart->prodVtx();
    if ( v->nIncomingParticles()==1 && v->nOutgoingParticles()==2 ) {
      int pdgBrother[2] = {0};
      for ( unsigned u=0 ; u<2 ; ++u ) {
        const xAOD::TruthParticle* p = v->outgoingParticle(u);
        if (p)
          pdgBrother[u] = p->pdgId();
      }
      if ( pdgBrother[0]+pdgBrother[1]==(22+pdgId) &&
           pdgBrother[0]*pdgBrother[1]==(22*pdgId) )
      {
        // e(+/-) -> e(+/-)gamma
	return OKint ;
      }
    }
  }
  return false ;
}


/**
 * @brief Test for a prompt photon.
 */
bool PhotonTruthTool::isPromptPhotonMC
  (const xAOD::TruthParticle* truePart)  const
{
  return ( isFinalStatePhotonMC(truePart) && isPromptParticleMC(truePart) ) ;
}


/**
 * @brief Test for a prompt particle.
 */
bool PhotonTruthTool::isPromptParticleMC
  (const xAOD::TruthParticle* truePart)  const
{
  if ( truePart == 0 ) return false ;
  const std::vector<const xAOD::TruthParticle*> mothers =
    getMothers(truePart);
  unsigned nmothers = mothers.size() ;
  if ( nmothers == 0 ) {
    // particles with NO mother are NEVER classified as PROMPT:
    return false ;
  }
  else if ( nmothers == 1 ) {
    // particles with ONE mother are classified as PROMPT if coming
    // from non-QCD-boson decay:
    // (including exotics like heavy bosons, MSSM Higgs, graviton)
    int aPdgMother = abs(mothers[0]->pdgId());
    return  (( aPdgMother>=23 && aPdgMother<=39 ) || aPdgMother==5000039 ) ;
  }
  else {
    // particles with more mothers are classified as PROMPT
    // if they come from at least 1 parton:
    // (is this sensible?)
    int nParentPartons = 0 ;
    for ( unsigned u=0 ; u<nmothers ; ++u ) {
      int pdgMother = mothers[u]->pdgId() ;
      if ( pdgMother==21 || ( std::abs(pdgMother)<7 && pdgMother!=0 ) )
        ++nParentPartons ;
    }
    return ( nParentPartons >= 1 ) ;
  }
}


/**
 * @brief Test for a brem.
 */
bool PhotonTruthTool::isQuarkBremMC
  (const xAOD::TruthParticle* truePart)  const
{
  if ( ! isFinalStatePhotonMC(truePart) ) return false ;
  const std::vector<const xAOD::TruthParticle*> mothers =
    getMothers(truePart) ;
  if ( mothers.size() != 1 )  return false ;
  int pdgMother = mothers[0]->pdgId() ;
  return ( pdgMother==21 || ( std::abs(pdgMother)<7 && pdgMother!=0 ) ) ;
}


/**
 * @brief Test for a final-state photon.
 */
bool PhotonTruthTool::isFinalStatePhotonMC
  (const xAOD::TruthParticle* truePart) const
{
  /*
  if ( truePart == 0 ) return false ;
  if ( truePart->genParticle()->barcode() >= 200000 ) return false ;
  if ( truePart->genParticle()->status() != 1 ) return false ;
  if ( truePart->pdgId() != 22 ) return false ;
  return true ;
  */
  return ( isFinalState(truePart) && truePart->pdgId()==22 ) ;
}


/**
 * @brief Test for a final-state particle.
 */
bool
PhotonTruthTool::isFinalState(const xAOD::TruthParticle* truePart) const
{
  /*
  if ( truePart->genParticle() == 0 ) return false ;
  int status = truePart->genParticle()->status() ;
  return ( status == 1 ) ;
  */
  if ( truePart==0 ) return false ;
  // decayed in generator?
  if ( truePart->status()!=1 ) return false ; 
  // 
  if ( ! m_useG4Particles ) {
    return ( truePart->barcode() < 200000 ) ;
  }
  else {
    // if it is a photon, keep it regardless of its Geant interaction
    if ( truePart->pdgId()==22 ) return true ; 
    // reject Geant electron from conversion
    if ( abs(truePart->pdgId())==11 && truePart->barcode()>=200000 ) {
      const xAOD::TruthParticle* mother = getMother(truePart) ;
      if ( mother!=0 && mother->pdgId()==22 )  return false ;
    }

    // reject particles interacted in detector
    const xAOD::TruthVertex* v = truePart->decayVtx();
    if (!v) return false;// should never happen, but just in case...
    if ( v->nOutgoingParticles()>0 ) { 
      if ( v->perp()<m_rTruthConv 
	   && fabs(v->z())<m_zTruthConv )  return false ;
    }
  } // end if ( ! m_useG4Particles ) 
  return true ;
}


/**
 * @brief Get the mother vertex for @a p.
 */
const xAOD::TruthVertex*
PhotonTruthTool::getMotherVert(const xAOD::TruthParticle* p) const
{
  const xAOD::TruthVertex* v = p->prodVtx();

  // if mother is a duplicate, try climbing up the tree by one step...
  while (v && v->nIncomingParticles() == 1 &&
         v->incomingParticle(0)->pdgId() == p->pdgId())
  {
    p = v->incomingParticle(0);
    v = p->prodVtx();
  }

  return v;
}


/**
 * @brief Get the (first) mother particle of @a p.
 */
const xAOD::TruthParticle*
PhotonTruthTool::getMother(const xAOD::TruthParticle* p) const
{
  const xAOD::TruthVertex* v = getMotherVert (p);

  if (!v || v->nIncomingParticles() == 0)
    return 0;

  return v->incomingParticle(0);
}


/**
 * @brief Return list of mother particles of @a p.
 */
std::vector<const xAOD::TruthParticle*>
PhotonTruthTool::getMothers(const xAOD::TruthParticle* p) const
{
  std::vector<const xAOD::TruthParticle*> out;
  const xAOD::TruthVertex* v = getMotherVert (p);

  if (v) {
    int n = v->nIncomingParticles();
    out.reserve (n);
    for (int i = 0; i < n; i++)
      out.push_back (v->incomingParticle(i));
  }

  return out;
}


} // namespace D3PD
