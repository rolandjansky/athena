/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
/// Name    : Muon.cxx
/// Package : offline/Reconstruction/MuonIdentification/muonEvent
/// Author  : Ketevi A. Assamagan
/// Created : December 2004
/// Note    : CombinedMuon Object in the ESD
///
/// DESCRIPTION:
///
///	see Muon.h

/// January 2007: Merge CombinedMuon with Muon into one class - simply Muon
////////////////////////////////////////////////////////////////////////////////


#include "muonEvent/Muon.h"

#include "VxVertex/RecVertex.h"
#include "Particle/TrackParticle.h"
#include "CaloEvent/CaloCluster.h"
#include "GaudiKernel/GaudiException.h"

#include <math.h>
#include <string>

namespace Analysis {

//------------------------------------------------------------
// Constructor -- just set defaults

Muon::Muon() :
  IAthenaBarCode(),
  INavigable (),
  I4Momentum (),
  INavigable4Momentum (),
  MuonImpl_t(),
  m_author(MuonParameters::unknown),
  m_hasCombinedMuon(false),
  m_hasInDetTrackParticle(false),
  m_hasMuonExtrapolatedTrackParticle(false),
  m_hasInnerExtrapolatedTrackParticle(false),
  m_hasCombinedMuonTrackParticle(false),
  m_hasCluster(false),
  m_matchChi2(-1.0),
  m_matchNumberDoF(0),
  m_outerMatchNumberDoF(0),
  m_transientOuterMatchChi2(Trk::FitQuality()),
  m_bestMatch(false),
  m_parameters( ),
  m_isAlsoFoundByLowPt(false),
  m_isAlsoFoundByCaloMuonId(false),
  m_caloMuonAlsoFoundByMuonReco(0),
  m_isCorrected(false),
  m_allAuthors(0),
  m_isMuonBits(0),
  m_isMuonLikelihood(0.0) 
{
    this->set_charge(-999.0);
    m_associatedEtaDigits.clear();
    m_associatedPhiDigits.clear();
}

//------------------------------------------------------------
/** Constructor -- just set defaults */
Muon::Muon(MuonParameters::Author author) :
  IAthenaBarCode(),
  INavigable (),
  I4Momentum (),
  INavigable4Momentum (),
  MuonImpl_t(),
  m_author(author),
  m_hasCombinedMuon(false),
  m_hasInDetTrackParticle(false),
  m_hasMuonExtrapolatedTrackParticle(false),
  m_hasInnerExtrapolatedTrackParticle(false),
  m_hasCombinedMuonTrackParticle(false),
  m_hasCluster(false),
  m_matchChi2(-1.0),
  m_matchNumberDoF(0),
  m_outerMatchNumberDoF(0),
  m_transientOuterMatchChi2(Trk::FitQuality()),
  m_bestMatch(false),
  m_parameters( ),
  m_isAlsoFoundByLowPt(false),
  m_isAlsoFoundByCaloMuonId(false),
  m_caloMuonAlsoFoundByMuonReco(0),
  m_isCorrected(false),
  m_allAuthors(0),
  m_isMuonBits(0),
  m_isMuonLikelihood(0.0)
{
    this->add_author( author );
    this->set_charge(-999.0);
    m_associatedEtaDigits.clear();
    m_associatedPhiDigits.clear();

}

/** Principal constructor for combined muons with 3 TrackParticles */
Muon::Muon(MuonParameters::Author author,
        const Rec::TrackParticle* inDetTP, const Rec::TrackParticle* extrapTP, 
	const Rec::TrackParticle* combTP, const bool isExtrapolatedToVertex) 
  :
  IAthenaBarCode(),
  INavigable (),
  I4Momentum (),
  INavigable4Momentum (),
  MuonImpl_t(),
  m_author(author),   
  m_hasCombinedMuon(false),
  m_matchChi2(-1.0),
  m_matchNumberDoF(0),
  m_outerMatchNumberDoF(0),
  m_transientOuterMatchChi2(Trk::FitQuality()),
  m_bestMatch(false),
  m_isAlsoFoundByLowPt(false), 
  m_isAlsoFoundByCaloMuonId(false),
  m_caloMuonAlsoFoundByMuonReco(0),
  m_isCorrected(false) 
{
  this->set4Mom(combTP->hlv());

  m_inDetTrackParticle.setElement(const_cast<Rec::TrackParticle*>(inDetTP));

  if(isExtrapolatedToVertex)
    m_muonExtrapolatedTrackParticle.setElement(const_cast<Rec::TrackParticle*>(extrapTP));
  else
    m_innerExtrapolatedTrackParticle.setElement(const_cast<Rec::TrackParticle*>(extrapTP));

  m_combinedMuonTrackParticle.setElement(const_cast<Rec::TrackParticle*>(combTP));

  m_hasCluster                       = m_cluster.isValid();
  m_hasInDetTrackParticle            = m_inDetTrackParticle.isValid();
  m_hasMuonExtrapolatedTrackParticle = m_muonExtrapolatedTrackParticle.isValid();
  m_hasInnerExtrapolatedTrackParticle= m_innerExtrapolatedTrackParticle.isValid();
  m_hasCombinedMuonTrackParticle     = m_combinedMuonTrackParticle.isValid();
  m_associatedEtaDigits.clear();
  m_associatedPhiDigits.clear();
  this->set_charge( combTP->charge() );
  m_allAuthors = 0;
  this->add_author( author );
  m_isMuonBits = 0;
  m_isMuonLikelihood = 0.0;

}

Muon::Muon(MuonParameters::Author author,
     const Rec::TrackParticle* trackParticle, const bool isExtrapolatedToVertex)
        : 
  IAthenaBarCode(),
  INavigable (),
  I4Momentum (),
  INavigable4Momentum (),
  MuonImpl_t(),
  m_author(author),
  m_hasCombinedMuon(false),
  m_matchChi2(-1.0),
  m_matchNumberDoF(0),
  m_outerMatchNumberDoF(0),
  m_transientOuterMatchChi2(Trk::FitQuality()),
  m_bestMatch(false),
  m_isAlsoFoundByLowPt(false), 
  m_isAlsoFoundByCaloMuonId(false),
  m_caloMuonAlsoFoundByMuonReco(0), 
  m_isCorrected(false)
 {
  this->set4Mom(trackParticle->hlv());

  if(isExtrapolatedToVertex)
    m_muonExtrapolatedTrackParticle.setElement(const_cast<Rec::TrackParticle*>(trackParticle));
  else {
    if (this->isCaloMuonId())
      m_inDetTrackParticle.setElement(const_cast<Rec::TrackParticle*>(trackParticle));
    else
      m_innerExtrapolatedTrackParticle.setElement(const_cast<Rec::TrackParticle*>(trackParticle));
  }
  m_hasCluster                       = m_cluster.isValid();
  m_hasInDetTrackParticle            = m_inDetTrackParticle.isValid();
  m_hasMuonExtrapolatedTrackParticle = m_muonExtrapolatedTrackParticle.isValid();
  m_hasInnerExtrapolatedTrackParticle= m_innerExtrapolatedTrackParticle.isValid();
  m_hasCombinedMuonTrackParticle     = m_combinedMuonTrackParticle.isValid();
  m_associatedEtaDigits.clear();
  m_associatedPhiDigits.clear();
  this->set_charge( trackParticle->charge() );
  m_allAuthors = 0;
  this->add_author( author );
  m_isMuonBits = 0;
  m_isMuonLikelihood = 0.0;

}

/** if the combined failed, still create the combined muon from the Inner Detector TrackParticle 
    and a muon segment */
Muon::Muon(MuonParameters::Author author,
        const Rec::TrackParticle* inDetTP, 
        const Trk::SegmentCollection * segmentContainer,
        const std::vector<const Trk::Segment*>& muonSegments)
        : 
  IAthenaBarCode(),
  INavigable (),
  I4Momentum (),
  INavigable4Momentum (),
  MuonImpl_t(),
  m_author(author),
  m_hasCombinedMuon(false),
  m_matchChi2(-1.0),
  m_matchNumberDoF(0),
  m_outerMatchNumberDoF(0),
  m_transientOuterMatchChi2(Trk::FitQuality()),
  m_bestMatch(false),
  m_isAlsoFoundByLowPt(false), 
  m_isAlsoFoundByCaloMuonId(false),
  m_caloMuonAlsoFoundByMuonReco(0), 
  m_isCorrected(false)
 {
  this->set4Mom(inDetTP->hlv());
  m_inDetTrackParticle.setElement(const_cast<Rec::TrackParticle*>(inDetTP));

  for (unsigned int i=0; i<muonSegments.size(); ++i) this->addSegment(segmentContainer, muonSegments[i]);

  m_hasCluster                       = m_cluster.isValid();
  m_hasInDetTrackParticle            = m_inDetTrackParticle.isValid();
  m_hasMuonExtrapolatedTrackParticle = m_muonExtrapolatedTrackParticle.isValid();
  m_hasInnerExtrapolatedTrackParticle= m_innerExtrapolatedTrackParticle.isValid();
  m_hasCombinedMuonTrackParticle     = m_combinedMuonTrackParticle.isValid();
  m_associatedEtaDigits.clear();
  m_associatedPhiDigits.clear();
  this->set_charge( inDetTP->charge() );
  m_allAuthors = 0;
  this->add_author( author );
  m_isMuonBits = 0;
  m_isMuonLikelihood = 0.0;
}

//------------------------------------------------------------
/** Copy Constructor */
Muon::Muon( const Muon& rhs ) :
  IAthenaBarCode(rhs),
  INavigable (rhs),
  I4Momentum (rhs),
  INavigable4Momentum (rhs),
  MuonImpl_t(rhs),
  m_author                           ( rhs.m_author ),
  m_hasCombinedMuon                  ( rhs.m_hasCombinedMuon ),
  m_hasInDetTrackParticle            ( rhs.m_hasInDetTrackParticle ),
  m_hasMuonExtrapolatedTrackParticle ( rhs.m_hasMuonExtrapolatedTrackParticle ),
  m_hasInnerExtrapolatedTrackParticle( rhs.m_hasInnerExtrapolatedTrackParticle ),
  m_hasCombinedMuonTrackParticle     ( rhs.m_hasCombinedMuonTrackParticle ),
  m_hasCluster                       ( rhs.m_hasCluster ),
  m_matchChi2                        ( rhs.m_matchChi2 ),
  m_matchNumberDoF                   ( rhs.m_matchNumberDoF ),
  m_outerMatchNumberDoF              ( rhs.m_outerMatchNumberDoF ),
  m_transientOuterMatchChi2          ( rhs.m_transientOuterMatchChi2 ),
  m_bestMatch                        ( rhs.m_bestMatch ),
  m_associatedEtaDigits              ( rhs.m_associatedEtaDigits ),
  m_associatedPhiDigits              ( rhs.m_associatedPhiDigits ),
  m_inDetTrackParticle               ( rhs.m_inDetTrackParticle ),
  m_muonExtrapolatedTrackParticle    ( rhs.m_muonExtrapolatedTrackParticle ),
  m_innerExtrapolatedTrackParticle   ( rhs.m_innerExtrapolatedTrackParticle ),
  m_combinedMuonTrackParticle        ( rhs.m_combinedMuonTrackParticle ),
  m_statCombinedMuonTrackParticle    ( rhs.m_statCombinedMuonTrackParticle ),
  m_cluster                          ( rhs.m_cluster ),
  m_muonSpectrometerTrackParticle    ( rhs.m_muonSpectrometerTrackParticle),
  m_parameters                       ( rhs.m_parameters ),
  m_isAlsoFoundByLowPt               ( rhs.m_isAlsoFoundByLowPt ),
  m_isAlsoFoundByCaloMuonId          ( rhs.m_isAlsoFoundByCaloMuonId ),
  m_caloEnergyLoss                   ( rhs.m_caloEnergyLoss ),
  m_caloMuonAlsoFoundByMuonReco      ( rhs.m_caloMuonAlsoFoundByMuonReco ),
  m_isCorrected                      ( rhs.m_isCorrected ),
  m_allAuthors                       ( rhs.m_allAuthors ),
  m_isMuonBits                       ( rhs.m_isMuonBits ),
  m_isMuonLikelihood                 ( rhs.m_isMuonLikelihood )

{
  copySegmentLinkVector(rhs.m_muonSegments);
} 


void Muon::copySegmentLinkVector( const ElementLinkVector<Trk::SegmentCollection>& segments ) {
  if( !m_muonSegments.empty() ) m_muonSegments.clear();
  m_muonSegments.reserve(segments.size());
  ElementLinkVector<Trk::SegmentCollection>::const_iterator sit = segments.begin();
  ElementLinkVector<Trk::SegmentCollection>::const_iterator sit_end = segments.end();
  for( ;sit!=sit_end;++sit) m_muonSegments.push_back( *sit );
}

//------------------------------------------------------------
/** Assignement operator */
Muon& Muon::operator=( const Muon& rhs )
{
  if ( this != &rhs ) {
    IAthenaBarCode::operator=(rhs);
    INavigable::operator=(rhs);
    I4Momentum::operator=(rhs);
    INavigable4Momentum::operator=(rhs);
    MuonImpl_t::operator=(rhs);
    m_author                           = rhs.m_author;
    m_hasCombinedMuon                  = rhs.m_hasCombinedMuon;
    m_hasInDetTrackParticle            = rhs.m_hasInDetTrackParticle;
    m_hasMuonExtrapolatedTrackParticle = rhs.m_hasMuonExtrapolatedTrackParticle;
    m_hasInnerExtrapolatedTrackParticle= rhs.m_hasInnerExtrapolatedTrackParticle;
    m_hasCombinedMuonTrackParticle     = rhs.m_hasCombinedMuonTrackParticle;
    m_hasCluster                       = rhs.m_hasCluster;
    m_matchChi2                        = rhs.m_matchChi2;
    m_matchNumberDoF                   = rhs.m_matchNumberDoF;
    m_outerMatchNumberDoF              = rhs.m_outerMatchNumberDoF;
    m_transientOuterMatchChi2          = rhs.m_transientOuterMatchChi2;
    m_bestMatch                        = rhs.m_bestMatch;
    m_associatedEtaDigits              = rhs.m_associatedEtaDigits;
    m_associatedPhiDigits              = rhs.m_associatedPhiDigits;
    m_inDetTrackParticle               = rhs.m_inDetTrackParticle;
    m_muonExtrapolatedTrackParticle    = rhs.m_muonExtrapolatedTrackParticle;
    m_innerExtrapolatedTrackParticle   = rhs.m_innerExtrapolatedTrackParticle;
    m_combinedMuonTrackParticle        = rhs.m_combinedMuonTrackParticle;
    m_statCombinedMuonTrackParticle    = rhs.m_statCombinedMuonTrackParticle;
    m_cluster                          = rhs.m_cluster;
    m_muonSpectrometerTrackParticle    = rhs.m_muonSpectrometerTrackParticle;
    m_parameters                       = rhs.m_parameters;
    m_isAlsoFoundByLowPt               = rhs.m_isAlsoFoundByLowPt;
    m_isAlsoFoundByCaloMuonId          = rhs.m_isAlsoFoundByCaloMuonId;
    m_caloEnergyLoss                   = rhs.m_caloEnergyLoss;
    m_caloMuonAlsoFoundByMuonReco      = rhs.m_caloMuonAlsoFoundByMuonReco;
    m_isCorrected                      = rhs.m_isCorrected;
    m_allAuthors                       = rhs.m_allAuthors; 
    m_isMuonBits                       = rhs.m_isMuonBits;
    m_isMuonLikelihood                 = rhs.m_isMuonLikelihood;

    copySegmentLinkVector( rhs.m_muonSegments );

  }
  return *this;
}

//------------------------------------------------------------
/** Destructor */
Muon::~Muon() {}

bool Muon::isAuthor ( const unsigned int author ) const {
  MuonParameters::Author x = static_cast<MuonParameters::Author>(author);
  return this->isAuthor(x);
}

bool Muon::isAuthor ( MuonParameters::Author author ) const {
  bool is_it = false;
  switch ( author ) {
     case MuonParameters::Muonboy :
        is_it = m_allAuthors & 1;
        break;
     case MuonParameters::STACO :
        is_it = m_allAuthors & 2;
        break;
     case MuonParameters::MuTag :
        is_it = m_allAuthors & 4;
        break;
     case MuonParameters::MuidSA :
        is_it = m_allAuthors & 8;
        break;
     case MuonParameters::MuidCo :
        is_it = m_allAuthors & 16;
        break;
     case MuonParameters::MuGirl :
        is_it = m_allAuthors & 32;
        break;
     case MuonParameters::MuGirlLowBeta :
        is_it = m_allAuthors & 64;
        break;
     case MuonParameters::CaloMuonId :
        is_it = m_allAuthors & 128;
        break;
     case MuonParameters::CaloTag :
        is_it = m_allAuthors & 256; 
        break;
      case MuonParameters::CaloLikelihood :
        is_it = m_allAuthors & 512;
        break;
      case MuonParameters::MuTagIMO :
        is_it = m_allAuthors & 1024;
        break;
      case MuonParameters::MuonCombinedRefit :
        is_it = m_allAuthors & 2048;
        break;
      case MuonParameters::ExtrapolateMuonToIP :
        is_it = m_allAuthors & 4096;
        break;

     default :
        is_it = false;
  }
  return is_it;
}

bool Muon::add_author ( MuonParameters::Author author ) {
  bool is_set = false;
  if ( !this->isAuthor( author ) ) {
    is_set = true;
   switch ( author ) {
     case MuonParameters::Muonboy : 
	m_allAuthors = m_allAuthors | 1;
	break;
     case MuonParameters::STACO : 
	m_allAuthors = m_allAuthors | 2;
	break;
     case MuonParameters::MuTag : 
	m_allAuthors = m_allAuthors | 4;
	break;
     case MuonParameters::MuidSA : 
	m_allAuthors = m_allAuthors | 8;
	break;
     case MuonParameters::MuidCo : 
        m_allAuthors = m_allAuthors | 16;	
	break;
     case MuonParameters::MuGirl : 
	m_allAuthors = m_allAuthors | 32;
	break;
     case MuonParameters::MuGirlLowBeta : 
	m_allAuthors = m_allAuthors | 64;
	break;
     case MuonParameters::CaloMuonId : 
	m_allAuthors = m_allAuthors | 128;
	break;
     case MuonParameters::CaloTag :
        m_allAuthors = m_allAuthors | 256;
        break;
     case MuonParameters::CaloLikelihood :
        m_allAuthors = m_allAuthors | 512;
        break;
     case MuonParameters::MuTagIMO :
        m_allAuthors = m_allAuthors | 1024;
        break;
     case MuonParameters::MuonCombinedRefit :
        m_allAuthors = m_allAuthors | 2048;
        break;
     case MuonParameters::ExtrapolateMuonToIP :
        m_allAuthors = m_allAuthors | 4096;
        break;

     default : 
	is_set = false;
   } 
  }
  return is_set;
}

/** test for a MS-ID combined muon */
bool Muon::isCombinedMuon() const {
  if (m_hasCombinedMuonTrackParticle && // mind DPD where hasParticle() but EL may be invalid
      ( m_author==MuonParameters::STACO ||
	m_author==MuonParameters::MuidCo ||
	m_author==MuonParameters::MuonCombinedRefit ||
        m_author==MuonParameters::MuGirl ) ) return true;
  else return false;
}

/** test for a MS-standalone muon */
bool Muon::isStandAloneMuon() const {

  if (m_hasMuonExtrapolatedTrackParticle && // mind DPD where hasParticle() but EL may be invalid
      ( m_author==MuonParameters::MuonboySP ||
	m_author==MuonParameters::Muonboy ||
        ( m_author==MuonParameters::STACO && !this->hasInDetTrackParticle()) ||
	m_author==MuonParameters::MOORE ||
	m_author==MuonParameters::MuidSA ||
	m_author==MuonParameters::ExtrapolateMuonToIP) ) return true;
  else return false;

  // Formerly: !this->bestMatch() && !this->hasCombinedMuonTrackParticle()
  //  && !this->isSegmentTaggedMuon() && this->isHighPt() && this->m_hasMuonExtr...
}


/** test for a MS-segment tagged muon */
bool Muon::isSegmentTaggedMuon() const {
  if (m_author==MuonParameters::MuTag    ||
      m_author==MuonParameters::MuTagIMO ||
      ( m_author==MuonParameters::MuGirl && ! m_hasCombinedMuonTrackParticle) ) return true;
  else return false;
}

/** test for a calo-tagged muon */
bool Muon::isCaloMuonId() const {
  if ( m_author==MuonParameters::CaloMuonId ||
       m_author==MuonParameters::CaloTag    ||
       m_author==MuonParameters::CaloLikelihood ) return true;
  else return false;
}

/** test for a silicon associated forward muon*/
bool Muon::isSiliconAssociatedForwardMuon() const {
  const Rec::TrackParticle* indetParticle = inDetTrackParticle();
  if( !indetParticle ) return false;
  return indetParticle->info().patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_ForwardTracks);
}

