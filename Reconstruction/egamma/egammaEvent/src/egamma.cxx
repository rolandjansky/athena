/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:

#include "egammaEvent/egamma.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/EMTrackMatch.h"
#include "egammaEvent/EMConvert.h"
#include "CaloEvent/CaloCluster.h"
#include "egammaEvent/EMErrorDetail.h"
#include "egammaEvent/egammaPIDdefsObs.h"
#include "AthenaKernel/BaseInfo.h"

#include <iomanip>
#include <iostream>

//  END OF HEADER FILES INCLUDE

// ----------------------------
//  Constructor
// ----------------------------
egamma::egamma() :
  IAthenaBarCode(),
  INavigable (),
  I4Momentum (),
  INavigable4Momentum (),
  egammaImpl_t(),
  m_author (egammaParameters::AuthorUnknown),
  m_momentumCluster() 
{ 
  m_egPID = new egPID;
}

// ==========================================================
egamma::egamma(unsigned int author) :
  IAthenaBarCode(),
  INavigable (),
  I4Momentum (),
  INavigable4Momentum (),
  egammaImpl_t(),
  m_author (author),
  m_momentumCluster() 
{ 
  m_egPID = new egPID;
}

// --------------------------
// Destructor
// --------------------------
egamma::~egamma()
{
  if(m_egPID) {
    delete m_egPID; 
    m_egPID = nullptr;
  }
}

// ==========================================================
// copy constructor
egamma::egamma(const egamma &rhs, bool copyDetails, bool copyMinimal):
  IAthenaBarCode      (rhs),
  INavigable          (rhs),
  I4Momentum          (rhs),
  INavigable4Momentum (rhs),
  egammaImpl_t        (rhs)
{
  m_author        = rhs.m_author; 
  m_momentumCluster = rhs.m_momentumCluster; 
  m_egPID         = new egPID(*(rhs.m_egPID));
  if (!copyMinimal) {
    m_cluster       = rhs.m_cluster;
    m_trackParticle = rhs.m_trackParticle;
    m_conversion    = rhs.m_conversion;
    if (copyDetails) {
      m_egDetails     = rhs.m_egDetails;
    }
  }
}

// ==========================================================
egamma& egamma::operator=(const egamma& rhs)
{
  if (this != &rhs) {
    IAthenaBarCode::operator=(rhs);
    INavigable::operator=(rhs);
    I4Momentum::operator=(rhs);
    INavigable4Momentum::operator=(rhs);
    egammaImpl_t::operator=(rhs);
    m_author        = rhs.m_author; 
    m_cluster       = rhs.m_cluster;
    m_trackParticle = rhs.m_trackParticle;
    m_conversion    = rhs.m_conversion;
    if(m_egPID)       delete m_egPID;
    m_egPID         = new egPID(*(rhs.m_egPID));
    m_egDetails     = rhs.m_egDetails;
    m_momentumCluster = rhs.m_momentumCluster; // ??? 
  }
  return *this;
}

// ----------------------------
// print egamma object
// ----------------------------
void egamma::print() const
{
  // loop over egDetails and print later.
}

// four momentum methods

///////////////////////////////////
// Navigation without parameters //
///////////////////////////////////

