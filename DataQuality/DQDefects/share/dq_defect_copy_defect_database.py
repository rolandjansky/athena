#!/usr/bin/env python

# This script allows copying from one defect DB to another, where the defects
# are matched by name (not channel number).
# Script does NOT copy virtual defects! Use copy_virtual_defects.py for that.

def parse_runlumi(instr):
    if instr is None: return None
    import re
    from DQUtils.sugar import RunLumi
    m = re.match('\((\d+),(\d+)\)', instr.replace(' ', ''))
    if m is None:
        raise ValueError('Unable to parse ' + instr + '; please specify the limits in the form (run, lb)')
    return RunLumi(int(m.group(1)), int(m.group(2)))

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
                      help='Create defects from input database on output if they do not exist')
    parser.add_option('--intag', default='HEAD', help='Tag to copy from input database')
    parser.add_option('--outtag', default='HEAD', help='Tag to copy to on output database')
    parser.add_option('--since', default=None, help='Start run/lb: specify as "(run, lb)"')
    parser.add_option('--until', default=None, help='End run/lb: specify as "(run, lb)"')
    parser.add_option('--defects', default=None, help='Only copy specific defects; give a comma-separated list')
    parser.add_option('--noabsent', default=False, action='store_true',
                      help='Do not copy absent defects')

    opts, args = parser.parse_args()

    if len(args) != 2:
        parser.print_help()
        sys.exit(1)

    if opts.createdb:
        opts.createdefects = True

    indb = DefectsDB(args[0], read_only=True, tag=opts.intag)
    outdb = DefectsDB(args[1], create=opts.createdb, read_only=False,
                      tag=opts.outtag)
    
    since = parse_runlumi(opts.since)
    until = parse_runlumi(opts.until)
    channels = parse_channels(opts.defects)

    print 'Reading in IOVs...'
    iniovs = indb.retrieve(since=since, until=until, channels=channels, primary_only=True, nonpresent=(not opts.noabsent))
    print '%d IOVs retrieved from input database' % len(iniovs)
    #inchannels = set((x.channel for x in iniovs))
    inchannels = set(channels if channels is not None else indb.defect_names)
    outchannels = set(outdb.defect_names)
    missingchannels = inchannels-outchannels
    if len(missingchannels) != 0:
        if not opts.createdefects:
            print 'Missing defect(s) in target database:'
            print list(missingchannels)
            print 'Rerun with a restricted defect list (--defects) or with --createdefects'
            sys.exit(1)
        else:
            print 'Creating missing channels on output database'
            descriptions = indb.get_channel_descriptions(missingchannels)
            for channel in missingchannels:
                outdb.create_defect(channel, descriptions[channel])
    
    with outdb.storage_buffer:
        print 'Writing out IOVs...'
        for iov in iniovs:
            outdb.insert(iov.channel, iov.since, iov.until, iov.comment,
                         iov.user, iov.present, iov.recoverable)

    print 'Done.'
