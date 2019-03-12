// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGNAVSTRUCTURE_TRIGNAVSTRUCTURE_H
#define TRIGNAVSTRUCTURE_TRIGNAVSTRUCTURE_H

#include <vector>

#include "CxxUtils/checker_macros.h"
#include "TrigNavStructure/TriggerElement.h"
#include "TrigNavStructure/TriggerElementFactory.h"
#include "TrigNavStructure/BaseHolder.h"

#include "TrigNavStructure/TrigHolderStructure.h"

namespace HLTNavDetails {
  // RAII trick to fill sizes at the exit of the function
  // at construction the extra word is reserved and the current size remembered
  // at destruction the current size is filled at the previously reserved index
  struct FillSize {
    std::vector<uint32_t>& m_output;
    size_t m_index;
    FillSize(std::vector<uint32_t>& o)
      : m_output(o), m_index(m_output.size()) {
      m_output.push_back(0);
    }
    ~FillSize() {
      m_output[m_index]  = m_output.size();
    }
  };
}

namespace HLT {
  class TrigNavStructure {
  public:
    
    virtual ~TrigNavStructure();

    /**
     * @brief resets all the navigation, goes to the factory and asks to withdraw all produced objects
     */
    virtual void reset();

    /**
     * @brief gets initial node, if node is not there then it is created on fly
     */
    TriggerElement* getInitialNode();
    const TriggerElement* getInitialNode() const;

    /**
     * @brief gets RoI node (attached to initial)
     */
    TriggerElement* addRoINode( TriggerElement* initial );

    /**
     * @brief gets all RoI type nodes seeding indirectly this TriggerElement
     *
     * For getting all RoI nodes one can call getDirectrSuccessors(getIntialNode()
     *
     * @warning If this method is called for RoI node then it returns all TEs originating from it
     */
    static const std::vector<TriggerElement*>& getRoINodes( const TriggerElement* somenode );

    /**
     * @brief adds nodes seeded form the one given as an argument,
     * @param seed a TriggerElement which direct predecessor of newly created
     * @param id an id to give newly created TriggerElement
     */
    TriggerElement* addNode( TriggerElement* seed, unsigned int id );

    /**
     * @brief adds node which is seeded from several other
     * Needed by Topological algorithms
     * @param seeds are all TriggerElements which seed newly created
     * @param id the id to give for the created TriggerElement
     * @param ghost flag is saying that this TE is a light weight one, no features attaching to it is allowed
     * @param nofwd flag is saying that this TE will not be deactivated automatically in fwd deactivation process(rerunning)
     */
    TriggerElement* addNode( std::vector<TriggerElement* >& seeds, unsigned int id, bool ghost=false, bool nofwd=false );

    /**
     * @brief returns list of direct predecessors (nodes seeding me)
     * @param te TriggerElement to query
     * @return collection of TriggerElements (if empty then node is initial)
     */
    static const std::vector<TriggerElement*>& getDirectPredecessors( const TriggerElement* te );

    /**
     * @brief returns list of direct predecessors (nodes I seed)
     * @param te TriggerElement to query
     * @return collection of TriggerElements (if empty then node is terminal)
     */
    static const std::vector<TriggerElement*>& getDirectSuccessors( const TriggerElement* te );

    /**
     * @brief returns successor of given type beeing seeded from this TE
     * @param te TriggerElement to query
     * @param id the id of the successor TreiggerElement to find
     * @return TriggerElement pointer which can be 0
     * @warning the implementation is recursive
     */
    static const TriggerElement* getSuccessor( const TriggerElement* te, unsigned int id );

    /**
     * @brief queries if node is an initial one
     * @param te node to be queried
     * @return true if passed node is initial
     */
    static bool isInitialNode( const TriggerElement* te );

    /**
     * @brief queries if node is an RoI type one
     * @param te node to be queried
     * @return true if passed node is RoI type
     */
    static bool isRoINode( const TriggerElement* te );

    /**
     * @brief queries if node is terminal (no more TriggerElement are seeded by it)
     * @param te node to be queried
     * @return true if passed node is terminal
     */
    static bool isTerminalNode( const TriggerElement* te );


    /**
     * @brief pretty printing of the navigational structure (heavy)
     */
    void printASCIIArt( std::string& str, const TriggerElement* te = 0, int offset = 0 ) const;

    /**
     * @brief method serizlizes the navigation structure
     *
     * @param output vector to place the result
     *
     * @return if true then OK else some sort of failure happened (likely badly truncated structure)
     */
    bool serializeTEs( std::vector<uint32_t>& output ) const;
    bool deserializeTEs( std::vector<uint32_t>::const_iterator& start, unsigned int totalSize);

