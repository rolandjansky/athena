#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Validate a package unified test configuration XML file.

Check the validity of a package XML file against the
externally referenced DTD. If no reference is found,
the XML file is considered valid. The userJobId
for each athena/transform job is also checked for
uniqueness within the file, and against the naming policy.
If the referenced DTD is unavailable (site down) or
inexistant (404), a list of alternatives is tried.
"""


import os.path
import re
import sys
import traceback
import StringIO
import urllib2 as URL
import socket

from xml.parsers.xmlproc import xmlproc, xmlval, xmldtd
from xml.dom.minidom import parse


def exc2string():
    llist = sys.exc_info()
    errmsg = str(llist[0])
    errmsg += str(llist[1])
    errmsg += ' '.join(traceback.format_tb(llist[2]))
    return errmsg


class Logger(object):
    def __init__(self):
        self.levels = ['info', 'error', 'fatal']
        self.log = []

    def __getattr__(self, n):
        return lambda x: self.log.append((n, x)) if n in self.levels else None

    def __str__(self):
        o = [line for level, line in self.log]
        return '' if not o else '%s\n' % ('\n'.join(o))


class Validator(object):

    dtdname = 'unifiedTestConfiguration.dtd'
    possibleURLS = ['https://atlas-rtt.cern.ch/prod/alternateDTD/']
    alternateDTDs = [os.path.join(pu, dtdname) for pu in possibleURLS]
    __altDTDs = {}
    [__altDTDs.setdefault(dtdname, []).append(aDTD) for aDTD in alternateDTDs]

    def __init__(self, pathToXMLfile):
        self.file = pathToXMLfile
        self.log = Logger()

    def validate(self):
        isValid = self._do_validation()
        self.cleanup()
        return isValid

    def _parseFile(self):
        try:
            self._dom = parse(self.file)
            return True
        except Exception, inst:
            msg = 'Cannot parse:\n%s' % str(inst)
            self.log.fatal(msg)
            return False
        except:
            msg = 'Cannot parse:\n%s' % exc2string()
            self.log.fatal(msg)
            return False

    def _hasExternalDTD(self):
        return self._getDTDFromXMLFile() is not None

    def _do_validation(self):
        if self._parseFile():
            if not self._hasExternalDTD():
                m = 'No external DTD reference found in XML file, '
                m += 'consider file valid.'
                self.log.info(m)
                return True

            dtdContent = self._getDTDContent()
            if dtdContent:
                isXMLValid = self._validate(dtdContent)
                if isXMLValid:
                    if self.ensureLegalUserJobIds():
                        return True
        return False

    def cleanup(self):
        try:
            self._dom.unlink()
        except:
            pass

        try:
            del self._dom
        except:
            pass

    def grabDTD(self, dtdURLs):
        # Test if we can get DTD, if so read it
        nDTDs = len(dtdURLs)
        for i, dtdURL in enumerate(dtdURLs, start=1):
            dtd = DTD(dtdURL)
            contents = dtd.contents
            if contents:
                if i > 1:  # we're using a fallback dtd
                    m = '%s: using fallback DTD' % dtdURL
                    self.log.info(m)
                return contents

            m = dtdURL
            if dtd.inexistant:
                m += ': inexistant'
                self.log.error(m)
            elif dtd.unreachable:
                m += ': unreachable, timeout'
                self.log.error(m)

        self.log.fatal('No DTDs available/reachable, will stop')
        return ''

    def ensureLegalUserJobIds(self):
        """Check the package userJobIds.
        These must:
            1. be unique within a package XML file.
            2. match the regex for acceptable names.
        """

        def getAllUserJobIds(el):
            ids = []
            for job in ['athena', 'jobTransform']:
                els = docEl.getElementsByTagName(job)
                ids.extend([el.getAttribute('userJobId') for el in els])
            return ids

        def reportDuplicates(jids, messages):
            dups = {}
            [dups.setdefault(jid, []).append(jid) for jid in jids]

            msgs = []
            [msgs.append('userJobId "%s" used %d times' % (k, len(v))) for
             k, v in dups.items() if len(v) > 1]
            messages.extend(msgs)
            return not msgs

        def reportIllegal(userJobIds, messages):
            acceptableFileName = r'^[a-zA-Z0-9_\.]+'
            c_acceptableFileName = re.compile(acceptableFileName)

            badIds = [jid for jid in userJobIds if
                      not c_acceptableFileName.search(jid)]

            m = 'userjobID "%s" '
            m += 'does not start with a letter, digit, underscore or dot'
            messages.extend([m % jid for jid in badIds])

            return not badIds

        # Start of the method
        rc = True
        docEl = self._dom.documentElement
        messages = []
        jobids = getAllUserJobIds(docEl)
        rc = rc and reportDuplicates(jobids, messages)
        rc = rc and reportIllegal(jobids, messages)

        if messages:
            messages = '\n'.join(messages)
            self.log.error(messages)

        return bool(rc)

    def _getDTDFromXMLFile(self):
        doctype = self._dom.doctype
        return doctype.systemId if doctype else None

    def _getDTDContent(self):
        """Read DTD into a string. DTD location is given in
        the configuration file. Use this, or backup DTDs if
        unable to use the user-suggested DTD."""

        # get system DTD path from XML file
        xmlFileDTD = self._getDTDFromXMLFile()
        dtdname = os.path.basename(xmlFileDTD)
        dtds = [xmlFileDTD]
        altDTDs = Validator.__altDTDs.get(dtdname, [])
        dtds.extend(altDTDs)
        return self.grabDTD(dtds)  # grab from 1st available

    def _validate(self, dtdContent):
        """Use read-in DTD to validate config file."""

        # now load this string dtd
        dtd = xmldtd.load_dtd_string(dtdContent)
        parser = xmlproc.XMLProcessor()
        parser.set_application(xmlval.ValidatingApp(dtd, parser))
        parser.dtd = dtd
        parser.ent = dtd

        def swap(o1, o2):
            return (o2, o1)

        newOut = StringIO.StringIO()
        newOut, sys.stdout = swap(newOut, sys.stdout)

        errmsg = ''

        try:
            parser.parse_resource(self.file)
        except Exception, e:
            newOut, sys.stdout = swap(newOut, sys.stdout)
            errmsg = '%s\n%s' % (newOut.getvalue().strip(), str(e))
        except:  # Exceptions not deriving from Exception.....
            newOut, sys.stdout = swap(newOut, sys.stdout)
            errmsg = newOut.getvalue().strip()
        else:
            newOut, sys.stdout = swap(newOut, sys.stdout)

        if errmsg:
            self.log.error(errmsg)

        return not errmsg


class DTD:

    TIMEOUT = 15  # give up trying to reach the URL after this many seconds

    def __init__(self, dtdURL):
        self.url = dtdURL
        self.inexistant = False
        self.unreachable = False
        self.contents = None
        resource = self._openDTD()
        if resource:
            self.contents = resource.read()
            resource.close()

    def exists(self):
        return not self.inexistant

    def reachable(self):
        return not self.unreachable

    def _openDTD(self):
        # prevents unreachable URLs from hanging the request
        resource = None
        try:
            resource = URL.urlopen(self.url, timeout=DTD.TIMEOUT)
        except URL.HTTPError as e:
            if e.code == 404:
                self.inexistant = True
        except (URL.URLError, socket.timeout) as e:
            self.unreachable = True

        return resource


if __name__ == '__main__':
    import getopt
    import os

    xmlfiles = sys.argv[1:]
    if not xmlfiles:
        print 'Please provide paths to one or more XML files.'
        sys.exit(1)

    for xmlfile in xmlfiles:
        if not os.path.exists(xmlfile):
            print 'Inexistant XML file: %s' % xmlfile
            continue

        v = Validator(xmlfile)
        isValid = v.validate()
        print '%s[%s] %s' % (str(v.log),
                             'OK' if isValid else 'FAIL',
                             xmlfile)

    sys.exit(0)
