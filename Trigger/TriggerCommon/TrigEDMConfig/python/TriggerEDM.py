# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


# ********************* All Tools/Functions for the TriggerEDM **********************
# Keeping all functions from the original TriggerEDM.py (Run 2 EDM) in this file
# with this name to not break backwards compatibility
# Where possible, functions will be adapted to also work with Run 3, they will then be moved
# to the Run 3 section
# ***********************************************************************************

from TrigEDMConfig.TriggerEDMRun1 import TriggerL2List,TriggerEFList,TriggerResultsRun1List
from TrigEDMConfig.TriggerEDMRun2 import TriggerResultsList,TriggerLvl1List,TriggerIDTruth,TriggerHLTList,EDMDetails,EDMLibraries,TriggerL2EvolutionList,TriggerEFEvolutionList
from TrigEDMConfig.TriggerEDMRun3 import TriggerHLTListRun3, AllowedOutputFormats,addExtraCollectionsToEDMList
from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
from AthenaCommon.Logging import logging
log = logging.getLogger('TriggerEDM')

import six

#************************************************************
#
#  For Run 3
#
#************************************************************
# Let adding collections via preExec for detailed validation
if flags.Trigger.ExtraEDMList:
    log.info( "Adding extra collections to EDM (): {}".format(len(TriggerHLTListRun3), flags.Trigger.ExtraEDMList) )
    addExtraCollectionsToEDMList(TriggerHLTListRun3, flags.Trigger.ExtraEDMList)
    log.info( "Number of EDM items after adding extra collections: {}".format(len(TriggerHLTListRun3)) )


def getTriggerEDMList(key, runVersion):
    """
    List (Literally Python dict) of trigger objects to be placed with flags:
    key can be" 'ESD', 'AODSLIM', 'AODFULL', 'DS'
    additionally for Run 3 the key can be 'AODSMALL' and 'AODLARGE'
    run can be: '1 (Run1)', '2 (Run2)', '3' (Run 3)
    """
    if runVersion == 2:
        if 'SLIM' in key:
            return getTriggerEDMSlimList(key)
        else:
            return getTriggerObjList(key,[TriggerHLTList, TriggerResultsList])

    elif runVersion ==3:
        if key in AllowedOutputFormats: # AllowedOutputFormats is the entire list of output formats including ESD
            #if 'SLIM' in key or 'SMALL' in key or 'LARGE' in key : #keeping for refernece/potential revert
            # this keeps only the dynamic variables that have been specificied in TriggerEDMRun3
            return getRun3TrigEDMSlimList(key)

        else:
            log.warning('Output format: %s is not in list of allowed formats, please check!', key)
            return getRun3TrigObjList(key, [TriggerHLTListRun3])

    else:
        return getTriggerObjList(key,[TriggerL2List,TriggerEFList, TriggerResultsRun1List])



def getRun3TrigObjProducedInView(theKey, trigEDMList):
    """
    Run 3 only
    Finds a given key from within the trigEDMList.
    Returns true if this collection is produced inside EventViews
    (Hence, has the special viewIndex Aux decoration applied by steering)
    """
    import itertools
    for item in itertools.chain(*trigEDMList):
        if len(item) < 4:
            continue
        if theKey not in item[0]:
            continue
        return ("inViews" in item[3])
    return False


def handleRun3ViewContainers( el ):
    if 'Aux.' in el:
        # Get equivalent non-aux string (fragile!!!)
        keyNoAux = el.split('.')[0].replace('Aux','')
        # Check if this interface container is produced inside a View
        inView = getRun3TrigObjProducedInView(keyNoAux, [TriggerHLTListRun3])
        if el.split('.')[1] == '':
            # Aux lists zero dynamic vars to save ...
            if inView:
                # ... but it was produced in a View, so we need to add the viewIndex dynamic aux
                return el.split('.')[0]+'.viewIndex'
            else:
                # ... and was not in a View, strip all dynamic
                return el.split('.')[0]+'.-'
        else:
            # Aux lists one or more dynamic vars to save ...
            if inView:
                # ... and was produced in a View, so add the viewIndex dynamic as well
                return el+'.viewIndex'
            else:
                # ... and was not produced in a View, keep user-supplied list
                return el
    else: # no Aux
        return el


