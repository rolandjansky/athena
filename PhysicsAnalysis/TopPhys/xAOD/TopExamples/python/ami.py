#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import re
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


# Call this function when the AMI client throws any exceptions.
# Quite likely, this is due to a missing VOMS proxy, so point
# that out to the user and exit with error code.
def exit_with_pyami_exception():
   print ''
   print logger.FAIL + 'is your voms proxy okay?' + logger.ENDC
   print 'Try voms-proxy-init -voms atlas'
   sys.exit(1)


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

  if failures == maxTries: exit_with_pyami_exception()

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

         if failures == maxTries: exit_with_pyami_exception()

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

        if failures == maxTries: exit_with_pyami_exception()

        for row in results:
           retName = row["ldn"]
           retNev  = row["events"]

           final[retName] = retNev

  results = {}
  for k in final.keys():
    results[str(k)] = int(final[k])

  return results


# Take a single LDN, extract its dataset number and return it.
def get_dataset_number(ldn):
   regex = re.compile("\.[0-9]{6}\.")
   match = regex.search(ldn)
   if not match:
      print "Could not find dataset number in LDN %s" % ldn
      sys.exit(1)
   else:
      return match.group(0)[1:-1]


# Take a single LDN, extract its p-tag and return it.
def get_ptag(ldn):
   regex = re.compile("_p[0-9]+")
   match = regex.search(ldn)
   if not match:
      print "Could not find p-tag in LDN %s" % ldn
      sys.exit(1)
   else:
      return match.group(0)[1:]


# Return the type of LDN we're looking at (EVNT, HITS, AOD, TOPQ1
# or other derivation etc.).
def get_type(ldn):
   match = re.search("\.recon\.AOD\.", ldn)
   if match: return "AOD"
   match = re.search("\.simul.HITS\.", ldn)
   if match: return "HITS"
   match = re.search("\.deriv\.DAOD_[A-Z]+[0-9]+\.", ldn)
   if match: return match.group(0)[7:-1]
   # If none of the above, exit with an error.
   print "Could not identify type of LDN %s" % ldn
   sys.exit(1)


# Get a list of all derivations registered on AMI for a list of
# LDNs. This removes the p-tags from the LDNs and replaces them
# with wildcards to search on AMI. The function returns a
# dictionary with dataset numbers as keys, and a list of LDNs as
# values. The LDNs are assumed to be TOPQ derivations.
def get_derivations(ldns):
   amiclient = pyAMI.client.Client('atlas')
   regex = re.compile("_p[0-9]+")
   wildcards = [regex.sub('_p%', d) for d in ldns]
   try: results = pyAMI.atlas.api.list_datasets(amiclient, patterns = wildcards, fields = ['ldn', 'dataset_number'], type = 'DAOD_TOPQ1')
   except pyAMI.exception.Error: exit_with_pyami_exception()

   dictionary = dict()
   for ldn in ldns: dictionary[get_dataset_number(ldn)] = []
   for d in results: dictionary[d['dataset_number']].append(d['ldn'])
   return dictionary


# Check whether an LDN is actually available and valid on AMI.
def is_available(ldn):
   amiclient = pyAMI.client.Client('atlas')
   try: results = pyAMI.atlas.api.list_datasets(amiclient, patterns = [ldn], fields = ['ldn'])
   except pyAMI.exception.Error: exit_with_pyami_exception()
   for d in results:
      if d['ldn'] == ldn: return True
   return False


# From a given list of LDNs, pick the latest one (i.e. extract
# the p-tag and pick the LDN with the highest one).
def pick_newest_derivation(ldn_list):
   if len(ldn_list) == 0: return ""
   get_ptag_as_int = lambda a: int(get_ptag(a)[1:])
   return sorted(ldn_list, key = get_ptag_as_int)[-1]


# Print the status of a list of samples on AMI. This takes a list
# of objects of type TopExamples.grid.Sample, wich then contain
# lists of datasets.
def check_sample_status(samples, stop_on_error):
   for s in samples:
      print '\n' + logger.WARNING + s.name + logger.ENDC + " (%s %s)" % (len(s.datasets), "dataset" if len(s.datasets) == 1 else "datasets")
      derivations = get_derivations(s.datasets)

      for ldn in s.datasets:
         dataset_number = get_dataset_number(ldn)
         dataset_type = get_type(ldn)
         latest_ldn = pick_newest_derivation(derivations[dataset_number])

         status = ""

         # First output whether the requested LDN exists and
         # has a valid entry in the AMI database.
         if is_available(ldn):
            status = logger.OKGREEN + "found" + logger.ENDC + ", "
         else:
            status = logger.FAIL + "not found" + logger.ENDC + ", "

         # If the dataset is _not_ a TOPQ1 derivation, output
         # its type and mark it in red.
         if dataset_type != "DAOD_TOPQ1":
            status += logger.WARNING + "Type: " + dataset_type + logger.ENDC + ", "

         # Then output the derivation status: (1) no
         # derivation available at all, (2) derivations
         # available, but the requested p-tag is not the
         # latest, (3) requested p-tag is the latest.
         if not latest_ldn:
            status += logger.FAIL + "no derivation available" + logger.ENDC
         elif not ldn == latest_ldn:
            status += logger.WARNING + "latest p-tag: " + get_ptag(latest_ldn) + logger.ENDC
         else:
            status += logger.OKGREEN + "latest" + logger.ENDC

         print " - %s (%s)" % (ldn, status)


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
