# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.dump_root_file
# @purpose ascii-fy a ROOT file
# @author Sebastien Binet
# @date December 2010

from __future__ import print_function

__doc__ = "ASCII-fy a ROOT file"
__author__ = "Sebastien Binet"


### imports -------------------------------------------------------------------
import PyUtils.acmdlib as acmdlib

@acmdlib.command(name='dump-root')
@acmdlib.argument('fname',
                  help='path to the ROOT file to dump')
@acmdlib.argument('-t', '--tree-name',
                  default=None,
                  help='name of the TTree to dump (default:all)')
@acmdlib.argument('--entries',
                  default=None,
                  help="""a list of entries (indices, not event numbers) or an expression leading to such a list, to compare (default:all).
                  ex: --entries='0:10' to get the first 10 events
                      --entries='10:20:2' to get the even events between 10 and 20
                      --entries='range(10)' to get the first 10 events
                      --entries=10 to get the first 10 events
                      --entries=0,2,1 to get the entry 0, then 2 then 1
                  """)
@acmdlib.argument('-v', '--verbose',
                  action='store_true',
                  default=False,
                  help="""Enable verbose printout""")
def main(args):
    """dump the content of a ROOT file into an ASCII format.
    """

    import PyUtils.RootUtils as ru
    root = ru.import_root()

    _inspect = root.RootUtils.PyROOTInspector.pyroot_inspect2  # noqa: F841

    import PyUtils.Logging as L
    msg = L.logging.getLogger('dump-root')
    msg.setLevel(L.logging.INFO)

    msg.info('fname: [%s]', args.fname)
    root_file = root.TFile.Open(args.fname)
    if (root_file is None or
        not isinstance(root_file, root.TFile) or not root_file.IsOpen()):
        msg.error('could not open [%s]', args.fname)
        return 1

    tree_names = []
    if args.tree_name:
        tree_names = args.tree_name.split(',')
    else:
        tree_names = []
        keys = [k.GetName() for k in root_file.GetListOfKeys()]
        for k in keys:
            o = root_file.Get(k)
            if isinstance(o, root.TTree):
                tree_names.append(k)
                
    msg.info('dumping trees:  %s', tree_names)

    for tree_name in tree_names:
        f = ru.RootFileDumper(args.fname, tree_name)
        nentries = f.tree.GetEntries()
        if args.entries:
            nentries = args.entries
        for d in f.dump(tree_name, nentries):
            tree_name, ientry, name, data = d
            n = '.'.join(map(str, [tree_name,"%03i"%ientry]+name))
            print ('%s %r' %(n, data))
    return 0