/** flag if the there was re-fit of ID and MS hits */
bool Muon::hasGlobalMuonTrackFit() const {
  if (m_author==MuonParameters::MuidCo ||
      m_author==MuonParameters::MuonCombinedRefit ||
      this->hasInnerExtrapolatedTrackParticle() ||
      (m_author==MuonParameters::MuGirl && m_hasCombinedMuonTrackParticle) ) return true;
  else return false;
}


/** get a parameter for this Muon - isolation energy in some cone for example */
double Muon::parameter( MuonParameters::ParamDef x) const {

  typedef std::pair<MuonParameters::ParamDef,float> muonParams;

  double result = -999.;
  std::vector<muonParams>::const_iterator p = m_parameters.begin();
 
  for (;p !=m_parameters.end(); p++) {
    if ( (*p).first == x )
      result = (*p).second;
  }

  return result;
}

void Muon::set_outerMatchChi2(double chi2, int ndof)
{
  if (ndof>0) {
    m_outerMatchNumberDoF = ndof;
    this->set_parameter(MuonParameters::segmentChi2OverDoF,(float)(chi2/ndof));
    m_transientOuterMatchChi2 = Trk::FitQuality(chi2,ndof);
  } else {
    m_outerMatchNumberDoF = 0;
    this->set_parameter(MuonParameters::segmentChi2OverDoF,-1.0);
  }
}


