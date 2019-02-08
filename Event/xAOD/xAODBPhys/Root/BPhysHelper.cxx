/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file   BPhysHelper.cxx
 *  @author Daniel Scheirich <daniel.scheirich@cern.ch>
 */

#include "xAODBPhys/BPhysHelper.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

/*****************************************************************************/
/** @{
 *  Some useful typedefs
 */

typedef ElementLink<xAOD::VertexContainer> VertexLink;
typedef std::vector<VertexLink> VertexLinkVector;
typedef ElementLink<xAOD::MuonContainer> MuonLink;
typedef std::vector<MuonLink> MuonLinkVector;
typedef ElementLink<xAOD::ElectronContainer> ElectronLink;
typedef std::vector<ElectronLink> ElectronLinkVector;

/** @} */

/*****************************************************************************/

/** @{
 *  Some useful static consts
 */

const unsigned int  xAOD::BPhysHelper::n_pv_types = 4;
const std::string   xAOD::BPhysHelper::pv_type_str[] = 
  {"PV_MAX_SUM_PT2", "PV_MIN_A0", "PV_MIN_Z0", "PV_MIN_Z0_BA"};

/*****************************************************************************/

/** @{
 *  Some useful macros
 */

/*****************************************************************************/
#define GET_PV( name )                                          \
{                                                               \
  static SG::AuxElement::Accessor<VertexLink> pvLinkAcc(name);  \
  if(!pvLinkAcc.isAvailable(*m_b)) {                            \
    return 0;                                                   \
  }                                                             \
  const VertexLink& pvLink = pvLinkAcc(*m_b);                   \
  if(!pvLink.isValid()) {                                       \
    return 0;                                                   \
  }                                                             \
  return *pvLink;                                               \
}
/*****************************************************************************/
#define SET_PV( name, pv, vertexContainer )                       \
{                                                                 \
  static SG::AuxElement::Decorator<VertexLink> pvLinkDecor(name); \
  VertexLink vertexLink;                                          \
  if(pv) {                                                        \
  vertexLink.setElement(pv);                                      \
  vertexLink.setStorableObject(*vertexContainer);                 \
  if( !vertexLink.isValid() )                                     \
    return false;}                                                \
  pvLinkDecor(*m_b) = vertexLink;                                 \
  return true;                                                    \
}
/*****************************************************************************/
#define GET_FLOAT(name)                                    \
{                                                          \
  static SG::AuxElement::Accessor<float> floatAcc(name);   \
  if(!floatAcc.isAvailable(*m_b)) return -9999999.;        \
  return floatAcc(*m_b);                                   \
}
/*****************************************************************************/
#define SET_FLOAT( name, val)                              \
{                                                          \
  static SG::AuxElement::Decorator<float> floatDec(name);  \
  floatDec(*m_b) = val;                                    \
  return true;                                             \
}
/*****************************************************************************/
#define GET_INT(name)                                  \
{                                                      \
  static SG::AuxElement::Accessor<int> intAcc(name);   \
  if(!intAcc.isAvailable(*m_b)) return -9999999;       \
  return intAcc(*m_b);                                 \
}
/*****************************************************************************/
#define SET_INT( name, val)                            \
{                                                      \
  static SG::AuxElement::Decorator<int> intDec(name);  \
  intDec(*m_b) = val;                                  \
  return true;                                         \
}
/*****************************************************************************/

/** @} */

/*****************************************************************************/
const TMatrixTSym<double>& xAOD::BPhysHelper::covariance()
{
  
  // cache covariance matrix
  if(!cacheCov())
    return xAOD::BPhysHelper::emptyMatrix;
  
  // all OK:
  return m_cachedCov;    
}

/*****************************************************************************/
int xAOD::BPhysHelper::nRefTrks()
{
  // cache refitted tracks
  if( !cacheRefTracks() ) return -1;
  
  // all OK:
  return m_cachedRefTracks.size();
  
}

/*****************************************************************************/
TVector3 xAOD::BPhysHelper::refTrk(const size_t index)
{
  // cache refitted tracks
  if( !cacheRefTracks() ) return 
      TVector3(0,0,0);

  // range check:
  if(index>=m_cachedRefTracks.size())
    return TVector3(0,0,0);
  
  // all OK:
  return m_cachedRefTracks[index];
  
}

/*****************************************************************************/
const std::vector<TVector3>& xAOD::BPhysHelper::refTrks()
{
  // cache refitted tracks
  if( !cacheRefTracks() ) 
    return xAOD::BPhysHelper::emptyVectorOfTVector3;

  // return
  return m_cachedRefTracks;  
}

/*****************************************************************************/
TLorentzVector xAOD::BPhysHelper::refTrk(const size_t index, const float mass)
{
  // cache refitted tracks
  if( !cacheRefTracks() ) return 
      TLorentzVector(0,0,0,0);

  // range check:
  if(index>=m_cachedRefTracks.size())
    return TLorentzVector(0,0,0,0);
  
  // all OK, create the 4-momentum
  TLorentzVector mom;
  mom.SetVectM(m_cachedRefTracks[index], mass);
  return mom;
  
}

/*****************************************************************************/
const xAOD::IParticle* xAOD::BPhysHelper::refTrkOrigin(const size_t index) const
{
  // range check:
  // FIXME: add neutral particles once they are available
  //if(index>=m_b->nTrackParticles()+m_b->nNeutralParticles())
  //  return 0;

  // FIXME:supports charged particles only for now  
  if(index>=m_b->nTrackParticles())
    return 0;
  
  // charged tracks
  if(index<m_b->nTrackParticles()) {
    return m_b->trackParticle(index);
  } else  {
    // FIXME: add neutral particles once they are available
    //return m_b->neutralParticle(index - m_b->nNeutralParticles());
    return 0;
  }
    
}

