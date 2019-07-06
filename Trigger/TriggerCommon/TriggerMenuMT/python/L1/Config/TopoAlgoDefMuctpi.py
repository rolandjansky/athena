# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# algorithm python base classes generated from C++ code
import L1TopoAlgorithms.L1TopoAlgConfig as AlgConf
import L1TopoHardware.L1TopoHardware as HW


from AthenaCommon.Logging import logging
log = logging.getLogger('Menu.L1.Config.TopoAlgoDefMUTCPI.py')

class TopoAlgoDefMuctpi:

    @staticmethod
    def registerTopoAlgos(tm):

        currentAlgoId = 0

        # dimu DR items
        listofalgos=[
            { "minDr": 0, "maxDr": 15, "mult": 2, 
              "otype1" : "MU", "ocut1": 6,  "olist" : "ab",
              "otype2" : "", "ocut2": 6, "onebarrel": 0 }, #0DR15-2MU6ab
        ]

        for x in listofalgos:
            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s%s" % ((str(mult) if mult>1 else ""), otype1, str(ocut1), olist)    # noqa: F821
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist)                                    # noqa: F821
            toponame = "%iDR%i-%s%s%s"  % (minDr, maxDr, "ONEBARREL-" if onebarrel==1 else "", obj1, "" if mult>1 else obj2)  # noqa: F821

            log.debug("Define %s", toponame)

            inputList = [otype1 + olist] if (mult>1 or otype1==otype2) else [otype1 + olist, otype2 + olist]    # noqa: F821
            algoname = AlgConf.DeltaRSqrIncl1 if (mult>1 or otype1==otype2) else AlgConf.DeltaRSqrIncl2         # noqa: F821
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1
 
            if (mult>1 or otype1==otype2):  # noqa: F821
                alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
                alg.addgeneric('RequireOneBarrel', onebarrel)  # noqa: F821
            else:
                alg.addgeneric('InputWidth1', HW.OutputWidthSelectMU)
                alg.addgeneric('InputWidth2', HW.OutputWidthSelectMU) 
                alg.addgeneric('MaxTob1', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob2', HW.OutputWidthSelectMU)


            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut1)            # noqa: F821
            alg.addvariable('MinET2', ocut2)            # noqa: F821
            alg.addvariable('DeltaRMin', minDr*minDr)   # noqa: F821
            alg.addvariable('DeltaRMax', maxDr*maxDr)   # noqa: F821
            tm.registerTopoAlgo(alg)
