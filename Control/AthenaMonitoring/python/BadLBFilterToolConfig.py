# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Logging import logging
from PyUtils.Decorators import memoize
from functools import reduce

#The memonize decorator makes sure that this piece of code isn't executed multiple times - as long as the name is the same
#The deduplication logic of the CA make sure that the algo isn't run mutliple time 
@memoize
def BadLBFilterAlgCfg(inputFlags,name, defects, writekey, ignoreRecoverable=False, origDbTag=None):
    log = logging.getLogger('BadLBFilterAlgCfg')
    result=ComponentAccumulator()

    from DQDefects import DefectsDB
    ddb = DefectsDB('COOLOFL_GLOBAL/' + inputFlags.IOVDb.DatabaseInstance, 
                    tag=origDbTag or inputFlags.IOVDb.GlobalTag)

    primary_defects = set()
    for defect in defects:
        if defect in ddb.defect_names:
            primary_defects.add(defect)
        elif defect in ddb.virtual_defect_names:
            primary_defects |= ddb.virtual_defect_logics(defect).primary_defects
        else:
            raise ValueError('%s is not a known defect' % defect)
        
    log.info ("Algorithm %s configured with intolerable defects %s" % (name, list(primary_defects)))

    primary_defect_ids = [ddb.defect_id_map[id] for id in primary_defects]

    monFilterAlg =CompFactory.DQBadLBFilterAlg(name)
    monFilterAlg.defectList = primary_defect_ids
    monFilterAlg.ignoreRecoverable = ignoreRecoverable
    monFilterAlg.WriteKey = writekey

    from IOVDbSvc.IOVDbSvcConfig import addFolders

    if not inputFlags.Input.isMC:
        result.merge(addFolders(inputFlags,'/GLOBAL/DETSTATUS/DEFECTS',detDb='GLOBAL_OFL',className='CondAttrListCollection'))

    result.addCondAlgo(monFilterAlg)
    return result



# Set up the bad lb filter tool
def BadLBFilterToolCfg(inputFlags,name, defects, alwaysReturnTrue=False, ignoreRecoverable=False, origDbTag=None):
    """
    Configure an instance of the bad LB filter tool.  If called twice with the same options, will return the same instance.
    Arguments:
        - name: name of *algorithm* instance to create
        - defects: the defects to use for LB rejection.  Tool will return false if any of these defects is present.  These may be virtual defects.  NOTE: the value you pass must be a 'hashable' type.  The best choice is to pass a tuple; you can create a tuple from any iterable object with tuple(obj).
        - alwaysReturnTrue (optional; default=False): if True, the tool will never reject any LBs.
        - ignoreRecoverable (optional; default=False): if True, the tool will ignore defects that are marked as recoverable
        - origDbTag (optional): if set, will override automatic configuration of database tag (only for testing)
    """
    log = logging.getLogger('BadLBFilterToolCfg')
    result=ComponentAccumulator()

    if inputFlags.Common.isOnline or inputFlags.Input.isMC:
        # if online or MC, we don't want to do any filtering, or even access
        log.info('Disabling bad LB filter tool due to inappropriate environment; returning dummy')
        result.setPrivateTools(CompFactory.DQDummyFilterTool(name))
        return result

    sgkey = 'DQBadLBFilterAlgResult_%s' % name
    #Schedule required cond-algo
    result.merge(BadLBFilterAlgCfg(inputFlags,name+'_Alg',defects,sgkey,ignoreRecoverable, origDbTag))

    monFilterTool=CompFactory.DQBadLBFilterTool(name,alwaysReturnTrue=alwaysReturnTrue,
                                                ReadKey= sgkey)
    result.setPrivateTools(monFilterTool)
    return result


@memoize
def LArDefectList(inputFlags,origDbTag=None):
    """
    Get the defects to configure for LAr - cache results to avoid lots of DB lookups
    Arguments:
        - origDbTag (optional): if set, will override automatic configuration of database tag (only for testing)
    """
    import operator
    from DQDefects import DefectsDB
    ddb = DefectsDB('COOLOFL_GLOBAL/' + inputFlags.IOVDb.DatabaseInstance, 
                    tag=origDbTag or inputFlags.IOVDb.GlobalTag)

    lar_defects = reduce(operator.or_, [set(ddb.virtual_defect_logics[z].clauses) for z in \
                                            ('EMBA', 'EMBC', 'EMECA', 'EMECC', 
                                             'HECA', 'HECC', 'FCALA',
                                             'FCALC')])-set(['LAR_UNCHECKED','LAR_GENERAL','DISABLED','NONGREEN'])
    lar_defects = set(defect for defect in lar_defects
                      if ('LAR' in defect and
                          (('HVTRIP' in defect) or ('SEV' in defect) or
                           ('CORRUPT' in defect))))
    return lar_defects


def LArBadLBFilterToolCfg(inputFlags,origDbTag=None):
    """
    Configure a LAr defect-specific instance of the bad LB filter tool.  If called twice with the same options, will return the same instance.
    Arguments:
        - origDbTag (optional): if set, will override automatic configuration of database tag (only for testing)
    """
    from AthenaCommon.Logging import logging
    log = logging.getLogger('BadLBFilterTool/LAr')

    if inputFlags.Common.isOnline or inputFlags.Input.isMC:
        log.info('Disabling LAr bad LB filter tool due to inappropriate environment')
        lar_defects= []
    else:
        lar_defects = LArDefectList(inputFlags,origDbTag)

    return BadLBFilterToolCfg(inputFlags,'LArBadLBFilterTool', tuple(lar_defects),
                              origDbTag=origDbTag)
