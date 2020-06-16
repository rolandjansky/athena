# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.merge_files
# @purpose take a bunch of input (pool/bs) files and produce a single one
#          autoconfiguration is (attempted to be) performed
# @author Sebastien Binet
# @date February 2010

__doc__ = "merge multiple input (pool/bs) files"
__author__ = "Sebastien Binet"


### imports -------------------------------------------------------------------
import PyUtils.acmdlib as acmdlib

@acmdlib.command(
    name='merge-files'
    )
@acmdlib.argument(
    '-o', '--output',
    required=True,
    help="Name of the merged output file"
    )
@acmdlib.argument(
    'files',
    nargs='+',
    help='path to the input (pool/bs) files'
    )
@acmdlib.argument(
    '--evts',
    type=int,
    default=-1,
    help="number of events to process"
    )
@acmdlib.argument(
    '--logfile',
    default='<stdout>',
    help = "Path to a file where to put athena job's logfile"
    )
def main(args):
    """merge multiple input (pool/bs) files"""

    exitcode = 0

    import PyUtils.Logging as L
    msg = L.logging.getLogger('merge-files')
    msg.setLevel(L.logging.INFO)

    msg.info(':'*40)

    import os.path as osp
    args.files = [ osp.expandvars(osp.expanduser(fname))
                   for fname in args.files ]

    msg.info('input files: %s', args.files)
    msg.info('output file: %s', args.output)
    msg.info('evts to process: %s', args.evts)
    msg.info('log-files: %s', args.logfile)

    import AthenaCommon.ChapPy as api
    app = api.AthenaApp()
    app << """
    from AthenaCommon.AthenaCommonFlags import jobproperties as jp
    acf = jp.AthenaCommonFlags
    acf.FilesInput = %(files)s

    # events to process
    acf.EvtMax = EvtMax = theApp.EvtMax = %(evts)s

    # configuration
    import AthenaPython.ConfigLib as apcl
    cfg = apcl.AutoCfg(
        name='merge-files',
        input_files=acf.FilesInput(),
        output_file='%(output)s')

    for type_name in ('evgen',
                      'hits',
                      'rdo',
                      'esd',
                      'aod',
                      'tag',
                      'usr',):
        res = getattr(cfg, 'is_' + type_name)()
        if res:
            cfg.msg.info('input file type is ['+type_name+']')
            break
    else:
        cfg.msg.info('input file stream is of unknown type')
        cfg.msg.info('autoconfiguration might not work...')
        pass
        
    cfg.configure_job()

    if (cfg.is_rdo() or
        cfg.is_esd() or
        cfg.is_aod()):
        # main jobos
        include ('RecExCond/RecExCommon_flags.py')
        # FIXME: work-around for bug #56185
        from AthenaCommon.DetFlags import DetFlags
        DetFlags.makeRIO.all_setOff()
        # FIXME -- end
        include ('RecExCommon/RecExCommon_topOptions.py')
        
    """ % args.__dict__

    stdout = args.logfile
    if stdout.lower() in ('<stdout>', 'stdout',):
        stdout = None
    else:
        stdout = open(stdout, 'w')

    exitcode = app.run(stdout=stdout)
    return exitcode
