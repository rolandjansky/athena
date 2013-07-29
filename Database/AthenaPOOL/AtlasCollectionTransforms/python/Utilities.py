# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import PyCintex
PyCintex.Cintex.Enable()
import ROOT,commands,os.path

# Takes prot:/conn/name to coralprot|conn|name 
def urlToPoolrl(url):
  poolrl = ''
  protocol = ''
  pieces = url.split(':')

  if len(pieces)>1:
    protocol = pieces[0]
    if protocol=='oracle':
      (conn,coll) = os.path.split(url)
      poolrl = 'RelationalCollection|' + conn + '|' + coll
      #print 'poolrl=',poolrl
    elif protocol=='root':
      poolrl = 'RootCollection||PFN:' + pieces[1]
      #print 'poolrl=',poolrl
    else:
      print 'Only supported protocols are oracle and root:',url
  else:
    print 'ERROR: Bad url:',url

  return (protocol,poolrl)

def urlToDesc(url):
  name = ''
  type = ''
  conn = ''
  pieces = url.split(':')
  (c,n) = os.path.split(url)

  if len(pieces)>1:
    protocol = pieces[0]
    if protocol=='oracle':
      type = 'RelationalCollection'
      conn = c
      name = n
    elif protocol=='root':
      type = 'RootCollection'
      name = 'PFN:'+pieces[1]
    else:
      print 'Only supported protocols are oracle and root:',url
  else:
    print 'ERROR: Bad url:',url

  return (name,type,conn)

# post creation utility
def renameColumns(url,oldToNewMap):

  read_only_flag = False
  colsvc = ROOT.pool.CollectionService()
  (name, type, conn) = urlToDesc(url)
  col=colsvc.handle(name, type, conn, read_only_flag)

  if col!=0:
    for key in oldToNewMap.keys():
      try:
        col.schemaEditor().renameColumn(key,oldToNewMap[key])
      except: print "Error on rename of", key

    col.commit()
    col.close()

  else:
    print "Did not get collection handle to "+file

  return

# post creation utility { coll:'TYPE=XXX;UNIT=YYY;GRP=ZZZ' }
def annotateColumns(url,colToAnnoMap):

  read_only_flag = False
  colsvc = ROOT.pool.CollectionService()
  (name, type, conn) = urlToDesc(url)
  col=colsvc.handle(name, type, conn, read_only_flag)

  if col!=0:
    for key in colToAnnoMap:
      try:
        col.schemaEditor().annotateColumn(key,colToAnnoMap[key])
      except: print "Error on annotation of",key

    col.commit()
    col.close()

  else:
    print "Did not get collection handle to "+file

  return

# Assumes root files (multiple) as input, database (singular) as output
def uploadFilesToUrls(argsmap):

  copymap = {}
  reqargs = set(['inputTagFile','dbConnect','dbCollection'])
  if reqargs.issubset(set(argsmap.keys())):
    copymap['urls_in'] = []
    copymap['urls_out'] = []
    for tagfile in argsmap['inputTagFile']: copymap['urls_in'] += ['root:'+tagfile]
    copymap['urls_out'] += [argsmap['dbConnect'] + '/' + argsmap['dbCollection']]
  else:
    print "uploadFilesToUrls (ERROR): missing required",reqargs
    return False

  argsmap.update(copymap)

  return True

