# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.get_tag_diff
# @purpose Get the list of tag differences between 2 releases (CERN centric)
# @author Sebastien Binet
# @date February 2010

__version__ = "$Revision: 276362 $"
__doc__ = "Get the list of the SVN differences between packages in 2 releases (CERN centric)."
__author__ = "Edward Moyse"


### imports -------------------------------------------------------------------
import PyUtils.acmdlib as acmdlib
from subprocess import check_call,CalledProcessError

@acmdlib.command(
    name='get-tagsvn-diff'
    )
@acmdlib.argument(
    'old',
    help="The description string of the reference release (eg: 12.0.X,rel_3,AtlasOffline)"
    )
@acmdlib.argument(
    'new',
    help="The description string of the to-be-compared release (eg: 12.0.X,rel_3 or 12.0.3)"
    )
    
@acmdlib.argument(
    '-d', '--domain',
    help="Restrict to a given domain (e.g. MuonSpectrometer)"
    )

@acmdlib.argument(
    '-p', '--project',
    help="Restrict to a given project (e.g. Event)"
    )
    
@acmdlib.argument(
    '-k', '--package',
    help="Restrict to a given package (e.g. MuonRecExample). This is incompatible with restricting to a given domain or project."
    )

@acmdlib.argument(
    '-f', '--files', action='store_true', default=False,
    help="Call 'svn diff' with '--depth files', which will typically just show ChangeLog differences."
    )
        
def main(args):
    """Get the list of tag differences between 2 releases (CERN centric)
    """

    print "===> Generating tag difference by calling CMT \n"
    
    import PyCmt.Cmt as Cmt
    diffs = Cmt.get_tag_diff(ref=args.old,
                             chk=args.new,
                             verbose=False)

    if (args.package):
        print "===> Will now dump the SVN diff for ",args.package
        args.domain=None
        args.files=None
    else:
        print "===> Will now dump the SVN diff for the packages"
    
    if (args.domain):
        print "Restricting to packages in the domain",args.domain
    if (args.project):
        print "Restricting to packages in the project",args.project
    if (args.files):
        print "... and will limit to file-level (i.e. ChangeLog) differences."
        
    for line in diffs:
        oldTag = line['ref']
        oldProject = line['ref_proj']
        newTag = line['chk']
        newProject = line['chk_proj']
        if (args.project and args.project!=newProject):
            continue
                    
        path = line['full_name']
        domain = path.split('/')[0]
        # print "domain=",domain
        
        if (args.domain and args.domain!=domain):
            continue
        package = path.split('/')[-1]
        
        if (args.package and package!=args.package):
            continue
        
        if ('N/A' in newProject):
            print "Package",package,"removed from release. Skipping. \n"
            continue
        
        print
        print '===> Getting SVN diff for package '+package+' at '+path+'\n'

        path="svn+ssh://svn.cern.ch/reps/atlasoff/"+path
        #path="$SVNOFF/"+path

        if ('N/A' in oldProject):
            print "New package, so showing diff from first version. \n"
            oldTag = package+'-00-00-00' #TODO What if this isn't the first tag?
        else:
            oldTag=package+'-'+oldTag
        newTag=package+'-'+newTag
        # print path+'/tags/'+oldTag, path+'/tags/'+newTag
        svnargs = ["svn", "diff", path+'/tags/'+oldTag, path+'/tags/'+newTag]
        if (args.files):
            svnargs.extend( ["--depth", "files"] )
            
        try:
            svndiff = check_call(svnargs)
            if (svndiff):
                for svnline in svndiff:
                   print svnline

        except CalledProcessError:
          print 'Some problem running SVN diff!'

    if len(diffs) > 0:
        return 1
    return 0
