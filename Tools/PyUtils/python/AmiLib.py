# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file PyUtils.AmiLib
#  @brief a set of functions and helpers to talk to the TagCollector
#  @version $Id$
#  @author Originally Sebastien Binet, substantial changes by Graeme Stewart

__all__ = [
    'Client',
    'PyUtilsAMIException'
    ]

# Not sure what's happening here - some testing that's off by default?
# if 0:
#     import xml.etree.cElementTree as ET
#     import pyAMI.client as PyAmi
#     amiclient = pyAmi.Client('atlas')
# 
#     import PyUtils.xmldict as _x
# 
#     clients_cmd = 'TCListPackageVersionClient  -processingStep=production -project=TagCollector   -groupName=AtlasOffline -releaseName=17.0.1 -fullPackageName=/AtlasTest/AthenaMPTest -repositoryName=AtlasOfflineRepository'.split(' ')
#     
#     rec_cmd = 'TCFormGetDependencyPackageVersionTree -expandedPackageID="*" -expandedTopContainerPackage="*" -groupName="AtlasProduction" -processingStep="production" -project="TagCollector" -releaseName="15.7.0"'.replace('"','').split(' ')
#     res = amiclient.execute(rec_cmd)
#     dd = _x.xml2dict(ET.fromstring(res.output('xml')))
#     dd['AMIMessage']['Result']['tree']
# 
# 
#     # all the leaf packages in AtlasProduction and its dependencies
#     cmd = """
#     TCFormGetDependencyPackageVersionTree -expandedPackageID=* 
#     -expandedTopContainerPackage=* -groupName=AtlasProduction 
#     -processingStep=production -project=TagCollector -releaseName=15.7.0
#     """.replace("\n","").split()
#     res = amiclient.execute(cmd)
#     d = _x.xml2dict(ET.fromstring(res.output('xml')))
# 
#     # only the leaf packages in groupName="AtlasProduction"
#     cmd = """
#     TCFormGetPackageVersionTree -expandedPackageID='*' 
#     -expandedTopContainerPackage='*' -groupName='AtlasProduction' 
#     -processingStep='production' -project='TagCollector' -releaseName='15.7.0'
#     """.replace("\n","").split()
#     res = amiclient.execute(cmd)
#     d = _x.xml2dict(ET.fromstring(res.output('xml')))
# 
# 
# 
#     # all the leaf packages in AtlasCore and its dependencies
#     cmd = """
#     TCFormGetDependencyPackageVersionTree
#     -expandedPackageID=* 
#     -expandedTopContainerPackage=*
#     -groupName=AtlasCore 
#     -processingStep=production
#     -project=TagCollector
#     -releaseName=15.7.0
#     """.replace("\n","").split()
#     res = amiclient.execute(cmd)
#     d = _x.xml2dict(ET.fromstring(res.output('xml')))
# 
#     # ami dataset:
#     cmd = """
#     GetDatasetInfo 
#     -logicalDatasetName=data09_900GeV.00142404.physics_RNDM.merge.AOD.f193_m320
#     """.replace("\n","").split()
#     res = amiclient.execute(cmt)
#     d = _x.xml2dict(ET.fromstring(res.output('xml')))
# 
#     """
#     [amiCommand] 
#     -logicalFileName=data09_1beam.00140536.physics_L1Calo.merge.HIST.f170_m255._0001.1 
# 
#     [amiCommand] GetDatasetInfo 
#     -logicalDatasetName=mc08.105609.Pythia_Zprime_tt2000.merge.AOD.e393_s462_s520_r635_t53 
# 
#     amiCommand=["GetDatasetInfo","logicalDatasetName=adatasetname"]
#     result=amiclient.execute(amiCommand)
# 
# 
#     or
#     amiCommand=["GetDatasetInfo","logicalFileName=aFilename"]
#     result=amiclient.execute(amiCommand)
#     """
#     def dsinfo(n):
#         import PyUtils.AmiLib as A
#         import xml.etree.cElementTree as ET
#         import PyUtils.xmldict as _x
#         c = A.Client()
#         try:
#             res = c.exec_cmd(cmd="GetDatasetInfo", logicalFileName=n)
#             dd = _x.xml2dict(ET.fromstring(res.output('xml')))
#             return dd['AMIMessage']['Result']
#         except PyAmi.AMI_Error:
#             # maybe a logical dataset name ?
#             res = c.exec_cmd(cmd="GetDatasetInfo", logicalDatasetName=n)
#             dd = _x.xml2dict(ET.fromstring(res.output('xml')))
#             return dd['AMIMessage']['Result']
#             
### imports -------------------------------------------------------------------
import json
import os
import pprint
import sys