void 
egamma::fillToken(INavigationToken& theToken) const
{

  bool checkFlag = false;

  //////////////////////////
  // First check on track //
  //////////////////////////

  const Rec::TrackParticle* aTrack = this->trackParticle();  
  if ( aTrack != nullptr )
    {
      // check requested object type with token type
      NavigationToken<Rec::TrackParticle>* trackToken =
	dynamic_cast< NavigationToken<Rec::TrackParticle>* >(&theToken);
      if ( trackToken != nullptr )
	{
	  // request is honored
	  trackToken->setObject(aTrack);
	  checkFlag = true;
	}

      // alternative token type may phrase the same request
      if ( ! checkFlag ) 
	{
	  NavigationToken<Rec::TrackParticle,double>* parTrackToken =
	    dynamic_cast< NavigationToken<Rec::TrackParticle,double>* >
	    (&theToken);
	  if ( parTrackToken != nullptr )
	    {
	      parTrackToken->setObject(aTrack);
	      checkFlag = true;
	    }
	}
    }

  //////////////////////////
  // At this point only a request for Rec::TrackParticle could have been
  // honored. This is indicated by checkFlag = true. If checkFlag = false,
  // a test on a request for CaloCluster is performed.
  //////////////////////////

  // bail out if request for Rec::TrackParticle is already honored
  if (  checkFlag ) return;

  //////////////////////
  // Check on cluster //
  //////////////////////

  const CaloCluster* aCluster = this->cluster();
  if ( aCluster != nullptr )
    {
      // check on requested type
      NavigationToken<CaloCluster>* clusToken =
	dynamic_cast< NavigationToken<CaloCluster>* >(&theToken);
      if ( clusToken != nullptr )
        {
          clusToken->setObject(aCluster);
          checkFlag = true;
	}
      // allow for visitor with relational parameter type
      if ( ! checkFlag ) 
	{ 
	  NavigationToken<CaloCluster,double>* parClusToken =
	    dynamic_cast< NavigationToken<CaloCluster,double>* >(&theToken);
	  if ( parClusToken != nullptr )
	    {
	      parClusToken->setObject(aCluster);
	      checkFlag = true;
	    }
	}
    }

  // concrete type request for CaloCluster honored
  if ( checkFlag ) return;

  ///////////////////////////////////////////////////////////////////
  // Pass on the token to composite constituents: ie. CaloCluster! //
  ///////////////////////////////////////////////////////////////////

  if ( aCluster!= nullptr ) aCluster->fillToken(theToken);

  //////////////////
  // At this point no request for concrete egamma constituents has been
  // submitted if checkFlag = false. Now we need to check if a generic
  // type has been requested (i.e. a base type for tracks and clusters).
  /////////////////

  // try for generic type requests
  if ( aTrack  != nullptr ) theToken.trySetObject(aTrack);
  if ( aCluster!= nullptr ) theToken.trySetObject(aCluster);
}

////////////////////////////////
// Navigation with parameters //
///////////////////////////////

void egamma::fillToken(INavigationToken& theToken,
		       const boost::any& theWeight) const
{
  bool checkFlag = false;

  //////////////////////////
  // First check on track //
  //////////////////////////
  const Rec::TrackParticle* aTrack = this->trackParticle();  
  if ( aTrack != nullptr )
    {
      // check requested object type with token type
      NavigationToken<Rec::TrackParticle>* trackToken =
	dynamic_cast< NavigationToken<Rec::TrackParticle>* >(&theToken);
      if ( trackToken != nullptr )
	{
	  // request is honored
	  trackToken->setObject(aTrack);
	  checkFlag = true;
	}

      // alternative token type may phrase the same request
      if ( ! checkFlag )
	{
	  NavigationToken<Rec::TrackParticle,double>* parTrackToken =
	    dynamic_cast< NavigationToken<Rec::TrackParticle,double>* >
	    (&theToken);
	  if ( parTrackToken != nullptr )
	    {
	      // no weight change in propagation
	      parTrackToken->setObject(aTrack,
				       boost::any_cast<double>(theWeight));
	      checkFlag = true;
	    }
	}
    }

  // checks on track successful (see above)
  if ( checkFlag ) return;

  //////////////////////
  // Check on cluster //
  //////////////////////

  const CaloCluster* aCluster = this->cluster();  
  if ( aCluster != nullptr )
    {
      // check on requested type
      NavigationToken<CaloCluster>* clusToken =
	dynamic_cast< NavigationToken<CaloCluster>* >(&theToken);
      if ( clusToken != nullptr )
        {
          clusToken->setObject(aCluster);
	  checkFlag = true;
	}
      // allow for visitor with relational parameter type
      if ( ! checkFlag )
	{ 
	  NavigationToken<CaloCluster,double>* parClusToken =
	    dynamic_cast< NavigationToken<CaloCluster,double>* >(&theToken);
	  if ( parClusToken != nullptr )
	    {
	      parClusToken->setObject(aCluster,
				      boost::any_cast<double>(theWeight));
	      checkFlag = true;
	    }
	}
    }

  // concrete check for CaloCluster honored
  if ( checkFlag ) return;

  ///////////////////////////////////////////////////////////////////
  // Pass on the token to composite constituents: ie. CaloCluster! //
  ///////////////////////////////////////////////////////////////////

  if ( aCluster != nullptr )  aCluster->fillToken(theToken,theWeight);


  //////////////////
  // At this point no request for concrete egamma constituents has been
  // submitted if checkFlag = false. Now we need to check if a generic
  // type has been requested (i.e. a base type for tracks and clusters).
  /////////////////

  // try for generic type requests
  if ( aTrack   != nullptr ) theToken.trySetObject(aTrack,  theWeight);
  if ( aCluster != nullptr ) theToken.trySetObject(aCluster,theWeight);
}

