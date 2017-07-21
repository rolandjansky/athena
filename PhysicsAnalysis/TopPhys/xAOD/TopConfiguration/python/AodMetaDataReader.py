#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Extract meta-data from xAODs -- without the help of Athena.
#

import argparse
import array
import ast
import json
import numbers
import os
import pickle
import re
import shutil
import sys
import tempfile


if not __name__ == '__main__':
   # We can't risk loading our dictionaries into your PyROOT process. So please, use the subprocess
   # module and the pickle interface. (If someone asks, we might implement a helper function.)
   raise ImportError('AodMetaDataReader cannot be loaded as a module')


# supported types of meta-data values (storage type, member name in IOVPayloadContainer, display type)
payloadItemDescs = [
   None,
   ('bool', 'm_bool', bool),
   ('char', 'm_char', int),
   ('unsigned char', 'm_unsignedChar', int),
   ('short', 'm_short', int),
   ('unsigned short', 'm_unsignedShort', int),
   ('int', 'm_int', int),
   ('unsigned int', 'm_unsignedInt', int),
   ('long', 'm_long', int),
   ('unsigned long', 'm_unsignedLong', int),
   ('Long64_t', 'm_longLong', int),
   ('ULong64_t', 'm_unsignedLongLong', int),
   ('float', 'm_float', float),
   ('double', 'm_double', float),
   ('std::string', 'm_string', str),
   ('ULong64_t', 'm_date', int),
   ('ULong64_t', 'm_timeStamp', int),
]
# whether the dictionary has been loaded
loadedDictionary = False


# create minimal header for reading persistent objects representing xAOD meta-data
def writeDictionaryHeader(f):
   f.write('''
#include <string>
#include <vector>
#include <Rtypes.h>
''')
   f.write('''
class AttrListIndexes {
public:
   int getTypeIndex() const { return (m_typeIndex & 0x3f); }
   int getNameIndex() const { return (m_typeIndex >> 6); }
   int getObjIndex() const { return m_objIndex; }
private:
   unsigned short m_typeIndex;
   unsigned short m_objIndex;
};
''')
   f.write('struct IOVPayloadContainer_p1 {\n')
   f.write('   std::vector<AttrListIndexes> m_attrIndexes;')
   for payloadItemDesc in payloadItemDescs:
      if not payloadItemDesc:
         continue
      f.write('   std::vector<%s> %s;\n' % (payloadItemDesc[:2]))
   f.write('   std::vector<std::string> m_attrName;')
   f.write('};\n')
   f.write('''
struct IOVMetaDataContainer_p1 {
   std::string m_folderName;
   std::string m_folderDescription;
   IOVPayloadContainer_p1 m_payload;
};
''')


# load classes for reading persistent objects representing xAOD meta-data
def loadDictionary():
   global loadedDictionary
   if loadedDictionary:
      return
   loadedDictionary = True
   tempPath = tempfile.mkdtemp()
   try:
      headerPath = os.path.join(tempPath, 'AodMetaData.h')
      with open(headerPath, 'wb') as f:
         writeDictionaryHeader(f)
      ROOT.gROOT.LoadMacro('%s+' % headerPath)
   finally:
      shutil.rmtree(tempPath, ignore_errors=True)


def extractMetaData(path):
   folderNames = '/Generation/Parameters', '/Simulation/Parameters', '/Digitization/Parameters', '/TagInfo'
   loadDictionary()
   f = ROOT.TFile.Open(path)
   if not f:
      raise RuntimeError('Failed to open file: %s' % repr(path))
   metaData = {}
   try:
      metaDataTree = f.MetaData
      metaDataTree.SetBranchStatus('*', False)
      folderNamesFound = []
      for folderName in folderNames:
         found = array.array('I', [0])
         metaDataTree.SetBranchStatus(folderName.replace('/', '_'), True, found)
         if found[0]:
            folderNamesFound.append(folderName)
         else:
            metaData[folderName] = None
      for metaDataRecord in metaDataTree:
         for folderName in folderNamesFound:
            payload = getattr(metaDataRecord, folderName.replace('/', '_')).m_payload
            folderData = {}
            for attrIndex in payload.m_attrIndexes:
               typeIndex, nameIndex, objIndex = attrIndex.getTypeIndex(), attrIndex.getNameIndex(), attrIndex.getObjIndex()
               payloadItemDesc = ( payloadItemDescs[typeIndex] if typeIndex < len(payloadItemDescs) else None )
               key = payload.m_attrName.at(nameIndex)
               value = ( payloadItemDesc[2](getattr(payload, payloadItemDesc[1]).at(objIndex)) if payloadItemDesc else TypeError )
               if not key in folderData:
                  folderData[key] = value
               elif not folderData[key] == value:
                  folderData[key] = ValueError
            metaData[folderName] = folderData
   finally:
      f.Close()
   return metaData