/** energy loss and its uncertainty in the calorimeter */
std::pair<double,double> Muon::energyLoss() const{
  double eloss = 0.0;
  double sigma = 0.0;
  if ( m_caloEnergyLoss.isValid() ) {
     const CaloEnergy * caloEnergy = *m_caloEnergyLoss;
     eloss = caloEnergy->deltaE();
     sigma = caloEnergy->sigmaDeltaE();
  }
  return std::make_pair(eloss, sigma);
}

/** number of B-layer hits */
int Muon::numberOfInnermostPixelLayerHits() const {
  if ( this->hasCombinedMuonTrackParticle() || this->hasInDetTrackParticle() )
    return (*m_inDetTrackParticle)->trackSummary()->get( Trk::numberOfInnermostPixelLayerHits );
  else return -1;
  
}
 
/** number of pixel hits */
int Muon::numberOfPixelHits() const {   
  if ( this->hasCombinedMuonTrackParticle() || this->hasInDetTrackParticle() )
    return (*m_inDetTrackParticle)->trackSummary()->get( Trk::numberOfPixelHits );
  else return -1;
 
}
 
/** number of Silicon hits */
int Muon::numberOfSCTHits() const {   
  if ( this->hasCombinedMuonTrackParticle() || this->hasInDetTrackParticle() )
    return (*m_inDetTrackParticle)->trackSummary()->get( Trk::numberOfSCTHits );
  else return -1;
 
}
 