def getRun3BSList(keys):
    """
    The keys should contain BS and all the identifiers used for scouting
    """

    from TrigEDMConfig.TriggerEDMRun3 import persistent
    keys = set(keys[:])
    collections = []
    for definition in TriggerHLTListRun3:

        typename,collkey = definition[0].split("#")
        # normalise collection name and the key (decorations)
        typename = persistent(typename)
        collkey  = handleRun3ViewContainers( collkey )
        destination = keys & set(definition[1].split())
        if len(destination) > 0:
            collections.append( (typename+"#"+collkey, list(destination ) ) )
    return collections


def getRun3TrigObjList(destination, trigEDMList):
    """
    Run 3 version
    Gives back the Python dictionary  with the content of ESD/AOD (dst) which can be inserted in OKS.
    """
    dset = set(destination.split())
    from collections import OrderedDict
    toadd = OrderedDict()
    import itertools

    for item in itertools.chain(*trigEDMList):
        if item[1] == '': # no output has been defined
            continue

        confset = set(item[1].split())

        if dset & confset: # intersection of the sets
            t,k = getTypeAndKey(item[0])
            colltype = t

            if colltype in toadd:
                if k not in toadd[colltype]:
                    toadd[colltype] += [k]
            else:
                toadd[colltype] = [k]

    return toadd


def getRun3TrigEDMSlimList(key):
    """
    Run 3 version
    Modified EDM list to remove all dynamic variables
    Requires changing the list to have 'Aux.-'
    """
    _edmList = getRun3TrigObjList(key,[TriggerHLTListRun3])
    from collections import OrderedDict
    output = OrderedDict()
    for k,v in _edmList.items():
        newnames = []
        for el in v:
            newnames.append( handleRun3ViewContainers( el ) )
        output[k] = newnames
    return output

#************************************************************
#
#  For Run 1 and Run 2 (not modified (so far))
#
#************************************************************
def getTriggerEDMSlimList(key):
    """
    Run 2 version
    Modified EDM list to remove all dynamic variables
    Requires changing the list to have 'Aux.-'
    """
    _edmList = getTriggerObjList(key,[TriggerHLTList, TriggerResultsList])
    output = {}
    for k,v in _edmList.items():
        newnames = []
        for el in v:
            if 'Aux' in el:
                newnames+=[el.split('.')[0]+'.-']
            else:
                newnames+=[el]
            output[k] = newnames
    return output

def getCategory(s):
    """ From name of object in AOD/ESD found by checkFileTrigSize.py, return category """

    """ Clean up object name """
    s = s.strip()

    # To-do
    # seperate the first part of the string at the first '_'
    # search in EDMDetails for the key corresponding to the persistent value
    # if a key is found, use this as the first part of the original string
    # put the string back together

    if s.count('.') : s = s[:s.index('.')]
    if s.count('::'): s = s[s.index(':')+2:]
    if s.count('<'):  s = s[s.index('<')+1:]
    if s.count('>'):  s = s[:s.index('>')]
    if s.count('.') : s = s[:s.index('.')]
    if s.count('Dyn') : s = s[:s.index('Dyn')]

    # containers from Run 1-2 and 3 require different preprocessing
    # s12 is for Run 1-2, s is for Run 3
    s12 = s

    if s12.startswith('HLT_xAOD__') or s12.startswith('HLT_Rec__') or s12.startswith('HLT_Analysis__') :
        s12 = s12[s12.index('__')+2:]
        s12 = s12[s12.index('_')+1:]
        #if s12.count('.') : s12 = s12[:s12.index('.')]
        s12 = "HLT_"+s12
    elif s12.startswith('HLT_'):
        #if s.count('Dyn') : s = s[:s.index('Dyn')]
        if s12.count('_'): s12 = s12[s12.index('_')+1:]
        if s12.count('_'): s12 = s12[s12.index('_')+1:]
        s12 = "HLT_"+s12

    TriggerListRun1 = TriggerL2List + TriggerEFList + TriggerResultsRun1List
    TriggerListRun2 = TriggerResultsList + TriggerLvl1List + TriggerIDTruth + TriggerHLTList
    TriggerListRun3 = TriggerHLTListRun3

    category = ''
    bestMatch = ''

    """ Loop over all objects already defined in lists (and hopefully categorized!!) """
    for item in TriggerListRun1+TriggerListRun2:
        t,k = getTypeAndKey(item[0])

        """ Clean up type name """
        if t.count('::'): t = t[t.index(':')+2:]
        if t.count('<'):  t = t[t.index('<')+1:]
        if t.count('>'):  t = t[:t.index('>')]
        if (s12.startswith(t) and s12.endswith(k)) and (len(t) > len(bestMatch)):
            bestMatch = t
            category = item[2]

        if k.count('.'): k = k[:k.index('.')]
        if (s12 == k):
            bestMatch = k
            category = item[2]

    for item in TriggerListRun3:
        t,k = getTypeAndKey(item[0])

        """ Clean up type name """
        if t.count('::'): t = t[t.index(':')+2:]
        if t.count('<'):  t = t[t.index('<')+1:]
        if t.count('>'):  t = t[:t.index('>')]

        if (s.startswith(t) and s.endswith(k)) and (len(t) > len(bestMatch)):
            bestMatch = t
            category = item[2]

        if k.count('.'): k = k[:k.index('.')]
        if (s == k):
            bestMatch = k
            category = item[2]
    if category == '': return 'NOTFOUND'
    return category



