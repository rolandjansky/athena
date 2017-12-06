#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# bwd compat
from __future__ import with_statement

# stdlib imports
import os
import sys
import getopt
import atexit

# 3rd party imports
import ROOT

# root globals to prevent ROOT garbage collector to sweep the rug....
_root_files = []
_root_trees = []

# Root has a global dtor ordering problem: the cintex trampolines
# may be deleted before open files are closed.  Workaround is to explicitly
# close open files before terminating.
#
def _close_root_files():
    for f in _root_files:
        if hasattr (f, 'Close'): f.Close()
    del _root_files[0:-1]
    return
atexit.register(_close_root_files)

def _fnmatch(fname, patterns):
    """helper function wrapping the original `fnmatch:fnmatch` function but providing
    support for a list of patterns to match against
    """
    from fnmatch import fnmatch
    if isinstance(patterns, basestring):
        patterns = [patterns]
    for pattern in patterns:
        if fnmatch(fname, pattern):
            return True
    return False

def _make_fake_output(fname, tree_name, tree_title=None):
    f = ROOT.TFile.Open(fname, "recreate")
    if tree_title is None:
        tree_title = tree_name
    t = ROOT.TTree(tree_name, tree_title)
    f.Write()
    f.Close()
    del t, f
    return
    
class LBRange(object):
    def __init__(self, run, lbmin, lbmax):
        self.run = run
        self.lbmin = lbmin
        self.lbmax = lbmax

def _interpret_grl(fname):
    if not os.path.exists(fname):
        raise OSError

    lbs = []
    if fname.endswith('.dat'):
        for l in open(fname):
            l = l.strip()
            run, lbmin, lbmax = map(int, l.split())
            lbs.append(LBRange(run, lbmin, lbmax))
    elif fname.endswith('.xml'):
        data = extract_data_from_xml(fname)
        for i in data:
            run, lbmin, lbmax = map(int, i)
            lbs.append(LBRange(run, lbmin, lbmax))
    else:
        raise RuntimeError("unknown file extension (%s)" % (fname,))
    return lbs

def interpret_grl(fname="GRL.dat"):
    fnames = []
    if isinstance(fname, basestring):
        fnames = [fname]
    elif isinstance(fname, (list,tuple)):
        fnames = fname[:]
    else:
        raise TypeError('fname must be a string or a sequence (got: %s)' %
                        type(fname))
    lbs = []
    for fname in fnames:
        lbs.extend(_interpret_grl(fname))
    return lbs

def pass_grl(run, lb, good_lbs):

    for ilb in good_lbs:
        if run != ilb.run:
            continue

        if ilb.lbmin <= lb <= ilb.lbmax:
            return True

    return False

def warm_up(fname):
    assert os.path.exists(fname)
    import commands
    rc,_ = commands.getstatusoutput("/bin/dd if=%s of=/dev/null" % (fname,))
    return rc

def apply_filters(branches, patterns):
    """extract the branches which match the patterns.
    a pattern can add or remove a branch.
    if a branch matches no pattern, it is discarded.
    if a branch matches several patterns, the last pattern wins.
    """
    from fnmatch import fnmatch
    from collections import defaultdict
    filtered = defaultdict(list)
    matched_patterns = []
    for br in branches:
        for p in patterns:
            if p == '':
                continue
            op = '-'
            if p.startswith('+') or not p.startswith('-'):
                if p[0] == '+':
                    p = p[1:]
                op = '+'
            if p.startswith('-'):
                op = '-'
                p = p[1:]
            if fnmatch(br, p):
                filtered[br].append(op)
                matched_patterns.append(p)
    for p in patterns:
        if not (p in matched_patterns):
            print '::: warning: pattern [%s] could not be matched against any branch' % p
            pass
        pass
    filtered = dict(filtered)
    return sorted([k for k,v in filtered.iteritems() if v[-1] == '+'])