/** number of TRT hits */
int Muon::numberOfTRTHits() const {   
  if ( this->hasCombinedMuonTrackParticle() || this->hasInDetTrackParticle() )
    return (*m_inDetTrackParticle)->trackSummary()->get( Trk::numberOfTRTHits );
  else return -1;
 
}
 
/** number of TRT high threshold hits */
int Muon::numberOfTRTHighThresholdHits() const {   
  if ( this->hasCombinedMuonTrackParticle() || this->hasInDetTrackParticle() )
    return (*m_inDetTrackParticle)->trackSummary()->get( Trk::numberOfTRTHighThresholdHits );
  else return -1;
 
}

/** number of shared hits at pixel b-layer */
int Muon::numberOfInnermostPixelLayerSharedHits() const {
  if ( this->hasCombinedMuonTrackParticle() || this->hasInDetTrackParticle() )
      return (*m_inDetTrackParticle)->trackSummary()->get( Trk::numberOfInnermostPixelLayerSharedHits );
  else return -1;
}
int Muon::numberOfPixelSharedHits() const {   
  if ( this->hasCombinedMuonTrackParticle() || this->hasInDetTrackParticle() )
      return (*m_inDetTrackParticle)->trackSummary()->get( Trk::numberOfPixelSharedHits );
  else return -1;
}

