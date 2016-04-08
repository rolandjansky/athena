// dear emacs, this is really -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_EGAMMA_H
#define EGAMMAEVENT_EGAMMA_H 1
/**
  @class egamma
          elec/gamma data class. It contains the different egDetails.
	  The class is made by egammaBuilder, which is an algorithm 
   @author H. Ma, S. Rajagopalan, D. Zerwas, K. Cranmer, 
          S. Laplace, M. Wielers, A. Khodinov, F. Derue, H. Kim 
*/
/********************************************************************
NAME:     egamma.h
PACKAGE:  offline/Reconstruction/egammaRec
CREATED:  Nov, 2000
REWRITTEN Nov, 2006
UPDATED:
          Jun 14, 2007 (FD) Doxygen documentation
          Nov 24, 2009 (FD) add isElectron, isPhoton and use unsigned int
          Jan 20, 2010 (MS) add enumeration and code for uncombined 4mom 
********************************************************************/

// INCLUDE HEADER FILES:

#include "FourMom/Lib/P4ImplEEtaPhiM.h"
#include "EventKernel/INavigable4Momentum.h"

#include "DataModel/ElementLink.h"
#include "DataModel/ElementLinkVector.h"

#include "ParticleEvent/ParticleImpl.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "VxVertex/VxContainer.h"
#include "egammaEvent/egDetailContainer.h"
#include "egammaEvent/egDetail.h"

#include "egammaEvent/egPID.h"
#include "egammaEvent/egammaPIDdefs.h"

#include "egammaEvent/CaloRingsContainer.h"

#include <boost/any.hpp>

class EMErrorDetail;
class CaloCluster;
typedef NavigableTerminalNode egammaNavigation;

