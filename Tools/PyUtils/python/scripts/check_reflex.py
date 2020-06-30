# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.check_reflex
# @purpose a script to check the definitions of (reflex) plugins
#          across multiple so-called 'rootmap' files
# @author Sebastien Binet
# @date February 2010

from __future__ import print_function

__doc__ = """
a script to check the definitions of (reflex) plugins across multiple so-called 'rootmap' files
"""
__author__ = "Sebastien Binet"


### imports -------------------------------------------------------------------
import PyUtils.acmdlib as acmdlib

@acmdlib.command(name='chk-rflx')
@acmdlib.argument(
    '--capabilities',
    nargs='?',
    default=None,
    help="Dump the capabilities of a given library (ex: libAthenaServices.so)")
@acmdlib.argument(
    '--dups',
    dest='chk_dups',
    default=None,
    help="Check if there is any duplicates among dictionaries for a given library")
@acmdlib.argument(
    '--dump-content',
    action='store_true',
    default=False,
    help="Dump the content of all the known plugins (dicts. and components)")
@acmdlib.argument(
    "--dso",
    dest = "dump_dso",
    action = "store_true",
    default = False,
    help = "Dump all the dsomap/rootmap files known to the Dso repository")
@acmdlib.argument(
    "--libs",
    dest = "dump_libs",
    action = "store_true",
    default = False,
    help = "Dump all the libraries known to the Dso repository")
@acmdlib.argument(
    "--check-dict-dups",
    action = "store_true",
    default = False,
    help = "Check if there is any duplicates among dictionaries")
@acmdlib.argument(
    "--check-pf-dups",
    action = "store_true",
    default = False,
    help = "Check if there is any duplicates among components declared to the PluginSvc")
@acmdlib.argument(
    "--check-all-dups",
    action = "store_true",
    default = False,
    help = "Check dictionaries *and* components")
@acmdlib.argument(
    "--detailed-dump",
    action = "store_true",
    default = False,
    help = "Performs a detailed dump if duplicates are found")
@acmdlib.argument(
    "--pedantic",
    action = "store_true",
    default = False,
    help = """Pedantic mode: if a component is found in 2 libraries which have
    the same name (usual case of a developer working on a (set of) package(s)),
    it is still being reported as being duplicated""")
@acmdlib.argument(
    "-l",
    "--level",
    default = "INFO",
    help = "Logging level (aka verbosity)")