def merge_all_trees(fnames, tree_name, memory, sfo,
                    vars_fname=None, grl_fname=None,
                    filter_fct=None,
                    keep_all_trees=False,
                    apply_recursive_opt=True):
    
    oname = sfo[:]
    if not oname.endswith(".root"):
        oname = oname + ".root"
        pass

    root_open = ROOT.TFile.Open
    fout = root_open(oname, "RECREATE", "", 1)
    fout.ResetBit(ROOT.kCanDelete)
    
    memory *= 1024 # change to bytes

    tree_maxsz = ROOT.TTree.GetMaxTreeSize()

    ## summing up branch sizes over all the files
    orig_file = root_open(fnames[0], "read")
    orig_tree = getattr(orig_file, tree_name)
    br_names = []
    all_br_names = set(br.GetName() for br in orig_tree.GetListOfBranches())
    
    if not (vars_fname is None):
        # open the file containing the list of branches to keep or discard
        patterns = []
        with open(vars_fname, 'r') as br_file:
            for p in br_file:
                patterns.append(p.strip())
        orig_tree.SetBranchStatus("*", 0)
        # apply_filters returns the list of branches to keep
        br_names = apply_filters(all_br_names, patterns)
        print "::: keeping only the following branches: (from file-list %s)" %\
              vars_fname
        for b in br_names:
            print ":::   [%s]" % (b,)
            orig_tree.SetBranchStatus(b,1)                            
    else:
        br_names = [br.GetName() for br in orig_tree.GetListOfBranches()]

    nleaves = len(br_names)
    print "::: nleaves=[%04i] tree=[%s]" % (nleaves, orig_tree.GetName())

    tot_sz = [0]*nleaves    # zipped sizes collected from all files
    basket_sz = [0]*nleaves # size to be optimized (starts with `tot_sz`)
    baskets = [1]*nleaves   # cache

    for idx,fname in enumerate(fnames):
        f = root_open(fname, "read")
        tree = getattr(f, tree_name)
        for ibr,br_name in enumerate(br_names):
            branch = tree.GetBranch(br_name)
            if not branch:
                print "***warning*** - tree [%s] has no branch [%s]" % (tree.GetName(),
                                                                        br_name)
                continue
            branch.SetAddress(0)

            tot_sz[ibr] += branch.GetTotBytes()
            basket_sz[ibr] = tot_sz[ibr]
            #baskets[ibr] = 1

            pass # loop over branches
        del tree
        f.Close()
        del f
        pass # loop over trees

    if apply_recursive_opt:
        while 1: # recursive optimization
            tot_mem = sum(basket_sz)
            if tot_mem < memory:
                break

            max_spare = -1
            max_spare_idx = None
            for i in xrange(nleaves):
                spare = tot_sz[i]/baskets[i] - tot_sz[i]/(baskets[i]+1)
                if max_spare < spare:
                    max_spare = spare
                    max_spare_idx = i
            if max_spare_idx is not None:
                idx = max_spare_idx
                baskets[idx] += 1
                basket_sz[idx] = tot_sz[idx]/baskets[idx]
            pass # end-while
        pass # apply_recursive_opt
    
    # create the new (optimized) tree
    new_tree = orig_tree.CloneTree(0) # no copy of events
    new_tree.ResetBit(ROOT.kCanDelete)
    new_tree.SetDirectory(fout)
    # once cloning is done, separate the trees to avoid as many side-effects
    # as possible
    #orig_tree.GetListOfClones().Remove(new_tree)
    orig_tree.ResetBranchAddresses()
    new_tree.ResetBranchAddresses()

    if vars_fname is not None:
        orig_tree.SetBranchStatus("*", 0)
        new_tree.SetBranchStatus("*", 0)
        for br_name in br_names:
            orig_tree.SetBranchStatus(br_name, 1)
            new_tree.SetBranchStatus(br_name, 1)

    # a list of other tree names to filter-and-merge
    other_trees = []
    if keep_all_trees:
        print "::: capturing other trees to filter and merge..."
        # also handle all other payload-trees
        # to decide if a tree is a payload-tree (and not a metadata tree
        # which we don't know -by default- what is the correct way to merge)
        # we just compare the number of events...
        # FIXME: handle deeply-nested structures (/dir1/dir2/tree0,...)
        _all_tree_names = list(n.GetName()
                               for n in orig_file.GetListOfKeys()
                               if (isinstance(getattr(orig_file, n.GetName()),
                                              ROOT.TTree)
                                   and n.GetName() != tree_name))
        for n in _all_tree_names:
            _old_tree = orig_file.Get(n)
            print ":::  ->",n,
            if _old_tree.GetEntries() != orig_tree.GetEntries():
                # probably not a payload-tree but a metadata one...
                del _old_tree
                print "[reject]"
                continue
            print "[keep]"
            _new_tree = _old_tree.CloneTree(0) # no copy of events
            _new_tree.ResetBit(ROOT.kCanDelete)
            _new_tree.SetDirectory(fout)
            _old_tree.ResetBranchAddresses()
            _new_tree.ResetBranchAddresses()
            other_trees.append(_new_tree)
            del _old_tree
        print "::: capturing other trees to filter and merge... [done]"

    if apply_recursive_opt:
        # setting optimized basket sizes
        tot_mem = 0.
        tot_bkt = 0
        max_bkt = 0
        min_bkt = 1024**3

        for ibr in xrange(nleaves):
            br = new_tree.GetBranch(br_names[ibr])
            if basket_sz[ibr] == 0:
                basket_sz[ibr] = 16

            basket_sz[ibr] = basket_sz[ibr] - (basket_sz[ibr] % 8)
            br.SetBasketSize(basket_sz[ibr])

            tot_mem += basket_sz[ibr]
            tot_bkt += baskets[ibr]

            if basket_sz[ibr] < min_bkt:
                min_bkt = basket_sz[ibr]
            if basket_sz[ibr] > max_bkt:
                max_bkt = basket_sz[ibr]

            pass # loop over leaves

        print "::: optimize baskets: "
        print ":::   total memory buffer: %8.3f kb" % (tot_mem/1024,)
        print ":::   total baskets:       %8.3f (min= %8.3f) (max= %8.3f) kb" % (
            tot_bkt, min_bkt, max_bkt)

        del tot_sz, basket_sz, baskets
        pass # apply_recursive_opt

    # copying data
    n_pass = 0
    n_tot = 0
    do_grl_selection = not (grl_fname is None)
    
    if do_grl_selection:
        good_lbs = interpret_grl(fname=grl_fname)

    print "::: processing [%i] trees..." % (len(fnames,))
    for idx, fname in enumerate(fnames):
        f = root_open(fname, "READ")

        for other_tree in other_trees:
            tree = getattr(f, other_tree.GetName())
            other_tree.CopyAddresses(tree)
                    
        tree = getattr(f, tree_name)
        new_tree.CopyAddresses(tree)
        nentries = tree.GetEntries()
        print ":::   entries:", nentries
        for i in xrange(nentries):

            nb = tree.GetEntry(i)
            if nb <= 0:
                print "*** error loading entry [%i]. got (%i) bytes" % (i,nb)
                raise RuntimeError
            n_tot += 1

            accept_entry = True
            if do_grl_selection:
                if not pass_grl(tree.RunNumber, tree.lbn, good_lbs):
                    accept_entry = False
                pass
            
            if filter_fct and accept_entry:
                try:
                    if not filter_fct(tree):
                        accept_entry = False
                except Exception, err:
                    print "*** problem running user filter fct:"
                    print err
                    print "*** (filter fct is now disabled)"
                    filter_fct = None

            if accept_entry:
                n_pass += 1
                if n_pass > 10:
                    _nentries_cur = new_tree.GetEntries()
                    fout = new_tree.GetCurrentFile()
                    fout.Flush()
                    out_fsize = fout.GetSize()
                    avg_entry_sz = out_fsize / float(_nentries_cur or 1.)
                    do_change_file = out_fsize + avg_entry_sz > 0.9 * tree_maxsz
                    if do_change_file:
                        #print "--- manually triggering TTree::ChangeFile..."
                        # manually trigger the file split...
                        # this is to ensure the split doesn't happen in between
                        # the new_tree.Fill() and the other_tree.Fill() which
                        # would de-synchronize the entries between the trees...
                        fout = new_tree.ChangeFile(fout)
                new_tree.Fill()
                for other_tree in other_trees:
                    _tree = f.Get(other_tree.GetName())
                    nb = _tree.GetEntry(i)
                    if nb <= 0:
                        print "*** error loading entry [%i] for tree [%s]. got (%i) bytes" % (
                            i, other_tree.GetName(), nb)
                        continue
                    other_tree.Fill()
                    del _tree
                    pass # loop over other trees
                pass # entry accepted
            pass # loop over entries
        del tree
        f.Close()
        del f
        pass # loop over input trees
    print "::: processing [%i] trees... [done]" % (len(fnames,))

    eff = 0.
    if n_tot != 0:
        eff = float(n_pass)/float(n_tot)
    print "::: filter efficiency: %d/%d -> %s" % (n_pass, n_tot, eff)

    fout = new_tree.GetCurrentFile()
    fout.Write()
    fout.Close()
    del fout

    return