class egamma
  : public ParticleImpl<
             egammaNavigation, // not really a terminal node: done here
             P4ImplEEtaPhiM >
{

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // for readability and lazy people
  typedef ParticleImpl< egammaNavigation,
			P4ImplEEtaPhiM
	  > egammaImpl_t;
  typedef egammaImpl_t::navigable_type navigable_type;
  typedef egammaImpl_t::momentum_type  momentum_type;
  typedef egammaImpl_t::particle_type  particle_type;

 public:
  
  enum eg4MomType {
    // standard cluster-track combined 4Mom
    Combined   = 0,
    // uncombined cluster 4Mom
    Uncombined = 1
  };      

 public:

  /** @brief Default constructor*/
  egamma();
  /** @brief Default constructor*/
  egamma(unsigned int author);
  /** @brief Destructor*/
  virtual ~egamma();
  egamma(const egamma &rhs, bool copyDetails=true, bool copyMinimal=false);
  egamma& operator=(const egamma& rhs);

  /** @brief print method*/
  void print() const;

  /** @brief set energy data member */
  void setE (double theE);
  /** @brief set eta data member */
  void setEta (double theEta);
  /** @brief set phi data member */
  void setPhi (double thePhi);
  /** @brief set mass data member */
  void setM (double theM);

  /** @brief set all 4-mom from another I4Momentum reference  */  
  void set4Mom (const I4Momentum & theI4Mom );
  /** @brief set all 4-mom from another I4Momentum pointer  */  
  void set4Mom (const I4Momentum * const theI4Mom );
  /** @brief set all 4-mom from a CLHEP HepLorentzVector */  
  void set4Mom (const CLHEP::HepLorentzVector & theHlv );  

  /** @brief set uncombined energy data member */
  void setUncombinedE (double theE);
  /** @brief set uncombined eta data member */
  void setUncombinedEta (double theEta);
  /** @brief set uncombined phi data member */
  void setUncombinedPhi (double thePhi);
  /** @brief set uncombined mass data member */
  void setUncombinedM (double theM);

  /** @brief set uncombined all 4-mom from another I4Momentum reference  */  
  void setUncombined4Mom (const I4Momentum & theI4Mom );
  /** @brief set uncombined all 4-mom from another I4Momentum pointer  */  
  void setUncombined4Mom (const I4Momentum * const theI4Mom );
  /** @brief set uncombined all 4-mom from a CLHEP HepLorentzVector */  
  void setUncombined4Mom (const CLHEP::HepLorentzVector & theHlv );  

  /** @brief pointer to CaloCluster*/
  const CaloCluster*        cluster()            const;
  /** @brief pointer to CaloRings*/
  const CaloRings*        rings()            const;
  /** @brief pointer to TrackParticle*/
  const Rec::TrackParticle* trackParticle (unsigned int index = 0) const;
  unsigned int nTrackParticles() const;
  /** @brief pointer to Conversion*/
  const Trk::VxCandidate* conversion (unsigned int index = 0) const;
  unsigned int nConversions() const;

  /** @brief retrieve eg-detail objects:*/
  template <typename T>
  const T* detail(const std::string& name = "", unsigned int index = 0) const;

  /** @brief pointer to detail className of container name*/
  const egDetail* detail (const std::string& className,
                          const std::string& name = "",
			  unsigned int index = 0) const;
  /** @brief pointer to detail i*/
  const egDetail* detail (int i) const;
  /** @brief ElementLink for detail i*/
  ElementLink<egDetailContainer> detailElementLink (int i) const;
  const ElementLinkVector<egDetailContainer>& detailElementLinkVector() const;
  /** @brief name of detail i*/
  std::string detailName (int i) const;
  /** @brief number of details in egamma object*/
  int nDetails() const;
  /** @brief get the float value of key in the container name*/
  double detailValue (egammaParameters::ParamDef key, 
		      const std::string& name = "", 
		      unsigned int index = 0) const;

  /** @brief element link to trackParticle*/
  ElementLink<Rec::TrackParticleContainer> trackParticleElementLink(unsigned int index = 0) const;
  const ElementLinkVector<Rec::TrackParticleContainer>& trackParticleElementLinkVector() const;
  /** @brief element link to cluster*/
  const ElementLink<CaloClusterContainer>& clusterElementLink() const;
  /** @brief element link to conversion*/
  ElementLink<VxContainer> conversionElementLink(unsigned int index = 0) const;
  const ElementLinkVector<VxContainer>& conversionElementLinkVector() const;
  /** @brief element link to rings*/
  const ElementLink<CaloRingsContainer>& ringsElementLink() const;

  /** @brief access to PID information, as double to work for IsEM 
      and all possible weights as likelihood */
  double egammaID(egammaPID::PID, bool *found) const;
  double egammaID(egammaPID::PID) const;
  const egPID* pid() const;

  /** @brief set detail */
  void setDetail(const egDetailContainer*, const egDetail*); 
  /** @brief set detailElementLink */
  void setDetailElementLink(const ElementLink<egDetailContainer>& link); 

  void setDetailElementLinkVector(const ElementLinkVector<egDetailContainer>& v); 
  void setDetailElementLinkVector(ElementLinkVector<egDetailContainer>&& v); 

  /** @brief set particle ID */
  void set_pid(egPID* );
  /** @brief set_egamma ID, for doubles
      and all possible weights as likelihood */
  bool set_egammaID(egammaPID::PID id, double result);

  /** @brief set_egamma ID, for usigned int values */
  bool set_egammaIDint(egammaPID::PID id, unsigned int result);

  // move this in egPID and just provide return of egPID 
  //  const std::vector< std::pair<egammaPID::PID,double> > & get_egammaID()       const;

  // to save pointers to access the different objects of the e/g data class

  /** @brief Set cluster */ 
  void setCluster(const CaloClusterContainer*, int);
  /** @brief Set cluster */
  void setCluster(const CaloClusterContainer*, const CaloCluster*);
  /** @briefSet Cluster Element Link */ 
  void setClusterElementLink(const ElementLink<CaloClusterContainer>& link);
  /** @brief Reset Cluster */
  void resetCluster();

  /** @brief Set rings object */
  void setRings(const CaloRingsContainer*, int);
  /** @brief Set rings object */
  void setRings(const CaloRingsContainer*, const CaloRings*);
  /** @briefSet Rings Element Link */
  void setRingsElementLink(const ElementLink<CaloRingsContainer>& link);
  /** @brief Reset Cluster */
  void resetRings();


  /** @brief Set Track Particle */
  void setTrackParticle(const Rec::TrackParticleContainer *, int) ;
  /** @brief Set Track Particle */
  void setTrackParticle(const Rec::TrackParticleContainer*, const Rec::TrackParticle*);
  /** @brief Set Track Particle Link */
  void setTrackParticleElementLink(const ElementLink<Rec::TrackParticleContainer>& link);
  /** @brief Reset Track Particle */
  void resetTrackParticle(unsigned int index = 0);

  void setTrackParticleElementLinkVector(const ElementLinkVector<Rec::TrackParticleContainer>& v);
  void setTrackParticleElementLinkVector(ElementLinkVector<Rec::TrackParticleContainer>&& v);

  /** @brief  Set Conversion */
  void setConversion(const VxContainer *, int) ;
  /** @brief  Set Conversion */
  void setConversion(const VxContainer*, const Trk::VxCandidate*);
  /** @brief  Set Conversion */
  void setConversionElementLink(const ElementLink<VxContainer>& link);
  /** @brief  Reset Conversion */
  void resetConversion(unsigned int index = 0);

  void setConversionElementLinkVector(const ElementLinkVector<VxContainer>& link);
  void setConversionElementLinkVector(ElementLinkVector<VxContainer>&& link);

  /** @brief Reconstruction Author  */
  unsigned int author() const {return m_author; }
  /** @brief Reconstruction Author */
  bool author(const unsigned int author) const;
  /** @brief Add author */
  void addAuthor(unsigned int author) {m_author |= author; }
  /** @brief set_egamma ID */
  void resetAuthor() {m_author = 0; }


  /** @brief fill token */
  virtual void fillToken( INavigationToken & thisToken ) const;
  /** @brief fill token */
  virtual void fillToken( INavigationToken & thisToken, const boost::any& ) const;

  /** @brief ID flag with cuts, true:e/phot, false:bkg  */
  bool isElectron(unsigned int mask= egammaPID::ALL, 
		  egammaPID::PID pid=egammaPID::IsEM, 
		  bool *found = NULL) const;
  bool isPhoton(unsigned int mask= egammaPID::ALL, 
		egammaPID::PID pid=egammaPID::IsEM, 
		bool *found = NULL) const; // exactly the same as isElectron

   /** @brief does electron/photon pass the given quality  */
  bool passID(egammaPID::egammaIDQuality id) const;

  /** @brief uses special softe cuts */
  bool isSofte(unsigned int mask= egammaPID::ALL, bool *found = NULL) const;

  /** @brief ID flag with cuts, 0:e, >1:jet  */
  unsigned int isem(unsigned int mask= egammaPID::ALL, 
		    egammaPID::PID pid=egammaPID::IsEM) const;
  unsigned int isem(unsigned int mask, 
		    egammaPID::PID pid,
		    bool *found) const;
  /** @brief ID flag with cuts for softe, 0:e, >1:jet */
  unsigned int isemse(unsigned int mask= egammaPID::ALL) const;
  unsigned int isemse(unsigned int mask, bool *found) const;

  /** @brief OQ flag  */  
  bool isGoodOQ(unsigned int mask= egammaPID::ALLOQ, bool *found = NULL) const;
  unsigned int isgoodoq(unsigned int mask= egammaPID::ALLOQ, bool *found = NULL) const;

  
  /** @brief override standard errors to allow lazy loading */
  virtual const I4MomentumError* errors() const;

  void setErrorMatrix(Amg::MatrixX);
  void setUncombinedErrorMatrix(Amg::MatrixX);

  /** @brief return the combined or additional uncombined cluster 4mom */
  //  P4ImplEEtaPhiM&  get4Mom() { return m_momentumCluster; } 
  const momentum_type&  get4Mom( egamma::eg4MomType type=egamma::Combined ) const;

 private:

  ElementLink<CaloClusterContainer> m_cluster;
  ElementLinkVector<Rec::TrackParticleContainer>  m_trackParticle;
  ElementLinkVector<VxContainer>  m_conversion;
  ElementLinkVector<egDetailContainer> m_egDetails;
  ElementLink<CaloRingsContainer> m_rings;

  unsigned int m_author;

  egPID* m_egPID;

 protected:

  void setupErrorMatrices() const;
  
  // extra 4 momentum for the cluster based egamma object 
  // P4ImplEEtaPhiM  m_momentumCluster; 
  momentum_type  m_momentumCluster; 

};

