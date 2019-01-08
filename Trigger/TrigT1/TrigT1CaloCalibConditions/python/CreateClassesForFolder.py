#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """
CreateClassesForFolder.py

Python script that creates C++ .h/.cxx files that contain the container and object class
representing the structure of a database folder.
"""

import argparse
import os.path
import string
import sys
import uuid

from CLIDComps.clidGenerator import clidGenerator
from PyCool import cool,coral

_dbSvc = cool.DatabaseSvcFactory.databaseService()
_cliddb = clidGenerator("")

_container_header = """// -*- C++ -*-
#ifndef TRIGT1CALOCALIBCONDITIONS_${CONTAINERNAME}_H
#define TRIGT1CALOCALIBCONDITIONS_${CONTAINERNAME}_H

#include <map>
#include <string>
#include <vector>
#include "AthenaKernel/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"
#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

class CondAttrListCollection;
class ${ObjectName};

/***
* Container of ${ObjectName} objects. Automatically created using:
*
* ${CommandLine}
*/
class ${ContainerName} : public DataObject, virtual public AbstractL1CaloPersistentCondition
{
private:
  enum eAttrSpecification { ${EnumAttrSpecification} };
public:
  ${ContainerName}();
  ${ContainerName}(const std::string& folderKey);
  virtual ~${ContainerName}() {}

  // interface of AbstractL1CaloPersistentCondition
  using AbstractL1CaloPersistentCondition::makeTransient;
  virtual void makeTransient(const std::map<std::string, CondAttrListCollection*>);
  virtual DataObject* makePersistent() const;
  virtual std::vector<std::string> coolInputKeys() const { return {m_coolFolderKey}; }
  virtual std::string coolOutputKey() const { return m_coolFolderKey; }
  virtual void clear() { m_${folderName}s.clear(); }

  // getters
  const ${ObjectName}* ${folderName}(unsigned int channelId) const;
  const ${ObjectName}* ${folderName}(const L1CaloCoolChannelId& channelId) const {
    return ${folderName}(channelId.id());
  }

  using const_iterator = std::vector<${ObjectName}>::const_iterator;
  const_iterator begin() const { return m_${folderName}s.begin(); }
  const_iterator end() const { return m_${folderName}s.end(); }

  // setters
  void add${FolderName}(const ${ObjectName}& ${folderName});

private:
  std::vector<${ObjectName}> m_${folderName}s;
  std::string m_coolFolderKey = "${FullFolderPath}";
};

${ClassDefMacro}

#endif // TRIGT1CALOCALIBCONDITIONS_${CONTAINERNAME}_H
"""

_container_src = """#include "TrigT1CaloCalibConditions/${ContainerName}.h"

#include <algorithm>
#include <memory>

#include "CxxUtils/make_unique.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/${ObjectName}.h"

${ContainerName}::${ContainerName}()
  : AbstractL1CaloPersistentCondition("CondAttrListCollection") 
{
${DefineSpecificationList}
}

${ContainerName}::${ContainerName}(const std::string& folderKey)
  : ${ContainerName}() // delegating constructor
{
  m_coolFolderKey = folderKey;
}


DataObject* ${ContainerName}::makePersistent() const
{
  using CxxUtils::make_unique;

  if(m_coolFolderKey.empty()) return nullptr;

  auto* attrSpecification = this->createAttributeListSpecification();
  if(!attrSpecification || !attrSpecification->size()) return nullptr;

  auto attrListCollection = make_unique<CondAttrListCollection>(true);
  for(const auto& item : m_${folderName}s) {
    AthenaAttributeList attrList(*attrSpecification);
${AddSpecificationList}
    
    attrListCollection->add(item.channelId(), attrList);
  }
  return static_cast<DataObject*>(attrListCollection.release());
}

void ${ContainerName}::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap)
{
  clear();

  auto it = condAttrListCollectionMap.find(m_coolFolderKey);
  if(it == std::end(condAttrListCollectionMap)) return;

  auto attrListCollection = it->second;
  for(const auto& item : *attrListCollection) {
    auto chanNum = item.first;
    const auto& attrList = item.second;
    
${ReadSpecificationList}

    add${FolderName}(${ObjectName}(chanNum, ${AttrList}));
  }
}

const ${ObjectName}* ${ContainerName}::${folderName}(unsigned int channelId) const
{
  auto it = std::lower_bound(std::begin(m_${folderName}s),
                            std::end(m_${folderName}s),
                            channelId,
                            [](const ${ObjectName}& el, unsigned int val) -> bool {
                              return el.channelId() < val;
                            });
  if(it == std::end(m_${folderName}s)) return nullptr;
  return &(*it);
}