/*****************************************************************************/
TVector3 xAOD::BPhysHelper::refTrkOriginP(const size_t index) const
{
  // range check:
  // FIXME: add neutral particles once they are available
  //if(index>=m_b->nTrackParticles()+m_b->nNeutralParticles())
  //  return TVector3(0,0,0);
  
  // FIXME:supports charged particles only for now  
  if(index>=m_b->nTrackParticles())
    return TVector3(0,0,0);
  
  if(index<m_b->nTrackParticles()) {
    // charged track
    const xAOD::TrackParticle* trk = dynamic_cast<const xAOD::TrackParticle*>(refTrkOrigin(index));
    
    // null-pointer protection
    if(!trk) return TVector3(0,0,0);
    
    // all OK
    TVector3 tmp;
    tmp.SetPtEtaPhi(trk->pt(), trk->eta(), trk->phi());
    return tmp;
    
  }else{
    // // neutral track
    // const xAOD::NeutralParticle* trk = dynamic_cast<const xAOD::NeutralParticle*>(refTrkOrigin(index));
       
    // // null-pointer protection
    // if(!trk) return TVector3(0,0,0);
       
    // // all OK
    // TVector3 tmp;
    // tmp.SetPtEtaPhi(trk->pt(), trk->eta(), trk->phi());
    // return tmp;
    
    // FIXME neutral tracks not implemented yet:
    return TVector3(0,0,0);
  }
  
}

/*****************************************************************************/
TLorentzVector xAOD::BPhysHelper::refTrkOriginP(const size_t index, const float mass) const
{
  // range check:
  // FIXME: add neutral particles once they are available
  //if(index>=m_b->nTrackParticles()+m_b->nNeutralParticles())
  //  return TVector3(0,0,0);
  
  // FIXME:supports charged particles only for now  
  if(index>=m_b->nTrackParticles())
    return TLorentzVector(0,0,0,0);
  
  // create TLorentzVector
  TLorentzVector tmp;
  tmp.SetVectM(refTrkOriginP(index), mass);
  return tmp;
  
}
      
/*****************************************************************************/
float xAOD::BPhysHelper::refTrkCharge(const size_t index) const
{
  // range check:
  // FIXME: add neutral particles once they are available
  //if(index>=m_b->nTrackParticles()+m_b->nNeutralParticles())
  //  return 0;
  
  // FIXME:supports charged particles only for now  
  if(index>=m_b->nTrackParticles())
    return -9999.;
  
  if(index<m_b->nTrackParticles()) {
    // charged track
    const xAOD::TrackParticle* trk = dynamic_cast<const xAOD::TrackParticle*>(refTrkOrigin(index));
    
    // null-pointer protection
    if(!trk)
      return -9999.;
    
    // all OK
    return trk->charge();
    
  }else{
    // we do not need to do anything here, since we know this track must be neutral
    return 0.;
  }
    
  
}

/*****************************************************************************/
bool xAOD::BPhysHelper::setRefTrks(const std::vector<float>& px,
                                   const std::vector<float>& py,
                                   const std::vector<float>& pz)
{
  // sanity check:
  if(px.size()!=py.size() || px.size()!=pz.size())
    return false;
  
  // erase refitted track cache to preserve consistency
  m_refTracksCached = false;
  m_cachedRefTracks.clear();
  
  // create decorators
  static SG::AuxElement::Decorator< std::vector<float> > refTrackPxDeco("RefTrackPx");
  static SG::AuxElement::Decorator< std::vector<float> > refTrackPyDeco("RefTrackPy");
  static SG::AuxElement::Decorator< std::vector<float> > refTrackPzDeco("RefTrackPz");
  
  // store the elements:
  refTrackPxDeco(*m_b) = px;
  refTrackPyDeco(*m_b) = py;
  refTrackPzDeco(*m_b) = pz;
  
  return true;
}

/*****************************************************************************/
bool xAOD::BPhysHelper::setRefTrks(const std::vector<TVector3>& refTrks)
{
  // sanity check
  if(refTrks.empty())
    return false;
  
  // tmp vectors of px,py,pz components:
  std::vector<float> px;  
  std::vector<float> py;  
  std::vector<float> pz;
  
  // loop over refitted track momenta and store the components
  std::vector<TVector3>::const_iterator refTrksItr = refTrks.begin();
  for(; refTrksItr!=refTrks.end(); ++refTrksItr) {
    px.push_back( refTrksItr->Px() );
    py.push_back( refTrksItr->Py() );
    pz.push_back( refTrksItr->Pz() );
  }
  
  // call overloaded method:
  return setRefTrks(px,py,pz);

}

/*****************************************************************************/
#ifndef XAOD_ANALYSIS

bool xAOD::BPhysHelper::setRefTrks()
{
  // refitted momenta
  std::vector<float> px;
  std::vector<float> py;
  std::vector<float> pz;
  
  // loop over refitted tracks at vertex
  for(uint i=0; i<vtx()->vxTrackAtVertex().size(); ++i) {
    const Trk::TrackParameters* aPerigee = vtx()->vxTrackAtVertex()[i].perigeeAtVertex();
    //sanity check
    if(!aPerigee) 
      return false;
    
    // store individual momentum components
    px.push_back( aPerigee->momentum()[Trk::px] );
    py.push_back( aPerigee->momentum()[Trk::py] );
    pz.push_back( aPerigee->momentum()[Trk::pz] );
  }
  
  // store as augmentation:
  setRefTrks(px, py, pz);
  
  // all OK
  return true;
  
}

