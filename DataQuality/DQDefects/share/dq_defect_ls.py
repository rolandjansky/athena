#! /usr/bin/env python
"""
dq_defect_ls.py

A short script to list defects.
"""

import fnmatch

from argparse import ArgumentParser

from DQDefects import DefectsDB
from DQDefects.db import DEFAULT_CONNECTION_STRING

def get_primary(ddb):
    return ddb.defect_names
def get_virtual(ddb):
    return ddb.virtual_defect_names
def get_both(ddb):
    return ddb.defect_names | ddb.virtual_defect_names

def show_tags(connection_string, args):
    ddb = DefectsDB(connection_string)
    
    nspec = sum(bool(x) for x in (args.tags, args.defect_tags, args.logic_tags))
    assert nspec == 1, "Only one of --tags, --logic-tags and --defect-tags can be specified."
    
    if args.tags:
       tags = ddb.tags
    if args.logic_tags:
        tags = ddb.logics_tags
    if args.defect_tags:
        tags = ddb.defects_tags
    
    for t in tags:
        print t

def show_defects(connection_string, tag, get_defects, patterns, args):
    ddb = DefectsDB(connection_string, tag=tag)
    
    all_defects = get_defects(ddb)
    
    for pattern in patterns:
        if len(patterns) != 1:
            print # Extra newline to separate the patterns
            print "%s:" % pattern
        for defect in sorted(fnmatch.filter(all_defects, pattern)):
            print defect

def show_defects_with_desc(connection_string, tag, get_defects, patterns, args):
    ddb = DefectsDB(connection_string, tag=tag)
    
    descriptions = ddb.all_defect_descriptions if args.descriptions else {}
    
    all_defects = get_defects(ddb)
    all_matching = [d for pat in patterns for d in fnmatch.filter(all_defects, pat)]
    if not all_matching:
        return
    
    get_virtuality = lambda defect: ""
    if args.show_virtuality:
        primary_defects = ddb.defect_names
        def get_virtuality(defect):
            if defect in primary_defects:
                return "[P] "
            return "[V] "
    
    ldl = longest_defect_length = max(len(d) for d in all_matching)
    
    for pattern in patterns:
        if len(patterns) != 1:
            print "%s:" % pattern
        for defect in sorted(fnmatch.filter(all_defects, pattern)):
            desc = descriptions.get(defect, "")
            print "{virtuality}{0:<{width}}: {1}".format(
                defect, desc, width=ldl, virtuality=get_virtuality(defect))
        print
        
def show_defects_with_logic(connection_string, tag, patterns, args):
    ddb = DefectsDB(connection_string, tag=tag)
    
    defects = ddb.virtual_defect_names
    logics = ddb.virtual_defect_logics
    
    all_matching = [d for pat in patterns for d in fnmatch.filter(defects, pat)]
    if not all_matching:
        return
    
    ldl = longest_defect_length = max(len(d) for d in all_matching)
    
    for pattern in patterns:
        if len(patterns) != 1:
            print "%s:" % pattern
        for defect in sorted(fnmatch.filter(defects, pattern)):
            print "{0:<{width}}: {1}".format(
                defect, ", ".join(logics[defect].clauses), width=ldl)
        print

def main():
    parser = ArgumentParser(description="List information from the defects "
                                        "database")
    a = parser.add_argument
    add_group = parser.add_argument_group
    
    a("-c", "--connection-string", default=DEFAULT_CONNECTION_STRING,
      help="Database connection to use (default: %s)" % DEFAULT_CONNECTION_STRING)
    
    #
    # TAGS
       #
    tag_group = add_group("tags", "Show information about tags")
    a = tag_group.add_argument
    
    a("--tags", action="store_true",
      help="List all available hierarchical tags")
    
    a("--logic-tags", action="store_true",
      help="Show all logic tags")
    
    a("--defect-tags", action="store_true",
      help="Show all defect tags")
    
    #
    # DEFECTS
    #
    defect_group = add_group("defects", "Show information about defects")
    a = defect_group.add_argument 
      
    a("-t", "--tag", default=None,
      help="Tag to use (default: HEAD)")
      
    a("-l", "--logic", action="store_true",
      help="Show logic used for virtual flags (use -v)")
      
    a("-V", "--show-virtuality", action="store_true",
      help="Indicate virtual vs nonvirtual flags")

    a("-v", "--virtual", action="store_true",
      help="Show only virtual flags")

    a("-p", "--primary", action="store_true",
      help="Show only primary flags")

    a("-d", "--descriptions", action="store_true",
      help="Show descriptions")
      
    a("patterns", metavar="defect", type=str, nargs="*", default=[],
      help="One or more defect globs")

    # These options shouldn't be specified with tag listing options
    DEFECT_OPTIONS = ["logic", "show_virtuality", "virtual", "primary", 
                      "descriptions", "patterns", "tag"]

    args = parser.parse_args()
    
    #print args
    
    if args.tags or args.logic_tags or args.defect_tags:
    
        for arg in DEFECT_OPTIONS:
            if getattr(args, arg):
                raise RuntimeError("--%s doesn't work with any --tags options" % arg)
                
        show_tags(args.connection_string, args)
       
    else:    
        if not args.patterns:
            # Default: show all
            args.patterns = ["*"]
        if not args.tag:
            args.tag = "HEAD"
        
        if args.logic:
            assert not args.primary, "--primary doesn't work with --logic"
            assert not args.descriptions, "--descriptions doesn't work with --logic"
            assert not args.show_virtuality, "--show-virtuality doesn't work with --logic"
            
            arguments = args.connection_string, args.tag, args.patterns, args
            
            show_defects_with_logic(*arguments)
            return
        
        get_defects = get_both
        if args.primary or args.virtual:
            if args.primary and not args.virtual:
                get_defects = get_primary
            else:
                get_defects = get_virtual
        
        arguments = args.connection_string, args.tag, get_defects, args.patterns, args
        
        if args.descriptions:
            show_defects_with_desc(*arguments)
        else:
            show_defects(*arguments)

if __name__ == "__main__":
    main()
