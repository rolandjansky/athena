# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from __future__ import with_statement

import cherrypy
import threading
import sys, os
import traceback
import time, datetime
import multiprocessing
import xml.etree.cElementTree as ET
import urllib
import tempfile
import shutil

def _do_import_and_get_obj(grlconfmod, grlobj = None):
    __import__(grlconfmod)
    m = sys.modules[grlconfmod]
    if grlobj is None:
        grlobj = grlconfmod.rsplit('.')[-1]
    return m.__dict__[grlobj]()

def _deep_get_grl_info(grlconf, cooltag, folder):
    from GoodRunsLists.GoodRunsListTool import getObjectOfClass
    from CoolRunQuery.AtlRunQueryLib import AtlRunQuery, AtlRunQueryOptions
    from CoolRunQuery.AtlRunQueryParser import ArgumentParser

    ## get search query
    config = _do_import_and_get_obj(grlconf)
    config.setdqfolder(folder)
    config.setdqctag(cooltag)
    #config.setPrefix('')

    query  = config.getsearchquery()
    print ">> Calling cmd equivalent of: "
    print "%s \"%s\"" % (config.querytool,query)

    ## atlrunquery parse arguments
    ap = ArgumentParser()
    atlqueryarg = config.querytool + " " + ap.ParseArgument( query )
    (options, args) = AtlRunQueryOptions().parse(atlqueryarg)

    #print atlqueryarg
    #print options

    ## query gets added to the xml file, in the field query
    proc = multiprocessing.Process(target=AtlRunQuery, args=(options,), 
           kwargs={'html':"NO", 'origQuery':query, 'loglevel':0}) # html can be "YES", "NO", "AUTO"
    proc.start()
    proc.join()
    xmlfile = 'data/'+config.listname
    #print ">> Good-run list stored as: \'%s\'" % xmlfile
    return xmlfile


def _get_grl_info(grlconf, cooltag, folder, queue = None):
    sys.path.append(os.path.dirname(grlconf))
    # grlconf is the input file
    tmpdir = tempfile.mkdtemp()
    os.chdir(tmpdir)
    rfile = _deep_get_grl_info(os.path.basename(grlconf).rstrip('.py'), cooltag, folder)
    rv = open(rfile, 'r').read()
    os.chdir(os.environ['TMPDIR'])
    shutil.rmtree(tmpdir)
    #print rv
    queue.put((rv, rfile))
    return

def _get_cool_tags_for_folder(folder):
    tagxml = urllib.urlopen('http://voatlas11.cern.ch:8080/cooldb/ATLAS_COOLPROD/ATLAS_COOLOFL_GLOBAL/COMP200/GLOBAL/DETSTATUS/%s/tags'
                            % folder)
    xml = ET.parse(tagxml)
    rv = []
    for t in xml.findall('tag'):
        text = t.text
        lock = t.attrib.get('lock', 'unlocked')
        description = t.attrib.get('description')
        rv.append((text, description, lock))
    return rv

GRL_DIRECTORY='/afs/cern.ch/user/a/atlasdqm/grl'


TAGS_TO_SUPPRESS = ['DetStatusLBSumm_m4initial',
                    'DetStatusLBSumm-cos08-01',
                    'DetStatusLBSumm-cos09-01',
                    'DetStatusLBSumm-cos08-02',
                    'COMP200_OFL',
                    'DetStatusLBSumm_RECP']

def _get_list_of_grl_classes():
    rl = []
    def walker(rl, d, children):
        for child in children:
            if child[-3:] == '.py' and child != '__init__.py':
                fullpath = os.path.join(d, child)
                classname = fullpath.replace(GRL_DIRECTORY, '')[:-3].lstrip('/').replace('/', '.') 
                rl.append((fullpath, classname))
    os.path.walk(GRL_DIRECTORY, walker, rl)
    return rl

class GRLGen(object):
    def __init__(self):
        self.genlock = threading.Lock()
        pass
    
    @cherrypy.expose
    def index(self):
        import glob
        rv = ['<html><head>'
            '<title>GRL generator</title>'
            '<script type="text/javascript" src="/static/js/grl.js"><!-- -->'
            '</script>'
            '</head><body onload="onLoad()">'
            '<h1>Good Run List Generator</h1>'
            '<p>This generator allows you to create good run list XML files from '
            'configurations that are currently kept on AFS, in <tt>~atlasdqm/grl</tt>.'
            '<p>Using tags:'
            '<p>CoolRunQuery-00-01-92'
            '<p>GoodRunsLists-00-00-71'
            ]
        rv.append('<div><form method="get" action="generate" style="border-width: 2px; border-style:solid">')
        rv.append('Configuration file:&nbsp;<select name="configuration" id="confselect">')
        for f, cn in _get_list_of_grl_classes():
            cname = os.path.basename(f)
            rv.append('<option value="%(config)s">%(config)s</option>' 
            % {'config': cn})
        rv.append('</select> <a href="javascript:downloadPython()">(Click to download)</a> <br/>\n')
        rv.append('DQ folder:&nbsp;<select name="folder" id="folderselect" onchange="adjustTagList(this)">\n'
        '<option value="LBSUMM" selected>LBSUMM: Full DQ summary (default, recommended)</option>\n'
        '<option value="SHIFTOFL">SHIFTOFL: Offline shifters (experts)</option>\n'
        '<option value="DQCALCOFL">DQCALCOFL: Automatic checks + online shifters (experts)</option>\n'
        '</select> If in doubt, leave as LBSUMM<br/>'
        )
        rv.append('COOL tag:&nbsp;<select name="cooltag" id ="cooltagselect">')
        for folder in ('LBSUMM', 'SHIFTOFL', 'DQCALCOFL'):
            for t, descr, lock in _get_cool_tags_for_folder(folder):
                if t in TAGS_TO_SUPPRESS: continue
                apptext = []
                if descr:
                    apptext.append('(%s)' % descr)
                if lock == 'unlocked':
                    apptext.append('Tag unlocked, all runs may change')
                elif lock == 'partial':
                    apptext.append('Tag partially locked, recent runs may change')
                else:
                    apptext.append('Tag locked')
                if apptext != []:
                    apptext.insert(0, '&mdash;')
                rv.append('<option value="%(tag)s">%(tag)s %(apptext)s</option>' 
                         % {'tag': t, 'apptext': ' '.join(apptext)})
        
        rv.append(
        '<option value="">HEAD</option>'
        '</select><br/>')
        rv.append(
        '<input type="submit" value="Submit"/>\n'
        '</form></div>')
        
        rv.append('</body></html>') 
            
            
        return '\n'.join(rv)
    
    @cherrypy.expose
    def generate(self, configuration=None, folder='LBSUMM', cooltag='HEAD'):
        if configuration is None:
            return ''
        with self.genlock as lock:
            q = multiprocessing.Queue()
            proc = multiprocessing.Process(target=_get_grl_info, 
                args = [os.path.join(GRL_DIRECTORY, configuration),
                cooltag],
                kwargs = {'queue': q, 'folder': folder})
            proc.start()
            rv = q.get()
            proc.join()
            cherrypy.response.headers['content-type'] = 'text/xml'
            cherrypy.response.headers['content-disposition'] = 'attachment; filename=%s' % rv[1]
            return rv[0]