// ==========================================================
// Don't use a default for the found parameter because PyROOT has
// problems passing bool* arguments.
double egamma::egammaID(egammaPIDObs::PID idflag, bool *found) const 
{
  //
  // method to retrieve the PID
  // set as double to work for IsEM but also for all weights as likelihood
  //

  return m_egPID->egammaID(idflag, found);
}

double egamma::egammaID(egammaPIDObs::PID idflag) const 
{
  //
  // method to retrieve the PID
  // set as double to work for IsEM but also for all weights as likelihood
  //

  return m_egPID->egammaID(idflag, nullptr);
}

// ==========================================================
bool egamma::set_egammaID(egammaPIDObs::PID idflag, double result)
{
  //
  // method to set the PID
  // set as double to work for IsEM but also for all weights as likelihood
  //

  return m_egPID->set_egammaID(idflag,result); 
}

// ==========================================================
bool egamma::set_egammaIDint(egammaPIDObs::PID idflag, unsigned int result)
{
  //
  // method to set the PID for ints directly
  //

  return m_egPID->set_egammaIDint(idflag,result); 
}

// ==========================================================
bool egamma::author(const unsigned int author) const
{
  //
  // method to check the author
  //

  return (m_author & author) != 0;
}

// ==========================================================
const CaloCluster*  egamma::cluster() const
{ 
  //
  // method to acces the cluster object
  //

  return (m_cluster.isValid()) ? *m_cluster : nullptr;
} 


const CaloRings*  egamma::rings() const
{ 
  //
  // methods to access RingerRings
  //

  return (m_rings.isValid()) ? *m_rings : nullptr;
}

const ElementLink<CaloRingsContainer>& egamma::ringsElementLink() const 
{ 
  //
  // methods to access RingerRings ElementLink
  //
  return m_rings;
}


// ==========================================================

// ==========================================================
const Rec::TrackParticle* egamma::trackParticle (unsigned int index) const
{ 
  //
  // method to acces the track object
  //

  if(index < m_trackParticle.size()){
    return ((m_trackParticle.at(index)).isValid()) ? *(m_trackParticle.at(index)) : nullptr;
  }
    return nullptr;
  
}   

// ==========================================================
unsigned int egamma::nTrackParticles() const
{
  return m_trackParticle.size();
}

// ==========================================================
const Trk::VxCandidate* egamma::conversion (unsigned int index) const
{ 
  if(index < m_conversion.size()){
    return ((m_conversion.at(index)).isValid()) ? *(m_conversion.at(index)) : nullptr;
  }
    return nullptr;
  
}   

// ==========================================================
unsigned int egamma::nConversions() const
{
  return m_conversion.size();
}

// ==========================================================
const ElementLink<CaloClusterContainer>& egamma::clusterElementLink() const 
{ 
  return m_cluster;
}

// ==========================================================
ElementLink<Rec::TrackParticleContainer> egamma::trackParticleElementLink(unsigned int index) const 
{ 
  if(index < m_trackParticle.size()){
    return m_trackParticle.at(index);
  }
    return ElementLink<Rec::TrackParticleContainer>();
  
}

// ==========================================================
const ElementLinkVector<Rec::TrackParticleContainer>&
egamma::trackParticleElementLinkVector() const 
{ 
  return m_trackParticle;
}

// ==========================================================
ElementLink<VxContainer> egamma::conversionElementLink(unsigned int index) const 
{ 
  if(index < m_conversion.size()){
    return m_conversion.at(index);
  }
    return ElementLink<VxContainer>();
  
}

