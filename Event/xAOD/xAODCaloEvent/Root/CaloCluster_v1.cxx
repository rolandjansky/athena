/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloCluster_v1.cxx 783594 2016-11-11 05:03:25Z ssnyder $

// System include(s):
#include <cmath>
#include <iostream>
#include <array>

// EDM include(s):
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODCaloEvent/versions/CaloCluster_v1.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloClusterAccessors_v1.h"


namespace xAOD {

   CaloCluster_v1::CaloCluster_v1()
     : IParticle(), 
       m_samplingPattern(0), 
       m_cellLinks(0),
       m_ownCellLinks(false)
   {
     setSignalState(CALIBRATED);
   }


  CaloCluster_v1::CaloCluster_v1(const CaloCluster_v1& other)
    : IParticle(), //IParticel does not have a copy constructor. AuxElement has one with same behavior as default ctor
      m_samplingPattern(other.samplingPattern()), 
      m_cellLinks(0), 
      m_ownCellLinks(false),
      m_recoStatus(other.m_recoStatus) {
    setSignalState(other.signalState());
    this->makePrivateStore(other);
#if !(defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS))
    const CaloClusterCellLink* links=other.getCellLinks();
    if (links) {
      this->addCellLink(new CaloClusterCellLink(*links));
    }
    static const Accessor<ElementLink<CaloClusterCellLinkContainer> > accCellLinks("CellLink");
    if (accCellLinks.isAvailable(*this)) { //In case an element link was copied by makePrivateStore, invalidate it
      accCellLinks(*this).reset();
    } //end if have element link to CaloClusterCellLink
#endif // not defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS)
  }
  

  CaloCluster_v1& CaloCluster_v1::operator=(const xAOD::CaloCluster_v1& other) {
    if (this == &other) return *this;

    SG::AuxElement::operator=( other ); //Call assignment operator of base-class
    m_recoStatus=other.m_recoStatus;
    setSignalState(other.signalState());
    m_samplingPattern=other.m_samplingPattern;
    m_ownCellLinks=false;

#if !(defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS))
     const CaloClusterCellLink* links=other.getCellLinks();
     if (links) {
       this->addCellLink(new CaloClusterCellLink(*links));
     }
     static const Accessor<ElementLink<CaloClusterCellLinkContainer> > accCellLinks("CellLink");
     if (accCellLinks.isAvailable(*this)) { //In case an element link was copied by  SG::AuxElement::operator=, invalidate it
       accCellLinks(*this).reset();
     } //end if have element link to CaloClusterCellLink
#endif // not defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS)
     return *this;
  }


   CaloCluster_v1::~CaloCluster_v1() {

      if( m_ownCellLinks ) delete m_cellLinks;
   }

  void CaloCluster_v1::setSamplingPattern( const unsigned sp, const bool clearSamplingVars) {

      // Check sampling variables ....
      static const Accessor< std::vector< float > > etaAcc( "eta_sampl" );
      static const Accessor< std::vector< float > > phiAcc( "phi_sampl" );
      static const Accessor< std::vector< float > > eAcc( "e_sampl" );
      static const Accessor< std::vector< float > > emaxAcc( "emax_sampl" );
      static const Accessor< std::vector< float > > etamaxAcc( "etamax_sampl" );
      static const Accessor< std::vector< float > > phimaxAcc( "phimax_sampl" );
      static const Accessor< std::vector< float > > etasizeAcc( "etasize_sampl" );
      static const Accessor< std::vector< float > > phisizeAcc( "phisize_sampl" );

#if __cplusplus < 201100
      static std::vector<  Accessor< std::vector< float > >* > allAcc;
      if( ! allAcc.size() ) {
         allAcc.push_back( &etaAcc );
         allAcc.push_back( &phiAcc );
         allAcc.push_back( &eAcc );
         allAcc.push_back( &emaxAcc );
         allAcc.push_back( &phimaxAcc );
         allAcc.push_back( &etamaxAcc );
         allAcc.push_back( &etasizeAcc );
         allAcc.push_back( &phisizeAcc );
      }
      std::vector< Accessor< std::vector< float > >* >::iterator acc_itr =
         allAcc.begin();
      std::vector< Accessor< std::vector< float > >* >::iterator acc_end =
         allAcc.end();
      for( ; acc_itr != acc_end; ++acc_itr ) {
         if( ( *acc_itr )->isAvailable( *this ) ) {
            if( ( **acc_itr )( *this ).size() > 0 ) {
	      if (clearSamplingVars) 
		( **acc_itr )( *this ).clear();
	      else
               std::cerr << "CaloCluster_v1 ERROR Attempt update sampling "
                         << "pattern while sampling variables are already set!"
                         << std::endl;
	      //std::abort();
            }
         }
      }
#else
      static const std::array< const Accessor< std::vector< float > >*, 8 > allAcc = {
         { &etaAcc, &phiAcc, &eAcc, &emaxAcc, &phimaxAcc, &etamaxAcc, &etasizeAcc,
           &phisizeAcc } };
      for( auto a : allAcc ) {
         if( a->isAvailable( *this ) ) {
            if( ( *a )( *this ).size() > 0 ) {
	      if (clearSamplingVars) 
		(*a)(*this).clear();
	      else
               std::cerr << "CaloCluster_v1 ERROR Attempt update sampling "
                         << "pattern while sampling variables are already set!"
                         << std::endl;
	      //std::abort();
            }
         }
      }
#endif // C++11

      m_samplingPattern=sp;
   }


