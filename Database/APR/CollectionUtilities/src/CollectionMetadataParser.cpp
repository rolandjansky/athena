/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/lexical_cast.hpp>
#include "POOLCore/boost_tokenizer_headers.h"
#include "CollectionUtilities/CollectionMetadataParser.h"
#include <algorithm>
#include <iostream>

using namespace std;
using namespace pool;

//help icc navigate pairs minefield, and speed up things a touch
const std::string SELFIE("SELF");

CollectionMetadataParser::CollectionMetadataParser() : m_numLevels(0) {}
 
CollectionMetadataParser::CollectionMetadataParser(pool::ICollectionMetadata& cmd) : 
  m_numLevels(0)
{
   bool stat = this->init(cmd);
   if (!stat) std::cout << "Unable to initialize parser" << std::endl;
   else std::cout << "Parser initialized" << std::endl;
}

bool CollectionMetadataParser::init(pool::ICollectionMetadata& cmd) 
{
   cout << "CollectionMetadataParser: reading metadata..." << endl;

   // first clear all the containers
   m_levelList.clear();
   m_tree.clear();
   //m_nodes.clear();
   m_nodeconns.clear();
   m_nodevalues.clear();
   m_keyvalues.clear();

   bool status(true);
   pool::ICollectionMetadata::const_iterator cmd_it = cmd.begin();

   //std::cout << "About to loop over metadata in parser init" << std::endl;
   while (cmd_it != cmd.end()) {
      std::string concatkey = cmd_it.key();
      std::string value     = cmd_it.value();

      // Now parse the key
      boost::char_separator<char> sep("|");
      boost::tokenizer<boost::char_separator<char> > tokens(concatkey,sep);
      int size = std::distance(tokens.begin(), tokens.end());
      //std::cout << "The num elements in " << concatkey << " is " << size << std::endl;
      status = (size > 1);
      if (status) {
         boost::tokenizer<boost::char_separator<char> >::iterator it = tokens.begin();
         boost::tokenizer<boost::char_separator<char> >::iterator key_it = tokens.begin();
         for (int i=0; i<size-1; ++i) ++key_it; 
         std::string key = boost::lexical_cast<std::string>(*key_it);
         ProvNode child(SELFIE);
         ProvNode parent;
         //std::cout << "Looking for parents" << std::endl;
         while (it != key_it) {
            //m_nodes.insert(*it);
            parent = boost::lexical_cast<std::string>(*it);
            // fill node connection vector ROOT->first->second->...->LEAF
            m_nodeconns.insert(std::make_pair(parent,child));
            child = parent;
            ++it;
         } // tokenizer
         std::string source = child;
         m_nodevalues.insert(
                              std::make_pair(source,
                                                     std::make_pair(key,value)));
         m_keyvalues.insert(
                              std::make_pair(key,
                                                  std::make_pair(source,value)));
      }
      else if (size==1) {
         m_nodeconns.insert(std::make_pair(SELFIE,SELFIE));
         std::string source = SELFIE;
         m_nodevalues.insert(
                              std::make_pair(source,
                                                     std::make_pair(concatkey,value)));
         m_keyvalues.insert(
                              std::make_pair(concatkey,
                                                  std::make_pair(source,value)));
      }
      ++cmd_it;   
   } // ICollectionMetadataIterator
   cout << "CollectionMetadataParser: parsing..." << endl;
   this->buildTree();
   return status;
}

bool CollectionMetadataParser::hasSelfData() const
{
   std::vector<ProvNode> Level = getParents(ProvNode(SELFIE));;
   std::vector<ProvNode>::const_iterator self_it= std::find(Level.begin(),Level.end(),SELFIE);
   return (self_it != Level.end());
}

std::set<ProvNode>
CollectionMetadataParser::listNodes() const
{ 
   //return m_nodes;
   std::set<ProvNode> nodes;
   for (int i=0; i<m_numLevels; ++i) {
      std::set<ProvNode> temps = this->listNodes(i);
      std::set<ProvNode>::const_iterator tempit = temps.begin();
      while (tempit != temps.end() ) {
         nodes.insert(*tempit);
         ++tempit;
      }
   }
   return nodes;
}