def order(m, chain_name, fnames, workdir):

    # disabling the file-split as it may interfere badly with the re-ordering...
    # set it to 2Tb
    ROOT.TTree.SetMaxTreeSize(2 * 1024 * 1024 * 1024 * 1024)

    print "::: nbr of files:", len(fnames)
    for i,fn in enumerate(fnames):

        timer = ROOT.TStopwatch()
        timer.Start()
        print "::: optimizing   [%s]..." % (fn,)
        #warm_up(fn)

        timer.Start()
        fin = ROOT.TFile.Open(fn, "read")
        tmp_fname = "%s_temporary_%03i.root" % (
            chain_name.replace("/","_").replace(" ","_"),
            i)
        fout = ROOT.TFile.Open(tmp_fname, "recreate", "", 6)

        # perform the (re)ordering for all trees
        _all_tree_names = list(
            n.GetName()
            for n in fin.GetListOfKeys()
            if isinstance(getattr(fin, n.GetName()),
                          ROOT.TTree))
        for chain_name in _all_tree_names:
            tc2 = fin.Get(chain_name)
            opt = {
                0: "SortBasketsByOffset",
                1: "SortBasketsByBranch",
                2: "SortBasketsByEntry",
                }.get(m, "SortBasketsByBranch")
            opt_tree = tc2.CloneTree(-1, opt + " fast")
            opt_tree.Write("", ROOT.TObject.kOverwrite)
        # -

        timer.Stop()

        print ":::   wallclock time:", timer.RealTime()
        print ":::   CPU time:      ", timer.CpuTime()

        try:
            # fout may have been invalidated if the file-size limit was hit
            # and _1.root, _2.root,... files were created...
            if fout:
                fout.Close()
        except Exception,err:
            print "**error**:",err
        fin.Close()

        dst = os.path.join(workdir, os.path.basename(fn))
        print "::: optimized as [%s]... [done]" % (dst,)
        
        # rename the temporary into the original
        import shutil
        shutil.move(src=tmp_fname,
                    dst=dst)
                                                    
        #os.rename(tmp_fname, fn)
    return

