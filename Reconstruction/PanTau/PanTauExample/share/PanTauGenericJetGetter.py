from JetRec.JetGetters import *
#from JetRec.JetRecConf import JetSetCalibStateTool
#from JetRecTools.JetRecToolsConf import JetSetCalibStateTool
from JetCalibTools.JetCalibToolsConf import JetCalibrationTool


def make_eflowJet(finder, param):

    #getStandardPostProcessTool(input, doCalib , **options):
    #mytoollist_AntiKt04 += getStandardPostProcessTool('Eflow', doCalib=False,addDefaultMoment=True)
    #mygetter_AntiKt04 = make_customJetGetter(getterDict_AntiKt04,
    #mytoollist_AntiKt04)

    # create tool list
    toolist = []
    # add standard jet finder(cone,kt,anti-kt,etc.) to tool list
    toolist += getStandardFinderTools(finder, param)
    # configure jet signal selector
    initT = JetSignalSelectorTool('JetInitialEtCut')
    initT.UseTransverseEnergy = True
    initT.MinimumSignal       = 0*GeV
    toolist += [initT]
    
    finalT = JetSignalSelectorTool('JetFinalEtCut')
    finalT.UseTransverseEnergy = True 
    finalT.UseTransverseMomentum = False
    finalT.UseEnergy = False
    finalT.MinimumSignal       = 7*GeV
    finalT.OutputLevel = 2
    toolist += [finalT]
    # ------------------------------------------------
    sortT = JetSorterTool('JetSorter')
    toolist += [sortT]
    sortT.SortOrder="ByEtDown"
    # ------------------------------------------------

    # ------------------------------------------------
    # append calibration tool to tool list
    #calibState = JetSetCalibStateTool('EflowCalibState')
    #calibState = JetSetCalibStateTool("eflowSetCalibState")
    calibTool = JetCalibrationTool('CalibDummy')
    #toolist += [calibTool]
    # ------------------------------------------------

    # create an attribute dictionnary
    #mydic = dict(customJetParams.getterDict)
    mydic = dict(jet_attribute_dict)
    mydic['_finderType'] = finder
    #mydic['_finderSuff'] = 'Bis'
    mydic['_finderParams'] = [param]
    mydic['_inputType'] = 'eflowObject'
    
    mydic['_inputColl'] = ["eflowObjects01Prime"]
        
    mygetter = make_customJetGetter(mydic, toolist)
    
    return mygetter




