/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiMiniConfig_h
#define dqiMiniConfig_h

#include <set>
#include <string>

#include <TObject.h>

#include "DataQualityInterfaces/MiniConfigTreeNode.h"


namespace dqi {

/**
 * description
 *
 * $Id: MiniConfig.h,v 1.2 2008-10-02 12:37:44 ponyisi Exp $
 *
 * @author Michael Wilson, CERN, March 2007
 */

class MiniConfig : public TObject {
public:

  MiniConfig();

  virtual ~MiniConfig();

  virtual void AddKeyword( std::string keyword_ );
  virtual void AddAttributeKeyword( std::string keyword_ );

  virtual void SetAttribKeywordPropagateDown( bool propagateDown );

  virtual bool ReadFile( std::string fileName );


  virtual std::string GetStringAttribute( std::string objName, std::string attName ) const;

  virtual int GetIntAttribute( std::string objName, std::string attName ) const;

  virtual float GetFloatAttribute( std::string objName, std::string attName ) const;

  virtual void GetAttributeNames( std::string objName, std::set<std::string>& attSet ) const;

  virtual void SendVisitor( const MiniConfigTreeNode::Visitor& visitor ) const;

  virtual void SendWriter(MiniConfigTreeNode::Writer& writer );

protected:

  typedef std::set<std::string>     KeySet_t;
  typedef KeySet_t::const_iterator  KeyIter_t;

  KeySet_t             m_keywords;
  KeySet_t             m_attKeywords;
  MiniConfigTreeNode*  m_tree;

  bool m_propagateDown;

private:

//Get rid of Root macros that confuse Doxygen
///\cond CLASSDEF
  ClassDef( MiniConfig, 0 ) // A class that reads a configuration from a file
///\endcond
};

} // namespace dqi

#endif
