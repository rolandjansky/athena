/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef CALOEVENT_CALOCLUSTER_H  
#define CALOEVENT_CALOCLUSTER_H
/***************************************************************************
  CaloCluster class
  base class for calorimeter classes
   
Update: Jan 10 2001 HMA
          order of static/virtual/const 

Update:  Redesign.  May 21, 2001 
         Concrete base class for Calorimeter cluster
	 Using CaloSampling for sampling information. 

Update:  Redesign.  Nov 12, 2001  efrank
         Inherit from Navigable to plug into navigation code.
	 Modify addcell() to update token.  I realize now that CaloCluster
	 and NavClusterToken may need to be extend so that a weight can
	 be stored with added cells.

Update:  Redesign. Mar 06, 2003 P Loch
         New navigation system.

Update:  Redesign. Feb 17, 2004 P Loch
         Complete overhaul. Now inherits from CaloEnergyCluster (navigable
         4-vector object with constituents). Non-backward compatible change
         of interfaces.

Update:  Jul 18, 2005 P Loch
         Add basic signal data word and access methods

Update:  Sep 18, 2005 P Loch
         Completely new EDM with external data objects storing the cell links
         and cluster data.
***************************************************************************/
#include "EventKernel/INavigable4Momentum.h"
#include "EventKernel/ISignalState.h"
#include "Navigation/AthenaBarCodeBase.h"

#include "CaloEvent/CaloClusterLinkTemplate.h"
#include "CaloEvent/CaloSamplingHelper.h"

#include "CaloEvent/CaloClusterMoment.h"
#include "CaloEvent/CaloClusterMomentStore.h"

#include "CaloEvent/CaloVariableType.h"
#include "CaloEvent/CaloShower.h"
#include "CaloEvent/CaloShowerContainer.h"

#include "CaloEvent/CaloCompositeKineBase.h"
#include "CaloEvent/CaloCompositeCellBase.h"
#include "CaloEvent/CaloClusterNavigable.h"

#include "CaloEvent/CaloClusterBadChannelData.h"

#include <vector>

#include "GeoPrimitives/GeoPrimitives.h"
#include "CxxUtils/checker_macros.h"


class CaloCell ; 

class CaloCluster;

struct CaloClusterSignalState;