// ==========================================================
const ElementLinkVector<VxContainer>&
egamma::conversionElementLinkVector() const 
{ 
  return m_conversion;
}

// ==========================================================
void egamma::setCluster(const CaloClusterContainer *pCont, int index) 
{ 
  m_cluster.toIndexedElement( *pCont, index);
} 

// ==========================================================
void egamma::setRings(const CaloRingsContainer *pCont, int index) 
{ 
  m_rings.toIndexedElement( *pCont, index);
} 

// ==========================================================
void egamma::resetRings()
{
  m_rings.reset();
}

// ==========================================================
void egamma::setRings (const CaloRingsContainer * pCont, const CaloRings* cluster)
{ 
  m_rings.toContainedElement(*pCont, cluster); 
}

// ==========================================================
void 
egamma::setRingsElementLink (const ElementLink<CaloRingsContainer>& link) 
{ 
  m_rings = link;
}


// ==========================================================
void egamma::resetCluster()
{
  m_cluster.reset();
}

// ==========================================================
void egamma::setCluster (const CaloClusterContainer * pCont, const CaloCluster* cluster)
{ 
  m_cluster.toContainedElement(*pCont, cluster); 
}

// ==========================================================
void 
egamma::setClusterElementLink (const ElementLink<CaloClusterContainer>& link) 
{ 
  m_cluster = link;
}

// ==========================================================
void egamma::resetTrackParticle(unsigned int index)
{
  if(index < m_trackParticle.size()){
    (m_trackParticle.at(index)).reset();
  }
}

// ==========================================================
void egamma::setTrackParticle (const Rec::TrackParticleContainer * pCont, int index) 
{ 
  ElementLink<Rec::TrackParticleContainer> link;
  link.toIndexedElement(*pCont, index);
  m_trackParticle.push_back(link); 
}

// ==========================================================
void egamma::setTrackParticle (const Rec::TrackParticleContainer * pCont, const Rec::TrackParticle* track)
{ 
  ElementLink<Rec::TrackParticleContainer> link;
  link.toContainedElement(*pCont, track);
  m_trackParticle.push_back(link); 
}

// ==========================================================
void 
egamma::setTrackParticleElementLink (const ElementLink<Rec::TrackParticleContainer>& link) 
{ 
  m_trackParticle.push_back(link);
}

// ==========================================================
void 
egamma::setTrackParticleElementLinkVector (const ElementLinkVector<Rec::TrackParticleContainer>& v) 
{ 
  m_trackParticle = v;
}

// ==========================================================
void 
egamma::setTrackParticleElementLinkVector (ElementLinkVector<Rec::TrackParticleContainer>&& v) 
{ 
  m_trackParticle = std::move(v);
}

// ==========================================================
void egamma::resetConversion(unsigned int index)
{
  if(index < m_conversion.size()){
    (m_conversion.at(index)).reset();
  }
}

// ==========================================================
void egamma::setConversion (const VxContainer * pCont, int index) 
{ 
  ElementLink<VxContainer> link;
  link.toIndexedElement(*pCont, index);
  m_conversion.push_back(link); 
}

// ==========================================================
void egamma::setConversion (const VxContainer * pCont, const Trk::VxCandidate* conv)
{ 
  ElementLink<VxContainer> link;
  link.toContainedElement(*pCont, conv);
  m_conversion.push_back(link); 
}

// ==========================================================
void
egamma::setConversionElementLink (const ElementLink<VxContainer>& link) 
{ 
  m_conversion.push_back(link);
}

// ==========================================================
void
egamma::setConversionElementLinkVector (const ElementLinkVector<VxContainer>& v) 
{ 
  m_conversion = v;
}

// ==========================================================
void
egamma::setConversionElementLinkVector (ElementLinkVector<VxContainer>&& v) 
{ 
  m_conversion = std::move(v);
}

// ==========================================================
// methods to save the pointer to the different data objects in e/g data class
void egamma::setDetail(const egDetailContainer* cont, 
			      const egDetail* ptr) 
{ 
  ElementLink<egDetailContainer> link;
  link.toContainedElement( *cont, ptr);
  m_egDetails.push_back( link );
} 

// ==========================================================
void
egamma::setDetailElementLink(const ElementLink<egDetailContainer>& link) 
{ 
  m_egDetails.push_back( link );
} 


