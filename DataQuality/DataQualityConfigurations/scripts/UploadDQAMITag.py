#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# 10-Sep-2010, Peter Onyisi <ponyisi@cern.ch>
#
# Script for uploading a new DQ configuration to AMI "h" tag
# ASSUMPTION: the binary DQ configurations are in AFS; they have the following
# directory structure:
#   $BASEDIR / Cosmics    / cosmics_minutes10.current.hcfg
#                         / cosmics_minutes30.current.hcfg
#                         / cosmics_run.current.hcfg
#            / Collisions / collisions_*current.hcfg
#            / HeavyIons  / heavyions_*current.hcfg
# and that the *.current.hcfg files are valid symlinks to files in the
# respective directories.

import sys, os

AMI_TAG_PREFIX='h'

def get_current_config(amiclient):

    #amicommand = ['ListElement', '-entity=h_config', '-processingStep=Atlas_Production', '-project=Atlas_Production', '-select=tag']
    amicommand = ['SearchQuery', '''-sql="SELECT V_AMITags.tagType, V_AMITags.tagNumber FROM V_AMITags WHERE V_AMITags.tagType = 'h' ORDER BY V_AMITags.tagNumber DESC"''' 
                                 '-project="AMITags"', '-processingStep=production']

    print amicommand

    result = amiclient.execute(amicommand, format='dict_object')
    resultlist = result.get_rows('Element_Info')
    latestTag = 'h' + resultlist[0]['tagNumber']
    #try:
    #    latestTag = 'h' + `max(int(_['tag'][1:]) for _ in resultlist)`
    #except:
    #    latestTag = 'h0'

    nextTag = latestTag[0] + str(int(latestTag[1:])+1)

    print 'Latest AMI tag is', latestTag
    print 'Will create AMI tag', nextTag
    #latestTag='h7'

    amicommand = ['AMIGetAMITagInfo', '-amiTag='+latestTag]

    result = amiclient.execute(amicommand, format='dict_object')
    print '----------'
    #print result
    #print result.get_rowset_types()
    #sprint result.get_rows('amiTagInfo')
    rv = result.get_rows('amiTagInfo')[0]

    print
    print '-------------------------------------'
    print 'Info of current AMI tag ...'
    for key, val in rv.items():
        print '  %s:' % key, val
    rv1 = {}
    for k, v in rv.items():
        rv1[k] = v.__str__()

    #del rv1['readStatus']
    #del rv1['writeStatus']
    for k in ('tag', 'createdBy', 'modifiedBy', 'tagStatus', 'tagNumber', 'tagType',
              'locked', 'updates', 'created', 'lastModified', 'processingStep',
              'transformationName', 'baseRelease'):
        try:
            del rv1[k]
        except KeyError:
            pass
    for k in ('inputs','outputs', 'moreInfo', 'phconfig', 'trfsetupcmd',
              'description'):
        rv1[k] = '"%s"' % str(rv1[k].__str__())

    #rv1['phconfig'] = str(rv1['phconfig'].__str__())
    return rv1, latestTag

def get_next_tag(latestTag):
    nextTag = latestTag[0] + str(int(latestTag[1:])+1)
    #nextTag='h8'
    return nextTag

def update_dict_for_configs(updict, indir):
  
    basedir = os.path.abspath(indir)

    print 'Looking for configurations in subdirectories of', indir

    types = ['minutes10', 'minutes30', 'run']
    searchparams = [('Cosmics', 'cosmics'), ('Collisions', 'collisions'),
                    ('HeavyIons', 'heavyions')]

    filepathdict = {}
    filelist = []

    for dir1, fn in searchparams:
        print dir1
        filepathdict[dir1] = {}
        for t in types:
            print '  ', t, '...',
            fname = os.path.join(basedir, dir1,
                                '%s_%s.current.hcfg' % (fn, t))
            if os.access(fname, os.R_OK):
                print 'found,',
                if os.path.islink(fname) and os.path.isfile(fname):
                    realname = os.readlink(fname)
                    if not os.path.isabs(realname):
                        realname = os.path.join(os.path.dirname(fname), realname)
                    print 'is symlink to', realname
                    filepathdict[dir1][t] = realname
                    filelist.append(realname)
                else:
                    print 'but is not valid symlink'
            else:
                print 'not found'
        if filepathdict[dir1] == {}:
            del filepathdict[dir1]

    commonpart = os.path.dirname(os.path.commonprefix(filelist)) + os.sep

    filepathdict['basename'] = commonpart

    # clean up common part
    for dir1, fn in searchparams:
        for t in types:
            try:
                filepathdict[dir1][t] =  filepathdict[dir1][t].replace(commonpart, '')
            except KeyError:
                pass

    print
    print '-------------------------------------'
    print 'File path dictionary to upload:'
    print filepathdict
    print '-------------------------------------'
    print

    val = updict.get('phconfig', {})
    if isinstance(val, basestring):
        val = {}
    val['filepaths'] = filepathdict
    updict['phconfig'] = '"%s"' % val