def getTypeAndKey(s):
    """ From the strings containing type and key of trigger EDM extract type and key
    """
    return s[:s.index('#')], s[s.index('#')+1:]

def keyToLabel(key):
    """ The key is usually HLT_*, this function returns second part of it or empty string
    """
    if '_' not in key:
        return ''
    else:
        return key[key.index('_'):].lstrip('_')

def getTriggerObjList(destination, lst):
    """
    Gives back the Python dictionary  with the content of ESD/AOD (dst) which can be inserted in OKS.
    """
    dset = set(destination.split())

    toadd = {}
    import itertools

    for item in itertools.chain(*lst):
        if item[1] == '':
            continue
        confset = set(item[1].split())
        if dset & confset: # intersection of the sets
            t,k = getTypeAndKey(item[0])
            colltype = t
            if 'collection' in EDMDetails[t]:
                colltype = EDMDetails[t]['collection']
            if colltype in toadd:
                if k not in toadd[colltype]:
                    toadd[colltype] += [k]
            else:
                toadd[colltype] = [k]
    return InsertContainerNameForHLT(toadd)


def getTrigIDTruthList(dst):
    """
    Gives back the Python dictionary  with the truth trigger content of ESD/AOD (dst) which can be inserted in OKS.
    """
    return getTriggerObjList(dst,[TriggerIDTruth])

def getLvl1ESDList():
    """
    Gives back the Python dictionary  with the lvl1 trigger result content of ESD which can be inserted in OKS.
    """
    return getTriggerObjList('ESD',[TriggerLvl1List])

def getLvl1AODList():
    """
    Gives back the Python dictionary  with the lvl1 trigger result content of AOD which can be inserted in OKS.
    """
    return getTriggerObjList('AODFULL',[TriggerLvl1List])



def getL2PreregistrationList():
    """
    List (Literally Python list) of trigger objects to be preregistered i.e. this objects we want in every event for L2
    """
    l = []
    for item in TriggerL2List:
        if len (item[1]) == 0: continue
        t,k = getTypeAndKey(item[0])
        if('Aux' in t):
            continue #we don't wat to preregister Aux containers
        l += [t+"#"+keyToLabel(k)]
    return l

def getEFPreregistrationList():
    """
    List (Literally Python list) of trigger objects to be preregistered i.e. this objects we want in every event for EF
    """
    l = []
    for item in TriggerEFList:
        if len (item[1]) == 0: continue
        t,k = getTypeAndKey(item[0])
        if('Aux' in t):
            continue #we don't wat to preregister Aux containers
        l += [t+"#"+keyToLabel(k)]
    return l