// ==========================================================
void
egamma::setDetailElementLinkVector(const ElementLinkVector<egDetailContainer>& v) 
{ 
  m_egDetails = v;
} 


// ==========================================================
void
egamma::setDetailElementLinkVector(ElementLinkVector<egDetailContainer>&& v) 
{ 
  m_egDetails = std::move(v);
} 


// ==========================================================
const egPID* egamma::pid() const 
{ 
  return m_egPID; 
}

// ==========================================================
void egamma::set_pid(egPID* ptr) 
{ 
  if (m_egPID != nullptr) delete m_egPID;
  m_egPID = ptr; 
}

// ==========================================================
bool egamma::isElectron(unsigned int mask, 
			egammaPIDObs::PID pid, 
			bool *found) const
{ 
  //
  // boolean method to define if egamma object is identified 
  // by standard cut-based selection
  //

  return (m_egPID->isEM(mask, pid, found)==0);
} 

// ==========================================================
bool egamma::isPhoton(unsigned int mask, 
		      egammaPIDObs::PID pid,
		      bool *found) const
{ 
  //
  // boolean method to define if egamma object is identified 
  // by standard cut-based selection
  //

  return (m_egPID->isEM(mask, pid, found)==0);
} 

// ==========================================================
bool egamma::isSofte(unsigned int mask, bool *found) const
{ 
  //
  // boolean method to define if egamma object is identified 
  // by cut-based selection optimized for non-isolated objects
  //

  return (m_egPID->isEMsofte(mask, found)==0);
} 

// ==========================================================
bool egamma::passID(egammaPIDObs::egammaIDQuality id) const
{ 
  //
  // boolean method to define if egamma object is identified 
  // by standard cut-based selection
  //

  switch(id) {
  case egammaPIDObs::ElectronIDLoose:
    return isElectron(egammaPIDObs::ElectronLoose, egammaPIDObs::IsEM);
  case egammaPIDObs::ElectronIDMedium:
    return isElectron(egammaPIDObs::ElectronMedium, egammaPIDObs::IsEM);
  case egammaPIDObs::ElectronIDTight:
    return isElectron(egammaPIDObs::ElectronTight, egammaPIDObs::IsEM);

  case egammaPIDObs::ElectronIDLoosePP:
    return isElectron(egammaPIDObs::ElectronLoosePP, egammaPIDObs::IsEMLoose);
  case egammaPIDObs::ElectronIDMediumPP:
    return isElectron(egammaPIDObs::ElectronMediumPP, egammaPIDObs::IsEMMedium);
  case egammaPIDObs::ElectronIDTightPP:
    return isElectron(egammaPIDObs::ElectronTightPP, egammaPIDObs::IsEMTight);

  case egammaPIDObs::PhotonIDLoose:
    return isPhoton(egammaPIDObs::PhotonLoose, egammaPIDObs::IsEM);
  case egammaPIDObs::PhotonIDMedium:
    return isPhoton(egammaPIDObs::PhotonMedium, egammaPIDObs::IsEMMedium);
  case egammaPIDObs::PhotonIDTight:
    return isPhoton(egammaPIDObs::PhotonTight, egammaPIDObs::IsEM);

  case egammaPIDObs::PhotonIDLooseAR:
    return isPhoton(egammaPIDObs::PhotonLooseAR, egammaPIDObs::IsEM);
  case egammaPIDObs::PhotonIDMediumAR:
    return isPhoton(egammaPIDObs::PhotonMediumAR, egammaPIDObs::IsEMMedium);
  case egammaPIDObs::PhotonIDTightAR:
    return isPhoton(egammaPIDObs::PhotonTightAR, egammaPIDObs::IsEM);

   case egammaPIDObs::ElectronIDLooseIso:
     return isElectron(egammaPIDObs::ElectronLooseIso, egammaPIDObs::IsEM);
  case egammaPIDObs::ElectronIDMediumIso:
    return isElectron(egammaPIDObs::ElectronMediumIso, egammaPIDObs::IsEM);
  case egammaPIDObs::ElectronIDTightIso:
    return isElectron(egammaPIDObs::ElectronTightIso, egammaPIDObs::IsEM);

  case egammaPIDObs::ElectronIDLoosePPIso:
    return isElectron(egammaPIDObs::ElectronLooseIso, egammaPIDObs::IsEMLoose);
  case egammaPIDObs::ElectronIDMediumPPIso:
    return isElectron(egammaPIDObs::ElectronMediumPPIso, egammaPIDObs::IsEMMedium);
  case egammaPIDObs::ElectronIDTightPPIso:
    return isElectron(egammaPIDObs::ElectronTightPPIso, egammaPIDObs::IsEMTight);

  case egammaPIDObs::PhotonIDLooseIso:
    return isPhoton(egammaPIDObs::PhotonLooseIso, egammaPIDObs::IsEM);
  case egammaPIDObs::PhotonIDTightIso:
    return isPhoton(egammaPIDObs::PhotonTightIso, egammaPIDObs::IsEM);
  case egammaPIDObs::PhotonIDLooseARIso:
    return isPhoton(egammaPIDObs::PhotonLooseARIso, egammaPIDObs::IsEM);
  case egammaPIDObs::PhotonIDTightARIso:
    return isPhoton(egammaPIDObs::PhotonTightARIso, egammaPIDObs::IsEM);

  case egammaPIDObs::frwdElectronIDLoose:
    return isElectron(egammaPIDObs::frwdElectronLoose, egammaPIDObs::IsEM);
  case egammaPIDObs::frwdElectronIDTight:
    return isElectron(egammaPIDObs::frwdElectronTight, egammaPIDObs::IsEM);

  case egammaPIDObs::NoIDCut:
    return true;

  default:
    return false;
  }
} 