int Muon::numberOfPixelHoles() const {   
  if ( this->hasCombinedMuonTrackParticle() || this->hasInDetTrackParticle() )
      return (*m_inDetTrackParticle)->trackSummary()->get( Trk::numberOfPixelHoles );
  else return -1;
}

int Muon::numberOfSCTSharedHits() const {   
  if ( this->hasCombinedMuonTrackParticle() || this->hasInDetTrackParticle() )
      return (*m_inDetTrackParticle)->trackSummary()->get( Trk::numberOfSCTSharedHits );
  else return -1;
}

int Muon::numberOfSCTHoles() const {   
  if ( this->hasCombinedMuonTrackParticle() || this->hasInDetTrackParticle() )
      return (*m_inDetTrackParticle)->trackSummary()->get( Trk::numberOfSCTHoles );
  else return -1;
}

int Muon::numberOfTRTOutliers() const {   
  if ( this->hasCombinedMuonTrackParticle() || this->hasInDetTrackParticle() )
      return (*m_inDetTrackParticle)->trackSummary()->get( Trk::numberOfTRTOutliers );
  else return -1;
}

int Muon::numberOfTRTHighThresholdOutliers() const {   
  if ( this->hasCombinedMuonTrackParticle() || this->hasInDetTrackParticle() )
      return (*m_inDetTrackParticle)->trackSummary()->get( Trk::numberOfTRTHighThresholdOutliers );
  else return -1;
}

/** number of MDT hits/holes */
int Muon::numberOfMDTHits() const {   
  if ( this->hasMuonExtrapolatedTrackParticle() )
      return (*m_muonExtrapolatedTrackParticle)->trackSummary()->get( Trk::numberOfMdtHits );
  else if ( this->hasInDetTrackParticle() && associatedEtaDigits().size() > 0)
      return ( associatedEtaDigits()[0] );
  else return -1;
}
int Muon::numberOfMDTHoles() const {   
  if ( this->hasMuonExtrapolatedTrackParticle() )
      return (*m_muonExtrapolatedTrackParticle)->trackSummary()->get( Trk::numberOfMdtHoles );
  else return -1;
}

/** number of CSC Phi hits/holes */
int Muon::numberOfCSCPhiHits() const {   
  if ( this->hasMuonExtrapolatedTrackParticle() )
      return (*m_muonExtrapolatedTrackParticle)->trackSummary()->get( Trk::numberOfCscPhiHits );
  else if ( this->hasInDetTrackParticle() && associatedPhiDigits().size() > 0)
      return ( associatedPhiDigits()[1] );
  else return -1;
}
int Muon::numberOfCSCPhiHoles() const {   
  if ( this->hasMuonExtrapolatedTrackParticle() )
      return (*m_muonExtrapolatedTrackParticle)->trackSummary()->get( Trk::numberOfCscPhiHoles );
  else return -1;
}

/** number of CSC Eta hits/holes */
int Muon::numberOfCSCEtaHits() const {   
  if ( this->hasMuonExtrapolatedTrackParticle() )
      return (*m_muonExtrapolatedTrackParticle)->trackSummary()->get( Trk::numberOfCscEtaHits );
  else if ( this->hasInDetTrackParticle() && associatedEtaDigits().size() > 0)
      return ( associatedEtaDigits()[1] );
  else return -1;
}
int Muon::numberOfCSCEtaHoles() const {   
  if ( this->hasMuonExtrapolatedTrackParticle() )
      return (*m_muonExtrapolatedTrackParticle)->trackSummary()->get( Trk::numberOfCscEtaHoles );
  else return -1;
}

/** number of RPC Phi hits/holes */
int Muon::numberOfRPCPhiHits() const {   
  if ( this->hasMuonExtrapolatedTrackParticle() )
      return (*m_muonExtrapolatedTrackParticle)->trackSummary()->get( Trk::numberOfRpcPhiHits );
  else if ( this->hasInDetTrackParticle() && associatedPhiDigits().size() > 0)
      return ( associatedPhiDigits()[2] );
  else return -1;
}
int Muon::numberOfRPCPhiHoles() const {   
  if ( this->hasMuonExtrapolatedTrackParticle() )
      return (*m_muonExtrapolatedTrackParticle)->trackSummary()->get( Trk::numberOfRpcPhiHoles );
  else return -1;
}

/** number of RPC Eta hits/holes */
int Muon::numberOfRPCEtaHits() const {   
  if ( this->hasMuonExtrapolatedTrackParticle() )
      return (*m_muonExtrapolatedTrackParticle)->trackSummary()->get( Trk::numberOfRpcEtaHits );
  else if ( this->hasInDetTrackParticle() && associatedEtaDigits().size() > 0)
      return ( associatedEtaDigits()[2] );
  else return -1;
}
int Muon::numberOfRPCEtaHoles() const {   
  if ( this->hasMuonExtrapolatedTrackParticle() )
      return (*m_muonExtrapolatedTrackParticle)->trackSummary()->get( Trk::numberOfRpcEtaHoles );
  else return -1;
}

/** number of TGC Phi hits/holes */
int Muon::numberOfTGCPhiHits() const {   
  if ( this->hasMuonExtrapolatedTrackParticle() )
      return (*m_muonExtrapolatedTrackParticle)->trackSummary()->get( Trk::numberOfTgcPhiHits );
  else if ( this->hasInDetTrackParticle() && associatedPhiDigits().size() > 0)
      return ( associatedPhiDigits()[3] );
  else return -1;
}
int Muon::numberOfTGCPhiHoles() const {   
  if ( this->hasMuonExtrapolatedTrackParticle() )
      return (*m_muonExtrapolatedTrackParticle)->trackSummary()->get( Trk::numberOfTgcPhiHoles );
  else return -1;
}

