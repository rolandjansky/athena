#!/usr/bin/env python
# @author: Sebastien Binet <binet@cern.ch>

"""unit tests for verifying the proper working of HDF5"""

import unittest
import tempfile, os, commands, atexit, shutil
import unittest, sys, atexit, os, commands, tempfile, re, difflib

### data ----------------------------------------------------------------------
__version__ = "$Revision: 1.1 $"
__author__  = "Sebastien Binet <binet@cern.cn>"

### helper methods ------------------------------------------------------------
def safe_remove(fname):
    if os.path.exists(fname) and \
       (os.path.isfile(fname) or os.path.islink(fname)):
        atexit.register(os.unlink, fname)

## For compatibility with ATN tests
from TestTools.iobench import workDir
def installRefFiles( fileNames, quiet=True ):
    for refFile in fileNames:
        for fileName in [ refFile, workDir(refFile) ]:
            if os.path.exists(fileName):
                os.remove(fileName)
        sc,out = commands.getstatusoutput( "get_files %s" % refFile )
        if sc != 0:
            print "## ERROR: could not retrieve [%s]" % refFile
            print "## reason:\n",out
            continue
        if os.path.exists(refFile) and \
           os.path.exists(workDir(refFile)) and \
           os.path.samefile( refFile, workDir(refFile) ):
            if not quiet:
                print " -%s" % workDir(refFile)
            continue
        try:
            shutil.move(refFile, workDir(refFile))
            if not quiet:
                print " -%s" % workDir(refFile)
        except OSError,err:
            print "## ERROR: could not install [%s] into [%s]" %\
                  ( refFile, workDir(refFile) )
            print "## reason:\n",err
            continue
    return

_hdf5_ref_out_hdr = """\
*******************************
*  Output of example program  *
*******************************

"""
_hdf5_ref_dump_hdr = """\



**********************
*  Output of h5dump  *
**********************

"""

class Hdf5TestCase (unittest.TestCase):
    def run_app (self, app_name):
        sc, app = commands.getstatusoutput ('which %s.exe'%app_name)
        self.failUnless(sc==0, 'could not find application [%s.exe] !'%app_name)
        sc, out = commands.getstatusoutput (app)
        self.failUnless(sc==0, 'problem running [%s.exe]:\n%s'%(app_name,out))
        return out

    def _h5dump (self, fname):
        sc, app = commands.getstatusoutput ('which h5dump')
        self.failUnless (sc==0, 'could not find \'h5dump\' application !')
        cmd = '%s %s' % (app, fname)
        sc, out = commands.getstatusoutput (cmd)
        self.failUnless (sc==0, 'problem running [%s]:\n%s'%(cmd,out))
        out = _hdf5_ref_dump_hdr+out
        return out
    
    def _run_hdf5_test (self, app_name, has_output=True):
        installRefFiles (['%s.ref'%app_name])
        out = self.run_app (app_name)
        out_file = open (workDir('%s.log'%app_name), 'w')
        out_file.writelines ([l for l in _hdf5_ref_out_hdr])
        out_file.writelines ([l for l in out])

        h5_out_file = '%s.h5'%app_name
        if has_output:
            self.failUnless (os.path.exists (h5_out_file),
                             "no such file [%s]"%h5_out_file)
            out = self._h5dump (h5_out_file)
            out_file.writelines ([l for l in out])
            out_file.writelines ([os.linesep])
        out_file.flush()
        out_file.close()

        if has_output:
            tmp_h5_out_file = h5_out_file
            h5_out_file     = workDir('%s.h5'%app_name)
            os.rename (tmp_h5_out_file, h5_out_file)
        
        out_file = open(workDir('%s.log'%app_name), 'r')
        ref_file = open(workDir('%s.ref'%app_name), 'r')

        out_for_diff = [l.strip() for l in out_file]
        ref_for_diff = [l.strip() for l in ref_file]

        diff = difflib.unified_diff (out_for_diff, ref_for_diff)
        diff = os.linesep.join ([d for d in diff])
        self.failUnless (diff=='', diff)

        ## delete files if all good
        safe_remove (out_file.name)
        safe_remove (ref_file.name)
        safe_remove (h5_out_file)
        return
    
