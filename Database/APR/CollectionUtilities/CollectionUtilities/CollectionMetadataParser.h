/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_COLLECTIONMETADATAPARSER
#define UTILITIES_COLLECTION_COLLECTIONMETADATAPARSER

#include "CollectionBase/ICollectionMetadata.h"
#include <set>
#include <map>
#include <vector>
#include <string>


namespace pool
{

   typedef std::string ProvNode;
   typedef std::string MetaKey;
   typedef std::string MetaValue;

/**
 * @class CollectionMetadataParser
 *
 * @brief Because the provenance is encoded in the keys for the CollectionMetadata 
 * CollectionMetadataParser was created to make the metadata available in a tree 
 * which presents the data based on provenance nodes.
 *
 */

   class CollectionMetadataParser
   {
  public:
     /// Constructor
     CollectionMetadataParser();
     /// Constructor init directly
     CollectionMetadataParser(pool::ICollectionMetadata& cmd);

     /// Make constructor simple
     bool init(pool::ICollectionMetadata& cmd);

     // Track nodes in the tree

     /// Was any data created along with the collection being queried
     bool                                      hasSelfData() const;
     /// list of all provenance nodes
     std::set<ProvNode>                        listNodes() const;
     /// list of all provenance nodes of a certain level (increasing number is further back in prov)
     std::set<ProvNode>                        listNodes(int level) const;
     /// get provenance below
     std::vector<ProvNode>                     getChildren(const ProvNode&) const;
     /// get provenance above
     std::vector<ProvNode>                     getParents (const ProvNode&) const;
     std::map<ProvNode,std::vector<ProvNode> > getTree() const;

     // Retrieve data for Node

     /// Unique metadata
     std::set< std::pair<MetaKey,MetaValue> >     getUniqueMetaForNode(const ProvNode&) const;
     /// All metadata
     std::vector< std::pair<MetaKey,MetaValue> >     getMetaForNode(const ProvNode&) const;
     /// Retrieve data for Key
     std::vector< std::pair<ProvNode,MetaValue> >    getMetaForKey(const MetaKey& key) const;

     //unsigned int numNodes()  const {return m_nodes.size();}
     unsigned int numLinks()  const {return m_nodeconns.size();}
     unsigned int numLevels() const {return m_numLevels;}
     unsigned int size()      const {return m_keyvalues.size();}

  private:

     /// Run through metadata and build provenance tree
     void buildTree();

     int m_numLevels;

     std::multimap<int,ProvNode>                           m_levelList;
     std::map<ProvNode,std::vector<ProvNode> >             m_tree;
     //std::set<ProvNode>                                    m_nodes;
     std::set< std::pair<ProvNode,ProvNode> >              m_nodeconns;
     std::multimap<ProvNode,std::pair<MetaKey,MetaValue> > m_nodevalues;
     std::multimap<MetaKey,std::pair<ProvNode,MetaValue> > m_keyvalues;
   };

} // end pool namespace

#endif  // UTILITIES_COLLECTION_COLLECTIONMETADATAPARSER

