// -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVSTRUCTURE_TRIGGERELEMENT_H
#define TRIGNAVSTRUCTURE_TRIGGERELEMENT_H

#include <map>
#include <vector>
#include <string>
#include <stdint.h>
#include <ostream>


#include "TrigNavStructure/Types.h"
namespace HLT {

  //  class NavigationCore;
  //  class RoICacheHelper;
  //  class TrigNavigationSlimmingTool;
  /*
  class FeatureDescriptor {
  public:
    FeatureDescriptor(std::string label, uint32_t objectsBegin, uint32_t objectsEnd)
      : m_label(label), m_begin(objectsBegin), m_end(objectsEnd) {}

    const std::string& label() const { return m_label; }
    uint32_t objectsBegin() const { return m_begin; }
    uint32_t objectsEnd() const { return m_end; }
  private:
    std::string m_label;
    uint32_t m_begin;
    uint32_t m_end;
  };
  */

  /**
   * @brief TriggerElement is the basic ingreedient of the interface between HLT algorithms and the navigation
   * It holds navigation links by itself but they are not exposed. The way to work with the navigation is only via the
   * Navigation class.
   *
   * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - U. of California - Irvine
   */
  class TriggerElement {
  public:
    
    TriggerElement();
    TriggerElement( te_id_type id, bool ghost, bool nofwd=false);
    ~TriggerElement();

    /**
     * @brief reset internals.
     * Needed because of TriggerStore (or TriggerFactory ...) can be used.
     */
    //    void reset();

    /**
     * @brief gets the id of this TriggerElement
     */
    inline te_id_type getId() const { return m_id; }

    /**
     * @brief get state of the TriggerElement
     */
    inline bool getActiveState() const { return m_state & activeState; }

    /**
     * @brief set state of the TriggerElement
     */
    void setActiveState( bool state );

    inline bool ghost() const { return m_state & ghostState; }
    bool getGhostState() const { return m_state & ghostState; }
    void setGhostState( bool t = true); 

    void setTransient(bool t = true) { m_transient = t; }
    bool transient() { return m_transient; }


    void setErrorState(bool state = true);

    inline bool getErrorState() const { return m_state & errorState; }

    //    friend class HLT::NavigationCore; //!< this class will be able to play with the navigation
    //    friend class HLT::TrigNavigationSlimmingTool;
    //    friend class HLT::RoICacheHelper; //!< this class will be able to play with the navigation

    /**
     * @brief Helper class for conversion from/to int stored in TE and pair of ints used in Navigation
     * Object pointer in Navigation is pair of ints. One is pointing so collection of givent type.
     * Second int points to object in this sub collection.
     * In TE fro historical reason (and space saving) this 2 ints need to colapse into one 32bit int.
     * @warning Therefore limitation ... we can have up to 65000 distinct collections of given type
     * @warning Therefore limitation ... we can have up to 65000 objects in each subcollection.
     */
    class ObjectIndex {
    public:
      
      ObjectIndex();

      ObjectIndex(sub_index_type subType, index_type begin, index_type end); //!< constructor used to create index used in TE
      sub_index_type subTypeIndex() const;  //!< to get collection index
      index_type objectsBegin() const;   //!< to get object number in th ecollection
      index_type objectsEnd() const;   //!< to get object number in th ecollection
      bool isSameOrWithin ( const ObjectIndex* idx) const; //!< check if idx is the same as this or is within this index
      void updateBeginAndEnd( index_type begin, index_type end);
      bool operator<(const ObjectIndex& obj) const;

      void serialize(std::vector<uint32_t>& output) const;
      void deserialize( std::vector<uint32_t>::const_iterator& inputIt);

      void setSubTypeIndex(sub_index_type idx) { m_subTypeIndex = idx; }
      bool valid() const;
      bool operator==(const ObjectIndex& rhs) const {
	if (m_subTypeIndex != rhs.m_subTypeIndex || m_objIndexBegin != rhs.m_objIndexBegin 
	    || m_objIndexEnd != rhs.m_objIndexEnd)
	  return false;
	return true;
      }
      bool operator!=(const ObjectIndex& rhs) const {
	return ! (*this == rhs);
      }

    private:
      sub_index_type m_subTypeIndex;           //!< actual index storage
      index_type m_objIndexBegin;          //!< auxiliary index word for big features
      index_type m_objIndexEnd;            //!< auxiliary index word for big features
    };





    /**
     * @brief possible relations enumeration (Note: uses no longer here)
     */
    enum Relation {
      sameRoIRelation,             //!< equilateral relation of all TEs rooted in one RoI
      seedsRelation,               //!< what TEs I am seeding
      seededByRelation             //!< what TEs are seeding me
    };

    /**
     * @brief returns reference to the likns to other TriggerElements related by relation r
     * @param rel relation name (one of HLT::Navigation::Relation)
     # @return is reference ot the vector of related TEs
     */
    inline const std::vector<TriggerElement*>& getRelated( Relation rel ) const {
      auto it = m_relations.find (rel);
      if (it != m_relations.end()) {
        return it->second;
      }
      static const std::vector<TriggerElement*> dumvec;
      return dumvec;
    }

    enum States { 
      activeState = 0x1,          //!< this bit is keeping active/inactive state of TE
      ghostState  = 0x2,          //!< this TE is of type ghost/evaporating/one can't attach features to this TE
      nofwdState  = 0x4,          //!< this TE is of type which stops fwd deactivation
      errorState  = 0x8           //!< somewhere in the algorithms dealing with this TE there was an error
    };