class Hdf5DataSets (Hdf5TestCase):
    """tests the DataSet-part API of HDF5"""
 
    def test01_h5ex_d_alloc (self):
        """Set Space Allocation Time for Dataset
        """
        self._run_hdf5_test ('h5ex_d_alloc')
        return

    def test02_h5ex_d_checksum (self):
        """Read / Write Dataset using Fletcher32 Checksum Filter
        """
        self._run_hdf5_test ('h5ex_d_checksum')
        return

    def test03_h5ex_d_chunk (self):
        """Read / Write Chunked Dataset
        """
        self._run_hdf5_test ('h5ex_d_chunk')
        return

    def test04_h5ex_d_compact (self):
        """Read / Write Compact Dataset
        """
        self._run_hdf5_test ('h5ex_d_compact')
        return

    def test05_h5ex_d_extern (self):
        """Read / Write to External Dataset
        """
        print "[SKIPPED] ",
        sys.stdout.flush()
        #self._run_hdf5_test ('h5ex_d_extern')
        return
 
    def test06_h5ex_d_fillval (self):
        """Read / Write Dataset w/ Fill Value
        """
        self._run_hdf5_test ('h5ex_d_fillval')
        return
 
    def test07_h5ex_d_gzip (self):
        """Read / Write GZIP Compressed Dataset
        """
        self._run_hdf5_test ('h5ex_d_gzip')
        return
 
    def test08_h5ex_d_hyper (self):
        """Read / Write Data by Hyperslabs
        """
        self._run_hdf5_test ('h5ex_d_hyper')
        return
 
    def test09_h5ex_d_nbit (self):
        """Read / Write Dataset with n-bit Filter
        """
        self._run_hdf5_test ('h5ex_d_nbit')
        return
 
    def test10_h5ex_d_rdwr (self):
        """Read / Write Integer Dataset
        """
        self._run_hdf5_test ('h5ex_d_rdwr')
        return
 
    def test11_h5ex_d_shuffle (self):
        """Read / Write Dataset w/ Shuffle Filter and GZIP Compression
        """
        self._run_hdf5_test ('h5ex_d_shuffle')
        return
 
    def test12_h5ex_d_sofloat (self):
        """Read / Write Dataset using Scale-Offset Filter (float)
        """
        self._run_hdf5_test ('h5ex_d_sofloat')
        return
 
    def test13_h5ex_d_soint (self):
        """Read / Write Dataset using Scale-Offset Filter (integer)
        """
        self._run_hdf5_test ('h5ex_d_soint')
        return
 
    def test14_h5ex_d_szip (self):
        """Read / Write Dataset using SZIP Compression
        """
        self._run_hdf5_test ('h5ex_d_szip')
        return
 
    def test15_h5ex_d_transform (self):
        """Read / Write Dataset using Data Transform Expression
        """
        self._run_hdf5_test ('h5ex_d_transform')
        return
 
    def test16_h5ex_d_unlimadd (self):
        """Read / Write Unlimited Dimension Dataset
        """
        self._run_hdf5_test ('h5ex_d_unlimadd')
        return
 
    def test17_h5ex_d_unlimgzip (self):
        """Read / Write GZIP Compressed Unlimited Dimension Dataset
        """
        self._run_hdf5_test ('h5ex_d_unlimgzip')
        return
 
    def test18_h5ex_d_unlimmod (self):
        """Read / Write / Edit Unlimited Dimension Dataset
        """
        self._run_hdf5_test ('h5ex_d_unlimmod')
        return
 
