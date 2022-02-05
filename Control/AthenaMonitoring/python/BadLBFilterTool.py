# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from functools import reduce, cache

def _resolve_db_tag(origDbTag):
    from AthenaCommon.GlobalFlags  import globalflags
    if origDbTag is not None:
        dbtag = origDbTag
    else:
        dbtag = globalflags.ConditionsTag()
        dbtag = dbtag if dbtag != '' else 'HEAD'
    print("RESOLVED AS TAG %s" % dbtag)
    return dbtag

#decide database instance based on project tag dataXX_
def _InstanceFromProjectName():
    from AthenaCommon.Logging import logging
    from RecExConfig.RecFlags import rec
    projectName=rec.projectName()
    try:
        year=int(projectName[4:6])
    except Exception:
        log = logging.getLogger('BadLBFilterTool')
        log.error("Failed to extract year from project tag "+ projectName+". Guessing run2")
        return "CONDBR2"
       
    if (year>13):
        return "CONDBR2"
    else:
        return "COMP200"

# Set up the bad lb filter conditions algorithm
# Cache instance once already created
@cache
def GetBadLBFilterAlg(name, defects, writekey, ignoreRecoverable=False, origDbTag=None):
    """
    Configure an instance of the bad LB filter conditions algorithm. Not intended to be called directly by users.
    Arguments:
        - name: name of instance to create
        - defects: the defects to use for LB rejection.  Tool will return false if any of these defects is present.  These may be virtual defects.  NOTE: the value you pass must be a 'hashable' type.  The best choice is to pass a tuple; you can create a tuple from any iterable object with tuple(obj).
        - ignoreRecoverable (optional; default=False): if True, the algorithm will ignore defects that are marked as recoverable
        - origDbTag (optional): if set, will override automatic configuration of database tag (only for testing)
    """
    from AthenaCommon.GlobalFlags  import globalflags
    from AthenaCommon.Logging import logging
    from AthenaCommon.AlgSequence import AthSequencer 
    log = logging.getLogger('BadLBFilterAlg')

    from AthenaMonitoring.AthenaMonitoringConf import DQBadLBFilterAlg
    from DQDefects import DefectsDB
    dbtag = _resolve_db_tag(origDbTag)
    dbname=_InstanceFromProjectName()
    ddb = DefectsDB('COOLOFL_GLOBAL/' + dbname, tag=dbtag)

    primary_defects = set()
    for defect in defects:
        if defect in ddb.defect_names:
            primary_defects.add(defect)
        elif defect in ddb.virtual_defect_names:
            primary_defects |= ddb.virtual_defect_logics(defect).primary_defects
        else:
            raise ValueError('%s is not a known defect' % defect)
        
    log.info ("Algorithm name %s to be configured with intolerable defects %s" % (name, list(primary_defects)))

    primary_defect_ids = [ddb.defect_id_map[id] for id in primary_defects]

    monFilterAlg = DQBadLBFilterAlg(name)
    monFilterAlg.defectList = primary_defect_ids
    monFilterAlg.ignoreRecoverable = ignoreRecoverable
    monFilterAlg.WriteKey = writekey

    if globalflags.DataSource.get_Value() != 'geant4':
        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested('/GLOBAL/DETSTATUS/DEFECTS'):
            conddb.addFolder('GLOBAL_OFL', '/GLOBAL/DETSTATUS/DEFECTS',
                             className='CondAttrListCollection')

    condSequence = AthSequencer("AthCondSeq")
    condSequence += monFilterAlg
    return monFilterAlg

