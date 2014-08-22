#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Unit test for module MuonRecFlags.py"""

from MuonRecExample.MuonRecUtils import RecConfigInfo
import UnitTest # from current test directory
import os
import __main__

from AthenaCommon.Logging import logging

log = logging.getLogger("MuonRecFlags_test.py")
log.setLevel(logging.DEBUG)

oldMuonboyFlags =  { 'doMuonboy' : 'Muonboy' }
oldMooreFlags = { 'doStandardMoore' : 'Moore.StandardMoore' ,
                  'doMoMuMoore'     : 'Moore.MoMuMoore' ,
                  'doCsc4dMoore'    : 'Moore.Csc4dMoore' , 
                  'doMoMu'          : 'Moore.MoMu' }

oldRecoFlags = { 'doMoore' : 'Moore' }
oldRecoFlags.update(oldMooreFlags)
oldRecoFlags.update(oldMuonboyFlags)

class MuonRecFlagsTestCase(UnitTest.TestCase):
    def setUp(self):
        UnitTest.TestCase.setUp(self)
        # remove all old moore flags
        import __main__
        self.removeOldFlags()
        from MuonRecExample import MuonRecFlags
        self.MuonRecFlags  = MuonRecFlags
        self.muonRecFlags  = MuonRecFlags.muonRecFlags
        MuonRecFlags.log.setLevel(logging.DEBUG)

    def removeOldFlags(self):
        global __main__,oldRecoFlags
        for f in oldRecoFlags:
            try:
                delattr(__main__,f)
            except AttributeError:
                pass


    def setOldFlags(self,**flagList):
        global __main__
        for n,v in flagList.items():
            setattr(__main__,n,v)


    def getOldFlag(self,name):
        global __main__
        return getattr(__main__,name,None)

    def getFlagsString(self):
        oldFlags = {}
        for f in oldRecoFlags:
            try:
                oldFlags[f] = getattr(__main__,f)
            except AttributeError:
                pass
        return ', '.join( [ '%s=%s' % (n,v) for n,v in oldFlags.items() ] )



