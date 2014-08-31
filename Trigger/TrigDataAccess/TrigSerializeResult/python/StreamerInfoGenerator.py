#!/usr/bin/env pyroot.py 

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import PyCintex

class StreamerInfoGenerator:
  def __init__(self):
    self.debug = True
    self.classlist = []
    self.blacklist = ['std::']
    PyCintex.Cintex.Enable()
    PyCintex.loadDict('libSTLRflx')
    PyCintex.loadDict('libSTLAddRflx')
    PyCintex.loadDict('libAtlasSTLAddReflexDict')
    self.type = PyCintex.makeClass("Reflex::Type")

    
  def inspect(self, typename):
    if self.debug: print 'inspecting ', typename

    dontAdd = False
    
    for b in self.blacklist:
      if typename.find(b)>-1:
        if self.debug: print 'blacklisted ', typename
        dontAdd = True
        
    print self.classlist
    for o in self.classlist:
      if o==typename:
        if self.debug: print 'seen before ', typename
        dontAdd = True

    
    try:
      t = self.type.ByName(typename)
      print t
      if t.IsFundamental():
        if self.debug: print typename, ' is fundamental'
        return
    except:
      pass

    try:
      cl = PyCintex.makeClass(typename)
      if not dontAdd: self.classlist.append(typename)
    except:
      print 'Cannot create class of ', typename

    t = self.type.ByName(typename)

    if t.IsComplete():
      if self.debug: print typename, 'is complete'
    else:
      print typename, ' isn\'t complete'

    if t.IsPointer():
      if self.debug: print typename, ' is a pointer'
    elif t.IsTypedef():
      if self.debug: print typename, ' is typedef'
      underlying = t.ToType()
      if (underlying):
        inspect(underlying)
    elif t.IsArray():
      print typename,' is an array'
    elif t.IsTemplateInstance():
      print typename, ' is template'
      if typename.find('std::')==0:
        print 'std::business removed'
        try:
          self.classlist.remove(typename)
        except:
          pass
      for i in range(t.TemplateArgumentSize()):
        tt = t.TemplateArgumentAt(i)
        ttname = tt.Name(7)
        if tt.IsPointer() or tt.IsArray() or tt.IsTypedef():
          ttt = tt.ToType()
          ttname = ttt.Name(7)
        self.inspect(ttname)
    elif t.IsClass():
      if self.debug: print typename, ' is a class'

      cname = t.Name(7)
      if self.debug: print cname
      
      
      for i in range(t.DataMemberSize()):
        d = t.DataMemberAt(i)
        dname = d.Name()
        dtype = d.TypeOf().Name(7)
        if self.debug:
          print 'DataMember: ', dname, ' ', dtype
        self.inspect(dtype)

    else:
      print 'what to do about ', typename,'?'
      return

        

  def streamers(self):
    print self.classlist


if __name__ == '__main__':
  from ROOT import Cintex, TClass, TFile
  Cintex.Enable()
  a = StreamerInfoGenerator()
  a.inspect('TrigTauClusterContainer_tlp1')

  print a.classlist
