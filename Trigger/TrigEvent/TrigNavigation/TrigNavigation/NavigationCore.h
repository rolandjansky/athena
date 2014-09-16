// Emacs -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGNAVIGATION_HLTNAVIGATIONCORE_H
#define TRIGNAVIGATION_HLTNAVIGATIONCORE_H

#include <stdint.h>
#include <set>
#include <sstream>
#include <iostream>

//#include <boost/cstdint.hpp>
//#include <stdint.h>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

#include "DataModel/OwnershipPolicy.h"
#include "DataModel/DataVector.h"

// for 2 ways of performing serialization
#include "StoreGate/StoreGateSvc.h"
//#include "TrigSerializeResult/ITrigSerializerToolBase.h"
//#include "TrigSerializeCnvSvc/TrigSerializeCnvSvc.h"

#include "TrigNavStructure/TrigNavStructure.h"

#include "TrigNavigation/Holder.h"
#include "TrigNavigation/TypeProxy.h"
#include "TrigNavigation/AccessProxy.h"
#include "TrigNavigation/TypeMaps.h"
#include "TrigNavigation/TrigFeatureLink.h"



#include "TrigStorageDefinitions/EDM_TypeInfoMethods.h"

class StringSerializer;

//static const InterfaceID IID_TrigNavigation("TrigNavigation", 1 , 0);


namespace HLTNavDetails { class TypeMapDeleter; }

class TrigBStoxAODTool;

namespace HLT {
  class TrigNavigationSlimmingTool;
  /**
   * @brief The NavigationCore class, adds on top of the TrigNavStructure the EDM read-only handling
   *
   * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - U. of California - Irvine
   * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
   * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
   *
   *
   * The created nodes are given to user with all the navigational links prepared
   *
   * -# features attaching and retrieval (the retrieval is implemented by this class whilc the attachement only needed in athena is in the HLTNavgation tool)
   *    - attaching feature to node
   *    - attaching feature to RoI node ( therefore all TE originating from node can use it )
   *    - attaching feature to initial (root) node for global (event wide) features
   *    - retrieval of features attached to given node
   *    - retrieval of features attached to given node or any predecessor (back to an initial node)
   *    - retrieval of features attached to given node or any predecessor (traversing back to an initial node)
   *    - retrieval of recent feature attached to given node or any predecessor (traversing back to an initial node)
   *    - retrieval of features attached to RoI nodes of given TriggerElement
   *    - retrieval of features attached to initial (root) node
   * .
   *
   *
   * Documentation of EDM related upgrade:
   * -# objects storage
   *    - all possible types are known at compile time (Scott's magic)
   *    - configured collections are placed in the SG always independently of the event content
   *    - there can be more collection of given type (one for each user label i.e. there can be several of tracks collections depending on the reconstruction)
   *
   *    - the object of given type are then organized then as follows:
   *      there is many collections of given type per event (each for one label)
   *      this collections are accessible by label or "SubTypeIndex" uint16_t - it is used to save space in the creation of the link from TE to the feature
   *      
   *     Is is like this
   *     collection of type T |XXXXXXX|       label: ID    subTypeIndex: 0
   *     collection of type T |XXXX|          label: MS    subTypeIndex: 1
   *     collection of type T |XXXXXXXXXXXXX| label: Muons subTypeIndex: 2
   *     collection of type T |XXXXXXXX|      label: Calo  subTypeIndex: 3
   *     (nothe that the association is not hardcoeded anywhere and can change from event to event. For that reason the association between the label and SubTypeIndex is recorded.)
   *
   * -# the features <-> TE link
   *    - Features are located in supercontainers DataVector<TrigVertexCollection>
   *    - each label yields in one supercontainer (the unlabeled objects are in the container with HLT_label)
   *    - The link to object in the TE is composed out of 2 numbers CLID, and index
   *    - index (from above) is composed out of 2 parts collection coordinate and second is coordinate within collection
   *    - collection index and inter-collection index are uint16_t word which means we are limited to 65000 collections
   *       of given type and same number of objects in each sub collection (this has been lifted up actually)
   */


  class NavigationCore : public HLT::TrigNavStructure {
    friend class HLT::TrigNavigationSlimmingTool;
    friend class HLTNavDetails::TypeMapDeleter;
    friend class ::TrigBStoxAODTool;
  public:
    /**
     * @brief constructor gets as an argument the TriggerElementFactory
     *
     * Will revise this Factory concept since it is probably better
     * to have templated factory ... to instantiate RoIDescriptors this way as well
     */

