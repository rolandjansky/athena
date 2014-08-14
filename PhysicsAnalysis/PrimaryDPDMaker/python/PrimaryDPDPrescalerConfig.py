# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name: PrimaryDPDPrescalerConfig.py   
##
## Author: Marcello Barisonzi (DESY)
## Email:  marcello.barisonzi@desy.de
##
## Description: Python wrapper for the C++ PrimaryDPDPrescaler algorithm
##
##=============================================================================

def ApplyPrescaler(theStream, AcceptAlgs=[], RequireAlgs=[], Prescale=1):

    from OutputStreamAthenaPool.MultipleStreamManager import AugmentedStreamBase

    if not isinstance(theStream, AugmentedStreamBase):
        if not isinstance(theStream, str):
            raise Exception("ApplyPrescaler : Unknown type")
        else:
            from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
            # get real stream from string name
            theStream = MSMgr.GetStream(theStream)


    from PrimaryDPDMaker.PrimaryDPDMakerConf import PrimaryDPDPrescaler

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    _theName = theStream.Stream.name()

    for i in AcceptAlgs:
        _theName += "_%s" % i

    for i in RequireAlgs:
        _theName += "_%s" % i
    
    _theName += "_Prescaler"
    
    _Prescaler = PrimaryDPDPrescaler(_theName)

    # deep copy
    _Prescaler.AcceptAlgs  = AcceptAlgs[:]
    _Prescaler.RequireAlgs = RequireAlgs[:]

    _Prescaler.Prescale = Prescale
    
    topSequence += _Prescaler

    #theStream.AddRequireAlgs( [_theName] )

    return _Prescaler
