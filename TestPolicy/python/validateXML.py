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

import os
import re
import sys
import string
import traceback
import StringIO
import urllib2 as URL
import socket
from collections import defaultdict
from functools import partial

from xml.parsers.xmlproc import xmlproc, xmlval, xmldtd
from xml.dom.minidom import parse


altDTDs = [('https://atlas-rtt.cern.ch/prod/'
            'alternateDTD/unifiedTestConfiguration.dtd')]


# -------------------------------------------------------------------
# --- LOGGING ---
# -------------------------------------------------------------------
def log(level, msg):
    if msg.strip():
        print('[{0}] {1}'.format(level.upper(), msg))


def devnull(msg):
    pass


error = partial(log, 'ERROR')
warning = partial(log, 'WARNING')
info = partial(log, 'INFO')
debug = devnull

clargs = sys.argv[1:]
if '--verbose' in clargs:
    debug = partial(log, 'DEBUG')

# -------------------------------------------------------------------
# --- HELPERS ---
# -------------------------------------------------------------------


def parseXML(xmlfile):
    """Return None if @xmlfile is unparseable,
    else the dom representation."""
    try:
        return parse(xmlfile)
    except Exception, inst:
        msg = 'Unparseable:\n{0}'.format(str(inst))
    except:
        tb, tbType, stacktrace = sys.exc_info()
        pt = traceback.format_tb(stacktrace)
        msg = 'Unparseable:\n{0}\n{1}\n{2}'.format(tb, tbType, pt)

    error(msg)


def getTag(el, tagname):
    """If available, return the first element @tagname
    below parent @el."""
    tags = el.getElementsByTagName(tagname)
    return tags[0] if tags else None


def xmlPathExists(el, path):
    """Does the xml @path element exist below parent @el?"""
    current = el
    for tag in path.split('/'):
        current = getTag(current, tag)
        if not current:
            break
    else:
        return True
    return False


def isValidAgainstDTD(xmlfile, dtdContent):
    """Validate the XML file at @xmfile against @dtdContent."""
    if not dtdContent:
        return False

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
        parser.parse_resource(xmlfile)
    except Exception, e:
        newOut, sys.stdout = swap(newOut, sys.stdout)
        errmsg = '%s\n%s' % (newOut.getvalue().strip(), str(e))
    except:
        newOut, sys.stdout = swap(newOut, sys.stdout)
        errmsg = newOut.getvalue().strip()
    else:
        newOut, sys.stdout = swap(newOut, sys.stdout)

    error(errmsg)
    return not errmsg


def getFirstAvailableDTDContents(*dtdURLs):
    """Return the first available dtd contents from
    the @dtdURLs passed in. If no DTDs exist or are
    reachable, return None."""
    for dtdURL in dtdURLs:
        contents = getDTDContent(dtdURL)
        if contents:
            debug('Validating against {0}'.format(dtdURL))
            break
    else:
        error('No DTDs available, cannot validate')
        contents = dtdURL = None

    return (contents, dtdURL)


def getDTDContent(dtdURL):
    """Fetch the contents of the DTD at @dtdURL, returning
    None if the URL either does not exist or is not available.
    Timeout to prevent unreachable URLs from blocking."""
    with DTD(dtdURL, timeout=1) as dtd:
        if dtd.inexistant:
            warning('{0}: inexistant'.format(dtdURL))
        elif dtd.unreachable:
            warning('{0}: unreachable, timeout'.format(dtdURL))
        return dtd.contents


def getUserJobIds(jobEls):
    return [el.getAttribute('userJobId') for el in jobEls]


def _idOccursMoreThanOnce(jobID, count):
    if count > 1:
        error('JobID {0} occurs {1} times'.format(jobID, count))
    return count > 1


def _idHasIllegalChars(jobID):
    allowed = string.digits + string.letters + '._:-'
    for ch in jobID.lower():
        if ch not in allowed:
            m = 'JobID "{0}" '.format(jobID)
            m += 'contains the illegal character ("{0}"). '.format(ch)
            m += 'Legal chars are letters, digits, '
            m += ':(colon), -(hyphen), .(dot) and _(underscore).'
            error(m)
            return True
    return False


