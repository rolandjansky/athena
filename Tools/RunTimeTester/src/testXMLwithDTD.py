# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys

if __name__ == '__main__':
    import os, getopt

    from xml.parsers.xmlproc import xmlproc, xmlval

    opts, args = getopt.getopt(sys.argv[1:], '')
    
    if len(args) != 1:
        print 'must supply the full path of the xml file'
        sys.exit(0)

    fn = args[0]
    if not os.path.exists(fn):
        print '%s: inexistant!' % fn
        sys.exit(0)

    try:
        validator = xmlval.XMLValidator()
        validator.parse_resource(fn)
        print '%s: valid OK.' % fn
    except Exception, e:
        print '%s: NOT valid: FAIL.' % fn