class Hdf5Groups (Hdf5TestCase):
    """tests the Groups-part API of HDF5"""

    def _run_hdf5_test_with_input (self, app_name):
        installRefFiles (['%s.ref'%app_name,
                          '%s.h5' %app_name])

        h5_out_file = '%s.h5'%app_name
        os.rename (workDir('%s.h5'%app_name), h5_out_file)
        self.failUnless (os.path.exists (h5_out_file),
                         "no such file [%s]"%h5_out_file)

        out = self.run_app (app_name)
        out_file = open (workDir('%s.log'%app_name), 'w')
        out_file.writelines ([l for l in _hdf5_ref_out_hdr])
        out_file.writelines ([l for l in out])

        out = self._h5dump (h5_out_file)
        out_file.writelines ([l for l in out])
        out_file.writelines ([os.linesep])
        out_file.flush()
        out_file.close()

        tmp_h5_out_file = h5_out_file
        h5_out_file     = workDir('%s.h5'%app_name)
        os.rename (tmp_h5_out_file, h5_out_file)
     
        out_file = open(workDir('%s.log'%app_name), 'r')
        ref_file = open(workDir('%s.ref'%app_name), 'r')

        out_for_diff = [l.strip() for l in out_file]
        ref_for_diff = [l.strip() for l in ref_file]

        diff = difflib.unified_diff (out_for_diff, ref_for_diff)
        diff = os.linesep.join ([d for d in diff])
        self.failUnless (diff=='', diff)

        ## delete files if all good
        safe_remove (out_file.name)
        safe_remove (ref_file.name)
        safe_remove (h5_out_file)
        return
 
    def test01_h5ex_g_compact (self):
        """Create 'compact-or-indexed' Format Groups
        """
        app_name = 'h5ex_g_compact'
        installRefFiles (['%s.ref'%app_name])
        out = self.run_app (app_name)
        out_file = open (workDir('%s.log'%app_name), 'w')
        out_file.writelines ([l for l in _hdf5_ref_out_hdr])
        out_file.writelines ([l for l in out])

        ## file1
        h5_out_file1 = '%s1.h5'%app_name
        self.failUnless (os.path.exists (h5_out_file1),
                         "no such file [%s]"%h5_out_file1)
        out = self._h5dump (h5_out_file1)
        out_file.writelines ([l for l in out])
        #out_file.writelines ([os.linesep])
        out_file.flush()

        tmp_h5_out_file1 = h5_out_file1
        h5_out_file1     = workDir('%s1.h5'%app_name)
        os.rename (tmp_h5_out_file1, h5_out_file1)

        ## file2
        h5_out_file2 = '%s2.h5'%app_name
        self.failUnless (os.path.exists (h5_out_file2),
                         "no such file [%s]"%h5_out_file2)
        out = self._h5dump (h5_out_file2)
        out_file.writelines ([l for l in out])
        out_file.writelines ([os.linesep])
        out_file.flush()
        out_file.close()

        tmp_h5_out_file2 = h5_out_file2
        h5_out_file2     = workDir('%s2.h5'%app_name)
        os.rename (tmp_h5_out_file2, h5_out_file2)
     
        out_file = open(workDir('%s.log'%app_name), 'r')
        ref_file = open(workDir('%s.ref'%app_name), 'r')

        out_for_diff = [l.strip() for l in out_file]
        ref_for_diff = [l.strip() for l in ref_file]

        diff = difflib.unified_diff (out_for_diff, ref_for_diff)
        diff = os.linesep.join ([d for d in diff])
        self.failUnless (diff=='', diff)

        ## delete files if all good
        safe_remove (out_file.name)
        safe_remove (ref_file.name)
        safe_remove (h5_out_file1)
        safe_remove (h5_out_file2)
        return

    def test02_h5ex_g_corder (self):
        """Track links in a Group by Creation Order
        """
        self._run_hdf5_test ('h5ex_g_corder')
        return

    def test03_h5ex_g_create (self):
        """Create / Open / Close a Group
        """
        self._run_hdf5_test ('h5ex_g_create')
        return

    def test04_h5ex_g_intermediate (self):
        """Create Intermediate Groups
        """
        self._run_hdf5_test ('h5ex_g_intermediate')
        return

    def test05_h5ex_g_iterate (self):
        """Iterate over Groups w/ H5Literate
        """
        self._run_hdf5_test_with_input ('h5ex_g_iterate')
        return

    def test06_h5ex_g_phase (self):
        """Set Conditions to Convert between Compact and Dense Groups
        """
        self._run_hdf5_test ('h5ex_g_phase')
        return

    def test07_h5ex_g_traverse (self):
        """Recursively Traverse a File w/ H5Literate
        """
        self._run_hdf5_test_with_input ('h5ex_g_traverse')
        return

    def test08_h5ex_g_visit (self):
        """Recursively Traverse a File w/ H5Ovisit and H5Lvisit
        """
        self._run_hdf5_test_with_input ('h5ex_g_visit')
        return