import pyAMI.client as PyAmi
import pyAMI.exception

from PyUtils.xmldict import xml2dict

### globals -------------------------------------------------------------------

### functions -----------------------------------------------------------------    
def xmlstr_todict(s):
    import PyUtils.xmldict as _x
    import xml.etree.cElementTree as ET
    return  _x.xml2dict(ET.fromstring(s))


## @basic Do some basic checks on an AMI JSON result
#  @return bool, errmsg tuple
def badresult(result, checkForValidRows=True, expectCommandStatus=True):
    try:
        if type(result) is not dict:
            return True, "Result is not a dictionary"
        if expectCommandStatus and 'commandStatus' not in result['AMIMessage'][0]:
            return True, "No results found (command probably didn't execute - do you have valid AMI authentication?)"
        if expectCommandStatus and result['AMIMessage'][0]['commandStatus'][0]['$'] != "successful":
            return True, "Command execution apprears to have failed"
        if checkForValidRows:
            if 'Result' not in result['AMIMessage'][0]:
                return True, "No result found in query"            
            if len(result['AMIMessage'][0]['Result']) == 0:
                return True, "No results found from query"
            if 'rowset' not in result['AMIMessage'][0]['Result'][0]:
                return True, "No rowsets found from query"
            if 'row' not in result['AMIMessage'][0]['Result'][0]['rowset'][0]:
                return True, "No rows found in rowset"
    except Exception, e:
        print >>sys.stderr, "Unexpected exception when querying result '{0}': {1}".format(pprint.pformat(result), e)
        return True, "This is Kari, werid things are happening and I don't know what to do"
    return False, ""

## @brief Convert the AMI result rows into a list of dictionaries
#  @param take_rowsets List of rowsets to use: 
#    None=use all
#    list of strings=take rowset if type matches the string
#  @note The rowset type is added to each element dictionary
def amijsontodict(result, take_rowsets=None):
    take_rowset_indexes = []
    if take_rowsets == None:
        take_rowset_indexes = range(len((result['AMIMessage'][0]['Result'][0]['rowset'])))
    else:
        for idx, rowset in enumerate(result['AMIMessage'][0]['Result'][0]['rowset']):
            for rowset_id in take_rowsets:
                if rowset_id == rowset['@type']:
                    take_rowset_indexes.append(idx)

    answer = []
    for rowset_index in take_rowset_indexes:
        for row in result['AMIMessage'][0]['Result'][0]['rowset'][rowset_index]['row']:
            answer_dict = {'rowset': result['AMIMessage'][0]['Result'][0]['rowset'][rowset_index]['@type']}
            for element in row['field']:
                if '$' in element and '@name' in element:
                    answer_dict[element['@name']] = element['$']
            answer.append(answer_dict)
    return answer

## @brief Iterator over a 'tree' AMI result set
#  @param tree Root of AMI tree dictionary
def tree_iter(tree):
    for d in tree:        
        y = {}
        for k,v in d.iteritems():
            if k=='treeBranch': 
                for sub in tree_iter(v):
                    yield sub
            else:
                y[k] = v
        yield y

### classes -------------------------------------------------------------------

class PyUtilsAMIException(Exception):
    pass

