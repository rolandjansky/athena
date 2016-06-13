// emacs: this is -*- c++ -*-
//
//   @file    addnode.h        
//
//                   
//  
//   Copyright (C) 2016 M.Sutton (sutt@cern.ch)    
//
//   $Id: addnode.h, v0.0   Thu 21 Apr 2016 20:31:49 CEST sutt $


#ifndef  ADDNODE_H
#define  ADDNODE_H

#include "node.h"

#include "TDirectory.h"

/// check if a subnode already exists
node* subnode( node* np, const std::string& name );

/// add a new node (or return the already existing node with this name)
node* addnode( node* np, const std::string& name, TObject* td=0, node::TYPE t=node::DIRECTORY );

#endif  // ADDNODE_H 