    /**
     * @brief method serizlizes entire navigation
     *
     * @param output vector to place the result
     *
     * @return if true then OK else some sort of failure happened (likely badly truncated structure)
     */
    virtual bool serialize( std::vector<uint32_t>&) const = 0;
    virtual bool deserialize( const std::vector<uint32_t>&) = 0;

    /**
     * @brief typeless feature access metod
     * @param te the TE from which the search should be done
     * @param clid the class ID of searched type
     * @param sub feature sub index (@see TriggerElement::ObjectIndex), if invalid_sub_type is passed then it is neglected
     * @return invalid FeatureAccessHelper is returned if nothng is found, else valid one
     */
    TriggerElement::FeatureAccessHelper getFeature(const TriggerElement* te, class_id_type clid, const index_or_label_type& index_or_label) const;


    /**
     * @brief recursive search for features 
     * the function is similar to the above butif th features is not found at the startTE the search descends the 
     * structure of TEs until it is found. It stops on RoI nodes.
     * @param sourceTE is the TE where the feature was found
     */
    TriggerElement::FeatureAccessHelper getFeatureRecursively(const TriggerElement* startTE, class_id_type clid,
							      const index_or_label_type& index_or_label, const TriggerElement*& sourceTE) const;


    /**
     * @brief The query returns a vector of all TriggerElements that represent a LVL1 RoI threshold
     */
    void getAllRoIThresholdTEs( std::vector< TriggerElement* >& output ) const;

    /**
     * @brief The query returning a collection of all TriggerElements
     */
    void getAll( std::vector< TriggerElement* >& output, const bool activeOnly = true ) const;

    /**
     * @brief access needed by slimming tools.
     */
    std::vector<TriggerElement*>& getAllTEs() { return m_factory.listOfProduced(); } 

    /**
     * @brief The query returning a collection of all TriggerElements if name is given
     * @param id name of TE, if "" given all TEs are returned
     */
    void getAllOfType( const te_id_type id, std::vector< TriggerElement* >& output,  const bool activeOnly = true ) const;    



    /**
     * @brief The query counting a collection of all TriggerElement of a given type
     */
    unsigned int countAllOfType( const te_id_type id, const bool activeOnly = true ) const;

    /**
     * @brief does this 2 share RoI
     *
     * RoI node itself can be passed, then it will tell if a given TriggerElement is originating in this RoI
     * for simple TriggerElement it is a bit overkill since it assumed that the number of RoIs can be any
     *
     * @warning this algorithm should not be used by sort or unique STL algorithms.
     * Comparing algorithms are required to be "Strict weak ordering" i.e.return false if comparing x and x
     * and if x < y and y < z then x < z (transitivity).
     * Sort algorithm will simply misbehave (crash trying to access objects under address 0)
     *
     * @return true if there is at least one common RoI
     */

    static bool haveCommonRoI( const TriggerElement* te1,  const TriggerElement* te2 );

    /**
     * @brief inversion of  haveCommonRoI
     *
     * But notice it is more suitable for SLT sort etc. since for comparison 2 identical objects
     * it returns false. However the transitivity can't be guaranteed.
     */
    static bool haveDistinctRoI(const TriggerElement* te1, const TriggerElement* te2 );

    static bool haveAllCommonRoIs ( const HLT::TriggerElement* te1, const HLT::TriggerElement* te2 );

    /**
     * @brief checks that there is any common predecessor of two TriggerElements
     * @warning See comments on haveCommonRoI. The same applies.
     */
    static bool haveCommonSeed( const TriggerElement* te1, const TriggerElement* te2 );

    /** inversion of  haveCommonSeed   */
    static bool haveDistinctSeed( const TriggerElement* te1, const TriggerElement* te2 );

    /**
     * @brief for sorting TriggerElements according to their id
     *
     * Note that this methods are not complementary. (It is required to be usable for STL algorithms.)
     */
    static bool increasingOrderByTEId( const TriggerElement* te1, const TriggerElement* te2 );
    static bool decreasingOrderByTEId( const TriggerElement* te1, const TriggerElement* te2 );

    /** simple helper */
    static bool hasIdFromTheSet( const TriggerElement* te, std::vector<unsigned int> ids );

    /** helper for the STL */
    static inline bool isNotActive( const TriggerElement* te ) { return !te->getActiveState(); }

    static inline bool isTopological( const TriggerElement* te) { return (getDirectPredecessors(te).size() > 1 ? true : false); }

    /**
     * @brief check compatibility of 2 navigation structures
     *
     * This what is checked is only the id of trigger elements whether they are having the same
     * place in the navigation structure; have identical subtrees (check is recursive)
     */
    static bool isCompatibleTree(const TriggerElement* te1, const TriggerElement* te2 );


    
    /**
     * @brief propagates deactivation of TE state
     * This finction should be used after Trigger is rerurn on L2 and then
     * the reruning on L2 is required. The deactivated lines must be propagated ahead.
     * @param nav is navigation which was reprocessed (additional branches deactivated)
     * @return true is all went fine, false in case it is spoted that the 2 navigation structures were not initially same
     */
    bool propagateDeactivation(const TrigNavStructure* nav);