class CaloCluster :  public CaloCompositeKineBase,
                            public CaloCompositeCellBase<CaloClusterNavigable>,
		                        virtual public INavigable4Momentum,
                            public ISignalState,
                            public AthenaBarCodeBase
{

 public:

  /** @brief enums to identify different cluster size
   */
  enum ClusterSize {
    // electrons
    SW_55ele   = 1,
    SW_35ele   = 2,
    SW_37ele   = 3,
    // photons
    SW_55gam   = 4,
    SW_35gam   = 5,
    SW_37gam   = 6,
    // early converted photons
    SW_55Econv = 7,
    SW_35Econv = 8,
    SW_37Econv = 9,
    // soft electrons
    SW_softe   = 10,
    // topological clusters
    Topo_420   = 11,
    Topo_633   = 12,
    // transient cluster for AODCellContainer
    SW_7_11   = 13,
    Unknown    = 99
  };


  //////////////////
  // Public Types //
  //////////////////

  /*! \brief Iterator on \a CaloCell s */
  typedef CaloCompositeCellBase<CaloClusterNavigable>::cell_iterator 
    cell_iterator;

  /*! \brief Sampling indicator */
  typedef CaloSampling::CaloSample                sampling_type;

  /*! \brief Data type indicator */
  typedef CaloVariableType::VariableType          variable_type;

  /*! \brief Variable value type */
  typedef double                                  value_type;


  /*! \brief Store type for cluster moments */
  typedef CaloClusterMomentStore                  moment_store;
  /*! \brief Cluster moment indicator type */
  typedef moment_store::moment_type               moment_type;
  /*! \brief Cluster moment value type */
  typedef moment_store::moment_value              moment_value;
  /*! \brief Cluster moment store iterator type 
   * 
   * \note This type is only used internally, and can easily change
   *       in future implementations. DO NOT USE!
   */
  typedef moment_store::moment_iterator           moment_iterator_i;

  // xAOD compatibility.
  typedef moment_type MomentType;

  typedef double (CaloCluster::*GET_VALUE)()   const;
  typedef void   (CaloCluster::*SET_VALUE)(double v);

  typedef P4SignalState              statename_t;
  typedef P4SignalState::State     signalstate_t;

  //////////////////////////
  // MomentStore Iterator //
  //////////////////////////

  /*! \brief Internal cell iterator */ 
  class MomentStoreIter
  {
  public:

    /*! \brief Default constructor builds unusable iterator */
    MomentStoreIter();
    /*! \brief Standard constructor for a useable iterator */
    MomentStoreIter(const moment_iterator_i& iter,
		    const moment_store* firstStore, 
		    const moment_store* secndStore=0) ;
    /*! Destructor */
    ~MomentStoreIter();

    /*! \brief Advance iterator */
    MomentStoreIter next();

    /*! Step back iterator */
    MomentStoreIter prev() ;
     
    /*! \brief Post-advance operator */
    MomentStoreIter operator++();
    /*! \brief Pre-advance operator */
    MomentStoreIter operator++(int);
    /*! \brief Post-step back operator */    
    MomentStoreIter operator--();
    /*! \brief Pre-step back operator */ 
    MomentStoreIter operator--(int);

    /*! \brief Equality comparator */
    bool operator==(const MomentStoreIter& anOther);
    /*! \brief Inequality comparator */
    bool operator!=(const MomentStoreIter& anOther);

    /*! \brief Operator access to \a CaloClusterMoment */
    const CaloClusterMoment& operator*() const;
    /*! \brief Function access to \a CaloClusterMoment */
    const CaloClusterMoment& getMoment() const;
    /*! \brief Function access to moment type */
    moment_type getMomentType() const;

  private: 
     
    /*! \brief Actual iterator on \a CaloClusterMomentStore*/
    moment_iterator_i m_iter;
    /*! \brief Pointer to first \a CaloClusterMomentStore */
    const moment_store*     m_firstStore;
    /*! \brief Pointer to second \a CaloClusterMomentStore */
    const moment_store*     m_secndStore;
  };

  /*! \brief Moment iterator type for \a CaloCluster clients */
  typedef MomentStoreIter          moment_iterator; 

  ////////////////////////////////
  // Constructors & Destructors //
  ////////////////////////////////

  /*! \brief Constructor */
  CaloCluster(double eta0 = 0, double phi0 = 0,
	      unsigned int varTypePattern = 0x00000000, 
	      unsigned int clusterSize=CaloCluster::SW_55ele);
  /*! \brief Destructor */
  virtual ~CaloCluster();
  /*! \brief Copy constructor */
  CaloCluster(const CaloCluster* pCluster);
  CaloCluster(const CaloCluster& rCluster);

  ///////////////////////////////////////////
  // General Set and Get for all Variables //
  ///////////////////////////////////////////
     
  /*! \brief General sampling variable access */
  double 
  getVariable(const variable_type& varType,
	      const sampling_type& samType,
	      bool                  useLink = true) const;
  
  /*! \brief General access to variables of a given type in all samplings */
  bool
  getVariable(const variable_type& varType,
	      std::vector<double>& varList,
	      bool                 useLink = true) const;
	      
  /*! \brief General sampling variable setter */
  bool
  setVariable(const variable_type& varType, 
	      const sampling_type& samType,
	      const double&        varData,
	      bool                 useLink = false);
  /*! \brief Set variables of a given type in all samplings at once */
  bool
  setVariable(const variable_type&       varType, 
	      const std::vector<double>& varList,
	      bool                       useLink = false);


  /*! \brief Lock variable (protect against future updates) */
  bool
  lockVariable(const variable_type& varType);

  /*! \brief Unlock variable (remove lock) */
  bool
  unlockVariable(const variable_type& varType);

  /*! \brief Check lock status of variable */
  bool isLocked(const variable_type& varType) const;
  bool isLocked(const unsigned int& bitPattern) const;

  /*! \brief Check if all variables are locked */
  bool allLocked() const;

  ////////////////
  // Kinematics //
  ////////////////

  /*! \brief redefine hlv() here to avoid ambiguities */
  virtual  CLHEP::HepLorentzVector hlv() const {return CaloCompositeKineBase::hlv();} 

  /*! \brief retrieve the 4-momentum at a given signal state */
  virtual  CLHEP::HepLorentzVector hlv(signalstate_t s) const;

  /*! \brief Retrieve energy independent of signal state */
  virtual double e() const;
  /*! \brief Retrieve eta independent of signal state */
  virtual double eta() const;
  /*! \brief Retrieve phi independent of signal state */
  virtual double phi() const;
  /*! \brief Retrieve mass independent of signal state */
  virtual double m() const;

  /*! \brief Retrieve energy for a specific signal state */
  double e(signalstate_t s) const;
  /*! \brief Retrieve eta for a specific signal state */
  double eta(signalstate_t s) const;
  /*! \brief Retrieve phi for a specific signal state */
  double phi(signalstate_t s) const;
  /*! \brief Retrieve mass for a specific signal state */
  double m(signalstate_t s) const;

  /*! \brief Set energy */
  virtual void setE(double e);
  /*! \brief Set eta */
  virtual void setEta(double eta);
  /*! \brief Set phi */
  virtual void setPhi(double phi);
  /*! \brief Set mass */
  virtual void setM(double m);

  /*! \brief Set kinematics from four-vector */
  virtual void set4Mom(const I4Momentum* const pMom);
  /*! \brief Set kinematics directly from four-vector */
  virtual void set4Mom(const I4Momentum& rMom);
  /*! \brief Set kinematics directly from HLV */
  virtual void set4Mom(const CLHEP::HepLorentzVector& rVec);

  /*! \brief Retrieve number of samplings in this cluster */
  unsigned int nSamples() const;
  /*! \brief Retrieve energy in a given sampling */
  double eSample(sampling_type sampling) const;
  /*! \brief Retrieve barycenter \f$ \eta \f$ in a given sample */
  double etaSample(sampling_type sampling) const;
  /*! \brief Retrieve barycenter \f$ \varphi \f$ in a given sample */
  double phiSample(sampling_type sampling) const;

  /*! \brief Retrieve energy for all samplings into a vector */
  void getEInSamples(std::vector<double>& theEnergies) const;
  /*! \brief Retrieve \f$ \eta \f$ for all samplings into a vector */
  void getEtaInSamples( std::vector<double>& theEtas)  const;
  /*! \brief Retrieve \f$ \varphi \f$ for all samplings into a vector */
  void getPhiInSamples( std::vector<double>& thePhis)  const;

  /*! \brief Set energy in all samplings from a vector */
  void setEInSamples(const std::vector<double>& theEnergies);
  /*! \brief Set \f$ \eta \f$ in all samplings from a vector */
  void setEtaInSamples(const std::vector<double>& theEtas);
  /*! \brief Set \f$ \varphi \f$ in all samplings from a vector */
  void setPhiInSamples(const std::vector<double>& thePhis);
  /*! \brief Access basic energy scale signal */
  virtual double getBasicEnergy() const;
  /*! \brief Set basic energy scale signal */
  virtual void setBasicEnergy(double theEnergy);  
  /*! \brief Access cluster time */
  double getTime() const;
  /*! \brief Set cluster time */
  void setTime(double theTime);

  /*! \brief Set cluster size */
  void setClusterSize(unsigned int theClusterSize);

  /*! \brief Get cluster size */
  unsigned int getClusterSize() const;

  /* \brief Get eta/phi size from cluster size */
  unsigned int getClusterEtaSize() const;
  unsigned int getClusterPhiSize() const;

  /*! \brief Set energy for a given sampling */
  void setenergy(sampling_type sampling, double e);
  /*! \brief Set \f$ \eta \f$ in a given sampling */
  void seteta   (sampling_type sampling, double eta);
  /*! \brief Set \f$ \varphi \f$ in a given sampling */
  void setphi   (sampling_type sampling, double phi);

  /////////////////////
  // Cluster Moments //
  /////////////////////

  /*! \brief Set individual moment */
  void insertMoment(const moment_type& momType,
                    const moment_value& momValue,
                    bool                useLink=true);
  /*! \brief First iterator on moment store */
  moment_iterator beginMoment(bool useLink=true) const;
  /*! \brief Last iterator on moment store */
  moment_iterator endMoment(bool useLink=true) const;
  /*! \brief Retrieve individual moment */
  bool retrieveMoment(const moment_type& momType,
                      moment_value& momValue,
                      bool useLink=true) const;
  bool retrieveMoment(const moment_type& momType,
                      double&       momData,
                      bool          useLink=true) const; 
  /*! \brief Get individual moment */
  moment_value getMoment(const moment_type& momType,
                         bool useLink=true) const;
  /*! \brief Get individual moment */
  moment_value getMoment(const moment_iterator& momIter) const;

  /*! \brief Get moment value */
  double getMomentValue(const moment_type& momType,
                        bool useLink=true) const;
  double getMomentValue(const moment_iterator& momIter) const;

  /*! \brief Get moment type */
  moment_type getMomentType(const moment_iterator& momIter) const;

  ///////////////////////
  // Special Variables //
  ///////////////////////

  /*! \brief Checks if cells from a given sampling in EMB or EMEC (!) 
   *         are in cluster */
  bool is_valid_sampling(const sampling_type& sampling) const;  
  /*! \brief Checks if cells from a given sampling in EMB and EMEC are in the
   *         cluster. */
  bool isEMSampling(const sampling_type& theSampling) const;
  /*! \brief Checks if certain sampling contributes to cluster */
  bool hasSampling(const sampling_type& theSampling) const; 
  /*! \brief Get sampling bitmask. */
  unsigned int samplingPattern() const;

  /*! \brief Returns raw \f$ \eta \f$ of cluster seed */
  double eta0() const;
  /*! \brief Returns raw \f$ \phi \f$ of cluster seed */
  double phi0() const;

  /*! \brief Returns cluster size in \f$ \eta \f$ for a given sampling */
  double etasize(sampling_type sampling) const;
  /*! \brief Returns cluster size in \f$ \varphi \f$ for a given sampling */
  double phisize(sampling_type sampling) const;
  /*! \brief Set the cluster size in \f$ \eta \f$ for a given sampling */
  void setetasize(sampling_type sampling, double size);
  /*! \brief Set the cluster size in \f$ \varphi \f$ for a given sampling */
  void setphisize(sampling_type sampling, double size) ;

  /*! \brief Retrieve maximum cell energy in given sampling */
  double energy_max(sampling_type sampling) const;
  /*! \brief Retrieve \f$ \eta \f$ of cell with maximum energy in 
   *         given sampling */
  double etamax(sampling_type sampling) const;
  /*! \brief Retrieve \f$ \varphi \f$ of cell with maximum energy in 
   *         given sampling */
  double phimax(sampling_type sampling) const;
  /*! \brief Set the maximum energy in a given sampling */
  void setenergymax(sampling_type sampling, double m) ;
  /*! \brief Set the \f$ \eta \f$ of cell with maximum energy in a 
   *         given sampling */
  void setetamax(sampling_type sampling, double m);
  /*! \brief Set the \f$ \varphi \f$ of cell with maximum energy in a
   *         given sampling */
  void setphimax(sampling_type sampling, double m) ;

  /*! \brief EMB/EMEC combined barycenter \f$ \eta \f$ */
  double etaBE(int sampling) const;
  /*! \brief EMB/EMEC combined barycenter \f$ \varphi \f$ */
  double phiBE(int sampling) const;
  /*! \brief EMB/EMEC combined signal */
  double energyBE(int sampling) const;

  /*! \brief Returns <tt>true</tt> if at least one clustered cell in EMB */
  bool inBarrel() const;
  /*! \brief Returns <tt>true</tt> if at least one clustered cell in EMEC */
  bool inEndcap() const;
  /*! \brief Sets EMB indicator */
  void setBarrel(bool barrel);
  /*! \brief Sets EMEC indicator */
  void setEndcap(bool endcap);

  /*! \brief Add Bad channel information */
  void addBadChannel(const CaloClusterBadChannelData& badChannel);
  /*! \brief Reset Bad channel list */
  void resetBadChannel();

  /*! \brief Get Bad Channel information */
  typedef std::vector<CaloClusterBadChannelData> badChannelList;
  const badChannelList* getBadChannel() const;

  /*! \brief Check if cluster has any bad channel in it */
  bool containsBadCells() const;

  /*! \brief Calculate cluster kinematics from contained cells. */
  void calculateKine(const bool useweight=true, const bool updateLayers=true );

 protected:

  friend class CaloClusterStoreHelper;
  friend struct CaloClusterSignalState;

  typedef 
  CaloClusterLinkTemplate<CaloShowerContainer>::link_type data_link_type;

  /*! \brief Internal error return for real numbers */
  static const double m_errorValue;

  /*!{\ brief Cached Stores */
  CaloSamplingData                    m_dataStore;   /*!< sampling data */
  CaloClusterMomentStore              m_momentStore; /*!< cluster moments */
  /*!}*/

  /*! \brief Temporary stores */
  //  CaloShower* m_dataStorePointer;
  bool m_ownDataStore;

  /*!{ \brief Linked Stores */
  data_link_type m_dataLink; /*!< link to data */ 
  /*!}*/

  /// Non-const pointer to linked store.
  CaloShower* m_shower;

  /*! \brief Stores basic energy signal */
  double m_basicSignal;

  /*! \brief Cluster timing */
  double m_time;

  /*! \brief Sampling pattern */
  unsigned int m_samplingPattern;

  /*! \brief Updates cluster kinematics when cell is added */
  virtual void updateKine(const CaloCell* theCell, double weight);

  double variableBE(const variable_type& theVariable,
                    const int& samplingIndex) const;

  CaloSamplingData* getDataStore(const variable_type& theVariable,
                                 bool useLink=true);
  const CaloSamplingData* getDataStore(const variable_type&
                                       theVariable,
                                       bool useLink=false) const;

  CaloClusterMomentStore* 
  getMomentStore(bool useLink=true);
  const CaloClusterMomentStore* 
  getMomentStore(bool useLink=false) const;
  const CaloClusterMomentStore* 
  getMomentStore(const moment_type& theMoment, bool useLink=false) const;
  CaloClusterMomentStore*
  getMomentStore(const moment_type& theMoment, bool useLink=true);
  bool
  getMomentStorePtrs(const CaloClusterMomentStore*& pFirstStore,
		     const CaloClusterMomentStore*& pSecndStore,
		     bool useLink) const;
    //DEBUG		     const std::string& mode) const;

  /*! \brief Set element link for shower data */
  bool setDataLink(CaloShowerContainer* pDataLink);

  /*! \brief Setup internal store */
  bool setStores(CaloShower* pData,CaloCellLink* pLink,bool ownStores=true);

  /*! \brief Setup data store */
  bool setDataStore(CaloShower* pData,bool ownStores=true);

 private:

  /*! \brief Set variables of a given type in all samplings at once,
             taking locking into account. */
  template <class InputIterator>
  void
  maybeSetVariable(variable_type varType, 
                   InputIterator beg,
                   InputIterator end);

  /*! \brief Flag is <tt>true</tt> if at least one cell in EMB */ 
  bool m_barrel;
  /*! \brief Flag is <tt>true</tt> if at least one cell in EMB */ 
  bool m_endcap;

  /*! \brief Cluster size (e.g. 5x5, 3.7, ...) */
  unsigned int m_clusterSize;

  // ... RAW eta,phi of cluster seed
  /*! \brief Cluster seed \f$ \eta \f$  */ 
  double m_eta0;
  /*! \brief Cluster seed \f$ \varphi \f$  */ 
  double m_phi0;   

  /*! \brief Stores the normalization for time calculation e.g. 
   *         sum(w * |w| * E^2) */
  double m_timeNorm;

  /*! \brief Stores the normalization e.g. sum(|E|) for the position
   *         calculations. */
  double m_posNorm;

  /*! \brief Stores the normalization e.g. sum(|E|) for the position
   *         calculations in each sampling. */
  std::vector<double> m_posSamNorm;

  /*! \brief Counter for number of barrel cells with non-zero weight
   *         and energy */
  int m_nBarrel;
  /*! \brief Counter for number of endcap cells with non-zero weight
    and energy */
  int m_nEndcap;
  /*! \brief Variable lock pattern */
  unsigned int m_lockPattern;

  badChannelList m_badChannelData;

  ///////////////////
  // Signal States //
  ///////////////////
  
 public:
  
  /*! \brief Retrieve signal state */
  signalstate_t signalState() const;
  /*! \brief Retrieve default signal state */
  signalstate_t defaultSignalState() const;
  
  /*! \brief check if signal state exists for current implementation*/
  virtual bool hasSignalState(signalstate_t s) const;
  /*! \brief check if we are at the passed state */
  virtual bool isAtSignalState(signalstate_t s) const;

 protected:

  /*! \brief Sets signal state */
  virtual bool setSignalState(signalstate_t s);

  /*! \brief reset the signal state */
  virtual void resetSignalState();
  
  /*! \brief Sets default signal state */
  bool setDefaultSignalState(signalstate_t s);

 private:

  /*! \brief Access to raw energy */
  double getRawE()   const;
  /*! \brief Access to raw eta */
  double getRawEta() const;
  /*! \brief Access to raw phi */
  double getRawPhi() const;
  /*! \brief Access to raw mass */
  double getRawM()   const;

  /*! \brief Access to calibrated (LC) energy */
  double getCalE()   const;
  /*! \brief Access to calibrated (LC) eta */
  double getCalEta() const;
  /*! \brief Access to calibrated (LC) phi */
  double getCalPhi() const;
  /*! \brief Access to calibrated (LC) m */
  double getCalM()   const;

  /*! \brief Access to calibrated (cell weight) energy */
  double getAltE()   const;
  /*! \brief Access to calibrated (cell weight) eta */
  double getAltEta() const;
  /*! \brief Access to calibrated (cell weight) phi */
  double getAltPhi() const;
  /*! \brief Access to calibrated (cell weight) m */
  double getAltM()   const;

  /*! \brief Set raw energy */
  void setRawE(double e);
  /*! \brief Set raw eta */
  void setRawEta(double eta);
  /*! \brief Set raw phi */
  void setRawPhi(double phi);
  /*! \brief Set raw m */
  void setRawM(double m);

  /*! \brief Set calibrated (LC) energy */
  void setCalE(double e);
  /*! \brief Set calibrated (LC) eta */
  void setCalEta(double eta);
  /*! \brief Set calibrated (LC) phi */
  void setCalPhi(double phi);
  /*! \brief Set calibrated (LC) m */
  void setCalM(double m);

  /*! \brief Set calibrated (cell weight) energy */
  void setAltE(double e);
  /*! \brief Set calibrated (cell weight) eta */
  void setAltEta(double eta);
  /*! \brief Set calibrated (cell weight) phi */
  void setAltPhi(double phi);
  /*! \brief Set calibrated (cell weight) m */
  void setAltM(double m);

  // Cint has trouble with the types of these members.
  // Just hide them from reflex for now.
#ifndef __REFLEX__
  /*! \brief Pointer to getter functions */
  GET_VALUE m_getE;
  /*! \brief Pointer to getter functions */
  GET_VALUE m_getEta;
  /*! \brief Pointer to getter functions */
  GET_VALUE m_getPhi;
  /*! \brief Pointer to getter functions */
  GET_VALUE m_getM;

  /*! \brief Pointer to setter functions */
  SET_VALUE m_setE;
  /*! \brief Pointer to setter functions */
  SET_VALUE m_setEta;
  /*! \brief Pointer to setter functions */
  SET_VALUE m_setPhi;
  /*! \brief Pointer to setter functions */
  SET_VALUE m_setM;
#endif

  /*! \brief Stores actual signal state */
  signalstate_t m_signalState;
  /*! \brief Stores default signal state */
  signalstate_t m_defSigState;

  /*! \brief Stores raw signal */
  double m_rawE;
  /*! \brief Stores raw signal */
  double m_rawEta;
  /*! \brief Stores raw signal */
  double m_rawPhi;
  /*! \brief Stores raw signal */
  double m_rawM;

  /*! \brief Stores calibrated (cell weight) signal */
  double m_altE;
  /*! \brief Stores calibrated (cell weight) signal */
  double m_altEta;
  /*! \brief Stores calibrated (cell weight) signal */
  double m_altPhi;
  /*! \brief Stores calibrated (cell weight) signal */
  double m_altM;

  /*! \brief Helper to switch to raw state */
  bool setStateRaw();

  /*! \brief Helper to switch to calibrated (LC) state */
  bool setStateCal();

  /*! \brief Helper to switch to calibrated (cell weight) state */
  bool setStateAlt();

public:
  // For interactive debugging from python --- 
  // should not be used from client code.
  data_link_type getMomentStoreLink () const
  { return m_dataLink; }

private:
  /// Disallow (avoid coverity warning).
  CaloCluster& operator= (const CaloCluster&);

  friend class CaloClusterContainerCnv_p1;
  friend class CaloClusterContainerCnv_p2;
  friend class CaloClusterContainerCnv_p3;
  friend class CaloClusterContainerCnv_p4;
  friend class CaloClusterContainerCnv_p5;
  friend class CaloClusterContainerCnv_p6;
  friend class CaloClusterContainerCnv_p7;
  friend class CaloClusterContainerCnvTest_p6;
  friend class CaloClusterContainerCnvTest_p7;

};

