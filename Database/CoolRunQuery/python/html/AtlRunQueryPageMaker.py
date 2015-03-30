# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import os, re

class PageMaker:

    @classmethod
    def makePage(cls, body, origQuery=None, extraReplace=None, removeExamples=False):
        from CoolRunQuery.AtlRunQueryQueryConfig import QC
        
        (top, bottom) = cls.getBlankPage(removeExamples=removeExamples)
        # put in the original query
        if origQuery != None:
            top = top.replace('name="q"','name="q" value="%s"' % origQuery)

        # extra replacements
        if extraReplace != None:
            for pat,repl in extraReplace:
                p = re.compile(pat,re.S)
                top = p.sub(repl, top)
                bottom = p.sub(repl, bottom)
        
        fh = open("%s/index.html" % QC.datapath, "w")
        print >> fh, top
        print >> fh, body.encode("utf-8")
        print >> fh, bottom
        fh.close()

        cls._copySomeFilesToCache()


    @classmethod
    def getBlankPage(cls, removeExamples=False):
        if '/var/vhost' in os.path.dirname(__file__):
            page = open("%s/index.html" % os.path.dirname(__file__).replace('CoolRunQuery/html',''))
        else:
            page = open("%s/atlas-runquery.html" % os.path.dirname(__file__).replace('python/CoolRunQuery/html','html'))
        s = page.read()
        top, bottom = re.match("(.*)<!--INSERTQUERYRESULT-->(.*)", s, re.S|re.M).group(1,2)
        if removeExamples:
            top = "".join(re.match("(.*)<!--START_EXAMPLES-->.*<!--END_EXAMPLES-->(.*)", top, re.S|re.M).group(1,2))
        return (top,bottom)
    

    @classmethod
    def _copySomeFilesToCache(cls):
        from commands import getoutput
        from CoolRunQuery.AtlRunQueryQueryConfig import QC
        getoutput("cp html/atlas-runquery-lb.css %s" % QC.datapath)
