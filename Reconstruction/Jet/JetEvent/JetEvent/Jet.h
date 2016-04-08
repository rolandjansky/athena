// emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JET_H
#define JETEVENT_JET_H

// navigation
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "Navigation/Navigable.h"

// four-momentum
#include "FourMom/Lib/P4ImplPxPyPzE.h"

// signal state
#include "EventKernel/ISignalState.h"

// particle character
#include "ParticleEvent/ParticleSigStateImpl.h"

#include "JetEvent/JetKeyDescriptor.h"
#include "JetEvent/JetAssociationBase.h"
#include "JetEvent/JetTagInfoBase.h"
#include "JetEvent/JetMoment.h"
//#include "JetEvent/JetMomentMap.h"

#include <boost/function.hpp>
#include <vector>

/////////////////////////
// Signal State Friend //
/////////////////////////

class JetCollection;
class JetMomentMap;

struct JetSignalState;

namespace fastjet {
  class PseudoJet;
}

class Jet : public ParticleSigStateImpl<Navigable<INavigable4MomentumCollection,double>,
	    P4ImplPxPyPzE>
{
  // make the templated converter base friend
  template <class PERS>
  friend class JetConverterBase;

  // make the JetCollection a friend
  friend class JetCollection;


  ///////////////
  // Jet Types //
  ///////////////
  
protected:

  /** @brief Internally used base type mixes particle/navigable/four-momentum
             interfaces with correct implementation
   */
  
private:
  
  typedef 
  ParticleSigStateImpl<Navigable<INavigable4MomentumCollection,double>,P4ImplPxPyPzE>
  base_t;
  
  /** @brief Navigable implementation type          */
  typedef base_t::navigable_type                                 navigable_t;
  /** @brief Four-momentum implementation type      */
  typedef base_t::momentum_type                                   momentum_t;
  /** @brief Patricle implementation type           */ 
  typedef base_t::particle_type                                   particle_t;

public:
  /** @brief Navigable object collection type (obsolete)
      @warning Depreciated type, do not use anymore! Will be removed 
      soon!
   */
  typedef INavigable4MomentumCollection                     collection_type;
  /** @brief Navigable object type  (obsolete)
      @warning Depreciated type, do not use anymore! Will be removed 
      soon! 
   */
  typedef INavigable4Momentum                              constituent_type;
  /** @brief Constituent iterator type (obsolete)
      @warning Depreciated type, do not use anymore! Will be removed 
      soon! 
   */
  typedef navigable_t::object_iter                      constituent_iterator;
  /** @brief Constituent index type (obsolete) 
      @warning Depreciated type, do not use anymore! Will be removed 
      soon! 
   */
  typedef navigable_t::external_index_type                        index_type;

  /**< @brief Navigable object collection type */
  typedef INavigable4MomentumCollection                         collection_t;  
  /**< @brief Navigable object type */
  typedef INavigable4Momentum                                  constituent_t;
  /**< @brief Constituent iterator type */
  typedef navigable_t::object_iter                            const_iterator;
  typedef navigable_t::external_index_type                           index_t;
  typedef navigable_t::constituent_coll                   constituent_coll_t;

  typedef size_t                                                numconstit_t;

  // types for categories and keys 
  typedef JetKeyDescriptor::key_t                                     mkey_t;
  typedef JetKeyDescriptor::category_t                                mcat_t;

 
  // types for jet shape (moment) information
  typedef JetMoment::float_t                                         shape_t;
  typedef JetMomentMap                                           shape_map_t;


  // types for associations
  typedef JetAssociationBase                                         assoc_t;
  typedef std::vector<const assoc_t*>                            assostore_t;
  
  // types for tag info
  typedef JetTagInfoBase                                           taginfo_t;
  typedef std::vector<const taginfo_t*>                           tagstore_t;
  
  // types for kinematics
  typedef CLHEP::HepLorentzVector                                             hlv_t;
  typedef I4Momentum                                               fourmom_t;
  typedef P4SignalState::State                                 signalstate_t;
  typedef P4SignalState                                          statename_t;
  
  typedef double (Jet::*GET_VALUE)() const;
  typedef void   (Jet::*SET_VALUE)(double v);
  
  ///////////////////////////////
  // Enumeration               //
  ///////////////////////////////
  /** @ brief fine control over what's copied in the various stores and constituents */
  enum CopyDataMembers { CopyNone = 0x0,
			 CopyMoments = 0x1,
			 CopyTagInfos = 0x2,
			 CopyAssociations = 0x4,
			 CopyConstituents = 0x8,
			 CopyAll = 0xF };
  
  ///////////////////////////////
  // Constructors & Destructor //
  ///////////////////////////////
  
  /** @brief Default constructor */
  Jet();
  /** @brief Construction from storable object */
  Jet(const collection_t* pColl,const constituent_t* pConst,double wght=1.0);
  /** @brief Construction from storable object */
  Jet(const collection_t* pColl,index_t ind,double wght=1.0);
  /** @brief Construction from @c I4Momentum type */
  Jet(const fourmom_t* pMom);
  /** @brief Construction from @c I4Momentum type */
  Jet(const fourmom_t& rMom);
  /** @brief Construction from @c CLHEP::HepLorentzVector type */
  Jet(const hlv_t& rVec); 
  /** @brief Construction from data */
  Jet(double px, double py, double pz, double e);
  /** @brief Copy constructor */ 
  Jet(const Jet* pJet ); 
  /** @brief Copy constructor, controlling copies of internals */ 
  Jet(const Jet* pJet , bool copyStores, bool copyConstituents); 
  /** @brief Copy constructor using reference */
  Jet(const Jet& rJet);

  /** @brief constructor, with finer controlling of copies of internals,
      two conventions possible, either do c-style bool | bool [ | bool ]
      or give [1-4] enums.
   */ 
  Jet(const Jet* pJet,
      CopyDataMembers cdm0, CopyDataMembers cdm1 = CopyNone,
      CopyDataMembers cdm2 = CopyNone, CopyDataMembers cdm3 = CopyNone); 
  
  /// clone method : do not clone stores and constituent
  virtual Jet* clone() const ; 
  /// clone method : do not clone stores and constituent
  virtual Jet* clone(bool copyStores, bool copyConstituents = true ) const ; 




  /** @brief Destructor */
  virtual ~Jet(); 

protected:

  void setup(const hlv_t& v);
  void copy_from_jet( const Jet* pjet, CopyDataMembers cdm0, CopyDataMembers cdm1 = CopyNone,
		      CopyDataMembers cdm2 = CopyNone, CopyDataMembers cdm3 = CopyNone);


  const shape_map_t* getMomentMap(bool addIfMissing) const;

public:

  const shape_map_t* getMomentMap() const;
  std::string getMomentMapName() const;
  

  virtual void setupKine(const hlv_t& v);

  ////////////////////////////////
  // Kinematic with SignalState //
  ////////////////////////////////

public:
  
  // Signal State functions already implemented generically in ParticleSigStateImpl
  
  /** @brief Kinematics are now handled by the base class, except for the following */
  virtual void setE (double e );
  virtual void setPx(double Px);
  virtual void setPy(double py);
  virtual void setPz(double pz);

  virtual void setE (double e , signalstate_t s);
  virtual void setPx(double Px, signalstate_t s);
  virtual void setPy(double py, signalstate_t s);
  virtual void setPz(double pz, signalstate_t s);
  
  virtual double ei()  const; 
  virtual double pxi() const;
  virtual double pyi() const;
  virtual double pzi() const;
  virtual double pti() const;

  /// compatibility with analysis EDM
  void setPxPyPzE(double px, double py, double pz, double e);
  void setPtEtaPhiE( double a, double b, double c, double d );
  void setPtEtaPhiM( double a, double b, double c, double d );
  void setEEtaPhiM( double a, double b, double c, double d );
  // void setPhi(double phi);
  // void setEta(double phi); NOT provided on purpose to avoid potential confusion with PtEtaPhiE/M based implementations.

  /** @brief @b (obsolete) set all 4-mom from CLHEP CLHEP::HepLorentzVector */
  void setP(const hlv_t& rVec);

  /** @brief Sum of weighted constituent four-momentums */
  hlv_t constituent_sum4Mom() const;
  
  /** @brief Sum of weighted constituent four-momentums, with SignalState */
  hlv_t constituent_sum4Mom(signalstate_t s) const;

  /** @brief scale the jet 4mom by scale_factor */
  void scale_momentum(double scale_factor);

  /** @brief return a NEW copy of this jet with 4mom scaled */
  Jet * get_scaled_copy(double scale_factor) const;
    
  ////////////////////////////
  // Constituent Management //
  ////////////////////////////

public:

  /** @brief Add a constituent with optional kinematic weight */
  bool addConstituent(const collection_t*  pColl,
		      const constituent_t* pConst, 
		      double              wght=1.0); 
  /** @brief Add a constituent with optional kinematic weight */
  bool addConstituent(const collection_t*  pColl,index_t ind,double wght=1.0);
  /** @brief Add a constituent with optional kinematic weight
      Also allows passing in a size hint, and disabling recalculation
      of kinematics. */
  bool addConstituent(const collection_t*  pColl,
                      index_t ind,
                      size_t sizeHint,
                      bool noKine,
                      double wght=1.0);
  /** @brief Remove a constituent */
  bool removeConstituent(const constituent_t* pConst);
  /** @brief Remove a constituent */
  bool removeConstituent(const collection_t* pColl,index_t ind);
  /** @brief Remove all constituents */
  bool removeConstituent(bool reset=false);

  /** @brief Add a jet */
  bool addJet(const Jet* pJet,double wght=1.0);

  /** @brief Change kinematic weight of constituent */
  bool reweight(const constituent_t* pConst,double wght);
  /** @brief Change kinematic weight of constituent */
  bool reweight(const collection_t* pColl,index_t ind,double wght);

  /** @brief Remove all constituents with kinematic weight = 0 */
  bool cleanUp();

  /** Number of constituents in jet. Use this rather than size()*/
  numconstit_t constituentsN()const {return m_constituentsN;}

  /** Set number of constituents in jet.

      this just sets a number, no actual effect on constituents. 
      Used when constituents are physically removed when jet is saved on disk.*/
  void setConstituentsN(numconstit_t n){m_constituentsN = n;}

protected:
  
  numconstit_t m_constituentsN;

  /** @brief Add element with checking */
  void putElement(const collection_t*  pColl,
		  const constituent_t* pConst,
		  double               wght = 1.0,
                  size_t               sizeHint = 0);
  /** @brief Add element with checking */
  void putElement(const collection_t*  pColl,
		  index_t              ind,
		  double               wght = 1.0,
                  size_t               sizeHint = 0);
  /** @brief insert element without checking */
  void insertElement(const collection_t*  pColl,
		     const constituent_t* pConst,
		     double              wght = 1.0,
		     size_t               sizeHint = 0);
  /** @brief insert element without checking */
  void insertElement(const collection_t*  pColl,
		     index_t              ind,
		     double              wght = 1.0,
		     size_t               sizeHint = 0);

  /** @brief Remove constituent */
  bool remove(const constituent_t* pConst);
  bool remove(const collection_t*  pColl,index_t ind);
  bool remove();

  ////////////////////////
  // Constituent Access //
  ////////////////////////

public:

  /** @brief @b (obsolete) begin iterator for constituent store */ 
  const_iterator firstConstituent()  const;
  /** @brief @b (obsolere) end iterator for constituent store */
  const_iterator lastConstituent()   const;
  
  /** @brief Begin iterator for constituent store */ 
  const_iterator begin() const;
  /** @brief End iterator for constituent store */
  const_iterator end() const;
  /** @brief Size of constituent store */
  size_t size() const;
  /** @brief Check on containment */
  bool contains(const constituent_t* pConst) const;
  /** @brief Check on containment */
  bool contains(const collection_t* pColl,index_t ind) const;

  /** @brief Retrieve kinematic weight of constituent */
  double getWeight(const constituent_t* pConst) const;
  /** @brief Retrieve kinematic weight of constituent */
  double getWeight(const collection_t* pColl,index_t ind) const;
  /** @brief Retrieve kinematic weight of constituent */
  double getWeight(const_iterator fConst) const;

  /** @brief Object navigation support, no relational parameter */
  virtual void fillToken(INavigationToken& navToken) const;
  /** @brief Object navigation support, relational parameter */
  virtual void fillToken(INavigationToken& navToken, 
			 const boost::any& par) const;

protected:

  /** @brief Retrieve weight */
  double getParameter(const constituent_t* pConst)            const;
  /** @brief Retrieve weight */
  double getParameter(const collection_t*  pColl,index_t ind) const;
  /** @brief Retrieve weight */
  double getParameter(const_iterator fConst)                  const;

public:
  /** @brief Retrieve pointer to constituent object container */
  const collection_t* getContainer(const constituent_t* pConst) const;
  /** @brief Retrieve pointer to constituent object container */
  const collection_t* getContainer(const_iterator fConst)       const;

  /** @brief Retrieve index of object in container */
  bool getIndex(const constituent_t* pConst,index_t& ind) const;
  /** @brief Retrieve index of object in container */
  bool getIndex(const_iterator fConst,index_t& ind)       const;



  ///////////////////////////
  // Access jet key system  //
  ///////////////////////////

protected :
  JetKeyDescriptorInstance * keyDesc() const;

  ///////////////////////////
  // Comparators & Overlap //
  ///////////////////////////

public: 

  
  /** @brief Test if @c Jet is identical to given @c Jet */
  bool isIdentical(const Jet& rJet) const;
  /** @brief Test if @c Jet is identical to given @c Jet */
  bool isIdentical(const Jet* pJet) const;

  /** @brief Retrieve constituent overlap between jets */
  Jet* getOverlap(const Jet& rJet, bool noKine = false) const;
  /** @brief Retrieve constituent overlap between jets */
  Jet* getOverlap(const Jet* pJet, bool noKine = false) const;

  ////////////
  // Shapes //
  ////////////

  /** @brief Set shape variable */
  void    setShape(const mkey_t& shapeName,shape_t shape,
		   bool addIfMissing=true) const ;
  /** @brief Retrieve shape variable */
  shape_t getShape(const mkey_t& shapeName,bool addIfMissing=false) const;

  /** @brief Retrieve list of avalailable keys */
  std::vector<mkey_t> getShapeKeys() const;

  /** @brief Alias for @c setShape */
  void    setMoment(const mkey_t& shapeName,shape_t shape,
		    bool addIfMissing=true) const; 
  /** @brief Alias for @c getShape */
  shape_t getMoment(const mkey_t& shapeName,bool addIfMissing=false) const; 
  
  /** @brief Alias for @c getShapeKeys */
  std::vector<mkey_t> getMomentKeys() const;


  /// Compatibility with JetAnalysisEDM
  template<typename T>
  T get(const std::string & mName) const ;

  template<typename T>
  void set(const std::string & mName, const T & v) const ;

  
  //////////////////
  // Associations //
  //////////////////

public:
  
  /** @brief Retrieve association object */
  template<typename ASSOC>
  const ASSOC* getAssociation(const mkey_t& key, bool useLink = true) const;
  const std::vector<mkey_t>& getAssociationKeys() const;
  const assoc_t* getAssociationBase (const mkey_t& key) const;
  
  /** @brief Set association object */
  template <class T>
  void setAssociation(const T* pAssoc,bool useLink = false ) const ;
  
  /** @brief remove and DELETE association object. */
  void removeAssociation( const mkey_t& key ) const ;
//   /** @brief Fill a token with association objects */  
//   void fillAssoToken(INavigationToken& token) const;

private:
  /** @brief clear association at given index */
  void removeAssociation(size_t index) const;
  
  /////////////
  // TagInfo //
  /////////////

public:
  
  /** @brief Retrieve tag info object */
  template<typename TAGINFO>
  const TAGINFO* getTagInfo(const mkey_t& key,bool useLink=true ) const ;
  
  /** @brief Set tag info object */
  void setTagInfo(const mkey_t& key,const taginfo_t* pTagInfo,
		  bool useLink=false) const ;
  
  /** @brief @b (depreciated) Retrieve vector of tag infos */
  const tagstore_t jetTagInfoVector() const;
  
  /** @brief @b (depreciated) list of tag info keys */
  const std::vector<mkey_t> infoKeys() const;
  
  /** @brief Retrieve tag info object */
  const taginfo_t* tagInfo(const mkey_t& key) const;
  
  /** @brief Retrieve a concrete tag info object */
  template<class TAGINFO>
  const TAGINFO* tagInfo(const mkey_t& key) const;
  
  /** @brief Add tag info object */
  template<class TAGINFO>
  void addInfo(const TAGINFO* tag) const;
  
  /** @brief @b (depreciated) Remove tag info object */ 
  void removeInfo(const mkey_t& key) const;

private:
  /** @brief Remove tag info for object located at index (internal) */
  void removeInfo(unsigned int index) const;
      


public:
  ////////////////////////////////
  // weight for Flavour tagging //
  ////////////////////////////////

  /// get the final b-tag result from the default tagger
  double getFlavourTagWeight() const;
  
  /// get the final b-tag result from a specific tagger
  double getFlavourTagWeight(const std::string& infoName) const;

  /** Get the current Signal state of the jet constituents */
  signalstate_t constituentSignalState() const;
  
  /** Set the current Signal state of the jet constituents */
  void setConstituentSignalState( signalstate_t s ) const;

  /** True if the jet constituent have a calibrated signal state*/
  bool has_calibrated_constit() const {return int(constituentSignalState())>0;};
  
  /** True if JETFINAL state equals JETEMSCALE state within  0.1 MeV on each E,px,pypz components*/
  bool finalScaleEqualsEMScale() const ;
  

 private:
  unsigned int m_RoIword;
  
 public:
  /** Return the RoI Word */
  unsigned int RoIword() const {return m_RoIword;};
  
  /** set RoI Word */
  bool set_RoIword( unsigned int word ) {m_RoIword = word; return true;};
  
  ///////////////
  // Operators //
  ///////////////
  
  /** @brief Comparison operator */
  bool operator==(const Jet& rJet) const;
  /** @brief Comparison operator */
  bool operator==(const Jet* pJet) const;
  /** @brief Comparison operator */
  bool operator!=(const Jet& rJet) const;
  /** @brief Comparison operator */
  bool operator!=(const Jet* pJet) const;
  /** @brief Assignment operator */
  Jet& operator=(const Jet& rJet);
  /** @brief Output operator */
  MsgStream& operator<<(MsgStream& out);

  ////////////////////////////////////////
  // Offline/Online Trigger Comparisons //
  ////////////////////////////////////////

  /** @brief Jet print-out function */
  std::string str();

 protected:

  //////////////////////////////////////
  // Protected SignalState Management //
  //////////////////////////////////////

  /** @brief Set signal state through a friend */
  friend class JetSignalStateHelper;
  friend class JetCollSigStateHelper;

  friend class JetAssociationBase;
  friend class JetTagInfoBase;
  template<class T> friend class SignalStateCollHelper;

private:
  /** @brief Set signal state to uncalibrated */
  void setStateRaw() const;

  /** @brief Set signal state to constituent Scale */
  void setStateCScale() const;
  
  /** @brief Set signal state to calibrated */
  void setStateCal() const;
  
public:
  void setRawE(double e);    /**< Sets uncalibrated @f$ E @f$   */
  void setRawPx(double px);  /**< Sets uncalibrated @f$ p_x @f$ */
  void setRawPy(double py);  /**< Sets uncalibrated @f$ p_y @f$ */
  void setRawPz(double pz);  /**< Sets uncalibrated @f$ p_z @f$ */

  void setCScaleE(double e);    /**< Sets uncalibrated @f$ E @f$   */
  void setCScalePx(double px);  /**< Sets uncalibrated @f$ p_x @f$ */
  void setCScalePy(double py);  /**< Sets uncalibrated @f$ p_y @f$ */
  void setCScalePz(double pz);  /**< Sets uncalibrated @f$ p_z @f$ */

  void setCalE(double e);    /**< Sets calibrated @f$ E @f$   */
  void setCalPx(double px);  /**< Sets calibrated @f$ p_x @f$ */
  void setCalPy(double py);  /**< Sets calibrated @f$ p_y @f$ */
  void setCalPz(double pz);  /**< Sets calibrated @f$ p_z @f$ */

  double getRawE()  const;
  double getRawPx() const;
  double getRawPy() const;
  double getRawPz() const;

  double getCScaleE()  const;
  double getCScalePx() const;
  double getCScalePy() const;
  double getCScalePz() const;

  double getCalE()  const;
  double getCalPx() const;
  double getCalPy() const;
  double getCalPz() const;

private:

  /** @brief Maximum difference up to which weights are considered equal */
  static double m_ignoreWeight;

  ///////////////////////////////////////////////
  // Protected Kinematics Recombination Scheme //
  ///////////////////////////////////////////////

  /** @brief Updates @c Jet kinematics */
  virtual void updateKine(const constituent_t* pConst,double wght=1.0);
  /** @brief Updates @c Jet kinematics */
  virtual void updateKine(const hlv_t& pVec,double wght=1.0);
  /** @brief Resummation of kinematics from constituents */
  virtual void updateKine();
  
 public:

  /** defaul function for preProcessConstituent and postProcessConstituent*/
  static bool nullOp( const constituent_type* /* constituent */ )
  { return true; };
  

 protected:  
  //////////////////////////////////////////
  // Protected Index Construction Support //
  //////////////////////////////////////////

  // /** @brief Index category for jet shapes */
  // static mcat_t m_shpCat;
  // /** @brief Index category for jet associations */
  // static mcat_t m_assCat;
  // /** @brief Index category for jet tag info */
  // static mcat_t m_tagCat;
  // /** @brief Index category for general jet info */
  // static mcat_t m_infCat;


  ////////////////////////
  // Jet Identification //
  ////////////////////////
public:
  
  /** returns this jet unique identifier in its collection */
  size_t id() const ;
  
  /** a pointer to the collection this jet belongs to (can be NULL)*/
  const JetCollection * parentCollection() const ;

protected:

  void setJetId(size_t id);
  
  static const size_t s_defaultJetId;
  /** @brief the identifier of this jet within its collection. (can be 0 if jet is orphan) */
  mutable size_t m_jetId;

  /** @brief Shape store link */
  //mutable DataLink<shape_map_t> m_momentMapLink;
  const JetCollection * m_collection;


  ////////////////
  // Jet Author //
  ////////////////

public:
  
  /// Author and calibration history are encoded in a
  /// simple way inside the jet object.
  /// This is done through a string of the form 
  ///  JetAuthor_AAAA_BB_CCC_ 
  ///  were AAAA , BBB, etc... are expected to tag calibration
  /// applied to this jet

  /** @brief Retrieve the author of this @c Jet  (only the author, without tags)*/
  std::string jetAuthor() const;

  /** @brief Retrieve the full author-and-tag string of this @c Jet */
  std::string jetAuthorAndCalibTags() const;
  

  /** @brief Set author of this @c Jet   (only the author, tags unchanged)*/
  void setJetAuthor(const std::string& author) ;

  /** @brief Set the full author-and-tag string */
  void setJetAuthorAndCalibTags(const std::string& author) ;
  
  /** @brief test if jet has calib tag @c tag */
  bool hasCalibTag(const std::string& tag) const;
  /** @brief number of calib tags for this jet  */
  int  numCalibTag() const ;
  /** @brief retrieve calib tag number @i (start at 1). returns "" if non-existant  */
  std::string getCalibTag(int i) const;
  /** @brief set calib tag @c tag  must be of the form "AAA" */
  void addCalibTag(const std::string& tag);


protected:
  friend class ParticleJetCnv_p1;
  friend class JetCnv_p1;
  friend class JetCnv_p2;
  friend class JetCnv_p3;
  friend class JetCnv_p4;

  static const size_t s_defaultJetAuthor;
  /** @brief Jet author store */
  size_t m_jetAuthor;

  
  /////////////////////
  // Protected Store //
  /////////////////////


  /** @brief key descriptor for all jet stores */
  //JetKeyDescriptorInstance* m_jetKeyDescr;


  /** @brief Association store */
  mutable assostore_t* m_assocStore;

  /** @brief Tag info store */
  mutable tagstore_t*  m_tagInfoStore;

  /** @brief Constituent Signal State */
  mutable signalstate_t m_constituentSigState;

  /////////////
  // Helpers //
  /////////////

  

  /** @brief Check key validity with automatic store generation */
  template<typename STORE>
  bool checkKeyStore(const mcat_t& cat,
		     const mkey_t& key,
		     STORE*&       store,
		     size_t&       aInd,
		     bool createIfMissing ) const
  {
    // invalid store pointer
    if ( store == 0 ) 
      {
	if ( !createIfMissing ) return false;
	store = new STORE();
      }

    // check index
    aInd = keyDesc()->getIndex(cat,key,createIfMissing);
    if ( keyDesc()->isValid(aInd) )
      {
	if ( aInd >= store->size() )
	  {
	    if ( ! createIfMissing ) return false;
	    store->resize(aInd+1);
	  }
	return true;
      }
    else
      {
	return false;
      }
  }

  /* @brief Check validity of key */  
  bool checkKey(const mcat_t& cat,
		const mkey_t& key,
		size_t& aInd,
		bool createIfMissing) const;

  
  /** @brief Get object from store with category and key */
  template<typename OBJ,class STORE>
  const OBJ* getObject(const mkey_t& key,
		       STORE* store,// not very nice but can not be const if using checkKey (P.A.)
		       const mcat_t& cat,
		       bool createIfMissing ) const
  {
    size_t aInd(0);
    return this->checkKeyStore(cat,key,store,aInd,createIfMissing)
      ? dynamic_cast<const OBJ*>((store->operator[])(aInd))
      : (const OBJ*)0;
    }


  /** @brief Get data from store with category and key */
  template<typename DATA,class STORE>
  bool getData(const mkey_t& key,
	       STORE*  store, // not very nice but can not be const if using checkKey (P.A.)
	       const mcat_t& cat,
	       DATA&         data,
	       bool createIfMissing ) const
  {
    size_t aInd(0);
    if ( this->checkKeyStore(cat,key,store,aInd, createIfMissing) )
      {
	data = (store->operator[])(aInd);
	return true;
      }
    else
      {
	return false;
      }
  }

  //////////////////////
  // Obsolete Methods //
  //////////////////////
  

  
private:
  // remember number of combinedLikelihood values stored in shapes
  size_t m_num_combinedLikelihood;
public:

  /** @brief <b>(depreciated)</b> Likelihood store access */
  const std::vector<double>& combinedLikelihood() const;
  
  /** @brief <b>(depreciated)</b> Likelihood store setter */
  void setCombinedLikelihood(const std::vector<double>& combinedLikelihood);


protected:
  /// Pointer to the fastjet object this jet is build from. WARNING : this pointer is meant to be used in reco flow only.
  ///  It is transient only,  provided only to ease calculation of some jet quantities, and will be invalid for jets read from files.
  const fastjet::PseudoJet *m_pseudoJet;

public:
  /// Access to the internal fastjet object. CAN RETURN NULL, see the description of m_pseudoJet
  const fastjet::PseudoJet * fastjetPtr() const {return m_pseudoJet;} 
  void setFastjetPtr(const fastjet::PseudoJet * fj){ m_pseudoJet = fj;}

};


