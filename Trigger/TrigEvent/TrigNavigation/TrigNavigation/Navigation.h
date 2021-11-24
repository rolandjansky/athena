// Emacs -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGNAVIGATION_HLTNAVIGATION_H
#define TRIGNAVIGATION_HLTNAVIGATION_H

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IConversionSvc.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"

#include "TrigNavigation/NavigationCore.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigNavigation/Holder.h"
#include "TrigNavigation/FullHolderFactory.h"


namespace HLT {
  /**
   * @brief The Navigation class, organizes TriggerElements into the tree structure.
   *
   * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - U. of California - Irvine
   * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
   * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
   * @author Lukas Heinrich  <lukas.heinrich@cern.ch>  - NYU
   *
   * The class is providing an interface to the whole navigation. The internal ingredients
   * of the navigation which are TriggerElements are well hidden.
   * All operations on the navigation structure are possible only via trick class.
   * This operations are basically:
   *
   * -# creation of TriggerElements
   *     - creation of the initial (root) node
   *     - creation of RoI nodes (one per RoI, also for secondary RoIs)
   *     - creation of normal nodes (seed by one TriggerElement)
   *     - creation of normal nodes suitable for topological selection (being seed by 2 TriggerElements)
   * .
   * The created nodes are given to user with all the navigational links prepared
   *
   * -# features attaching and retrieval
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
   * -# queering methods
   *    - simple queries (isInitial, isTerminal, isRoI)
   *    - more complicated queries for 2 TriggerElements (if they share RoI etc)
   *    - global comparison for 2 navigation structures (needed up to the moment only for validation of serialization)
   *    - query for all TriggerElements of a given type
   * Queries are designed to be usable with STL algorithms.
   *
   *
   * Documentation of EDM related upgrade:
   * -# objects storage
   *    - all possible types are known at compile time (Scott's magic)
   *    - some collections are placed in the SG always independently of the event content
   *    - there can be more collection of given type (one for each user label i.e. number of tracks collections)
   *    - if in navigation config this is given: TrigVertexCollection#HLTAutoKey_T2HistoPrmVtx  --> 
   *      then DataVector<TrigVertexCollection> of label "HLTAutoKey_T2HistoPrmVtx" is placed in the SG in every event
   *    - if in config only type is mentioned TrigVertexCollection#HLTAutoKey ---> 
   *      then  DataVector<TrigVertexCollection> of label HLTAutoKey is placed in SG for each event
   *    - if nothing is mentioned in config the DataVector<TrigVertexCollection> labeled at runtime depending on user labels will be placed in SG (one per label)
   *
   *    - the object of given type are then organized then as follows:
   *      there is many collections of given type per event (each for one label) 
   *      this collections are accessible by label or "SubTypeIndex"
   *      second is to keep the TE -> feature link unchanged if we remove obe of collection during slimming or we simply not record it 
   *
   *     Is is like this
   *     collection of type T |XXXXXXX|       label: ID    subTypeIndex: 0
   *     collection of type T |XXXX|          label: MS    subTypeIndex: 1
   *     collection of type T |XXXXXXXXXXXXX| label: Muons subTypeIndex: 2
   *     collection of type T |XXXXXXXX|      label: Calo  subTypeIndex: 3
   *
   * -# the features <-> TE link
   *    - Features are located in supercontainers DataVector<TrigVertexCollection>
   *    - each label yields in one supercontainer (the unlabeled objects are in the container with HLTAutoKey_ label)
   *    - The link to object in the TE is composed out of 2 numbers CLID, and index
   *    - index (from above) is composed out of 2 parts collection coordinate and second is coordinate within collection
   *    - this way we do not need to touch TE serialization/desetialization
   *    - collection index and inter-collection index are uint16_t word which means we are limited to 65000 collections
   *       of given type and same number of objects in each sub collection
   */

  class Navigation : public AthAlgTool, public NavigationCore {
  public:
    // resolve ambiguity between base classes
    using AthAlgTool::msg;
    using AthAlgTool::msgLvl;

    Navigation( const std::string& type,
                const std::string& name,
                const IInterface* parent );

    virtual ~Navigation();

    virtual StatusCode initialize() override;

    /**
     * @brief attaches feature to given TriggerElement
     * @param te TriggerElement to which attach feature
     * @param feature is feature ptr to attach
     * @param key is filled up by the key used to put into SG (if not SG is involved then untouched)
     * @param label for additional marking of this feature
     * @return false if error enountered, true if no error encountered
     *  if this feature is on the list of classes to be serialized by reference then
     *  it is put into the SG (using SG->record)
     */
    template<class T> 
    bool attachFeature( TriggerElement* te, const T* feature,
			MemoryManagement, std::string& key,
			const std::string& label="" );

    template<class T> 
    bool attachFeature( TriggerElement* te, const ConstDataVector<T>* feature,
			MemoryManagement, std::string& key,
			const std::string& label="" );



    template<class T>
    bool associateExternalCollection(const std::string& label);
    /**
     * @brief find all TriggerElements which have this object attached given feature
     * @warning This search is costly since all TEs needs to be searched for. It can be significantly improved if
     *           search is limited to TEs with given ID
     * @param obj is a pointer to object of interest
     * @param owners is a vector of TEs which will be filled by all owners of the feature
     * @param id the id of TEs which should be inspected
     * @return true if no error was encountered, this does not mean that object is found, false if data access error occured
     **/
    template<class T> bool findOwners(const T* obj, std::vector<const TriggerElement*>& owners, unsigned int id = 0);

    /**
     * @brief Get the next key for a given object
     */
    template<class T> const std::string getNextKey( const std::string& label = "" );

    /**
     * @brief Get a unique key (not in the usual series) for a given object
     */
    template<class T> const std::string getUniqueKey( const std::string& label = "" );

  protected:
    Navigation(); //> not implemented
    Navigation (const Navigation&); //> not implemented
    Navigation& operator= (const Navigation&); //> not implemented

///////////////////////////////////////////////////////////////////////////////////////////////////////////
  private:
    // private stuff of Navigation class
    ServiceHandle<IConversionSvc>          m_serializerServiceHandle;
    ServiceHandle<IClassIDSvc>             m_clidSvc;
    Gaudi::Property<std::vector<std::string>>               m_dlls{this, "Dlls", {}, "Libraries to load (with trigger EDM)", "OrderedSet<T>"};
    FullHolderFactory                      m_fullholderfactory;
    bool                                   m_readonly;
    StatusCode classKey2CLIDKey(const std::vector<std::string>& property, std::vector<CSPair>& decoded);

  };

  MsgStream& operator<< ( MsgStream& m, const Navigation& nav ); //<! printing helper

} // eof namespace

#endif //#ifndef HLTNAVIGATION_H
