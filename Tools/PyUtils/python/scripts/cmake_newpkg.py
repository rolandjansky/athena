# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.cmake_newpkg
# @purpose streamline and ease the creation of new cmake packages
# @author Will Buttinger
# @date Feb 2017

from __future__ import with_statement

__version__ = "$Revision: 795362 $"
__author__ = "Will buttinger"
__doc__ = "streamline and ease the creation of new cmake packages"

### imports -------------------------------------------------------------------
import os
import textwrap
import commands
import PyUtils.acmdlib as acmdlib

### functions -----------------------------------------------------------------
@acmdlib.command(
    name='cmake.new-pkg'
    )
@acmdlib.argument(
    'pkgname',
    help="(fully qualified) name of the new package"
    )
@acmdlib.argument(
    '--author',
    default='${USER}',
    help='name of the author of this new package'
    )

def main(args):
    """create a new cmake package with sensible atlas-oriented defaults

    ex:
     $ acmd cmake new-pkg Control/MyContainer/NewPackage
    """
    sc = 0
    
    full_pkg_name = args.pkgname
    if full_pkg_name[0] == '/':
        full_pkg_name = full_pkg_name[1:]
    
    pkg_path = os.path.dirname(os.getcwd() + "/" + full_pkg_name)
    pkg_name = os.path.basename(full_pkg_name)
    pkg_vers = '%s-00-00-00' % pkg_name
    author = os.path.expanduser(os.path.expandvars(args.author))

    if os.path.exists(pkg_path+"/"+pkg_name):
        print "ERROR: %s package already exists" % full_pkg_name
        return 1
        
    print textwrap.dedent("""\
    ::: creating package [%(full_pkg_name)s]...
    :::   - pkg name:    %(pkg_name)s
    :::   - pkg version: %(pkg_vers)s
    :::   - pkg path:    %(pkg_path)s
    :::   - author:      %(author)s""" % locals())


    #create the directories
    try:
        os.makedirs(pkg_path+"/"+pkg_name+"/src")
        os.makedirs(pkg_path+"/"+pkg_name+"/share")
        os.makedirs(pkg_path+"/"+pkg_name+"/python")
        os.makedirs(pkg_path+"/"+pkg_name+"/data");
        os.makedirs(pkg_path+"/"+pkg_name+"/util");
    except OSError:
        print "ERROR while making directories for " % (pkg_path+"/"+pkg_name+"/src")
        return -1


    with open(os.path.join(pkg_path+"/"+pkg_name,'CMakeLists.txt'), 'w') as req:
        print >> req, textwrap.dedent("""\
        ## automatically generated CMakeLists.txt file

        # Declare the package
        atlas_subdir( %(pkg_name)s )

        # Declare external dependencies ... default here is to include ROOT
        find_package( ROOT COMPONENTS MathCore RIO Core Tree Hist )

        # Declare public and private dependencies
        # Public dependencies are anything that appears in the headers in public include dir
        # Private is anything else
        
        # An example is included
        atlas_depends_on_subdirs(
            PUBLIC
   
            PRIVATE
            # Control/AthAnalysisBaseComps
        )

        # Declare package as a library
        # Note the convention that library names get "Lib" suffix
        # Any package you add to dependencies above, you should add
        # to LINK_LIBRARIES line below (see the example)
        atlas_add_library( %(pkg_name)sLib src/*.cxx
                           PUBLIC_HEADERS %(pkg_name)s
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES}
                                            #AthAnalysisBaseCompsLib
        )

        # if you add components (tools, algorithms) to this package
        # uncomment the next lines
        # atlas_add_component( %(pkg_name)s src/components/*.cxx
        #                      LINK_LIBRARIES %(pkg_name)sLib 
        # )
      
        # if you add an application (exe) to this package
        # declare it like this (note convention that apps live in util dir)
        # atlas_add_executable( MyApp util/myApp.cxx
        #                       LINK_LIBRARIES %(pkg_name)sLib
        # )

        # Install python modules, joboptions, and share content
        atlas_install_python_modules( python/*.py )
        atlas_install_joboptions( share/*.py )
        atlas_install_data( data/* )
        # You can access your data from code using path resolver, e.g.
        # PathResolverFindCalibFile("%(pkg_name)s/file.txt")

        
        """%locals())
    
    #also create a version.cmake file with PackageName-00-00-01 in it
    with open(os.path.join(pkg_path+"/"+pkg_name,'version.cmake'), 'w') as req:
        print >> req, ("%s-00-00-01" % pkg_name)

    #need to reconfigure cmake so it knows about the new files
    #rely on the WorkDir_DIR env var for this
    workDir = os.environ.get("WorkDir_DIR")
    if workDir == None:
        print "::: ERROR No WorkDir_DIR env var, did you forget to source the setup.sh script?"
        print "::: ERROR Please do this and reconfigure cmake manually!"
    else:
        print ":::  INFO Reconfiguring cmake %s/../." % workDir
        res = commands.getstatusoutput('cmake %s/../.' % workDir)
        if res[0]!=0:
            print ":::  WARNING reconfigure unsuccessful. Please reconfigure manually!"


    print "::: creating package [%(full_pkg_name)s]... [done]" % locals()
    
    
    
    return sc

