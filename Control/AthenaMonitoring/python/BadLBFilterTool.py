# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from PyUtils.Decorators import memoize

def _resolve_db_tag(origDbTag):
    from AthenaCommon.GlobalFlags  import globalflags
    if origDbTag is not None:
        dbtag = origDbTag
    else:
        dbtag = globalflags.ConditionsTag()
        dbtag = dbtag if dbtag != '' else 'HEAD'
    print "RESOLVED AS TAG", dbtag
    return dbtag

#decide database instance based on project tag dataXX_
def _InstanceFromProjectName():
    from AthenaCommon.Logging import logging
    from RecExConfig.RecFlags import rec
    projectName=rec.projectName()
    try:
        year=int(projectName[4:6]);
    except:
        log = logging.getLogger('BadLBFilterTool')
        log.error("Failed to extract year from project tag "+ projectName+". Guessing run2")
        return "CONDBR2"
       
    if (year>13):
        return "CONDBR2"
    else:
        return "COMP200"

# Set up the bad lb filter tool
# Cache instances that are already created
@memoize
def GetBadLBFilterTool(name, defects, alwaysReturnTrue=False, ignoreRecoverable=False, autoconfigure=True, origDbTag=None):
    """
    Configure an instance of the bad LB filter tool.  If called twice with the same options, will return the same instance.
    Arguments:
        - name: name of instance to create
        - defects: the defects to use for LB rejection.  Tool will return false if any of these defects is present.  These may be virtual defects.  NOTE: the value you pass must be a 'hashable' type.  The best choice is to pass a tuple; you can create a tuple from any iterable object with tuple(obj).
        - alwaysReturnTrue (optional; default=False): if True, the tool will never reject any LBs.
        - ignoreRecoverable (optional; default=False): if True, the tool will ignore defects that are marked as recoverable
        - autoconfigure (optional; default=True): automatically handle certain cases, e.g. Monte Carlo, where we want to always return True
        - origDbTag (optional): if set, will override automatic configuration of database tag (only for testing)
    """
    import operator
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    from AthenaCommon.GlobalFlags  import globalflags
    from AthenaCommon.AppMgr import ToolSvc
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
        ToolSvc += monFilterTool
        return monFilterTool

    from AthenaMonitoring.AthenaMonitoringConf import DQBadLBFilterTool
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
        
    log.info ("Tool name %s to be configured with intolerable defects %s" % (name, list(primary_defects)))

    primary_defect_ids = [ddb.defect_id_map[id] for id in primary_defects]

    monFilterTool = DQBadLBFilterTool(name)
    monFilterTool.defectList = primary_defect_ids
    monFilterTool.alwaysReturnTrue = alwaysReturnTrue
    monFilterTool.ignoreRecoverable = ignoreRecoverable

    if globalflags.DataSource.get_Value() != 'geant4':
        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested('/GLOBAL/DETSTATUS/DEFECTS'):
            conddb.addFolder('GLOBAL_OFL', '/GLOBAL/DETSTATUS/DEFECTS')

    ToolSvc += monFilterTool
    return monFilterTool

def GetLArBadLBFilterTool(origDbTag=None):
    """
    Configure a LAr defect-specific instance of the bad LB filter tool.  If called twice with the same options, will return the same instance.
    Arguments:
        - origDbTag (optional): if set, will override automatic configuration of database tag (only for testing)
    """
    import operator
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
        from DQDefects import DefectsDB
        dbtag = _resolve_db_tag(origDbTag)
        dbname = _InstanceFromProjectName()
        ddb = DefectsDB('COOLOFL_GLOBAL/'+dbname, tag=dbtag)
        defects = ddb.defect_names
        defectliststr = []
        defectlist = []
        lar_defects = reduce(operator.or_, [set(ddb.virtual_defect_logics[z].clauses) for z in \
    ('EMBA', 'EMBC', 'EMECA', 'EMECC', 'HECA', 'HECC', 'FCALA', \
    'FCALC', 'LAR_GENERAL')])-set(['LAR_UNCHECKED','LAR_GENERAL'])
        lar_defects = set(defect for defect in lar_defects
                          if ('LAR' in defect and
                              (('HVTRIP' in defect) or ('SEV' in defect) or
                               ('CORRUPT' in defect))))

    return GetBadLBFilterTool('LArBadLBFilterTool', tuple(lar_defects),
                              ignoreRecoverable=True, origDbTag=origDbTag)
