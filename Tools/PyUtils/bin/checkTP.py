#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file:    checkTP.py
# @purpose: dump the layout of a class (data members and bases)
#           Inspired from SealDictTest/DictClassCheck.py from RD.
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    September 2006
#
# @example:
#
# python checkTP.py TruthParticle
#
# if checkTP.py has been made 'chmod +x' one can just do:
# ./checkTP.py CaloCellContainer

from __future__ import print_function

import sys
import os

__author__  = "Sebastien Binet"

# MN: this has no effect when using RootType 
S   = 4 # SCOPED
SF  = 5 # SCOPED|FINAL
SQ  = 6 # SCOPED|QUALIFIED
SFQ = 7 # SCOPED|FINAL|QUALIFIED
DICTSCOPE = SF

_cpp_builtins = (
    'char',      'unsigned char',      'signed char',
    'short',     'unsigned short',
    'int',       'unsigned int',
    'long',      'unsigned long',
    'long long', 'unsigned long long', 'ulonglong',
    'float',
    'double',
    'bool',
    )

class DataMember(object):
    def __init__(self, offset, name, _type):
        self.offset = offset
        self.name   = name
        self.type   = _type
        return
    
class FctMember(object):
    def __init__(self, offset, name, _type):
        self.name   = name
        self.type   = _type
        return
    
class Columbo(object):

    def __init__(self):
        object.__init__(self)
        print ("")
        print ("#"*80)
        print ("## initializing...")
        import cppyy
        self.gbl = cppyy.gbl
        self.Type = cppyy.gbl.RootType

        self.report = []
        return

    def loadDicts(self, klassName):
        klassNames = [klassName]
        print ("## loading dictionary... [%s]" % klassName)

        ## protect against STL internals...
        if klassName.startswith("std::_"):
            return klassNames
            #return []

        ## protect against builtins
        if klassName in _cpp_builtins:
            return klassNames
        
        loaded = False
        try:
            loaded = getattr (self.gbl, klassName)
        except Exception as e:
            print ("Error loading dict. for [%s]" % klassName)
            print ("--> ", e)
        if not loaded:
            print ("Failed to load dict for [%s]" % klassName)
            return klassNames
        klass = self.Type.ByName(klassName)

        if not klass.IsStruct() and not klass.IsClass():
            return klassNames
        
        for i in range(klass.BaseSize()):
            baseKlassName = klass.BaseAt(i).Name()
            klassNames.extend (self.loadDicts(baseKlassName))
            pass
##         for i in xrange(klass.DataMemberSize()):
##             mbr = klass.DataMemberAt(i).TypeOf().Name(DICTSCOPE)
##             klassNames.append (self.loadDicts(mbr))
        return klassNames
    
    def dumpDataMembers(self, klass):
        dataMembers = []
        for i in range(klass.DataMemberSize()):
            d = klass.DataMemberAt(i)
            scope = klass.Name()
            offset = '<s>' if d.IsStatic() else d.Offset()
            fullname = '::'.join([scope, d.Name(SFQ)])
            typename = d.TypeOf().Name(SFQ)
            dataMembers.append( DataMember(offset, fullname, typename) )
        return dataMembers

    def dumpFctMembers(self, klass):
        fctMembers = []
        for i in range(klass.FunctionMemberSize()):
            f = klass.FunctionMemberAt(i)
            fctMembers.append( FctMember( f.Name(SFQ),
                                          f.TypeOf().Name(SFQ) ) )
            pass
        return fctMembers

    def inspect(self, klassName):
        self.report = []
        print ("")
        print ("#"*80)
        print ("## loading all relevant dictionaries...")
        try:
            klassNames = self.loadDicts(klassName)
            print ("#"*80)
        except Exception as err:
            print ("")
            print ("#"*80)
            print ("## ERROR while trying to load dict for [%s]" % klassName)
            print ("##  -Most probably you DIDN'T give a fully qualified name !")
            print ("##   Ex: try 'Analysis::Muon' instead of 'Muon'")
            print ("##")
            print ("##  -Could also mean that you are missing a dictionary ")
            print ("##   of one of the base classes...")
            print ("#"*80)
            print (err)
            raise
            return

        print ("")
        print ("#"*80)
        print ("## infos for class [%s]:" % klassName)
        print ("## sizeof(%s) = %i" % \
               (klassName,
                self.Type.SizeOf(self.Type.ByName(klassName))))
        print ("##")
        print ("## (offset, data member name, data member type)")
        print ("")
        # we want to dump from the base to the most derived class
        klassNames.reverse()
        for klass in klassNames:
            line = "%s %s %s" % (
                "-" * (40-len(klass)//2-1),
                "[%s]" % klass, 
                "-" * (40-len(klass)//2-1) )
                
            print (line)
            self.report.append(line)
            dataMembers = self.dumpDataMembers( self.Type.ByName(klass) )
            for i in dataMembers:
                line = "%3s %s %-50s %s %s" % ( str(i.offset),
                                                      " "*5,
                                                      i.name,
                                                      " "*5, i.type )
                print (line)
                self.report.append(line)
        print ("#"*80)
        return

    def save(self, fileName = "./columbo.out" ):
        file = open(os.path.expandvars(os.path.expanduser(fileName)),
                    "w+")
        for line in self.report:
            file.writelines(line + os.linesep)
            pass
        file.close()
        
    pass # Columbo


if __name__ == '__main__':
    if len(sys.argv) > 1:
        klassName = sys.argv[1]
    else:
        klassName = "xAOD::TruthParticle_v1"
        pass

    columbo = Columbo()
    columbo.inspect(klassName)
    columbo.save()