// ==========================================================
// Note: We have two separate overloads here (and for isemse) rather
// than using a default for `found' because PyROOT complains about
// passing bool* arguments.
unsigned int egamma::isem(unsigned int mask, 
			  egammaPIDObs::PID pid) const
{ 
  //
  // method which applies a mask to the isEM variable
  // and returns the corresponding decoded value
  //

  return m_egPID->isEM(mask, pid, nullptr); 
} 


unsigned int egamma::isem(unsigned int mask, 
			  egammaPIDObs::PID pid,
			  bool *found) const
{ 
  //
  // method which applies a mask to the isEM variable
  // and returns the corresponding decoded value
  //

  return m_egPID->isEM(mask, pid, found); 
} 

// ==========================================================
unsigned int egamma::isemse(unsigned int mask) const
{ 
  //
  // method which applies a mask to the isEMsofte variable
  // and returns the corresponding decoded value
  //

  return m_egPID->isEMsofte(mask, nullptr); 
} 


unsigned int egamma::isemse(unsigned int mask, bool *found) const
{ 
  //
  // method which applies a mask to the isEMsofte variable
  // and returns the corresponding decoded value
  //

  return m_egPID->isEMsofte(mask, found); 
} 


// ==========================================================
bool egamma::isGoodOQ(unsigned int mask, bool *found) const
{
  return (m_egPID->IsGoodOQ(mask, found) == 0);
}

// ==========================================================
unsigned int egamma::isgoodoq(unsigned int mask, bool *found) const
{ 
  //
  // method which applies a mask to theIsGoodOQ  variable
  // and returns the corresponding decoded value
  //

  return m_egPID->IsGoodOQ(mask, found); 
} 

// ==========================================================
const egDetail* egamma::detail (int i) const
{
  if (i < 0 || i >= (int)m_egDetails.size())
    return nullptr;
  return *m_egDetails[i];
}

// ==========================================================
const egDetail* egamma::detail (const std::string& className,
				const std::string& name /*= ""*/,
				unsigned int index) const
{
  size_t nsize = m_egDetails.size();
//   const egDetail* ptr = 0;
  for (size_t i = 0; i < nsize; i++) {
    if (m_egDetails[i].isValid() &&
        (*m_egDetails[i])->className() == className)
    {
      if (name.empty()) {
//         if (ptr) //this is potentially highly confusing now with the ElementLinkVectors and the conversion refits, rather return the first match if there are multiple matches
//           return 0;
//         else
	if((*m_egDetails[i])->linkIndex() == (int)index)
	  return *m_egDetails[i];
//  	  ptr = *m_egDetails[i];
      }
      else if (m_egDetails.elementDataID(i) == name && (*m_egDetails[i])->linkIndex() == (int)index)
        return *m_egDetails[i];
    }
  }
//   return ptr;
  return nullptr;
}

