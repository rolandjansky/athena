/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@page AthenaPoolTest_page 

AthenaPoolTest is a package containing tests of I/O for
AthenaPOOL. The tests write and read data objects which are both
simple types and as well classes taken from the standard atlas
event. Different features of I/O are also tested, i.e.

  - Back navigation from file to file: where a first file is written
    and then read and a second file is written. When reading back the
    EventSelector.BackNavigation property is set true and objects
    from both files may be read
  - Navigating element links across files: when objects in a
    container in one file have element links to objects in a previously
    written file, one can read objects from both files by opening the
    second file and following the element links
  - Use of Logical File Names (LFN) instead of Physical File Names
    (PFN).  One can add to the pool file catalog LFN for any file and
    to read back one simply specifies the input collection as
    LFN:\<logical file name\>
  - Use of explicit event collections
  - Adding of detector description tags to the EventInfo object


@section AthenaPoolTest_HowTheTestWork How the tests work

Currently the tests are run using the athenarun_test pattern. This
allows a pre and a post script to be run before/after an athena
jobOption file is run. The tests are set up in the requirements file
and run via "gmake check". They are also run in the nightly. Some of
these tests are run as kit validation tests in the package
JobTransforms.

Normally a complete test is composed of two or more jobs. For each job
the post script compares the output of an athena job with a standard
output which has been stored with the package in cvs. The comparison
currently allows one to specify which lines in the output to compare
as specified by a selection string, which is typically the algorithm
name which appears in the output.


@section AthenaPoolTest_TheTests The tests

The following tests are currently being run:

  - AthenaPoolTest: Write/read out some simple objects
    - Two jobOptions run: 
      - AthenaPoolTestWrite.py
      - AthenaPoolTestRead.py
      .
    .

  - Navigation tests: use back navigation, used in combination with
    previous test
    - Two jobOptions run: 
      - AthenaPoolTestStep2Write.py : rewrites part of the above write
        into a second file
      - AthenaPoolTestReadNav.py : read part from second file, part
        from first file via navigation
      .
    .

  - Write/read InDetRawData Identifiable Containers for pixel, sct
    and trt
    - Two jobOptions run: 
      - InDetRawDataWriter_jobOptions.py
      - InDetRawDataReader_jobOptions.py
      .
    .

  - Write/read LAr &Cell &Identifiable Containers
    - Two jobOptions run: 
      - LArCellContWriter_jobOptions.py
      - LArCellContReader_jobOptions.py
      .
    .

  - Test of explicit collection write/read
    - Two jobOptions run: 
      - LArCellContWriter_jobOptionsReg.py
      - LArCellContReader_jobOptionsReg.py
      .
    .

  - Write/read LAr &Cell &Identifiable Containers using LFNs - write
    into two files and read then both one after the other.
    - Three jobOptions run: 
      - LArCellContWriter_jobOptionsStep1.py
      - LArCellContWriter_jobOptionsStep2.py
      - LArCellContReader_jobOptionsByLFN.py
      .
    .

  - Write/read CaloCluster Identifiable Containers
    - Two jobOptions run: 
      - CaloClusterWriter_jobOptions.py
      - CaloClusterReader_jobOptions.py
      .
    .

  - Write/read test of Element Links: write LAr &Cell &Identifiable
    Containers in one file, read this file and store Calo Clusters in
    a second file with Element Links back to the LAr Cells the first
    file.  Then read back the CaloClusters and navigate back to the
    LArCells. 
    - Three jobOptions run: 
      - CaloClusterStep1Writer_jobOptions.py
      - CaloClusterStep2Writer_jobOptions.py
      - CaloCluster2StepReader_jobOptions.py
      .
    .


  - Write explicit root collections with an extra ref. That is one
    first writes one file, reads the first file and writes a second
    file and at the same time write an explicit collection with a
    reference the data header in the second file. The collection also
    has an extra ref as part of the tag attributes and this can be
    used to navigate to the first file.
    - Four jobOptions run: 
      - AthenaPoolTestWrite.py: write out implicit collection with athena pool test data
      - LArCellContWriter_jobOptionsRegStep2.py: write out explicit collection with LArCellCont
      - LArCellContReader_jobOptionsRegStep1.py: read in LArCellCont
      - LArCellContReader_jobOptionsRegStep2.py: read in athena pool test data
      .
    .

  - Check for adding DetDescr tags to event info object
    - Five jobOptions run:
      - EventInfoWrite.py: write out new tags into EventInfo
      - EventInfoWriteStep2.py: read in EventInfo with tags and write dummy tags 
      - EventInfoWriteStep3.py: read in EventInfo with tags and write
        dummy tags with override 
      - EventInfoWriteStep4.py: read in EventInfo with tags and write
        dummy tags with override EventInfoTags 
      - EventInfoRead.py: read back EventInfo tags
      .
    .
  .






*/
