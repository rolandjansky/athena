# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.AmiLib
# @purpose a set of functions and helpers to talk to the TagCollector

__version__ = "$Revision: 538932 $"
__author__ = "Sebastien Binet"
__doc__ = "a set of functions and helpers to talk to AMI and the TagCollector"

__all__ = [
    'ami_todict',
    'Client',
    ]

if 0:
    import xml.etree.cElementTree as ET
    from pyAMI.pyAMI import *
    amiclient = AMI(certAuth = True)

    import PyUtils.xmldict as _x

    clients_cmd = 'TCListPackageVersionClient  -processingStep=production -project=TagCollector   -groupName=AtlasOffline -releaseName=17.0.1 -fullPackageName=/AtlasTest/AthenaMPTest -repositoryName=AtlasOfflineRepository'.split(' ')
    
    rec_cmd = 'TCFormGetDependencyPackageVersionTree -expandedPackageID="*" -expandedTopContainerPackage="*" -groupName="AtlasProduction" -processingStep="production" -project="TagCollector" -releaseName="15.7.0"'.replace('"','').split(' ')
    res = amiclient.execute(rec_cmd)
    dd = _x.xml2dict(ET.fromstring(res.output('xml')))
    dd['AMIMessage']['Result']['tree']


    # all the leaf packages in AtlasProduction and its dependencies
    cmd = """
    TCFormGetDependencyPackageVersionTree -expandedPackageID=* 
    -expandedTopContainerPackage=* -groupName=AtlasProduction 
    -processingStep=production -project=TagCollector -releaseName=15.7.0
    """.replace("\n","").split()
    res = amiclient.execute(cmd)
    d = _x.xml2dict(ET.fromstring(res.output('xml')))

    # only the leaf packages in groupName="AtlasProduction"
    cmd = """
    TCFormGetPackageVersionTree -expandedPackageID='*' 
    -expandedTopContainerPackage='*' -groupName='AtlasProduction' 
    -processingStep='production' -project='TagCollector' -releaseName='15.7.0'
    """.replace("\n","").split()
    res = amiclient.execute(cmd)
    d = _x.xml2dict(ET.fromstring(res.output('xml')))



    # all the leaf packages in AtlasCore and its dependencies
    cmd = """
    TCFormGetDependencyPackageVersionTree
    -expandedPackageID=* 
    -expandedTopContainerPackage=*
    -groupName=AtlasCore 
    -processingStep=production
    -project=TagCollector
    -releaseName=15.7.0
    """.replace("\n","").split()
    res = amiclient.execute(cmd)
    d = _x.xml2dict(ET.fromstring(res.output('xml')))

    # ami dataset:
    cmd = """
    GetDatasetInfo 
    -logicalDatasetName=data09_900GeV.00142404.physics_RNDM.merge.AOD.f193_m320
    """.replace("\n","").split()
    res = amiclient.execute(cmt)
    d = _x.xml2dict(ET.fromstring(res.output('xml')))

    """
    [amiCommand] 
    -logicalFileName=data09_1beam.00140536.physics_L1Calo.merge.HIST.f170_m255._0001.1 

    [amiCommand] GetDatasetInfo 
    -logicalDatasetName=mc08.105609.Pythia_Zprime_tt2000.merge.AOD.e393_s462_s520_r635_t53 

    amiCommand=["GetDatasetInfo","logicalDatasetName=adatasetname"]
    result=amiclient.execute(amiCommand)


    or
    amiCommand=["GetDatasetInfo","logicalFileName=aFilename"]
    result=amiclient.execute(amiCommand)
    """
    def dsinfo(n):
        import PyUtils.AmiLib as A
        import xml.etree.cElementTree as ET
        import PyUtils.xmldict as _x
        c = A.Client()
        try:
            res = c.exec_cmd(cmd="GetDatasetInfo", logicalFileName=n)
            dd = _x.xml2dict(ET.fromstring(res.output('xml')))
            return dd['AMIMessage']['Result']
        except PyAmi.AMI_Error:
            # maybe a logical dataset name ?
            res = c.exec_cmd(cmd="GetDatasetInfo", logicalDatasetName=n)
            dd = _x.xml2dict(ET.fromstring(res.output('xml')))
            return dd['AMIMessage']['Result']
            