#endif // not XAOD_ANALYSIS
/*****************************************************************************/
TVector3 xAOD::BPhysHelper::totalP()
{
  // cache refitted tracks
  if( !cacheRefTracks() && nCascadeVertices()<=0) {
    // In case of cascade decays, vector with no refitted tracks can still be correct.
    // Therefore, return error only is this is not the case
    return TVector3(0,0,0);
  }
  
  // sum the momenta
  TVector3 sum(0,0,0);
  int n = nRefTrks();
  for(int i=0; i<n; ++i) {
    sum += refTrk(i);
  }
  
  // treat cascade vertices, if they exist
  if(nCascadeVertices()>0) {
    for(int i=0; i<nCascadeVertices(); ++i) {
      // create helper class for the daughter vertex
      xAOD::BPhysHelper daughterVtx( cascadeVertex(i) );
      TVector3 daughterP = daughterVtx.totalP();
      
      // check that the daughter total momentum was retrieved correctly
      if(daughterP==TVector3(0,0,0))
        return TVector3(0,0,0);
      
      //OK, sum the momenta
      sum += daughterP;
    }
  }
  
  return sum;
}

/*****************************************************************************/
TLorentzVector xAOD::BPhysHelper::totalP(const std::vector<double>& masses)
{
  // cache refitted tracks
  if( !cacheRefTracks() ) 
    return TLorentzVector(0,0,0,0);
  int n = nRefTrks();
  // input check
  if(int(masses.size()) != n)
    return TLorentzVector(0,0,0,0);
  
  // sum the 4-momenta
  TLorentzVector sum;
  for(int i=0; i<n; ++i) {
    sum += refTrk(i, masses[i]);
  }
  
  return sum;
   
}

/*****************************************************************************/
float xAOD::BPhysHelper::ptErr()
{
  GET_FLOAT("PtErr");
}
/*****************************************************************************/
bool xAOD::BPhysHelper::setPtErr(const float val)
{
  SET_FLOAT("PtErr", val);
}
/*****************************************************************************/
int xAOD::BPhysHelper::nMuons()
{
  // cache linked muons
  if(!cacheMuons())
    return -1;
  
  // all OK:
  return m_cachedMuons.size();
}

/*****************************************************************************/
const xAOD::Muon* xAOD::BPhysHelper::muon(const size_t index)
{
  // cache linked muons
  if(!cacheMuons())
    return 0;
  
  // range check
  if(index>=m_cachedMuons.size())
    return 0;
  
  // all OK:
  return m_cachedMuons[index];
  
}

/*****************************************************************************/
const std::vector<const xAOD::Muon*>& xAOD::BPhysHelper::muons()
{
  // cache linked muons
  if(!cacheMuons())
    return xAOD::BPhysHelper::emptyVectorOfMuons;
  
  // all OK:
  return m_cachedMuons;  
}

/*****************************************************************************/
bool xAOD::BPhysHelper::setMuons(const std::vector<const xAOD::Muon*>& muons,
                                     const xAOD::MuonContainer* muonContainer)
{
  // erase muon cache to preserve cosistency
  m_muonsCached = false;
  m_cachedMuons.clear();
  
  // Create muon links decorator 
  static SG::AuxElement::Decorator<MuonLinkVector> muonLinksDecor("MuonLinks"); 
  
  // create tmp vector of muon links
  MuonLinkVector muonLinks;

  // loop over input muons  
  std::vector<const xAOD::Muon*>::const_iterator muonsItr = muons.begin();
  for(; muonsItr!=muons.end(); ++muonsItr) {
    // sanity check 1: protect against null pointers
    if( !(*muonsItr) )
      return false;
    
    // create element link
    ElementLink<xAOD::MuonContainer> muLink;
    muLink.setElement(*muonsItr);
    muLink.setStorableObject(*muonContainer);
    
    // sanity check 2: is the link valid?
    if( !muLink.isValid() )
      return false;
    
    // link is OK, store it in the tmp vector
    muonLinks.push_back( muLink );
    
  } // end of loop over muons
  
  // all OK: store muon links in the aux store
  muonLinksDecor(*m_b) = muonLinks;
  
  return true;
  
}

/*****************************************************************************/
int xAOD::BPhysHelper::nElectrons()
{
  // cache linked electrons
  if(!cacheElectrons())
    return -1;
  
  // all OK:
  return m_cachedElectrons.size();
}

/*****************************************************************************/
const xAOD::Electron* xAOD::BPhysHelper::electron(const size_t index)
{
  // cache linked electrons
  if(!cacheElectrons())
    return 0;
  
  // range check
  if(index>=m_cachedElectrons.size())
    return 0;
  
  // all OK:
  return m_cachedElectrons[index];
  
}

/*****************************************************************************/
const std::vector<const xAOD::Electron*>& xAOD::BPhysHelper::electrons()
{
  // cache linked electrons
  if(!cacheElectrons())
    return xAOD::BPhysHelper::emptyVectorOfElectrons;
  
  // all OK:
  return m_cachedElectrons;  
}

/*****************************************************************************/
bool xAOD::BPhysHelper::setElectrons(const std::vector<const xAOD::Electron*>& electrons,
                                     const xAOD::ElectronContainer* electronContainer)
{
  // erase electron cache to preserve cosistency
  m_electronsCached = false;
  m_cachedElectrons.clear();
  
  // Create electron links decorator 
  static SG::AuxElement::Decorator<ElectronLinkVector> electronLinksDecor("ElectronLinks"); 
  
  // create tmp vector of electron links
  ElectronLinkVector electronLinks;

  // loop over input electrons  
  std::vector<const xAOD::Electron*>::const_iterator electronsItr = electrons.begin();
  for(; electronsItr!=electrons.end(); ++electronsItr) {
    // sanity check 1: protect against null pointers
    if( !(*electronsItr) )
      return false;
    
    // create element link
    ElementLink<xAOD::ElectronContainer> elLink;
    elLink.setElement(*electronsItr);
    elLink.setStorableObject(*electronContainer);
    
    // sanity check 2: is the link valid?
    if( !elLink.isValid() )
      return false;
    
    // link is OK, store it in the tmp vector
    electronLinks.push_back( elLink );
    
  } // end of loop over electrons
  
  // all OK: store electron links in the aux store
  electronLinksDecor(*m_b) = electronLinks;
  
  return true;
  
}

