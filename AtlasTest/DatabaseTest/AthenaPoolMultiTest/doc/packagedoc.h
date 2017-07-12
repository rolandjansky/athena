/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page AthenaPoolMultiTest_page AthenaPoolMultiTest package
 *
 * @date 26 May 2009
 * @author J. Cranshaw
 *
@section AthenaPoolMultiTest_@section Purpose Purpose: TEST
 *
 * AthenaPoolMultiTest is the part of the DatabaseTest packages 
 * that checks functionality which requires multiple files or 
 * multiple streams. 
 * <br> <br>
@section AthenaPoolMultiTest_@section One Dependencies and Associations
 * It uses algorithms from AthenaPoolExampleAlgorithms to create 
 * the dummy data. The dummy data classes are defined in 
 * AthenaPoolExampleData and AthenaPoolTestData.
 * <br><br>
@section AthenaPoolMultiTest_@section Two Components
 * The algorithms defined here serve as test filters to be applied to output streams 
 * (EventSplit, PassAllFilter, PassNoneFilter), utilities useful for the generating test data (AddTrigMap), 
 * tag building (EventTagWriter, RunEventTagWriter), 
 * and utilities for validation (EventTagReader, EventCheck).
@section AthenaPoolMultiTest_@section Three Running the Tests
 * The tests which run in the nightlies are defined in the requirements 
 * file using the athenarun_test pattern. They show up in the nightly 
 * test report under the package name. The reference files are in the 
 * test directory. The scripts which run the tests are labeled as 
 * bi(BuildInput), es(EventSplit), co(CheckOutput), and ci(CollectionInput). 
 * After the package is built, the tests can be run by hand using gmake check. 
 * The output from gmake check shows up in the run directory as *.log.small. 
 * These are the files which should be copied into new reference files.
 * <br>
 * <ul>
 * <li>AthenaPoolMultiTestBuildInput: Writes input data file and input collection. 
 * Does not test multi functionality. </li>
 * <li>AthenaPoolMultiTestCollectInput: Tests readability of multiple collections 
 * and it use the TagTool rather than the LCGTool.</li>
 * <li>AthenaPoolMulitTestEventSplit: Uses input data file to test multiple streams 
 * for data files and collections. 
 * It tests the methods of AthenaOutputStream and RegistrationStream 
 * used for filtering. It also check that multiple streams (2,3) can 
 * write to the same output file. </li>
 * <li>AthenaPoolMultiTestCheckCollections: Test collection metadata writing and copying.</li>
 * <li>AthenaPoolMultiTestCheckOutput: Checks that the output from the 
 * EventSplit test is readable.</li>
 * </ul>
 * <br>
 *
 *
 */
