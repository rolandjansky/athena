#!/usr/bin/env python

import os,sys,string




def makeSource(className):
    text = """// First the corresponding header.
#include \"TrigT1MuctpiPhase1/CLASSNAME.h\"


// The headers from other ATLAS packages,
// from most to least dependent.


// Headers from external packages.


// System headers.


CLASSNAME::CLASSNAME()
{

}

CLASSNAME::~CLASSNAME()
{

}

""" 

    text = text.replace("CLASSNAME", "%s" % className)


    openfile = open("src/%s.cxx" % className,"w")
    openfile.write(text)
    openfile.close()

def makeHeader(className):
    text = """// This file is really -*- C++ -*-.
#ifndef TRIGT1MUCTPIPHASE1_CLASSNAME<CAPS>_H
#define TRIGT1MUCTPIPHASE1_CLASSNAME<CAPS>_H

/*
class description
*/

class CLASSNAME
{

 public:

  CLASSNAME();
  ~CLASSNAME();


 private:

};

#endif // TRIGT1MUCTPIPHASE1_CLASSNAME<CAPS>_H
"""

    text = text.replace("CLASSNAME<CAPS>", "%s" % className.upper())
    text = text.replace("CLASSNAME", "%s" % className)

    openfile = open("TrigT1MuctpiPhase1/%s.h" % className,"w")
    openfile.write(text)
    openfile.close()
    

if len(sys.argv) > 1:
    className = sys.argv[1]
    makeSource(className)
    makeHeader(className)

else:
    print "Useage: python makeClass.py <class-name>"

