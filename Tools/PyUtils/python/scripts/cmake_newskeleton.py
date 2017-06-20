# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.cmt_newanalysisalg
# @purpose streamline and ease the creation of new athena algs
# @author Will Buttinger
# @date February 2017

#Note - this code could use a serious rewrite, I just hacked it together to get something working

from __future__ import with_statement

__version__ = "$Revision: 795362 $"
__author__ = "Will Buttinger"
__doc__ = "streamline and ease the creation of new AthAnalysisAlgorithm in a new package"

### imports -------------------------------------------------------------------
import os
import textwrap
import commands
import PyUtils.acmdlib as acmdlib
import fileinput



### functions -----------------------------------------------------------------
@acmdlib.command(
    name='cmake.new-skeleton'
    )
@acmdlib.argument(
    'pkgname',
    help="name of the new pkg"
    )


def main(args):
    """create a new skeleton package

    ex:
     $ acmd cmake new-skeleton MyPackage
    """
    sc = 0
    
    full_pkg_name = args.pkgname

    #make new package
    res = commands.getstatusoutput('acmd cmake new-pkg %s' % full_pkg_name)
    if res[0]!=0:
        print ":::  ERROR could not create new package"
        return -1


    #add algorithm
    res = commands.getstatusoutput('cd %s;acmd cmake new-analysisalg --newJobo %sAlg' % (full_pkg_name,full_pkg_name))
    if res[0]!=0:
        print ":::  ERROR could not create new alg"
        return -1

    pkg_name = full_pkg_name

    # overwrite CMakeLists with our skeleton
    with open(os.path.join(full_pkg_name,'CMakeLists.txt'), 'w') as req:
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
            Control/AthAnalysisBaseComps
        )

        # Declare package as a library
        # Note the convention that library names get "Lib" suffix
        # Any package you add to dependencies above, you should add
        # to LINK_LIBRARIES line below (see the example)
        atlas_add_library( %(pkg_name)sLib src/*.cxx
                           PUBLIC_HEADERS %(pkg_name)s
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES}
                                            AthAnalysisBaseCompsLib
        )

        # if you add components (tools, algorithms) to this package
        # these lines are needed so you can configure them in joboptions
        atlas_add_component( %(pkg_name)s src/components/*.cxx
                              LINK_LIBRARIES %(pkg_name)sLib 
        )
      
        # if you add an application (exe) to this package
        # declare it like this
        # atlas_add_executable( MyApp src/myApp.cxx
        #                       LINK_LIBRARIES %(pkg_name)sLib
        # )
        # Install python modules, joboptions, and share content
        atlas_install_python_modules( python/*.py )
        atlas_install_joboptions( share/*.py )
        atlas_install_data( data/* )
        # You can access your data from code using path resolver, e.g.
        # PathResolverFindCalibFile("%(pkg_name)s/file.txt")

        
        """%locals())




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
        

