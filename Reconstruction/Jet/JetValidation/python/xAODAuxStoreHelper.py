# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import ROOT

cast_offset_str="""
#include "TBranchElement.h"
 #include "TObjArray.h"
 #include "TStreamerElement.h"
 #include "TStreamerInfo.h"

 Long_t cast_with_offset( TBranchElement* be ) {
    Long_t offset = ((TStreamerElement*)be->GetInfo()->GetElements()->At(be->GetID()))->GetOffset();
    return (Long_t)(be->GetObject() + offset);
 } 
"""
open("cast_with_offset.C","w").write(cast_offset_str)
ROOT.gROOT.LoadMacro("cast_with_offset.C+")


def initBranch( tree, name ):
    
    tree.SetBranchStatus( name, ROOT.kTRUE )
    be = tree.GetBranch( name )
    be.GetEntry(0)   # tickle object creation
    values = ROOT.BindObject( ROOT.cast_with_offset( be ), be.GetTypeName() )
    setattr( tree, name, values ) 
    return values

def initStaticStoreBranch(tree, be):
    be.GetEntry(0)   # tickle object creation
    values = ROOT.BindObject( ROOT.cast_with_offset( be ), be.GetTypeName() )
    setattr( tree, be.GetName(), values ) 
    return values
    