  /// Notice that this function is very slow for calorimeter clusters, so it
  /// should be called as few times as possible.
  ///
  /// @returns The transverse momentum of the cluster
  ///
  double CaloCluster_v1::pt(const State s) const {
    // Calculate the momentum of the object:
    double theE = 0;
    double theM = 0;
    switch (s) {
    case CALIBRATED:
      theE=calE();
      theM=calM();
      break;
    case UNCALIBRATED:
      theE=rawE();
      theM=rawM();
      break;
    case ALTCALIBRATED:
      theE=altE();
      theM=altM();
      break;
    default:
      break;
    }

    double p = 0.0;
    if( std::abs( theM ) < 0.00001 ) {
      p = theE;
    } else {
      p = std::sqrt( theE * theE - theM * theM );
      if( theE < 0 ) {
        p = -p;
      }
    }

    // Calculate sinTh:
    double aEta = std::abs( eta(s) );
    if( aEta > 710.0 ) {
      aEta = 710.0;
    }
    const double sinTh = 1.0 / std::cosh( aEta );
    
    // Calculate pT from these two:
    return p * sinTh;
  }

  /**
   * @brief Return eta for a specific signal state.
   * @param s The desired signal state.
   */
  double CaloCluster_v1::e(const State s) const
  {
    switch (s) {
    case CALIBRATED:
      return calE();
      break;
    case UNCALIBRATED:
      return rawE();
      break;
    case ALTCALIBRATED:
      return altE();
      break;
    default:
      return 0;
    }
  }

  /**
   * @brief Return eta for a specific signal state.
   * @param s The desired signal state.
   */
  double CaloCluster_v1::eta(const State s) const
  {
    switch (s) {
    case CALIBRATED:
      return calEta();
      break;
    case UNCALIBRATED:
      return rawEta();
      break;
    case ALTCALIBRATED:
      return altEta();
      break;
    default:
      return -999;
    }
  }


  /**
   * @brief Return phi for a specific signal state.
   * @param s The desired signal state.
   */
  double CaloCluster_v1::phi(const State s) const
  {
    switch (s) {
    case CALIBRATED:
      return calPhi();
      break;
    case UNCALIBRATED:
      return rawPhi();
      break;
    case ALTCALIBRATED:
      return altPhi();
      break;
    default:
      return -999;
    }
  }
   /**
   * @brief Return m for a specific signal state.
   * @param s The desired signal state.
   */
 double CaloCluster_v1::m(const State s) const {
    switch (s) {
    case CALIBRATED:
      return calM();
      break;
    case UNCALIBRATED:
      return rawM();
      break;
    case ALTCALIBRATED:
      return altM();
      break;
    default:
      return -999;
      }
   }
 

  double CaloCluster_v1::pt() const {
    return pt(m_signalState);
  }

  double CaloCluster_v1::eta() const
  {
    return eta (m_signalState);
  }
  
  double CaloCluster_v1::phi() const
  {
    return phi (m_signalState);
  }

  double CaloCluster_v1::m() const {
    return m(m_signalState);
   }
  
