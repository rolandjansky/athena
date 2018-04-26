// emacs: this is -*- c++ -*-
//
//   @file    node.h        
//
//                    
//   Copyright (C) 2016 M.Sutton (sutt@cern.ch)    
//
//   $Id: node.h, v0.0   Fri  8 Apr 2016 00:57:16 CEST sutt $


#ifndef  HANCONFIGGENERATOR_NODE_H
#define  HANCONFIGGENERATOR_NODE_H

#include <iostream>
#include <string>
#include <vector>
#include <regex>


#include "TObject.h"

class node : private std::vector<node*> { 

public: 

  enum TYPE { HISTOGRAM, DIRECTORY, DUFF }; 

public:

  using std::vector<node*>::size;
  using std::vector<node*>::operator[];
  using std::vector<node*>::at;
  using std::vector<node*>::push_back;
  
public:

  /// if they have a parent, nodes push themselves back onto 
  /// their parent's child list 
 
  node( node* n=0, const std::string d="", TObject* t=0 ) : 
    mname("duff"), mparent(n), mtype(DUFF), mpath( n && n->path()!="" ? n->path()+"/" : "" ), mdepth(d), mobj(t) { 
    if ( t!=0 )  mname = t->GetName();
    mhirate = std::pair<std::string, double>( "", 0);
    if ( t!=0 ) mpath += mname;
    if ( mparent ) mparent->push_back( this );
  } 
  
  virtual ~node() { 
    /// delete the children
    for ( int i=0 ; i<size(); i++ ) delete at(i);
    /// now clear the vector in code this node in a tree and is 
    /// being deleted before the head of the tree  
    clear();
  } 

  void               name( const std::string& n)  { mname=n; }
  const std::string& name() const { return mname; }
  
  void        path(const std::string& p) { mpath=p; }
  std::string path() const               { return mpath; }
  
  node*       parent()       { return mparent; }
  const node* parent() const { return mparent; }

  std::string depth() const { return mdepth; }

  void type(TYPE t)         { mtype=t; }
  virtual TYPE type() const { return mtype; }
  
  std::string stype() const { 
    if ( type()==DIRECTORY ) return "DIRECTORY";
    if ( type()==HISTOGRAM ) return "HISTOGRAM";
    return "DUFF";
  };
  
  const TObject* object() const { return mobj; }
  TObject*       object()       { return mobj; }

  void addrate( const std::string& s, double r ) { 
    addrate( std::pair<std::string, double>( s, r ) );
  }

  void addrate( const std::pair<std::string, double>& r ) { 
    if ( r.second > mhirate.second ) mhirate = r;
  }

  const std::pair<std::string, double>& rate() const { return mhirate; }

private:

  std::string mname;
  node*       mparent;
  TYPE        mtype;

  std::string mpath;

  std::string mdepth;

  TObject*    mobj;

  std::pair<std::string, double> mhirate; 

  std::vector<node*> mchildren;

};



inline std::ostream& operator<<( std::ostream& s, const node& n ) {
  s << n.depth() << n.name() << "::" << n.stype() << " : obj " << n.object() << " : size " << n.size() << "\tpath " << n.path();
  if      ( n.type()==node::HISTOGRAM ) return s;
  else if ( n.size() ) {  
    if ( n.parent() ) s << "\t( parent " << n.parent()->name() << " )";
    if ( n.rate().first!="" )s << "\t\t:::(max rate chain " << n.rate().first << " " << n.rate().second << " ):::";
    for ( size_t i=0 ; i<n.size() ; i++ ) { 
      //      if ( n[i]->type()!=node::HISTOGRAM ) 
      s << "\n" << i << " " << n.depth() << " " << *n[i];
    }
  }
  return s;
}




/// lovely recursive function to traverse the 
/// entire tree

inline std::string travel( node* n, const std::string& regx="", int depth=0 ) { 
  if ( n==0 ) return ""; 

  if ( regx!="" && std::regex_match( n->name(), std::regex(regx) ) ) return n->name();
  
  if ( regx=="" ) {
    for ( int i=0 ; i<depth ; i++ ) std::cout << "\t";
    std::cout << n->name()  << "\t" << n->path();
    //   if  ( n->size()==0 && n->type()==node::HISTOGRAM ) std::cout << " ::H";
    std::cout << "\n";
  }

  for ( size_t i=0 ; i<n->size() ; i++ ) {
    std::string s = travel( n->at(i), regx, depth+1 );
    if ( s!="" ) return s;
  }

  return "";
}






#endif  // HANCONFIGGENERATOR_NODE_H










