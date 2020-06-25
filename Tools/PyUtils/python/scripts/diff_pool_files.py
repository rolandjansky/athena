# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.diff_pool_files
# @purpose check that 2 POOL files have same content (containers and sizes).
# @author Sebastien Binet
# @date February 2010

__doc__ = "diff two POOL files (containers and sizes)"
__author__ = "Sebastien Binet"


### imports -------------------------------------------------------------------
import PyUtils.acmdlib as acmdlib

@acmdlib.command(name='diff-pool')
@acmdlib.argument('old',
                  help='path to the reference POOL file to analyze')
@acmdlib.argument('new',
                  help='path to the POOL file to compare to the reference')
@acmdlib.argument('-v', '--verbose',
                  action='store_true',
                  default=False,
                  help="""Enable verbose printout""")
def main(args):
    """diff two POOL files (containers and sizes)"""

    import os.path as osp
    old = osp.expandvars(osp.expanduser(args.old))
    new = osp.expandvars(osp.expanduser(args.new))

    import PyUtils.PoolFile as PF
    diff = PF.DiffFiles(refFileName = old,
                        chkFileName = new,
                        verbose = args.verbose)
    diff.printSummary()
    return diff.status()