    NavigationCore();
    virtual ~NavigationCore();

    /*
    Navigation( const std::string& type,
		const std::string& name,
		const IInterface* parent );


    static const InterfaceID& interfaceID() {
      return IID_TrigNavigation;
    }

    virtual StatusCode initialize();  //!< initialization, there JO are read,parsed
    virtual StatusCode finalize();    //!< finalization, as reset()
    */


    /**
     * @brief prepapres the navigation for next event
     */
    virtual void prepare();
    /**
     * @brief resets all the navigation, goes to the factory and asks to withdraw all produced objects
     */
    virtual void reset();

    /**
     * @brief method serizlizes the navigation structure
     * The structure is serrizlized in following order ...
     * 1. TEs(together with links)
     * 2. features in accordane to the lists given in property ClassesToPayload
     *
     * The truncation mechainsm. The parameter space should be given to the method.
     * No more space than that will be ever taken. But since the actuall space limitation is not known
     * at the time whrn this call happens additional list of safe to cut places is returned.
     * One may ask why no tonly this cuts. ... Answer is follwing. If the event is huge then
     * giving this rought limit would make procedure faster since not all features will
     * be serialized. Otherwice they would be serialized and then thrown away, which is waste of CPU.
     * For safe operation one can easilly make space huge (like 2MB) and not care. But for online such tuning
     * can be considered. Space can be given as a property.
     *
     * @param output vector to place the result
     * @param cuts are indexes where one can safely cut the navigation
     *
     * @return if true then OK
     *         if false then truncated because of missing space - if output.size() != 0
     *         if false and output.size() == 0 internal error
     */
    bool serialize( std::vector<uint32_t>& output, std::vector<unsigned int>& cuts ) const;
    bool serialize( std::vector<uint32_t>& output, std::vector<unsigned int>& cuts, std::vector<std::pair<CLID, std::string> >& clid_name) const;
    bool serialize_DSonly( std::vector<uint32_t>& output, std::vector<unsigned int>& cuts, std::vector<std::pair<CLID, std::string> >& clid_name) const;
    bool deserialize( const std::vector<uint32_t>& input );

    /**
     * @brief reports on number of features which were unpacked frm BS but never accessed
     * This mehtod is to report what objects are unnecesarilly put in BS or not extracted.
     */
    //int numberOfSerializedByPayload( std::vector<CLID>& listOfTypes );


    /**
     * @brief defines 3 possible origins of the objects which are attached to TEs
     * This should be used like this:
     * -# All the objects which are produced out of blue
     *   using "new" operator should be attached with ObjectCreatedByNew switch.
     *   This objects may also go to SG depending on configuration.
     * -# All objects which needs to be registered in the StoreGate
     *  should be registered using switch "ObjectToStoreGate" which tells
     *  basically thet SG take control over the cleanup.
     *  All objects registered like this will for sure go to SG.
     * -# the switch "ObjectInStoreGate" should be used for objects
     *  which are already in SG. (i.e. put there by offline reco. tools).
     */
    enum MemoryManagement { ObjectInStoreGate = 1, ObjectToStoreGate = 2, ObjectCreatedByNew = 3 };


    /**
     * @brief retrieve features attached to given TriggerElement
     * @param te TriggerElement from retrieve
     * @param features is collection to be feed
     * @param label for additional marking of this feature
     * @param labels list of all features of this type already in place and thier labels
     * @return false if error enountered, true if no error encountered
     * @warning No error (return true) does not mean that features were found,
     * therefore what has to be checked is ... returned bool and
     * size of features vector (if it has grew).
     */
    template<class T> 
    bool getFeatures( const TriggerElement* te, std::vector< const T*>&  features, const std::string& label="", 
		      std::map<const T*, std::string>* labels=0 );

    template<class T> 
    bool getFeature( const TriggerElement* te, const T*&  features, const std::string& label="", std::string& sourcelabel = m_unspecifiedLabel);

    template<class T> 
    const T* featureLink2Object( const TrigFeatureLink& );

    template<class T> 
    TrigFeatureLink object2FeatureLink(const TriggerElement* te, const std::string& label,
				       const T* obj);


    template<class C> 
    TrigFeatureLink object2FeatureLink(const TriggerElement* te, const std::string& label,
				       const typename Container2Object<C>::type * obj, 
				       const C* container);

    //const TriggerElement* te, const T*&  features, const std::string& label="", std::string& sourcelabel = m_unspecifiedLabel);