/** @brief Difference between jets - Non-Class function required by trigger*/
void diff(const Jet& rJet1, const Jet& rJet2, std::map<std::string,double> varDiff);

#include "JetEvent/Jet.icc"



/**  @class Jet                                                                                                            
     @brief Basic data class defines behavior for all @c Jet objects   
     The @c Jet class is the principal data class for all reconstructed jets,     
     independent of the signal course. It describes a navigable composite         
     reconstruction objects with with a four-momentum representation consistent   
     with all other reconstruction objects. Its constituents are referenced as                                      
     generic @c INavigable4Momentum types.                                        
                                                                                  
     All @c Jet objects hold stores for constituents, shape information,          
     tagging information, and general associations to other reconstruction        
     objects.  These stores are constructed empty, except for the constituent      
     store if a (pointer) reference to a storable constituent object is given     
     to the @c Jet constructor.                                                   
                                                                                  
     The @c Jet can be constructed without constituents from just four-momentum   
     using the @c CLHEP::HepLorentzVector or the @c I4Momentum representation, or just   
     a sequence of four variables corresponding to the momentum components and    
     the energy. 


     The default signal state of a @c Jet depends on where it comes from. If 
     the @c Jet is retreived from a persistent store (e.g, ESD or AOD), the 
     default signal state is <tt>CALIBRATED</tt>. If the @ Jet is just
     constructed in transient memory, its initial (and default) signal state
     is UNCALIBRATED. If a @c Jet is a copy of another @c Jet, its default
     signal state is the same as the original.

     \author Peter Loch <loch@physics.arizona.edu>,                               
             Ambreesh Gupta <Ambreesh.Gupta@cern.ch>,                             
             Rolf Seuster <Rolf.Seuster@cern.ch>                                  
                                                                                  
     \date   April 10, 2004 - first implementation after re-design meetings       
     \date   May 24, 2004   - added constructors for Jets with 4-momentum and     
                              no constitents                                      
     \date   Feb. 11, 2008  - new EDM, new inheritance structure and              
                              including information for b-tagging                 
     \date   Feb. 16, 2008  - added signal states and clean-up        

*/
#endif //JETEVENT_JET_H