///////////////////////////////////////////////////////////////////////////////
//                           Inline Funtions                                 //
///////////////////////////////////////////////////////////////////////////////

////////////////
// Kinematics //
////////////////

#ifndef __REFLEX__
inline double CaloCluster::e()   const 
{ return (this->*m_getE)(); }
inline double CaloCluster::eta() const
{
  return (this->*m_getEta)(); 
}
inline double CaloCluster::phi() const
{
  return (this->*m_getPhi)(); 
}

inline double CaloCluster::m()   const
{ return (this->*m_getM)(); }

inline void CaloCluster::setE(double e) 
{ (this->*m_setE)(e); }
inline void CaloCluster::setEta(double eta)
{ (this->*m_setEta)(eta); }
inline void CaloCluster::setPhi(double phi)
{ (this->*m_setPhi)(phi); }
inline void CaloCluster::setM(double m)
{ (this->*m_setM)(m); }  
#endif

inline double CaloCluster::e(signalstate_t s)   const 
{
  if (!hasSignalState(s)) return m_errorValue;
  switch (s) {
  case statename_t::CALIBRATED:
    return P4EEtaPhiM::e();
  case statename_t::UNCALIBRATED:
    return m_rawE;
  case statename_t::ALTCALIBRATED:
    return m_altE;
  default:
    return m_errorValue;
  }
}