def getHLTPreregistrationList():
    """
    List (Literally Python list) of trigger objects to be preregistered i.e. this objects we want in every event for merged L2/EF in addition to default L2 and EF
    """
    l = []
    for item in TriggerHLTList:
        if len (item[1]) == 0: continue
        t,k = getTypeAndKey(item[0])
        if('Aux' in t):
            continue #we don't wat to preregister Aux containers
        l += [t+"#"+keyToLabel(k)]
    return l


def getPreregistrationList(version=2):
    """
    List (Literally Python list) of trigger objects to be preregistered i.e. this objects we want for all levels
    version can be: '1 (Run1)', '2 (Run2)'
    """

    l=[]
    if version==2:
        l = getHLTPreregistrationList()
    else:
        l=list(set(getL2PreregistrationList()+getEFPreregistrationList()+getHLTPreregistrationList()))
    return l



def getEFDSList():
    """
    List (Literally Python list) of trigger objects to be placed in RAW data. i.e. BS after EF
    """
    l = []
    for item in TriggerEFList:
        if 'DS' in item[1].split():
            t,k = getTypeAndKey(item[0])
            l += [t+"#"+keyToLabel(k)]
    return l

def getHLTDSList():
    """
    List (Literally Python list) of trigger objects to be placed in RAW data. i.e. BS after merged L2EF
    """
    l = []
    for item in TriggerHLTList:
        if 'DS' in item[1].split():
            t,k = getTypeAndKey(item[0])
            l += [t+"#"+keyToLabel(k)]
    return l

def getL2BSList():
    """
    List (Literally Python list) of L2 trigger objects to be placed in output BS
    """
    l = []
    for item in TriggerL2List:
        if 'BS' in item[1]:
            t,k = getTypeAndKey(item[0])
            l += [t+"#"+keyToLabel(k)]
    return l

def getEFBSList():
    """
    List (Literally Python list) of EF trigger objects to be placed in output BS
    """
    l = []
    for item in TriggerEFList:
        if 'BS' in item[1]:
            t,k = getTypeAndKey(item[0])
            l += [t+"#"+keyToLabel(k)]
    return l

def getHLTBSList():
    """
    List (Literally Python list) of merged HLT trigger objects to be placed in output BS
    """
    l = []
    for item in TriggerHLTList:
        if 'BS' in item[1]:
            t,k = getTypeAndKey(item[0])
            l += [t+"#"+keyToLabel(k)]
    return l

def getL2BSTypeList():
    """ List of L2 types to be read from BS, used by the TP
    """
    l = []
    for item in TriggerL2List:
        t,k = getTypeAndKey(item[0])
        ctype = t
        if 'collection' in EDMDetails[t]:
            ctype = EDMDetails[t]['collection']
        l += [ctype]
    return l

def getEFBSTypeList():
    """ List of EF types to be read from BS, used by the TP
    """
    l = []
    for item in TriggerEFList:
        t,k = getTypeAndKey(item[0])
        ctype = t
        if 'collection' in EDMDetails[t]:
            ctype = EDMDetails[t]['collection']
        l += [ctype]
    return l

def getHLTBSTypeList():
    """ List of HLT types to be read from BS, used by the TP
    """
    l = []
    for item in TriggerHLTList:
        t,k = getTypeAndKey(item[0])
        ctype = t
        if 'collection' in EDMDetails[t]:
            ctype = EDMDetails[t]['collection']
        l += [ctype]
    return l

def getEFDSTypeList():
    """ List of types to be placed in BS after EF
    """
    l = []
    for item in TriggerEFList:
        if 'DS' in item[1].split():
            t,k = getTypeAndKey(item[0])
            ctype = t
            if 'collection' in EDMDetails[t]:
                ctype = EDMDetails[t]['collection']
            l += [ctype]
    return l

def getHLTDSTypeList():
    """ List of types to be placed in BS after L2EF
    """
    l = []
    for item in TriggerHLTList:
        if 'DS' in item[1].split():
            t,k = getTypeAndKey(item[0])
            ctype = t
            if 'collection' in EDMDetails[t]:
                ctype = EDMDetails[t]['collection']
            l += [ctype]
    return l


