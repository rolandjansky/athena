# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from LArROD.LArRODConf import LArDigitThinner
from LArCabling.LArCablingAccess import LArOnOffIdMapping 
        
class DefaultLArDigitThinner (LArDigitThinner) :
     def __init__(self, name = 'LArDigitThinner', addToAlgSeq = True) :
        super( DefaultLArDigitThinner, self ).__init__( name )

        from AthenaCommon.BeamFlags import jobproperties
        if jobproperties.Beam.beamType() == "cosmics" :
            self.EnergyCuts_Barrel = [ 1000, 500, 1000, 1000 ] # values for [presampler, strips, mid, back ]
            self.EnergyCuts_Endcap = [ 1000, 500, 1000, 1000 ] # values for [presampler, strips, mid, back ]
            self.EnergyCut_HEC  = 2000
            self.EnergyCut_FCAL = 3000
        else :
            self.EnergyCuts_Barrel = [ 1000, 1000, 1000, 1000 ] # values for [presampler, strips, mid, back ]
            self.EnergyCuts_Endcap = [ 2000, 2000, 2000, 2000 ] # values for [presampler, strips, mid, back ]
            self.EnergyCut_HEC  = 5000
            self.EnergyCut_FCAL = 20000

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        from AthenaCommon.GlobalFlags import globalflags
        if globalflags.DataSource()=='data':
           if globalflags.InputFormat() == 'bytestream':
                from LArByteStream.LArByteStreamConf import LArRawDataReadingAlg
                if LArRawDataReadingAlg() not in topSequence:
                     topSequence+=LArRawDataReadingAlg()
                self.InputContainerName="FREE"

        LArOnOffIdMapping()

        if addToAlgSeq : 
             topSequence += self

     def setDefaults(self, handle):
        pass
    