inline double CaloCluster::eta(signalstate_t s) const
{
  if (!hasSignalState(s)) return m_errorValue;
  switch (s) {
  case statename_t::CALIBRATED:
    return P4EEtaPhiM::eta();
  case statename_t::UNCALIBRATED:
    return m_rawEta;
  case statename_t::ALTCALIBRATED:
    return m_altEta;
  default:
    return m_errorValue;
  }
}


inline double CaloCluster::phi(signalstate_t s) const
{
  if (!hasSignalState(s)) return m_errorValue;
  switch (s) {
  case statename_t::CALIBRATED:
    return P4EEtaPhiM::phi();
  case statename_t::UNCALIBRATED:
    return m_rawPhi;
  case statename_t::ALTCALIBRATED:
    return m_altPhi;
  default:
    return m_errorValue;
  }
}


inline double CaloCluster::m(signalstate_t s)   const
{
  if (!hasSignalState(s)) return m_errorValue;
  switch (s) {
  case statename_t::CALIBRATED:
    return P4EEtaPhiM::m();
  case statename_t::UNCALIBRATED:
    return m_rawM;
  case statename_t::ALTCALIBRATED:
    return m_altM;
  default:
    return m_errorValue;
  }
}


inline void CaloCluster::set4Mom(const I4Momentum* const pMom)
{
  this->setE(pMom->e());
  this->setEta(pMom->eta());
  this->setPhi(pMom->phi());
  this->setM(pMom->m());
}