def _load_filter_fct(selection):
    """
    helper function to locate a filter function or compile one from the
    source code snippet
    if `selection` begins with 'file:' selection is then interpreted as a
    string holding the location to a file where a 'filter_fct' fonction is
    defined and importable.
    otherwise, `selection` is compiled into a lambda function
    """
    import imp
    import inspect
    import os.path as osp

    filter_fct = None

    if selection is None:
        return filter_fct
    
    if not isinstance(selection, basestring):
        print "** invalid filter-fct type (%r)" % (type(selection),)
        return filter_fct
    
    if selection == "":
        return filter_fct

    def plugin_filter(obj):
        if inspect.isfunction(obj):
            return obj.__name__ == 'filter_fct'
        
    if selection.startswith('file:'):
        fname = selection[len('file:'):]
        fname = osp.expanduser(osp.expandvars(fname))
        plugin = open(fname, 'r')
        mod = imp.load_source(plugin.name[:-3], plugin.name, plugin)
        plugin.close()
        filter_fct = inspect.getmembers(mod, plugin_filter)[0][1]
    else:
        fct_code = "filter_fct = lambda t: %s" % selection
        my_locals = dict(locals())
        exec fct_code in {}, my_locals
        filter_fct = my_locals['filter_fct']
    return filter_fct

