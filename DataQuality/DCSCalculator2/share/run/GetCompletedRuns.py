
import xmlrpclib

from optparse import OptionParser

parser = OptionParser()
(options, args) = parser.parse_args()

lastRun = args[0]

s=xmlrpclib.Server('http://atlasdqm.cern.ch:8080')
runList = s.get_completed_runs({'low_run':lastRun})

print runList
