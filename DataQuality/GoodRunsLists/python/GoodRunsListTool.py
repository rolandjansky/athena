# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import os, sys

def importModule(moduleName,className,objectName=""):
  if len(objectName)==0: objectName=className
  if len(moduleName)>0:
    modname = '%s.%s' % (moduleName,className)
  else:
    modname = className
  import importlib
  mod = importlib.import_module (modname)
  return getattr (mod, objectName)

def getObjectOfClass(moduleName,className=""):
  moduleArr = moduleName.split(".")
  if len(moduleArr)==2: 
    libraryName = moduleArr[0]
  else: libraryName = "GoodRunsLists"
  configModule = moduleArr[len(moduleArr)-1]
  if len(className)==0: className=configModule

  try:
    cls = importModule (libraryName,configModule,className)
  except Exception:
    try:
      cls = importModule (configModule,className)
    except Exception:
      print ("Could not import configuration module \'" + configModule + "\'")
      sys.exit(1)

  return cls()

def createAndVerifyGRL(configModule,prefix=""):
  xmlfile = createGRL(configModule,prefix)
  allOk = verifyGRL(xmlfile)
  return (allOk,xmlfile)

def createGRL(configModule,prefix=""):
  from CoolRunQuery.AtlRunQueryLib import AtlRunQuery
  from CoolRunQuery.AtlRunQueryOptions import AtlRunQueryOptions
  from CoolRunQuery.AtlRunQueryParser import ArgumentParser

  ## get search query
  config = getObjectOfClass(configModule)
  config.setPrefix(prefix)
  query  = config.getsearchquery()
  print (">> Calling cmd equivalent of: ")
  print ("%s \"%s\"" % (config.querytool,query))

  ## atlrunquery parse arguments
  ap = ArgumentParser()
  atlqueryarg = config.querytool + " " + ap.ParseArgument( query )
  (options, args) = AtlRunQueryOptions().parse(atlqueryarg)

  #print (atlqueryarg)
  #print (options)

  ## query gets added to the xml file, in the field query
  AtlRunQuery(options, html="NO", origQuery=query)  # html can be "YES", "NO", "AUTO"

  xmlfile = 'data/'+config.listname
  print (">> Good-run list stored as: \'%s\'" % xmlfile)
  return xmlfile

def verifyGRL(xmlfile):
  ## Needed to correct ROOT behavior; see below
  CWD = os.getcwd()
  ## Importing gSystem may change the current directory to one of the
  ## command-line arguments; chdir to original directory to have
  ## predictable behavior
  from ROOT import gSystem
  os.chdir(CWD)
  ## Import the ROOT library for reading han results
  gSystem.Load('libGoodRunsListsLib')
  from ROOT import Root
  ## read the goodrunslist xml files
  reader = Root.TGoodRunsListReader()
  ## now adding xml file to interpreter
  reader.SetXMLFile(xmlfile)
  reader.Interpret()

  grlcollection = reader.GetMergedGRLCollection()
  print (">> Found %d independent good-run list(s). Now checking content." % len(grlcollection))

  allOk = True

  ## now checking name, version, metadata, content
  for idx in range(len(grlcollection)):
    grl        = grlcollection[idx]
    grlName    = grl.GetName()
    grlVersion = grl.GetVersion()
    grlMDSize  = grl.GetMetaDataSize()
    grlIsEmpty = grl.IsEmpty()
    if len(grlName)==0: 
      print (">> ERROR: good run list %d has no name." % (idx))
      allOk = False
    if len(grlVersion)==0:
      print (">> ERROR: good run list %d, with name \'%s\', has no version." % (idx,grlName))
      allOk = False
    if grlMDSize==0:
      print (">> ERROR: good run list %d, with name \'%s\', has no metadata." % (idx,grlName))
      allOk = False
    if grlIsEmpty: 
      print (">> ERROR: good run list %d, with name \'%s\', has no run(s) and lumi-block range(s)." % (idx,grlName))
      allOk = False

  if allOk:
    print (">> Content okay.")

  return allOk

