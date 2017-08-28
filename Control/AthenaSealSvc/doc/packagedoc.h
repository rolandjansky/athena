/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page AthenaSealSvc_page 

This Athena service check "completeness" of class definitions in
Reflex dictionaries. 

The "completeness" check for classes makes sure that the types of all
attributes of a class have their corresponding types defined.  The
checks are automatically done for the classes being written or read
in. The checking is controled by a jobOption  flag, which by default
is True:

<pre>
# Check the dictionary in memory for completeness (set to True of False)
AthenaSealSvc.CheckDictionary = True;
</pre>

A list of class names to be ignored during the completeness check may
be added to AthenaSealSvcIgnore_joboptions.py.

Note that from release 13 onwards, Reflex dictionary loading is done
automatically by ROOT. This is done using rootmap files which can be
found in LD_LIBRARY_PATH - the rootmap files contain a list of classes
that each dictionary holds and the corresponding name of the DLL
(dynamically loadable library).


*/
