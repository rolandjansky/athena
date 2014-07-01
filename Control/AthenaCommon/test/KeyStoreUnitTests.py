#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Author: Sebastien Binet (binet@cern.ch)

"""Unit tests for verifying setting of CfgItemList and CfgKeyStore."""

import unittest, sys

from AthenaCommon.KeyStore import CfgItemList, CfgKeyStore
from AthenaCommon.KeyStore import logging, msg

### data ---------------------------------------------------------------------
__version__ = '$Revision: 1.10 $'
__author__  = 'Sebastien Binet (binet@cern.ch)'

__all__ = [
    'BasicCfgItemListTestCase',
    'BasicCfgKeyStoreTestCase',
    ]

### basic behaviour of CfgItemList -------------------------------------------
class BasicCfgItemListTestCase( unittest.TestCase ):
    """Verify basic behaviour of CfgItemList to be as expected"""

    def test1NamedSingleton( self ):
        """Test that instances w/ same name are the same"""

        i1 = CfgItemList( "MyItems" )
        i2 = CfgItemList( "MyItems" )
        self.failUnless( i1 == i2, "instances are not equal !" )
        self.failUnless( i1 is i2, "instances are not identical !" )

    def test2KwConstructor( self ):
        """Test keyword constructor"""
        i = CfgItemList( 'MyItems',
                         items = [ 'Klass1#klass1',
                                   'Klass2#klass2', ],
                         allowWildCard = False )
        self.failUnless( i.has_item( 'Klass1#klass1' ),
                         'Corrupted output item list !' )
        self.failUnless( i.has_item( 'Klass2#klass2' ),
                         'Corrupted output item list !' )
        self.failUnless( i.has_item( 'Klass1#*' ),
                         'Corrupted output item list !' )
        self.failUnless( i.has_item( 'Klass2#*' ),
                         'Corrupted output item list !' )
        self.failUnless( i.has_item( 'Klass1#klass*' ),
                         'Corrupted output item list !' )
        self.failUnless( i.has_item( 'Klass2#klass*' ),
                         'Corrupted output item list !' )
        self.failUnless( not i.has_item( 'Klass1#klass2' ),
                         'Corrupted output item list !' )
        self.failUnless( not i.has_item( 'Klass2#klass1' ),
                         'Corrupted output item list !' )
        self.failUnless( i._allowWildCard == False,
                         'Invalid "allowWildCard" state !' )
        self.failUnless( i() == [ 'Klass1#klass1', 'Klass2#klass2', ],
                         'Invalid output item list value !' )
        
        del i
        i = CfgItemList( 'MyItems',
                         items = [ 'Klass3#klass3',
                                   'Klass4#klass4', ],
                         allowWildCard = True )
        self.failUnless( i._allowWildCard == True,
                         'Invalid "allowWildCard" state !' )
        self.failUnless( i() == [ 'Klass3#klass3', 'Klass4#klass4', ],
                         'Invalid output item list value !' )

        del i
        msg.setLevel( logging.ERROR )
        i = CfgItemList( 'MyItems',
                         items = [ 'Klass3#*' ],
                         allowWildCard = False )
        self.failUnless( len(i()) == 0, "AllowWildCard does not work !" )

        del i
        i = CfgItemList( 'MyItems', items = [ 'Klass3#*' ] )
        self.failUnless( len(i()) == 0, "AllowWildCard does not work !" )
        
        del i
        i = CfgItemList( 'MyItems' )
        i.add( 'Klass3#*' )
        self.failUnless( len(i()) == 0, "AllowWildCard does not work !" )
        
    def test3Hierarchy( self ):
        """Test tree-like hierarchy structure of item lists"""
        i = CfgItemList( 'MyItems',
                         items = [ 'Klass1#klass1',
                                   'Klass2#klass2', ],
                         allowWildCard = False )
        ii = CfgItemList( 'MySubItems',
                          items = [ 'Klass3#klass3',
                                    'Klass4#klass4',
                                    'Klass5#klass5',
                                    'Klass6#klass6' ] )
        self.failUnless( len(i())  == 2,
                         "Invalid output item list !!" )
        self.failUnless( len(ii()) == 4,
                         "Invalid output item list !!" )

        i += ii
        self.failUnless( len(i())  == 6,
                         "Invalid output item list !!" )

        self.failUnless( hasattr(i, ii.name()),
                         'Invalid hierarchy structure !!' )
        oo = getattr(i, ii.name())
        self.failUnless( oo == ii,
                         'Invalid hierarchy structure !!' )
        self.failUnless( oo is ii,
                         'Invalid hierarchy structure !!' )
        self.failUnless( len(oo()) == 4,
                         "Invalid output item list !!" )
        
        del ii
        ii = CfgItemList( 'MySubItems' )
        self.failUnless( len(ii())  == 4,
                         "Invalid output item list !!" )

        del i,ii
        i  = CfgItemList( 'MyItems' )
        ii = CfgItemList( 'MySubItems' )
        self.failUnless( len( i())  == 0, "Invalid output item list !!" )
        self.failUnless( len(ii())  == 4, "Invalid output item list !!" )
        del oo,ii
        ii = CfgItemList( 'MySubItems' )
        self.failUnless( len(ii())  == 0, "Invalid output item list !!" )
        
    def test4Methods( self ):
        """Test various methods of CfgItemList"""
        i = CfgItemList( 'MyItems' )
        self.failUnless( i.name() == 'MyItems' )

        i.add( { 'Klass' : ['key1', 'key2'] } )
        self.failUnless( i() == [ 'Klass#key1', 'Klass#key2' ] )

        i.add( { 'Klass' : ['key1', 'key3' ] } )
        self.failUnless( i() == [ 'Klass#key1', 'Klass#key2', 'Klass#key3' ] )

        props = i.getProperties()
        self.failUnless( props.has_key( 'Klass' ) )
        self.failUnless( len(props['Klass']) == len(['key1','key2','key3']) )
        self.failUnless( 'key1' in props['Klass'] )
        self.failUnless( 'key2' in props['Klass'] )
        self.failUnless( 'key3' in props['Klass'] )

        self.failUnless( len(i.allChildren()) == 0 )

        self.failUnless(     i.has_item( 'Klass#key1' ) )
        self.failUnless(     i.has_item( 'Klass#key*' ) )
        self.failUnless( not i.has_item( 'Klass#Key1' ) )
        self.failUnless( not i.has_item( 'Klass#Key*' ) )

        i.clear()
        self.failUnless( len(i()) == 0 )
        
        i.add( { 'Klass' : ['key1', 'key2'] } )
        self.failUnless( len(i()) == 2 )
        i.remove( 'Klass' )
        self.failUnless( len(i()) == 2 )
        i.remove( 'Klass#key1' )
        self.failUnless( len(i()) == 2 )
        i.removeAll()
        self.failUnless( len(i()) == 2 )

        i.removeItem( 'Klass' )
        self.failUnless( len(i()) == 2 )
        i.removeItem( 'Klass#key1' )
        self.failUnless( len(i()) == 1 )

        _keys = ['key1', 'key2', 'key3']
        i.add( { 'Klass' : _keys } )
        self.failUnless( len(i()) == 3 )
        i.removeAllItems( 'Klass#key2' )
        self.failUnless( len(i()) == 2 )

        ## test we don't modify input dict
        d = {  'Klassy' : ['key1', 'key2', 'key3'] }
        orig_d = d.copy()
        i.add( d )
        self.failUnless( d == orig_d )

        d = {  'Klassy' : 'key4' }
        orig_d = d.copy()
        i.add( d )
        self.failUnless( d == orig_d )

        ## test extraction of the item list of a given Container type
        _keys = [ "Klassy#%s" % k for k in 'key1', 'key2', 'key3', 'key4' ]
        self.failUnless(      i(       "Klassy" ) == _keys )
        self.failUnless( i.list(       "Klassy" ) == _keys )
        self.failUnless(      i( key = "Klassy" ) == _keys )
        self.failUnless( i.list( key = "Klassy" ) == _keys )

        ## test dict()
        _keys = [ 'key1', 'key2', 'key3', 'key4' ]
        self.failUnless( i.dict()['Klass' ] == ['key1', 'key3'] )
        self.failUnless( i.dict()['Klassy'] == _keys )
        _dict = { 'Klass' : ['key1', 'key3'],
                  'Klassy': ['key1', 'key2', 'key3', 'key4']}
        self.failUnless( i.dict() == _dict )

    def test5WildCard( self ):
        """Test the wildcard capabilities of CfgItemList"""
        i = CfgItemList( 'MyItems' )
        self.failUnless( i.name() == 'MyItems' )
        
        ## test wildcard
        i.add( "Klass#Foo" )
        self.failUnless( i("Klass") == ["Klass#Foo"] )

        # ignored as not allowing wildcarded items
        i.add( "Klass#*" )
        self.failUnless( i("Klass") == ["Klass#Foo"] )

        # allowed as special cases for trigger
        i.clear()
        i.add( "TrigKlass#HLTAutoKey*" )
        self.failUnless( i("TrigKlass") == ["TrigKlass#HLTAutoKey*"] )
        
        i.clear()
        i.add( "TrigKlass#HLTAutoKey_*" )
        self.failUnless( i("TrigKlass") == ["TrigKlass#HLTAutoKey_*"] )
        
        i.clear()
        i.add( "TrigKlass#HLTAutoKey_foo_*" )
        self.failUnless( i("TrigKlass") == ["TrigKlass#HLTAutoKey_foo_*"] )
        
        i.clear()
        i.add( "TrigKlass#HLTAutoKey_foo_42_*" )
        self.failUnless( i("TrigKlass") == ["TrigKlass#HLTAutoKey_foo_42_*"] )
        
