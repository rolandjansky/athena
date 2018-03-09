#!/usr/bin/env python

import json
import sys
import requests

print "running ..."

sessionid = None
with open('cookiefile.txt', 'r') as f:
    for line in f:
        line = line.rstrip('\n')
        if line.find('JSESSIONID') > - 1:
            sessionid = line.split()[-1]

if not sessionid:
    sys.exit()

headers={'cookie': 'JSESSIONID=%s' % (sessionid), 'Content-Type': 'application/json'}

i=0
myrequest=""

for i in xrange(1,len(sys.argv)):
    if sys.argv[i] == "-f" :
       with open (sys.argv[i+1], "r") as myfile:
         myrequest = myfile.read()
       break
    else:   
       myrequest += sys.argv[i]
       myrequest += " "
       print sys.argv[i]


print myrequest


if myrequest == "" :
    print "no message to post"
    exit(-1)

data = {
    "body": myrequest
}

result = requests.post('https://its.cern.ch/jira/rest/api/2/issue/ATLINFR-2021/comment', headers=headers, data=json.dumps(data))

print json.dumps(result.json(), indent=4)