void ${ContainerName}::add${FolderName}(const ${ObjectName}& ${folderName})
{
  // insert into the correct position mainting the sorted vector
  m_${folderName}s.insert(std::lower_bound(std::begin(m_${folderName}s),
                                           std::end(m_${folderName}s),
                                           ${folderName}.channelId(),
                                           [](const ${ObjectName}& el, unsigned int va) -> bool {
                                             return el.channelId() < va;
                                           }),
                          ${folderName});
}
"""

_object_header = """// -*- C++ -*-
#ifndef TRIGT1CALOCALIBCONDITIONS_${OBJECTNAME}_H
#define TRIGT1CALOCALIBCONDITIONS_${OBJECTNAME}_H

/**
 * Folder <-> Object mapping for ${FullFolderPath} .
 * Automatically created using:
 *
 * ${CommandLine}
 */
class ${ObjectName}
{
public:
  ${ObjectName}() {}
  ${ObjectName}(unsigned int channelId, ${AttrTypeNameList});

  unsigned int channelId() const { return m_channelId; }
${GetterList}

  void setChannelId(unsigned int channelId) { m_channelId = channelId; }
${SetterList}

private:
  unsigned int m_channelId = 0;
${AttrDefinitions}
};

#endif // TRIGT1CALOCALIBCONDITIONS_${OBJECTNAME}_H
"""

_object_src = """#include "TrigT1CaloCalibConditions/${ObjectName}.h"

${ObjectName}::${ObjectName}(unsigned int channelId, ${AttrTypeNameList})
 : m_channelId(channelId)
