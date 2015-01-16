# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Limits import Limits
from AthenaCommon.Logging import logging
log = logging.getLogger("Cabling")
log.setLevel(logging.INFO)

"""
Thilo's proposal in the CTP meeting Oct 13

* Run2 Cable to slot assignments:

CTPIN-slot7
-----------
EM1  : 8x3b EM - mapping 0-7
EM2  : 8x3b EM - mapping 8-15
TAU1 : 8x3b TAU - mapping 0-7
TAU2 : 8x3b TAU - mapping 8-15

CTPIN-slot8
-----------
JET1 : 10x3b JET - mapping 0-9
JET2 : 15x2b JET - mapping 10-24
EN1  : 8x1b TE [0..7] (SumET)
       8x1b XE [8..15] (missing ET)
       8x1b XS [16..23] (missing ET significance)
EN2  : 8x1b weighted SumET
       8x1b restricted range MissET


CTPIN-slot9
-----------
MUCTPI : first bit unused,
         6x3b MU [1..18] - mapping 0-5
CTPCAL : 6x1b BCM [0..5] - mapping 0-5
         8x1b DBM [6..13] - mapping 0-7
         2x1b BPTX [14..15] - mapping 0-1
         6x1b LUCID [16..21] - mapping 0-5
         3x1b ZDC [22..24] - mapping 0-2
         3x1b CALREQ [28..30] last 3 bits! - mapping 0-2
NIM1   : 12x1b MBTSSI [0..11] - mapping 0-11 A-side
         4x1b NO [12..15] unused
         1x3b MBTS [16..18]  A-side multiplicity
         1x1b NIM [19] L1A  - mapping 0
         1x1b NIM [20] LHCf - mapping 1
         10x1b NIM [21..30] - mapping 2-11
NIM2   : 12x1b MBTSSI [0..11]  - mapping 12-23 C-side 
         4x1b NO [12..15] unused
         1x3b MBTS [16..18] C-side multiplicity
         1x1b NIM TGC-burst [19] - mapping 12
         1x1b NIM RPC-burst [20] - mapping 13
         1x1b NIM TRT-FastOR [21] - mapping 14
         9x1b NIM [22..30] - mapping 15-23

CTPCORE-slot10
--------------
L1Topo0 : 64x1b TOPO mapping 0-63
L1Topo1 : 64x1b TOPO mapping 64-127
ALFA    : 64x1b ALFA mapping 0-63

* Notes:
- JET1: 8x3b will occupy all CTPIN LUTs, leaving only 2b per LUT.
        This means that 2x3b cannot be counted properly.

- Define where delayed minimum bias trigger is done, e.g. 1 bit in JET1
"""


class Cabling:
    @staticmethod
    def getInputCable(thrtype, mapping, zbSeedType=None):
        #if thrtype=='EM' and mapping>=8: # for run 1
        #    return Cabling.getInputCable('TAU', 15-mapping)

        if thrtype=='ZB': # special case for ZB triggers that are sitting on the last bit of the same connector
            inputCable             = Cabling.getInputCable(zbSeedType, mapping)
            inputCable.bitnum      = 1
            inputCable.range_begin = 30
            inputCable.range_end   = 30
            return inputCable
        
        return InputCable( thrtype, mapping )

    @staticmethod
    def getCableName(thrtype,mapping):

        from TriggerMenu.l1.Lvl1Flags import Lvl1Flags
        run1 = Lvl1Flags.CTPVersion()<=3
        
        if run1:
            type2cablename  = { 'MUON'   : [(0,6,'MUCTPI')],
                                'EM'     : [(0,8,'EM1'), (8,16,'EM2')],
                                'TAU'    : [(0,8,'TAU1'), (8,16,'TAU2')],
                                'JET'    : [(0,10,'JET1'), (10,25,'JET2')],
                                'TE'     : [(0,8,'EN1')],
                                'XE'     : [(0,8,'EN1')],
                                'XS'     : [(0,8,'EN1')],
                                'MBTSSI' : [(0,16,'NIM1'),(16,32,'NIM2')],
                                'MBTS'   : [(0,1,'NIM1'), (1,2,'NIM2')],
                                'LUCID'  : [(0,6,'CTPCAL')],
                                'ZDC'    : [(0,3,'CTPCAL')],
                                'CALREQ' : [(0,3,'CTPCAL')],
                                'TOPO'   : [(0,64,'TOPO1'), (64,128,'TOPO2')],
                                'ALFA'   : [(0,64,'ALFA')],
                                'BCM'    : [(0,3,'CTPCAL')],
                                'BCMCMB' : [(0,3,'CTPCAL')],
                                'NIM'    : [(0,30,'NIM1'), (30,60,'NIM2')],
                                'JF'     : [(0,4,'JET1')],
                                'JB'     : [(0,4,'JET1')],
                                'JE'     : [(0,4,'JET2')],
                                }
        else:
            type2cablename  = { 'MUON'   : [(0,6,'MUCTPI')],
                                'EM'     : [(0,8,'EM1'), (8,16,'EM2')],
                                'TAU'    : [(0,8,'TAU1'), (8,16,'TAU2')],
                                'JET'    : [(0,10,'JET1'), (10,25,'JET2')],
                                'TE'     : [(0,8,'EN1')],
                                'XE'     : [(0,8,'EN1')],
                                'XS'     : [(0,8,'EN1')],
                                'MBTSSI' : [(0,12,'NIM1'),(12,24,'NIM2')],
                                'MBTS'   : [(0,1,'NIM1'), (1,2,'NIM2')],
                                'LUCID'  : [(0,6,'CTPCAL')],
                                'ZDC'    : [(0,3,'CTPCAL')],
                                'CALREQ' : [(0,3,'CTPCAL')],
                                'TOPO'   : [(0,64,'TOPO1'), (64,128,'TOPO2')],
                                'ALFA'   : [(0,64,'ALFA')],
                                'BCM'    : [(0,3,'CTPCAL')],
                                'BCMCMB' : [(0,3,'CTPCAL')],
                                'NIM'    : [(0,12,'NIM1'), (12,24,'NIM2')],
                                'JF'     : [(0,4,'JET1')],
                                'JB'     : [(0,4,'JET1')],
                                'JE'     : [(0,4,'JET2')],
                                }

        x = type2cablename[thrtype]
        for (minthr, maxthr, name) in x:
            #print "BETTA ",mapping, minthr, maxthr, name
            if mapping>=minthr and mapping<maxthr:
                return name

        raise RuntimeError("Threshold type %s has no cable for mapping '%i'" % (thrtype,mapping))


    @staticmethod
    def calcBitnum(thrtype):
        # get the widths for the threshold types is defined in L1Common
        if thrtype=='TOPO':
            nbits = 1
        else:
            exec("nbits = Limits.%s_bitnum" % thrtype)
        return nbits