    template<class C, class T> 
    bool getFeaturesLinks( const TriggerElement* te, ElementLinkVector<C>& links, const std::string& label="");

    

    /**
     * @brief retrieve features attached to given TriggerElement or its predecessors
     * the algorithm will go recursively to all predecessors and once the feature is found attached to given TE
     * returns all features (of this type and label) attached to this TE
     * @param te TriggerElement from which to start
     * @param features is vector to be filled up
     * @param label for additional marking of this feature
     * @param labels list of all features of this type already in place and thier labels (of not specified) faster query
     * @return false if error enountered, true if no error encountered
     * @warning No error (return true) does not mean that features were found,
     * therefore what has to be checked is ... returned bool and
     * feature ptr != 0;
     */
    template<class T> 
    bool getRecentFeatures( const TriggerElement* te, 
			    std::vector< const T*>&  features, const std::string& label="", 
			    std::map<const T*, std::string>* labels=0 );

    template<class T> 
    bool getRecentFeature( const TriggerElement* te, 
			   const T*&  feature, const std::string& label="", 
			   const TriggerElement*& source = m_unspecifiedTE, 
			   std::string& sourcelabel = m_unspecifiedLabel );

    template<class LinkType> 
    bool getRecentFeatureDataOrElementLink( const TriggerElement* te,
			    LinkType& link, const std::string& label="",
			    const TriggerElement*& source = m_unspecifiedTE, std::string& sourcelabel = m_unspecifiedLabel);

    template<class C, class T> 
    bool getRecentFeaturesLinks( const TriggerElement* te,
			    ElementLinkVector<C>& links, const std::string& label="" );

    template<class C, class T> 
    bool getRecentFeatureLink( const TriggerElement* te,
			       ElementLink<C>& link, const std::string& label="", 
			       const TriggerElement*& source = m_unspecifiedTE, 
			       std::string& sourcelabel = m_unspecifiedLabel );

    

    /**
     * @brief retrieve features accessors according to the requrements
     * This method is actually workhorse for all above. I.e. the TEs structure traversing happens inside it.
     *
     * @param te is TriggerElement at which searching starts
     * @param clid is ClassID of type which is searched for
     * @param label is object label (as used by attachFeature), empty lables (i.e. == "") means any label, 
     *        nonempty label means exact matching, and noverlty label == "!" means we are specifically looking for empty label
     * @param only_single_feature if true means we are looking for only one last object (i.e. imagine several object of the same type attached to TE)
     * @param features vector of FeatureAccessHelper objects to fill up
     * @param with_cache_recording record informations for caching 
     * @param travel_backward_recursively if true will doe actual travering TriggerElement structure to find objects, if false search will stop at the TriggerElement te
     * @param source TriggerElement where the object was found (or query stopped)
     * @param sourcelabel is labels which the object had (useful when calling this routine with empty label)
     **/
     
    bool getFeatureAccessors( const TriggerElement* te, CLID clid, const std::string& label, 
			      bool only_single_feature,
			      TriggerElement::FeatureVec& features, 
			      bool with_cache_recording,
			      bool travel_backward_recursively,
			      const TriggerElement*& source = m_unspecifiedTE, 
			      std::string& sourcelabel  = m_unspecifiedLabel);
      

    /**
     * @brief retrieve features attached to the RoIs seeding this TriggerElement
     * @param te TriggerElement from which start
     * @param features is collection to be feed
     * @param label for additional marking of this feature
     * @return false if error enountered, true if no error encountered
     * @param labels list of all features of this type already in place and thier labels (of not specified) faster query
     * @warning No error (return true) does not mean that features were found,
     * therefore what has to be checked is ... returned bool and
     * size of features vector (if it has grew).
     */
    template<class T> bool getFeaturesInRoI( const TriggerElement* te,  std::vector<const T*>&  features, 
					     const std::string& label="", std::map<const T*, std::string>* labels=0 );


    /**
     * @brief gets all features of type T atachedd to whichever TE
     * @param features vector of pointers to features to which will be filled up
     * @param label the label for a given feature
     * @param labels list of all features of this type already in place and thier labels (of not specified) faster query
     * @return true if no errors encountered
     */
    template<class C, class T> bool getAllFeatures( ElementLinkVector<C>&  features, const std::string& label="" ) ;



    /**
     * @brief Specialized by type and container for this type
     */
    template<class T, class C> static void registerFeatureContainer();



    /**
     * @brief attemtps to merge two trees
     */

    bool merge(const NavigationCore& l2);


