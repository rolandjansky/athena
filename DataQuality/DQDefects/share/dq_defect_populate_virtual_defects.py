#!/usr/bin/env python

if __name__ == '__main__':
    import optparse, sys, re, yaml
    parser = optparse.OptionParser()
    parser.usage = 'Usage: %prog [options] inputfile(s)'
    parser.add_option('--db', default='test_defects.db/COMP200',
                      help='Change database virtual defects will be added to')
    parser.add_option('--create', default=False, action='store_true',
                      help='Will create database if set')
    parser.add_option('--tag', default='HEAD',
                      help='Choose tag to store virtual defects in')
##     parser.add_option('--ignore_redef', default=False, action='store_true',
##                       help='Ignore redefinitions of defects in multiple files')
    

    opts, args = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(1)

    indict = {}
    for f in args:
        try:
            tmp_indict = yaml.safe_load(open(f, 'r'))
        except yaml.scanner.ScannerError, e:
            print 'ERROR: Problem parsing file', f
            print str(e)
            sys.exit(1)
        if not tmp_indict:
            print 'NOTICE: No definitions in file', f
            continue
        interset = set(tmp_indict) & set(indict)
        if len(interset) > 0:
            for k in interset:
                print 'WARNING: Redefinition of defect', k, 'in file', f
                if not opts.ignore_redef:
                    sys.exit(1)
        indict.update(tmp_indict)

    #print '\n'.join(map(str,tocreate))
    from DQDefects import DefectsDB
    from DQDefects.exceptions import DefectUnknownError
    ddb = DefectsDB(opts.db, create=opts.create, read_only=False,
                    tag=opts.tag)

    print 'Now updating defects on', opts.db
    with ddb.storage_buffer:
        for defect, clause in indict.items():
            print defect, clause
            try:
                ddb.update_virtual_defect(defect, ' '.join(clause))
            except DefectUnknownError:
                ddb.new_virtual_defect(defect, '', ' '.join(clause))
                #print 'ERROR: defect', defect, 'not present in DB, unable to update'
                #sys.exit(1)
