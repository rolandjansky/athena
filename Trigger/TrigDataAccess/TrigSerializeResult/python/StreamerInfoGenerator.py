#!/usr/bin/env pyroot.py 

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
import cppyy
import ROOT

class StreamerInfoGenerator:
  def __init__(self):
    self.debug = True
    print("StreamerInfoGenerator   v1.0.0")
    self.classlist = []
    self.problemclasses = []
    #MN: ROOT6 strips std:: from types, so we need to check the names
    self.blacklist = ['std::', 'vector<', 'map<', 'queue<', 'list<']
    self.type = cppyy.gbl.RootType
    self.type.EnableCintex()
    cppyy.load_library('libAtlasSTLAddReflexDict')
    #MN: switch off auto dict generation - god knows what that can mess up
    cppyy.gbl.gROOT.ProcessLine(".autodict")

    
  def inspect(self, typename):
    if self.debug: print('inspecting ', typename)

    dontAdd = False
    
    for b in self.blacklist:
      if typename.find(b) == 0:
        if self.debug: print('blacklisted ', typename)
        dontAdd = True
        
    # print self.classlist
    if typename in self.classlist:
      if self.debug: print('seen before ', typename)
      dontAdd = True
    
    try:
      t = self.type.ByName(typename)
      print(t)
      if t.IsFundamental():
        if self.debug: print(typename, ' is fundamental')
        return
      if t.IsAbstract(): 
        dontAdd = True 
    except:
      pass

    try:
      # This doesn't work in ROOT 6.22 anymore
      # cl = cppyy.makeClass(typename)
      #
      bind_name = typename
      # If it's a type with template argument, replace outermost <...> with ['...']
      bind_name = bind_name.replace("<", "['", 1)[::-1].replace(">", "]'", 1)[::-1]
      # Replace "::" with "." to set namespace, for the base type
      base_and_arg = bind_name.split("[")
      base_and_arg[0] = base_and_arg[0].replace("::", ".")
      bind_name = "[".join(base_and_arg)
      bind_name = "ROOT." + bind_name
      print("Making class {} -> {}".format(typename, bind_name))
      print("cl = " + bind_name)
      exec("cl = " + bind_name, globals())
      print(cl)
      if not dontAdd:
        self.classlist.append(typename)
        print("appended type to the classlist")
    except:
      print('Cannot create class of ', typename)

    t = self.type.ByName(typename)

    if t.IsComplete():
      if self.debug: print(typename, 'is complete')
    else:
      print(typename, ' isn\'t complete')

    if t.IsPointer():
      if self.debug: print(typename, ' is a pointer')
    elif t.IsTypedef():
      if self.debug: print(typename, ' is typedef')
      underlying = t.ToType()
      if (underlying):
        self.inspect(underlying.Name(7))
    elif t.IsArray():
      print(typename,' is an array')
    elif t.IsTemplateInstance():
      print(typename, ' is template')
      if typename.find('std::')==0:
        print('std::business removed')
        try:
          self.classlist.remove(typename)
        except:
          pass
      for i in range(t.TemplateArgumentSize()):
        tt = t.TemplateArgumentAt(i)
        ttname = tt.Name(7)
        if tt.IsPointer() or tt.IsArray() or tt.IsTypedef():
           ttname = tt.ToType().Name(7)
        self.inspect(ttname)
    elif t.IsClass():
      if self.debug: print(typename, ' is a class')
      cname = t.Name(7)
      if self.debug: print(cname)
            
      for i in range(t.DataMemberSize()):
        d = t.DataMemberAt(i)
        dname = d.Name()
        dtype = d.TypeOf().Name(7)
        if self.debug:
          print('DataMember: ', dname, ' ', dtype, '  transient=',  d.IsTransient())
        if not d.IsTransient():
          self.inspect(dtype)

    else:
      print('what to do about ', typename,'?')
      self.problemclasses.append( typename )
      return

        

  def streamers(self):
    print(self.classlist)


if __name__ == '__main__':
  from ROOT import TClass, TFile
  a = StreamerInfoGenerator()
  a.inspect('TrigTauClusterContainer_tlp1')

  print(a.classlist)
