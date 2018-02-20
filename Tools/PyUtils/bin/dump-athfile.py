#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    dump-athfile.py
# @purpose: simple command-line utility wrapping PyUtils.AthFile.fopen
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    May 2009
#
# @example:
# @code
# dump-athfile aod.pool.root
# dump-athfile daq.data
# dump-athfile /castor/cern.ch/user/j/johndoe/aod.pool.root
# dump-athfile somedir/*/*.pool
# @endcode
#

__version__ = "$Revision: 1.4 $"
__author__  = "Sebastien Binet <binet@cern.ch>"

import sys
import os

try:                import cPickle as pickle
except ImportError: import pickle
    
from optparse import OptionParser

if __name__ == "__main__":

    parser = OptionParser(usage="usage: %prog [options] [-f] my.file")
    parser.add_option( "-f",
                       "--file",
                       dest = "fname",
                       help = "The path to the POOL/BS file to analyze" )
    parser.add_option( "-o",
                       "--output",
                       dest = "oname",
                       default = 'athfile-infos.ascii',
                       help = "Name of the output file which will contain the informations gathered during AthFile processing. These informations will be stored into a python-shelve file." )
    parser.add_option( '--evtmax',
                       dest = 'evtmax',
                       default = 1,
                       type = int,
                       help = 'Maximum number of events to process in the file(s)')
    parser.add_option( '--debug',
                       dest = 'debug',
                       default = False,
                       action='store_true',
                       help = 'enable debugging informations')
    
    (options, args) = parser.parse_args()

    if options.debug:
        os.environ['ATHFILE_DEBUG'] = '1'
        os.environ['PYUTILS_SHUTUP_DEBUG'] = '1'
        pass
        
    fnames = []
    
    if len(args) > 0:
        fnames = [ arg for arg in args if arg[0] != "-" ]
        pass

    if options.fname == None and len(fnames) == 0:
        str(parser.print_help() or "")
        sys.exit(1)

    if options.fname != None:
        fname = os.path.expandvars(os.path.expanduser(options.fname))
        fnames.append(fname)

    fnames = list(set(fnames))
    sc = 0
    import PyUtils.AthFile as af
    msg = af.msg
    infos = []
    try:
        infos = af.pfopen(fnames, evtmax=options.evtmax)
    except Exception, e:
        msg.error("Caught exception [%s] !!", str(e.__class__))
        msg.error("What:\n%s\n%s\n%s",e,
                  sys.exc_info()[0],
                  sys.exc_info()[1])
        sc = 1
        pass

    except :
        msg.error("Caught something !! (don't know what)")
        msg.error("\n%s\n%s",sys.exc_info()[0], sys.exc_info()[1])
        sc = 10
        pass

    if sc != 0:
        msg.info("Bye.")
        sys.exit(sc)
        pass
    
    for f in infos:
        file_size = f.infos['file_size']/1024./1024.
        if file_size < 0: file_size = None
        else:             file_size = str(file_size)+' MB'

        msg.info(':'*80)
        msg.info('::::: summary :::::')
        fmt = ' - %-15s: %s'
        print fmt % ('file md5',       f.infos['file_md5sum'])
        print fmt % ('file name',      f.infos['file_name'])
        print fmt % ('file type',      f.infos['file_type'])
        print fmt % ('file size',      file_size)
        print fmt % ('file guid',      f.infos['file_guid'])
        print fmt % ('nentries',       f.infos['nentries'])
        print fmt % ('run number',     f.infos['run_number'])
        print fmt % ('run type',       f.infos['run_type'])
        print fmt % ('evt number',     f.infos['evt_number'])
        print fmt % ('evt type',       f.infos['evt_type'])
        print fmt % ('mc channel #',   f.infos['mc_channel_number'])
        print fmt % ('lumi block',     f.infos['lumi_block'])
        print fmt % ('beam energy',    f.infos['beam_energy'])
        print fmt % ('beam type',      f.infos['beam_type'])
        print fmt % ('stream tags',    f.infos['stream_tags'])
        print fmt % ('stream names',   f.infos['stream_names'])
        print fmt % ('geometry',       f.infos['geometry'])
        print fmt % ('conditions tag', f.infos['conditions_tag'])
        _metadata = f.infos['metadata']

        if _metadata is not None:
            # ATEAM-162: determine if this is 25ns or 50ns sub-campaign
            DigitizationParameters = _metadata['/Digitization/Parameters'] if '/Digitization/Parameters' in _metadata.keys() else {}
            if DigitizationParameters is not None and 'bunchSpacing' in DigitizationParameters.keys() and 'BeamIntensityPattern' in DigitizationParameters.keys() :
                bunchSlotLength = DigitizationParameters['bunchSpacing']
                pattern = DigitizationParameters['BeamIntensityPattern']
                if pattern is not None:
                    firstBunch = pattern.index(1.0)
                    bunchCountInTwoFirstSlots = pattern[firstBunch:firstBunch+2].count(1.0)
                    if bunchCountInTwoFirstSlots == 1:
                        campaign = '50 ns'
                    elif bunchCountInTwoFirstSlots == 2:
                        campaign = '25 ns'
                    else:
                        campaign = None
                    print fmt % ('bunch spacing',   campaign + ' i.e. ..., ' + str(pattern[firstBunch:firstBunch+4])[1:-1] + ', ...')

            _metadata = _metadata.keys() if isinstance(_metadata,dict) else None
        print fmt % ('meta data',      _metadata)

        msg.info(':'*80)
        if len(infos) > 1:
            print ""
        pass # loop over infos
    
    if options.oname:
        oname = options.oname
        msg.info("saving report into [%s]..." % oname)
        if os.path.exists(oname):
            os.rename(oname, oname+'.bak')
        af.server.save_cache(oname)

    msg.info("Bye.")
    sys.exit(sc)