class Options(object):
    """place holder for command line options values"""
    pass

def main():

    global _root_files, _root_trees
    
    _opts = []
    _useropts = "i:o:t:m:s:h"
    _userlongopts = [
        "in=", "out=", "tree=", "var=", "maxsize=", "grl=", "fakeout",
        "selection=",
        "keep-all-trees",
        "disable-recursive-opt",
        "help"
        ]
    _error_msg = """\
Accepted command line options:
 -i, --in=<INFNAME>                   ...  file containing the list of input files
 -o, --out=<OUTFNAME>                 ...  output file name
 -t, --tree=<TREENAME>                ...  name of the tree to be filtered.
                                           other trees won't be copied by default
                                           (except if you pass --keep-all-trees)
     --var=<VARSFNAME>                ...  path to file listing the branch names
                                           to be kept in the output file.
     --grl=<GRLFNAME>                 ...  path to a GRL XML file or a list of
                                           comma-separated GRL XML files
 -m, --maxsize=<sz>                   ...  maximum zip size of the main tree (in Mb.)
     --fakeout                        ...  create fake output file if empty or
                                           non valid input tree is found (ease
                                           the pain on the GRID)
 -s, --selection=<PYTHON_CODE>        ...  a python snippet to select events
                                           or the path to python file holding
                                           the definition of a 'filter_fct'.
                                           ex:
                                             t.eg_px[0] > 10000 and t.eg_py[0] > 10000
                                           NOTE: the tree must be named 't' in your code.
                                           or:
                                            file:foo.py
                                            where foo.py contains:
                                            def filter_fct(t):
                                                return t.eg_px[0] > 10000
                                           NOTE: the function must be named 'filter_fct' and take the tree as a parameter
     --keep-all-trees                 ...  keep, filter and merge all other trees.
     --disable-recursive-opt          ...  switch to disable a recursive (size)
                                           optimization. (The recursive optimization
                                           might be excessively SLOW on large n-tuples.)
 """

    for arg in sys.argv[1:]:
        _opts.append(arg)
    
    opts = Options()
    opts.maxsize = 1800
    opts.output_file = None
    opts.vars_fname = None
    opts.grl_fname = None
    opts.fake_output = False
    opts.selection = None
    opts.keep_all_trees = False
    opts.apply_recursive_opt = True
    
    try:
        optlist, args = getopt.getopt(_opts, _useropts, _userlongopts)
    except getopt.error:
        print sys.exc_value
        print _error_msg
        sys.exit(1)

    for opt,arg in optlist:
        if opt in ("-i", "--in"):
            opts.input_files = arg

        elif opt in ("-o", "--out"):
            opts.output_file = arg

        elif opt in ("-t", "--tree"):
            opts.tree_name = str(arg).strip()

        elif opt in ("--var",):
            opts.vars_fname = arg

        elif opt in ("-m", "--maxsize"):
            opts.maxsize = int(arg)

        elif opt in ('--grl',):
            opts.grl_fname = arg

        elif opt in ('--fakeout',):
            opts.fake_output = True

        elif opt in ('-s', '--selection',):
            opts.selection = str(arg).strip()

        elif opt in ('--keep-all-trees',):
            opts.keep_all_trees = True

        elif opt in ('--disable-recursive-opt',):
            opts.apply_recursive_opt = False
            
        elif opt in ("-h", "--help"):
            print _error_msg
            sys.exit(0)

    print ":"*80
    print "::: filter'n'merge d3pds"
    print ":::"
    # for AttributeListLayout which uses CINT for its dict...
    #ROOT.gSystem.Load('liblcg_RootCollection')
    
    workdir = os.path.dirname(opts.output_file)
    if workdir == '':
        workdir = '.'
    if not os.path.exists(workdir):
        os.makedirs(workdir)

    if isinstance(opts.grl_fname, basestring):
        opts.grl_fname = opts.grl_fname.split(',')
        from glob import glob
        grl_fnames = []
        for grl_fname in opts.grl_fname:
            grl_fnames.extend(glob(grl_fname))
        opts.grl_fname = grl_fnames
        
    print "::: input files:   ",opts.input_files
    print "::: output file:   ",opts.output_file
    print "::: vars fname:    ",opts.vars_fname
    print "::: tree name:     ",opts.tree_name
    print "::: GRL file:      ",opts.grl_fname
    print "::: max tree sz:   ",opts.maxsize, "Mb"
    if opts.fake_output:
        print "::: creation of fake-output (if needed) [ON]"
    print "::: user filter:   ",opts.selection
    print "::: keep all trees:", opts.keep_all_trees
    print "::: recursive opt: ", opts.apply_recursive_opt
    
    # slightly increase the max size (so that the manual ChangeFile at 0.9 of
    # the current MaxTreeSize will fall within the user-provided one...)
    ROOT.TTree.SetMaxTreeSize(long(opts.maxsize * 1024 * 1024 / 0.9))
    
    ## try to compile the user filtering function
    filter_fct = None
    try:
        filter_fct = _load_filter_fct(opts.selection)
    except Exception,err:
        print "*** problem loading filter-fct:"
        print err
        print "*** filter-fct is now disabled"
        filter_fct = None
        
    iflist = [l.strip() for l in open(opts.input_files, "r") if l.strip()]
    for l in iflist:
        fname = l.strip()
        if not fname:
            continue
        f = ROOT.TFile.Open(fname,"read")
        if not f:
            raise RuntimeError("no such file [%s]" % fname)

        tree = f.Get(opts.tree_name)
        if not tree:
            print "***warning*** no such tree [%s] in file [%s] (IGNORING!)" % (
                opts.tree_name, fname,
                )
            continue
        if tree.GetEntries()==0:
            print "**warning** no entries in tree [%s] in file [%s] (IGNORING!)" % (
                opts.tree_name, fname,
                )
            continue
        if tree.GetListOfBranches().GetEntriesFast() == 0:
            print "**warning** tree [%s] in file [%s] has no branches (IGNORING!)" % (
                opts.tree_name, fname,
                )
            continue
                
        #f.ResetBit(ROOT.kCanDelete)
        _root_files.append(fname)
        print " - loaded [%s]" % (fname,)

        #tree.ResetBit(ROOT.kCanDelete)
        _root_trees.append(opts.tree_name) # whatever...
        del tree
        f.Close()
        del f

    if len(_root_trees) == 0:
        print "::: no valid tree left"
        if opts.fake_output:
            print "::: crafting an empty output file"
            _make_fake_output(opts.output_file, opts.tree_name)
            return 0
        return 0 # FIXME: should this become an error of some sort ?
    
    ## chain = ROOT.TChain(opts.tree_name)
    ## _root_chains.append(chain)
    
    nfiles = len(_root_files)
    if nfiles <= 0:
        print "::: no input files found"
        return 2

    timer = ROOT.TStopwatch()
    timer.Start()
    merge_all_trees(fnames=_root_files,
                    tree_name =opts.tree_name,
                    memory=1024*30,
                    sfo=opts.output_file,
                    vars_fname=opts.vars_fname,
                    grl_fname=opts.grl_fname,
                    filter_fct=filter_fct,
                    keep_all_trees=opts.keep_all_trees,
                    apply_recursive_opt=opts.apply_recursive_opt)

    timer.Stop()

    print "::: merging done in:"
    print ":::   wallclock:",timer.RealTime()
    print ":::   CPU time: ",timer.CpuTime()

    # del _root_chains[:]
    
    print "::: performing re-ordering..."
    import glob
    import os.path as osp
    fname_pattern = osp.splitext(opts.output_file)[0]
    # re-order all output files (in case they were split off)
    fnames= sorted(glob.glob(fname_pattern + "*.root"))
    order(m=2,
          chain_name=opts.tree_name,
          fnames=fnames,
          workdir=workdir)
    print "::: performing re-ordering... [done]"

    print "::: bye."
    print ":"*80
    return 0