class SelectRecoTestCase(MuonRecFlagsTestCase):
    """Tests of the muonRecFlags.SelectReco"""
    def setUp(self):
        MuonRecFlagsTestCase.setUp(self)
        self.SelectReco = self.muonRecFlags.SelectReco
        
    def assertSelectReco(self,value,message):
        # get combination of flags for error message
        global __main__
        shortSelected = [ RecConfigInfo(s).shortTag for s in self.SelectReco() ]
        shortExpected = [ RecConfigInfo(s).shortTag for s in value ]
        message += os.linesep + \
                   'Expected new flags: %r' % (shortExpected,) + os.linesep + \
                   '     Got new flags: %r' % (shortSelected,)

        self.assertEqual( shortSelected, shortExpected, message )


    def assertNewFlagsOK(self,enableMoore,enableMuonboy,outFlags,**inFlags):
        global oldRecoFlags
        self.SelectReco._doNewToOldFlags = False
        self.MuonRecFlags.resetOriginalState()
        self.removeOldFlags()
        self.setOldFlags( **inFlags )
        self.MuonRecFlags.doNewStyleMooreConfig = enableMoore
        self.MuonRecFlags.doNewStyleMuonboyConfig = enableMuonboy
        inputOldFlags = "newStyleMuonboy=%r newStyleMoore=%r %s" % (enableMuonboy, enableMoore, self.getFlagsString())
        self.SelectReco.set_Off()
        inputNewFlags = None
        self.SelectReco.oldToNewFlags()
        message = os.linesep + \
                  '   Input new flags: %r' % (inputNewFlags,) + os.linesep + \
                  '   Input old flags: %s' % (inputOldFlags,)
        self.assertSelectReco( outFlags, message )
        self.SelectReco.set_On()
        inputNewFlags = self.SelectReco.get_Value()
        self.SelectReco.oldToNewFlags()
        message = os.linesep + \
                  '   Input new flags: %r' % (inputNewFlags,) + os.linesep + \
                  '   Input old flags: %s' % (inputOldFlags,)
        self.assertSelectReco( outFlags, message )


    def testAllOff(self):
        """All Muon Reco off"""
        self.assertNewFlagsOK( False, False, [] )
        self.assertNewFlagsOK( False, False, [], doMoore=False, doMuonboy=False )
        self.assertNewFlagsOK( False, False, [], doMoore=True,  doMuonboy=True  )
        self.assertNewFlagsOK( True,  True,  [], doMoore=False, doMuonboy=False )
        self.assertNewFlagsOK( False, True,  [], doMoore=False, doMuonboy=False )
        self.assertNewFlagsOK( False, True,  [], doMoore=True,  doMuonboy=False )
        self.assertNewFlagsOK( True,  False, [], doMoore=False, doMuonboy=False )
        self.assertNewFlagsOK( True,  False, [], doMoore=False, doMuonboy=True  )
        self.assertNewFlagsOK( False, False, [], doMoore=True,  doMuonboy=True  )
        self.assertNewFlagsOK( False, False, [], doMoore=True,  doStandardMoore=True  )
        self.assertNewFlagsOK( True,  False, [], doMoore=False, doStandardMoore=True  )
        self.assertNewFlagsOK( False, False, [], doMoore=True,  doMoMuMoore=True  )
        self.assertNewFlagsOK( True,  False, [], doMoore=False, doMoMuMoore=True  )


    def testMuonboyOldMoore(self):
        """Muonboy with old-style Moore"""
        self.assertNewFlagsOK( False, True, ['Muonboy'] )
        self.assertNewFlagsOK( False, True, ['Muonboy'], doMuonboy=True )
        self.assertNewFlagsOK( False, True, ['Muonboy'], doMuonboy=True, doMoore=True )
        self.assertNewFlagsOK( False, True, ['Muonboy'], doMuonboy=True, doStandardMoore=True )
        self.assertNewFlagsOK( False, True, ['Muonboy'], doMuonboy=True, doStandardMoore=True, doMoore=True )


    def testDefaultMooreOldMuonboy(self):
        """Default Moore with new-style Muonboy"""
        # no doMuonboy defined
        self.assertNewFlagsOK( True, False, ['Moore'], # from default
                            doMoore=True )
        # doMuonboy=False
        self.assertNewFlagsOK( True, False, ['Moore'],
                            doMoore=True, doMuonboy=False )
        # doMuonboy=True
        self.assertNewFlagsOK( True, False, ['Moore'],
                            doMoore=True, doMuonboy=True )

 
    def testDefaultMooreNewMuonboy(self):
        """Default Moore with new-style Muonboy"""
        # no doMuonboy defined
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore'], # from default
                            doMoore=True )
        # doMuonboy=False
        self.assertNewFlagsOK( True, True, ['Moore'],
                            doMoore=True, doMuonboy=False )
        # doMuonboy=True
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore'],
                            doMoore=True, doMuonboy=True )

 
    def testStandardMooreOldMuonboy(self):
        """StandardMoore with old-style Muonboy"""
        # no doMuonboy defined
        self.assertNewFlagsOK( True, False, ['Moore.StandardMoore'],
                            doStandardMoore=True )
        self.assertNewFlagsOK( True, False, ['Moore.StandardMoore'],
                            doStandardMoore=True, doMoore = True )
        self.assertNewFlagsOK( True, False, [],
                            doStandardMoore=True, doMoore = False )
        # doMuonboy=False
        self.assertNewFlagsOK( True, False, ['Moore.StandardMoore'],
                            doStandardMoore=True, doMuonboy=False )
        self.assertNewFlagsOK( True, False, ['Moore.StandardMoore'],
                            doStandardMoore=True, doMuonboy=False, doMoore=True )
        self.assertNewFlagsOK( True, False, [],
                            doStandardMoore=True, doMuonboy=False, doMoore=False )
        # doMuonboy=True
        self.assertNewFlagsOK( True, False, ['Moore.StandardMoore'],
                            doStandardMoore=True, doMuonboy=True )
        self.assertNewFlagsOK( True, False, ['Moore.StandardMoore'],
                            doStandardMoore=True, doMuonboy=True, doMoore=True )
        self.assertNewFlagsOK( True, False, [],
                            doStandardMoore=True, doMuonboy=True, doMoore=False )


    def testStandardMooreNewMuonboy(self):
        """StandardMoore with new-style Muonboy"""
        # no doMuonboy defined
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.StandardMoore'],
                            doStandardMoore=True )
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.StandardMoore'],
                            doStandardMoore=True, doMoore = True )
        self.assertNewFlagsOK( True, True, ['Muonboy'],
                            doStandardMoore=True, doMoore = False )
        # doMuonboy=False
        self.assertNewFlagsOK( True, True, ['Moore.StandardMoore'],
                            doStandardMoore=True, doMuonboy=False )
        self.assertNewFlagsOK( True, True, ['Moore.StandardMoore'],
                            doStandardMoore=True, doMuonboy=False, doMoore=True )
        self.assertNewFlagsOK( True, True, [],
                            doStandardMoore=True, doMuonboy=False, doMoore=False )
        # doMuonboy=True
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.StandardMoore'],
                            doStandardMoore=True, doMuonboy=True )
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.StandardMoore'],
                            doStandardMoore=True, doMuonboy=True, doMoore=True )
        self.assertNewFlagsOK( True, True, ['Muonboy'],
                            doStandardMoore=True, doMuonboy=True, doMoore=False )


    def testMoMuMooreOldMuonboy(self):
        """MoMuMoore with old-style Muonboy"""
        # no doMuonboy defined
        self.assertNewFlagsOK( True, False, ['Moore.MoMuMoore'],
                            doMoMuMoore=True )
        self.assertNewFlagsOK( True, False, ['Moore.MoMuMoore'],
                            doMoMuMoore=True, doMoore = True )
        self.assertNewFlagsOK( True, False, [],
                            doMoMuMoore=True, doMoore = False )
        # doMuonboy=False
        self.assertNewFlagsOK( True, False, ['Moore.MoMuMoore'],
                            doMoMuMoore=True, doMuonboy=False )
        self.assertNewFlagsOK( True, False, ['Moore.MoMuMoore'],
                            doMoMuMoore=True, doMuonboy=False, doMoore=True )
        self.assertNewFlagsOK( True, False, [],
                            doMoMuMoore=True, doMuonboy=False, doMoore=False )
        # doMuonboy=True
        self.assertNewFlagsOK( True, False, ['Moore.MoMuMoore'],
                            doMoMuMoore=True, doMuonboy=True )
        self.assertNewFlagsOK( True, False, ['Moore.MoMuMoore'],
                            doMoMuMoore=True, doMuonboy=True, doMoore=True )
        self.assertNewFlagsOK( True, False, [],
                            doMoMuMoore=True, doMuonboy=True, doMoore=False )


    def testMoMuMooreNewMuonboy(self):
        """MoMuMoore with new-style Muonboy"""
        # no doMuonboy defined
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.MoMuMoore'],
                            doMoMuMoore=True )
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.MoMuMoore'],
                            doMoMuMoore=True, doMoore = True )
        self.assertNewFlagsOK( True, True, ['Muonboy'],
                            doMoMuMoore=True, doMoore = False )
        # doMuonboy=False
        self.assertNewFlagsOK( True, True, ['Moore.MoMuMoore'],
                            doMoMuMoore=True, doMuonboy=False )
        self.assertNewFlagsOK( True, True, ['Moore.MoMuMoore'],
                            doMoMuMoore=True, doMuonboy=False, doMoore=True )
        self.assertNewFlagsOK( True, True, [],
                            doMoMuMoore=True, doMuonboy=False, doMoore=False )
        # doMuonboy=True
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.MoMuMoore'],
                            doMoMuMoore=True, doMuonboy=True )
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.MoMuMoore'],
                            doMoMuMoore=True, doMuonboy=True, doMoore=True )
        self.assertNewFlagsOK( True, True, ['Muonboy'],
                            doMoMuMoore=True, doMuonboy=True, doMoore=False )


    def testCsc4dMooreOldMuonboy(self):
        """Csc4dMoore with old-style Muonboy"""
        # no doMuonboy defined
        self.assertNewFlagsOK( True, False, ['Moore.Csc4dMoore'],
                            doCsc4dMoore=True )
        self.assertNewFlagsOK( True, False, ['Moore.Csc4dMoore'],
                            doCsc4dMoore=True, doMoore = True )
        self.assertNewFlagsOK( True, False, [],
                            doCsc4dMoore=True, doMoore = False )
        # doMuonboy=False
        self.assertNewFlagsOK( True, False, ['Moore.Csc4dMoore'],
                            doCsc4dMoore=True, doMuonboy=False )
        self.assertNewFlagsOK( True, False, ['Moore.Csc4dMoore'],
                            doCsc4dMoore=True, doMuonboy=False, doMoore=True )
        self.assertNewFlagsOK( True, False, [],
                            doCsc4dMoore=True, doMuonboy=False, doMoore=False )
        # doMuonboy=True
        self.assertNewFlagsOK( True, False, ['Moore.Csc4dMoore'],
                            doCsc4dMoore=True, doMuonboy=True )
        self.assertNewFlagsOK( True, False, ['Moore.Csc4dMoore'],
                            doCsc4dMoore=True, doMuonboy=True, doMoore=True )
        self.assertNewFlagsOK( True, False, [],
                            doCsc4dMoore=True, doMuonboy=True, doMoore=False )


    def testCsc4dMooreNewMuonboy(self):
        """Csc4dMoore with new-style Muonboy"""
        # no doMuonboy defined
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.Csc4dMoore'],
                            doCsc4dMoore=True )
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.Csc4dMoore'],
                            doCsc4dMoore=True, doMoore = True )
        self.assertNewFlagsOK( True, True, ['Muonboy'],
                            doCsc4dMoore=True, doMoore = False )
        # doMuonboy=False
        self.assertNewFlagsOK( True, True, ['Moore.Csc4dMoore'],
                            doCsc4dMoore=True, doMuonboy=False )
        self.assertNewFlagsOK( True, True, ['Moore.Csc4dMoore'],
                            doCsc4dMoore=True, doMuonboy=False, doMoore=True )
        self.assertNewFlagsOK( True, True, [],
                            doCsc4dMoore=True, doMuonboy=False, doMoore=False )
        # doMuonboy=True
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.Csc4dMoore'],
                            doCsc4dMoore=True, doMuonboy=True )
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.Csc4dMoore'],
                            doCsc4dMoore=True, doMuonboy=True, doMoore=True )
        self.assertNewFlagsOK( True, True, ['Muonboy'],
                            doCsc4dMoore=True, doMuonboy=True, doMoore=False )


    def testMoMuOldMuonboy(self):
        """MoMu with old-style Muonboy"""
        # no doMuonboy defined
        self.assertNewFlagsOK( True, False, ['Moore.MoMu'],
                            doMoMu=True )
        self.assertNewFlagsOK( True, False, ['Moore.MoMu'],
                            doMoMu=True, doMoore = True )
        self.assertNewFlagsOK( True, False, [],
                            doMoMu=True, doMoore = False )
        # doMuonboy=False
        self.assertNewFlagsOK( True, False, ['Moore.MoMu'],
                            doMoMu=True, doMuonboy=False )
        self.assertNewFlagsOK( True, False, ['Moore.MoMu'],
                            doMoMu=True, doMuonboy=False, doMoore=True )
        self.assertNewFlagsOK( True, False, [],
                            doMoMu=True, doMuonboy=False, doMoore=False )
        # doMuonboy=True
        self.assertNewFlagsOK( True, False, ['Moore.MoMu'],
                            doMoMu=True, doMuonboy=True )
        self.assertNewFlagsOK( True, False, ['Moore.MoMu'],
                            doMoMu=True, doMuonboy=True, doMoore=True )
        self.assertNewFlagsOK( True, False, [],
                            doMoMu=True, doMuonboy=True, doMoore=False )


    def testMoMuNewMuonboy(self):
        """MoMu with new-style Muonboy"""
        # no doMuonboy defined
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.MoMu'],
                            doMoMu=True )
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.MoMu'],
                            doMoMu=True, doMoore = True )
        self.assertNewFlagsOK( True, True, ['Muonboy'],
                            doMoMu=True, doMoore = False )
        # doMuonboy=False
        self.assertNewFlagsOK( True, True, ['Moore.MoMu'],
                            doMoMu=True, doMuonboy=False )
        self.assertNewFlagsOK( True, True, ['Moore.MoMu'],
                            doMoMu=True, doMuonboy=False, doMoore=True )
        self.assertNewFlagsOK( True, True, [],
                            doMoMu=True, doMuonboy=False, doMoore=False )
        # doMuonboy=True
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.MoMu'],
                            doMoMu=True, doMuonboy=True )
        self.assertNewFlagsOK( True, True, ['Muonboy','Moore.MoMu'],
                            doMoMu=True, doMuonboy=True, doMoore=True )
        self.assertNewFlagsOK( True, True, ['Muonboy'],
                            doMoMu=True, doMuonboy=True, doMoore=False )





# execute the tests
if __name__ == "__main__":
    UnitTest.main()