/** number of TGC Eta hits/holes */
int Muon::numberOfTGCEtaHits() const {   
  if ( this->hasMuonExtrapolatedTrackParticle() )
      return (*m_muonExtrapolatedTrackParticle)->trackSummary()->get( Trk::numberOfTgcEtaHits );
  else if ( this->hasInDetTrackParticle() && associatedEtaDigits().size() > 0)
      return ( associatedEtaDigits()[3] );
  else return -1;
}
int Muon::numberOfTGCEtaHoles() const {   
  if ( this->hasMuonExtrapolatedTrackParticle() )
      return (*m_muonExtrapolatedTrackParticle)->trackSummary()->get( Trk::numberOfTgcEtaHoles );
  else return -1;
}

/** GangedPixel, outliers, summaryTypes */
int Muon::numberOfGangedPixels() const {
  const Rec::TrackParticle * theTrack = this->track();   
  if ( theTrack )
      return theTrack->trackSummary()->get( Trk::numberOfGangedPixels );
  else return -1;
}

int Muon::numberOfOutliersOnTrack() const {
  const Rec::TrackParticle * theTrack = this->track();   
  if ( theTrack )
      return theTrack->trackSummary()->get( Trk::numberOfOutliersOnTrack );
  else return -1;

}

int Muon::numberOfTrackSummaryTypes(unsigned int type) const {	 
   const Rec::TrackParticle * theTrack = this->track();	 
   if ( theTrack && type < Trk::numberOfTrackSummaryTypes )	 
       return theTrack->trackSummary()->get( static_cast<Trk::SummaryType>(type) );	 
   else return -1;	 
}

void Muon::set_energyLoss ( const MuonCaloEnergyContainer* cont, const CaloEnergy* caloEnergy )
{
  if (m_caloEnergyLoss.isValid() ) m_caloEnergyLoss.reset();
  m_caloEnergyLoss.toContainedElement(*cont, const_cast<CaloEnergy*>(caloEnergy));
}

/** Set Inner Detector TrackParticle */
void Muon::set_inDetTrackParticle(const Rec::TrackParticleContainer* cont, 
				  const Rec::TrackParticle* trackParticle )
{
  if (m_inDetTrackParticle.isValid() ) m_inDetTrackParticle.reset();
  m_inDetTrackParticle.toContainedElement(*cont, const_cast<Rec::TrackParticle*>(trackParticle));
  m_hasInDetTrackParticle = m_inDetTrackParticle.isValid();
}

void Muon::set_inDetTrackParticle(const Rec::TrackParticle* trackParticle )
{
  m_inDetTrackParticle.setElement(const_cast<Rec::TrackParticle*>(trackParticle));
  m_hasInDetTrackParticle = m_inDetTrackParticle.isValid();
}

/** Set MuonSegments  */
void Muon::set_muonSegments(const Trk::SegmentCollection* segmentContainer, 
                            const std::vector<const Trk::Segment*>& muonSegments)
{
  m_muonSegments.clear(); 
  for (unsigned int i=0; i<muonSegments.size(); ++i) this->addSegment(segmentContainer, muonSegments[i]);
}

void Muon::addSegment ( const Trk::SegmentCollection* segmentContainer, const Trk::Segment* segment )
{
  ElementLink<Trk::SegmentCollection> link;
  link.toContainedElement( *segmentContainer, const_cast<Trk::Segment*>(segment) ) ;
  m_muonSegments.push_back( link ) ;
}

const ElementLink<Trk::SegmentCollection> Muon::muonSegmentLink(const unsigned int i, bool& validLink) const
{
   ElementLink<Trk::SegmentCollection> theLink;
   if ( i < this->numberOfSegments() ) {
     validLink = true;
     theLink = m_muonSegments[i];
   } else {
     validLink = false;
   }
   return theLink;
}

/** muon spectromeetr track particle needed by MOORE */
void Muon::set_muonSpectrometerTrackParticle(const Rec::TrackParticleContainer* cont, 
                                             const Rec::TrackParticle* trackParticle)
{
  if ( m_muonSpectrometerTrackParticle.isValid() ) m_muonSpectrometerTrackParticle.reset();
  if (cont && trackParticle) // allow also to remove an EL
    m_muonSpectrometerTrackParticle.toContainedElement(*cont, const_cast<Rec::TrackParticle*>(trackParticle));
}

/** Set Muon Extrapolated TrackParticle */
void Muon::set_muonExtrapolatedTrackParticle(const Rec::TrackParticleContainer* cont, 
					     const Rec::TrackParticle* trackParticle )
{
  if (m_muonExtrapolatedTrackParticle.isValid() ) m_muonExtrapolatedTrackParticle.reset();
  if (cont && trackParticle) { // allow also to remove an EL
    m_muonExtrapolatedTrackParticle.toContainedElement(*cont, const_cast<Rec::TrackParticle*>(trackParticle));
    m_hasMuonExtrapolatedTrackParticle = m_muonExtrapolatedTrackParticle.isValid();
  }
}

void Muon::set_muonExtrapolatedTrackParticle(const Rec::TrackParticle* trackParticle )
{
  m_muonExtrapolatedTrackParticle.setElement(const_cast<Rec::TrackParticle*>(trackParticle));
  m_hasMuonExtrapolatedTrackParticle = m_muonExtrapolatedTrackParticle.isValid();
}

/** Set Muon Extrapolated TrackParticle */
void Muon::set_innerExtrapolatedTrackParticle(const Rec::TrackParticleContainer* cont, 
					     const Rec::TrackParticle* trackParticle )
{
  if (m_innerExtrapolatedTrackParticle.isValid() ) m_innerExtrapolatedTrackParticle.reset();
  if (cont && trackParticle) { // allow also to remove an EL
    m_innerExtrapolatedTrackParticle.toContainedElement(*cont, const_cast<Rec::TrackParticle*>(trackParticle));
    m_hasInnerExtrapolatedTrackParticle = m_innerExtrapolatedTrackParticle.isValid();
  }
}