def update_dict_for_release(updict, release):
    # From UpdateAMITag.py
    #Check if release exists
    relSp=release.split("-")
    if len(relSp)!=2:
        s="ERROR: Expected parameter 'release' in the form Project-number, got "+release
        raise RuntimeError(s)
    relProj=relSp[0]
    if ',' in relSp[1]:
        relNbr, relAddtl=relSp[1].split(',', 1)
    else:
        relNbr, relAddtl=relSp[1], None
    ### 21.0 stored on cvmfs
    baseRelNbr=".".join(relNbr.split(".")[:2])
    relPath = "/cvmfs/atlas.cern.ch/repo/sw/software/%s/%s/%s" % (baseRelNbr, relProj, relNbr)
    if not os.path.isdir(relPath):
        s="ERROR Release directory " + relPath + " does not exist"
        raise RuntimeError(s)
    #Release exists if we reach this point

    extraSetup = " oracleCOOL"
    if relProj == "AtlasProduction":
        extraSetup += " AtlasProduction"
    elif relProj == "Athena":
        extraSetup += " Athena"
    elif relProj != "AtlasOffline":
        s="ERROR: The project specified (" + relProj +") is not Athena, AtlasOffline or AtlasProduction. Are you sure?"
        raise RuntimeError(s)

    # update dictionary
    tasktransinfo = {'trfpath': 'DQM_Tier0Wrapper_trf.py',
                               'trfsetupcmd': "/afs/cern.ch/atlas/tzero/software/setup/usetuptrf.sh " + relNbr + extraSetup}
    updict['moreInfo'] = '"{\'tasktransinfo\': %s}"' % tasktransinfo.__str__()
    updict['SWReleaseCache'] = release.replace('-', '_')
    updict['groupName'] = relProj
    updict['cacheName'] = relSp[1]
    updict['description'] = "'Trf for combined DQM histogram merging and DQM webpage creation, to get periodic DQ monitoring updates. Using " + release +"'"
    updict['trfsetupcmd'] = '"%s"' % tasktransinfo['trfsetupcmd']
    #updict['tagNumber'] = nextTag[1:]
    #del updict['trfsetupcmd']

def upload_new_config(amiclient, nextTag, updict):

    amicommand = ['AddAMITag', 'tagType="h"', ]
    
    for key, val in updict.items():
        amicommand.append('%s=%s' % (key, val))

    print '-------------------------------------'
    print 
    print 'Now uploading new AMI tag'
    print 'AMI command:', amicommand

    result = amiclient.execute(amicommand)
    print result
    print
    print 'Success!'

if __name__ == '__main__':
    import optparse

    parser = optparse.OptionParser(usage='Usage: %prog [options] cmd arg\n'
                               '  cmd can be:\n'
                               '    configs: update only DQ configurations; args should be the config base directory\n'
                               '    release: update only the release; args should be the new release'
                               )

    parser.set_defaults(certificate=False)
    parser.add_option('-u', '--user', dest='amiuser', help='Set AMI user; should set password at the same time.')
    parser.add_option('-p', '--password', dest='amipass', help='Set AMI password; should set user at the same time.')
    parser.add_option('-c', '--certificate', dest='certificate',
                      action='store_true',
                      help='Use grid certificate/VOMS proxy.')

    options, args = parser.parse_args()

    if len(args) < 2:
        parser.error('Must provide type of operation and argument!')

    if args[0].lower() not in ('configs', 'release'):
        parser.error('cmd must be "configs" or "release"')


    #Get pyAMI client
#    try:
#        from pyAMI.client import Client
#    except ImportError:
#        print "WARNING unable to import AMI from pyAMI with standard $PYTHONPATH."
#        print "Will manually add ZSI and 4suite, then try again..."
#        import sys
#        sys.path.insert(0,'/afs/cern.ch/atlas/offline/external/ZSI/2.1-a1/lib/python')
#        sys.path.insert(0,'/afs/cern.ch/sw/lcg/external/4suite/1.0.2_python2.5/slc4_ia32_gcc34/lib/python2.5/site-packages')
#        from pyAMI.client import Client
#
#    amiclient=Client('atlas', ignore_proxy = not options.certificate)
#    if options.amiuser is not None and options.amipass is not None:
#        amiclient.auth(options.amiuser, options.amipass)

    import pyAMI.client
    import pyAMI.atlas.api as AtlasAPI
    amiclient = pyAMI.client.Client('atlas')
    AtlasAPI.init()

    cfgdict, latestTag = get_current_config(amiclient)
    nextTag = get_next_tag(latestTag)
    
    if args[0].lower() == 'configs':
        update_dict_for_configs(cfgdict, args[1])
    elif args[0].lower() == 'release':
        update_dict_for_release(cfgdict, args[1])

    upload_new_config(amiclient, nextTag, cfgdict)
