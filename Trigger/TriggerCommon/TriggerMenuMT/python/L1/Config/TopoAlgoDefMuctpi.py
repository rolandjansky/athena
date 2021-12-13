# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# algorithm python base classes generated from C++ code
import L1TopoAlgorithms.L1TopoAlgConfig as AlgConf
import L1TopoHardware.L1TopoHardware as HW


from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

class TopoAlgoDefMuctpi:

    @staticmethod
    def registerTopoAlgos(tm):

        # TODO: no conversion applied!

        # SELECT
        alg = AlgConf.MuonSelect( name = 'MU5VFab', inputs = 'MuonTobs', outputs = 'MU5VFab' )
        alg.addgeneric('InputWidth', HW.muonInputWidth)
        alg.addgeneric('OutputWidth', HW.muonOutputWidthSelect)
        alg.addvariable('MinEta',  0)
        alg.addvariable('MaxEta', 25)
        alg.addvariable('MinEtTGC', 5)
        alg.addvariable('MinEtRPC', 6)
        alg.addvariable('InnerCoinCut', 0)
        alg.addvariable('FullStationCut',1)
        alg.addvariable('GoodMFieldCut', 0)
        tm.registerTopoAlgo(alg)

        # dimu DR items
        listofalgos=[
            { "minDr": 0, "maxDr": 15, "mult": 2, 
              "otype1" : "MU5VFab", "ocut1": 6,  "olist" : "ab",
              "otype2" : "",}, #0DR15-2MU5VFab
        ]

        for x in listofalgos:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])

            obj1 = "%s%s" % ((str(d.mult) if d.mult>1 else ""), d.otype1)
            obj2 = "-%s" % (d.otype2)
            toponame = "MUCTP-%iDR%i-%s%s"  % (d.minDr, d.maxDr, obj1, "" if d.mult>1 else obj2)

            log.debug("Define %s", toponame)

            inputList = [d.otype1] if (d.mult>1) else [d.otype1, d.otype2]
            algoname = AlgConf.DeltaRSqrIncl1 if (d.mult>1) else AlgConf.DeltaRSqrIncl2
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ])
 
            if (d.mult>1):
                alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
            else:
                alg.addgeneric('InputWidth1', HW.OutputWidthSelectMU)
                alg.addgeneric('InputWidth2', HW.OutputWidthSelectMU) 
                alg.addgeneric('MaxTob1', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob2', HW.OutputWidthSelectMU)

            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', 0)
            alg.addvariable('MinET2', 0)
            alg.addvariable('DeltaRMin', d.minDr*d.minDr)
            alg.addvariable('DeltaRMax', d.maxDr*d.maxDr)
            tm.registerTopoAlgo(alg)
