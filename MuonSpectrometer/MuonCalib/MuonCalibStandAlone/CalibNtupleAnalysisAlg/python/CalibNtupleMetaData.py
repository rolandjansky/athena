# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import TFile, TTree
import sys

class CalibNtupleMetaData:
	
  def __init__(self, filelist):
    self.MetaData={}
    fl=file(filelist)
    root_file=None
    inf=None
    for line in fl.readlines():
      items=line.replace('\n', '').split()
      print line.replace('\n', ''), items
      if len(items)==0 or items[0][0]=="#":
        continue
      if items[0]=="[":
        if len(items)<2: 
          continue
        root_file=items[1]
      else:
        root_file=items[0]
      inf=TFile.Open(root_file)
      if not inf:
        sys.stderr.write("CalibNtupleMetaData WARNING: cannot open first file in filelist\n")
        continue
 	    
    if not inf:
      sys.stderr.write("CalibNtupleMetaData WARNING: find an input file in filelist\n")
      return
 			
    tree=inf.Get("meta_data")
    if not tree:
      sys.stderr.write("CalibNtupleMetaData WARNING: input file does not contain meta_data\n")
      return
 	
    i=0
    while tree.GetEntry(i):
      i+=1
      self.MetaData[str(tree.key)] = str(tree.value)
    print "Found meta data:"
    for k in self.MetaData:
      print k, self.MetaData[k]