   double CaloCluster_v1::e() const {
    return e(m_signalState);
   }


  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CaloCluster_v1, CaloCluster_v1::flt_t,  eta0, setEta0 )			 
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CaloCluster_v1, CaloCluster_v1::flt_t,  phi0, setPhi0 )			 
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CaloCluster_v1, CaloCluster_v1::flt_t,  time, setTime )			 

  void CaloCluster_v1::setBadChannelList(const CaloClusterBadChannelList& bcl) {
    static const Accessor<xAOD::CaloClusterBadChannelList> accBCL("BadChannelList");
    accBCL(*this)=bcl;
    return;
  }
    
   const CaloClusterBadChannelList& CaloCluster_v1::badChannelList() const {
    static const Accessor<xAOD::CaloClusterBadChannelList> accBCL("BadChannelList");
    return accBCL(*this);
  }

  void CaloCluster_v1::setRawE(const CaloCluster_v1::flt_t value) {
    static const Accessor<CaloCluster_v1::flt_t> accRawE("rawE");
    accRawE(*this)=value;
  }

  void CaloCluster_v1::setRawEta(const CaloCluster_v1::flt_t value) {
    static const Accessor<CaloCluster_v1::flt_t> accRawEta("rawEta");
    accRawEta(*this)=value;
  }

  void CaloCluster_v1::setRawPhi(const CaloCluster_v1::flt_t value) {
    static const Accessor<CaloCluster_v1::flt_t> accRawPhi("rawPhi");
    accRawPhi(*this)=value;
  }

  void CaloCluster_v1::setRawM(const CaloCluster_v1::flt_t value) {
    static const Accessor<CaloCluster_v1::flt_t> accRawM("rawM");
    accRawM(*this)=value;
  }  

  //----------------------------------------------------------------

  void CaloCluster_v1::setCalE(const CaloCluster_v1::flt_t value) {
    static const Accessor<CaloCluster_v1::flt_t> accCalE("calE");
    accCalE(*this)=value;
  }

  void CaloCluster_v1::setCalEta(const CaloCluster_v1::flt_t value) {
    static const Accessor<CaloCluster_v1::flt_t> accCalEta("calEta");
    accCalEta(*this)=value;
  }

  void CaloCluster_v1::setCalPhi(const CaloCluster_v1::flt_t value) {
    static const Accessor<CaloCluster_v1::flt_t> accCalPhi("calPhi");
    accCalPhi(*this)=value;
  }

  void CaloCluster_v1::setCalM(const CaloCluster_v1::flt_t value) {
    static const Accessor<CaloCluster_v1::flt_t> accCalM("calM");
    accCalM(*this)=value;
  }  

  //----------------------------------------------------------------

  void CaloCluster_v1::setAltE(const CaloCluster_v1::flt_t value) {
    static const Accessor<CaloCluster_v1::flt_t> accAltE("altE");
    accAltE(*this)=value;
  }

  void CaloCluster_v1::setAltEta(const CaloCluster_v1::flt_t value) {
    static const Accessor<CaloCluster_v1::flt_t> accAltEta("altEta");
    accAltEta(*this)=value;
  }

  void CaloCluster_v1::setAltPhi(const CaloCluster_v1::flt_t value) {
    static const Accessor<CaloCluster_v1::flt_t> accAltPhi("altPhi");
    accAltPhi(*this)=value;
  }

  void CaloCluster_v1::setAltM(const CaloCluster_v1::flt_t value) {
    static const Accessor<CaloCluster_v1::flt_t> accAltM("altM");
    accAltM(*this)=value;
  } 

  AUXSTORE_PRIMITIVE_GETTER( CaloCluster_v1, CaloCluster_v1::flt_t,  rawE)	           		 
  AUXSTORE_PRIMITIVE_GETTER( CaloCluster_v1, CaloCluster_v1::flt_t,  rawEta)
  AUXSTORE_PRIMITIVE_GETTER( CaloCluster_v1, CaloCluster_v1::flt_t,  rawPhi)	           	 
  AUXSTORE_PRIMITIVE_GETTER( CaloCluster_v1, CaloCluster_v1::flt_t,  rawM)
	           		 
  AUXSTORE_PRIMITIVE_GETTER( CaloCluster_v1, CaloCluster_v1::flt_t,  altE)
  AUXSTORE_PRIMITIVE_GETTER( CaloCluster_v1, CaloCluster_v1::flt_t,  altEta)
  AUXSTORE_PRIMITIVE_GETTER( CaloCluster_v1, CaloCluster_v1::flt_t,  altPhi)	           	 
  AUXSTORE_PRIMITIVE_GETTER( CaloCluster_v1, CaloCluster_v1::flt_t,  altM)

  AUXSTORE_PRIMITIVE_GETTER( CaloCluster_v1, CaloCluster_v1::flt_t,  calE)
  AUXSTORE_PRIMITIVE_GETTER( CaloCluster_v1, CaloCluster_v1::flt_t,  calEta)
  AUXSTORE_PRIMITIVE_GETTER( CaloCluster_v1, CaloCluster_v1::flt_t,  calPhi)	           	 
  AUXSTORE_PRIMITIVE_GETTER( CaloCluster_v1, CaloCluster_v1::flt_t,  calM)


  CaloCluster_v1::ClusterSize  CaloCluster_v1::clusterSize() const {
    static const Accessor<unsigned> acc("clusterSize");
    return (CaloCluster_v1::ClusterSize)acc(*this);
  }
   
  void  CaloCluster_v1::setClusterSize(CaloCluster_v1::ClusterSize sc) {
    static const Accessor<unsigned> acc("clusterSize");
    acc(*this)=sc;
  }
  

  void CaloCluster_v1::setE(CaloCluster_v1::flt_t theE) {
    switch (m_signalState) {
    case CALIBRATED:
      return setCalE(theE);
      break;
    case UNCALIBRATED:
      return setRawE(theE);
       break;
     case ALTCALIBRATED:
       return setAltE(theE);
       break;
     default:
       break;
     }
    return;
  }

  void CaloCluster_v1::setEta(CaloCluster_v1::flt_t theEta) {
    switch (m_signalState) {
    case CALIBRATED:
      return setCalEta(theEta);
      break;
    case UNCALIBRATED:
      return setRawEta(theEta);
       break;
     case ALTCALIBRATED:
       return setAltEta(theEta);
       break;
     default:
       break;
     }
    return;
  }
  
  void CaloCluster_v1::setPhi(CaloCluster_v1::flt_t thePhi) {
    switch (m_signalState) {
    case CALIBRATED:
      return setCalPhi(thePhi);
      break;
    case UNCALIBRATED:
      return setRawPhi(thePhi);
       break;
     case ALTCALIBRATED:
       return setAltPhi(thePhi);
       break;
     default:
       break;
     }
    return;
  }


  void CaloCluster_v1::setM(CaloCluster_v1::flt_t theM) {
    switch (m_signalState) {
    case CALIBRATED:
      return setCalM(theM);
      break;
    case UNCALIBRATED:
      return setRawM(theM);
       break;
     case ALTCALIBRATED:
       return setAltM(theM);
       break;
     default:
       break;
     }
    return;
  }
 
  bool CaloCluster_v1::setSignalState( CaloCluster_v1::State s)  {
    m_signalState=s;
    return true;
  }
  
  CaloCluster_v1::GenVecFourMom_t CaloCluster_v1::genvecP4(const CaloCluster_v1::State s) const {
    switch (s) {
    case CALIBRATED:
      return GenVecFourMom_t(pt(s),calEta(),calPhi(),calM());
    case UNCALIBRATED:
      return GenVecFourMom_t(pt(s),rawEta(),rawPhi(), rawM());
    case ALTCALIBRATED:
      return GenVecFourMom_t(pt(s),altEta(),altPhi(), altM());
    default:
      return GenVecFourMom_t();
    }
  }
  CaloCluster_v1::GenVecFourMom_t CaloCluster_v1::genvecP4() const {
    return genvecP4(m_signalState);
  }

  double CaloCluster_v1::rapidity() const {
    return genvecP4().Rapidity();
  }
  
  CaloCluster_v1::FourMom_t CaloCluster_v1::p4() const {
    return p4(m_signalState);
  }


  CaloCluster_v1::FourMom_t  CaloCluster_v1::p4(const CaloCluster_v1::State s) const  {
    CaloCluster_v1::FourMom_t p4;
    switch(s) {
    case CALIBRATED:
      p4.SetPtEtaPhiM(pt(s),calEta(),calPhi(),calM());   
      break;
    case UNCALIBRATED:
      p4.SetPtEtaPhiM(pt(s),rawEta(),rawPhi(), rawM());  
      break;
    case ALTCALIBRATED:
      p4.SetPtEtaPhiM(pt(s),altEta(),altPhi(), altM());  
      break;
    default:
      break;
    }
    return p4;
  }

  
  Type::ObjectType CaloCluster_v1::type() const {
    return Type::CaloCluster;
  }
  
  
  float CaloCluster_v1::getSamplVarFromAcc(const Accessor< std::vector <float > >& acc , const CaloSample sampling, const float errorvalue) const {
    const std::vector<float>& vec=acc(*this);
    const unsigned idx=sampVarIdx(sampling); 
    if (idx<vec.size() ) {
      return vec[idx];
    }
    else
      //std::cout <<Sampling " << sampling << ", Pattern=" << std::hex <<m_samplingPattern << std::dec << ", index=" << idx << " size=" << vec.size() << std::endl;
      return errorvalue;
  }

  bool CaloCluster_v1::setSamplVarFromAcc(const Accessor< std::vector <float > >& acc, const CaloSample sampling, const float value) {
    const unsigned idx=sampVarIdx(sampling); 
    std::vector<float>& vec=acc(*this);
    //std::cout << "Set sampling var. Sampling " << sampling << ", index=" << idx << " size=" << vec.size() << std::endl;
    if (idx==CaloSampling::Unknown) {
      std::cout << "ERROR: Sampling #" << sampling << " is not part of this cluster!" << std::endl;
      return false;
    }

    if (vec.size()<nSamples())
      vec.resize(nSamples());  
    vec[idx]=value;
    return true;
  }


  float CaloCluster_v1::eSample( CaloSample sampling ) const {
    static const Accessor< std::vector <float > > eAcc("e_sampl");
    return getSamplVarFromAcc(eAcc,sampling,0.0); //Return energy 0 in case of failure (eg. sampling not set)
  }

  bool CaloCluster_v1::setEnergy(const CaloSample sampling, const float theEnergy) {
    static const Accessor< std::vector <float > > eAcc("e_sampl");
    return setSamplVarFromAcc(eAcc,sampling,theEnergy);
  }
  

  float CaloCluster_v1::etaSample(const CaloSample sampling) const {
    static const Accessor< std::vector <float > > etaAcc("eta_sampl");
    if (!etaAcc.isAvailable( *this ))
      return -999;
    else
      return getSamplVarFromAcc(etaAcc,sampling);
  }

  bool CaloCluster_v1::setEta(const CaloSample sampling, const float eta) {
    static const Accessor< std::vector <float > > etaAcc("eta_sampl");
    return setSamplVarFromAcc(etaAcc,sampling,eta);
   }


  float CaloCluster_v1::phiSample(const CaloSample sampling) const {
    static const Accessor< std::vector <float > > phiAcc("phi_sampl");
    if (!phiAcc.isAvailable( *this ))
      return -999;
    else
      return getSamplVarFromAcc(phiAcc,sampling);
  }

  bool CaloCluster_v1::setPhi(const CaloSample sampling, const float phi) {
    static const Accessor< std::vector <float > > phiAcc("phi_sampl");
    return setSamplVarFromAcc(phiAcc,sampling,phi);
  }
  


  float CaloCluster_v1::energy_max(const CaloSample sampling) const {
    static const Accessor< std::vector <float > > emaxAcc("emax_sampl");
    if (!emaxAcc.isAvailable( *this ))
      return 0.0;
    else
      return getSamplVarFromAcc(emaxAcc,sampling,0.0); //Return energy 0 in case of failure (eg. sampling not set)
  }

  bool CaloCluster_v1::setEmax(const CaloSample sampling, const float eMax ) {
    static const Accessor< std::vector <float > > emaxAcc("emax_sampl");
    return setSamplVarFromAcc(emaxAcc,sampling,eMax);
  }
  
  float CaloCluster_v1::etamax(const CaloSample sampling) const {
    static const Accessor< std::vector <float > > etamaxAcc("etamax_sampl");
    if (!etamaxAcc.isAvailable( *this ))
      return -999;
    else
      return getSamplVarFromAcc(etamaxAcc,sampling);
  }

  bool CaloCluster_v1::setEtamax(const CaloSample sampling, const float etaMax ) {
    static const Accessor< std::vector <float > > etamaxAcc("etamax_sampl");
    return setSamplVarFromAcc(etamaxAcc,sampling,etaMax);
  }
  
  float CaloCluster_v1::phimax(const CaloSample sampling) const {
    static const Accessor< std::vector <float > > phimaxAcc("phimax_sampl");
    if (!phimaxAcc.isAvailable( *this ))
      return -999;
    else
      return getSamplVarFromAcc(phimaxAcc,sampling);
  }

  bool CaloCluster_v1::setPhimax(const CaloSample sampling, const float phiMax ) {
    static const Accessor< std::vector <float > > phimaxAcc("phimax_sampl");
    return setSamplVarFromAcc(phimaxAcc,sampling,phiMax);
  }

  
  float CaloCluster_v1::etasize(const CaloSample sampling) const {
    static const Accessor< std::vector <float > > etasizeAcc("etasize_sampl");
    if (!etasizeAcc.isAvailable( *this ))
      return -999;
    else
      return getSamplVarFromAcc(etasizeAcc,sampling);
  }

  bool CaloCluster_v1::setEtasize(const CaloSample sampling, const float etaSize ) {
    static const Accessor< std::vector <float > > etasizeAcc("etasize_sampl");
    return setSamplVarFromAcc(etasizeAcc,sampling,etaSize);
  }
  
  float CaloCluster_v1::phisize(const CaloSample sampling) const {
    static const Accessor< std::vector <float > > phisizeAcc("phisize_sampl");
    if (!phisizeAcc.isAvailable( *this ))
      return -999;
    else
      return getSamplVarFromAcc(phisizeAcc,sampling);
  }

  bool CaloCluster_v1::setPhisize(const CaloSample sampling, const float phiSize ) {
    static const Accessor< std::vector <float > > phisizeAcc("phisize_sampl");
    return setSamplVarFromAcc(phisizeAcc,sampling,phiSize);
  }


  float CaloCluster_v1::energyBE(const unsigned sample) const {
    if (sample>3) return -999;
    const CaloSample barrelSample=(CaloSample)(CaloSampling::PreSamplerB+sample);
    const CaloSample endcapSample=(CaloSample)(CaloSampling::PreSamplerE+sample);
    double energy=0;
    if (this->hasSampling(barrelSample)) {
      energy+=eSample(barrelSample); //Check for errorcode? Should not happen...
    }
    if (this->hasSampling(endcapSample)) {
      energy+=eSample(endcapSample);
    } 
    return energy;
  }

  float CaloCluster_v1::etaBE(const unsigned sample) const {
    if (sample>3) return -999;
    const CaloSample barrelSample=(CaloSample)(CaloSampling::PreSamplerB+sample);
    const CaloSample endcapSample=(CaloSample)(CaloSampling::PreSamplerE+sample);
    const bool haveBarrel=this->hasSampling(barrelSample);
    const bool haveEndcap=this->hasSampling(endcapSample);
    if (haveBarrel && haveEndcap) {
      //cluster spans barren and endcap
       float eBarrel=eSample(barrelSample);  //Check for errorcode? Should not happen...
       float eEndcap=eSample(endcapSample);
       
       float etaBarrel=etaSample(barrelSample);
       float etaEndcap=etaSample(endcapSample);
       float eSum=eBarrel + eEndcap;
       if (eSum > 100 /*MeV*/) {
	 //E-weighted average ...
	 return  (eBarrel * etaBarrel + eEndcap * etaEndcap ) / (eBarrel + eEndcap);
       }//else eSum==0 case, should never happen
       return (0.5*(etaBarrel+etaEndcap));
    }
    if  (haveBarrel) {
      return etaSample(barrelSample);
    }
    if (haveEndcap) {
      return etaSample(endcapSample);
    }
      
    //Should never reach this point ...
    return -999;
  }

  
 float CaloCluster_v1::phiBE(const unsigned sample) const {
    if (sample>3) return -999;
    const CaloSample barrelSample=(CaloSample)(CaloSampling::PreSamplerB+sample);
    const CaloSample endcapSample=(CaloSample)(CaloSampling::PreSamplerE+sample);
    const bool haveBarrel=this->hasSampling(barrelSample);
    const bool haveEndcap=this->hasSampling(endcapSample);
    if (haveBarrel && haveEndcap) {
      //cluster spans barren and endcap
       float eBarrel=eSample(barrelSample);  //Check for errorcode? Should not happen...
       float eEndcap=eSample(endcapSample);
       float eSum=eBarrel+eEndcap;
       float phiBarrel=phiSample(barrelSample);
       float phiEndcap=phiSample(endcapSample);
       static CaloPhiRange phiRange;
       if (eSum!=0.0) {
	 float phiSum = eSum * phiBarrel + eEndcap * phiRange.diff(phiEndcap,phiBarrel);
	 return phiRange.fix(phiSum/(eBarrel+eEndcap));
       }
       else {// energy==0 case, should never happen
	 return phiRange.fix(0.5*(phiBarrel+phiEndcap));
       }
    }
    if  (haveBarrel) {
      return phiSample(barrelSample);
    }
    if (haveEndcap) {
      return phiSample(endcapSample);
    }
      
    //Should never reach this point ...
    return -999;
  }


   void  CaloCluster_v1::clearSamplingData() {

      static const Accessor< std::vector< float > > etaAcc( "eta_sampl" );
      static const Accessor< std::vector< float > > phiAcc( "phi_sampl" );
      static const Accessor< std::vector< float > > eAcc( "e_sampl" );
      static const Accessor< std::vector< float > > emaxAcc( "emax_sampl" );
      static const Accessor< std::vector< float > > etamaxAcc( "etamax_sampl" );
      static const Accessor< std::vector< float > > phimaxAcc( "phimax_sampl" );
      static const Accessor< std::vector< float > > etasizeAcc( "etasize_sampl" );
      static const Accessor< std::vector< float > > phisizeAcc( "phisize_sampl" );

#if __cplusplus < 201100
      static std::vector<  Accessor< std::vector< float > >* > allAcc;
      if( ! allAcc.size() ) {
         allAcc.push_back( &etaAcc );
         allAcc.push_back( &phiAcc );
         allAcc.push_back( &eAcc );
         allAcc.push_back( &emaxAcc );
         allAcc.push_back( &phimaxAcc );
         allAcc.push_back( &etamaxAcc );
         allAcc.push_back( &etasizeAcc );
         allAcc.push_back( &phisizeAcc );
      }
      std::vector< Accessor< std::vector< float > >* >::iterator acc_itr =
         allAcc.begin();
      std::vector< Accessor< std::vector< float > >* >::iterator acc_end =
         allAcc.end();
      for( ; acc_itr != acc_end; ++acc_itr ) {
         if( ( *acc_itr )->isAvailableWritable( *this ) ) {
            ( **acc_itr )( *this ).clear();
         }
      }
#else
      static const std::array< const Accessor< std::vector< float > >*, 8 > allAcc = {
         { &etaAcc, &phiAcc, &eAcc, &emaxAcc, &phimaxAcc, &etamaxAcc,
           &etasizeAcc, &phisizeAcc } };
      for( auto a : allAcc ) {
         if( a->isAvailableWritable( *this ) ) {
            ( *a )( *this ).clear();
         }
      }
#endif // C++11

      return;
   }

  bool CaloCluster_v1::retrieveMoment( MomentType type, double& value ) const {

      // Get the moment accessor:
      const Accessor< float >* acc = momentAccessorV1( type );
      if( ! acc ) return false;
      // Check if the moment is available:
      if( ! acc->isAvailable( *this ) ) {
         return false;
      }
      // Retrieve the moment:
      value = ( *acc )( *this );
      return true;
   }


 void CaloCluster_v1::insertMoment( MomentType type, double value ) {
   ( *( momentAccessorV1( type ) ) )( *this ) = value;
   return;
  }
  

  /** for debugging only ...
  std::vector<std::pair<std::string,float> > CaloCluster_v1::getAllMoments() {
    std::vector<std::pair<std::string,float> > retval;
    const SG::auxid_set_t& auxIds=container()->getAuxIDs(); //->getDynamicAuxIDs();
    const size_t idx= this->index();
    for (auto ai: auxIds) {
      const std::string& auxName=SG::AuxTypeRegistry::instance().getName(ai);
      const float v=container()->getData<float>(ai,idx);
      //std::cout << "Index=" <<idx << ", Auxid=" << ai << ", Name=" << auxName << " value=" << v << std::endl;
      retval.push_back(std::make_pair(auxName,v));      
    }
    return retval;
  }
  **/

  unsigned int CaloCluster_v1::getClusterEtaSize() const{
    const unsigned clustersize=clusterSize();
    unsigned int size = 0;
    if(clustersize==SW_55ele || 
       clustersize==SW_55gam || 
       clustersize==SW_55Econv){
      size = 5;
    }else if(clustersize==SW_35ele ||
	     clustersize==SW_37ele ||
	     clustersize==SW_35gam ||
	     clustersize==SW_37gam ||
	     clustersize==SW_35Econv ||
	     clustersize==SW_37Econv){
      size = 3;
    }else if(clustersize==SW_7_11){
      size = 7;
    }
    
    return size;
    
  }

  unsigned int CaloCluster_v1::getClusterPhiSize() const{
    const ClusterSize clustersize=clusterSize();
    unsigned int size = 0;
    if(clustersize==SW_55ele || 
       clustersize==SW_55gam || 
       clustersize==SW_55Econv ||
       clustersize==SW_35ele ||
       clustersize==SW_35gam ||
       clustersize==SW_35Econv){
      size = 5;
    }else if(
	     clustersize==SW_37ele ||
	     clustersize==SW_37gam ||
	     clustersize==SW_37Econv){
      size = 7;
    }else if(clustersize==SW_7_11){
      size = 11;
    }
    return size;
  }
  



  

