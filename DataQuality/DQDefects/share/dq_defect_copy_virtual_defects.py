#!/usr/bin/env python

# This script allows copying of virtual defects from one defect DB to another

def parse_channels(instr):
    if instr is None: return None
    return instr.replace(' ','').split(',')

if __name__ == '__main__':
    import optparse, sys, re
    from DQUtils.db import Databases, fetch_iovs
    from DQUtils.sugar import RunLumi
    from DQUtils.channel_mapping import get_channel_ids_names
    from DQDefects import DefectsDB
    from DQDefects.exceptions import DefectExistsError
    
    parser = optparse.OptionParser(usage="usage: %prog [options] indb outdb\n\nindb and outdb can be COOL connection strings or \"filename/dbname\", e.g. mydb.db/COMP200")
    parser.add_option('--createdb', default=False, action='store_true',
                      help='Create output database if non-existent; implies --createdefects')
    parser.add_option('--createdefects', default=False, action='store_true',
                      help='Create virtual defects from input database on output if they do not exist')
    parser.add_option('--intag', default='HEAD', help='Tag to copy from input database')
    parser.add_option('--outtag', default='HEAD', help='Tag to copy to on output database')
    parser.add_option('--defects', default=None, help='Only copy specific virtual defects; give a comma-separated list')

    opts, args = parser.parse_args()

    if len(args) != 2:
        parser.print_help()
        sys.exit(1)

    if opts.createdb:
        opts.createdefects = True

    indb = DefectsDB(args[0], read_only=True, tag=opts.intag)
    outdb = DefectsDB(args[1], create=opts.createdb, read_only=False,
                      tag=opts.outtag)
    
    channels = parse_channels(opts.defects)

    print 'Reading in definitions...'
    inclauses = indb.virtual_defect_logics
    print '%d virtual defects retrieved from input database' % len(inclauses)
    inchannels = set(channels if channels is not None else inclauses.keys())
    outchannels = set(outdb.virtual_defect_names)
    missingchannels = inchannels-outchannels
    with outdb.storage_buffer:
        if len(missingchannels) != 0:
            if not opts.createdefects:
                print 'Missing virtual defect(s) in target database:'
                print list(missingchannels)
                print 'Rerun with a restricted virtual defect list (--defects) or with --createdefects'
                sys.exit(1)
            else:
                print 'Creating missing virtual defects on output database...'
                descriptions = indb.get_virtual_channel_descriptions(missingchannels)
                for channel in missingchannels:
                    outdb.new_virtual_defect(channel, descriptions[channel],
                                             ' '.join(inclauses[channel].clauses))
                print 'Done'

        print 'Updating existing virtual defects...'
        for channel in inchannels-missingchannels:
            outdb.update_virtual_defect(channel, ' '.join(inclauses[channel].clauses))

        print 'Done.'
