# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.diff_root_files
# @purpose check that 2 ROOT files have same content (containers and sizes).
# @author Sebastien Binet
# @date February 2010

__version__ = "$Revision: 543921 $"
__doc__ = "check that 2 ROOT files have same content (containers and sizes)."
__author__ = "Sebastien Binet"

### imports -------------------------------------------------------------------
import PyUtils.acmdlib as acmdlib
import PyUtils.RootUtils as ru
ROOT = ru.import_root()

### globals -------------------------------------------------------------------
g_ALLOWED_MODES = ('summary', 'detailed')
g_ALLOWED_ERROR_MODES = ('bailout', 'resilient')
g_args = None

### classes -------------------------------------------------------------------

### functions -----------------------------------------------------------------
def _is_summary():
    global g_args
    return g_args.mode == 'summary'

def _is_exit_early():
    global g_args
    return g_args.error_mode == 'bailout'

@acmdlib.command(name='diff-root')
@acmdlib.argument('old',
                  help='path to the reference ROOT file to analyze')
@acmdlib.argument('new',
                  help='path to the ROOT file to compare to the reference')
@acmdlib.argument('-t', '--tree-name',
                  default='CollectionTree',
                  help='name of the TTree to compare')
@acmdlib.argument('--ignore-leaves',
                  nargs='*',
                  default=('Token',),
                  help='set of leaves names to ignore from comparison')
@acmdlib.argument('--enforce-leaves',
                  nargs='*',
                  default=('BCID',),
                  help='set of leaves names we make sure to compare')
@acmdlib.argument('--known-hacks',
                  nargs='*',
                  default=('m_athenabarcode', 'm_token',),
                  help='set of leaves which are known to fail (but should be fixed at some point) [default: %(default)s]')
@acmdlib.argument('--entries',
                  default='',
                  help='a list of entries (indices, not event numbers) or an expression (like range(3) or 0,2,1 or 0:3) leading to such a list, to compare.')
@acmdlib.argument('-v', '--verbose',
                  action='store_true',
                  default=False,
                  help="""Enable verbose printout""")
@acmdlib.argument('--mode',
                  choices=g_ALLOWED_MODES,
                  default='detailed',
                  help="""\
Enable a particular mode.
  'summary': only report the number of differences.
  'detailed': display everything.
default='%(default)s'.
allowed: %(choices)s
"""
                  )
@acmdlib.argument('--error-mode',
                  choices=g_ALLOWED_ERROR_MODES,
                  default='bailout',
                  help="""\
Enable a particular error mode.
  'bailout':   bail out on first error.
  'resilient': keep running.
default='%(default)s'.
allowed: %(choices)s
"""
                  )
