# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"run validation against a DTD. The DTD location is written into the xml file."

import os.path, sys
from xml.parsers.xmlproc      import xmlproc, xmlval, xmldtd
from xml.dom.minidom          import parse
from Evaluate                 import Evaluate
from RTTSException            import RTTInputError
from RTTSException            import RTTCodingError
from exc2string2              import exc2string2
# -------------------------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------


# ====================================================================================================
# ====================================================================================================
# ====================================================================================================

class Validator:
    __alternativeDTDs = {'RTT.dtd': ['http://cern.ch/atlasrtt/alternateDTD/RTT.dtd'],
                         'unifiedTestConfiguration.dtd': ['http://cern.ch/atlasrtt/alternateDTD/unifiedTestConfiguration.dtd'],
                         'vetoFile.dtd': ['http://cern.ch/atlasrtt/alternateDTD/vetoFile.dtd']}

    def __init__(self, pathToXMLfile):
        self.file = pathToXMLfile

        try:
            self._dom = parse(self.file)
            self._dom.normalize()
        except Exception, e:
            m = 'Exception raised while parsing user input file:\n%s' % str(e)
            raise RTTInputError(m)

        self.parseOK  = True
        
        
    def validate(self):
        if not self.parseOK:
            raise RTTInputError('User xml file failed parse')

        try:
            self._validate()
        except Exception, e:
            msg = 'Exception of type %s raised while validating xml file\nException:\n%s\nTraceback:\n%s' % (e.__class__.__name__, str(e), exc2string2())
            raise RTTInputError(msg)

        try:
            self.ensureUniqueTrendID()
        except Exception, e:
            msg = 'Exception of type %s raised while emsuring unique trend id\nException:\n%s\nTraceback:\n%s' % (e.__class__.__name__, str(e), exc2string2())
            raise RTTInputError(msg)
        
        logger.debug('%s is valid against the DTD.' % self.file)
    
        
    def grabDTD(self, dtdPaths):
        # Test if we can get DTD, if so read it
        import socket
        import urllib
        def grab(dtdPath):
            socket.setdefaulttimeout(15) # prevents unreachable URLs from hanging the request
            conts = ''
            try:
                a = urllib.urlopen(dtdPath)
                conts = a.read()
                a.close()
                socket.setdefaulttimeout(None)
                return conts
            except IOError, e:
                socket.setdefaulttimeout(None)        
                m = 'Inaccessible DTD! [%s]' % dtdPath
                if not 'timed out' in str(e):
                    m  = 'Inaccessible DTD [==> %s]: %s\n\n' % (str(e), dtdPath)
                logger.error(m)
                return conts
            
        for d in dtdPaths:
            c = grab(d)
            if c: return c
        return ''

    def ensureUniqueTrendID(self):
        # new step: no XML file can have non-unique CoolDozer trend Id values.
        docEl  = self._dom.documentElement
        ath = [thing for thing in [el.getAttribute('trendId') for el in Evaluate('rtt/jobList/athena', docEl)] if thing]
        trf = [thing for thing in [el.getAttribute('trendId') for el in Evaluate('rtt/jobList/jobTransform', docEl)] if thing]
        dup = {}
        [dup.setdefault(t, []).append(t) for t in ath]
        [dup.setdefault(t, []).append(t) for t in trf]
        dups = [(k,v) for k,v in dup.items() if len(v)>1]
        if dups:
            m  = 'InavlidXMLFile: \n%s is __NOT__ valid XML!\n' % os.path.basename(self.file)
            m += 'It declares non-unique trendId attributes.\n'
            for k,v in dups:
                m += '%s: declared by %d jobs\n' % (k, len(v))
            raise RTTInputError(m)

    def _validate(self):
        
        xmlFileDTD = self._dom.doctype.systemId # get system DTD path from XML file (if it exists)
        if not xmlFileDTD:
            logger.info('No external DTD detected in XML file. Considering %s as valid.' % os.path.basename(self.file))
            return True
        
        dtds = [xmlFileDTD]
        dtds.extend(Validator.__alternativeDTDs.get(os.path.basename(xmlFileDTD), []))

        dtdContent = self.grabDTD(dtds)
        if not dtdContent:
            m = 'Invalid XML File: Unable to access any version of the DTD %s. Considering XML file [%s] as invalid.' % (os.path.basename(xmlFileDTD),
                                                                                                                         os.path.basename(self.file))
            raise RTTInputError(m)

        # now load this string dtd
        dtd = xmldtd.load_dtd_string(dtdContent)
        parser = xmlproc.XMLProcessor()
        parser.set_application(xmlval.ValidatingApp(dtd, parser))
        parser.dtd = dtd
        parser.ent = dtd
        
        try:
            parser.parse_resource(self.file)
        except Exception, e:
            m  = 'Invalid XML File: %s is __NOT__ valid against the DTD.' % self.file
            m += 'Cause: Either this is not a valid DTD, or the XML file is genuinely invalid against the DTD'
            raise RTTInputError(m)


# ====================================================================================================
# ====================================================================================================
# ====================================================================================================

if __name__ == '__main__':
    import getopt, os, sys

    try:
        opts,args = getopt.getopt(sys.argv[1:],'')
    except getopt.GetoptError:
        print '1 usage validateXMLFile fn'
        sys.exit(0)

    if len(args) != 1:
        print 'Usage: python validateXMLFile.py /path/to/xml_file'
        sys.exit(0)
        
    file = args[0]
    if not os.path.exists(file):
        print 'unknown file', file
        sys.exit(0)