    void tweakMsgLvl(int newOffset);

    void testMsgService() const; //!< test Message Svc

    void testStaticMaps();

    /**
     * @brief convert strin g to hash.
     * This is just making the function available, since there is no dict
     * for TrigConf::HLTUtils
     */
    static uint32_t string2hash( const std::string&, const std::string& category="TE" );


    /**
     * @brief return trigger elements given the name of TEs
     */
    void getAllOfType ( const std::string& id, 
			std::vector< HLT::TriggerElement* >& output,
			const bool activeOnly=true) const;

    using HLT::TrigNavStructure::getAllOfType;//( const HLT::te_id_type id, std::vector< TriggerElement* >& output,  const bool activeOnly = true ) const;

    /** @brief return StoreGate label and position for each feature.
     *
     *  The format used here is: label::position, e.g. "HLT_initialRoi::1"
     * @warning obsolete, will be removed
     **/
    /* maybe now
    std::vector< std::string >
    getFeatureSGLabelAndPosition( unsigned int clid, const TriggerElement* te,
    				  const std::string& label="");

    std::vector< std::string >
    getFeatureSGLabelAndPosition( std::string featureClassName, const TriggerElement* te,
				  const std::string& label="");

    std::vector< std::string >
    getRecentFeatureSGLabelAndPosition( unsigned int clid, const TriggerElement* te,
    					const std::string& label="");

    std::vector< std::string >
    getRecentFeatureSGLabelAndPosition( std::string featureClassName, const TriggerElement* te,
    					const std::string& label="");
    */

    /** @brief retrieve position of features inside the SG collections
     *
     * All data objects (features) attached to the TriggerElement te - with the given CLID + label -
     * are retrieved and their positions are returned.
     * @warning obsolete, will be removed
     **/
    /* maybe now
    std::vector< unsigned int >
    getFeatureContainerPosition( unsigned int clid, const TriggerElement* te,
				 const std::string& label);


    std::vector< unsigned int >
    getFeatureContainerPosition( std::string typeName, const TriggerElement* te,
    				 const std::string& label);
    */
    /** @brief retrieve position of features inside the SG collections
     *
     * All data objects (features) attached to the TriggerElement te - with the given CLID -
     * are retrieved and their (label, position start - end) are returned.
     *
     * @warning obsolete, will be removed
     */
    // used in:  TrigOfflineVal.cxx (follow up)
    /*
    std::vector< HLT::FeatureDescriptor >
    getFeatureContainerPosition( unsigned int clid, const TriggerElement* te);
    

    std::vector< HLT::FeatureDescriptor >
    getFeatureContainerPosition( std::string typeName, const TriggerElement* te);


    std::pair<std::string, std::string> getClassContainerNames(unsigned int clid) const;
    */

    /**
     * @brief sets the object mediating communcation with SG
     * We need this extra lightweight layer because in ARA we need to redirect SG::retrieve calls to ROOT TTree
     **/    
    void setAccessProxy(AccessProxy* ap);

    /**
     * @brief gets the access proxy
     **/    
    AccessProxy* getAccessProxy() const;
    
    /**
     * @brief  changes the default collections prefix (HLT) to another
     **/
    void setObjKeyPrefix(const std::string& k);


    template<class T> HLTNavDetails::Holder<T>* getHolder ( uint16_t subTypeIndex );                             //!< as above but does not create holder on demand (return 0 if not found)
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  protected:
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    // private stuff of Navigation class


    bool createHolder ( HLTNavDetails::IHolder*& holder, CLID clid, const std::string& label, uint16_t idx );  //!< creates holder for type given by CLID
    bool registerHolder ( HLTNavDetails::IHolder* holder );

    template<class T> HLTNavDetails::Holder<T>* getHolder ( const std::string& label, uint16_t suggestedIndex ); //!< aware holder discovery, creates holder if needed

    //    template<class T> HLTNavDetails::HolderCont<T>* getHolderCont ( const std::string& label );                          //!< type aware holder discovery, creates holder if needed
    // template<class T> HLTNavDetails::HolderCont<T>* getHolderCont ( uint16_t subTypeIndex );                             //!< as above but does not create holder on demand (return 0 if not found)
    HLTNavDetails::IHolder*                     getHolder ( CLID clid, uint16_t subTypeIndex ) const;            //!< as above but not type wise holder returned
    HLTNavDetails::IHolder*                     getHolder ( CLID clid, const std::string& label ) const;         //!< as above

    std::map< CLID, std::map< uint16_t, std::string> > m_lookupLabels;

