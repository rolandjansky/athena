/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiMiniConfigTreeNode_h
#define dqiMiniConfigTreeNode_h

#include <map>
#include <set>
#include <string>

#include <TObject.h>


namespace dqi {

/**
 * A node of a tree structure holding a configuration, where each node may be given
 * attributes, and a node inherits the attributes of parent nodes if those attributes
 * are not redefined.
 *
 * $Id: MiniConfigTreeNode.h,v 1.5 2008-12-04 16:40:16 ponyisi Exp $
 *
 * @author Michael Wilson, CERN, March 2007
 */

class MiniConfigTreeNode : public TObject {
public:

  class Visitor {
  public:
    virtual ~Visitor() { }
    virtual void Visit( const MiniConfigTreeNode* node ) const = 0;
  };

  class Writer {
  public:
    virtual ~Writer() { }
    virtual void Write(MiniConfigTreeNode* node ) = 0;
  };

  MiniConfigTreeNode( std::string name_, MiniConfigTreeNode* parent_ );

  virtual ~MiniConfigTreeNode();

  virtual const char* GetName() const;

  virtual const char* GetPathName() const;


 /**
  * Returns a daughter of this node, creating one if necessary.  This method only returns
  * daughters, not granddaughters.
  */

  virtual MiniConfigTreeNode*        GetNewDaughter( std::string name_ );

  virtual MiniConfigTreeNode*        GetDaughter( std::string name_ ) const;

  virtual MiniConfigTreeNode*        GetParent() const;

  virtual std::map<std::string,dqi::MiniConfigTreeNode*> GetDaughters() const;

 /**
  * This function takes the full path name of a subnode (in UNIX directory style) and
  * returns the corresponding subnode of this node, traversing the entire subtree until it is found.
  * If the bottom of the tree is reached before the full path has been used, the last node found
  * is returned.  If any part of the path is invalid before reaching the end of the tree, 0 is returned.
  */

  virtual const MiniConfigTreeNode*  GetNode( std::string name_ ) const;


  virtual void                   SetAttribute( std::string attName, std::string attValue, bool isAttribKeyword = false );

  virtual std::string            GetAttribute( std::string attName, bool calledFromDaughter = false ) const;

  virtual std::string            GetAttributeLocal( std::string attName ) const;

  virtual void                   GetAttributeNames( std::set<std::string>& attSet, bool calledFromDaughter = false ) const;

  virtual void                   GetAttributeNamesLocal( std::set<std::string>& attSet ) const;


  virtual void                   Accept( const Visitor& visitor ) const;

  virtual void                   Accept(Writer& writer );

  virtual void                   SetAttribKeywordPropagateDown( bool propagateDown );

  virtual bool                   GetAttribKeywordPropagateDown() const;

protected:

  typedef std::map<std::string,MiniConfigTreeNode*>  NodeMap_t;
  typedef NodeMap_t::const_iterator              NodeIter_t;

  typedef std::map<std::string,std::pair<std::string,bool> >      AttMap_t;
  typedef AttMap_t::const_iterator               AttIter_t;

  const std::string  m_name;
  mutable std::string  m_path;
  MiniConfigTreeNode*    m_parent;

  NodeMap_t          m_daughters;
  AttMap_t           m_attributes;

  bool m_propagateDown;

private:

//Get rid of Root macros that confuse Doxygen
///\cond CLASSDEF
  ClassDef( MiniConfigTreeNode, 0 ) // A node in a configuration tree
///\endcond
};

} // namespace dqi

#endif
