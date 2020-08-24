# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.jira
# @purpose Interface with CERN JIRA instance
# @author Edward Moyse
# @date July 2016

from __future__ import print_function

__doc__ = "Interface with CERN JIRA instance."
__author__ = "Edward Moyse"

# Tried to use jira module, but apparently doesn't support cookie-based authenticatoon.

# pip install --user requests
import requests
import PyUtils.acmdlib as acmdlib

### functions -----------------------------------------------------------------

def queryJira(querystring, cookies):
    # Now, let's make this request!
    base_url = "https://its.cern.ch/jira/rest/api/2/"
    url = base_url+querystring

    r = requests.get(url, cookies=cookies)
    if r.status_code != 200:
        if r.status_code == 401:
            print ('Authorisation has failed. Have you got a valid SSO cookie? If not, re-run "cern-get-sso-cookie -u https://its.cern.ch/jira/loginCern.jsp -o jira.txt" ')
        else:
            print ("Something has gone wrong! The response is: ", r.text)
        return

    import json
    response = json.loads(r.text)
    return response

@acmdlib.command(name='jira.issues')
@acmdlib.argument('-c', '--cookies',
                  required=False,
                  default = 'jira.txt',
                  help='the CERN SSO cookies file')
@acmdlib.argument('-r', '--release',
                  required=False,
                  help='Limit the response to the specified release')
@acmdlib.argument('-m', '--myissues',
                  action='store_true',
                  default=True,
                  required=False,
                  help='Limit the response your own issues')
@acmdlib.argument('-q', '--query',
                  required=False,
                  help='Use the specified query string i.e. "search?jql=assignee=currentUser()+and+resolution=Unresolved"')
@acmdlib.argument('-s', '--status',
                  required=False,
                  help='Limit the search to issues with this status, e.g. "Open"')
                  
def main(args):
    """Interface to the CERN JIRA instance"""

    #authentication
    try: 
        cookiesFile = open(args.cookies, 'r')
        cookies = {}
        for line in cookiesFile:
            text = line.split()
            if 'JSESSIONID' in line:
                cookies['JSESSIONID'] = text[-1]
            if 'atlassian.xsrf.token' in line:
                cookies['atlassian.xsrf.token'] = text[-1]   
    except Exception:
         print ("Problems opening cookie file at ", args.cookies)
         return 1
    
    querystring = ""
    if (args.query):
        print ("Will use the following search string: ",args.query)
        querystring = args.query
    else:
        querystring = "search?jql=resolution=Unresolved"
        if (args.myissues):
            querystring += "+AND+assignee=currentUser()"
        if (args.release):
            querystring += "+AND+affectedVersion="+args.release
        if (args.status):
            querystring += "+AND+status="+args.status
    
    response = queryJira(querystring, cookies)
    if not response:
        return 1
    
    # Now lets get some information out
    issues = response['issues']
    print()
    print ('Key'.ljust(20), 'Summary'.ljust(20))
    print ('---'.ljust(20), '-------'.ljust(20))
    for issue in issues:
        print (issue['key'].ljust(20), issue['fields']['summary'].ljust(20))
    return


    