inline void CaloCluster::set4Mom(const I4Momentum& rMom)
{ this->set4Mom(&rMom); }

inline void CaloCluster::set4Mom(const CLHEP::HepLorentzVector& rVec)
{
  this->setE(rVec.e());
  this->setEta(rVec.eta());
  this->setPhi(rVec.phi());
  this->setM(rVec.m());
}

inline CaloCluster::signalstate_t CaloCluster::signalState() const
{ return m_signalState; }
inline CaloCluster::signalstate_t CaloCluster::defaultSignalState() const
{ return m_defSigState; }

inline bool CaloCluster::setDefaultSignalState(signalstate_t s) 
{
  m_defSigState = s;
  return this->setSignalState(s);
}

///////////////////////////
// Basic Signal and Time //
///////////////////////////

inline double 
CaloCluster::getBasicEnergy() const { return m_basicSignal; }
/*!
 *  \param theEnergy basic energy signal
 *
 *  Overwrites default behaviour in \a CaloEnergyCluster base class
 */
inline void 
CaloCluster::setBasicEnergy(double theEnergy) { m_basicSignal = theEnergy; }

inline double
CaloCluster::getTime() const { return m_time; }

inline void
CaloCluster::setTime(double theTime) { m_time = theTime; }

inline void
CaloCluster::setClusterSize(unsigned int theClusterSize) { m_clusterSize = theClusterSize; }