###################### xmldict #########################
# @file PyUtils/python/xmldict.py
# @brief converts an XML file into a python dict, back and forth
# @author http://code.activestate.com/recipes/573463
#         slightly adapted to follow PEP8 conventions

__version__ = "$Revision: 547442 $"
__doc__ = """\
functions to convert an XML file into a python dict, back and forth
"""
__author__ = "Sebastien Binet <binet@cern.ch>"


# hack: LCGCMT had the py-2.5 xml.etree module hidden by mistake.
#       this is to import it, by hook or by crook
def import_etree():
    import xml
    # first try the usual way
    try:
        import xml.etree
        return xml.etree
    except ImportError:
        pass
    # do it by hook or by crook...
    import sys, os, imp
    xml_site_package = os.path.join(os.path.dirname(os.__file__), 'xml')
    m = imp.find_module('etree', [xml_site_package])

    etree = imp.load_module('xml.etree', *m)
    setattr(xml, 'etree', etree)
    return etree
try:
    etree = import_etree()
    from xml.etree import ElementTree

    ## module implementation ---------------------------------------------------
    class XmlDictObject(dict):
        def __init__(self, initdict=None):
            if initdict is None:
                initdict = {}
            dict.__init__(self, initdict)

        def __getattr__(self, item):
            return self.__getitem__(item)

        def __setattr__(self, item, value):
            self.__setitem__(item, value)

        def __str__(self):
            if '_text' in self:
                return self['_text']
            else:
                return dict.__str__(self)

        @staticmethod
        def wrap(x):
            if isinstance(x, dict):
                return XmlDictObject ((k, XmlDictObject.wrap(v))
                                      for (k, v) in x.iteritems())
            elif isinstance(x, list):
                return [XmlDictObject.wrap(v) for v in x]
            else:
                return x

        @staticmethod
        def _unwrap(x):
            if isinstance(x, dict):
                return dict ((k, XmlDictObject._unwrap(v))
                             for (k, v) in x.iteritems())
            elif isinstance(x, list):
                return [XmlDictObject._unwrap(v) for v in x]
            else:
                return x

        def unwrap(self):
            return XmlDictObject._unwrap(self)

        pass # Class XmlDictObject
    
    def _dict2xml_recurse(parent, dictitem):
        assert type(dictitem) is not type(list)

        if isinstance(dictitem, dict):
            for (tag, child) in dictitem.iteritems():
                if str(tag) == '_text':
                    parent.text = str(child)
                elif type(child) is type(list):
                    for listchild in child:
                        elem = ElementTree.Element(tag)
                        parent.append(elem)
                        _dict2xml_recurse (elem, listchild)
                else:                
                    elem = ElementTree.Element(tag)
                    parent.append(elem)
                    _dict2xml_recurse (elem, child)
        else:
            parent.text = str(dictitem)
    
    def dict2xml(xmldict):
        """convert a python dictionary into an XML tree"""
        roottag = xmldict.keys()[0]
        root = ElementTree.Element(roottag)
        _dict2xml_recurse (root, xmldict[roottag])
        return root

    def _xml2dict_recurse (node, dictclass):
        nodedict = dictclass()

        if len(node.items()) > 0:
            # if we have attributes, set them
            nodedict.update(dict(node.items()))

        for child in node:
            # recursively add the element's children
            newitem = _xml2dict_recurse (child, dictclass)
            if nodedict.has_key(child.tag):
                # found duplicate tag, force a list
                if type(nodedict[child.tag]) is type([]):
                    # append to existing list
                    nodedict[child.tag].append(newitem)
                else:
                    # convert to list
                    nodedict[child.tag] = [nodedict[child.tag], newitem]
            else:
                # only one, directly set the dictionary
                nodedict[child.tag] = newitem

        if node.text is None: 
            text = ''
        else: 
            text = node.text.strip()

        if len(nodedict) > 0:            
            # if we have a dictionary add the text as a dictionary value
            # (if there is any)
            if len(text) > 0:
                nodedict['_text'] = text
        else:
            # if we don't have child nodes or attributes, just set the text
            if node.text: nodedict = node.text.strip()
            else:         nodedict = ""



        return nodedict
        
    def xml2dict (root, dictclass=XmlDictObject):
        """convert an xml tree into a python dictionary
        """
        return dictclass({root.tag: _xml2dict_recurse (root, dictclass)})
    #####################################################################

