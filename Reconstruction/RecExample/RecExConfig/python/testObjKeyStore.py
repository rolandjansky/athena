# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import user
from RecExConfig.ObjKeyStore import ObjKeyStore, objKeyStore

# simple dictionary test
thed= { "JetCollection" : [ "Cone7Jets","another"], "TrucCluster" : "blah" , "RunOut" : "OfIdeas" }
print "thed:",thed
bla=thed["JetCollection"]

print bla

anotherd= { "JetCollection" : [ "Cone7Jets" ,"yetanother"], "autruche" : "runFast" }

print "anotherd:",anotherd

for k in anotherd.keys():
    if thed.has_key(k):
        for v in anotherd[k]:
            if not v in thed[k]: thed[k]+=[v]
            pass
        pass
    else:
        thed[k]=anotherd[k]
        pass
    pass

print "thed merged with anotherd:",thed
print thed

print ":"*80

from RecExConfig.RecConfFlags import jobproperties as jp
jp.RecConfFlags.AllowBackNavigation = True

## testing singleton-ness
m = ObjKeyStore()
b = ObjKeyStore()

assert m is b
assert m is ObjKeyStore()
assert m is objKeyStore

print "empty"
print ":"*80
m.Print()

print "now use addStreamESD"
print ":"*80

m.addStreamESD( 'McEventCollection', [ 'GEN_EVENT', 'GEN_AOD', 'TruthEvent' ] )
m.addStreamESD( 'JetCollection',
                [ 'Cone4Jets', 'Cone7Jets', 'KtJets',
                  'Cone4TruthJets' ] )
print "."*80
m.Print()

print "now merge in: addMany takes a dictionary as argument"
m.addManyTypesStreamESD(thed)
print "."*80
m.Print()
print "now merge in: addMany with a dictionary generated on the fly"
m.addManyTypesStreamAOD({'JetCollection' : [ 'Cone4Jets', 'Paraboloid' ] ,'Cacatoes': 'Yellow'})
print "."*80
m.Print()

print "."*80
hasT = m.isInStreamESD('t','*')
hasMc= m.isInStreamESD('McEventCollection','*')

print "t:", hasT
print "Mc:",hasMc
assert( not hasT  )
assert(     hasMc )

###print m.isInInput('McEventCollection')

print "MCGEN_EVENT:",m.isInStreamESD('McEventCollection','GEN_EVENT')
print "MCGEN_TRUC:",m.isInStreamESD('McEventCollection','GEN_TRUC')


print "."*80
m.addStreamAOD( 'ParticleJetContainer', 'ConeTowerParticleJets' )
m.Print()
print "."*80

print "### writing ESD stream..."
m.writeStreamESD('esd.cfg.py')

# first, create the file
import os
aodFile = open("aod.cfg.py", "w+")
aodFile.writelines( os.linesep.join( """
def _fill( item ):
    item.add( {
      'JetCollection' : ['Cone4Jets', 'Cone7Jets', 'KtJets', 'Cone4TruthJets'],
      'McEventCollection' : ['GEN_EVENT', 'GEN_AOD', 'TruthEvent']
    } )
    return
""".split(os.linesep) ) )
aodFile.close()
del aodFile

print "###DRDR reading AOD stream..."
m.readStreamESD('aod.cfg.py')
print "."*80
m.addDetector( 'A-metaux', [ 'CornoFulgur', 'AsteroHache', 'FulgurAuPoing' ] )

print "."*80
print objKeyStore
print "."*80

import os
if not os.path.exists("myrun"): os.mkdir( "myrun" )
myAodFile = open( "myrun/myAod.py", "w+" )
myAodFile.writelines( os.linesep.join( """
def _fill( item ):
    item.add( {
      'JetCollection' : ['Cone4Jets', 'Cone7Jets', 'KtJets', 'Cone4TruthJets'],
      'McEventCollection' : ['GEN_EVENT', 'GEN_AOD', 'TruthEvent' ],
      'TruthParticleContainer' : ['SpclMC'],
    } )
    return
""".split(os.linesep) ) )
myAodFile.close()
del myAodFile
objKeyStore.readStreamAOD("myrun/myAod.py")
assert( objKeyStore.isInStreamAOD('TruthParticleContainer','SpclMC') )

caught = False
try:
    objKeyStore['unallowedKey'] = range(10)
except KeyError, err:
    caught = True
    print "Caught: %s" % str(err)
    print "OK"
assert(caught)

caught = False
try:
    print objKeyStore['unallowedKey']
except KeyError, err:
    caught = True
    print "Caught: %s" % str(err)
    print "OK"
assert(caught)

caught = False
try:
    print "StreamAOD content:",objKeyStore['streamAOD']
except KeyError, err:
    caught = True
    print "Caught: %s" % str(err)
    print "ARGH!!"
assert(not caught)

## reset StreamTAG:
objKeyStore['streamTAG'] = {}
assert( not objKeyStore.isInStreamTAG( "JetCollection",     "Tag_Cone4Jets" ) )
assert( not objKeyStore.isInStreamTAG( "McEventCollection", "*" ) )

print "## testing the include of a 'config' file of objKeyStore..."
objKeyStore.readStreamTAG("RecExConfig/test_tag_from_share.py")
assert(     objKeyStore.isInStreamTAG( "JetCollection",     "Tag_Cone4Jets" ) )
assert( not objKeyStore.isInStreamTAG( "McEventCollection", "*" ) )
print "## testing the include of a 'config' file of objKeyStore... [OK]"

print ""
print "#"*80
print "## testing 'baseline' RecExCommon..."
print "#"*80

def objReset():
   from RecExConfig.ObjKeyStore import objKeyStore
   objKeyStore.clear()

print "## loading 'test_rdo'..."
objReset()
objKeyStore.readInputFile("RecExConfig/test_rdo.py")
assert( objKeyStore.isInInputFile('LArRawChannelContainer', 'LArRawChannels') )
print "## loading 'test_rdo'... [OK]"

print "## loading 'test_esd'..."
objReset()
objKeyStore.readInputBackNav("RecExConfig/test_rdo.py")
objKeyStore.readInputFile   ("RecExConfig/test_esd.py")
assert( not objKeyStore.isInInputFile('LArRawChannelContainer',
                                      'LArRawChannels') )
assert( objKeyStore.isInInputFile('CaloCellContainer', 'AllCalo' ) )
assert( objKeyStore.isInInputBackNav('LArRawChannelContainer',
                                     'LArRawChannels') )
print "## loading 'test_esd'... [OK]"

print "## loading 'test_aod'..."
objReset()
objKeyStore.readInputBackNav("RecExConfig/test_rdo.py")
objKeyStore.readInputBackNav("RecExConfig/test_esd.py")
objKeyStore.readInputFile   ("RecExConfig/test_aod.py")
assert( not objKeyStore.isInInputFile('LArRawChannelContainer',
                                      'LArRawChannels') )
assert( not objKeyStore.isInInputFile('CaloCellContainer', 'AllCalo' ) )
assert( objKeyStore.isInInputBackNav('LArRawChannelContainer',
                                     'LArRawChannels') )
assert( objKeyStore.isInInputBackNav('CaloCellContainer', 'AllCalo' ) )
assert( objKeyStore.isInInputFile('CaloCellContainer', 'AODCellContainer' ) )
print "## loading 'test_aod'... [OK]"
print "##"
print "## testing 'baseline' RecExCommon... [OK]"
print "#"*80
print objKeyStore

## import sys
## modules = sys.modules.keys()
## modules.sort()
## print "modules:",modules