def getTPList(version=2):
    """
    Mapping  of Transient objects to Peristent during serialization (BS creation)
    version can be: '1 (Run1)', '2 (Run2)'
    """
    l = {}
    if version==2:
        bslist = getHLTBSTypeList()
    else:
        bslist = list(set(getL2BSTypeList() + getEFBSTypeList()))
        
    for t,d in six.iteritems (EDMDetails):
        colltype = t
        if 'collection' in d:
            colltype = EDMDetails[t]['collection']
        if colltype in bslist:
            l[colltype] = d['persistent']
    return l


#FPP: how to change this for the merged HLT in view of splitting?
def getARATypesRenaming():
    """
    Defines how to rename collecion keys in ARA when two types have the same key.
    i.e. TrigRoiDescriptorCollection#HLT
    and TrigTau#HLT
    After the remapping they will be named HLT_tau and HLT_roi so are distinct.
    """
    edm = set(TriggerL2List + TriggerEFList + TriggerHLTList)
    keys = [ getTypeAndKey(i[0])[1] for i in edm]
    # find repeating keys
    tmp = [ i for i in keys if keys.count(i) > 1 ]
    nonunique = {}
    for i in tmp:
        nonunique[i] = 1
    # nonunique = nonunique.keys()

    # define remens for all object of which key appeared in nonunique
    renames = {}
    for entry in edm:
        t, key = getTypeAndKey(entry[0])
        if key in nonunique: # potential problem we have to do something

            if 'typealias' not in EDMDetails[t] or EDMDetails[t]['typealias'] == '':
                if nonunique[key] == 1:
                    # First time's ok.
                    nonunique[key] = t
                elif nonunique[key] == t:
                    # Duplicate entry; ok.
                    continue
                else:
                    log.error("types/keys will catch %s %s", t, key)
                continue
            else:
                obj = t
                if 'collection' in EDMDetails[t]:
                    obj = EDMDetails[t]['collection']

                # form the branch name
                bname = key+'_'+EDMDetails[t]['typealias']

                renames[(key, obj)] = bname

    return renames

def getEDMLibraries():
    return EDMLibraries

def InsertContainerNameForHLT(typedict):
    import re
    output = {}
    for k,v in six.iteritems (typedict):
        newnames = []
        for el in v:
            if el.startswith('HLT_') or el == 'HLT':
                prefixAndLabel = el.split('_',1) #only split on first underscore
                containername = k if 'Aux' not in k else EDMDetails[k]['parent'] #we want the type in the Aux SG key to be the parent type #104811
                #maybe this is not needed anymore since we are now versionless with the CLIDs but it's not hurting either
                containername = re.sub('::','__',re.sub('_v[0-9]+$','',containername))
                newnames+=['_'.join([prefixAndLabel[0],containername]+([prefixAndLabel[1]] if len(prefixAndLabel) > 1 else []))]
            else:
                newnames+=[el]
            output[k] = newnames
    return output

def getEFRun1BSList():
    """
    List of EF trigger objects that were written to ByteStream in Run 1
    """
    l = []
    for item in TriggerEFEvolutionList:
        if len (item[1]) == 0: continue
        t,k = getTypeAndKey(item[0])
        l += [t+"#"+keyToLabel(k)]
    return l

def getEFRun2EquivalentList():
    """
    List of Run-2 containers equivalent to Run-1 EF containers
    """
    l = []
    for item in TriggerEFEvolutionList:
        if len (item[1]) == 0: continue
        t,k = getTypeAndKey(item[1])
        l += [t+"#"+keyToLabel(k)]
    return l

def getL2Run1BSList():
    """
    List of L2 trigger objects that were written to ByteStream in Run 1
    """
    l = []
    for item in TriggerL2EvolutionList:
        if len (item[1]) == 0: continue
        t,k = getTypeAndKey(item[0])
        l += [t+"#"+keyToLabel(k)]
    return l

def getL2Run2EquivalentList():
    """
    List of Run-2 containers equivalent to Run-1 L2 containers
    """
    l = []
    for item in TriggerL2EvolutionList:
        if len (item[1]) == 0: continue
        t,k = getTypeAndKey(item[1])
        l += [t+"#"+keyToLabel(k)]
    return l