### basic behaviour of CfgKeyStore -------------------------------------------
class BasicCfgKeyStoreTestCase( unittest.TestCase ):
    """Verify basic behaviour of CfgKeyStore to be as expected"""

    def test1NamedSingleton( self ):
        """Test that instances w/ same name are the same"""
        self.failUnless( len(CfgKeyStore.instances.keys()) == 0 )
        self.failUnless( len(CfgItemList.instances.keys()) == 0 )

        i1 = CfgKeyStore( "MyStore" )
        i2 = CfgKeyStore( "MyStore" )
        self.failUnless( i1 == i2 )
        self.failUnless( i1 is i2 )
        self.failUnless( i1 !=     CfgKeyStore( "mystore" ) )
        self.failUnless( i1 is not CfgKeyStore( "mystore" ) )

        self.failUnless( len(CfgKeyStore.instances.keys()) == 1 )
        self.failUnless( len(CfgItemList.instances.keys()) ==
                         len(CfgKeyStore.__slots__['Labels'])+1 )

        del i1
        self.failUnless( len(CfgKeyStore.instances.keys()) == 1 )
        self.failUnless( len(CfgItemList.instances.keys()) ==
                         len(CfgKeyStore.__slots__['Labels'])+1 )

        del i2
        self.failUnless( len(CfgKeyStore.instances.keys()) == 0 )
        self.failUnless( len(CfgItemList.instances.keys()) == 0 )
        
    def test2Constructor( self ):
        """Test constructor"""
        ks = CfgKeyStore( "MyStore" )

        self.failUnless( ks.name() == "MyStore" )
        self.failUnless( len(ks.keys()) == len(ks._items)+4 )
        for label in ks.keys():
            if label == 'transient': n = 4 # nbr of streamXyz
            else : n = 0
            self.failUnless( len(ks[label])        == n )
            self.failUnless( len(ks[label].list()) == 0 )
            self.failUnless( hasattr(ks, label) )

        ks.streamAOD.add( { 'Klass' : 'key1' } )
        self.failUnless( ks.streamAOD() == [ 'Klass#key1' ] )
        del ks

        ks = CfgKeyStore( "MyStore" )
        self.failUnless( ks.streamAOD() == [] )
        
        pass

    def test3Methods( self ):
        """Test various methods of CfgKeyStore"""
        ks = CfgKeyStore( "MyStore" )
        self.failUnless( ks.streamESD()    == [] )
        self.failUnless( ks['streamESD']() == [] )
        ks.streamESD.add( {
            'McEventCollection' : [ 'GEN_EVENT', 'GEN_AOD', 'TruthEvent' ],
            } )
        self.failUnless( len(ks.streamESD()) == 3 )
        self.failUnless( len(ks.transient()) == 3 )
        self.failUnless(ks.streamESD.has_item('McEventCollection#TruthEvent'))
        self.failUnless(not ks.streamESD.has_item('McEventCollection#gen_aod'))

        ks.streamAOD += CfgItemList( 'subAod',
                                     items = [ 'Klass#key1', 'Klass#key2' ] )
        self.failUnless( len(ks.streamAOD()) == 2 )
        self.failUnless( len(ks.transient()) == 5 )

        ks.streamESD.add( {
            'JetCollection' : [ 'Cone4Jets', 'Cone7Jets', 'KtJets',
                                'Cone4TruthJets' ],
            } )
        self.failUnless( len(ks.streamESD()) == 3+4 )
        self.failUnless( len(ks.streamESD.dict().keys()) == 2 )
        self.failUnless( len(ks.transient.dict().keys()) == 3 )
        
        ks.streamESD.add( {
            'JetCollection' : [ 'Cone4Jets', 'Cone7Jets', 'KtJets' ] } )
        self.failUnless( len(ks.streamESD()) == 3+4 )
        self.failUnless( len(ks.streamESD.dict().keys()) == 2 )
        self.failUnless( len(ks.transient.dict().keys()) == 3 )

        ks.streamAOD.add( 'ParticleJetContainer#ConeTowerParticleJets' )
        self.failUnless( len(ks.streamAOD())             == 3 )
        self.failUnless( len(ks.streamAOD.dict().keys()) == 2 )
        self.failUnless( len(ks.transient.dict().keys()) == 4 )

        ks.streamAOD.add( ['ParticleJetContainer#ConeTowerParticleJets'] )
        self.failUnless( len(ks.streamAOD())             == 3 )
        self.failUnless( len(ks.streamAOD.dict().keys()) == 2 )
        self.failUnless( len(ks.transient.dict().keys()) == 4 )
        
        ks.streamAOD.add( ['ParticleJetContainer#Cone4TowerParticleJets'] )
        self.failUnless( len(ks.streamAOD())             == 4 )
        self.failUnless( len(ks.streamAOD.dict().keys()) == 2 )
        self.failUnless( len(ks.transient.dict().keys()) == 4 )

        caught = False
        try:
            ks['unallowedKey'] = range(10)
        except KeyError, err:
            caught = True
        self.failUnless( caught )

        caught = False
        try:                  dummy = ks['unallowedKey']
        except KeyError, err: caught = True
        self.failUnless( caught )

        caught = False
        try:                  dummy = ks['streamAOD']
        except KeyError, err: caught = True
        self.failUnless( not caught )
        del dummy

        ## test reset...
        ks.streamTAG.add( 'JetCollection#Tag_Cone4Jets' )
        self.failUnless( len(ks.streamTAG()) == 1 )
        ks.streamTAG.clear()
        self.failUnless( len(ks.streamTAG()) == 0 )

        self.failUnless( len(ks.streamAOD.dict().keys()) == 2 )
        self.failUnless( len(ks.transient.dict().keys()) == 4 )

        ks.transient.clear()
        self.failUnless( len(ks.transient.dict().keys()) == 0 )
        ## might be a bit confusing but 'clear' also removes children
        caught = False
        try:                        dummy = ks.streamAOD
        except AttributeError, err: caught = True
        self.failUnless( caught )

        self.failUnless( len(CfgKeyStore.instances.keys()) == 1 )

        del ks
        ## FIXME
        ## ARGH!!! Somebody is keeping a ref toward ks!
        self.failUnless( len(CfgKeyStore.instances.keys()) == 1 )