def getJobIds(docEl):
    athenaTags = docEl.getElementsByTagName('athena')
    athenaIds = getUserJobIds(athenaTags)
    transformTags = docEl.getElementsByTagName('jobTransform')
    transformIds = getUserJobIds(transformTags)
    return athenaIds + transformIds


def hasLegalUserJobIds(docEl):
    """Check the package userJobIds.
    These must:
        1. be unique within a package XML file.
        2. match the regex for acceptable names.
    """

    allIDs = getJobIds(docEl)

    # count how many times each job id occurs
    dd = defaultdict(int)
    for jobID in allIDs:
        dd[jobID] += 1

    legal = True
    for jobID, count in dd.items():
        if _idOccursMoreThanOnce(jobID, count) or _idHasIllegalChars(jobID):
            legal = False

    return legal

# -------------------------------------------------------------------


class XMLFile(object):
    def __init__(self, xmlfile):
        self.xmlfile = xmlfile
        self.dtdURL = None
        self.docEl = None
        self.parsedOK = False

    def isValid(self):
        return self.parsedOK and self._do_validate()

    def __enter__(self):
        dom = parseXML(self.xmlfile)
        if dom:
            self.parsedOK = True
            self.docEl = dom.documentElement
            self.checkForDeprecatedTags()
            doctype = dom.doctype
            self.dtdURL = doctype.systemId if doctype else None

        return self

    def __exit__(self, etype, evalue, stacktrace):
        if etype is not None:
            error('XMLFile raised a {0} exception'.format(etype))
            return False  # will cause re-raise

    def checkForDeprecatedTags(self):
        found = False
        for path in ('atn/author', 'atn/mailto'):
            if xmlPathExists(self.docEl, path):
                warning('{0} tag is deprecated'.format(path))
                found = True
        return found

    def _do_validate(self):
        if not self.dtdURL:
            debug('No external DTD reference found, considering file valid.')
            return True

        content, dtdURL = getFirstAvailableDTDContents(self.dtdURL, *altDTDs)
        isValid = isValidAgainstDTD(self.xmlfile, content)
        hasLegalIds = hasLegalUserJobIds(self.docEl)
        return isValid and hasLegalIds

# -------------------------------------------------------------------


class DTD(object):
    def __init__(self, dtdURL, timeout):
        self.url = dtdURL
        self.timeout = timeout  # seconds before we should give up
        self.inexistant = False
        self.unreachable = False
        self.contents = None

    def __enter__(self):
        resource = self._openDTD()
        if resource:
            self.contents = resource.read()
            resource.close()
        return self

    def __exit__(self, etype, evalue, stacktrace):
        if etype is not None:
            error('DTD() raised a {0} exception'.format(etype))
            return False  # will cause re-raise

    def exists(self):
        return not self.inexistant

    def reachable(self):
        return not self.unreachable

    def _openDTD(self):
        # prevents unreachable URLs from hanging the request
        resource = None
        try:
            resource = URL.urlopen(self.url, timeout=self.timeout)
        except URL.HTTPError as e:
            if e.code == 404:
                self.inexistant = True
        except (URL.URLError, socket.timeout) as e:
            self.unreachable = True

        return resource

# -------------------------------------------------------------------


def validate(*xmlfiles):
    if not xmlfiles:
        error('Please provide paths to one or more XML files.')
        sys.exit(1)

    exitcode = 0
    for xmlfile in xmlfiles:
        if not os.path.exists(xmlfile):
            warning('{0}: inexistant'.format(xmlfile))
            continue

        with XMLFile(xmlfile) as xf:
            if xf.isValid():
                txt = 'OK -'
            else:
                txt = 'FAIL -'
                exitcode = 1

            info('{0} {1}'.format(txt, xmlfile))

    return exitcode


def _getargs():
    args = sys.argv[1:]
    try:
        args.remove('--verbose')
    except ValueError:
        pass
    return args


def main():
    args = _getargs()
    sys.exit(validate(*args))


if __name__ == '__main__':
    main()
else:
    # just importing from test suite, switch off logging
    error = devnull
    warning = devnull
    info = devnull
    debug = devnull