void Muon::set_innerExtrapolatedTrackParticle(const Rec::TrackParticle* trackParticle )
{
  m_innerExtrapolatedTrackParticle.setElement(const_cast<Rec::TrackParticle*>(trackParticle));
  m_hasInnerExtrapolatedTrackParticle = m_innerExtrapolatedTrackParticle.isValid();
}

/** Set Combined Muon TrackParticle */
void Muon::set_combinedMuonTrackParticle(const Rec::TrackParticleContainer* cont, 
					 const Rec::TrackParticle* trackParticle )
{
  if (m_combinedMuonTrackParticle.isValid() ) m_combinedMuonTrackParticle.reset();
  if (cont && trackParticle) { // allow also to remove an EL
    m_combinedMuonTrackParticle.toContainedElement(*cont, const_cast<Rec::TrackParticle*>(trackParticle));
    m_hasCombinedMuonTrackParticle = m_combinedMuonTrackParticle.isValid();
  }
}

void Muon::set_combinedMuonTrackParticle(const Rec::TrackParticle* trackParticle )
{
  m_combinedMuonTrackParticle.setElement(const_cast<Rec::TrackParticle*>(trackParticle)); 
  m_hasCombinedMuonTrackParticle = m_combinedMuonTrackParticle.isValid();
}

/** Set Statistically Combined Muon TrackParticle */
void Muon::set_statCombinedMuonTrackParticle(const Rec::TrackParticleContainer* cont, 
                                             const Rec::TrackParticle* trackParticle )
{
  if (m_statCombinedMuonTrackParticle.isValid() ) m_statCombinedMuonTrackParticle.reset();
  if (cont && trackParticle) { // allow also to remove an EL
    m_statCombinedMuonTrackParticle.toContainedElement(*cont, const_cast<Rec::TrackParticle*>(trackParticle));
  }
}

/** low Pt parameters from low pt */
void Muon::set_numberOfAssociatedEtaDigits(int nMDT, int nCSC, int nRPC, int nTGC) {
  m_associatedEtaDigits.clear();
  m_associatedEtaDigits.push_back(nMDT);
  m_associatedEtaDigits.push_back(nCSC);
  m_associatedEtaDigits.push_back(nRPC);
  m_associatedEtaDigits.push_back(nTGC);

}
void Muon::set_numberOfAssociatedPhiDigits(int nMDT, int nCSC, int nRPC, int nTGC) {
  m_associatedPhiDigits.clear();
  m_associatedPhiDigits.push_back(nMDT);
  m_associatedPhiDigits.push_back(nCSC);
  m_associatedPhiDigits.push_back(nRPC);
  m_associatedPhiDigits.push_back(nTGC);
}

void Muon::set_numberOfAssociatedEtaDigits(const std::vector<int>& associated_digits) {
  m_associatedEtaDigits = associated_digits;
}

void Muon::set_numberOfAssociatedPhiDigits(const std::vector<int>& associated_digits) {
  m_associatedPhiDigits = associated_digits;
}
void Muon::set_numberOfSegmentEtaDigits(int nMDT, int nCSC, int nRPC, int nTGC) {
  m_associatedEtaDigits.clear();
  m_associatedEtaDigits.push_back(nMDT);
  m_associatedEtaDigits.push_back(nCSC);
  m_associatedEtaDigits.push_back(nRPC);
  m_associatedEtaDigits.push_back(nTGC);

}
void Muon::set_numberOfSegmentPhiDigits(int nMDT, int nCSC, int nRPC, int nTGC) {
  m_associatedPhiDigits.clear();
  m_associatedPhiDigits.push_back(nMDT);
  m_associatedPhiDigits.push_back(nCSC);
  m_associatedPhiDigits.push_back(nRPC);
  m_associatedPhiDigits.push_back(nTGC);
}

void Muon::set_numberOfSegmentEtaDigits(const std::vector<int>& associated_digits) {
  m_associatedEtaDigits = associated_digits;
}

void Muon::set_numberOfSegmentPhiDigits(const std::vector<int>& associated_digits) {
  m_associatedPhiDigits = associated_digits;
}

/** set a parameter in the MuonParamDefs.h */
void Muon::set_parameter(MuonParameters::ParamDef index, double value, bool overwrite) { 

  typedef std::pair<MuonParameters::ParamDef,float> muonParams;

  std::vector<muonParams>::iterator p = m_parameters.begin();
 
  for (;p !=m_parameters.end(); p++) {
    if ( (*p).first == index ) break;
  }

  if ( p == m_parameters.end() ) {
    m_parameters.push_back( muonParams(index,(float)value) );
  }
  else {
    if ( overwrite ) {
      (*p).second = (float)value;
    }
    //else {      
    //    throw GaudiException("parameter not saved", "Muon::set_parameter(...)", true);
      //int ind = static_cast<int>(index);
      //std::cout << "parameter not overwritten - Muon::set_parameter(...) index = " << ind << std::endl;

    //}
  }
}

///////////////////////////////////
// Navigation without parameters //
///////////////////////////////////