#if !(defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS))
  /*
  bool CaloCluster_v1::createCellElemLink(const std::string& CCCL_key, const size_t idx) {
    static const Accessor<ElementLink<CaloClusterCellLinkContainer> > accCellLinks("CellLink");
    ElementLink<CaloClusterCellLinkContainer> el(CCCL_key,idx);
    accCellLinks(*this)=el;
    if (m_cellLinks!=0 && m_cellLinks!=(*el)) {
      std::cerr << "ERROR Pointer mismatch! Internal m_cellLinks ptr doesn't match element link" << std::endl;
      return false;
    }	
    return true;
  }
  */
  bool CaloCluster_v1::setLink(CaloClusterCellLinkContainer* cccl,
                               IProxyDict* sg /*= nullptr*/)
  {
    if (!m_cellLinks || !cccl) return false;
    cccl->push_back(m_cellLinks);
    m_ownCellLinks=false; //Cell Links now owned by CaloClusterCellLinkContainer
    const size_t idx=cccl->size()-1; //Use index for speed
    static const Accessor<ElementLink<CaloClusterCellLinkContainer> > accCellLinks("CellLink");
    const CaloClusterCellLinkContainer& ref=*cccl;
    ElementLink<CaloClusterCellLinkContainer> el(ref,idx,sg);
    accCellLinks(*this)=el;    
    return true;
  }

  const CaloClusterCellLink* CaloCluster_v1::getCellLinks() const {
    if (m_cellLinks) return m_cellLinks;
    static const Accessor<ElementLink<CaloClusterCellLinkContainer> > accCellLinks("CellLink");
    if (!accCellLinks.isAvailable(*this)) return 0;

    ElementLink<CaloClusterCellLinkContainer> el=accCellLinks(*this);
    if (el.isValid()) 
      return *el;
    else
      return 0;
  }

  bool CaloCluster_v1::removeCell(const CaloCell* ptrToDelete) {
    //1. Get a ptr to the CaloClusterCellLink
    CaloClusterCellLink* cccl=getCellLinks();
    if (!cccl) return false; //No link found (expected for TopoClusters in xAOD files)
    //2. Remove cell
    return cccl->removeCell(ptrToDelete);
  }