def main(args):
    """a script to check the definitions of (reflex) plugins
    across multiple so-called 'rootmap' files
    """
    exitcode = 0

    print (":"*80)
    print ("::: chk-rflx :::")

    import os
    import PyUtils.Dso as Dso

    _suppression_dct = {
        'TMath' : ('libCore.so', 'libMathCore.so'),
        'string': ('libGaudiKernelDict.so',
                   'libCore.so',
                   'liblcg_PyCoolDict.so',
                   'libSTLAddRflx.so'),
        '__pf__::CNV_71_9631': ('libDataModelTestDataReadCnvPoolCnv.so',
                                 'libDataModelTestDataWriteCnvPoolCnv.so',),
        '__pf__::CNV_71_9632': ('libDataModelTestDataReadCnvPoolCnv.so',
                                 'libDataModelTestDataWriteCnvPoolCnv.so',),
        '__pf__::CNV_71_9633': ('libDataModelTestDataReadCnvPoolCnv.so',
                                'libDataModelTestDataWriteCnvPoolCnv.so',),
        '__pf__::CNV_71_9634': ('libDataModelTestDataReadCnvPoolCnv.so',
                                'libDataModelTestDataWriteCnvPoolCnv.so',),
        '__pf__::CNV_71_9639': ('libDataModelTestDataReadCnvPoolCnv.so',
                                'libDataModelTestDataWriteCnvPoolCnv.so',),
        ## FIXME !! this shouldn't be suppressed !!
        '__pf__::RootCollection': ('liblcg_RootCollection.so',
                                   'libAthAnalysisTools.so',),
        ## !!
        
        }

    
    def print_db( db, detailedDump = False ):
        if detailedDump : fct = lambda x: x
        else:             fct = os.path.basename
        keys = db.keys()
        keys.sort()
        for k in keys:
            print ("%s:" % k)
            libs = db[k]
            libs.sort()
            for lib in libs:
                print ("  ",fct(lib))
        return

    dsodb = Dso.DsoDb()

    if args.capabilities:
        libname = args.capabilities
        try:
            capabilities = dsodb.capabilities(libname)
            print ("::: capabilities of [%s]" % (libname,))
            print (os.linesep.join([" %s"%c for c in capabilities]))
        except ValueError:
            exitcode = 1
            pass

    if args.chk_dups:
        libname = args.chk_dups
        try:
            print ("::: checking duplicates for [%s]..." % (libname,))
            dups = dsodb.duplicates(libname, pedantic=args.pedantic)
            for k in dups:
                print (" -",k)
                print (os.linesep.join([" %s"%v for v in dups[k]]))
            if len(dups.keys())>0:
                exitcode = 1
        except ValueError:
            exitcode = 1
            pass

    if args.dump_content:
        print ("::: dumping content of all known plugins...")
        entries = dsodb.content(pedantic=args.pedantic)
        print_db(entries, args.detailed_dump)
        print ("::: known entries:",len(entries.keys()))

    if args.dump_libs:
        print ("::: dumping all known libraries...")
        libs = dsodb.libs(detailedDump=args.detailed_dump)
        for lib in libs:
            print (" -",lib)
        print ("::: known libs:",len(libs))

    if args.dump_dso:
        print ("::: dumping all known dso/rootmap files...")
        dso_files = [dso for dso in dsodb.dsoFiles]
        dso_files.sort()
        for dso_file in dso_files:
            if not args.detailed_dump:
                dso_file = os.path.basename(dso_file)
            print (" -",dso_file)
        print ("::: known dsos:",len(dso_files))

    if args.check_dict_dups:
        print (":: checking dict. duplicates...")
        dups = dsodb.dictDuplicates(pedantic=args.pedantic)
        suppression_log = []
        for k in dups:
            v = dups[k]
            # mark as error only if it isn't a known dup'
            if k in _suppression_dct:
                suppressed = [os.path.basename(ii) in _suppression_dct[k]
                              for ii in v]
                if all(suppressed):
                    suppression_log.append(k[:])
                    pass
                else:
                    # that's a new one !!
                    exitcode = 1
            else:
                # that's a new one !!
                exitcode = 1
                # print ("---> NOT ignoring [%s]" % (k,))
        print_db(dups, args.detailed_dump)
        if len(suppression_log):
            print ("-"*40)
            print ("## ignoring the following dups':")
            for k in suppression_log:
                print (" -",k)
            print ("-"*40)
        print ("## all dups:",len(dups.keys()))
        print ("##     dups:",len(dups.keys())-len(suppression_log))

    if args.check_pf_dups:
        print ("::: checking (plugin factories) components dups...")
        dups = dsodb.pfDuplicates(pedantic=args.pedantic)
        suppression_log = []
        for k in dups:
            v = dups[k]
            # mark as error only if it isn't a known dup'
            if k in _suppression_dct:
                suppressed = [os.path.basename(ii) in _suppression_dct[k]
                              for ii in v]
                if all(suppressed):
                    suppression_log.append(k[:])
                    pass
                else:
                    # that's a new one !!
                    exitcode = 1
            else:
                # that's a new one !!
                exitcode = 1
                # print ("---> NOT ignoring [%s]" % (k,))
        print_db(dups, args.detailed_dump)
        if len(suppression_log):
            print ("-"*40)
            print ("## ignoring the following dups':")
            for k in suppression_log:
                print (" -",k)
            print ("-"*40)
        print ("## all dups:",len(dups.keys()))
        print ("##     dups:",len(dups.keys())-len(suppression_log))

    if args.check_all_dups:
        print ("::: checking all components dups...")
        dups = dsodb.pfDuplicates(pedantic=args.pedantic)
        dups.update(dsodb.dictDuplicates(pedantic=args.pedantic))
        
        suppression_log = []
        for k in dups:
            v = dups[k]
            # mark as error only if it isn't a known dup'
            if k in _suppression_dct:
                suppressed = [os.path.basename(ii) in _suppression_dct[k]
                              for ii in v]
                if all(suppressed):
                    suppression_log.append(k[:])
                    pass
                else:
                    # that's a new one !!
                    exitcode = 1
            else:
                # that's a new one !!
                exitcode = 1
                # print ("---> NOT ignoring [%s]" % (k,))
        print_db(dups, args.detailed_dump)
        if len(suppression_log):
            print ("-"*40)
            print ("## ignoring the following dups':")
            for k in suppression_log:
                print (" -",k)
            print ("-"*40)
        print ("## all dups:",len(dups.keys()))
        print ("##     dups:",len(dups.keys())-len(suppression_log))

    if exitcode:
        print ("::: ERROR !!")
    else:
        print ("::: All good.")

    print (":"*80)
    return exitcode
