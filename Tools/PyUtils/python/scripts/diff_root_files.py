# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.diff_root_files
# @purpose check that 2 ROOT files have same content (containers and sizes).
# @author Sebastien Binet
# @date February 2010

__version__ = "$Revision: 681245 $"
__doc__ = "check that 2 ROOT files have same content (containers and sizes)."
__author__ = "Sebastien Binet"

### imports -------------------------------------------------------------------
import PyUtils.acmdlib as acmdlib

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
                  help='set of leaves names to ignore from comparison; can be a branch name or a partial leaf name (without any trailing dots)')
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
    if args.verbose:
        msg.setLevel(L.logging.VERBOSE)
    else:
        msg.setLevel(L.logging.INFO)

    from PyUtils.Helpers import ShutUp

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
        # l.GetBranch().GetName() gives the full leaf path name
        leaves = [l.GetBranch().GetName() for l in tree.GetListOfLeaves()
                  if l.GetBranch().GetName() not in args.ignore_leaves]
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
                
        old_leaves = infos['old']['leaves'] - infos['new']['leaves']
        if old_leaves:
            msg.warning('the following variables exist only in the old file !')
            for l in old_leaves:
                msg.warning(' - [%s]', l)
        new_leaves = infos['new']['leaves'] - infos['old']['leaves']
        if new_leaves:
            msg.warning('the following variables exist only in the new file !')
            for l in new_leaves:
                msg.warning(' - [%s]', l)

        # need to remove trailing dots as they confuse reach_next()
        skip_leaves = [ l.rstrip('.') for l in old_leaves | new_leaves | set(args.ignore_leaves) ]
        for l in skip_leaves:
            msg.debug('skipping [%s]', l)

        leaves = infos['old']['leaves'] & infos['new']['leaves']
        msg.info('comparing [%s] leaves over entries...', len(leaves))
        all_good = True
        n_good = 0
        n_bad = 0
        import collections
        from itertools import izip
        summary = collections.defaultdict(int)

        old_dump_iter = fold.dump(args.tree_name, itr_entries)
        new_dump_iter = fnew.dump(args.tree_name, itr_entries)

        def leafname_fromdump(entry):
            return '.'.join([s for s in entry[2] if not s.isdigit()])
        
        def reach_next(dump_iter, skip_leaves):
            keep_reading = True
            while keep_reading:
                try:
                    entry = dump_iter.next()
                except StopIteration:
                    return None
                entry[2][0] = entry[2][0].rstrip('.\0')  # clean branch name
                name = []
                skip = False
                for n in leafname_fromdump(entry).split('.'):
                    name.append(n)
                    if '.'.join(name) in skip_leaves or n in skip_leaves:
                        skip = True
                        break
                if not skip:
                    return entry
                # print 'SKIP:', leafname_fromdump(entry)
            pass

        read_old = True
        read_new = True
        d_old = None
        d_new = None
        
        while True:
            if read_old:
                prev_d_old = d_old
                d_old = reach_next(old_dump_iter, skip_leaves)
            if read_new:
                prev_d_new = d_new
                d_new = reach_next(new_dump_iter, skip_leaves)
                
            if not d_new and not d_old:
                break
            
            read_old = True
            read_new = True
            if d_old == d_new:
                n_good += 1
                continue
            
            if d_old:    
                tree_name, ientry, name, iold = d_old
            if d_new:
                tree_name, ientry, name, inew = d_new

            # FIXME: that's a plain (temporary?) hack
            if name[-1] in args.known_hacks:
                continue
            
            n_bad += 1

            in_synch = d_old and d_new and d_old[:-1] == d_new[:-1]
            if not in_synch:
                if not _is_summary():
                    if d_old:
                        print '::sync-old %s' %'.'.join(["%03i"%ientry]+map(str, d_old[2]))
                    else:
                        print '::sync-old ABSENT'
                    if d_new:
                        print '::sync-new %s' %'.'.join(["%03i"%ientry]+map(str, d_new[2]))
                    else:
                        print '::sync-new ABSENT'
                    pass
                # remember for later
                if not d_old:
                    fold.allgood = False
                    summary[d_new[2][0]] += 1
                elif not d_new:
                    fnew.allgood = False
                    summary[d_old[2][0]] += 1
                else:
                    branch_old = '.'.join(["%03i"%ientry, d_old[2][0]])
                    branch_new = '.'.join(["%03i"%ientry, d_new[2][0]])
                    if branch_old < branch_new: 
                        if not _is_summary():
                            print '::sync-old skipping entry'
                        summary[d_old[2][0]] += 1
                        fnew.allgood = False
                        read_new = False
                    elif branch_old > branch_new:
                        if not _is_summary():
                            print '::sync-new skipping entry'
                        summary[d_new[2][0]] += 1
                        fold.allgood = False
                        read_old = False
                    else:
                        # MN: difference in the leaves
                        prev_leaf_old = leafname_fromdump(prev_d_old)
                        prev_leaf_new = leafname_fromdump(prev_d_new)
                        leaf_old = leafname_fromdump(d_old)
                        leaf_new = leafname_fromdump(d_new)
                        if prev_leaf_old == prev_leaf_new:
                            # array size difference?
                            if leaf_old == leaf_new and leaf_old == prev_leaf_old:
                                # could be a size difference in >1 dim arrays
                                # hard to sync, skipping both
                                pass
                            elif leaf_old == prev_leaf_old:
                                # old has bigger array, skip old entry
                                read_new = False
                                if not _is_summary():
                                    print '::sync-old skipping entry'
                                summary[leaf_old] += 1
                            elif leaf_new == prev_leaf_new:
                                # new has bigger array, skip new entry
                                read_old = False
                                if not _is_summary():
                                    print '::sync-new skipping entry'
                                summary[leaf_new] += 1
                                                            
                        if read_old and read_new:
                            summary[d_new[2][0]] += 1
                            if not _is_summary():
                                print '::sync-old+new skipping both entries'
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
            summary[leafname_fromdump(d_old)] += 1

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
