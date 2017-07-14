/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page DetDescrCondTools_page The DetDescrCondTools package

@section DetDescrCondTools_introductionDetDescrCondTools Introduction

This package provides a set of tools and services related to conditions data
use in Detector Description. The available utilities (so far the services 
CoolStrFileSvc and CoolHistSvc, and the AlgTool AlignTransTool) 
are discussed below.

@section DetDescrCondTools_CoolStrFileSvc 

The service CoolStrFileSvc provides a service to store and retrieve strings
of arbitrary length (theoretically up to 16MB) directly into the COOL 
conditions database as inline payloads (stored in the database internally 
as strings or CLOBS). It is intended for use as a convenient way to store
data which was previously stored in flat ASCII files, typically kept in the
release InstallArea, located in Athena code using the PathResolver and read 
in using filestreams. 
Such data is typically time dependent (different files are required 
for different running periods) and so should logically be stored in the
conditions database.

The service header file ICoolStrFileSvc.h defines three public methods:

 - putFile: given a filename, save the data from this file in the transient
detector store (and hence into the conditions database) with a specified 
folder, channel and technology.

 - getString: retrieve data corresponding to a specified folder and channel
from the conditions database into a string.

 - getFile: retrieve data corresponding to a specified folder and channel
from the conditions database into a file.

The data returned from the getString method as a string can be turned into a 
std::istringstream and 
processed in the same way as a std::ifstream using code such as the following:
<pre>
  // turn the std::string data into a istringstream
  std::istringstream istr(data.c_str());
  // use stream-oriented input
  std::string a,b,c;
  istr >> a >> b >> c;
</pre>

The tool methods have to be used in conjunction with other algorithms and 
job options to link up with the conditions database. On writing, the putFile
method saves the transient copy of the data in the TDS, and the 
algorithm OutputConditionsAlg (in Database/RegistrationServices) is typically
used to actually write this to the conditions database, including specifying
the corresponding interval of validity for which it is valid. On reading,
the IOVDbSvc must be used to connect to the database, and the IOVDbSvc.Folders
property must include the name of the folder where the data was saved,
thus ensuring the appropriaate data (depending on run/event number) is loaded
into the TDS. The algorithm ReadWriteCoolStrFile in the package 
DetDescrCondExample demonstrates using the CoolStrFileSvc tool to read and
write some actual data.

The specification of the channel (integer) in the get/put calls allows
more than one set of files to be stored in a single COOL folder. This would
typically be used for example if several files are required for different
regions of the detector, each with independent intervals of validity. Note
that channel 0 (zero) should not be used (technically because IOVDbSvc
treats folders with channel zero as an AthenaAttributeList, not as a 
CondAttrListCollection).

The choice of 'technology' allows different representations of the data to
be stored:
 - tech=0 corresponds to COOL strings, limited to 4000 characters
 - tech=1 corresponds to COOL CLOB objects, limited to 16 million characters
 - in the future, it is planned to add a 'compressed' technology, where
   objects are first zipped before storage.

The choice of technology must be the same for ALL objects stored in a given
folder (spanning different intervals of validity and/or different channels). 
Thus unless
all objects for a particular folder are known to be smaller than
4000 characters, technology 1 (CLOBS) is recommended. However, using 
technology 1 requires the COOL ExtendedAttributeList interface, which means 
new COOL
folders cannot be created for technology 1 folders inside Athena. This means
the folder itself must first be created using the Python tool 
coolStrFileSvc_set.py with the --tech 1 option.

This tool would normally be used in reconstruction jobs to read data after the
first event has been read in (i.e. NOT from an algorithm initialisation 
method, but from an execute method or in response to a beginRun incident
or callback).
Clients should bear in mind that in principle such conditions data could
change every run, or even every event. It is possible for algorithms to be
called back whenever the data is changed (e.g. to update an internal cache).
To do this, set a call back on the CondAttrListCollection object with
the Storegate key set to the same name as the COOL folder being used.

An alternative way to input data for CoolStrFileSvc is to use the standalone
tool coolStrFileSvc_set.py. This provides the same 
functionality as the putFile method without the need to run an Athena job. This
tool can both create a folder (if not already existing) and store a given
object. Use coolStrFileSvc_set.py without arguments to see the syntax.

@section DetDescrCondTools_CoolHistSvc 

The service CoolHistSvc provides Athena-based access to histograms stored in
ROOT file, using the COOL conditions database to manage the histograms
intervals of validity. It is intended to control access to e.g. calibration
or reference histograms which need to be versioned and may have validity
ranges depending on run number (i.e. they are typical conditions data).

The basic model is as follows: The calibration expert prepares standard
ROOT files containing one or many histograms, valid for a particular interval
of validity (they may be prepared e.g. by writing out histograms from Athena
analysis of calibration data using THistSvc). These files are published in the
conditons DDM system (as for conditions data POOL files), and references to
them are stored in a given COOL folder and channel. The file-referencing is
done using a 'fileGUID' which is inserted into the histogram file using a 
special command, and plays the same role as a POOL file GUID (though it is 
stored as a simple ROOT TObjString under the key 'fileGUID' in the ROOT file,
rather than being stored using POOL tools.

@section DetDescrCondTools_AlignTransTool 

The tool AlignTransTool provides a tool to access HepTransform3Ds stored
in AlignableTransform objects in the TDS. It sorts the contents of the 
AlignableTransform objects by Identifier, facilititing rapid access.
This tool is not widely used, and might be declared obselete.



*/