    /**
     * @brief For Algorithms: return all the possible n-tuples of TE* objects (te0, te1..., ten)
     *
     * such that te0.id() == types[0], te1.id() == types[1]...
     * In the case where ids appear multiple times, n-tuple permutations are counted only once.
     * (so if te0.id() == te1.id(), (te0, te1) will appear but not (te1, te0))
     * "output" is populated with all the poClassID_traits<T>::ID();ssible combinations of this type.
     * if "activeOnly" is true, only active trigger elements are used.
     * The list is truncated at maxResults if maxResults > 0.
     */
    bool combine(std::vector<unsigned int>& types,
                 std::vector< std::vector<TriggerElement*> >& output,
                 unsigned int maxResults = 1000, bool onlyActive = true);

    /**
     * @brief Check overlap between trigger elements (pointer equality or RoI overlap)
     */
    bool overlaps(const TriggerElement* t1, const TriggerElement* t2) const;

    /**
     * @brief Check overlap between a TE and a vector of TEs, using the method above.
     */
    bool overlaps(const TriggerElement* t1, const std::vector<TriggerElement*>& teVec) const;


    /**
     * @brief gets the TEs whose predecessors were used to built a given topological TE
     * @param te topological TE from which to start
     * @param types vector of types to search for
     * @param childs vector of retrieved TEs; empty in case some of the required TEs cannot be found or are not active (see the activeOnly parameter)
     * @param activeOnly optional parameter specifying whether inactive TEs should be discarded from the search; true by default
     */
    bool getTopologicallySpannedBy( const TriggerElement* te, const std::vector<unsigned int>& types,
				    std::vector<TriggerElement*>& children, const bool activeOnly = true ) const;

    /**
     * @brief gets all features from the sourceTE and copies a reference (FeatureAccessHelper) to the destTE
     * @param sourceTE (source) TriggerElement holding features
     * @param destTE   (destiny) TriggerElement: copy feature references here
     * @return number of features that were copied
    */

    unsigned int copyAllFeatures( const TriggerElement* sourceTE, TriggerElement* destTE);

    const BaseHolder* getHolder(const TriggerElement::FeatureAccessHelper& fea) const;

    sub_index_type subType(class_id_type clid, const index_or_label_type& sti_or_label) const;
    std::string label(class_id_type clid, const index_or_label_type& sti_or_label) const;


    virtual bool getFeatureAccessors( const TriggerElement* te, class_id_type clid,
				      const index_or_label_type& index_or_label,
				      bool only_single_feature,
			      	      TriggerElement::FeatureVec& features, 
			              bool with_cache_recording,
			              bool travel_backward_recursively,
			              const TriggerElement*& source = m_unspecifiedTE, 
			              std::string& sourcelabel  = m_unspecifiedLabel) const;
  protected:

    virtual bool getFeatureAccessorsSingleTE( const TriggerElement* te, class_id_type clid,
					      const index_or_label_type& index_or_label,
					      bool only_single_feature,
					      TriggerElement::FeatureVec& features,
					      bool with_cache_recording,
					      const TriggerElement*& source,
					      std::string& sourcelabel ) const ;
    
    
    //method to see if feature matches by clid and either index or label
    bool matchFeature(const TriggerElement::FeatureAccessHelper& fea, class_id_type clid,  const index_or_label_type& index_or_label) const;

    /**
     * @brief rebuilds the sameRoIRelation between trigger elements (used internally by deserialize)
     * @warning should be called for each RoI like node
     */
    void fillSameRoIRelation ( TriggerElement* roi, TriggerElement* te );

    /**
     * @brief Helper method for "combine": add one "level" of multiplicity to the results.
     */
    bool addOneLevel(std::vector< std::vector<TriggerElement*> >& currentCombs,
                     std::vector< std::vector<std::pair<unsigned int, unsigned int> > >& currentIdxs,
                     unsigned int type,
                     std::vector< std::vector<TriggerElement*> >& newCombs,
                     std::vector< std::vector<std::pair<unsigned int, unsigned int> > >& newIdxs,
                     unsigned int maxResults = 1000, bool onlyActive = 1);



    TriggerElementFactory m_factory;                     //!< factory of trigger elements
    TrigHolderStructure m_holderstorage;                 //!< structure for feature holders
    static const TriggerElement* m_unspecifiedTE ATLAS_THREAD_SAFE;
    static std::string m_unspecifiedLabel ATLAS_THREAD_SAFE;


  };
} // end of HLT namespace

#endif // TRIGNAVSTRUCTURE_TRIGNAVSTRUCTURE_H
