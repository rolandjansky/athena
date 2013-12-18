#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import myunittest
import os

from PyJobTransformsCore.envutil import *


class PathEnvManips(myunittest.TestCase):

 
    def assertPath(self, pathIn, pathOut, *vargs ):
        envName = 'TESTPATH'
        if pathIn is None:
            envStart=None
            if envName in os.environ:
                del os.environ[envName]
        else:
            os.environ[envName] = pathIn
            envStart=pathIn

        self.func(envName, *vargs)
        if pathIn is None:
            pathInMess = "Undefined environment variable"
        else:
            pathInMess = pathIn
        pathNew = os.environ.get(envName)
        errMess = 'Starting with \"%s\" gives \"%s\" instead of \"%s\"' % \
                  ( pathInMess, pathNew, pathOut )
        
        self.assertEqual( pathNew, pathOut, errMess )


    
    def testAppendPathEnv(self):
        """append_path_env"""
        self.func = append_path_env

        self.assertPath( None, 'mydir', 'mydir' )
        self.assertPath( 'mydir', 'mydir:mydir', 'mydir' )
        self.assertPath( "", 'mydir', 'mydir' )
        self.assertPath( 'dir1', 'dir1:mydir', 'mydir' )
        self.assertPath( 'dir1:dir2', 'dir1:dir2:mydir', 'mydir' )
        self.assertPath( 'mydir:dir1:dir2', 'mydir:dir1:dir2:mydir', 'mydir' )
        self.assertPath( 'dir1:mydir:dir2', 'dir1:mydir:dir2:mydir', 'mydir' )
        self.assertPath( 'dir1:dir2:mydir', 'dir1:dir2:mydir:mydir', 'mydir' )
                        

    def testAppendPathEnvIf(self):
        """append_path_env_if"""
        self.func = append_path_env_if

        self.assertPath( None, 'mydir', 'mydir' )
        self.assertPath( 'mydir', 'mydir', 'mydir' )
        self.assertPath( "", 'mydir', 'mydir' )
        self.assertPath( 'dir1', 'dir1:mydir', 'mydir' )
        self.assertPath( 'dir1:dir2', 'dir1:dir2:mydir', 'mydir' )
        self.assertPath( 'mydir:dir1:dir2', 'mydir:dir1:dir2', 'mydir' )
        self.assertPath( 'dir1:mydir:dir2', 'dir1:mydir:dir2', 'mydir' )
        self.assertPath( 'dir1:dir2:mydir', 'dir1:dir2:mydir', 'mydir' )
                        

    def testAppendPathEnvForce(self):
        """append_path_env_force"""
        self.func = append_path_env_force

        self.assertPath( None, 'mydir', 'mydir' )
        self.assertPath( 'mydir', 'mydir', 'mydir' )
        self.assertPath( "", 'mydir', 'mydir' )
        self.assertPath( 'dir1', 'dir1:mydir', 'mydir' )
        self.assertPath( 'dir1:dir2', 'dir1:dir2:mydir', 'mydir' )
        self.assertPath( 'mydir:dir1:dir2', 'dir1:dir2:mydir', 'mydir' )
        self.assertPath( 'dir1:mydir:dir2', 'dir1:dir2:mydir', 'mydir' )
        self.assertPath( 'dir1:dir2:mydir', 'dir1:dir2:mydir', 'mydir' )
                        

    def testPrependPathEnv(self):
        """prepend_path_env"""
        self.func = prepend_path_env

        self.assertPath( None, 'mydir', 'mydir' )
        self.assertPath( 'mydir', 'mydir:mydir', 'mydir' )
        self.assertPath( "", 'mydir', 'mydir' )
        self.assertPath( 'dir1', 'mydir:dir1', 'mydir' )
        self.assertPath( 'dir1:dir2', 'mydir:dir1:dir2', 'mydir' )
        self.assertPath( 'mydir:dir1:dir2', 'mydir:mydir:dir1:dir2', 'mydir' )
        self.assertPath( 'dir1:mydir:dir2', 'mydir:dir1:mydir:dir2', 'mydir' )
        self.assertPath( 'dir1:dir2:mydir', 'mydir:dir1:dir2:mydir', 'mydir' )
                        

    def testPrependPathEnvIf(self):
        """prepend_path_env_if"""
        self.func = prepend_path_env_if

        self.assertPath( None, 'mydir', 'mydir' )
        self.assertPath( 'mydir', 'mydir', 'mydir' )
        self.assertPath( "", 'mydir', 'mydir' )
        self.assertPath( 'dir1', 'mydir:dir1', 'mydir' )
        self.assertPath( 'dir1:dir2', 'mydir:dir1:dir2', 'mydir' )
        self.assertPath( 'mydir:dir1:dir2', 'mydir:dir1:dir2', 'mydir' )
        self.assertPath( 'dir1:mydir:dir2', 'dir1:mydir:dir2', 'mydir' )
        self.assertPath( 'dir1:dir2:mydir', 'dir1:dir2:mydir', 'mydir' )
                        

    def testPrependPathEnvForce(self):
        """prepend_path_env_force"""
        self.func = prepend_path_env_force

        self.assertPath( None, 'mydir', 'mydir' )
        self.assertPath( 'mydir', 'mydir', 'mydir' )
        self.assertPath( "", 'mydir', 'mydir' )
        self.assertPath( 'dir1', 'mydir:dir1', 'mydir' )
        self.assertPath( 'dir1:dir2', 'mydir:dir1:dir2', 'mydir' )
        self.assertPath( 'mydir:dir1:dir2', 'mydir:dir1:dir2', 'mydir' )
        self.assertPath( 'dir1:mydir:dir2', 'mydir:dir1:dir2', 'mydir' )
        self.assertPath( 'dir1:dir2:mydir', 'mydir:dir1:dir2', 'mydir' )


    def testReplacePathEnv(self):
        """replace_path_env"""
        self.func = replace_path_env

        self.assertPath(    None,       None,    'mydir', 'mynewdir' )
        self.assertPath( 'mydir', 'mynewdir',    'mydir', 'mynewdir' )
        self.assertPath( 'mydir',    'mydir', 'notmydir', 'mydir'    )
        self.assertPath(      "",         "",    'mydir', 'mynewdir' )
        self.assertPath(      "",         "",    '.*', 'mynewdir' )
        self.assertPath( 'mydir:dir1', 'mynewdir:dir1', 'mydir', 'mynewdir' )
        self.assertPath( 'mydir:dir1:dir2', 'mynewdir:dir1:dir2', 'mydir', 'mynewdir' )
        self.assertPath( 'dir1:mydir:dir2', 'dir1:mynewdir:dir2', 'mydir', 'mynewdir' )
        self.assertPath( 'dir1:dir2:mydir', 'dir1:dir2:mynewdir', 'mydir', 'mynewdir' )
        self.assertPath( 'mydir:dir1:dir2', 'mynewdir:newdir1:newdir2', 'dir', 'newdir' )
        self.assertPath( 'mydir:dir1:keepthisone:dir2', 'mynewdir:newdir1:keepthisone:newdir2', 'dir', 'newdir' )



    def testRemovePathEnv(self):
        """remove_path_env"""
        self.func = remove_path_env

        testdir = '^mydir$'
        self.assertPath( None, None, testdir )
        self.assertPath( 'mydir', '', testdir ) 
        self.assertPath( 'dir1:mydir', 'dir1',       testdir )
        self.assertPath( 'mydir:dir1', 'dir1',       testdir )
        self.assertPath( 'mydir:dir1', 'mydir:dir1', 'otherdir' )
        self.assertPath( 'dir1:mydir', 'dir1',       testdir    )
        self.assertPath( 'dir1:mydir', 'dir1:mydir', 'otherdir' )
        self.assertPath( 'dir1:mydir:dir2', 'dir1:dir2',  testdir    )
        self.assertPath( 'dir1:mydir:dir2', 'dir1:mydir:dir2', 'otherdir' )


    def testRemovePathEnvRE(self):
        """remove_path_env regular expression"""
        self.func = remove_path_env
        self.assertPath( 'dir1:mydir:mydir2:keep_mydir:dir2', 'dir1:dir2',  'mydir'    )
        self.assertPath( 'dir1:mydir:mydir2:keep_mydir:dir2', 'dir1:keep_mydir:dir2',  '^mydir'    )
        self.assertPath( 'dir1:mydir:mydir2:keep_mydir:dir2', '',  '.*'    )
        self.assertPath( 'dir1:mydir:mydir2:keep_mydir:dir2', 'dir1:mydir:keep_mydir',  '2$'    )


class TestFileFind(myunittest.TestCase):
    def testIgnoreNoneExistingDir(self):
        """Non-existing dir in path"""
        found = find_file( '.', [ "not_existing_dir" ], depth=3 )
        self.assertEqual( found, None )
            
                        
if __name__ == "__main__":
    myunittest.main()