inline unsigned int
CaloCluster::getClusterSize() const { return m_clusterSize; }

///////////////////
// Configuration //
///////////////////

inline bool
CaloCluster::setStores(CaloShower* pData, CaloCellLink* pLink, bool ownStores)
{
  return ( this->setDataStore(pData,ownStores) &&  
	   this->setLinkStore(pLink,ownStores) );
}

inline bool
CaloCluster::setDataStore(CaloShower* pData,bool ownStores) 
{ 
  //  m_dataStorePointer = pData;
  m_ownDataStore = ownStores;
  m_dataLink.setElement(pData);
  m_shower = pData;
  return m_dataLink.isValid(); 
} 

//////////
// Tags //
//////////

/*!
 * Returns <tt>true</tt> if cluster has more than one cell with positive 
 * signal in barrel calorimeter.
 */
inline bool CaloCluster::inBarrel() const { return m_barrel; }
/*!
 * Returns <tt>true</tt> if cluster has more than one cell with 
 * positive signal in endcap calorimeter.
 */
inline bool CaloCluster::inEndcap() const { return m_endcap; }
/*!
 *  \param barrel input flag is <tt>true</tt> if at least one cell in EMB
 */
inline void CaloCluster::setBarrel(bool barrel) { m_barrel = barrel; }
/*!
 *  \param endcap input flag is <tt>true</tt> if at least one cell in EMEC
 */
inline void CaloCluster::setEndcap(bool endcap) { m_endcap = endcap; }
/*!
 *  \param sampling calorimeter sampling indicator
 *
 *  \note The original implementation only check on cells in the 
 *        electromagnetic calorimeters. This method is depreciated,
 *        use \a isEMSampling(sampling) instead.
 */
inline bool 
CaloCluster::is_valid_sampling(const sampling_type& sampling) const
{ return this->isEMSampling(sampling); }

inline bool CaloCluster::isEMSampling(const sampling_type& theSampling) const
{ return CaloSamplingHelper::isEMSampling(theSampling); }

inline bool CaloCluster::hasSampling(const sampling_type& theSampling) const
{
  unsigned int bitPattern = CaloSamplingHelper::getSamplingBit(theSampling);
  return (m_samplingPattern & bitPattern) == bitPattern;
}

/////////////////////////////////////
// Get Energy/Eta/Phi in Samplings //
/////////////////////////////////////

/*!
 * Returns the maximum possible number of samplings, not the actual number
 * contributing to the cluster!
 */
inline unsigned int CaloCluster::nSamples() const 
{ return CaloSampling::getNumberOfSamplings(); }
/*!
 *  \param theSampling sampling indicator
 *
 *   Returns the energy in a given sampling. 
 */
inline double CaloCluster::eSample(sampling_type theSampling) const
{ return this->getVariable(CaloVariableType::ENERGY,theSampling,true); }
/*!
 *  \param theEnergies  reference to a modifiable vector 
 *
 *  \warning The vector referenced on input will be completely overwritten
 *           and resized.
 */
inline void CaloCluster::getEInSamples(std::vector<double>& theEnergies) const
{ this->getVariable(CaloVariableType::ENERGY,theEnergies,true); }

/*! 
 *  \param theSampling sampling indicator
 *
 *  Returns \f$ \eta \f$ in sampling. 
 */
inline double CaloCluster::etaSample(sampling_type theSampling) const
{ return this->getVariable(CaloVariableType::ETA,theSampling,true); }
/*!
 *  \param theEtas  reference to a modifiable vector 
 *
 *  \warning The vector referenced on input will be completely overwritten
 *           and resized.
 */
inline void CaloCluster::getEtaInSamples(std::vector<double>& theEtas) const
{ this->getVariable(CaloVariableType::ETA,theEtas,true); }
/*!
 *  \param theSampling sampling indicator
 *
 *  Returns \f$ \varphi \f$ in sampling.
 */
inline double CaloCluster::phiSample(sampling_type theSampling) const
{ return this->getVariable(CaloVariableType::PHI,theSampling,true); }
/*!
 *
 *  \param thePhis  reference to a modifiable vector 
 *
 *  \warning The vector referenced on input will be completely overwritten
 *           and resized.
 */
inline void CaloCluster::getPhiInSamples(std::vector<double>& thePhis) const
{ this->getVariable(CaloVariableType::PHI,thePhis,true); }

////////////////////////////////////////
// Set Methods for Sampling Kinematic //
////////////////////////////////////////

/*!
 *  \param theSampling  sampling indicator
 *  \param theEnergy    energy data
 *  
 *  Depreciated. Sets the sampling energy without updating the global 
 *  kinematics. 
 */
inline void CaloCluster::setenergy(sampling_type theSampling, double theEnergy)
{ this->setVariable(CaloVariableType::ENERGY,theSampling,theEnergy,true); }
/*!
 *  \param theEnergies  reference to a vector holding the energies in 
 *                      samplings (input unchanged)
 *
 *  \note The \a CaloSampling::CaloSample indicator is interpreted as
 *        an (unsigned) \a int internally. The input vector should be
 *        set up such that the index to an energy value is understood to be
 *        the \a CaloSampling::CaloSample of the corresponding calorimeter 
 *        sampling.
 */