except ImportError:
    print "**WARNING: could not import 'xml.etree' (check your python version)"
    print "           you won't be able to correctly read GRL XML files !"
    
def extract_data_from_xml(fname="GRL.xml"):
    """simple helper function to convert a GRL xml file into a list
    of tuples (run-nbr, lumi-block-start, lumi-block-stop)
    """
    import sys
    assert "xml.etree" in sys.modules, \
           "no 'xml.etree' module were imported/available"
    data =[]
    dd=xml2dict(etree.ElementTree.parse(str(fname)).getroot())

    lbks = dd['LumiRangeCollection']['NamedLumiRange']['LumiBlockCollection']
    if not isinstance(lbks, (list, tuple)):
        lbks = [lbks]
    for lbk in lbks:
        assert isinstance(lbk,dict), \
               "expect a dict-like object (got type=%s - value=%r)" % (type(lbk), repr(lbk))
        runnumber=lbk['Run']
        run_ranges=lbk['LBRange']

        #xml2dict return a dataset when only one lbn range per run
        #and return a list when there are several lbn ranges per run
        #==> need different piece of code
        #The following lines 'convert' a dict into a list of 1 dict 
        if isinstance(run_ranges,dict):
            run_ranges=[run_ranges]
            pass

        #loop over run ranges
        for lbrange in run_ranges: 
            lbn_min=lbrange['Start']
            lbn_max=lbrange['End']
            # GRL schema changed from:
            #  <LumiBlockCollection>
            #     <Run>178044</Run>
            #     <LBRange Start="42" End="666"/>
            #    ...
            # to:
            #  <LumiBlockCollection>
            #     <Run PrescaleRD0="8" PrescaleRD1="8">178044</Run>
            #     <LBRange Start="42" End="666"/>
            #    ...
            if isinstance(runnumber, XmlDictObject):
                runnumber = runnumber['_text']
            #print runnumber,"  ", lbn_min,"  ", lbn_max
            data.append((runnumber, lbn_min, lbn_max))
            pass
    return data

### script entry point ###
if __name__ == "__main__":
    sys.exit(main())

"""
tests:

xrdcp root://eosatlas.cern.ch//eos/atlas/user/b/binet/utests/utests/filter-d3pd/ntuple.0.root .
xrdcp root://eosatlas.cern.ch//eos/atlas/user/b/binet/utests/utests/filter-d3pd/ntuple.1.root .
cat > input.txt << EOF
ntuple.0.root
ntuple.1.root
EOF
cat > vars.txt << EOF
-*
+el_vertx
+el_verty
+el_L2_errpt
EOF
filter-and-merge-d3pd -i input.txt -o merged.root -t egamma --var=vars.txt -s ''
filter-and-merge-d3pd -i input.txt -o merged.root -t egamma --var=vars.txt -s 't.el_verty.size() > 0 and t.el_verty[0]>=0.'
cat > foo.py << EOF
def filter_fct(t):
    return t.el_verty.size() > 0 and t.el_verty[0]>=0.
EOF
filter-and-merge-d3pd -i input.txt -o merged.root -t egamma --var=vars.txt -s 'file:foo.py'
"""