/*****************************************************************************/
int xAOD::BPhysHelper::nPrecedingVertices()
{
  // cache linked preceding vertices
  if(!cachePrecedingVertices())
    return -1;
  
  // all OK:
  return m_cachedPrecedingVertices.size();
   
}
      
/*****************************************************************************/
const xAOD::Vertex* xAOD::BPhysHelper::precedingVertex(const size_t index)
{
  // cache linked precedingVertices
  if(!cachePrecedingVertices())
    return 0;
  
  // range check
  if(index>=m_cachedPrecedingVertices.size())
    return 0;
  
  // all OK:
  return m_cachedPrecedingVertices[index];
  
}

/*****************************************************************************/
const std::vector<const xAOD::Vertex*>& xAOD::BPhysHelper::precedingVertices()
{
  // cache linked precedingVertices
  if(!cachePrecedingVertices())
    return xAOD::BPhysHelper::emptyVectorOfVertices;
  
  // all OK:
  return m_cachedPrecedingVertices;  
  
}

/*****************************************************************************/
bool xAOD::BPhysHelper::setPrecedingVertices(const std::vector<const xAOD::Vertex*>& vertices,
                                                 const xAOD::VertexContainer* vertexContainer)
{
  // erase precedingVertex cache to preserve consistency
  m_precedingVerticesCached = false;
  m_cachedPrecedingVertices.clear();
  
  // Create preceding vertex links decorator 
  static SG::AuxElement::Decorator<VertexLinkVector> precedingVertexLinksDecor("PrecedingVertexLinks"); 
  
  // create tmp vector of preceding vertex links
  VertexLinkVector precedingVertexLinks;

  // loop over input precedingVertices  
  std::vector<const xAOD::Vertex*>::const_iterator precedingVerticesItr = vertices.begin();
  for(; precedingVerticesItr!=vertices.end(); ++precedingVerticesItr) {
    // sanity check 1: protect against null pointers
    if( !(*precedingVerticesItr) )
      return false;
    
    // create element link
    VertexLink vertexLink;
    vertexLink.setElement(*precedingVerticesItr);
    vertexLink.setStorableObject(*vertexContainer);
    
    // sanity check 2: is the link valid?
    if( !vertexLink.isValid() )
      return false;
    
    // link is OK, store it in the tmp vector
    precedingVertexLinks.push_back( vertexLink );
    
  } // end of loop over preceding vertices
  
  // all OK: store preceding vertex links in the aux store
  precedingVertexLinksDecor(*m_b) = precedingVertexLinks;
  
  return true;
  
}
/*****************************************************************************/
int xAOD::BPhysHelper::nCascadeVertices()
{
  // cache linked cascade vertices
  if(!cacheCascadeVertices())
    return -1;
  
  // all OK:
  return m_cachedCascadeVertices.size();
   
}
      
/*****************************************************************************/
const xAOD::Vertex* xAOD::BPhysHelper::cascadeVertex(const size_t index)
{
  // cache linked cascadeVertices
  if(!cacheCascadeVertices())
    return 0;
  
  // range check
  if(index>=m_cachedCascadeVertices.size())
    return 0;
  
  // all OK:
  return m_cachedCascadeVertices[index];
  
}

/*****************************************************************************/
const std::vector<const xAOD::Vertex*>& xAOD::BPhysHelper::cascadeVertices()
{
  // cache linked cascadeVertices
  if(!cacheCascadeVertices())
    return xAOD::BPhysHelper::emptyVectorOfVertices;
  
  // all OK:
  return m_cachedCascadeVertices;  
  
}

/*****************************************************************************/
bool xAOD::BPhysHelper::setCascadeVertices(const std::vector<const xAOD::Vertex*>& vertices,
                                                 const xAOD::VertexContainer* vertexContainer)
{
  // erase cascadeVertex cache to preserve consistency
  m_cascadeVerticesCached = false;
  m_cachedCascadeVertices.clear();
  
  // Create cascade vertex links decorator 
  static SG::AuxElement::Decorator<VertexLinkVector> cascadeVertexLinksDecor("CascadeVertexLinks"); 
  
  // create tmp vector of cascade vertex links
  VertexLinkVector cascadeVertexLinks;

  // loop over input cascadeVertices  
  std::vector<const xAOD::Vertex*>::const_iterator cascadeVerticesItr = vertices.begin();
  for(; cascadeVerticesItr!=vertices.end(); ++cascadeVerticesItr) {
    // sanity check 1: protect against null pointers
    if( !(*cascadeVerticesItr) )
      return false;
    
    // create element link
    VertexLink vertexLink;
    vertexLink.setElement(*cascadeVerticesItr);
    vertexLink.setStorableObject(*vertexContainer);
    
    // sanity check 2: is the link valid?
    if( !vertexLink.isValid() )
      return false;
    
    // link is OK, store it in the tmp vector
    cascadeVertexLinks.push_back( vertexLink );
    
  } // end of loop over cascade vertices
  
  // all OK: store cascade vertex links in the aux store
  cascadeVertexLinksDecor(*m_b) = cascadeVertexLinks;
  
  return true;
  
}

