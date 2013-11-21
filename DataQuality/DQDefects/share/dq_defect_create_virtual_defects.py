#!/usr/bin/env python

# This script populates the virtual defect folder with
# 1. the old primary DQ flags, set to have empty clauses
# 2. the old virtual flags (with definitions as per the iov/tag specified)

DEPRECATED_PRIMARY_FLAGS = ['IDPF', 'LCD', 'MET', 'IDBCM', 'TIGB']

if __name__ == '__main__':
    import optparse, sys, re
    from DQUtils.db import Databases, fetch_iovs
    from DQUtils.channel_mapping import get_channel_ids_names
    from DQDefects import DefectsDB
    from DQDefects.exceptions import DefectExistsError
    from DetectorStatus.DetStatusLib import DetStatusNames
    
    parser = optparse.OptionParser()
    parser.add_option('--db', default='test_defects.db/COMP200',
                      help='Set output database')
    parser.add_option('--origpridb', default='LBSUMM',
                      help='Set input database (connection string format as in DQUtils.db.Databases)')
    parser.add_option('--origvfdb', default='VIRTUALFLAGS',
                      help='Set input database (connection string format as in DQUtils.db.Databases)')
    parser.add_option('--create', default=False, action='store_true',
                      help='If set, will create output database')
    parser.add_option('--vfrun', default=160000, type='int',
                      help='Set the run number to get virtual flag definitions from')
    parser.add_option('--vftag', default='DetStatusVIRTUALFLAGS-03',
                      help='Set the tag of the virtual flags folder to copy from')
    parser.add_option('--ignoreold', default=False, action='store_true',
                      help='Ignore attempted redefinitions of existing defects')
    parser.add_option('--defecttag', default='HEAD',
                      help='Set the tag of the virtual defect folder to write to')

    opts, args = parser.parse_args()

    db = DefectsDB(opts.db, read_only=False, tag=opts.defecttag)

    primary_names = DetStatusNames()
    
    with db.storage_buffer:
        print 'Populating old primary flags'
        ## for defect in primary_names.allNames():
        ##     #print defect
        ##     try:
        ##         print db.new_virtual_defect(0, defect, defect, ''), defect
        ##     except DefectExistsError:
        ##         if opts.ignoreold:
        ##             print 'Defect', defect, 'already exists; ignoring'
        ##         else:
        ##             raise
        folder = Databases.get_folder(opts.origpridb, read_only=True)
        ids, names, cdict = get_channel_ids_names(folder)
        for defect in names:
            if defect in DEPRECATED_PRIMARY_FLAGS:
                print 'Skipping', defect, 'as it is deprecated'
                continue
            #print defect, folder.channelDescription(cdict[defect])
            try:
                print db.new_virtual_defect(defect, defect, ''), defect
            except DefectExistsError:
                if opts.ignoreold:
                    print 'Defect', defect, 'already exists; ignoring'
                else:
                    raise

        print 'Populating old virtual flags'
        folder = Databases.get_folder(opts.origvfdb, 
                                              read_only=True)

        ids, names, cdict = get_channel_ids_names(folder)
        iovs = fetch_iovs(opts.origvfdb, (opts.vfrun, 1), (opts.vfrun, 2),
                          tag=opts.vftag, with_channel=True)

        for defect in iovs:
            clause = defect.expression
            clause = clause.replace('worst_of(', '')
            clause = clause.replace(')', '')
            clause = clause.replace(', ', ' ')
            clause = clause.replace(',', ' ')
            dname = cdict[defect.channel]
            try:
                print db.new_virtual_defect(dname, defect.comment, clause), dname, ':', clause
            except DefectExistsError:
                if opts.ignoreold:
                    print 'Defect', dname, 'already exists; ignoring'
                else:
                    raise  