std::set<ProvNode>
CollectionMetadataParser::listNodes(int level) const
{
   std::set<ProvNode> nodes;
   for (std::multimap<int,ProvNode>::const_iterator it = m_levelList.lower_bound(level); 
                                                    it != m_levelList.upper_bound(level);
                                                  ++it) {
      nodes.insert(it->second);
   }
   return nodes;
}

std::vector<ProvNode> 
CollectionMetadataParser::getChildren(const ProvNode& p) const
{
   std::vector<ProvNode> children;
   std::set< std::pair<ProvNode,ProvNode> >::const_iterator it = m_nodeconns.begin();
   while (it != m_nodeconns.end() ) {
      if (it->first==p) children.push_back(it->second);
      ++it;
   }
   return children;
}

std::vector<ProvNode> 
CollectionMetadataParser::getParents (const ProvNode& c) const
{
   std::vector<ProvNode> parents;
   std::set< std::pair<ProvNode,ProvNode> >::const_iterator it = m_nodeconns.begin();
   while (it != m_nodeconns.end() ) {
      if (it->second==c) parents.push_back(it->first);
      ++it;
   }
   return parents;
}

std::map<ProvNode, std::vector<ProvNode> > CollectionMetadataParser::getTree() const
{ 
   return m_tree;
}

void CollectionMetadataParser::buildTree()
{
   std::vector<ProvNode> currentLevel = getParents(ProvNode(SELFIE));;
   std::vector<ProvNode> nextLevel;
   m_numLevels = 0;
   std::vector<ProvNode> selfvec; selfvec.push_back(ProvNode(SELFIE));
   //if (hasSelfData()) {
   m_levelList.insert(std::make_pair(m_numLevels,SELFIE));
   m_tree.insert(std::make_pair(SELFIE,selfvec));
   //}
   m_numLevels++;
   while (currentLevel.size()>0) {
      //std::cout << "for numLevel = " << m_numLevels << ", processing currentLevel of size = " << currentLevel.size() << std::endl;
      nextLevel.clear();
      std::vector<ProvNode>::const_iterator it = currentLevel.begin();
      while (it!=currentLevel.end()) {
         if (*it!=SELFIE) {
            m_levelList.insert(std::make_pair(m_numLevels,*it));
            std::vector<ProvNode> parents = getParents(*it);
            m_tree.insert(std::make_pair(*it,parents));
            std::copy(parents.begin(),parents.end(),back_inserter(nextLevel));
         }
         ++it;
      }
      currentLevel = nextLevel;
      m_numLevels++;
   }
   std::cout << "Provenance tree has " << m_numLevels << " levels" << std::endl;
}

std::set< std::pair<MetaKey,MetaValue> > 
CollectionMetadataParser::getUniqueMetaForNode(const ProvNode& node) const
{
   std::set< std::pair<MetaKey,MetaValue> > keyAndValue;
   std::multimap<ProvNode,std::pair<MetaKey,MetaValue> >::const_iterator last = m_nodevalues.upper_bound(node);
   std::multimap<ProvNode,std::pair<MetaKey,MetaValue> >::const_iterator first = m_nodevalues.lower_bound(node);
   while (first!=last) {
      keyAndValue.insert(first->second);
      ++first;
   }
   return keyAndValue;
}

std::vector< std::pair<MetaKey,MetaValue> > 
CollectionMetadataParser::getMetaForNode(const ProvNode& node) const
{
   std::vector< std::pair<MetaKey,MetaValue> > keyAndValue;
   std::multimap<ProvNode,std::pair<MetaKey,MetaValue> >::const_iterator last = m_nodevalues.upper_bound(node);
   std::multimap<ProvNode,std::pair<MetaKey,MetaValue> >::const_iterator first = m_nodevalues.lower_bound(node);
   while (first!=last) {
      keyAndValue.push_back(first->second);
      ++first;
   }
   return keyAndValue;
}

std::vector< std::pair<ProvNode,MetaValue> > 
CollectionMetadataParser::getMetaForKey(const MetaKey& key) const
{
   std::vector< std::pair<ProvNode,std::string> > nodeAndValue;
   std::multimap<MetaKey,std::pair<ProvNode,MetaValue> >::const_iterator last = m_keyvalues.upper_bound(key);
   std::multimap<MetaKey,std::pair<ProvNode,MetaValue> >::const_iterator first = m_keyvalues.lower_bound(key);
   while (first!=last) {
      nodeAndValue.push_back(first->second);
      ++first;
   }
   return nodeAndValue;
}