/*****************************************************************************/
int xAOD::BPhysHelper::nRefTrksCascade()
{
  // get number of refitted tracks in this vertex
  int n = nRefTrks();
  
  // check for errors. If there are cascade vertices stored in this vertex, 
  // it is possible there are no refitted tracks, so report error only in case 
  // when this veretx has no cascade vertices and no refitted tracks:
  if(n<0 && nCascadeVertices()<=0) {
    return -1;
  } else if(n<0 && nCascadeVertices()>0) {
    n = 0;
  }
  
  // loop over cascade vertices, if they exist:
  for(int i=0; i<nCascadeVertices(); ++i) {
    // create helper class for the daughter vertex
    xAOD::BPhysHelper daughterVtx(cascadeVertex(i));
    
    // check for errors
    if(daughterVtx.nRefTrksCascade()<0)
      return -1;
    
    // OK, add the numbers
    n += daughterVtx.nRefTrksCascade();
  }
  
  
  // all OK, return the number of tracks
  return n;
}
/*****************************************************************************/
const xAOD::Vertex* xAOD::BPhysHelper::pv(const pv_type vertexType) 
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : GET_PV("PvMaxSumPt2Link");
    case PV_MIN_A0      : GET_PV("PvMinA0Link");
    case PV_MIN_Z0      : GET_PV("PvMinZ0Link");
    case PV_MIN_Z0_BA   : GET_PV("PvMinZ0BALink");
    default: return 0;
  }      
}        
/*****************************************************************************/
const xAOD::Vertex* xAOD::BPhysHelper::origPv(const pv_type vertexType) 
{        
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : GET_PV("OrigPvMaxSumPt2Link");
    case PV_MIN_A0      : GET_PV("OrigPvMinA0Link");
    case PV_MIN_Z0      : GET_PV("OrigPvMinZ0Link");
    case PV_MIN_Z0_BA   : GET_PV("OrigPvMinZ0BALink");
    default: return 0;
  } 
}
/*****************************************************************************/
bool xAOD::BPhysHelper::setOrigPv(const xAOD::Vertex* pv, 
                              const xAOD::VertexContainer* vertexContainer,
                              const pv_type vertexType)
{        
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : SET_PV("OrigPvMaxSumPt2Link", pv, vertexContainer);
    case PV_MIN_A0      : SET_PV("OrigPvMinA0Link"    , pv, vertexContainer);
    case PV_MIN_Z0      : SET_PV("OrigPvMinZ0Link"    , pv, vertexContainer);
    case PV_MIN_Z0_BA   : SET_PV("OrigPvMinZ0BALink"  , pv, vertexContainer);
    default: return 0;
  } 
}
/*****************************************************************************/
bool xAOD::BPhysHelper::setPv(const xAOD::Vertex* pv, 
                              const xAOD::VertexContainer* vertexContainer,
                              const pv_type vertexType)
{        
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : SET_PV("PvMaxSumPt2Link", pv, vertexContainer);
    case PV_MIN_A0      : SET_PV("PvMinA0Link"    , pv, vertexContainer);
    case PV_MIN_Z0      : SET_PV("PvMinZ0Link"    , pv, vertexContainer);
    case PV_MIN_Z0_BA   : SET_PV("PvMinZ0BALink"  , pv, vertexContainer);
    default: return false;
  }
}
/*****************************************************************************/
bool xAOD::BPhysHelper::setRefitPVStatus(int code, const pv_type vertexType)
{        
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : SET_INT("PvMaxSumPt2Status", code);
    case PV_MIN_A0      : SET_INT("PvMinA0Status"    , code);
    case PV_MIN_Z0      : SET_INT("PvMinZ0Status"    , code);
    case PV_MIN_Z0_BA   : SET_INT("PvMinZ0BAStatus"  , code);
    default: return 0;
  } 
}
/*****************************************************************************/
int xAOD::BPhysHelper::RefitPVStatus(const pv_type vertexType)
{        
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : GET_INT("PvMaxSumPt2Status");
    case PV_MIN_A0      : GET_INT("PvMinA0Status"    );
    case PV_MIN_Z0      : GET_INT("PvMinZ0Status"    );
    case PV_MIN_Z0_BA   : GET_INT("PvMinZ0BAStatus"  );
    default: return -999999;
  }
}
/*****************************************************************************/
float xAOD::BPhysHelper::lxy(const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : GET_FLOAT("LxyMaxSumPt2");
    case PV_MIN_A0      : GET_FLOAT("LxyMinA0");
    case PV_MIN_Z0      : GET_FLOAT("LxyMinZ0");
    case PV_MIN_Z0_BA   : GET_FLOAT("LxyMinZ0BA");
    default: return -9999999.;
  }
}
/*****************************************************************************/
float xAOD::BPhysHelper::lxyErr(const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : GET_FLOAT("LxyErrMaxSumPt2");
    case PV_MIN_A0      : GET_FLOAT("LxyErrMinA0");
    case PV_MIN_Z0      : GET_FLOAT("LxyErrMinZ0");
    case PV_MIN_Z0_BA   : GET_FLOAT("LxyErrMinZ0BA");
    default: return -9999999.;
  }
}
/*****************************************************************************/
bool xAOD::BPhysHelper::setLxy(const float val, const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : SET_FLOAT("LxyMaxSumPt2", val);
    case PV_MIN_A0      : SET_FLOAT("LxyMinA0"    , val);
    case PV_MIN_Z0      : SET_FLOAT("LxyMinZ0"    , val);
    case PV_MIN_Z0_BA   : SET_FLOAT("LxyMinZ0BA"  , val);
    default: return false;
  }
}
/*****************************************************************************/
bool xAOD::BPhysHelper::setLxyErr(const float val, const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : SET_FLOAT("LxyErrMaxSumPt2", val);
    case PV_MIN_A0      : SET_FLOAT("LxyErrMinA0"    , val);
    case PV_MIN_Z0      : SET_FLOAT("LxyErrMinZ0"    , val);
    case PV_MIN_Z0_BA   : SET_FLOAT("LxyErrMinZ0BA"  , val);
    default: return false;
  }
}