class Client(object):
    def __init__(self, certAuth=True, dryrun=False):
        self._client = PyAmi.Client('atlas')
        import PyUtils.Logging as L
        self.msg = L.logging.getLogger('ami-client')
        if 'PYUTILS_DEBUG' in os.environ:
            self.msg.setLevel(L.logging.DEBUG)
        else:
            self.msg.setLevel(L.logging.INFO)
        self.dryrun = dryrun
        return

    ## @brief Accept an AMI command with a set of arguments given as a dictionary
    #  execute it and return the JSON result
    #  @note No longer accept arbirtary keyword:value pairs
    def exec_cmd(self, cmd, args={}, defaults=True, dryrun = None):
        if defaults:
            args.setdefault('-project', 'TagCollector')
            args.setdefault('-processingStep', 'production')
            args.setdefault('-repositoryName', 'AtlasOfflineRepository')

        # transform into an AMI command string
        ami_cmd = map(
            lambda a,b: "%s=%s" % (a,b,),
            args.keys(),
            args.values()
            )
        ami_cmd.insert(0, cmd)

        if dryrun is not None:
            my_dryrun = dryrun
        else:
            my_dryrun = self.dryrun

        self.msg.debug('ami_cmd: {0}'.format(ami_cmd))
        self.msg.debug('Dry run setting is {0}'.format(my_dryrun))
        if my_dryrun:
            self.msg.info('Dry run detected - actual AMI command execution is suppressed')
            return True

        result = self._client.execute(ami_cmd, format = 'json')
        self.msg.debug(pprint.pformat(result))
        return json.loads(result)

    ## @brief Find the full path name of a package. 
    #  @return list of dictionaries with all AMI information by key:value
    def find_pkg(self, pkg, check_tag=True):
        self.msg.debug("Finding package {0}".format(pkg))
        
        # if '-' in name, a tag was given.
        if '-' in pkg:
            tag = pkg.split('/')[-1]
            pkg = pkg.split('-',1)[0]
        elif check_tag:
            raise PyUtilsAMIException('No tag was given for {0}'.format(pkg))
        else:
            tag = None

        orig_pkg = pkg
        # check if we need to find the full pkg path
        if '/' in pkg:
            pkg = pkg.split('/')[-1]

        args = {
            '-glite': ('"select packages.path,packages.packageName,packages.archive'
                       ' where repositories.repositoryName=\'AtlasOfflineRepository\''
                       ' and packages.packageName=\'{0}\' and packages.archive=0"'.format(pkg)
                ),
            }

        result = self.exec_cmd(cmd='SearchQuery', args=args)
        self.msg.debug(pprint.pformat(result))

        bad, msg = badresult(result)
        if bad:
            errmsg = 'Could not resolve [{0}] to full package path: {1}'.format(pkg, msg)
            self.msg.error(errmsg)
            raise PyUtilsAMIException(errmsg)
        
        pkg_list=amijsontodict(result)

        idx = 0
        if len(pkg_list) == 0:
            raise PyUtilsAMIException('package [{0}] does not exist'.format(pkg))

        elif len(pkg_list)>1:
            # Multiple matches - try and use some resolution,
            # like searching the path+name
            # (Somehow this all stinks - there must be a way to get more reliable
            #  information back from AMI with a better query)
            ambiguous = True
            if '/' in orig_pkg:
                pkg_candidates = []
                for i, pkg in enumerate(pkg_list):
                    if orig_pkg in pkg:
                        pkg_candidates.append(i)
                if len(pkg_candidates) == 1:
                    idx = pkg_candidates[0]
                    pkg = pkg_list[idx]
                    ambiguous = False
                pass

            if ambiguous:
                self.msg.info('multiple packages found for [%s]:', pkg)
                for i,v in enumerate(pkg_list):
                    self.msg.info(' %i) %s', i, v['path']+v['packageName'])
                raise PyUtilsAMIException('multiple packages found for [{0}]'.format(pkg))

        else:
            idx = 0
            pkg = pkg_list[0]

        # normalize...
        if pkg['path'][0] != '/':
            pkg['path'] = '/%s' % pkg['path']

        pkg.update(dict(packagePath=pkg['path'],
                        id=str(pkg['AMIELEMENTID'])))
        if tag:
            pkg['packageTag'] = tag
        del pkg['path']
        del pkg['AMIELEMENTID']
        
        # remove all unicode strings...
        _pkg = dict(pkg)
        pkg = {}
        
        for k,v in _pkg.iteritems():
            if isinstance(v, basestring):
                v = str(v)
            pkg[str(k)] = v

        return pkg

    ## @brief retrieve the tag collector information for a given release and package
    #  optional arguments control the type of information returned
    def get_pkg_info(self, package, release, resultKey="groupName", filterRelease=False):

        pkg = self.find_pkg(package, check_tag=False)
        
        full_pkg_name = pkg['packagePath']+pkg['packageName']
        result = self.exec_cmd(cmd='TCGetPackageVersionHistory',
                               args={'-fullPackageName': '"' + full_pkg_name + '"',
                                     '-releaseName': release})
        
        bad, msg = badresult(result) 
        if bad:
            errmsg = "Bad AMI result for projects of package {0}: {1}".format(full_pkg_name, msg)
            self.msg.error(errmsg)
        raw_result_list = amijsontodict(result,['Package_version_history'])
        self.msg.debug(pprint.pformat(raw_result_list))
        
        # Use a set to remove duplicated results in the key we're interested in
        results = set()
        for res in raw_result_list:
            if filterRelease:
                if res.get("releaseName") != release:
                    continue
            if resultKey in res:
                results.add(res[resultKey])

        return list(results)

    ## @brief retrieve the package version from AMI taking into account project dependencies
    def get_version_of_pkg_with_deps(self, pkg, project, release):        
        versions = []
        result = self.exec_cmd(cmd='TCSearchPackageVersion', 
                            args = {
                            '-keyword': pkg,
                            '-groupName': project,
                            '-withDep': "True",
                            '-releaseName': release}
                            )
        self.msg.debug(pprint.pformat(result))

        bad, msg = badresult(result, expectCommandStatus=False)
        if bad:
            errmsg = "Failed to find package {0} in release {1}, project {2}: {3}".format(pkg, release, project, msg)
            raise PyUtilsAMIException(errmsg)

        results_list = amijsontodict(result)
        for res in results_list:
            versions.append((res.get('groupName'), res.get('releaseName'), res.get('fullPackageName'), res.get('packageTag')))
        
        # If more than one result, match full package name
        self.msg.debug(pprint.pformat(versions))
        if len(versions)>1:
            pkg = self.find_pkg(pkg, check_tag=False)
            full_pkg_name = pkg['packagePath']+pkg['packageName']
            self.msg.debug(pprint.pformat(full_pkg_name))
            versions = [ v for v in versions if v[2] == full_pkg_name ]

        if len(versions)==0:
            errmsg = "No version found for package {0} in release {1}".format(pkg, release)
            raise PyUtilsAMIException(errmsg)
            
        return versions


    def get_project_tree(self, project, release, recursive=False):
        """return the dependency tree of packages for a given project
        and a given release
        if ``recursive`` it will also visit the project dependencies of
        ``project``
        """
        cmd = 'TCFormGetPackageVersionTree'
        if recursive:
            cmd = 'TCFormGetDependencyPackageVersionTree'
        result = self.exec_cmd(cmd=cmd,
                               args = {'-expandedPackageID': '*',
                                       '-expandedTopContainerPackage': '*',
                                       '-groupName': project,
                                       '-processingStep': 'production',
                                       '-project': 'TagCollector',
                                       '-releaseName': release,
                                       },
                               defaults=False
                               )
        
        bad, msg = badresult(result) 
        if bad:
            errmsg = "Bad AMI result for project {0} in release {1}: {2}".format(project, release, msg)
            self.msg.error(errmsg)
            raise PyUtilsAMIException(errmsg)
            
        result_list = amijsontodict(result)
        self.msg.debug(pprint.pformat(result_list))

        # Results here seem to be out of kilter with what the following code
        # was trying to parse. This is also true in pyAMI4, so probably this is
        # a dead function...

