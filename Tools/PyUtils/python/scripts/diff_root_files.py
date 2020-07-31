# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.diff_root_files
# @purpose check that 2 ROOT files have same content (containers and sizes).
# @author Sebastien Binet
# @date February 2010

__doc__ = "diff two ROOT files (containers and sizes)"
__author__ = "Sebastien Binet"

### imports -------------------------------------------------------------------
import PyUtils.acmdlib as acmdlib
from math import isnan
from numbers import Real

### globals -------------------------------------------------------------------
g_ALLOWED_MODES = ('summary', 'semi-detailed', 'detailed')
g_ALLOWED_ERROR_MODES = ('bailout', 'resilient')
g_args = None

### classes -------------------------------------------------------------------

### functions -----------------------------------------------------------------
def _is_detailed():
    global g_args
    return g_args.mode == 'detailed'

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
                  default=('Token', 'index_ref',),
                  help='set of leaves names to ignore from comparison; can be a branch name or a partial leaf name (without any trailing dots)')
@acmdlib.argument('--enforce-leaves',
                  nargs='*',
                  default=('BCID',),
                  help='set of leaves names we make sure to compare')
@acmdlib.argument('--leaves-prefix',
                  default='',
                  help='Remove prefix value from all leaves')
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
@acmdlib.argument('--order-trees',
                  action='store_true',
                  default=False,
                  help="""To order trees according to event numbers""")
