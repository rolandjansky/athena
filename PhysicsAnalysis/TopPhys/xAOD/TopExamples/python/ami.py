#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os
import sys
from xml.dom.minidom import parse, parseString

import logger

try:
   import pyAMI.client
   import pyAMI.atlas.api
   #import pyAMI
except:
    print "Maybe you didn't do localSetupPyAMI?"
    sys.exit()

def getContainerContents(containerName):
  amiclient = pyAMI.client.Client('atlas')

  query = containerName.rstrip('/')
  data = "'%"+query+"%'"

  argument = 'SearchQuery -glite="select dataset.logicalDatasetName, dataset.identifier where dataset.logicalDatasetName like %s" -processingStep=real_data -project=dataSuper_001' % data

  maxTries = 3

  failures = 0
  for i in range(maxTries):
      try:
          results = amiclient.execute(argument, format='xml')
      except:
          failures += 1

  if failures == maxTries:
      print ''
      print logger.FAIL + 'is your voms proxy okay?' + logger.ENDC
      print 'Try voms-proxy-init -voms atlas'
      sys.exit(1)


  dom = parseString(results)
  rows = dom.getElementsByTagName('row')

  datasets = []
  for row in rows:
      fields = row.getElementsByTagName('field')
      for field in fields:
        if field.attributes['name'].value == 'logicalDatasetName':
          retName=field.firstChild.nodeValue
        if field.attributes['name'].value == 'identifier':
          identifier = field.firstChild.nodeValue

          #Now search for all datasets that match that
          argument = 'SearchQuery -glite="SELECT contained_dataset.contained_datasetName WHERE dataset.identifier=%s" -processingStep=real_data -project=dataSuper_001' % identifier

          results = amiclient.execute(argument, format='xml')
          dom = parseString(results)
          container = dom.getElementsByTagName('row')

          for dataset in container:
               fields = dataset.getElementsByTagName('field')
               for field in fields:
                 if field.attributes['name'].value == 'contained_datasetName':
                   datasets.append(field.firstChild.nodeValue.encode('ascii','ignore') + '/')
  
  return sorted(datasets)

def askAmi(query, property = 'totalEvents'):
  amiclient = pyAMI.client.Client('atlas')

  #swap * for % - databases
  query = query.replace('*', '%%')
  data = "'"+query+"'"

  argument = 'SearchQuery -glite="SELECT dataset.logicalDatasetName, dataset.%s WHERE dataset.logicalDatasetName LIKE %s"' % (property, data)

  if data.find('data') > -1:
        argument += ' -processingStep=real_data'
        if data.find('data15') > -1:
            argument += ' -project=data15_001'
        elif data.find('data16') > -1:
            argument += ' -project=data16_001'
        elif data.find('data12') > -1:
            argument += ' -project=data12_001'
        elif data.find('data11') > -1:
            argument += ' -project=data11_001'
        else:
            argument += ' -project=data10_001'
  else:
        argument += ' -processingStep=production'
        if data.find('mc09') > -1:
          argument += ' -project=mc09'
        elif data.find('mc10') > -1:
          argument += ' -project=mc10'
        elif data.find('mc11') > -1:
          argument += ' -project=mc11_001'
        elif data.find('mc12') > -1:
          argument += ' -project=mc12_001'
        elif data.find('mc14') > -1:
          argument += ' -project=mc14_001'
        elif data.find('mc15') > -1:
          argument += ' -project=mc15_001'
        elif data.find('mc16') > -1:
           argument += ' -project=mc16_001'

  maxTries = 3


  failures = 0
  for i in range(maxTries):
     final = {}

     if not "period" in query:
         try:
            results = amiclient.execute(argument, format='xml')
         except:
            failures += 1

         if failures == maxTries:
            print ''
            print logger.FAIL + 'is your voms proxy okay?' + logger.ENDC
            print 'Try voms-proxy-init -voms atlas'
            sys.exit(1)

         dom = parseString(results)

         #they changed it so that a voms-proxy-init error now returns a search
         #result with error in it. That's annoying.
         mayBeAnError = dom.getElementsByTagName('error')
         for row in mayBeAnError:
            print row.firstChild.nodeValue
            sys.exit(1)

         rows = dom.getElementsByTagName('row')

         for row in rows:

            fields = row.getElementsByTagName('field')
            retName = ''
            retNev = 0

            for field in fields:
               if field.attributes['name'].value == 'logicalDatasetName':
                  retName = field.firstChild.nodeValue

               if field.attributes['name'].value == property:
                  if field.firstChild != None:
                     retNev = field.firstChild.nodeValue

            final[retName] = retNev
     else:
        try:
           results = pyAMI.atlas.api.list_datasets(amiclient, query, fields="ldn,events,modified") 
        except:
           failures += 1

        if failures == maxTries:
           print ''
           print logger.FAIL + 'is your voms proxy okay?' + logger.ENDC
           print 'Try voms-proxy-init -voms atlas'
           sys.exit(1)
           
        for row in results:           
           retName = row["ldn"]
           retNev  = row["events"]
           
           final[retName] = retNev
  

  results = {}
  for k in final.keys():
    results[str(k)] = int(final[k])

    

  return results

if __name__ == '__main__':
    #data
    print 'Testing data15'
    results = askAmi('data15_comm.%.physics_Main.%.DAOD_TOPQ1.%p2356')
    for ds in sorted(results):
        print ds

    #MC simulation
    print 'Testing MC15'
    results = askAmi('mc15_13TeV.%.DAOD_TOPQ1.%p2352')
    for ds in sorted(results):
        print ds
