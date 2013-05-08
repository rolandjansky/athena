/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

   @mainpage TauTools Package

   @author Jyothsna Rani Komaragiri, Jennifer Godfrey and Dugan O'Neil
   
   @section Introduction
   This package has few examples of how to use common analysis tools for tau 
   studies. This documentation is relevant to simulated data reconstructed in
   release 13 or later.

   There are currently several choices of "analysis styles" for ATLAS users. 
   For example, one could opt for ATHENA-based analysis code or something 
   based on ROOT macros. The tools documented here are written in a general 
   way so that they are helpful regardless of the choice one makes. Where 
   possible, several implementations of each example are given in different 
   analysis styles (eg. ATHENA or AthenaROOTAccess).

   For more details on how to use and run this package visit the 
   <a href="https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TauAnalysis">
   wiki page</a>.

   @section Overview
   The TauTools package contains of following classes:
   - TruthHandler: contains methods which take truth particle/collection as 
   input. Truth tau methods are tested with different generators (ex: Pythia, Herwig, Sherpa, MC@NLO).
   - RecoHandler: contains tools to select reconstructed taus
   - MatchingTools: contains simple deltaR matching tools to match truth and 
   reco objects
   - TauExamples: Example usage of the tools provided above. Inherits from 
   AraToolBase.
   - TauCommonDetailsMerger: Merges old (Tau1P3P/TauRec(Extra)Details) into
   TauCommon(Extra)Details

   @section Examples
   These tools can be accessed in ATHENA as well as AthenaROOTAccess 
   (C++ or Python).
   - TauToolAthenaExample: Example to use TauExamples in Athena
   - TauToolARAExample: CINT AthenaROOTAccess example
   - TauToolCompiledARAExample: C++ AthenaROOTAccess example
   - TauToolPythonExample: Python AthenaROOTAccess example
   - TauToolCommonDetailsMergerExample: TauCommonDetailsMerger example

   @section Tau EDM dumper
   TauTools package also houses the tau EDM dumper to dump EDM contents into a flat root tuple and store map in storegate
   - TauEDMdump: dumps the tau EDM contents
   - TauAssocdump: dumps the contents of some associated objects (eg. tracks)

   @section Tau Calibration tools
   TauTools contains tools to derive tau energy calibration factors.
   - TauCalibrationTools: Provides methods to derive the calibration factors for taus while running tau reconstruction.
   - FindFactors: Athena Algorithm to derive the calibration factors.

   Packages used by TauTools are listed here:

   @htmlinclude used_packages.html

   The requirements file for TauTools is shown here:

   @include requirements


*/