# merge two meta-data dictionaries
# Conflicting values are set to ValueError.
# Folders/values that exist in one but not another are set to None.
def mergeMetaData(md1, md2):
   assert len(md1) == len(md2)
   md = {}
   for folderName, folderData1 in md1.iteritems():
      folderData2 = md2[folderName]
      if folderData1 is None or folderData2 is None:
         folderData = None
      else:
         folderData = dict( (key, None) for key in folderData1 )
         folderData.update( (key, None) for key in folderData2 )
         for key in folderData:
            if key in folderData1 and key in folderData2:
               if folderData1[key] == folderData2[key]:
                  folderData[key] = folderData1[key]
               else:
                  folderData[key] = ValueError
      md[folderName] = folderData
   return md


# add derived meta-data for convenience
def augmentMetaData(metaData):
   derived = {}
   try:
      value = metaData['/Generation/Parameters']['HepMCWeightNames']
      mcWeightIndexByName = ast.literal_eval(value)
      mcWeightNames = []
      for name, index in mcWeightIndexByName.iteritems():
         if index >= len(mcWeightNames):
            mcWeightNames += ( None for _ in xrange(len(mcWeightNames), index + 1) )
         elif not mcWeightNames[index] is None:
            raise KeyError('multiple names for HepMCWeight index %d' % index)
         mcWeightNames[index] = name
      derived['HepMCWeightNames'] = ','.join(mcWeightNames)
   except Exception:
      # TODO proper error handling
      pass
   metaData['_derived'] = derived


# file-object wrapper around os.dup
def fdup(fileObject):
   fd2 = os.dup(fileObject.fileno())
   fileObject2 = None
   try:
      fileObject2 = os.fdopen(fd2, fileObject.mode, -1)
   finally:
      if fileObject2 is None:
         os.close(fd2)
   return fileObject2


# file-object wrapper around os.dup2
def fdup2(fileObject, fileObject2):
   os.dup2(fileObject.fileno(), fileObject2.fileno())


# write out meta-data as tab-separated plain text
def dumpPlain(metaData, out):
   for folderName in sorted(metaData):
      folderData = metaData[folderName]
      if folderData is None:
         continue
      for key in sorted(folderData):
         value = folderData[key]
         if isinstance(value, basestring):
            pass
         elif isinstance(value, numbers.Number):
            value = repr(value)
         else:
            # keys with conflicting/unrecognized values are completely ignored
            continue
         value = re.sub(r'[^\x21-\x7e]+', ' ', value).strip()
         out.write('%s\t%s\t%s\n' % (folderName, key, value))
   out.write('\n')


# supported meta-data dumpers
_dumpers = {
      'plain': dumpPlain,
      'pickle': pickle.dump,
      'json': json.dump,
   }


def main(argv, out):
   parser = argparse.ArgumentParser()
   parser.add_argument('--format', dest='dumper', action='store', choices=_dumpers.keys())
   parser.add_argument('--pickle', dest='dumper', action='store_const', const='pickle')
   parser.add_argument('--files-from', '-T', dest='filesFrom', action='store')
   parser.add_argument('--output', dest='outputFile', action='store')
   parser.add_argument('files', action='store', nargs='*')
   parser.set_defaults(dumper='plain')
   options = parser.parse_args(argv[1:])

   paths = []
   if options.filesFrom:
      with open(options.filesFrom, 'r') as f:
         for line in f:
            paths.extend(filter(None, line.rstrip('\n\r').split(',')))
   if options.files:
      paths += options.files
   dumper = _dumpers[options.dumper]

   path = paths[0]
   metaData = extractMetaData(path)
   for path in paths[1:]:
      metaData = mergeMetaData(metaData, extractMetaData(path))

   augmentMetaData(metaData)
   if options.outputFile:
      with tempfile.NamedTemporaryFile(dir=os.path.dirname(options.outputFile), delete=True) as f:
         dumper(metaData, f)
         f.flush()
         os.link(f.name, options.outputFile)
   else:
      dumper(metaData, out)
      out.flush()


# load ROOT in batch mode, skip loading of any (possibly conflicting) ROOT dictionaries through rootlogon
def loadROOT():
   args = sys.argv[1:]
   try:
      sys.argv[1:] = '-b', '-l', '-n'
      import ROOT
      ROOT.gErrorIgnoreLevel = ROOT.kError
      globals()['ROOT'] = ROOT
   finally:
      sys.argv[1:] = args


# acquire copy of stdout, then redirect /dev/null
# (so stray prints by ROOT don't mess up our dumps)
with fdup(sys.stdout) as out:
   with open(os.devnull, 'w') as nulout:
      fdup2(nulout, sys.stdout)
   loadROOT()
   main(sys.argv, out)