@acmdlib.argument('--mode',
                  choices=g_ALLOWED_MODES,
                  default='detailed',
                  help="""\
Enable a particular mode.
  'summary': only report the number of differences.
  'semi-detailed': report the number of differences and the leaves that differ.
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
@acmdlib.argument('--nan-equal',
                  action='store_true',
                  default=False,
                  help="""Compare nan as equal to nan""")

def main(args):
    """diff two ROOT files (containers and sizes)"""

    global g_args
    g_args = args
    
    import PyUtils.RootUtils as ru
    root = ru.import_root()  # noqa: F841

    import PyUtils.Logging as L
    msg = L.logging.getLogger('diff-root')
    if args.verbose:
        msg.setLevel(L.logging.VERBOSE)
    else:
        msg.setLevel(L.logging.INFO)

    from PyUtils.Helpers import ShutUp  # noqa: F401

    if args.entries == '':
        args.entries = -1
        
    msg.info('comparing tree [%s] in files:', args.tree_name)
    msg.info(' old: [%s]', args.old)
    msg.info(' new: [%s]', args.new)
    msg.info('ignore  leaves: %s', args.ignore_leaves)
    msg.info('enforce leaves: %s', args.enforce_leaves)
    msg.info('leaves prefix:  %s', args.leaves_prefix)
    msg.info('hacks:          %s', args.known_hacks)
    msg.info('entries:        %s', args.entries)
    msg.info('mode:           %s', args.mode)
    msg.info('error mode:     %s', args.error_mode)
    msg.info('order trees:    %s', args.order_trees)

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
        if args.leaves_prefix:
            leaves = [l.replace(args.leaves_prefix, '') for l in leaves]
        return {
            'entries' : nentries,
            'leaves': set(leaves),
            }
    
    def ordered_indices(tree, reverse_order = False):
        from collections import OrderedDict
        import operator

        dict_in = {}
        nevts = tree.GetEntriesFast()

        for idx in range(0, nevts):
            if idx % 100 == 0:
                msg.debug('Read {} events from the input so far'.format(idx))
            tree.GetEntry(idx)
            if hasattr(tree,'xAOD::EventAuxInfo_v2_EventInfoAux.'):
                event_info = getattr(tree,'xAOD::EventAuxInfo_v2_EventInfoAux.')
                event_number = event_info.eventNumber
            elif hasattr(tree,'xAOD::EventAuxInfo_v1_EventInfoAux.'):
                event_info = getattr(tree,'xAOD::EventAuxInfo_v1_EventInfoAux.')
                event_number = event_info.eventNumber
            elif hasattr(tree,'EventInfoAux.'):
                event_info = getattr(tree,'EventInfoAux.')
                event_number = event_info.eventNumber
            elif hasattr(tree,'EventInfo_p4_McEventInfo'):
                event_info = getattr(tree,'EventInfo_p4_McEventInfo')
                event_number = event_info.m_event_ID.m_event_number
            elif hasattr(tree,'EventInfo_p4_ByteStreamEventInfo'):
                event_info = getattr(tree,'EventInfo_p4_ByteStreamEventInfo')
                event_number = event_info.m_event_ID.m_event_number
            elif hasattr(tree,'ByteStreamEventInfo'):
                event_info = getattr(tree,'ByteStreamEventInfo')
                event_number = event_info.m_event_ID.m_event_number
            else:
                msg.error('Cannot read event info, will bail out.')
                break
            msg.debug('Idx : EvtNum {:10d} : {}'.format(idx,event_number))
            dict_in[idx] = event_number

        # Sort the dictionary by event numbers
        dict_out = OrderedDict(sorted(dict_in.items(), key=operator.itemgetter(1), reverse = reverse_order))

        # Write out the ordered index list
        return [idx for idx in dict_out]

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
        n_good = 0
        n_bad = 0
        import collections
        summary = collections.defaultdict(int)

        if args.order_trees:
            slice_max = int(itr_entries) if int(itr_entries) > 0 else None
            itr_entries_old = ordered_indices(fold.tree)[0:slice_max]
            itr_entries_new = ordered_indices(fnew.tree)[0:slice_max]
            msg.debug('List of old indices {}'.format(itr_entries_old))
            msg.debug('List of new indices {}'.format(itr_entries_new))
        else:
            itr_entries_old = itr_entries
            itr_entries_new = itr_entries

        old_dump_iter = fold.dump(args.tree_name, itr_entries_old)
        new_dump_iter = fnew.dump(args.tree_name, itr_entries_new)

        def leafname_fromdump(entry):
            return '.'.join([s for s in entry[2] if not s.isdigit()])
        
        def reach_next(dump_iter, skip_leaves, leaves_prefix=None):
            keep_reading = True
            while keep_reading:
                try:
                    entry = next(dump_iter)
                except StopIteration:
                    return None
                entry[2][0] = entry[2][0].rstrip('.\0')  # clean branch name
                if leaves_prefix:
                    entry[2][0] = entry[2][0].replace(leaves_prefix, '')
                name = []
                skip = False
                for n in leafname_fromdump(entry).split('.'):
                    name.append(n)
                    if '.'.join(name) in skip_leaves or n in skip_leaves:
                        skip = True
                        break
                if not skip:
                    return entry
                # print('SKIP:', leafname_fromdump(entry))
            pass

        read_old = True
        read_new = True
        d_old = None
        d_new = None
        
        while True:
            if read_old:
                prev_d_old = d_old
                d_old = reach_next(old_dump_iter, skip_leaves, args.leaves_prefix)
            if read_new:
                prev_d_new = d_new
                d_new = reach_next(new_dump_iter, skip_leaves, args.leaves_prefix)
                
            if not d_new and not d_old:
                break
            
            read_old = True
            read_new = True

            if (args.order_trees and d_old and d_new and d_old[-1] == d_new[-1]) or d_old == d_new:
                n_good += 1
                continue
            
            if d_old:    
                tree_name, ientry, name, iold = d_old
            if d_new:
                tree_name, jentry, name, inew = d_new

            # for regression testing we should have NAN == NAN
            if args.nan_equal:
                if all([isinstance(x,Real) and isnan(x) for x in [iold,inew]]):
                    n_good += 1
                    continue

            # FIXME: that's a plain (temporary?) hack
            if name[-1] in args.known_hacks:
                continue
            
            n_bad += 1

            if not args.order_trees:
                in_synch = d_old and d_new and d_old[:-1] == d_new[:-1]
            else:
                in_synch = d_old and d_new and d_old[0] == d_new[0] and d_old[2] == d_new[2]
            if not in_synch:
                if _is_detailed():
                    if d_old:
                        print('::sync-old %s' %'.'.join(["%03i"%ientry]+list(map(str,
                                                                             d_old[2]))))
                    else:
                        print('::sync-old ABSENT')
                    if d_new:
                        print('::sync-new %s' %'.'.join(["%03i"%jentry]+list(map(str,
                                                                             d_new[2]))))
                    else:
                        print('::sync-new ABSENT')
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
                    branch_new = '.'.join(["%03i"%jentry, d_new[2][0]])
                    if branch_old < branch_new: 
                        if _is_detailed():
                            print('::sync-old skipping entry')
                        summary[d_old[2][0]] += 1
                        fnew.allgood = False
                        read_new = False
                    elif branch_old > branch_new:
                        if _is_detailed():
                            print('::sync-new skipping entry')
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
                                if _is_detailed():
                                    print('::sync-old skipping entry')
                                summary[leaf_old] += 1
                            elif leaf_new == prev_leaf_new:
                                # new has bigger array, skip new entry
                                read_old = False
                                if _is_detailed():
                                    print('::sync-new skipping entry')
                                summary[leaf_new] += 1
                                                            
                        if read_old and read_new:
                            summary[d_new[2][0]] += 1
                            if _is_detailed():
                                print('::sync-old+new skipping both entries')
                        fold.allgood = False
                        fnew.allgood = False
 
                if _is_exit_early():
                    print('*** exit on first error ***')
                    break
                continue
            
            if not args.order_trees:
                n = '.'.join(list(map(str, ["%03i"%ientry]+name)))
            else:
                n = '.'.join(list(map(str, ["%03i.%03i"%(ientry,jentry)]+name)))
            diff_value = 'N/A'
            try:
                diff_value = 50.*(iold-inew)/(iold+inew)
                diff_value = '%.8f%%' % (diff_value,)
            except Exception:
                pass
            if _is_detailed():
                print('%s %r -> %r => diff= [%s]' %(n, iold, inew, diff_value))
                pass
            summary[leafname_fromdump(d_old)] += 1

            if name[0] in args.enforce_leaves:
                msg.info("don't compare further")
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
            msg.info('fold.allgood: %s' , fold.allgood)
            msg.info('fnew.allgood: %s' , fnew.allgood)
            n_bad += 0.5
        return n_bad
    
    ndiff = diff_tree(fold, fnew, args)
    if ndiff != 0:
        msg.info('files differ!')
        return 2
    msg.info('all good.')
    return 0
