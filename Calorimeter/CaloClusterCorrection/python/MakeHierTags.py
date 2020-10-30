# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/MakeHierTags.py
# Created: Apr 2009, sss
# Purpose: Add hierarchical cluster correction tags to a COOL database.
#
# Construct with the database sqlite file and the top-level corrections
# class.  Then call run().  Run takes an optional generation argument
# (the default is taken from the correction class).
#
# Example:
#  from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections
#  mh = MakeHierTags ('swcool.db', CaloSwCorrections)
#  mh.run()
#

from __future__ import print_function

from PyCool import cool
import CoolConvUtilities.AtlCoolLib as AtlCoolLib
from CaloClusterCorrection.constants import CALOCORR_COOL, CALOCORR_JO
from CaloClusterCorrection.common import split_version_spec
import sys


class MakeHierTags:
    def __init__ (self, dbfile, corrtop, dryrun = False, domagic = True):
        self.corrtop = corrtop
        self.dryrun = dryrun
        self.domagic = domagic
        cool.DatabaseSvcFactory.databaseService()

        if dbfile.endswith ('.db'):
            connstring = "sqlite://;schema=%s;dbname=OFLP200" % dbfile
            self.makeleaf = True
        else:
            connstring = dbfile
            self.makeleaf = False

        self.db=AtlCoolLib.indirectOpen(connstring,False,False,True)
        
        # List of all folders.
        self.folders = []

        # Map from tool creation functions to folders.
        self.funcmap = {}

        # Map from (folder, hier-tag) pairs to individual folder tags
        self.tags = {}

        self.get_all_folders (corrtop)
        return


    def get_all_folders (self, corrtop):
        ftmp = {}
        for tooldesc in corrtop.versions['@']:
            tool = tooldesc[0]('', source=[CALOCORR_COOL, CALOCORR_JO])
            folder = None
            try:
                folder = tool.detStoreKey
                if len(folder) > 0:
                    ftmp[folder] = 1
            except AttributeError:
                pass
            self.funcmap[tooldesc[0]] = folder
        self.folders = ftmp.keys()
        return


    def run (self, generation=None):
        if generation is None:
            generation = self.corrtop.correction_generation_default
        if generation != '':
            generation = generation + '-'
            
        for (version, toollist) in self.corrtop.versions.items():
            if version[0] == '@': continue
            self.make_tag (version, toollist, generation)

        if self.domagic:
            self.make_magic (generation)
        return


    def make_tag (self, version, toollist, generation):
        htag = None
        for toolspec in toollist:
            toolfunc = toolspec[0]
            if len(toolspec) >= 2 and isinstance(toolspec[1], str):
                toolversion = toolspec[1]
            else:
                toolversion = ''

            if toolfunc not in self.funcmap:
                print ("WARNING: skipping tagging for tool", toolfunc.__name__)
                continue
            folder = self.funcmap[toolfunc]
            if not folder: continue
            (dum1, dum2, corrclass, basename) = folder.split ('/')

            htmp = corrclass + '-' + generation + version
            if htag is None:
                htag = htmp
                print (htag)
            else:
                assert htag == htmp

            tag = corrclass + '.' + generation + basename
            if len(toolversion) > 0:
                tag = tag + '-' + toolversion

            self.set_tag (folder, tag, htag)

        if htag is None:
            return

        for folder in self.folders:
            if (folder, htag) not in self.tags:
                (dum1, dum2, corrclass, basename) = folder.split ('/')
                tag = corrclass + '.' + generation + basename + '-dummy'
                self.set_tag (folder, tag, htag)
        return


    def make_magic (self, generation):
        geom_strings = scrape_tags()
        if self.makeleaf:
            for g in geom_strings:
                (globver, tryhier) = self.corrtop.geom_match('MakeHierTags', g)
                if globver[0] == '@': continue
                (vlist, version) = self.corrtop.lookup_version (globver, None)
                for f in self.folders:
                    self.make_magic_target (f, g, vlist, generation)
                print ("Made magic targets for", g)
                
        for f in self.folders:
            (dum1, dum2, corrclass, basename) = f.split ('/')
            magictag = 'TagInfo/' + corrclass + "." + generation + basename + \
                       '-/GeoAtlas'
            toptag = corrclass + "-" + generation[:-1]
            self.set_tag (f, magictag, toptag, False)
            print ("Make magic", f)
        return


    def make_magic_target (self, folder, g, vlist, generation):
        (dum1, dum2, corrclass, basename) = folder.split ('/')

        for v in vlist:
            (func, toolversion, order, extra) = split_version_spec (v)
            if self.funcmap.get(func) == folder:
                break
        else:
            toolversion = 'dummy'

        src = corrclass + "." + generation + basename
        if len(toolversion) > 0:
            src = src + "-" + toolversion

        target = corrclass + "." + generation + basename + "-" + g

        self.copy_tag (folder, src, target)

        return


    def copy_tag (self, folder, src, target):
        if self.dryrun:
            print ('--> Clone in', folder, src, '->', target)
            return

        dbf = self.db.getFolder (folder)
        objs = dbf.browseObjects (cool.ValidityKeyMin,
                                  cool.ValidityKeyMax,
                                  cool.ChannelSelection.all(),
                                  src)
        objlist=[]
        while objs.goToNext():
            objlist+=[objs.currentRef()]
        if len(objlist) == 0:
            print ("ERROR: No objects cloning tag %s in folder %s" % 
                   (src, folder))
        for obj in objlist:
            dbf.storeObject (obj.since(),
                             obj.until(),
                             obj.payload(),
                             obj.channelId(),
                             target,
                             True)
        dbf.setTagDescription (target, "Copied from " + src)
        #print ("Cloned", folder, src, target)
        return


    def set_tag (self, folder, tag, htag, check = True):
        if self.dryrun:
            print ('--> Set htag in', folder, tag, htag)
            return

        oldtag = self.tags.get ((folder, htag))
        if oldtag is None:
            self.tags[(folder,htag)] = tag
            if not self.db.existsFolder(folder):
                print ("ERROR: Folder %s doesn't exist" % folder)
                sys.exit(1)
            dbf = self.db.getFolder(folder)
            if check:
                if tag not in dbf.listTags():
                    print ("ERROR: Tag %s doesn't exist in folder %s"
                           % (tag,folder))
                    sys.exit(1)
            try:
                dbf.findTagRelation(htag) # will throw if htag doesn't exist
                dbf.deleteTagRelation(htag)
            except Exception:
                pass
            dbf.createTagRelation (htag, tag)
            #print ('set_tag', folder, tag, htag)
        else:
            assert oldtag == tag, (tag, oldtag, folder, htag)
        return


##############################################################################
# Scrape current list of geom tags from the wiki page.
#

import urllib2
from HTMLParser import HTMLParser

class Parser (HTMLParser):

    def __init__ (self, url):
        HTMLParser.__init__ (self)
        self.tags = []
        self.feed (urllib2.urlopen(url).read())
        self.close()
        return


    def handle_starttag (self, tag, attrs):
        if tag != 'a': return
        href = dict(attrs).get ('href')
        if not href: return
        ii = href.find ('?tag_name=')
        if ii < 0: return
        tag = href[ii + 10:]
        self.tags.append (tag)
        return

def scrape_tags():
    p = Parser('https://twiki.cern.ch/twiki/bin/view/Atlas/AtlasGeomDBTags')
    p.tags.sort()
    return p.tags
