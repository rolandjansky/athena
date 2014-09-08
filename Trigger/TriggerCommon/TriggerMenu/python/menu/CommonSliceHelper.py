# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" helpers for slice flags manipulations """
class CommonSliceHelper:
    def setAll(self):
        #self.setL2()
        #self.setEF()
        self.signatures.set_On()
        
    def unsetAll(self):
        #self.unsetL2()
        #self.unsetEF()
        self.signatures.set_Off()
        
    def setL2(self):
        if hasattr(self, 'doL2ID'):
            self.doL2ID=True
        if hasattr(self, 'doL2Calo'):
            self.doL2Calo=True
        if hasattr(self, 'doL2Muon'):
            self.doL2Muon=True
        
    def unsetL2(self):
        if hasattr(self, 'doL2ID'):
            self.doL2ID=False
        if hasattr(self, 'doL2Calo'):            
            self.doL2Calo=False
        if hasattr(self, 'doL2Muon'):            
            self.doL2Muon=False

    def setEF(self):
        if hasattr(self, 'doEFID'):        
            self.doEFID=True
        if hasattr(self, 'doEFCalo'):            
            self.doEFCalo=True
        if hasattr(self, 'doEFMuon'):            
            self.doEFMuon=True
        
    def unsetEF(self):
        if hasattr(self, 'doEFID'):
            self.doEFID=False
        if hasattr(self, 'doEFCalo'):            
            self.doEFCalo=False
        if hasattr(self, 'doEFMuon'):            
            self.doEFMuon=False
        
    def unsetID(self):
        if hasattr(self, 'doL2ID'):        
            self.doL2ID = False
        if hasattr(self, 'doEFID'):        
            self.doEFID = False

    def setID(self):
        if hasattr(self, 'doL2ID'):        
            self.doL2ID = True
        if hasattr(self, 'doEFID'):        
            self.doEFID = True

    def unsetCalo(self):
        if hasattr(self, 'doL2Calo'):        
            self.doL2Calo = False
        if hasattr(self, 'doEFCalo'):                
            self.doEFCalo = False

    def setCalo(self):
        if hasattr(self, 'doL2Calo'):                
            self.doL2Calo = True
        if hasattr(self, 'doEFCalo'):                    
            self.doEFCalo = True
        return 

    def unsetMuon(self):
        if hasattr(self, 'doL2Muon'):                
            self.doL2Muon = False
        if hasattr(self, 'doEFMuon'):                
            self.doEFMuon = False

    def setMuon(self):
        if hasattr(self, 'doL2Muon'):                
            self.doL2Muon = True
        if hasattr(self, 'doEFMuon'):                
            self.doEFMuon = True

    def doAny(self):
        return (self.doL2() or self.doEF())

    def doL2(self):
        return (self.doL2Muon or self.doL2Calo or self.doL2ID )

    def doEF(self):
        return (self.doEFMuon or self.doEFCalo or self.doEFID )

    def enableSignature(self, chainInfo):
        """ Add signature defined by chainInfo array """
        self.enableSignatures([chainInfo])
        
    def enableSignatures(self, chainInfoList):
        """ Add list of signatures """
        current=self.signatures.get_Value()
        new = [sig for sig in chainInfoList if sig not in current]
        new.extend(current)
        self.signatures.set_Value(new)

    def disableSignature(self, l):
        """ Disable signature with name l """
        self.disableSignatures([l])
        
    def disableSignatures(self, list):
        """ Disable list of sigantures """
        current = self.signatures.get_Value()
        new = [sig for sig in current if sig[0] not in list ]
        self.signatures.set_Value(new)


class Bag:
    def __init__(self,l):
        self.allowedValues = l

    def __str__(self):
        return str(self.allowedValues)

    def __repr__(self):
        return "%r" % self.allowedValues

    def __contains__(self, val):
        return True in [ x == val for x in self.allowedValues]

    def __eq__(self, arg):
        if type(arg)==list or type(arg)==tuple or type(arg)==set:
            
            notContained = [a for a in arg if not a in self]

            if len(notContained)==0: return True

            from AthenaCommon.Logging import logging
            log = logging.getLogger( 'AllowedList' )
            log.error("the element %s is not in the list of allowed values: %s" % (notContained, self) )
            return False
        return True


class AllowedList(list):
    def __init__(self, l):
        super(AllowedList,self).__init__([Bag(l)])