#         out = d
#         abs_path = ('AMIMessage', 'Result', 'tree', 'treeBranch',)
#         for i,k in enumerate(abs_path):
#             if not k in out:
#                 raise RuntimeError(
#                     'malformated answer from AMI (no [%s] key)' % k
#                     )
#             out = out[k]

        return False

    def get_open_releases(self, project):
        return self.get_releases(project, lambda x : x!='terminated')
        
    def get_releases(self, project, relStatusCond=lambda x : True):        
        """return the list of releases for a given ``project``"""

        result = self.exec_cmd(cmd='TCFormGetReleaseTreeDevView', 
                               args={'-groupName' : project,
                                     '-expandedRelease' : '*',
                               })
        
        bad, msg = badresult(result) 
        if bad:
            errmsg = "Got bad result back from AMI for {0} releases: {1}".format(project, msg)
            self.msg.error(errmsg)
            raise PyUtilsAMIException(errmsg)

        releases = []
        for r in tree_iter(result['AMIMessage'][0]['Result'][0]['tree']):
            if not '@releaseName' in r: continue
            if relStatusCond(r['@status']): 
                releases.append(r['@releaseName'])

        releases.sort(key=lambda x: [int(y) if y.isdigit() else 0 for y in x.split('.')])
        self.msg.debug(pprint.pformat(releases))
        return releases


    def get_clients(self, project, release, full_pkg_name):
        """return the list of clients (full-pkg-name, version) of
        `full_pkg_name` for project `project` and release `release`
        
        Currently this query is broken - doesn't work in pyAMI4 either
        """
        args = {
            '-groupName': project, # AtlasOffline, AtlasEvent, ...
            '-releaseName': release,
            }
        if full_pkg_name[0] != "/":
            full_pkg_name = "/"+full_pkg_name
        args['fullPackageName'] = '"'+full_pkg_name+'"'
        
        result = self.exec_cmd(cmd="TCListPackageVersionClient", args=args)
        if badresult(result):
            self.msg.error("Got bad result back from AMI for clients of {0} in {1} and {2}".format(full_pkg_name, project, release))
            
        result_list = amijsontodict(result)
        
        return None
        