# Set up the bad lb filter tool
def GetBadLBFilterTool(name, defects, alwaysReturnTrue=False, ignoreRecoverable=False, autoconfigure=True, origDbTag=None):
    """
    Configure an instance of the bad LB filter tool.  If called twice with the same options, will return the same instance.
    Arguments:
        - name: name of *algorithm* instance to create
        - defects: the defects to use for LB rejection.  Tool will return false if any of these defects is present.  These may be virtual defects.  NOTE: the value you pass must be a 'hashable' type.  The best choice is to pass a tuple; you can create a tuple from any iterable object with tuple(obj).
        - alwaysReturnTrue (optional; default=False): if True, the tool will never reject any LBs.
        - ignoreRecoverable (optional; default=False): if True, the tool will ignore defects that are marked as recoverable
        - autoconfigure (optional; default=True): automatically handle certain cases, e.g. Monte Carlo, where we want to always return True
        - origDbTag (optional): if set, will override automatic configuration of database tag (only for testing)
    """
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    from AthenaCommon.GlobalFlags  import globalflags
    from AthenaCommon.Logging import logging
    log = logging.getLogger('BadLBFilterTool')

    # if online or MC, we don't want to do any filtering, or even access
    # the DBs
    if autoconfigure and (DQMonFlags.monManEnvironment == 'online'
                          or globalflags.DataSource.get_Value() == 'geant4'
                          ):
        log.info('Disabling bad LB filter tool due to inappropriate environment; returning dummy')
        from AthenaMonitoring.AthenaMonitoringConf import DQDummyFilterTool
        monFilterTool = DQDummyFilterTool(name)
        return monFilterTool

    sgkey = 'DQBadLBFilterAlgResult_%s' % name
    GetBadLBFilterAlg('DQBadLBFilterAlg_%s' % name, 
                      defects, sgkey, ignoreRecoverable, origDbTag)
    from AthenaMonitoring.AthenaMonitoringConf import DQBadLBFilterTool

    monFilterTool = DQBadLBFilterTool(name)
    monFilterTool.alwaysReturnTrue = alwaysReturnTrue
    monFilterTool.ReadKey = sgkey

    return monFilterTool

@cache
def LArBadDefectList(origDbTag=None):
    """
    Get the defects to configure for LAr - cache results to avoid lots of DB lookups
    Arguments:
        - origDbTag (optional): if set, will override automatic configuration of database tag (only for testing)
    """
    import operator
    from DQDefects import DefectsDB
    dbtag = _resolve_db_tag(origDbTag)
    dbname = _InstanceFromProjectName()
    ddb = DefectsDB('COOLOFL_GLOBAL/'+dbname, tag=dbtag)
    lar_defects = reduce(operator.or_, [set(ddb.virtual_defect_logics[z].clauses) for z in \
                                            ('EMBA', 'EMBC', 'EMECA', 'EMECC', 
                                             'HECA', 'HECC', 'FCALA',
                                             'FCALC')])-set(['LAR_UNCHECKED','LAR_GENERAL','DISABLED','NONGREEN'])
    lar_defects = set(defect for defect in lar_defects
                          if ('LAR' in defect and
                              (('HVTRIP' in defect) or ('SEV' in defect) or
                               ('CORRUPT' in defect))))
    return lar_defects

def GetLArBadLBFilterTool(origDbTag=None):
    """
    Configure a LAr defect-specific instance of the bad LB filter tool.  If called twice with the same options, will return the same instance.
    Arguments:
        - origDbTag (optional): if set, will override automatic configuration of database tag (only for testing)
    """
    from AthenaCommon.Logging import logging
    log = logging.getLogger('BadLBFilterTool/LAr')

    # if online or MC, we don't want to do any filtering, or even access
    # the DBs
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    from AthenaCommon.GlobalFlags  import globalflags
    if (DQMonFlags.monManEnvironment == 'online'
        or globalflags.DataSource.get_Value() == 'geant4'
        ):
        log.info('Disabling LAr bad LB filter tool due to inappropriate environment')
        lar_defects = []
    else:
        lar_defects = LArBadDefectList(origDbTag)

    return GetBadLBFilterTool('LArBadLBFilterTool', tuple(lar_defects),
                              origDbTag=origDbTag)
