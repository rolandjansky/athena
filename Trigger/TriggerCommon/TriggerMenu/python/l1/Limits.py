# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
This temporarily holds CTP sizes
"""

class Access(type):
    """Metaclass to implement __getattr__ for class variables"""
    def __getattr__(cls, key):
        if cls.ctpDataFormat is None:
            raise RuntimeError("CTP version has not been set, you need to call Limits.setLimits(ctpversion) before you can use the limits")

        if hasattr(cls.ctpDataFormat, key):
            return getattr( cls.ctpDataFormat, key)
        elif hasattr(cls.l1common, key):
            return getattr( cls.l1common, key)
        else:
            raise AttributeError("Neither class 'CTPdataformat' nor class 'L1Common' have an attribute '%s'" % key)
    
    def __str__(cls):
        if cls.ctpDataFormat == None:
            return "None"

        s = "CTP DataFormat version %i\n" % cls.CTPVersion
        varnames = ['MaxTrigItems', 'NumBunchgroups', 'NumRndmTriggers']
        for varname in varnames:
            s += "     %s = %r\n" % (varname, getattr( cls.ctpDataFormat, varname)) 
        s += "L1Common version %i\n" % cls.L1CommonVersion
        varnames = ['MUON_bitnum','EM_bitnum', 'TAU_bitnum', 'JET_bitnum', 'JE_bitnum', 'JB_bitnum', 'JF_bitnum', 'TE_bitnum', 'XE_bitnum', 'XS_bitnum',
                    'MBTS_bitnum', 'MBTSSI_bitnum', 'NIM_bitnum', 'ZDC_bitnum', 'TRT_bitnum', 'BCM_bitnum', 'BCMCMB_bitnum', 'LUCID_bitnum', 'CALREQ_bitnum',
                    'MUON_cable', 'EM_cable', 'TAU_cable', 'JET_cable', 'JE_cable', 'JB_cable', 'JF_cable', 'TE_cable', 'XE_cable', 'XS_cable', 'MBTS_cable', 
                    'MBTSSI_cable', 'NIM_cable', 'ZDC_cable', 'TRT_cable', 'BCM_cable', 'BCMCMB_cable', 'LUCID_cable', 'CALREQ_cable']
        for varname in varnames:
            s += "     %s = %r\n" % (varname, getattr( cls.l1common, varname)) 
        return s




class Limits:

    __metaclass__ = Access

    CTPVersion      = None
    L1CommonVersion = None

    ctpDataFormat = None
    l1common      = None
    
    @staticmethod
    def getCTPdataformat(version):
        module = __import__('CTPfragment.CTPdataformat_v%i' % version, globals(), locals(), ['CTPdataformat_v%i' % version], -1)
        CTPdataformat = getattr(module,'CTPdataformat_v%i' % version)
        return CTPdataformat

    @staticmethod
    def getL1Common(version):
        module = __import__('L1Common.L1Common_v%i' % version, globals(), locals(), ['L1Common_v%i' % version], -1)
        L1Common = getattr(module,'L1Common_v%i' % version)
        return L1Common

    @staticmethod
    def setLimits(CTPVersion, verbose = False):
        Limits.CTPVersion = CTPVersion
        Limits.L1CommonVersion = 0 if CTPVersion <= 3 else 1
        #print "Setting limits for CTP version %i and L1Common Version %i" % (Limits.CTPVersion, Limits.L1CommonVersion)
        Limits.ctpDataFormat = Limits.getCTPdataformat( Limits.CTPVersion )
        Limits.l1common = Limits.getL1Common( Limits.L1CommonVersion )
        if verbose:
            print Limits

    


class CaloLimits:
    # Maximum values for calo thresholds to disable that threshold
    ClusterOff   = 255
    IsolationOff = 63
    JetOff       = 1023
    EtSumOff     = 32767
    EtMissOff    = 32767 
    JetEtOff     = 13286