    std::map<Relation, std::vector<TriggerElement*> > m_relations;   //!< relations holder (features outside)

    /**
     * @brief reates given TE to other TE
     * @param te TriggerElement pointer
     * @param r relation .. one of Relation enums
     * Nothe that the relation is usually reflexive ...
     */
    void relate( TriggerElement* te, Relation r );

    /**
     * @brief relates this TE to vecor of other TEs
     * @param tes vector of TriggerElement pointers
     * @param r relation .. one of Relation enums
     */
    void relate( const std::vector<TriggerElement*>& tes, Relation r );

    static unsigned int enquireId( std::vector<uint32_t>::const_iterator& inputIt );

    /**
     * @brief serializes TE
     * @param output is vector of raw integers where to put serialized TE
     * @param keys is a map from pointers to ksys ... this keas go to serializized TE
     * @param previous UNUSED parameter
     */
    void serialize( std::vector<uint32_t>& output, const std::map<TriggerElement*, uint16_t>& keys, const TriggerElement* previous ) const;

    /**
     * @brief deserialization
     * @param inputIt a point where we should start reading
     * @param keys keys wich point to the TE related by seeds relation
     * @param previous UNUSED parameter
     */
    void deserialize( std::vector<uint32_t>::const_iterator& inputIt, const std::map<uint16_t, TriggerElement*>& keys, const TriggerElement* previous );



    /**
     * @brief the FeatureAccessHelper is a class used to keep track of features attached to this TE.
     * The storage is organized somewhere outside the TriggerElement (i.e. Navigation)
     * Once the TE is queried for the features it returns this access helpers and the entity is supposed to
     * make reasonable use of it.
     * There is number of advantages.
     * -# TriggerElement objects are lighter
     * -# the external storage can be organized more efficient
     * -# query for all features of given type can be much quicker
     * -# TriggerElement object can be streamed as it is (the index, CLID) remain valid after deserialization
     * -# the features can be serialized separately facilitating requirement of safe truncation
     */
    class FeatureAccessHelper {      
    public:
      FeatureAccessHelper() 
	: m_CLID(0), m_forget(false) {}

      FeatureAccessHelper( class_id_type clid, ObjectIndex index, bool forget=false) //!< the only constructor
	: m_CLID(clid), m_index(index),m_forget(forget) {}

      //FeatureAccessHelper( unsigned int clid, const ObjectIndex& index, bool forget=false) //!< the only constructor
      //	: m_CLID(clid), m_index(index),m_forget(forget) {}

      //method needed for navigation rewiring (AOD->xAOD)
      void setCLIDandIndex(class_id_type clid, sub_index_type idx){
	m_CLID = clid;
	m_index.setSubTypeIndex(idx);
      }
      inline class_id_type getCLID() const {return m_CLID;}     //!< Class ID of object
      inline const ObjectIndex& getIndex() const {return m_index;}   //!< index in the external ojects array

      inline void setForget( bool value = true ) { m_forget = value; }  //!< sets the forget flag
      inline bool forget() const { return m_forget; }          //!< flag, if true saying that the object does not need to be serialized
      friend std::ostream& operator<<( std::ostream& o, const FeatureAccessHelper& f ) {
	o << " CLID:" << f.m_CLID 
	  << " idx:" << f.m_index.subTypeIndex() 
	  << ":" << f.m_index.objectsBegin() 
	  << ":" << f.m_index.objectsEnd()   << std::dec;
	return o;
      }
      bool operator==(const FeatureAccessHelper& rhs) const {
	if (m_CLID != rhs.m_CLID || m_index != rhs.m_index)
	  return false;
	return true;
      }
      
      bool valid() const { return m_CLID != invalid_class_id and m_index.valid(); }

    private:
      class_id_type m_CLID;        //!< the CLID of the object
      ObjectIndex   m_index;  //!< the external index (here we do not know what it is)
      bool          m_forget; //!< flag, if true saying that link to this feature should not be serialized
    };
    typedef std::vector< FeatureAccessHelper > FeatureVec;

    /**
     * @brief returns all features which ara attached to this TE
     */
    const std::vector< FeatureAccessHelper >& getFeatureAccessHelpers() const { return m_uses; }
    std::vector< FeatureAccessHelper >& getFeatureAccessHelpers() { return m_uses; }

    /**
     * @brief add helper for new feature
     * @param clid the CLID of the object attached
     * @param index an integer used to retrieve it
     * @param forget is the flag notifying that during serialization link to this objects should be lost
     */
    void addFeature( class_id_type clid, const ObjectIndex& index, bool forget=false );
    void addFeature( const FeatureAccessHelper& f);

    
    const std::vector<FeatureAccessHelper>& getPreviousFeatures() const {return m_prev; }
    std::vector<FeatureAccessHelper>& getPreviousFeatures() {return m_prev; }
    //    void appendFeaturesKeyPrint(const std::vector<FeatureAccessHelper>& k);

  private:
    unsigned int m_state;        //!< one word for all states, info in bits  (saving space in serialized object)
    te_id_type m_id;                //!< id
    bool m_transient;            //!< marker that TE should not be serialized
    std::vector< FeatureAccessHelper > m_uses; //!< all features attached are in this storage
    std::vector< FeatureAccessHelper > m_prev; //!< all features attached are in this TE and seeding TEs
    
  };
} // eof namespace

#endif //TRIGNAVSTRUCTURE_TRIGGERELEMENT_H