    typedef std::map<uint16_t, HLTNavDetails::IHolder*> IndexToHolderMap;
    typedef std::map<std::string, HLTNavDetails::IHolder*> StringToHolderMap;
    typedef std::map<CLID,  IndexToHolderMap>     FeaturesStructure;
    typedef std::map<CLID,  StringToHolderMap>  FeaturesStructureLabelIndexed;

    FeaturesStructure             m_featuresByIndex;     //!< all objects map (it can be addressed by m_features[clid][idx]

    FeaturesStructureLabelIndexed m_featuresByLabel;     //!< all objects map (it can be addressed by m_features[clid][label]

    std::map< CLID, std::vector<uint32_t>* > m_serializedFeatures; //!< map can be replaced by the same concept as in TriggerStore

    /**
     * @brief Helper method for "combine": add one "level" of multiplicity to the results.
     */
    bool addOneLevel(std::vector< std::vector<TriggerElement*> >& currentCombs,
                     std::vector< std::vector<std::pair<unsigned int, unsigned int> > >& currentIdxs,
                     unsigned int type,
                     std::vector< std::vector<TriggerElement*> >& newCombs,
                     std::vector< std::vector<std::pair<unsigned int, unsigned int> > >& newIdxs,
                     unsigned int maxResults = 1000, bool onlyActive = 1);


    //    ITrigSerializerToolBase* m_serializer;     //!< pointer to Serializer

    MsgStream                             *m_log;                                //!< log stream
    //    IConversionSvc                        *m_serializerSvc;
    ServiceHandle<IConversionSvc>           m_serializerSvc;
    StringSerializer                      *m_stringSerializer;
    AccessProxy                           *m_storeGate;
    //    StoreGateSvc                          *m_storeGate;
    //    unsigned int                           m_serializeSpaceLimit;   //!< proprty to limit BS content to some reasonable size
    std::string                            m_objectsKeyPrefix;      //!< property setting prefix which is to be given to all trigger EDM objects
    unsigned                               m_objectsIndexOffset;    //!< small integer used to generate sub type index 

    typedef std::pair<CLID, std::string> CSPair;

    std::vector<std::string> m_classesToPayloadProperty;              //!< list of classes#keys to be put to BS payload
    std::vector<CSPair>  m_classesToPayload;   //!< classess are put to payload according to that priority list (CLID + key)
    
    std::vector<std::string> m_classesToPayloadProperty_DSonly;              //!< list of classes#keys to be put to DS payload
    std::vector<CSPair>  m_classesToPayload_DSonly;   //!< classess are put to payload according to that priority list (CLID + key)

    std::vector<std::string> m_classesFromPayloadProperty;              //!< list of classes#keys to be extracted from BS payload


    std::vector<std::string> m_classesToPreregisterProperty;             //!< as above but for preregistration
    std::vector<CSPair> m_classesToPreregister;   //!< classes mentioned here will be put to SG irrespectively of thier presence in event



    /**
     * @brief initialization helper - turns the list of class#key into map of CLID:key
     * As the same conversion (class name ==> CLID) needs to be done for classes to payload and classes to preregister this is outsourced to this function
     */
    StatusCode classKey2CLIDKey(const std::vector<std::string> prop, std::vector<CSPair>& decoded);



    bool m_referenceAllClasses;
    inline bool toBeReferenced(CLID /*c*/) { return m_referenceAllClasses; }
    bool toBePutToPayload(const HLTNavDetails::IHolder*) const;



    bool appendSerializedFeatureToPayload(CLID c, std::vector<uint32_t>& payload, const std::vector<uint32_t>& feature) const;


    uint16_t nextSubTypeIndex(CLID clid, const std::string&label);

    bool addBlob(std::vector<uint32_t>& output,
		 const std::vector<uint32_t>& blob1,
		 const std::vector<uint32_t>& blob2 ) const ;

    bool extractBlob(const std::vector<uint32_t>& input,
		     std::vector<uint32_t>::const_iterator& it,
		     std::vector<uint32_t>& blob) const ;




  private:
    static const TriggerElement* m_unspecifiedTE;
    static std::string m_unspecifiedLabel;
    
    HLTNavDetails::IHolder* prepareOneHolder(CLID clid, const std::string& label);

  };

  MsgStream& operator<< ( MsgStream& m, const NavigationCore& nav ); //<! printing helper

} // eof namespace

//#include "TrigNavigation/NavigationCore.icc"

#endif //#ifndef HLTNAVIGATION_H