class Hdf5DataTypes (Hdf5TestCase):
    """tests the DataTypes-part API of HDF5"""

    def test01_h5ex_t_arrayatt (self):
        """Read / Write Array Datatypes (Attribute)
        """
        self._run_hdf5_test ('h5ex_t_arrayatt')
        return

    def test02_h5ex_t_array (self):
        """Read / Write Array Datatypes (Dataset)
        """
        self._run_hdf5_test ('h5ex_t_array')
        return

    def test03_h5ex_t_bitatt (self):
        """Read / Write Bitfield Datatypes (Attribute)
        """
        self._run_hdf5_test ('h5ex_t_bitatt')
        return

    def test04_h5ex_t_bit (self):
        """Read / Write Bitfield Datatypes (Dataset)
        """
        self._run_hdf5_test ('h5ex_t_bit')
        return

    def test05_h5ex_t_cmpdatt (self):
        """Read / Write Compound Datatypes (Attribute)
        """
        self._run_hdf5_test ('h5ex_t_cmpdatt')
        return

    def test06_h5ex_t_cmpd (self):
        """Read / Write Compound Datatypes (Dataset)
        """
        self._run_hdf5_test ('h5ex_t_cmpd')
        return

    def test07_h5ex_t_commit (self):
        """Commit Named Datatype and Read Back
        """
        self._run_hdf5_test ('h5ex_t_commit')
        return

    def test08_h5ex_t_convert (self):
        """Convert Between Datatypes in Memory
        """
        self._run_hdf5_test ('h5ex_t_convert', has_output=False)
        return

    def test09_h5ex_t_cpxcmpdatt (self):
        """Read / Write Complex Compound Datatype (Attribute)
        """
        self._run_hdf5_test ('h5ex_t_cpxcmpdatt')
        return

    def test10_h5ex_t_cpxcmpd (self):
        """Read / Write Complex Compound Datatype (Dataset)
        """
        self._run_hdf5_test ('h5ex_t_cpxcmpd')
        return

    def test11_h5ex_t_enumatt (self):
        """Read / Write Enumerated Datatypes (Attribute)
        """
        self._run_hdf5_test ('h5ex_t_enumatt')
        return

    def test12_h5ex_t_enum (self):
        """Read / Write Enumerated Datatypes (Dataset)
        """
        self._run_hdf5_test ('h5ex_t_enum')
        return

    def test13_h5ex_t_floatatt (self):
        """Read / Write Floating Point Datatype (Attribute)
        """
        self._run_hdf5_test ('h5ex_t_floatatt')
        return

    def test14_h5ex_t_float (self):
        """Read / Write Floating Point Datatype (Dataset)
        """
        self._run_hdf5_test ('h5ex_t_float')
        return

    def test15_h5ex_t_intatt (self):
        """Read / Write Integer Datatype (Attribute)
        """
        self._run_hdf5_test ('h5ex_t_intatt')
        return

    def test16_h5ex_t_int (self):
        """Read / Write Integer Datatype (Dataset)
        """
        self._run_hdf5_test ('h5ex_t_int')
        return

    def test17_h5ex_t_objrefatt (self):
        """Read / Write Object References (Attribute)
        """
        self._run_hdf5_test ('h5ex_t_objrefatt')
        return

    def test18_h5ex_t_objref (self):
        """Read / Write Object References (Dataset)
        """
        self._run_hdf5_test ('h5ex_t_objref')
        return

    def test19_h5ex_t_opaqueatt (self):
        """Read / Write Opaque Datatypes (Attribute)
        """
        self._run_hdf5_test ('h5ex_t_opaqueatt')
        return

    def test20_h5ex_t_opaque (self):
        """Read / Write Opaque Datatypes (Dataset)
        """
        self._run_hdf5_test ('h5ex_t_opaque')
        return

    def test21_h5ex_t_regrefatt (self):
        """Read / Write Region References (Attribute)
        """
        self._run_hdf5_test ('h5ex_t_regrefatt')
        return

    def test22_h5ex_t_regref (self):
        """Read / Write Region References (Dataset)
        """
        self._run_hdf5_test ('h5ex_t_regref')
        return

    def test23_h5ex_t_stringatt (self):
        """Read / Write String Datatype (Attribute)
        """
        self._run_hdf5_test ('h5ex_t_stringatt')
        return

    def test24_h5ex_t_string (self):
        """Read / Write String Datatype (Dataset)
        """
        self._run_hdf5_test ('h5ex_t_string')
        return

    def test25_h5ex_t_vlenatt (self):
        """Read / Write Variable Length Datatype (Attribute)
        """
        self._run_hdf5_test ('h5ex_t_vlenatt')
        return

    def test26_h5ex_t_vlen (self):
        """Read / Write Variable Length Datatype (Dataset)
        """
        self._run_hdf5_test ('h5ex_t_vlen')
        return

    def test27_h5ex_t_vlstringatt (self):
        """Read / Write Variable Length String Datatype (Attribute)
        """
        self._run_hdf5_test ('h5ex_t_vlstringatt')
        return

    def test28_h5ex_t_vlstring (self):
        """Read / Write Variable Length String Datatype (Dataset)
        """
        self._run_hdf5_test ('h5ex_t_vlstring')
        return

### ---------------------------------------------------------------------------
## actual test run
if __name__ == '__main__':
   loader = unittest.TestLoader()
   testSuite = loader.loadTestsFromModule( sys.modules[ __name__ ] )

   runner = unittest.TextTestRunner( verbosity = 2 )
   result = not runner.run( testSuite ).wasSuccessful()

   sys.exit( result )
