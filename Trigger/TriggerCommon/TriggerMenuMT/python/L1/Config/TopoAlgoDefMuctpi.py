# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# algorithm python base classes generated from C++ code
import L1TopoAlgorithms.L1TopoAlgConfig as AlgConf
import L1TopoHardware.L1TopoHardware as HW


from AthenaCommon.Logging import logging
log = logging.getLogger('Menu.L1.Config.TopoAlgoDefMUTCPI.py')

class TopoAlgoDefMuctpi:

    @staticmethod
    def registerTopoAlgos(tm):

        # dimu DR items
        listofalgos=[
            { "minDr": 0, "maxDr": 15, "mult": 2, 
              "otype1" : "MU", "ocut1": 6,  "olist" : "ab",
              "otype2" : "", "ocut2": 6, "onebarrel": 0 }, #0DR15-2MU6ab
        ]

        for x in listofalgos:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])

            obj1 = "%s%s%s%s" % ((str(d.mult) if d.mult>1 else ""), d.otype1, str(d.ocut1), d.olist)
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist)
            toponame = "%iDR%i-%s%s%s"  % (d.minDr, d.maxDr, "ONEBARREL-" if d.onebarrel==1 else "", obj1, "" if d.mult>1 else obj2)

            log.debug("Define %s", toponame)

            inputList = [d.otype1 + d.olist] if (d.mult>1 or d.otype1==d.otype2) else [d.otype1 + d.olist, d.otype2 + d.olist]
            algoname = AlgConf.DeltaRSqrIncl1 if (d.mult>1 or d.otype1==d.otype2) else AlgConf.DeltaRSqrIncl2
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = -1)
 
            if (d.mult>1 or d.otype1==d.otype2):
                alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
                alg.addgeneric('RequireOneBarrel', d.onebarrel)
            else:
                alg.addgeneric('InputWidth1', HW.OutputWidthSelectMU)
                alg.addgeneric('InputWidth2', HW.OutputWidthSelectMU) 
                alg.addgeneric('MaxTob1', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob2', HW.OutputWidthSelectMU)


            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut1)
            alg.addvariable('MinET2', d.ocut2)
            alg.addvariable('DeltaRMin', d.minDr*d.minDr)
            alg.addvariable('DeltaRMax', d.maxDr*d.maxDr)
            tm.registerTopoAlgo(alg)