void Muon::fillToken(INavigationToken& theToken) const
{

  bool checkFlag = false;

  //////////////////////////
  // First check on track //
  //////////////////////////

  
  const Rec::TrackParticle* aTrack;

  /// First combined, then try Extrap & In Det
  if ( this->combinedMuonTrackParticle() )    {

    aTrack = this->combinedMuonTrackParticle();  

    // check requested object type with token type
    NavigationToken<Rec::TrackParticle>* trackToken =
      dynamic_cast< NavigationToken<Rec::TrackParticle>* >(&theToken);
    if ( trackToken != 0 )	{
      // request is honored
      trackToken->setObject(aTrack);
      checkFlag = true;
    }

    // alternative token type may phrase the same request
    if ( ! checkFlag ) 	{
      NavigationToken<Rec::TrackParticle,double>* parTrackToken =
	dynamic_cast< NavigationToken<Rec::TrackParticle,double>* >
	(&theToken);
      if ( parTrackToken != 0 )	    {
	parTrackToken->setObject(aTrack);
	checkFlag = true;
      }
    }
    if(!checkFlag) {   
      // try for generic type requests
      if ( aTrack  != 0 ) theToken.trySetObject(aTrack);
    }

  }
  else     {
    if ( this->muonExtrapolatedTrackParticle() ) {
      aTrack = this->muonExtrapolatedTrackParticle();  

      // check requested object type with token type
      NavigationToken<Rec::TrackParticle>* trackToken =
	dynamic_cast< NavigationToken<Rec::TrackParticle>* >(&theToken);
      if ( trackToken != 0 )	{
	// request is honored
	trackToken->setObject(aTrack);
	checkFlag = true;
      }

      // alternative token type may phrase the same request
      if ( ! checkFlag ) 	{
	NavigationToken<Rec::TrackParticle,double>* parTrackToken =
	  dynamic_cast< NavigationToken<Rec::TrackParticle,double>* >
	  (&theToken);
	if ( parTrackToken != 0 )	    {
	  parTrackToken->setObject(aTrack);
	  checkFlag = true;
	}
      }
      if(!checkFlag) {
    
	// try for generic type requests
	if ( aTrack  != 0 ) theToken.trySetObject(aTrack);
      }

    }
   
    if ( this->inDetTrackParticle() ) {
      aTrack = this->inDetTrackParticle();  

      // check requested object type with token type
      NavigationToken<Rec::TrackParticle>* trackToken =
	dynamic_cast< NavigationToken<Rec::TrackParticle>* >(&theToken);
      if ( trackToken != 0 )	{
	// request is honored
	trackToken->setObject(aTrack);
	checkFlag = true;
      }

      // alternative token type may phrase the same request
      if ( ! checkFlag ) 	{
	NavigationToken<Rec::TrackParticle,double>* parTrackToken =
	  dynamic_cast< NavigationToken<Rec::TrackParticle,double>* >
	  (&theToken);
	if ( parTrackToken != 0 )	    {
	  parTrackToken->setObject(aTrack);
	  checkFlag = true;
	}
      }
      if(!checkFlag) {
    
	// try for generic type requests
	if ( aTrack  != 0 ) theToken.trySetObject(aTrack);
      }
    }
    
  }

  return;
}

////////////////////////////////
// Navigation with parameters //
///////////////////////////////

void Muon::fillToken(INavigationToken& theToken,
			 const boost::any& theWeight) const
{
  bool checkFlag = false;

  
  const Rec::TrackParticle* aTrack;

  /// First combined, then try Extrap & In Det
  if ( this->combinedMuonTrackParticle() )    {

    aTrack = this->combinedMuonTrackParticle();  

    // check requested object type with token type
    NavigationToken<Rec::TrackParticle>* trackToken =
      dynamic_cast< NavigationToken<Rec::TrackParticle>* >(&theToken);
    if ( trackToken != 0 )	{
      // request is honored
      trackToken->setObject(aTrack);
      checkFlag = true;
    }

    // alternative token type may phrase the same request
    if ( ! checkFlag ) 	{
      NavigationToken<Rec::TrackParticle,double>* parTrackToken =
	dynamic_cast< NavigationToken<Rec::TrackParticle,double>* >
	(&theToken);
      if ( parTrackToken != 0 )	    {
	parTrackToken->setObject(aTrack,
				 boost::any_cast<double>(theWeight));
	checkFlag = true;
      }
    }
    if(!checkFlag) {
    
      // try for generic type requests
      if ( aTrack  != 0 ) theToken.trySetObject(aTrack, theWeight);
    }

  } 
  else     {
    if ( this->muonExtrapolatedTrackParticle() ) {
      aTrack = this->muonExtrapolatedTrackParticle();  

      // check requested object type with token type
      NavigationToken<Rec::TrackParticle>* trackToken =
	dynamic_cast< NavigationToken<Rec::TrackParticle>* >(&theToken);
      if ( trackToken != 0 )	{
	// request is honored
	trackToken->setObject(aTrack);
	checkFlag = true;
      }

      // alternative token type may phrase the same request
      if ( ! checkFlag ) 	{
	NavigationToken<Rec::TrackParticle,double>* parTrackToken =
	  dynamic_cast< NavigationToken<Rec::TrackParticle,double>* >
	  (&theToken);
	if ( parTrackToken != 0 )	    {
	  parTrackToken->setObject(aTrack,
				   boost::any_cast<double>(theWeight));
	  checkFlag = true;
	}
      }
      if(!checkFlag) {
    
	// try for generic type requests
	if ( aTrack  != 0 ) theToken.trySetObject(aTrack, theWeight);
      }

    }
   
    if ( this->inDetTrackParticle() ) {
      aTrack = this->inDetTrackParticle();  

      // check requested object type with token type
      NavigationToken<Rec::TrackParticle>* trackToken =
	dynamic_cast< NavigationToken<Rec::TrackParticle>* >(&theToken);
      if ( trackToken != 0 )	{
	// request is honored
	trackToken->setObject(aTrack);
	checkFlag = true;
      }

      // alternative token type may phrase the same request
      if ( ! checkFlag ) 	{
	NavigationToken<Rec::TrackParticle,double>* parTrackToken =
	  dynamic_cast< NavigationToken<Rec::TrackParticle,double>* >
	  (&theToken);
	if ( parTrackToken != 0 )	    {
	  parTrackToken->setObject(aTrack,
				   boost::any_cast<double>(theWeight));
	  checkFlag = true;
	}
      }
      if(!checkFlag) {
    
	// try for generic type requests
	if ( aTrack  != 0 ) theToken.trySetObject(aTrack, theWeight);
      }
    }
    
  }

}

/** return the primary track of this muon */
const Rec::TrackParticle * Muon::track() const {
   if (this->hasCombinedMuonTrackParticle()) return (*m_combinedMuonTrackParticle);
   else if (this->hasInnerExtrapolatedTrackParticle()) return (*m_innerExtrapolatedTrackParticle);
   //else if (this->hasMuonExtrapolatedTrackParticle()) return (*m_muonExtrapolatedTrackParticle);
   else if (this->hasInDetTrackParticle()) return (*m_inDetTrackParticle);
   else if (this->hasMuonExtrapolatedTrackParticle()) return (*m_muonExtrapolatedTrackParticle);
   else return 0;
}

} //namespace


