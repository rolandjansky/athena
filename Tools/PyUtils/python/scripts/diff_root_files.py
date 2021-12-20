# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.diff_root_files
# @purpose check that 2 ROOT files have same content (containers and sizes).
# @author Sebastien Binet
# @date February 2010

__doc__ = "diff two ROOT files (containers and sizes)"
__author__ = "Sebastien Binet"

### imports -------------------------------------------------------------------
import PyUtils.acmdlib as acmdlib
import re
from functools import cache, reduce
from math import isnan
from numbers import Real
from os import environ

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
@acmdlib.argument('--branches-of-interest',
                  nargs='*',
                  default='',
                  help='set of regex matching names of branches to compare; assumes all if none specified.')
@acmdlib.argument('--ignore-leaves',
                  nargs='*',
                  default=('Token', 'index_ref', r'(.*)_timings\.(.*)', r'(.*)_mems\.(.*)', r'(.*)TrigCostContainer(.*)'),
                  help='set of leaves names to ignore from comparison; can be a branch name or a partial leaf name (accepts regex)')
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

    # Force load some dictionaries to work around ATLASRECTS-6261/ROOT-10940
    if 'AtlasProject' in environ and environ['AtlasProject'] == 'Athena':
        root.xAOD.Init().ignore()

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
    msg.info('branches of interest: %s', args.branches_of_interest)
    msg.info('ignore  leaves:       %s', args.ignore_leaves)
    msg.info('enforce leaves:       %s', args.enforce_leaves)
    msg.info('leaves prefix:        %s', args.leaves_prefix)
    msg.info('hacks:                %s', args.known_hacks)
    msg.info('entries:              %s', args.entries)
    msg.info('mode:                 %s', args.mode)
    msg.info('error mode:           %s', args.error_mode)
    msg.info('order trees:          %s', args.order_trees)

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
            'entries': nentries,
            'leaves': set(leaves),
            }
    
    def ordered_indices(tree, reverse_order = False):
        from collections import OrderedDict
        import operator

        dict_in = {}
        nevts = tree.GetEntriesFast()

        eiDict = {'eventNumber':['EventInfoAux.',
                                 'Bkg_EventInfoAux.',
                                 'xAOD::EventAuxInfo_v3_EventInfoAux.',
                                 'xAOD::EventAuxInfo_v2_EventInfoAux.',
                                 'xAOD::EventAuxInfo_v1_EventInfoAux.',
                                 'xAOD::EventAuxInfo_v3_Bkg_EventInfoAux.',
                                 'xAOD::EventAuxInfo_v2_Bkg_EventInfoAux.',
                                 'xAOD::EventAuxInfo_v1_Bkg_EventInfoAux.'],
                  'm_event_ID.m_event_number':['McEventInfo',
                                               'ByteStreamEventInfo',
                                               'EventInfo_p4_McEventInfo',
                                               'EventInfo_p4_ByteStreamEventInfo']}

        @cache
        def find_attrs():
            """Find the relevant attributes for reading the event number"""
            for ii, jj in eiDict.items():
                for kk in jj:
                    if hasattr(tree, kk):
                        return kk, ii
            else:
                return None, None

        for idx in range(0, nevts):
            if idx % 100 == 0:
                msg.debug('Read {} events from the input so far'.format(idx))
            tree.GetEntry(idx)
            attr1, attr2 = find_attrs()
            if attr1 is None or attr2 is None:
                msg.error('Cannot read event info, will bail out.')
                msg.error(f"Tried attributes {attr1} and {attr2}")
                break
            event_number = reduce(getattr, [attr1] + attr2.split('.'), tree)
            msg.debug('Idx : EvtNum {:10d} : {}'.format(idx,event_number))
            dict_in[idx] = event_number

        # Sort the dictionary by event numbers
        dict_out = OrderedDict(sorted(dict_in.items(), key=operator.itemgetter(1), reverse = reverse_order))

        # Write out the ordered index and event number pairs
        return [(idx, ival) for idx, ival in dict_out.items()]

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
        
        oldBranches = set(b.GetName().rstrip('\0') for b in fold.tree.GetListOfBranches())
        newBranches = set(b.GetName().rstrip('\0') for b in fnew.tree.GetListOfBranches())
        branches = oldBranches & newBranches

        if args.branches_of_interest:
            BOI_matches = set()
            #branches_of_interest = [ b.rstrip('.') for b in set(args.branches_of_interest) ]
            branches_of_interest = args.branches_of_interest

            for branch_of_interest in branches_of_interest:
                try:
                    r = re.compile(branch_of_interest)
                    BOI_matches.update(filter(r.match, branches))
                     
                except TypeError:
                    continue

            if len(BOI_matches)<1:
              msg.error('No matching branches found in both files for supplied branches of interest, quitting.')
              return 1
            msg.info('only the following branches of interest will be compared: ')
            for l in BOI_matches:
              msg.info(' - [%s]', l)
            branches = BOI_matches

        msg.info('comparing [%s] leaves over entries...', len(infos['old']['leaves'] & infos['new']['leaves']))
        n_good = 0
        n_bad = 0
        import collections
        summary = collections.defaultdict(int)

        def get_event_range(entry):
            smin, smax = 0, None
            # Parse user input
            if isinstance(entry, str):
                # We support three main cases in this format: 5:10 (5th to 10th),
                # 5: (5th to the end), and :5 (from the start to 5th)
                if ':' in entry:
                    vals = entry.split(':')
                    smin = int(vals[0]) if len(vals) > 0 and vals[0].isdigit() else 0
                    smax = int(vals[1]) if len(vals) > 1 and vals[1].isdigit() else None
                # This is the case where the user inputs the total number of events
                elif entry.isdigit():
                    smin = 0
                    smax = int(entry) if int(entry) > 0 else None
            # Handle the case where the input is a number (i.e. default)
            elif isinstance(entry, int):
                smin = 0
                smax = entry if entry > 0 else None
            # If we come across an unhandled case, bail out
            else:
                msg.warning(f"Unknown entries argument {entry}, will compare all events...")
            msg.debug(f"Event slice is parsed as [{smin},{smax}]")
            return smin, smax

        if args.order_trees:
            smin, smax = get_event_range(itr_entries)
            idx_old = ordered_indices(fold.tree)[smin:smax]
            idx_new = ordered_indices(fnew.tree)[smin:smax]
            itr_entries_old, event_numbers_old = list(map(list,zip(*idx_old)))
            itr_entries_new, event_numbers_new = list(map(list,zip(*idx_new)))
            msg.debug(f"List of old indices {itr_entries_old}")
            msg.debug(f"List of new indices {itr_entries_new}")
            msg.debug(f"List of old events {event_numbers_old}")
            msg.debug(f"List of new events {event_numbers_new}")
            if event_numbers_old != event_numbers_new:
                msg.error('Events differ, quitting!')
                msg.error(f"List of old events {event_numbers_old}")
                msg.error(f"List of new events {event_numbers_new}")
                return 1
        else:
            itr_entries_old = itr_entries
            itr_entries_new = itr_entries

        branches = sorted(branches)
        old_dump_iter = fold.dump(args.tree_name, itr_entries_old, branches)
        new_dump_iter = fnew.dump(args.tree_name, itr_entries_new, branches)

        def leafname_fromdump(entry):
            if entry is None:
                return None
            else:
                return '.'.join([s for s in entry[2] if not s.isdigit()])
        
        def elindices_fromdump(entry):
            if entry is None:
                return None
            else:
                return [int(s) for s in entry[2] if s.isdigit()]

        @cache
        def skip_leaf(name_from_dump, skip_leaves):
            """ Here decide if the current leaf should be skipped.
            Previously the matching was done based on the full or partial
            leaf name. E.g. foo.bar.zzz would be skipped if any of the
            following were provided:
                * foo
                * foo.bar
                * foo.bar.zzz
                * Any of the foo, bar, or zzz
            Now, we make a regex matching such that the user doesn't
            need to provide full branch names.
            """
            for pattern in skip_leaves:
                try:
                    m = re.match(pattern, name_from_dump)
                except TypeError:
                    continue
                if m:
                    return True
            else:
                return False

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
                if not skip_leaf(leafname_fromdump(entry), tuple(set(skip_leaves))):
                    return entry
                msg.debug('SKIP: {}'.format(leafname_fromdump(entry)))
            pass

        read_old = True
        read_new = True
        d_old = None
        d_new = None
        
        while True:
            if read_old:
                d_old = reach_next(old_dump_iter, skip_leaves, args.leaves_prefix)
            if read_new:
                d_new = reach_next(new_dump_iter, skip_leaves, args.leaves_prefix)
                
            if not d_new and not d_old:
                break
            
            read_old = True
            read_new = True

            if (args.order_trees and d_old and d_new and d_old[2:] == d_new[2:]) or d_old == d_new:
                n_good += 1
                continue
            
            if d_old:    
                tree_name, ientry, iname, iold = d_old
            if d_new:
                tree_name, jentry, jname, inew = d_new

            # for regression testing we should have NAN == NAN
            if args.nan_equal:
                if all([isinstance(x,Real) and isnan(x) for x in [iold,inew]]):
                    n_good += 1
                    continue

            # FIXME: that's a plain (temporary?) hack
            if iname[-1] in args.known_hacks or jname[-1] in args.known_hacks:
                continue
            
            n_bad += 1

            # Identifiers are event numbers if we're ordering the trees, otherwise tree indices
            if args.order_trees:
                id_old = dict(idx_old)[ientry]
                id_new = dict(idx_new)[jentry]
            else:
                id_old = ientry
                id_new = jentry

            if not args.order_trees:
                in_synch = d_old and d_new and d_old[:-1] == d_new[:-1]
            else:
                in_synch = d_old and d_new and d_old[0] == d_new[0] and d_old[2] == d_new[2] and id_old == id_new
            if not in_synch:
                if _is_detailed():
                    if d_old:
                        msg.info('::sync-old %s','.'.join(["%03i"%ientry]+list(map(str, d_old[2]))))
                    else:
                        msg.info('::sync-old ABSENT')
                    if d_new:
                        msg.info('::sync-new %s','.'.join(["%03i"%jentry]+list(map(str, d_new[2]))))
                    else:
                        msg.info('::sync-new ABSENT')
                    pass
                # remember for later
                if not d_old:
                    fold.allgood = False
                    summary[leafname_fromdump(d_new)] += 1
                elif not d_new:
                    fnew.allgood = False
                    summary[leafname_fromdump(d_old)] += 1
                else:
                    branch_old = f"{id_old}.{d_old[2][0]}"
                    branch_new = f"{id_new}.{d_new[2][0]}"
                    leaf_old = leafname_fromdump(d_old)
                    leaf_new = leafname_fromdump(d_new)
                    indices_old = elindices_fromdump(d_old)
                    indices_new = elindices_fromdump(d_new)
                    # Branches/Leaves are alphabetically ordered
                    # If we're out-of-sync, we try to figure out
                    # if we should advance the old or the new branch
                    # For same branches, we look at the full leaf name
                    # If that fails we look at the indices
                    if branch_old > branch_new:
                        read_old = False
                    elif branch_old < branch_new:
                        read_new = False
                    else:
                        if leaf_old > leaf_new:
                            read_old = False
                        elif leaf_old < leaf_new:
                            read_new = False
                        elif indices_old and indices_new and len(indices_old) == len(indices_new):
                            if indices_old > indices_new:
                                read_old = False
                            elif indices_old < indices_new:
                                read_new = False
                    # Let's see if we can reconcile
                    # If not, just bail out to avoid false positivies
                    if read_old and not read_new:
                        if _is_detailed():
                            msg.info('::sync-old skipping entry')
                        fold.allgood = False
                        summary[leaf_old] += 1
                    elif read_new and not read_old:
                        if _is_detailed():
                            msg.info('::sync-new skipping entry')
                        fnew.allgood = False
                        summary[leaf_new] += 1
                    else:
                        msg.error('::sync attempt failed, bailing out...')
                        msg.error(f"::sync-old Leaf vs Index : {leaf_old} vs {indices_old}")
                        msg.error(f"::sync-new Leaf vs Index : {leaf_new} vs {indices_new}")
                        fold.allgood = False
                        fnew.allgood = False
                        summary[leaf_old] += 1
                        summary[leaf_new] += 1
                        break
 
                if _is_exit_early():
                    msg.info('*** exit on first error ***')
                    break
                continue
            
            if not args.order_trees:
                n = '.'.join(list(map(str, ["%03i"%ientry]+iname)))
            else:
                n = '.'.join(list(map(str, ["%03i"%ientry]+iname+["%03i"%jentry]+jname)))
            diff_value = 'N/A'
            try:
                diff_value = 50.*(iold-inew)/(iold+inew)
                diff_value = '%.8f%%' % (diff_value,)
            except Exception:
                pass
            if _is_detailed():
                msg.info('%s %r -> %r => diff= [%s]', n, iold, inew, diff_value)
                pass
            summary[leafname_fromdump(d_old)] += 1

            if iname[0] in args.enforce_leaves or jname[0] in args.enforce_leaves:
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
            msg.error('NOTE: there were errors during the dump')
            msg.info('fold.allgood: %s' , fold.allgood)
            msg.info('fnew.allgood: %s' , fnew.allgood)
            n_bad += 0.5
        return n_bad
    
    ndiff = diff_tree(fold, fnew, args)
    if ndiff != 0:
        msg.error('files differ!')
        return 2
    msg.info('all good.')
    return 0
