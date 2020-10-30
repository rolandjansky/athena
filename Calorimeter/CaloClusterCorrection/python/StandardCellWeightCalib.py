# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#--
#-- Configures standard cell weight calibration tools (extracted from JetGetter)
#--
#-- Note the input for this calibration is jet driven. The fitted default

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging
from AthenaCommon.GlobalFlags import globalflags

_logger = logging.getLogger('Calorimeter::StandardCellWeightCalibration')

def editParm(param):
    numStr = str(param)
    posDec = numStr.find(".")
    lenStr = len(numStr)
    if posDec == 0:
        if lenStr >= 1:
            selStr = numStr[1:]
        else:
            selStr = "_"
    else:
        if posDec > 0:
            if lenStr < posDec+1:
                selStr = numStr[0:posDec-1]
            else:
                selStr = numStr[0:posDec-1] + numStr[posDec+1:]
        else:
            selStr = numStr
    return selStr

class H1Calibration(object):
    #-- 
    #-- Calibration dictionaries. Note that DB folders are truncated from the usual
    #-- jet naming conventions! The code here deals with this.
    #--
    calibdicV14 = {
        "Cone4Topo"  : "CaloH1WeightsCone4Topo-02-000",
        "Cone4Tower" : "CaloH1WeightsCone4Tower-02-000",
        "ConeTopo"   : "CaloH1WeightsConeTopo-02-000",
        "ConeTower"  : "CaloH1WeightsConeTower-02-000",
        "Kt4Topo"    : "CaloH1WeightsKt4Topo-02-000",
        "Kt4Tower"   : "CaloH1WeightsKt4Tower-02-000",
        "Kt6Topo"    : "CaloH1WeightsKt6Topo-02-000",
        "Kt6Tower"   : "CaloH1WeightsKt6Tower-02-000",
        "Cone4TopoAtlFastII"  : "CaloH1WeightsCone4TopoAtlFastII-02-000",
        "Cone4TowerAtlFastII" : "CaloH1WeightsCone4TowerAtlFastII-02-000",
        "ConeTopoAtlFastII"   : "CaloH1WeightsConeTopoAtlFastII-02-000",
        "ConeTowerAtlFastII"  : "CaloH1WeightsConeTowerAtlFastII-02-000",
        "Kt4TopoAtlFastII"    : "CaloH1WeightsKt4TopoAtlFastII-02-000",
        "Kt4TowerAtlFastII"   : "CaloH1WeightsKt4TowerAtlFastII-02-000",
        "Kt6TopoAtlFastII"    : "CaloH1WeightsKt6TopoAtlFastII-02-000",
        "Kt6TowerAtlFastII"   : "CaloH1WeightsKt6TowerAtlFastII-02-000",
        }
    calibdicV13 = {
        "Cone4Topo"  : "CaloH1WeightsCone4Topo-01-000",
        "Cone4Tower" : "CaloH1WeightsCone4Tower-01-000",
        "ConeTopo"   : "CaloH1WeightsConeTopo-01-000",
        "ConeTower"  : "CaloH1WeightsConeTower-01-000",
        "Kt4Topo"    : "CaloH1WeightsKt4Topo-01-000",
        "Kt4Tower"   : "CaloH1WeightsKt4Tower-01-000",
        "Kt6Topo"    : "CaloH1WeightsKt6Topo-01-000",
        "Kt6Tower"   : "CaloH1WeightsKt6Tower-01-000",
        "Cone4TopoAtlFastII"  : "CaloH1WeightsCone4TopoAtlFastII-01-000",
        "Cone4TowerAtlFastII" : "CaloH1WeightsCone4TowerAtlFastII-01-000",
        "ConeTopoAtlFastII"   : "CaloH1WeightsConeTopoAtlFastII-01-000",
        "ConeTowerAtlFastII"  : "CaloH1WeightsConeTowerAtlFastII-01-000",
        "Kt4TopoAtlFastII"    : "CaloH1WeightsKt4TopoAtlFastII-01-000",
        "Kt4TowerAtlFastII"   : "CaloH1WeightsKt4TowerAtlFastII-01-000",
        "Kt6TopoAtlFastII"    : "CaloH1WeightsKt6TopoAtlFastII-01-000",
        "Kt6TowerAtlFastII"   : "CaloH1WeightsKt6TowerAtlFastII-01-000",
        }
    calibdicV12 = {
        "Cone4Topo"  : "CaloH1WeightsCone4Topo-00-000",
        "Cone4Tower" : "CaloH1WeightsCone4Tower-00-000",
        "ConeTopo"   : "CaloH1WeightsConeTopo-00-000",
        "ConeTower"  : "CaloH1WeightsConeTower-00-000",
        "Kt4Topo"    : "CaloH1WeightsKt4Topo-00-000",
        "Kt4Tower"   : "CaloH1WeightsKt4Tower-00-000",
        "Kt6Topo"    : "CaloH1WeightsKt6Topo-00-000",
        "Kt6Tower"   : "CaloH1WeightsKt6Tower-00-000",
        "Cone4TopoAtlFastII"  : "CaloH1WeightsCone4TopoAtlFastII-00-000",
        "Cone4TowerAtlFastII" : "CaloH1WeightsCone4TowerAtlFastII-00-000",
        "ConeTopoAtlFastII"   : "CaloH1WeightsConeTopoAtlFastII-00-000",
        "ConeTowerAtlFastII"  : "CaloH1WeightsConeTowerAtlFastII-00-000",
        "Kt4TopoAtlFastII"    : "CaloH1WeightsKt4TopoAtlFastII-00-000",
        "Kt4TowerAtlFastII"   : "CaloH1WeightsKt4TowerAtlFastII-00-000",
        "Kt6TopoAtlFastII"    : "CaloH1WeightsKt6TopoAtlFastII-00-000",
        "Kt6TowerAtlFastII"   : "CaloH1WeightsKt6TowerAtlFastII-00-000",
        }

    #--
    loaded_folder = []

    # folder overriding -----------------------
    forceOverrideFolder = False
    @staticmethod
    def overrideFolder():
        return H1Calibration.forceOverrideFolder or globalflags.DataSource()!="data"

    #--
    #-- Helper functions to access DB parameters. Note that providing a dbtag (like from JetCalibrationDBTag)
    #-- overwrites the default tag extracted from the detector description version
    #--
    @staticmethod
    def calibration_dict(dbtag=""):
        #-- DB tag provided
        if dbtag != "":
            calibdic = {
                "Cone4Topo"  : "CaloH1WeightsCone4Topo-"+dbtag,
                "Cone4Tower" : "CaloH1WeightsCone4Tower-"+dbtag,
                "ConeTopo"   : "CaloH1WeightsConeTopo-"+dbtag,
                "ConeTower"  : "CaloH1WeightsConeTower-"+dbtag,
                "Kt4Topo"    : "CaloH1WeightsKt4Topo-"+dbtag,
                "Kt4Tower"   : "CaloH1WeightsKt4Tower-"+dbtag,
                "Kt6Topo"    : "CaloH1WeightsKt6Topo-"+dbtag,
                "Kt6Tower"   : "CaloH1WeightsKt6Tower-"+dbtag
                }
            return calibdic
        #-- default extraction
        else:
            from AthenaCommon.GlobalFlags import globalflags
            ddv = globalflags.DetDescrVersion()
            #-- establish relation between detector description and calibration
            if ddv.startswith("ATLAS-CSC"):
                if ddv >= "ATLAS-CSC-01-00-01":
                    return H1Calibration.calibdicV13
                else:
                    return H1Calibration.calibdicV12
            elif ddv.startswith("ATLAS-Comm"):
                return H1Calibration.calibdicV13
            else:
                return H1Calibration.calibdicV14
    #-- get calibration parameters 
    @staticmethod
    def getCalibDBParams(finder,mainparam,input, onlyCellWeight=False, isMC=False):
        """ Retrieves calibration DB parameters from jet finder specs, returns a triplet (key,folder,tag)
        """
        #-- adapt to DB convention
        input = input.replace('H1','')
        param = editParm(mainparam)
        if param == '7':
            param = ''
        #-- get dictionary
        caldict = H1Calibration.calibration_dict()

        calibtype = finder+param+input
        #-- specific calibration
        if calibtype in caldict:
            key    = 'H1Weights' + calibtype
            tag    = caldict[calibtype]
            if isMC:
               folder = '/CALO/Ofl/'
               tag = "CALOOfl"+tag[4:]
            else:   
               folder = '/CALO/'
            folder += 'H1Weights/' + key
            key    = folder
            return (key,folder,tag)
        #-- default calibration
        else:
            if onlyCellWeight: 
                # then we don't really care : just give back Cone4Tower or Cone4Topo
                # and avoid the warning below.
                if 'Topo' in input: return  H1Calibration.getCalibDBParams('Cone',0.4,'H1Topo',onlyCellWeight,isMC)
                else:               return  H1Calibration.getCalibDBParams('Cone',0.4,'H1Tower',onlyCellWeight,isMC)
            # else try to find a good fall back
            _logger.warning("getCalibDBParams: no dedicated calibration for %s %s %s", finder,mainparam,input)
            if finder not in [ 'Kt', 'Cone' ]    : finder = 'Cone'     # fall back to ATLAS Cone
            if input  not in [ 'Topo', 'Tower' ] : input  = 'Tower'    # fall back to Tower
            if finder == 'Kt' : l = [ 0.4, 0.6 ]
            else:               l = [ 0.4, 0.7 ]
            (m,mainparam) = min( [ (abs(p-mainparam),p) for p in l ] ) # main parameter optimization (??)
            _logger.warning("getCalibDBParams: defaulted calibration to %s %s %s", finder,mainparam,input)
            return H1Calibration.getCalibDBParams(finder,mainparam,input,onlyCellWeight,isMC)
    #-- load DB folder
    @staticmethod
    def loadCaloFolder(folder,tag,isMC=False):
        from IOVDbSvc.CondDB       import conddb
        from AthenaCommon.AppMgr   import ServiceMgr
        IOVDbSvc = CompFactory.IOVDbSvc  # IOVDbSvc
        ServiceMgr += IOVDbSvc()
        IOVDbSvc = ServiceMgr.IOVDbSvc
        if isMC:
           dbString="CALO_OFL"
        else:
           dbString="CALO"
        if (folder,tag) not in H1Calibration.loaded_folder:
            if H1Calibration.overrideFolder():
                conddb.addFolder(dbString,folder+'<tag>'+tag+'</tag>',
                                 className = 'CaloRec::ToolConstants')
            else:
                conddb.addFolder(dbString,folder,
                                 className = 'CaloRec::ToolConstants')

            H1Calibration.loaded_folder.append( (folder,tag) )
    
def getCellWeightTool(finder="Cone",mainparam=0.4,input="Topo", onlyCellWeight=False):
    """
    Returns a fully configured H1-style cell weighting calibration tool. This tool only uses cell weights!
    Parameters/type:
    finder/str     : name of jet finder
    input/str      : input objects triggers calibration weights
    mainparam/float: size parameter for jet
    """
    H1WeightToolCSC12Generic = CompFactory.H1WeightToolCSC12Generic  # CaloClusterCorrection
    
    if globalflags.DataSource()=='data':
      isMC=False
    else:  
      isMC=True
    #-- DB access  
    (key,folder,tag) = H1Calibration.getCalibDBParams(finder,mainparam,input, onlyCellWeight, isMC)
    H1Calibration.loadCaloFolder(folder,tag, isMC)
    #-- configure tool
    toolName = finder + editParm(mainparam) + input
    cellcalibtool = H1WeightToolCSC12Generic("H1Weight"+toolName)
    cellcalibtool.DBHandleKey = key
    # --
    return cellcalibtool
