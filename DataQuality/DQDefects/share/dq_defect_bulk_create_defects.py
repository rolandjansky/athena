#!/usr/bin/env python

if __name__ == '__main__':
    import optparse, sys, re
    parser = optparse.OptionParser()
    parser.usage = 'Usage: %prog [options] inputfiles'
    parser.add_option('--db', default='test_defects.db/COMP200',
                      help='Change database defects will be added to')
    parser.add_option('--create', default=False, action='store_true',
                      help='Will create database if set')
    parser.add_option('--ignoreold', default=False, action='store_true',
                      help='Ignore attempted redefinitions of existing defects')

    opts, args = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(1)

    tocreate = []
    good_defect_regex = re.compile('^[A-Za-z0-9_]*$')
    for f in args:
        infile = open(f, 'r')


        for line in infile:
            line = line.strip()
            if line == '' or line[0] == '#':
                continue
            linep = line.partition(' ')
            comment = linep[2].strip()
            if comment == '':
                print 'Missing comment:', line
                sys.exit(1)
            defect = linep[0].strip()
            if not good_defect_regex.match(defect):
                print 'Invalid defect name:', defect
                sys.exit(1)
            tocreate.append((defect, comment))
    
    #print '\n'.join(map(str,tocreate))
    from DQDefects import DefectsDB
    from DQDefects.exceptions import DefectExistsError
    #from DQUtils import init_logger
    #init_logger(2)
    ddb = DefectsDB(opts.db, create=opts.create, read_only=False)

    print 'Now creating defects on', opts.db
    for defect, comment in tocreate:
        try:
            print 'Creating', defect, comment
            ddb.create_defect(defect,comment)
        except DefectExistsError:
            if opts.ignoreold:
                print 'Defect', defect, 'already exists; ignoring'
            else:
                raise