// ==========================================================
ElementLink<egDetailContainer> egamma::detailElementLink (int i) const
{
  if (i < 0 || i >= (int)m_egDetails.size())
    return ElementLink<egDetailContainer>();
  return m_egDetails[i];
}

// ==========================================================
const ElementLinkVector<egDetailContainer>& egamma::detailElementLinkVector () const
{
  return m_egDetails;
}

// ==========================================================
std::string egamma::detailName (int i) const
{
  if (i < 0 || i >= (int)m_egDetails.size())
    return "";
  return m_egDetails.elementDataID(i);
}

// ==========================================================
int egamma::nDetails () const
{
  return m_egDetails.size();
}


// ==========================================================
double 
egamma::detailValue (egammaParameters::ParamDef key, 
		     const std::string& name, unsigned int index) const
{
  size_t nsize = m_egDetails.size();

  const egDetail* ptr = nullptr;
  for (size_t i = 0; i < nsize; i++) {
    if (m_egDetails[i].isValid()) {
      if (name.empty()) {
	ptr = *m_egDetails[i];
      }
      else if (m_egDetails.elementDataID(i) == name)
	ptr = *m_egDetails[i];
    }
    if ( ptr && ptr->hasParameter(key) && ptr->linkIndex() == (int)index)
      return ptr->parameter(key);
  }
  
  return egammaParameters::EgParamUndefined;
}

// ==========================================================
/** @brief set combined energy data member */
void
egamma::setUncombinedE (double ene)
{ m_momentumCluster.setE (ene); }

// ==========================================================
/** @brief set combined eta data member */
void
egamma::setUncombinedEta (double eta)
{ m_momentumCluster.setEta (eta); }

// ==========================================================
/** @brief set combined phi data member */
void
egamma::setUncombinedPhi (double phi)
{ m_momentumCluster.setPhi (phi); }

// ==========================================================
/** @brief set combined mass data member */
void
egamma::setUncombinedM (double m)
{ m_momentumCluster.setM (m); }

// ==========================================================
/** @brief set combined all 4-mom from another I4Momentum reference  */  
void 
egamma::setUncombined4Mom (const I4Momentum & mom )
{ m_momentumCluster.set4Mom (mom); }

// ==========================================================
/** @brief set combined all 4-mom from another I4Momentum pointer  */  
void 
egamma::setUncombined4Mom (const I4Momentum * const mom )
{ m_momentumCluster.set4Mom (mom); }

// ==========================================================
/** @brief set combined all 4-mom from a CLHEP::HepLorentzVector */  
void 
egamma::setUncombined4Mom (const CLHEP::HepLorentzVector & hlv )
{ m_momentumCluster.set4Mom (hlv); }


// ==========================================================
const I4MomentumError* egamma::errors() const
{
  return momentumBase().errors();
}

// ==========================================================
void egamma::setErrorMatrix( Amg::MatrixX thematrix) 
{
  const unsigned int Nrows =thematrix.rows();
  CLHEP::HepSymMatrix thematrixCLHEP(Nrows,0);
  
  for (unsigned int i=0; i<Nrows;++i) {
    for (unsigned int j=0; j<Nrows;++j){
      thematrixCLHEP[i][j]= thematrix(i,j);
    }
  }
  this->momentumBase().setErrors(thematrixCLHEP);	
}
	
// ==========================================================
void egamma::setUncombinedErrorMatrix(Amg::MatrixX thematrix) 
{
  //This should be symmetrix in any case
  const unsigned int Nrows =thematrix.rows();
  CLHEP::HepSymMatrix thematrixCLHEP(Nrows,0);
  
for (unsigned int i=0; i<Nrows;++i) {
    for (unsigned int j=0; j<Nrows;++j){
      thematrixCLHEP[i][j]= thematrix(i,j);
    }
  }
  m_momentumCluster.setErrors(thematrixCLHEP);	
}


SG_ADD_BASE (egamma, SG_VIRTUAL (IAthenaBarCode));