class InputCable:
    def __init__(self, thrtype, mapping):

        self.thrtype = thrtype
        self.mapping = int(mapping)
        self.isDirectIn  = False # True for TOPO and ALFA which go into CTPCore
        self.slot        = None  # input cable slot, possible values 7..9
        self.connector   = None  # input cable connector, possible values 0..3
        self.clock       = 0     # for direct inputs this can be 0 or 1 since they are overclocked x2
        self.bitnum      = 0     # number of bits for multiplicity transmittion, possible values 1..3 
        self.range_begin = 0     # first bit of range, possible values 0..30
        self.range_end   = 0     # last bit of range, possible values 0..30 (should be first bit + bitnum - 1)

        if thrtype=='TOPO' or thrtype=='ALFA':
            self.isDirectIn = True

        self.name = Cabling.getCableName(self.thrtype,self.mapping)

        #if self.thrtype=='NIM':
        #    print str(self)

        self.calcSlotAndConnector()

        if self.isDirectIn:
            log.debug( 'Threshold type %s (mapping=%i) comes in on CTPCore on cable %s, bit %s, clock %i' % ( self.thrtype, self.mapping, self.connector,
                                                                                                              ("%i" % self.range_begin) if self.bitnum==1
                                                                                                              else ("%i-%i" % (self.range_begin, self.range_end)), self.clock ) )
        else:
            log.debug( 'Threshold type %s (mapping=%i) comes in on CTPIN on cable %s/%s and bits %s' % ( self.thrtype, self.mapping, self.slot, self.connector,
                                                                                                         ("%i" % self.range_begin) if self.bitnum==1
                                                                                                         else ("%i-%i" % (self.range_begin, self.range_end)) ) )

        if not self.connector:
            raise RuntimeError("No cable has been assigned to threshold type '%s' with mapping %i" % (self.thrtype,self.mapping))



    def __str__(self):
        return "%s %r  (%s/%s) with %i bits [%i-%i] clock=%i" % (self.thrtype, self.mapping, self.slot, self.connector, self.bitnum, self.range_begin, self.range_end, self.clock)



    def calcSlotAndConnector(self):

        #self.bitnum = Cabling.calcBitnum(self.thrtype)
        #if self.bitnum == 0:
        #    return;

        # CTPIN
        if not self.isDirectIn:

            cableAssign = self.getCTPINCableAssignment(self.thrtype)

            from TriggerMenu.l1.Lvl1Flags import Lvl1Flags
            run1 = Lvl1Flags.CTPVersion()<=3
            if run1 and self.thrtype=="EM":
                cableAssign += self.getCTPINCableAssignment("TAU")

            offset = self.mapping
            for (slot, connector, start, stop, bitnum) in cableAssign:
                
                self.bitnum = bitnum
                
                delta = (stop - start + 1) / self.bitnum
                log.debug( 'Cable SLOT%i / CON%i has room for %i thresholds of type %s' % (slot, connector, delta, self.thrtype) )

                if offset >= delta: # does not fit on this connector (only 0 to offset-1 will fit)
                    offset -= delta # move to the next cable for checking
                    continue

                self.slot        = "SLOT%i" % slot
                self.connector   = "CON%i" % connector
                self.range_begin = start + offset * self.bitnum
                self.range_end   = self.range_begin + self.bitnum-1
                break

            if not self.connector:
                print "Cable mapping ",cableAssign
                raise RuntimeError("No cable has been assigned to threshold type '%s' with mapping %i" % (self.thrtype,self.mapping))



        else:
            # CTPCORE
            self.bitnum = 1
            module = int(self.mapping) / 64
            self.connector   = "CON%i" % module
            self.clock = (self.mapping % 64) / 32
            self.range_begin = self.mapping % 32
            self.range_end   = self.range_begin
            #print "Connectpr ",self.connector,"  clock=",self.clock," range",self.range_begin," ",self.range_end



    def getCTPINCableAssignment(self,thrtype):
        """
        Gets the cable assignment from L1Common
        """
        exec("cable = Limits.%s_cable" % thrtype)

        # we change the format for run 2, the tuple now contains also the bit multiplicity, as it is not constant per type
        infosize = (len(cable)-1)/cable[0]

        if infosize==5:
            cableAssign = [tuple(cable[x:x+5]) for x in range(1,len(cable),5)]
        else:
            #print "Cabling for threshold type %s is not yet defined for Run 2" % thrtype
            bitnum = Cabling.calcBitnum(thrtype)
            cableAssign = [tuple(cable[x:x+4] + [bitnum]) for x in range(1,len(cable),4)]

        return cableAssign