#endif // not defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS)

   /// This function takes care of preparing (all) the ElementLink(s) in the
   /// object to be persistified.
   ///
   void CaloCluster_v1::toPersistent() {

#if !(defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS))
      static const Accessor< ElementLink< CaloClusterCellLinkContainer > >
         accCellLinks( "CellLink" );
      if( accCellLinks.isAvailableWritable( *this ) ) {
         accCellLinks( *this ).toPersistent();
      }
#endif // not defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS)

      static const Accessor< ElementLink< xAOD::CaloClusterContainer_v1 > > accSisterCluster("SisterCluster");
      if( accSisterCluster.isAvailableWritable( *this ) ) {
         accSisterCluster( *this ).toPersistent();
      }
      
      // Return gracefully:
      return;
   }

  const CaloCluster_v1* CaloCluster_v1::getSisterCluster() const {
    static const Accessor< ElementLink< xAOD::CaloClusterContainer_v1 > > accSisterCluster("SisterCluster");
    if (!accSisterCluster.isAvailable(*this)) return 0;
    ElementLink<CaloClusterContainer_v1> el=accSisterCluster(*this);
    if (el.isValid()) 
      return *el;
    else
      return 0;
  }

  const ElementLink<xAOD::CaloClusterContainer_v1>& CaloCluster_v1::getSisterClusterLink() const {
    static const Accessor< ElementLink< xAOD::CaloClusterContainer_v1 > > accSisterCluster("SisterCluster");
    static const ElementLink<xAOD::CaloClusterContainer_v1> empty;
    if (!accSisterCluster.isAvailable(*this))
      return empty;
    return accSisterCluster(*this);
  }

  bool CaloCluster_v1::setSisterClusterLink(const ElementLink<xAOD::CaloClusterContainer_v1>& sister)
  {
    static const Accessor< ElementLink<xAOD::CaloClusterContainer_v1 > > accSisterCluster("SisterCluster");
    accSisterCluster(*this)=sister;
    return true;
  }

} // namespace xAOD