//3dLxyz
/*****************************************************************************/
float xAOD::BPhysHelper::lxyz(const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : GET_FLOAT("LxyzMaxSumPt2");
    case PV_MIN_A0      : GET_FLOAT("LxyzMinA0");
    case PV_MIN_Z0      : GET_FLOAT("LxyzMinZ0");
    case PV_MIN_Z0_BA   : GET_FLOAT("LxyzMinZ0BA");
    default: return -9999999.;
  }
}
/*****************************************************************************/
float xAOD::BPhysHelper::lxyzErr(const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : GET_FLOAT("LxyzErrMaxSumPt2");
    case PV_MIN_A0      : GET_FLOAT("LxyzErrMinA0");
    case PV_MIN_Z0      : GET_FLOAT("LxyzErrMinZ0");
    case PV_MIN_Z0_BA   : GET_FLOAT("LxyzErrMinZ0BA");
    default: return -9999999.;
  }
}
/*****************************************************************************/
bool xAOD::BPhysHelper::setLxyz(const float val, const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : SET_FLOAT("LxyzMaxSumPt2", val);
    case PV_MIN_A0      : SET_FLOAT("LxyzMinA0"    , val);
    case PV_MIN_Z0      : SET_FLOAT("LxyzMinZ0"    , val);
    case PV_MIN_Z0_BA   : SET_FLOAT("LxyzMinZ0BA"  , val);
    default: return false;
  }
}
/*****************************************************************************/
bool xAOD::BPhysHelper::setLxyzErr(const float val, const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : SET_FLOAT("LxyzErrMaxSumPt2", val);
    case PV_MIN_A0      : SET_FLOAT("LxyzErrMinA0"    , val);
    case PV_MIN_Z0      : SET_FLOAT("LxyzErrMinZ0"    , val);
    case PV_MIN_Z0_BA   : SET_FLOAT("LxyzErrMinZ0BA"  , val);
    default: return false;
  }
}

/*****************************************************************************/
float xAOD::BPhysHelper::a0     (const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : GET_FLOAT("A0MaxSumPt2");
    case PV_MIN_A0      : GET_FLOAT("A0MinA0");
    case PV_MIN_Z0      : GET_FLOAT("A0MinZ0");
    case PV_MIN_Z0_BA   : GET_FLOAT("A0MinZ0BA");
    default: return -9999999.;
  }
}
/*****************************************************************************/
float xAOD::BPhysHelper::a0Err  (const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : GET_FLOAT("A0ErrMaxSumPt2");
    case PV_MIN_A0      : GET_FLOAT("A0ErrMinA0");
    case PV_MIN_Z0      : GET_FLOAT("A0ErrMinZ0");
    case PV_MIN_Z0_BA   : GET_FLOAT("A0ErrMinZ0BA");
    default: return -9999999.;
  }
}
/*****************************************************************************/
float xAOD::BPhysHelper::a0xy   (const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : GET_FLOAT("A0xyMaxSumPt2");
    case PV_MIN_A0      : GET_FLOAT("A0xyMinA0");
    case PV_MIN_Z0      : GET_FLOAT("A0xyMinZ0");
    case PV_MIN_Z0_BA   : GET_FLOAT("A0xyMinZ0BA");
    default: return -9999999.;
  }  
}
/*****************************************************************************/
float xAOD::BPhysHelper::a0xyErr(const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : GET_FLOAT("A0xyErrMaxSumPt2");
    case PV_MIN_A0      : GET_FLOAT("A0xyErrMinA0");
    case PV_MIN_Z0      : GET_FLOAT("A0xyErrMinZ0");
    case PV_MIN_Z0_BA   : GET_FLOAT("A0xyErrMinZ0BA");
    default: return -9999999.;
  }    
}
/*****************************************************************************/
float xAOD::BPhysHelper::z0     (const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : GET_FLOAT("Z0MaxSumPt2");
    case PV_MIN_A0      : GET_FLOAT("Z0MinA0");
    case PV_MIN_Z0      : GET_FLOAT("Z0MinZ0");
    case PV_MIN_Z0_BA   : GET_FLOAT("Z0MinZ0BA");
    default: return -9999999.;
  }
}
/*****************************************************************************/
float xAOD::BPhysHelper::z0Err  (const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : GET_FLOAT("Z0ErrMaxSumPt2");
    case PV_MIN_A0      : GET_FLOAT("Z0ErrMinA0");
    case PV_MIN_Z0      : GET_FLOAT("Z0ErrMinZ0");
    case PV_MIN_Z0_BA   : GET_FLOAT("Z0ErrMinZ0BA");
    default: return -9999999.;
  }
}
/*****************************************************************************/
float xAOD::BPhysHelper::setA0     (const float val, const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : SET_FLOAT("A0MaxSumPt2", val);
    case PV_MIN_A0      : SET_FLOAT("A0MinA0"    , val);
    case PV_MIN_Z0      : SET_FLOAT("A0MinZ0"    , val);
    case PV_MIN_Z0_BA   : SET_FLOAT("A0MinZ0BA"  , val);
    default: return false;
  }  
}
/*****************************************************************************/
float xAOD::BPhysHelper::setA0Err  (const float val, const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : SET_FLOAT("A0ErrMaxSumPt2", val);
    case PV_MIN_A0      : SET_FLOAT("A0ErrMinA0"    , val);
    case PV_MIN_Z0      : SET_FLOAT("A0ErrMinZ0"    , val);
    case PV_MIN_Z0_BA   : SET_FLOAT("A0ErrMinZ0BA"  , val);
    default: return false;
  }    
}
/*****************************************************************************/
float xAOD::BPhysHelper::setA0xy   (const float val, const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : SET_FLOAT("A0xyMaxSumPt2", val);
    case PV_MIN_A0      : SET_FLOAT("A0xyMinA0"    , val);
    case PV_MIN_Z0      : SET_FLOAT("A0xyMinZ0"    , val);
    case PV_MIN_Z0_BA   : SET_FLOAT("A0xyMinZ0BA"  , val);
    default: return false;
  }    
}
/*****************************************************************************/
float xAOD::BPhysHelper::setA0xyErr(const float val, const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : SET_FLOAT("A0xyErrMaxSumPt2", val);
    case PV_MIN_A0      : SET_FLOAT("A0xyErrMinA0"    , val);
    case PV_MIN_Z0      : SET_FLOAT("A0xyErrMinZ0"    , val);
    case PV_MIN_Z0_BA   : SET_FLOAT("A0xyErrMinZ0BA"  , val);
    default: return false;
  }    
}
/*****************************************************************************/
float xAOD::BPhysHelper::setZ0     (const float val, const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : SET_FLOAT("Z0MaxSumPt2", val);
    case PV_MIN_A0      : SET_FLOAT("Z0MinA0"    , val);
    case PV_MIN_Z0      : SET_FLOAT("Z0MinZ0"    , val);
    case PV_MIN_Z0_BA   : SET_FLOAT("Z0MinZ0BA"  , val);
    default: return false;
  }  
}
/*****************************************************************************/
float xAOD::BPhysHelper::setZ0Err  (const float val, const pv_type vertexType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2 : SET_FLOAT("Z0ErrMaxSumPt2", val);
    case PV_MIN_A0      : SET_FLOAT("Z0ErrMinA0"    , val);
    case PV_MIN_Z0      : SET_FLOAT("Z0ErrMinZ0"    , val);
    case PV_MIN_Z0_BA   : SET_FLOAT("Z0ErrMinZ0BA"  , val);
    default: return false;
  }    
}
/*****************************************************************************/
bool xAOD::BPhysHelper::cacheCov()
{
  // if cov matrix is already cached, do nothing and return true 
  if(m_covCached && m_cachedCov.GetNrows()!=0)
    return true;
 
  // if cov matrix is cached but the cache is empty, return false (error)
  if(m_covCached && m_cachedCov.GetNrows()==0)
    return false;
  
  // get number of tracks
  // if ref tracks are stripped from the vertex, this will not work and
  // error will be returned
  const int nTrk = nRefTrks();
  if(nTrk<0)
    return false;
  
  // Covariance matrix not cached, yet. Retrieve them from 
  // the vertex and convert:
  m_covCached = true;
    
  // retrieve native covariance matrix
  const std::vector<float>& cov = m_b->covariance();
  
  // convert covariance matrix
  if(int(cov.size())==(3*nTrk+3)*(3*nTrk+3+1)/2) {
    // this is a VKalVrtFitter covariance matrix
    m_cachedCov.ResizeTo(3+3*nTrk, 3+3*nTrk);
    
    long int ij=0;
    for(int i=0; i<(3+3*nTrk); i++) {
      for(int j=0; j<=i; j++) {
        m_cachedCov(i,j) = cov[ij];
        ij++;
      }
    }
    
    return true;
  }
  
  // FIXME: so far we can only conver VKalVrt cov matrix

  // error
  return false;
    
}

