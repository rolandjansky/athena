/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page AthenaPoolUtilities_page 

AthenaPoolUtilities is a package containing tools for using POOL
persistency software with Athena. Presently it contains CMT patterns
for generating POOL converters, iohandlers for element links, and some
command line tools for accessing and managing POOL collections and
metadata. This package also contains a class AthenaAttributeList which
is an Athena adapter for [[http://pool.cern.ch/releases/POOL_2_0_0/doc/classReference/html/AttributeList_8cpp-source.html][AttributeList]] and is described
below. Finally, there are a number of POOL Collection utilities whose
use are as well described below.

@section AthenaPoolUtilities_converters Generating POOL Converters For Data Packages

15 August 2003  RDS

This is a brief how-to which explains how to generate the pool
converters for one or more data packages. (See the corresponding
how-to in the AtlasSEAL package for how to generate a SEAL dictionary
for you data classes.)

Pool converters are automatically generated using a cmt pattern. By
convention, the generation is done in a single package for a number
of classes. For example, the InDet Event classes are done in the
InnerDetector/InDetEventCnv/InDetEventAthenaPool package. We use this
package as an example.

1) Rule 1: each class must be declared in a separate .h file. For
   example, InDetRawData/PixelRDO_Container.h holds the definition of
   only the PixelRDO Container.

2) In the requirements file of converter package one has:

@verbatim
   package InDetEventAthenaPool

   author  Davide Costanzo <Davide.Costanzo@cern.ch>

   use AtlasPolicy          AtlasPolicy-* 
   use AthenaPoolUtilities  AthenaPoolUtilities-00-*   Database/AthenaPOOL
   use InDetRawData         InDetRawData-01-*	    InnerDetector/InDetRawEvent

   # temporarily add in explicit link to dictionary
   macro_append InDetEventAthenaPool_linkopts " -lInDetRawDataDict "

   apply_pattern poolcnv files="-s=${InDetRawData_root}/InDetRawData \
   PixelRDO_Container.h SCT_RDO_Container.h TRT_RDO_Container.h "
@endverbatim

The "use AthenaPoolUtilities" is needed to get the poolcnv
pattern. The other users should refer to packages with data objects
which need converters.

One should provide the list of header files for the pattern
"poolcnv". These can be taken from a number of packages following the
syntax of 

@verbatim
   "-s=${<package>_root}/<package> <hdr1> hdr2>" 
@endverbatim

which can be repeated. 

The necessary "uses" should be done to the packages with the header
files. 

Finally, one should temporarily link against the dictionary libraries
containing the object descriptions needed by the converter. This is
done by adding the library names in the list of <package>_linkopts as
seen above. Note that this will be automated in the future so that the
dictionaries will be loaded when needed. When this happens the
<package>_linkopts will need to be removed.

@section AthenaPoolUtilities_AthenaAttributeList AthenaAttributeList

The class AthenaAttributeList is used to set the Tag in an Event
Collection. An !AttributeList represents a logical row of attributes in
a metadata table.  The name and type of each attribute is given in an
AttributeListSpecification.  AthenaAttributeList is an !AttributeList
which is also a DataObject and thus can be saved in an Athena data
store, e.g. the Event Store. The classes [[http://pool.cern.ch/releases/POOL_2_0_0/doc/classReference/html/classpool_1_1AttributeList.html][AttributeList]] and
[[http://pool.cern.ch/releases/POOL_2_0_0/doc/classReference/html/classpool_1_1AttributeListSpecification.html][AttributeListSpecification]] are defined in POOL (and eventually in
SEAL).

@section AthenaPoolUtilities_TPObjRef TPObjRef and TPCnvTokenList_p1 classes

This package contains 2 classes that are used in Athena TP-separated
persistent data model: TPObjRef and TPCnvTokenList_p1.

TPObjRef is used to replace object composition constructs existing in
the transient data model: pointers, inheritance and embedding. A TP
    converter toPersistent() method will create a persistent representation, store it in the top-level persistent object and return TPObjRef, which can be persistified. When reading, the TPObjRef is used to locate the persistent representation and convert it to transient.

TPCnvTokenList_p1 is used in case a top-level persistent object needs to be extended. The extensions are written as separate POOL objects and tokens referencing these objects are kept in the principal top-level object in a data member of TPCnvTokenList_p1 type.

    Full description of the TP separation used in Athena can be found on the Wiki page:
    https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation
*/
