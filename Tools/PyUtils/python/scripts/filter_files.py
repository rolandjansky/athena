# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.filter_files
# @purpose take a bunch of input (pool/bs) files and produce a filtered one
#          autoconfiguration is (attempted to be) performed
# @author Sebastien Binet
# @date March 2010
from __future__ import with_statement

__version__ = "$Revision: 523884 $"
__doc__ = "take a bunch of input (pool/bs) files and produce a filtered one"
__author__ = "Sebastien Binet"


### imports -------------------------------------------------------------------
import PyUtils.acmdlib as acmdlib

@acmdlib.command(
    name='filter-files'
    )
@acmdlib.argument(
    '-o', '--output',
    required=True,
    help="Name of the filtered output file"
    )
@acmdlib.argument(
    'files',
    nargs='+',
    help='path to the input (pool/bs) files'
    )
@acmdlib.argument(
    '-s', '--selection',
    required=True,
    help='comma separated list of tuples (run,event) numbers to select or an ascii file containg a list of such run+event numbers to select'
    )
def main(args):
    """take a bunch of input (pool/bs) files and produce a filtered one
    """
    exitcode = 0

    import PyUtils.Logging as L
    msg = L.logging.getLogger('filter-files')
    msg.setLevel(L.logging.INFO)

    msg.info(':'*40)
    msg.info('welcome to filter-files version %s', __version__)

    import os.path as osp
    args.files = [ osp.expandvars(osp.expanduser(fname))
                   for fname in args.files ]

    args.selection = osp.expandvars(osp.expanduser(args.selection))
    
    msg.info('input files: %s', args.files)
    msg.info('output file: %s', args.output)
    msg.info('selection:   %s', args.selection)

    import os
    if os.path.exists(args.selection):
        selection = []
        with open(args.selection, 'r') as s:
            for line in s:
                if line.strip().startswith('#'):
                    continue
                l = line.strip().split()
                if len(l)==1: # assume this is only the event number
                    runnbr, evtnbr = None, long(l[0])
                elif len(l)==2: # a pair (run,evt) number
                    runnbr, evtnbr = long(l[0]), long(l[1])
                else:
                    raise RuntimeError(
                        'file [%s] has invalid format at line:\n%r' %
                        (args.selection, line)
                        )
                selection.append((runnbr, evtnbr))
    else:
        try:
            args.selection = eval(args.selection)
        except Exception,err:
            msg.error('caught:\n%s', err)
            msg.error('.. while trying to parse selection-string')
            return 1
        
        selection = []
        for item in args.selection:
            if not isinstance(item, (tuple, list, int, long)):
                raise TypeError('type: %r' % type(item))

            if isinstance(item, (tuple, list)):
                if len(item) == 1:
                    runnbr, evtnbr = None, long(item[0])
                elif len(item) == 2:
                    runnbr, evtnbr = long(item[0]), long(item[1])
                else:
                    raise RuntimeError(
                        'item [%s] has invalid arity (%s)' %
                        (item, len(item))
                        )
            else:
                runnbr, evtnbr = None, long(item)
            selection.append((runnbr, evtnbr))

    # put back the massaged selection into our workspace
    args.selection = selection[:]
    
    import PyUtils.AthFile as af
    fi = af.fopen(args.files[0]).infos
    af.save_cache()
    
    if fi['file_type'] == 'bs':
        # optimization: run directly 'AtlCopyBSEvent.exe
        import subprocess
        cmd = ' '.join([
            'AtlCopyBSEvent',
            '-e %(evt-list)s',
            '%(run-list)s',
            '--out %(output)s',
            '%(files)s',
            ])
        evt_list = [str(i) for _,i in args.selection]
        run_list = [str(i) for i,_ in args.selection if not i is None]
        cmd = cmd % {
            'evt-list': ','.join(evt_list),
            'run-list': '' if len(run_list)==0 else '-r '+','.join(run_list),
            'output': args.output,
            'files':  ' '.join(args.files),
            }
        return subprocess.call(cmd.split())
    
    import AthenaCommon.ChapPy as api
    app = api.AthenaApp()
    app << """
    import AthenaCommon.Constants as Lvl
    from AthenaCommon.AthenaCommonFlags import jobproperties as jp
    acf = jp.AthenaCommonFlags
    acf.FilesInput = %(files)s

    # events to process
    acf.EvtMax = EvtMax = theApp.EvtMax = -1

    # configuration
    import AthenaPython.ConfigLib as apcl
    cfg = apcl.AutoCfg(
        name='filter-files',
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
        
    # add the filtering algorithm
    # get a handle on the job main sequence
    import AthenaCommon.AlgSequence as acas
    job = acas.AlgSequence()
    
    ## filter configuration ##
    ##  -> we use the special sequence 'AthMasterSeq' which
    ##      is run before any other algorithm (which are usually in the
    ##      'TopAlg' sequence
    seq = acas.AthSequencer('AthMasterSeq')
    
    import GaudiSequencer.PyComps as gspc
    seq += gspc.PyEvtFilter(
       'filter_pyalg',
       # the store-gate key. leave as an empty string to take any eventinfo instance
       evt_info=None,
       OutputLevel=Lvl.INFO)
    seq.filter_pyalg.evt_list = %(selection)s   


    cfg.configure_job()

    if (cfg.is_rdo() or
        cfg.is_esd() or
        cfg.is_aod()):
        # main jobos
        include ('RecExCond/RecExCommon_flags.py')
        include ('RecExCommon/RecExCommon_topOptions.py')

    """ % args.__dict__
    
    stdout = None
    exitcode = app.run(stdout=stdout)
    return exitcode