### imports -------------------------------------------------------------------
import os
import sys

import pyAMI.client as PyAmi
import pyAMI.auth as PyAmiAuth

from PyUtils.xmldict import xml2dict

### globals -------------------------------------------------------------------

### functions -----------------------------------------------------------------
def ami_todict(res):
    return res.to_dict()
    
def xmlstr_todict(s):
    import PyUtils.xmldict as _x
    import xml.etree.cElementTree as ET
    return  _x.xml2dict(ET.fromstring(s))

### classes -------------------------------------------------------------------
class Client(object):

    _instance = None
    
    @property
    @staticmethod
    def instance(self):
        if Client._instance is None:
            c = PyAmi.AMI()
            import os.path as osp
            if not osp.exists(PyAmiAuth.AMI_CONFIG):
                PyAmiAuth.create_auth_config()
                pass
            c.read_config(PyAmiAuth.AMI_CONFIG)
            Client._instance = c
        return Client._instance

    def __init__(self, certAuth=True, dry_run=False):
        self._client = PyAmi.AMI()
        import os.path as osp
        if not osp.exists(PyAmiAuth.AMI_CONFIG):
            PyAmiAuth.create_auth_config()
            pass
        self._client.read_config(PyAmiAuth.AMI_CONFIG)
        import PyUtils.Logging as L
        self.msg = L.logging.getLogger('ami-client')
        self.msg.setLevel(L.logging.INFO)
        self.dry_run = dry_run
        return

    def exec_cmd(self, cmd, **args):
        """execute an AMI command"""
        if 'args' in args and len(args)==1:
            args = args['args']
        # add some defaults
        args.setdefault('project', 'TagCollector')
        args.setdefault('processingStep', 'production')
        args.setdefault('repositoryName', 'AtlasOfflineRepository')

        # transform into an AMI command string
        ami_cmd = map(
            lambda a,b: "%s=%s" % (a,b,),
            args.keys(),
            args.values()
            )
        ami_cmd.insert(0, cmd)

        self.msg.debug('ami_cmd: %s', ami_cmd)
        if self.dry_run:
            return True

        # execute
        ## try:
        ##     result = self._client.execute(ami_cmd)
        ##     return result
        ## except Exception, err:
        ##     if self.reraise:
        ##         raise
        ##     self.msg.error('caught an exception:\n%s', err)
        ##     return
        return self._client.execute(ami_cmd)
    
    def find_pkg(self, pkg, check_tag=True, cbk_fct=None):
        """Find the full path name of a package.
        @return (pkg,tag) tuple
        """
        
        # if '-' in name, a tag was given.
        if '-' in pkg:
            tag = pkg.split('/')[-1]
            pkg = pkg.split('-',1)[0]
        elif check_tag:
            raise ValueError('no tag was given for [%s]' % (pkg,))
        else:
            tag = None

        orig_pkg = pkg
        # check if we need to find the full pkg path
        if '/' in pkg:
            pkg = pkg.split('/')[-1]

        args = {
            'glite': (
                "select packages.path,packages.packageName,packages.archive "
                "where repositories.repositoryName='AtlasOfflineRepository' "
                "and packages.packageName='%s' and packages.archive=0" % pkg
                ),
            }

        result = self.exec_cmd(cmd='SearchQuery', args=args)
        if not result:
            raise RuntimeError(
                'could not resolve [%s] to full package path' %
                (pkg,)
                )
        res_dict = result.to_dict()
        if not 'Element_Info' in res_dict:
            raise RuntimeError(
                'could not resolve [%s] to full package path' %
                (pkg,)
                )
            
        pkg_list = []
        for v in res_dict['Element_Info'].values():
            pkg_list.append(v) # += [v['path'] + v['packageName']]

        idx = 0
        if len(pkg_list) == 0:
            raise RuntimeError('package [%s] does not exist' % pkg)

        elif len(pkg_list)>1:
            ambiguous = True
            if '/' in orig_pkg:
                pkg_candidates = []
                for i,v in enumerate(pkg_list):
                    if orig_pkg in v['path']+v['packageName']:
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
                if cbk_fct:
                    try:
                        n = cbk_fct()
                    except StopIteration:
                        raise RuntimeError(
                            'multiple packages found for [%s]' % pkg
                            )
                    idx = n
                    pkg = pkg_list[n]
                else:
                    raise RuntimeError('multiple packages found for [%s]' % pkg)

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
        
        
        ## if tag is None:
        ##     tag = tag_list[idx]
            
        ## print "-"*80
        ## print res_dict
        ## print "-"*80
        return pkg

    def get_project_of_pkg(self, pkg, release):
        """
        retrieve the list of projects from AMI for a given release and package
        """
        pkg = self.find_pkg(pkg,check_tag=False)
        
        projects = []
        full_pkg_name = pkg['packagePath']+pkg['packageName'] # pkg['packageTag']
        try:
            res = self.exec_cmd(cmd='TCGetPackageVersionHistory',
                                fullPackageName=full_pkg_name,
                                releaseName=release)
            rows = res.rows()
            if isinstance(rows, dict):
                rows = [rows]
            # print "---"
            # print list(rows)
            # print "---"
            for row in rows:
                projects.append(row.get('groupName'))
            if not projects:
                self.msg.error(
                    "no project found for package [%s] and release [%s]",
                    full_pkg_name,
                    release)
        except PyAmi.AMI_Error, err:
            pass
        return projects

    def get_version_of_pkg(self, pkg, release):
        """
        retrieve the list of versions from AMI for a given release and package
        """
        pkg = self.find_pkg(pkg,check_tag=False)
        
        versions = []
        full_pkg_name = pkg['packagePath']+pkg['packageName'] # pkg['packageTag']
        try:
            res = self.exec_cmd(cmd='TCGetPackageVersionHistory',
                                fullPackageName=full_pkg_name,
                                releaseName=release)
            rows = res.rows()
            if isinstance(rows, dict):
                rows = [rows]
            ## print "---"
            ## print list(rows)
            ## print "---"
            for row in rows:
                versions.append(row.get('packageTag'))
            if not versions:
                self.msg.error(
                    "no version found for package [%s] and release [%s]",
                    full_pkg_name,
                    release)
        except PyAmi.AMI_Error, err:
            pass
        return versions


    def get_version_of_pkg_with_deps(self, pkg, project, release):
        """
        retrieve the package version from AMI taken into account project dependencies
        """
        
        versions = []
        try:
            res = self.exec_cmd(cmd='TCSearchPackageVersion',
                                keyword=pkg,
                                groupName=project,
                                withDep=True,
                                releaseName=release)
            rows = res.rows()
            if isinstance(rows, dict):
                rows = [rows]

            for row in rows:
                packageTag = row.get('packageTag', None)
                fullPackageName = row.get('fullPackageName', None)
                groupName = row.get('groupName', None)
                releaseName = row.get('releaseName', None)
                versions.append((groupName,releaseName,fullPackageName,packageTag))
                
            # If more than one result, match full package name
            if len(versions)>1:
                pkg = self.find_pkg(pkg, check_tag=False)
                full_pkg_name = pkg['packagePath']+pkg['packageName']
                versions = filter(lambda v:v[2]==full_pkg_name, versions)

            if len(versions)==0:
                self.msg.error(
                    "no version found for package [%s] and release [%s]",
                    pkg,
                    release)

        except PyAmi.AMI_Error, err:
            pass
            
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
        result = self.exec_cmd(
            cmd=cmd,
            expandedPackageID='*',
            expandedTopContainerPackage='*',
            groupName=project,
            processingStep='production',
            project='TagCollector',
            releaseName=release,
            )
        if not result:
            raise RuntimeError(
                "Could not retrieve the dependency tree for project [%s]"
                " and release [%s]" % (project, release,)
                )
        import xml.etree.cElementTree as ET
        d = result.to_dict()

        out = d
        abs_path = ('AMIMessage', 'Result', 'tree', 'treeBranch',)
        for i,k in enumerate(abs_path):
            if not k in out:
                raise RuntimeError(
                    'malformated answer from AMI (no [%s] key)' % k
                    )
            out = out[k]
        return out

    def get_open_releases(self, project):
        return self.get_releases(project, lambda x : x!='terminated')
        
    def get_releases(self, project, relStatusCond=lambda x : True):        
        """return the list of open releases for a given ``project``"""
        args = {
           'groupName' : project,
            'expandedRelease': '*',
           }

        result = self.exec_cmd(cmd='TCFormGetReleaseTreeDevView', args=args)
        if not result:
            raise RuntimeError(
                "Could not find open releases in project %s" % project
                )

        rxml = result.output('xml')
        import xml.etree.cElementTree as ET
   
        try:
            reltree = ET.fromstring(
                rxml
                ).find("Result").find("tree")
            releases = [ r.get("releaseName") 
                         for r in reltree.getiterator("treeBranch") 
                         if relStatusCond(r.get("status")) ]

            # Filter all special purpose releases (e.g. -MIG, -SLHC)
            releases = filter(lambda x: x.count("-")==0, releases)
        except Exception, e:
            self.msg.error(e.message)
            raise RuntimeError(
                'Could not parse result of TCFormGetReleaseTreeDevView:\n%s' % rxml
                )

        # Sort by release number
        releases.sort(key=lambda x: [int(y) if y.isdigit() else 0 for y in x.split('.')])
        return releases

    def get_clients(self, project, release, full_pkg_name):
        """return the list of clients (full-pkg-name, version) of
        `full_pkg_name` for project `project` and release `release`
        """
        args = {
            'groupName': project, # AtlasOffline, AtlasEvent, ...
            'releaseName': release,
            }
        if full_pkg_name[0] != "/":
            full_pkg_name = "/"+full_pkg_name
        args['fullPackageName'] = full_pkg_name
        
        result = self.exec_cmd(cmd="TCListPackageVersionClient", args=args)
        if not result:
            raise RuntimeError(
                'error executing TCListPackageVersionClient'
                )
        
        rxml = result.output('xml')
        import xml.etree.cElementTree as ET
        try:
            rows = xml2dict(ET.fromstring(rxml))['AMIMessage']["Result"]["rowset"]['row']
        except Exception, e:
            self.msg.error(e.message)
            raise RuntimeError(
                'could not parse result of TCListPackageVersionClient:\n%s' % rxml
                )

        if not isinstance(rows, (tuple,list)):
            rows = [rows]
            
        clients = []
        for row in rows:
            fields = row['field']
            client_name = None
            client_vers = None
            release_vers = None
            group_name = None
            for f in fields:
                if f['name'] == 'fullPackageName':
                    client_name = f['_text']
                elif f['name'] == 'packageTag':
                    client_vers = f['_text']
                elif f['name'] == 'releaseName':
                    release_vers = f['_text']
                elif f['name'] == 'groupName':
                    group_name = f['_text']
            if client_name is None or client_vers is None:
                self.msg.warning("could not find client-info for:\n%s", fields)
            else:
                if client_name[0] == '/':
                    client_name = client_name[1:]
                clients.append((client_name, client_vers, release_vers, group_name))
        return clients
    
    pass # Client

