//
//   @file    addnode.cxx         
//   
//
//   @author M.Sutton
// 
//   Copyright (C) 2016 M.Sutton (sutt@cern.ch)    
//
//   $Id: addnode.cxx, v0.0   Thu 21 Apr 2016 20:31:53 CEST sutt $


#include "addnode.h"
#include "spacer.h"

/// check whether a subnode with this name 
/// already exists 
node* subnode( node* np, const std::string& name ) {
  if ( np==0 ) return 0;
  for ( unsigned i=np->size() ; i-- ; ) if ( np->at(i)->name()==name ) return np->at(i);
  return 0;
}

/// add a submode with a specific name, or return the existing
/// node if one already exists
node* addnode( node* np, const std::string& name, TObject* td, node::TYPE t ) {

  node* np_ = subnode( np, name );
  
  if ( np_ ) return np_;

  np_ = new node( np, np->depth()+spacer, td );
  np_->name( name );
  np_->type( t );
  
  return np_;
}   