inline void CaloCluster::setEInSamples(const std::vector<double>& theEnergies)
{ this->setVariable(CaloVariableType::ENERGY,theEnergies,true); }

/*!
 *  \param sampling calorimeter sampling indicator
 *  \param eta      \f$ \eta \f$ value to be set
 */
inline void CaloCluster::seteta(sampling_type sampling, double eta) 
{ this->setVariable(CaloVariableType::ETA,sampling,eta,true); }
/*!
 *  \param theEtas  reference to a vector holding the\f$ \eta \f$ in 
 *                  samplings (input unchanged)
 *
 *  \note The \a CaloSampling::CaloSample indicator is interpreted as
 *        an (unsigned) \a int internally. The input vector should be
 *        set up such that the index to a \f$ \eta \f$ 
 *        value is understood to be
 *        the \a CaloSampling::CaloSample of the corresponding calorimeter 
 *        sampling.
 */
inline void CaloCluster::setEtaInSamples(const std::vector<double>& theEtas)
{ this->setVariable(CaloVariableType::ETA,theEtas,true); }

/*!
 *  \param sampling calorimeter sampling indicator
 *  \param phi      \f$ \varphi \f$ value to be set
 */
inline void CaloCluster::setphi(sampling_type sampling, double phi) 
{ this->setVariable(CaloVariableType::PHI,sampling,phi,true); }
/*!
 *  \param thePhis  reference to a vector holding the\f$ \eta \f$ in 
 *                  samplings (input unchanged)
 *
 *  \note The \a CaloSampling::CaloSample indicator is interpreted as
 *        an (unsigned) \a int internally. The input vector should be
 *        set up such that the index to a \f$ \varphi \f$ value is 
 *        understood to be
 *        the \a CaloSampling::CaloSample of the corresponding calorimeter 
 *        sampling.
 */
inline void CaloCluster::setPhiInSamples(const std::vector<double>& thePhis)
{ this->setVariable(CaloVariableType::PHI,thePhis,true); }

///////////////////////////////////////////
// Energy/eta/phi of Maximum Signal Cell //
///////////////////////////////////////////
/*!
 * \param sampling sampling indicator
 *
 * Returns energy of cell with maximum signal in sampling.
 *
 * \warning
 * Depreciated. This method should not be used anymore. Use
 * \a getVariable(CaloVariableType::MAX_ENERGY,...) instead.
 */ 
inline double CaloCluster::energy_max(sampling_type sampling) const
{
  return this->isEMSampling(sampling)
    ? this->getVariable(CaloVariableType::MAX_ENERGY,sampling,true)
    : double(0);
}

/*!
 * \param sampling sampling indicator
 *
 * Returns \f$ \eta \f$ of cell with maximum signal in sampling.
 *
 * \warning
 * Depreciated. This method should not be used anymore. Use
 * \a getVariable(CaloVariableType::MAX_ETA,...) instead.
 */ 
inline double CaloCluster::etamax(sampling_type sampling) const
{
  return this->isEMSampling(sampling)
    ? this->getVariable(CaloVariableType::MAX_ETA,sampling,true)
    : double(0);
}

/*!
 * \param sampling sampling indicator
 *
 * Returns \f$ \varphi \f$ of cell with maximum signal in sampling.
 *
 * \warning
 * Depreciated. This method should not be used anymore. Use
 * \a getVariable(CaloVariableType::MAX_PHI,...) instead.
 */ 
inline double CaloCluster::phimax(sampling_type sampling) const
{
  return this->isEMSampling(sampling)
    ? this->getVariable(CaloVariableType::MAX_PHI,sampling,true)
    : double(0);
}

////////////////////////////////
// Cluster Sizes in Samplings //
////////////////////////////////

/*! 
 * \param sampling sampling indicator
 *
 * Returns eta size \f$ \Delta\eta \f$ of cluster in sampling.
 *
 * \warning
 * Depreciated. This method should not be used anymore. Use
 * \a getVariable(CaloVariableType::DELTA_ETA,...) instead.
 */ 
inline double CaloCluster::etasize(sampling_type sampling) const
{
  return this->isEMSampling(sampling)
    ? this->getVariable(CaloVariableType::DELTA_ETA,sampling,true)
    : double(0);
}

/*! 
 * 
 * \param sampling sampling indicator
 *
 * Returns phi size \f$ \Delta\varphi \f$ of cluster in sampling.
 *
 * \warning
 * Depreciated. This method should not be used anymore. Use
 * \a getVariable(CaloVariableType::DELTA_PHI,...) instead.
 */ 
inline double CaloCluster::phisize(sampling_type sampling) const
{
  return this->isEMSampling(sampling)
    ? this->getVariable(CaloVariableType::DELTA_PHI,sampling,true)
    : double(0);
}

///////////
// Seeds //
///////////

/*! 
 * 
 * Returns seed \f$ \eta \f$ for cluster.
 */
inline double CaloCluster::eta0() const { return m_eta0; }

/*!
 *
 * Returns seed \f$ \varphi \f$ for cluster.
 */
inline double CaloCluster::phi0() const { return m_phi0; }

////////////////////////////////////
// Kinematics in Barrel/EndCap EM //
////////////////////////////////////
/*!
 *  \param  sam sampling counter
 *
 *  \warning The \a sam input argument refers to the sampling number
 *           (first, second, third,...) in the EMB and EMEC. It is not
 *           identical to the corresponding \a CaloSampling::CaloSample 
 *           indicators in these calorimeters.
 */ 
