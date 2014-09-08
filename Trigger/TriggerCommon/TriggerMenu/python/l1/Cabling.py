# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Limits import Limits
from AthenaCommon.Logging import logging
log = logging.getLogger("Cabling")
log.setLevel(logging.INFO)

class Cabling:
    @staticmethod
    def getInputCable(thrtype, mapping, zbSeedType=None):
        if thrtype=='EM' and mapping>=8:
            return Cabling.getInputCable('TAU', 15-mapping)

        if thrtype=='ZB': # special case for ZB triggers that are sitting on the last bit of the same connector
            inputCable             = Cabling.getInputCable(zbSeedType, mapping)
            inputCable.bitnum      = 1
            inputCable.range_begin = 30
            inputCable.range_end   = 30
            return inputCable
        
        return InputCable( thrtype, mapping )

    @staticmethod
    def getCableName(thrtype):
        type2cablename  = { 'MUON'  : 'MU',
                            'EM'    : 'CP1',
                            'TAU'   : 'CP2',
                            'JET'   : 'JEP1',
                            'JE'    : 'JEP1',
                            'FJET'  : 'JEP2',
                            'JB'    : 'JEP2',
                            'JF'    : 'JEP2',
                            'TE'    : 'JEP3',
                            'XE'    : 'JEP3',
                            'XS'    : 'JEP3',
                            'LUCID' : 'NIM',
                            'TRT'   : 'NIM',
                            'ZDC'   : 'NIM',
                            'BCM'   : 'NIM',
                            'BCMCMB': 'NIM',
                            'MBTSSI': 'NIM',
                            'MBTS'  : 'NIM',
                            'CALREQ': 'NIM',
                            'NIM'   : 'NIM',
                            'TOPO'  : 'TOPO1'
                            }
        return type2cablename[thrtype]


    @staticmethod
    def calcBitnum(thrtype):
        # get the widths for the threshold types is defined in L1Common
        if thrtype=='TOPO':
            nbits = 1
        else:
            exec("nbits = Limits.%s_bitnum" % thrtype)
        return nbits

    @classmethod
    def getCableAssignment(cls, thrtype):
        """
        Gets the cable assignment from L1Common
        """
        if thrtype=='TOPO' or thrtype=='ALFA':
            directInput = True
            cableAssign = [ (0, 2, 0, 31), (1, 2, 0, 31), (2, 2, 0, 31) ] # mapping 0-63, 64-127, 128-191
        else:
            directInput = False
            exec("cable = Limits.%s_cable" % thrtype)
            cableAssign = []
            for i in range(cable[0]):
                cableAssign.append( tuple(cable[1+4*i:5+4*i]) )

        return directInput, cableAssign



class InputCable:
    def __init__(self, thrtype, mapping):
        self.name    = Cabling.getCableName(thrtype)
        self.thrtype = thrtype
        self.mapping = mapping

        self.isDirectIn  = False # True for TOPO and ALFA which go into CTPCore
        self.slot        = 0     # input cable slot, possible values 7..9
        self.connector   = 0     # input cable connector, possible values 0..3
        self.clock       = 0     # for direct inputs this can be 0 or 1 since they are overclocked x2
        self.bitnum      = 0     # number of bits for multiplicity transmittion, possible values 1..3 
        self.range_begin = 0     # first bit of range, possible values 0..30
        self.range_end   = 0     # last bit of range, possible values 0..30 (should be first bit + bitnum - 1)

        self.calcSlotAndConnector(self.thrtype, self.mapping)

        if thrtype=='EM' and mapping>=8: self.name = 'CP2'  # need to understand this
        if thrtype=='TOPO' and mapping>=64: self.name = 'TOPO2'

        if self.isDirectIn:
            log.debug( 'Threshold type %s (mapping=%i) comes in on CTPCore on cable %s, bit %s, clock %i' % ( self.thrtype, self.mapping, self.connector,
                                                                                                              ("%i" % self.range_begin) if self.bitnum==1
                                                                                                              else ("%i-%i" % (self.range_begin, self.range_end)), self.clock ) )
        else:
            log.debug( 'Threshold type %s (mapping=%i) comes in on CTPIN on cable %s/%s and bits %s' % ( self.thrtype, self.mapping, self.slot, self.connector,
                                                                                                         ("%i" % self.range_begin) if self.bitnum==1
                                                                                                         else ("%i-%i" % (self.range_begin, self.range_end)) ) )
            

    def calcSlotAndConnector(self, thrtype, mapping):

        self.bitnum = Cabling.calcBitnum(thrtype)
        if self.bitnum == 0:
            return;


        isDirectInput, cableAssign = Cabling.getCableAssignment(thrtype)

        # CTPIN
        if not isDirectInput:
            self.isDirectIn = False
            offset = mapping
            for (slot, connector, start, stop) in cableAssign:

                delta = (stop - start + 1) / self.bitnum
                log.debug( 'Cable SLOT%i / CON%i has room for %i thresholds of type %s' % (slot, connector, delta, thrtype) )

                if offset >= delta: # does not fit on this connector (only 0 to offset-1 will fit)
                    offset -= delta # move to the next cable for checking
                    continue

                self.slot        = "SLOT%i" % slot
                self.connector   = "CON%i" % connector
                self.range_begin = start + offset * self.bitnum
                self.range_end   = self.range_begin + self.bitnum-1
                break

        else:
            self.isDirectIn = True
            offset = mapping
            for (connector, clock, start, stop) in cableAssign:

                delta = clock * (stop - start + 1) / self.bitnum
                log.debug( 'Cable CON%i has room for %i thresholds of type %s' % (connector, delta, thrtype) )

                if offset >= delta: # does not fit on this connector (only 0 to offset-1 will fit)
                    offset -= delta # move to the next cable for checking
                    continue

                w = int( (stop - start + 1) / self.bitnum )

                self.connector   = "CON%i" % connector
                self.clock       = int(offset / w)
                self.range_begin = start + (offset % w) * self.bitnum
                self.range_end   = self.range_begin + self.bitnum-1
                break
            

