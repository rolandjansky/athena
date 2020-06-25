# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.ath_dump
# @purpose entry point for ath-dump command, the dump-athfile cousin
# @author Sebastien Binet
# @date January 2010

from __future__ import print_function

__author__ = "Sebastien Binet"
__doc__ = "entry point for ath-dump command, the dump-athfile cousin"

### imports -------------------------------------------------------------------
import PyUtils.acmdlib as acmdlib

@acmdlib.command(name='ath-dump')
@acmdlib.argument('files', nargs='+',
                  help='path to POOL or BS file(s) to analyse')
@acmdlib.argument('-o', '--output',
                  default='athfile-infos.ascii',
                  help="""Name of the output file which will contain the
                  informations gathered during AthFile processing.
                  These informations will be stored into a python-shelve file.
                  """)
@acmdlib.argument('--evtmax',
                  default=1,
                  type=int,
                  help="""Maximum number of events to process in each file""")
def main(args):
    """simple command-line utility wrapping PyUtils.AthFile.fopen
    """
    exitcode = 0
    fnames = args.files
    if isinstance(fnames, str):
        fnames = [fnames]

    import sys
    import os
    import os.path as osp
    for i,f in enumerate(fnames):
        fnames[i] = osp.expandvars(osp.expanduser(f))

    import PyUtils.AthFile as af
    msg = af.msg
    for fname in fnames:
        try:
            f = af.fopen(fname, evtmax=args.evtmax)
            msg.info(':'*80)
            msg.info('::::: summary :::::')
            fmt = ' - %-15s: %s'
            print (fmt % ('file md5',       f.infos['file_md5sum']))
            print (fmt % ('file name',      f.infos['file_name']))
            print (fmt % ('file type',      f.infos['file_type']))
            print (fmt % ('file guid',      f.infos['file_guid']))
            print (fmt % ('nentries',       f.infos['nentries']))
            print (fmt % ('run number',     f.infos['run_number']))
            print (fmt % ('run type',       f.infos['run_type']))
            print (fmt % ('evt number',     f.infos['evt_number']))
            print (fmt % ('evt type',       f.infos['evt_type']))
            print (fmt % ('lumi block',     f.infos['lumi_block']))
            print (fmt % ('beam energy',    f.infos['beam_energy']))
            print (fmt % ('beam type',      f.infos['beam_type']))
            print (fmt % ('stream tags',    f.infos['stream_tags']))
            print (fmt % ('stream names',   f.infos['stream_names']))
            print (fmt % ('geometry',       f.infos['geometry']))
            print (fmt % ('conditions tag', f.infos['conditions_tag']))
            _metadata = f.infos['metadata']
            _metadata = _metadata.keys() if isinstance(_metadata,dict) else None
            print (fmt % ('meta data',      _metadata))

            msg.info(':'*80)
        except Exception as e:
            msg.error("Caught exception [%s] !!", str(e.__class__))
            msg.error("What:\n%s\n%s\n%s",e,
                      sys.exc_info()[0],
                      sys.exc_info()[1])
            import traceback
            traceback.print_exc()
            exitcode = 1
            pass

        except Exception:
            msg.error("Caught something !! (don't know what)")
            msg.error("\n%s\n%s",sys.exc_info()[0], sys.exc_info()[1])
            exitcode = 10
            pass
        if len(fnames) > 1:
            print ("")
        pass # loop over fileNames
    
    if args.output:
        oname = args.output
        msg.info("saving report into [%s]...", oname)
        if osp.exists(oname):
            os.rename(oname, oname+'.bak')
        af.server.save_cache(oname)

    msg.info("Bye.")
    return exitcode