${AttrConstructorList}
{
}
"""

def add_args(parser):
    """ Add arguments to the ArgumentParser @parser. """
    parser.add_argument('folder', help='Folder which the class should represent')
    parser.add_argument('--include', help='Comma separated list of attributes to include')
    parser.add_argument('--exclude', help='Comma separated list of attributes to exclude')
    parser.add_argument('--objectName', help='Name of object class')
    parser.add_argument('--containerName', help='Name of container class')
    parser.add_argument('--db', help='Connection string of the database',
                        default='COOLONL_TRIGGER/CONDBR2')
    
def _mapToCpp(t):
    _m = {
        'UInt16': 'unsigned short',
        'UInt32': 'unsigned int',
        'UInt63': 'unsigned long long',
        'Int16' : 'short',
        'Int32' : 'int',
        'Double': 'double',
        'Blob64k': 'char*',
        'String255': 'char*'
        }
    return _m[t]

def _toCamelCase(identifier):
    return '{0}{1}'.format(identifier[0].lower(), identifier[1:])

def _createAddSpecificationList(nameList):
    _l = '    attrList[specificationName(e{AttrName})].setValue(item.{attrName}());'
    result = []
    for name in nameList:
        result.append(_l.format(AttrName=name, attrName=_toCamelCase(name)))
    return '\n'.join(result)

def _createDefineSpecificationList(nameTypeList):
    _l = '  this->addSpecification(e{AttrName}, "{AttrName}", "{attrType}");'
    result = []
    for (n, t) in nameTypeList:
        result.append(_l.format(AttrName=n, attrType=_mapToCpp(t)))
    return '\n'.join(result)

def _createReadSpecificationList(nameTypeList):
    _l = '    auto {attrName} = attrList[specificationName(e{AttrName})].data<{attrType}>();'
    result = []
    for (n, t) in nameTypeList:
        result.append(_l.format(AttrName=n, attrName=_toCamelCase(n), attrType=_mapToCpp(t)))
    return '\n'.join(result)

def _createGetterList(nameTypeList):
    _l = '  {attrType} {attrName}() const {{ return m_{attrName}; }}'
    result = []
    for (n, t) in nameTypeList:
        result.append(_l.format(attrName=_toCamelCase(n), attrType=_mapToCpp(t)))
    return '\n'.join(result)

def _createSetterList(nameTypeList):
    _l = '  void set{AttrName}({attrType} {attrName}) {{ m_{attrName} = {attrName}; }}'
    result = []
    for (n, t) in nameTypeList:
        result.append(_l.format(AttrName=n, attrName=_toCamelCase(n), attrType=_mapToCpp(t)))
    return '\n'.join(result)

def _createAttrDefinitions(nameTypeList):
    _l = '  {attrType} m_{attrName} = 0;'
    result = []
    for (n, t) in nameTypeList:
        result.append(_l.format(attrName=_toCamelCase(n), attrType=_mapToCpp(t)))
    return '\n'.join(result)

def _createAttrConstructorList(nameList):
    _l = ' , m_{attrName}({attrName})'
    result = []
    for n in nameList:
        result.append(_l.format(attrName=_toCamelCase(n)))
    return '\n'.join(result)

def _writeToFile(fname, template, mapping):
    f = open(fname, 'w')
    f.write(string.Template(template).substitute(mapping))
    f.close()

def create_classes_from_folder(folder, attrExcludeList, attrIncludeList, containerName, objectName):
    """ Does the actual work ... """
    folderName = folder.fullPath().split('/')[-1]
    attrNames = [ps.name() for ps in folder.payloadSpecification()]
    if attrIncludeList:
        attrNames = filter(lambda n: n in attrIncludeList, attrNames)
    elif attrExcludeList:
        attrNames = filter(lambda n: n not in attrExcludeList, attrNames)

    attrTypes = [ps.storageType().name() for ps in folder.payloadSpecification() \
                 if ps.name() in attrNames]
    attrNameTypes = zip(attrNames, attrTypes)

    if not containerName:
        containerName = 'L1Calo{0}Container'.format(folderName)
    if not objectName:
        objectName = 'L1Calo{0}'.format(folderName)

    # prepare the replacements for the container class
    mapping = {}
    mapping['CommandLine'] = ' '.join(sys.argv)
    mapping['ContainerName'] = containerName
    mapping['CONTAINERNAME'] = containerName.upper()
    mapping['FolderName'] = folderName
    mapping['folderName'] = _toCamelCase(folderName)
    mapping['ObjectName'] = objectName
    mapping['EnumAttrSpecification'] = ', '.join(['e{0}'.format(n) for n in attrNames])
    mapping['FullFolderPath'] = folder.fullPath()
    mapping['ClassDefMacro'] = \
       'CLASS_DEF( {0}, {1}, 1 )'.format(containerName,
                                         _cliddb.genClidFromName(containerName))
    
    mapping['DefineSpecificationList'] = _createDefineSpecificationList(attrNameTypes)
    mapping['AddSpecificationList'] = _createAddSpecificationList(attrNames)
    mapping['ReadSpecificationList'] = _createReadSpecificationList(attrNameTypes)
    mapping['AttrList'] = ', '.join(map(_toCamelCase, attrNames))

    # write the files
    _writeToFile('{0}.h'.format(containerName), _container_header, mapping)
    _writeToFile('{0}.cxx'.format(containerName), _container_src, mapping)

    # prepare the replacements for the object class
    for not_needed in ('ContainerName', 'CONTAINERNAME', 'FolderName', 'folderName',
                       'EnumAttrSpecification', 'ClassDefMacro', 'DefineSpecificationList',
                       'AddSpecificationList', 'ReadSpecificationList', 'AttrList'):
        del mapping[not_needed]
    mapping['OBJECTNAME'] = objectName.upper()
    mapping['AttrTypeNameList'] = ', '.join(['{0} {1}'.format(_mapToCpp(t), _toCamelCase(n)) for (n,t) in attrNameTypes])
    mapping['GetterList'] = _createGetterList(attrNameTypes)
    mapping['SetterList'] = _createSetterList(attrNameTypes)
    mapping['AttrDefinitions'] = _createAttrDefinitions(attrNameTypes)
    mapping['AttrConstructorList'] = _createAttrConstructorList(attrNames)

    # write the files
    _writeToFile('{0}.h'.format(objectName), _object_header, mapping)
    _writeToFile('{0}.cxx'.format(objectName), _object_src, mapping)

    # redefine mapping for slection.xml / *PACKAGE*Dict.h
    mapping = {
        'cont' : containerName,
        'obj' : objectName,
        'id' : str(uuid.uuid4()).upper()
    }
    print
    print '[I] Add the following to the selection.xml file:'
    print '   <class name="{cont}" id="{id}" />'.format(**mapping)
    print '   <class name="{cont}::const_iterator" />'.format(**mapping)
    print '   <class name="{obj}" />'.format(**mapping)
    print
    print '[I] Add the following to the TrigT1CaloCalibConditionsDict.h file:'
    print '#include "TrigT1CaloCalibConditions/{cont}.h"'.format(**mapping)
    print '#include "TrigT1CaloCalibConditions/{obj}.h"'.format(**mapping)
    print
    print '   {cont}::const_iterator {cont}ConstInterator;'.format(**mapping)

def main():
    """ Entry point """
    parser = argparse.ArgumentParser(description='Create C++ classes that represent COOL database folders')
    add_args(parser)
    args = parser.parse_args()

    if args.include and args.exclude:
        raise RuntimeError("Options --include and --exclude are mutually exclusive.")

    db = _dbSvc.openDatabase(args.db)
    folder = db.getFolder(args.folder)

    create_classes_from_folder(folder,
                               args.exclude.split(',') if args.exclude else None,
                               args.include.split(',') if args.include else None,
                               args.containerName, args.objectName)

if __name__ == "__main__":
    main();
