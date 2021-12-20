# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.flagTestLOD import flagTestLOD, deferFlag
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags

def perigeeLOD (expr, flags):
    func = flagTestLOD (expr, flags)
    def perigeeLODFunc (reqlev, args, hookargs):
        if not func (reqlev, args, hookargs): return False
        if not args.get('FillPerigee',True) or hookargs.has_key ('_didTQP'):
            args['FillThetaAndQoverP'] = False
        else:
            args['FillThetaAndQoverP'] = True
            hookargs['_didTQP'] = True
        return True
    return perigeeLODFunc


def PerigeeAssociation (parent, associator, blockname, suffix='',
                        flags = TrackD3PDFlags,
                        fillMomName = 'False',
                        levelName = None,
                        **kw):

    perigeeAssoc = SimpleAssociation (parent, associator, **kw)

    perigeeAssoc.defineBlock (perigeeLOD ('%s or %s>0' %(fillMomName,levelName),
                                          flags),
                              blockname,
                              TrackD3PDMaker.PerigeeFillerTool,
                              Suffix = suffix,
                              FillPerigee = deferFlag('%s>0'%levelName,flags),
                              FillMomentum = deferFlag(fillMomName,flags))

    perigeeCovarianceAssoc = SimpleAssociation (
        perigeeAssoc,
        TrackD3PDMaker.PerigeeCovarianceAssociationTool,
        blockname = blockname + 'Assoc',
        level = flagTestLOD('%s>=2'%levelName,flags))

    perigeeCovarianceAssoc.defineBlock (
        flagTestLOD('%s>=2'%levelName,flags),
        blockname + 'Error',
        TrackD3PDMaker.CovarianceFillerTool,
        Error = deferFlag ('storeDiagonalCovarianceAsErrors', flags),
        DiagCovariance = deferFlag('not storeDiagonalCovarianceAsErrors',flags),
        OffDiagCovariance = False,
        IsTrackPerigee = True,
        Suffix = suffix)

    perigeeCovarianceAssoc.defineBlock (flagTestLOD('%s>=3'%levelName,flags),
                                        blockname + 'OffDiagCovariance',
                                        TrackD3PDMaker.CovarianceFillerTool,
                                        Error = False,
                                        DiagCovariance = False,
                                        OffDiagCovariance = True,
                                        IsTrackPerigee = True,
                                        Suffix = suffix)

    return perigeeAssoc