##         ks = CfgKeyStore( "MyStore" )

##         self.failUnless( ks.streamESD()    == [] )
##         self.failUnless( ks['streamESD']() == [] )
##         ks.streamESD.add( {
##             'McEventCollection' : [ 'GEN_EVENT', 'GEN_AOD', 'TruthEvent' ],
##             } )
##         self.failUnless( len(ks['streamESD']()) == 3 )
##         self.failUnless( len(ks.transient.dict().keys()) == 1 )

##         ks.transient.removeItem( 'McEventCollection#GEN_EVENT' )
##         self.failUnless( len(ks['streamESD']()) == 2 )
##         self.failUnless( len(ks.transient.dict().keys()) == 1 )
        
    def test4Persistency( self ):
        """Test persistency of CfgKeyStore"""
        ks = CfgKeyStore( "MyStore" )
        self.failUnless( ks.streamESD() == [] )
        self.failUnless( ks.streamAOD() == [] )
        self.failUnless( ks.transient() == [] )
        ks.streamESD.add( {
            'McEventCollection' : [ 'GEN_EVENT', 'GEN_AOD', 'TruthEvent' ],
            } )
        self.failUnless( len(ks.streamESD()) == 3 )
        self.failUnless( len(ks.streamAOD()) == 0 )
        self.failUnless( len(ks.transient()) == 3 )

        import os
        outFileName = 'esd_%s.py' % str(os.getpid())
        ks.write( outFileName, 'streamESD' )

        del ks
        ks = CfgKeyStore( "MyStore" )
        self.failUnless( ks.streamESD() == [] )
        self.failUnless( ks.streamAOD() == [] )
        self.failUnless( ks.transient() == [] )

        ks.read( outFileName, 'streamAOD' )
        self.failUnless( len(ks.streamESD()) == 0 )
        self.failUnless( len(ks.streamAOD()) == 3 )
        self.failUnless( len(ks.transient()) == 3 )

        del ks
        ks = CfgKeyStore( "MyStore" )
        self.failUnless( ks.streamESD() == [] )
        self.failUnless( ks.streamAOD() == [] )
        self.failUnless( ks.transient() == [] )

        ks.read( outFileName, 'streamAOD' )
        self.failUnless( len(ks.streamESD()) == 0 )
        self.failUnless( len(ks.streamAOD()) == 3 )
        self.failUnless( len(ks.transient()) == 3 )

        del ks
        ks = CfgKeyStore( "MyStore" )
        self.failUnless( ks.streamESD() == [] )
        self.failUnless( ks.streamAOD() == [] )
        self.failUnless( ks.transient() == [] )

        ks.read( outFileName, 'streamESD' )
        self.failUnless( len(ks.streamESD()) == 3 )
        self.failUnless( len(ks.streamAOD()) == 0 )
        self.failUnless( len(ks.transient()) == 3 )

        del ks
        ks = CfgKeyStore( "MyStore" )
        self.failUnless( ks.streamESD() == [] )
        self.failUnless( ks.streamAOD() == [] )
        self.failUnless( ks.transient() == [] )

        ks.read( outFileName, 'streamESD' )
        ks.read( outFileName, 'streamAOD' )
        self.failUnless( len(ks.streamESD()) == 3 )
        self.failUnless( len(ks.streamAOD()) == 3 )
        self.failUnless( len(ks.transient()) == 3 )

        os.remove( outFileName )
        os.remove( outFileName+'c' )

        import shelve
        outFileName = 'esd_%s.dat' % str(os.getpid())

        del ks
        ks = CfgKeyStore( "MyStore" )
        ks.streamESD.add( {
            'McEventCollection' : [ 'GEN_EVENT', 'GEN_AOD', 'TruthEvent' ],
            } )
        self.failUnless( len(ks.streamESD()) == 3 )
        self.failUnless( len(ks.streamAOD()) == 0 )
        self.failUnless( len(ks.transient()) == 3 )

        print 'outFileName:',outFileName
        db = shelve.open( outFileName, 'c' )
        db['store'] = ks
        db.close()
        
        del ks
        db = shelve.open( outFileName, 'r' )
        ks = db['store']
        db.close()

        self.failUnless( len(ks.streamESD()) == 3 )
        self.failUnless( len(ks.streamAOD()) == 0 )
        self.failUnless( len(ks.transient()) == 3 )
        ## MacOSX silently appends the .db suffix to shelve files. I haven't found a
        ## mechanism for determining the actual filename, which is why I've added the
        ## MacOSX-specific hack.
        from sys import platform
        if platform != 'darwin' :
            os.remove( outFileName )
        else:
            os.remove( outFileName+'.db')

    def test5LoadKeyStoreFromPoolFile( self ):
        """Test loading a CfgKeyStore from a POOL file"""
        from AthenaCommon.KeyStore import loadKeyStoreFromPoolFile
        ## FIXME
        ## Can't run that test as the content of that file lives in AtlasEvent
        ## --> skip it for now
        return
        import os
        pool_file = os.sep.join(["/afs/cern.ch/atlas/offline/test",
                                 "atlfast.aod.herwig_rel13.pool"])
        ks = loadKeyStoreFromPoolFile(keyStore='test_loadFromPoolFile',
                                      pool_file=pool_file,
                                      label='inputFile')

        pool_file_content = [
            'Analysis::MuonContainer#AtlfastMuonCollection',
            'Analysis::MuonContainer#AtlfastNonIsoMuonCollection',
            'Analysis::TauJetContainer#AtlfastTauJet1p3pContainer',
            'Analysis::TauJetContainer#AtlfastTauJetContainer',
            'DataHeader#OutStream',
            'ElectronContainer#AtlfastElectronCollection',
            'EventInfo#McEventInfo',
            'INav4MomAssocs#AtlfastMcAodAssocs',
            'INav4MomAssocs#DeltaRAssocs',
            'McEventCollection#GEN_AOD',
            'MissingET#AtlfastMissingEt',
            'ParticleJetContainer#AtlfastParticleJetContainer',
            'PhotonContainer#AtlfastPhotonCollection',
            'TruthParticleContainer#SpclMC']
        self.failUnless(sorted(ks.inputFile()) == sorted(pool_file_content))
        self.failUnless(len(ks.inputFile.dict().keys()) == 11)

        del ks
        return
    
    def test6KeyStoreDiff( self ):
        """Test keystore_diff"""
        from AthenaCommon.KeyStore import keystore_diff
        
        # reference
        ref = CfgKeyStore( "ref" )
        self.failUnless( ref.streamESD() == [] )
        self.failUnless( ref.streamAOD() == [] )
        self.failUnless( ref.transient() == [] )
        ref.streamESD.add( {
            'McEvent' : [ 'GEN_EVENT', 'GEN_AOD', 'TruthEvent' ],
            'NoEvent' : [ 'GEN_EVENT', 'GEN_AOD', 'TruthEvent' ],
            } )
        self.failUnless( len(ref.streamESD()) == 6 )
        self.failUnless( len(ref.streamAOD()) == 0 )
        self.failUnless( len(ref.transient()) == 6 )

        # check
        chk = CfgKeyStore( "chk" )
        self.failUnless( chk.streamESD() == [] )
        self.failUnless( chk.streamAOD() == [] )
        self.failUnless( chk.transient() == [] )
        chk.streamESD.add( {
            'NoEvent' : [ 'GEN_EVENT', 'TruthEvent' ],
            } )
        self.failUnless( len(chk.streamESD()) == 2 )
        self.failUnless( len(chk.streamAOD()) == 0 )
        self.failUnless( len(chk.transient()) == 2 )

        diff_ref = "".join ([
            "- len(ref[streamESD]) == 6",
            "+ len(chk[streamESD]) == 2",
            "- ref[streamESD] : McEvent#GEN_AOD",
            "- ref[streamESD] : McEvent#GEN_EVENT",
            "- ref[streamESD] : McEvent#TruthEvent",
            "- ref[streamESD] : NoEvent#GEN_AOD",
            ""
            ])

        import os,atexit
        
        test1_fname = 'test1_%s.diff' % str(os.getpid())
        otest1 = open (test1_fname, 'w')
        atexit.register (os.unlink, test1_fname)
        diff = keystore_diff (ref, chk, labels='streamESD', ofile=otest1)
        self.failUnless (diff==diff_ref)
        self.failUnless (os.path.exists (test1_fname))
        diff = keystore_diff ("ref", "chk", labels='streamESD', ofile=otest1)
        self.failUnless (diff==diff_ref)
        self.failUnless (os.path.exists (test1_fname))
        

        test2_fname = 'test2_%s.diff' % str(os.getpid())
        otest2 = open (test2_fname, 'w')
        atexit.register (os.unlink, test2_fname)
        diff_ref = "".join ([
            "- len(ref[transient]) == 6",
            "+ len(chk[transient]) == 2",
            "- ref[transient] : McEvent#GEN_AOD",
            "- ref[transient] : McEvent#GEN_EVENT",
            "- ref[transient] : McEvent#TruthEvent",
            "- ref[transient] : NoEvent#GEN_AOD",
            "- len(ref[streamESD]) == 6",
            "+ len(chk[streamESD]) == 2",
            "- ref[streamESD] : McEvent#GEN_AOD",
            "- ref[streamESD] : McEvent#GEN_EVENT",
            "- ref[streamESD] : McEvent#TruthEvent",
            "- ref[streamESD] : NoEvent#GEN_AOD",
            ""
            ])
        diff = keystore_diff (ref, chk, ofile=otest2)
        self.failUnless (diff==diff_ref)
        self.failUnless (os.path.exists (test2_fname))
        diff = keystore_diff ("ref", "chk", ofile=otest2)
        self.failUnless (diff==diff_ref)
        self.failUnless (os.path.exists (test2_fname))

        test3_fname = 'test3_%s.diff' % str(os.getpid())
        otest3 = open (test3_fname, 'w')
        atexit.register (os.unlink, test3_fname)
        diff = keystore_diff (ref, ref, otest3)
        self.failUnless (diff=='')
        self.failUnless (os.path.exists (test3_fname))
        diff = keystore_diff ("ref", "ref", otest3)
        self.failUnless (diff=='')
        
        ## valid arguments...
        err_msg = None
        test4_fname = 'test4_%s.diff' % str(os.getpid())
        otest4 = open (test4_fname, 'w')
        atexit.register (os.unlink, test4_fname)
        try:
            diff = keystore_diff ("ref", "typo-chk", ofile=otest4)
        except ValueError, err:
            err_msg = str(err)
        ref_errmsg = "invalid `chk` argument (non existing instance name [typo-chk])"
        self.failUnless (err_msg == ref_errmsg)

        #
        err_msg = None
        test5_fname = 'test5_%s.diff' % str(os.getpid())
        otest5 = open (test5_fname, 'w')
        atexit.register (os.unlink, test5_fname)
        try:
            diff = keystore_diff ("typo-ref", "chk", ofile=otest4)
        except ValueError, err:
            err_msg = str(err)
        ref_errmsg = "invalid `ref` argument (non existing instance name [typo-ref])"
        self.failUnless (err_msg == ref_errmsg)
        
        return # test6
    
## actual test run
if __name__ == '__main__':
   loader = unittest.TestLoader()
   testSuite = loader.loadTestsFromModule( sys.modules[ __name__ ] )

   runner = unittest.TextTestRunner( verbosity = 2 )
   result = not runner.run( testSuite ).wasSuccessful()

   sys.exit( result )