inline double CaloCluster::etaBE(int sam) const
{ return this->variableBE(CaloVariableType::ETA,sam); }
/*!
 *  \param  sam sampling counter
 *
 *  \warning The \a sampling input argument refers to the sampling number
 *           (first, second, third,...) in the EMB and EMEC. It is not
 *           identical to the corresponding \a CaloSampling::CaloSample 
 *           indicators in these calorimeters.
 */ 
inline double CaloCluster::phiBE(int sam) const
{ return this->variableBE(CaloVariableType::PHI,sam); }
/*!
 *  \param  sam sampling counter
 *
 *  \warning The \a sampling input argument refers to the sampling number
 *           (first, second, third,...) in the EMB and EMEC. It is not
 *           identical to the corresponding \a CaloSampling::CaloSample 
 *           indicators in these calorimeters.
 */ 
inline double CaloCluster::energyBE(int sam) const
{ return this->variableBE(CaloVariableType::ENERGY,sam); }

/////////////
// Moments //
/////////////

inline CaloClusterMoment 
CaloCluster::getMoment(const moment_type& momType,bool useLink) const {  
  CaloClusterMoment value(0.0);
  if (!this->retrieveMoment(momType,value,useLink))
    value = 0; // Make coverity happy.
  return value;
}


inline CaloClusterMoment
CaloCluster::getMoment(const moment_iterator& rMomIter) const
{ return rMomIter.getMoment(); }

inline double
CaloCluster::getMomentValue(const moment_type& momType,bool useLink) const {
  double value(0.0);
  if (!this->retrieveMoment(momType,value,useLink))
    value = 0; // Make coverity happy.
  return value;
}



inline double
CaloCluster::getMomentValue(const moment_iterator& momIter) const
{ return (momIter.getMoment()).getValue(); }

inline CaloClusterMoment::MomentType
CaloCluster::getMomentType(const moment_iterator& rMomIter) const
{ return rMomIter.getMomentType(); }

/////////////////////
// Internal Stores //
/////////////////////

// set data link store
inline bool CaloCluster::setDataLink(CaloShowerContainer* pDataLink)
{
  if (!pDataLink) 
    return false;
  CaloShower* pData = m_shower;
  m_ownDataStore = !pDataLink->ownElements();
  return CaloClusterLinkTemplate<CaloShowerContainer>::setLink(pDataLink,
							       pData,
							       m_dataLink);
}

//////////////////////
// Variable Locking //
//////////////////////

// check if variable is locked
inline bool CaloCluster::isLocked(const variable_type& varType) const
{
  unsigned int bitPattern(CaloVariableType::getVariableBit(varType));
  return this->isLocked(bitPattern); 
}
inline bool CaloCluster::isLocked(const unsigned int& bitPattern) const
{
  return ( m_lockPattern & bitPattern ) == bitPattern;
}

// check if all variables are locked
inline bool CaloCluster::allLocked() const
{
  return 
    ( m_lockPattern & CaloVariableType::getAllVariableBits() ) ==
    CaloVariableType::getAllVariableBits();
}

/////////////////////////////////////////////
//  containsBadCell
/////////////////////////////////////////////

inline bool CaloCluster::containsBadCells() const {
  return !(this->getBadChannel()->empty());
}

// internal functions


// Override the default hash function for NavigationToken so that
// we'll get reproducible ordering.
inline
std::size_t navigationHash (const CaloCluster* p)
{
  return static_cast<std::size_t> (static_cast<long long>(p->energy_nonvirt()));
}


template <class InputIterator>
void CaloCluster::maybeSetVariable (variable_type varType,
                                    InputIterator beg,
                                    InputIterator end)
{
  if (this->isLocked (varType)) return;
  CaloSamplingData* p = this->getDataStore (varType, true);
  if (p)
    p->storeData (varType, beg, end);
}



// check if variable is locked
/*! \class CaloCluster
 *
 *  \brief Principal data class for \a CaloCell clusters. 
 *
 *  \brief Data class for CaloCell clusters
 *
 *  \author  H. Ma <hma@bnl.gov> (original author)
 *  \author  P. Loch <loch@physics.arizona.edu>
 *  \author  S. Menke <Sven.Menke@cern.ch>
 *  \author  M. Boonekamp <Maarten.Boonekamp@cern.ch>
 *  \date    April 1, 2005
 *  \date    July 18, 2005 significant updates in cluster moment handling
 *  \date    September 23, 2005 complete change of underlying store structure
 *  \date    May 1, 2009 third signal state added (PL)
 *  \version 6
 *
 *  \a CaloCluster stores a list of \a CaloCell s with some correlated signal.
 *  In all generality this object describes an energy blob inside the 
 *  calorimeter, with its location completely defined by the cell content.
 *  In particular, there is no assumption on any detector view (regular 
 *  binning in a given coordinate system or similar) needed or used for
 *  the cluster description (unlike for \a CaloTower , for example). In
 *  general \a CaloCluster behaves like a navigable object with the
 *  default 4-momentum representation.
 *
 *  \remarks
 *  The links to \a CaloCell objects are now stored externally in a 
 *  \a CaloCellLink object. Also, a configurable set of sampling vaiables
 *  can be selected and stored directly in the cluster. The rest will 
 *  automatically be stored in a associated \a CaloSamplingData object, 
 *  which is linked to the cluster via a \a CaloShower object. Cluster
 *  moments are also kept in a split store following the same philosophy.
 *  All these features are completely transparent to clients of \a CaloCluster.
 *
 *  \todo 
 *    
 */

/*! \class CaloCluster::MomentStoreIter
 *
 * Principle iterator of the (split) moment store. Only for read access 
 * presently. Automatically loops cached and external moment store, the latter
 * only if available. The iterator range is as follows:
 * - external store available
 *        - \c cell_begin() = \c localStore.begin()
 *        - \c cell_end()   = \c externalStore.end()
 * - external store not available
 *        - \c cell_begin() = \c localStore.begin()
 *        - \c cell_end()   = \c localStore.end()
  */
#endif // CALOCLUSTER_H
 