/*****************************************************************************/
bool xAOD::BPhysHelper::cacheRefTracks()
{
  // if refitted tracks are already cached, do nothing and return true 
  if(m_refTracksCached && !m_cachedRefTracks.empty())
    return true;
  
  // if refitted tracks are cached but the cache is empty, return false (error)
  if(m_refTracksCached && m_cachedRefTracks.empty())
    return false;
  
  // Refitted tracks are not cached, yet. Retrieve them from the aux store:
  m_refTracksCached = true;
  m_cachedRefTracks.clear();
  
  // Create auxiliary branches accessors 
  static SG::AuxElement::Accessor< std::vector<float> > refTrackPxAcc("RefTrackPx");
  static SG::AuxElement::Accessor< std::vector<float> > refTrackPyAcc("RefTrackPy");
  static SG::AuxElement::Accessor< std::vector<float> > refTrackPzAcc("RefTrackPz");
  
  // check if branches are available:
  if(!refTrackPxAcc.isAvailable(*m_b) || 
     !refTrackPyAcc.isAvailable(*m_b) ||
     !refTrackPzAcc.isAvailable(*m_b) )
    return false;
  
  // retrieve branches:
  const std::vector<float>& refTrackPx = refTrackPxAcc(*m_b);
  const std::vector<float>& refTrackPy = refTrackPyAcc(*m_b);
  const std::vector<float>& refTrackPz = refTrackPzAcc(*m_b);
  
  // all must have the same lenght
  if(refTrackPx.size()!=refTrackPy.size() || refTrackPx.size()!=refTrackPz.size())
    return false;
  
  // all OK: store refitted tracks in the cache
  for(uint i=0; i<refTrackPx.size(); ++i) {
    m_cachedRefTracks.push_back( TVector3(refTrackPx[i], refTrackPy[i], refTrackPz[i] ));
  }
  
  return true;
  
}

/*****************************************************************************/
bool xAOD::BPhysHelper::cacheMuons()
{
  // if linked muons are already cached do nothing and return true 
  if(m_muonsCached && !m_cachedMuons.empty())
    return true;
  
  // if linked muons are cached but the cache is empty, return false (error)
  if(m_muonsCached && m_cachedMuons.empty())
    return false;
  
  // Linked muons are not cached, yet. Retrieve them from the aux store:
  m_muonsCached = true;
  m_cachedMuons.clear();
  
  // Create auxiliary branches accessors 
  static SG::AuxElement::Accessor<MuonLinkVector> muonLinksAcc("MuonLinks"); 
  
  // check if branch exists
  if(!muonLinksAcc.isAvailable(*m_b)) {
    return false;
  }
  
  // retrieve the muon links...
  const MuonLinkVector& muonLinks = muonLinksAcc(*m_b);
  
  // ... and check if they are all valid
  MuonLinkVector::const_iterator muonLinksItr = muonLinks.begin();
  for(; muonLinksItr!=muonLinks.end(); ++muonLinksItr) {
    // check if links are valid
    if(!(*muonLinksItr).isValid()) {
      return false;
    }
  }
  
  // all OK, cache the muons
  muonLinksItr = muonLinks.begin();
  for(; muonLinksItr!=muonLinks.end(); ++muonLinksItr) {
    m_cachedMuons.push_back(*(*muonLinksItr));
  }
  
  return true;
  
}

