# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: McParticleUtils/python/DecayParser.py
# @purpose: implement the parsing/tokenization of decay-patterns' strings
# @author:  Sebastien Binet <binet@cern.ch>

import re

_slot_separator = "+"
_candidate_sep  = "|"
_decay_arrow    = "->"
_wild_card      = "*"

def py_parse (cmd):
    # remove all spaces to ease further parsing
    cmd = re.sub(pattern=" ",
                 repl="",
                 string=cmd)
##     print "=> [%s]"%cmd

    if cmd == _decay_arrow:
        # special case of a single arrow without any parent nor child:
        # this decay pattern will select every single vertex
        return (0, None, None)
    buf = cmd.split (_decay_arrow)
    assert (len(buf)==2)
    
##     print "==> buf:",buf
    parents = process_block (buf[0])
    children= process_block (buf[1])
    
    return (0, parents, children)

def process_block (cmd):
    if cmd == "":
        return None
    candidates = cmd.split (_slot_separator)
    return [list(set(c.split(_candidate_sep))) for c in candidates]


if __name__ == "__main__":
    print ":"*80
    print "::: tests..."
    for cmd in ("-1|-2|-3|-4|-5|-6|21 + 1|2|3|4|5|6|21 -> ",
                "-> 91|92|94"
                ):
        print "::: testing [%s]..." % cmd
        print "::: result:",py_parse (cmd)

    print "::: bye."
    print ":"*80
    