template <typename T>
const T* egamma::detail(const std::string& key, unsigned int index) const {
    std::size_t nsize = m_egDetails.size();
    std::size_t iloop = 0;
//     int nptr = 0;
    const T* ptr = 0;
//     const T* tmp = 0;

    for (; iloop != nsize; iloop++) {
      if (key.empty()) {
	if(!m_egDetails[iloop]) continue;
        ptr = dynamic_cast<const T*>(*(m_egDetails[iloop]));
	if (0 != ptr && ptr->linkIndex() == (int)index) {
// 	  nptr++; //this is potentially highly confusing now with the ElementLinkVectors and the conversion refits, rather return the first match if there are multiple matches
// 	  ptr = tmp;
	  return ptr;
	}
      }
      else if (m_egDetails[iloop].isValid() &&
               key == m_egDetails.elementDataID(iloop))
      {
	if(!m_egDetails[iloop]) continue;
        ptr = dynamic_cast<const T*>(*(m_egDetails[iloop]));
	if (0 != ptr && ptr->linkIndex() == (int)index){
	  return ptr;
	}
      }
    }
    return ptr;
//     return (nptr==1) ? ptr : 0;
}

// inline methods

/** @brief set energy data member */
inline
void
egamma::setE (double ene)
{ this->momentumBase().setE (ene); }

/** @brief set eta data member */
inline
void
egamma::setEta (double eta)
{ this->momentumBase().setEta (eta); }

/** @brief set phi data member */
inline
void
egamma::setPhi (double phi)
{ this->momentumBase().setPhi (phi); }

/** @brief set mass data member */
inline
void
egamma::setM (double m)
{ this->momentumBase().setM (m); }

/** @brief set all 4-mom from another I4Momentum reference  */  
inline
void 
egamma::set4Mom (const I4Momentum & mom )
{ this->momentumBase().set4Mom (mom); }

/** @brief set all 4-mom from another I4Momentum pointer  */  
inline
void 
egamma::set4Mom (const I4Momentum * const mom )
{ this->momentumBase().set4Mom (mom); }

/** @brief set all 4-mom from a CLHEP HepLorentzVector */  
inline
void 
egamma::set4Mom (const CLHEP::HepLorentzVector & hlv )
{ this->momentumBase().set4Mom (hlv); }

/** @brief return the combined, or additional uncombined cluster 4mom */
inline                                                       
const egamma::momentum_type&  egamma::get4Mom( egamma::eg4MomType type ) const
{ 
  const I4MomentumError* errors = momentumBase().errors();
  if (errors == 0) {
    setupErrorMatrices();
  }
  if ( type==egamma::Combined ) return this->momentumBase(); 
  else                          return m_momentumCluster;    
}                                                            


  

#endif