/*****************************************************************************/
bool xAOD::BPhysHelper::cacheElectrons()
{
  // if linked electrons are already cached do nothing and return true 
  if(m_electronsCached && !m_cachedElectrons.empty())
    return true;
  
  // if linked electrons are cached but the cache is empty, return false (error)
  if(m_electronsCached && m_cachedElectrons.empty())
    return false;
  
  // Linked electrons are not cached, yet. Retrieve them from the aux store:
  m_electronsCached = true;
  m_cachedElectrons.clear();
  
  // Create auxiliary branches accessors 
  static SG::AuxElement::Accessor<ElectronLinkVector> electronLinksAcc("ElectronLinks"); 
  
  // check if branch exists
  if(!electronLinksAcc.isAvailable(*m_b)) {
    return false;
  }
  
  // retrieve the electron links...
  const ElectronLinkVector& electronLinks = electronLinksAcc(*m_b);
  
  // ... and check if they are all valid
  ElectronLinkVector::const_iterator electronLinksItr = electronLinks.begin();
  for(; electronLinksItr!=electronLinks.end(); ++electronLinksItr) {
    // check if links are valid
    if(!(*electronLinksItr).isValid()) {
      return false;
    }
  }
  
  // all OK, cache the electrons
  electronLinksItr = electronLinks.begin();
  for(; electronLinksItr!=electronLinks.end(); ++electronLinksItr) {
    m_cachedElectrons.push_back(*(*electronLinksItr));
  }
  
  return true;
  
}

/*****************************************************************************/
bool xAOD::BPhysHelper::cachePrecedingVertices()
{
  // if linked preceding vertices are already cached do nothing and return true 
  if(m_precedingVerticesCached && !m_cachedPrecedingVertices.empty())
    return true;
  
  // if linked preceding vertices are cached but the cache is empty, return false (error)
  if(m_precedingVerticesCached && m_cachedPrecedingVertices.empty())
    return false;
  
  // Linked preceding vertices are not cached, yet. Retrieve them from the aux store:
  m_precedingVerticesCached = true;
  m_cachedPrecedingVertices.clear();
  
  // Create auxiliary branches accessors 
  static SG::AuxElement::Accessor<VertexLinkVector> precedingVertexLinksAcc("PrecedingVertexLinks"); 
  
  // check if branch exists
  if(!precedingVertexLinksAcc.isAvailable(*m_b)) {
    return false;
  }
  
  // retrieve the precedingVertex links...
  const VertexLinkVector& precedingVertexLinks = precedingVertexLinksAcc(*m_b);
  
  // ... and check if they are all valid
  VertexLinkVector::const_iterator precedingVertexLinksItr = precedingVertexLinks.begin();
  for(; precedingVertexLinksItr!=precedingVertexLinks.end(); ++precedingVertexLinksItr) {
    // check if links are valid
    if(!(*precedingVertexLinksItr).isValid()) {
      return false;
    }
  }
  
  // all OK, cache the precedingVertices
  precedingVertexLinksItr = precedingVertexLinks.begin();
  for(; precedingVertexLinksItr!=precedingVertexLinks.end(); ++precedingVertexLinksItr) {
    m_cachedPrecedingVertices.push_back(*(*precedingVertexLinksItr));
  }
  
  return true;
  
}

/*****************************************************************************/
bool xAOD::BPhysHelper::cacheCascadeVertices()
{
  // if linked cascade vertices are already cached do nothing and return true 
  if(m_cascadeVerticesCached && !m_cachedCascadeVertices.empty())
    return true;
  
  // if linked cascade vertices are cached but the cache is empty, return false (error)
  if(m_cascadeVerticesCached && m_cachedCascadeVertices.empty())
    return false;
  
  // Linked cascade vertices are not cached, yet. Retrieve them from the aux store:
  m_cascadeVerticesCached = true;
  m_cachedCascadeVertices.clear();
  
  // Create auxiliary branches accessors 
  static SG::AuxElement::Accessor<VertexLinkVector> cascadeVertexLinksAcc("CascadeVertexLinks"); 
  
  // check if branch exists
  if(!cascadeVertexLinksAcc.isAvailable(*m_b)) {
    return false;
  }
  
  // retrieve the cascadeVertex links...
  const VertexLinkVector& cascadeVertexLinks = cascadeVertexLinksAcc(*m_b);
  
  // ... and check if they are all valid
  VertexLinkVector::const_iterator cascadeVertexLinksItr = cascadeVertexLinks.begin();
  for(; cascadeVertexLinksItr!=cascadeVertexLinks.end(); ++cascadeVertexLinksItr) {
    // check if links are valid
    if(!(*cascadeVertexLinksItr).isValid()) {
      return false;
    }
  }
  
  // all OK, cache the cascadeVertices
  cascadeVertexLinksItr = cascadeVertexLinks.begin();
  for(; cascadeVertexLinksItr!=cascadeVertexLinks.end(); ++cascadeVertexLinksItr) {
    m_cachedCascadeVertices.push_back(*(*cascadeVertexLinksItr));
  }
  
  return true;
  
}
/*****************************************************************************/
/*****************************************************************************/
const std::vector<TVector3>            xAOD::BPhysHelper::emptyVectorOfTVector3(0);
const std::vector<const xAOD::Muon*>   xAOD::BPhysHelper::emptyVectorOfMuons(0);
const std::vector<const xAOD::Electron*>   xAOD::BPhysHelper::emptyVectorOfElectrons(0);
const TMatrixTSym<double>              xAOD::BPhysHelper::emptyMatrix(0);
const std::vector<const xAOD::Vertex*> xAOD::BPhysHelper::emptyVectorOfVertices(0);
/*****************************************************************************/