def main(args):
    """check that 2 ROOT files have same content (containers and sizes)
    """
    global g_args
    g_args = args
    
    import PyUtils.RootUtils as ru
    root = ru.import_root()

    import PyUtils.Logging as L
    msg = L.logging.getLogger('diff-root')
    msg.setLevel(L.logging.INFO)

    if args.entries == '':
        args.entries = -1
        
    msg.info('comparing tree [%s] in files:', args.tree_name)
    msg.info(' old: [%s]', args.old)
    msg.info(' new: [%s]', args.new)
    msg.info('ignore  leaves: %s', args.ignore_leaves)
    msg.info('enforce leaves: %s', args.enforce_leaves)
    msg.info('hacks:          %s', args.known_hacks)
    msg.info('entries:        %s', args.entries)
    msg.info('mode:           %s', args.mode)
    msg.info('error mode:     %s', args.error_mode)

    import PyUtils.Helpers as H
    with H.ShutUp() :
        fold = ru.RootFileDumper(args.old, args.tree_name)
        fnew = ru.RootFileDumper(args.new, args.tree_name)
        pass
    
    def tree_infos(tree, args):
        nentries = tree.GetEntriesFast()
        leaves = [l.GetName() for l in tree.GetListOfLeaves()
                  if l not in args.ignore_leaves]
        return {
            'entries' : nentries,
            'leaves': set(leaves),
            }
    
    def diff_tree(fold, fnew, args):
        infos = {
            'old' : tree_infos(fold.tree, args),
            'new' : tree_infos(fnew.tree, args),
            }

        nentries = min(infos['old']['entries'],
                       infos['new']['entries'])
        itr_entries = nentries
        if args.entries in (-1,'','-1'):
            #msg.info('comparing over [%s] entries...', nentries)
            itr_entries = nentries
            if infos['old']['entries'] != infos['new']['entries']:
                msg.info('different numbers of entries:')
                msg.info(' old: [%s]', infos['old']['entries'])
                msg.info(' new: [%s]', infos['new']['entries'])
                msg.info('=> comparing [%s] first entries...', nentries)
        else:
            itr_entries = args.entries
            pass
        msg.info('comparing over [%s] entries...', itr_entries)
        
        leaves = infos['old']['leaves'] & infos['new']['leaves']
        diff_leaves = infos['old']['leaves'] - infos['new']['leaves']
        if diff_leaves:
            msg.info('the following variables exist in only one tree !')
            for l in diff_leaves:
                msg.info(' - [%s]', l)
        leaves = leaves - set(args.ignore_leaves)
        
        msg.info('comparing [%s] leaves over entries...', len(leaves))
        all_good = True
        n_good = 0
        n_bad = 0
        import collections
        from itertools import izip
        summary = collections.defaultdict(int)
        for d in izip(fold.dump(args.tree_name, itr_entries),
                      fnew.dump(args.tree_name, itr_entries)):
            tree_name, ientry, name, iold = d[0]
            _,              _,    _, inew = d[1]
            name[0] = name[0].rstrip('\0')
            if ((not (name[0] in leaves)) or
                # FIXME: that's a plain (temporary?) hack
                name[-1] in args.known_hacks):
                continue
            
            if d[0] == d[1]:
                diff = False
                n_good += 1
                continue
            n_bad += 1
            diff = True

            in_synch = d[0][:-1] == d[1][:-1]
            if not in_synch:
                if not _is_summary():
                    print '::sync-old %s' % \
                          '.'.join(["%03i"%ientry]+map(str, d[0][2]))
                    print '::sync-new %s' % \
                          '.'.join(["%03i"%ientry]+map(str, d[1][2]))
                    pass
                summary[name[0]] += 1
                # remember for later
                fold.allgood = False
                fnew.allgood = False

                if _is_exit_early():
                    print "*** exit on first error ***"
                    break
                continue
            
            n = '.'.join(map(str, ["%03i"%ientry]+name))
            diff_value = 'N/A'
            try:
                diff_value = 50.*(iold-inew)/(iold+inew)
                diff_value = '%.8f%%' % (diff_value,)
            except Exception:
                pass
            if not _is_summary():
                print '%s %r -> %r => diff= [%s]' %(n, iold, inew, diff_value)
                pass
            summary[name[0]] += 1

            if name[0] in args.enforce_leaves:
                msg.info("don't compare further")
                all_good = False
                break
            pass # loop over events/branches
        
        msg.info('Found [%s] identical leaves', n_good)
        msg.info('Found [%s] different leaves', n_bad)

        if not _is_summary():
            keys = sorted(summary.keys())
            for n in keys:
                v = summary[n]
                msg.info(' [%s]: %i leaves differ', n, v)
                pass
            pass
        
        if (not fold.allgood) or (not fnew.allgood):
            msg.info('NOTE: there were errors during the dump')
            msg.info('fold.allgood: %s' % fold.allgood)
            msg.info('fnew.allgood: %s' % fnew.allgood)
            n_bad += 0.5
        return n_bad
    
    ndiff = diff_tree(fold, fnew, args)
    if ndiff != 0:
        msg.info('files differ!')
        return 2
    msg.info('all good.')
    return 0
