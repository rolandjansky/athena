/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page LWHists_page LWHists: Light-weight versions of ROOT histogram classes.

This package provides lightweight versions of the widely used ROOT
histogram classes TH1F, TH1D, TH1I, TH2F, TH2D and TH2I: @m TH1F_LW,
@m TH1D_LW, @m TH1I_LW, @m TH2F_LW, @m TH2D_LW and @m TH2I_LW .<br/>

The classes are lightweight in the sense that they have a very low
memory foot-print as well as fast booking and filling times

All of these classes derive from the common @m LWHist class, and the
1D and 2D histograms derive from the @m LWHist1D and @m LWHist2D classes
respectively.

All of the classes implements a general @m LWHist::createTH1 method,
as well as a specialised version (e.g. @m TH1F_LW::createTH1F), which
can be used to create an instance of a corresponding ROOT histogram
class. The obvious use case for this is that one can use the
light-weight histogram during a given job, but still be able to write
out an actual ROOT histogram instance into an output file.

@section LWHists_optimisationsLWHists Optimisations used internally

@todo : Write this section

@section LWHists_testingLWHists Testing

In addition to the shared library, the package also provides two
applications for testing the performance (cpu/mem) and validity
(meaning that they must act like the corresponding ROOT classes) of
the provided histogram classes. The applications are <b>lwhists_benchmark.exe</b>
 and <b>lwhists_validate.exe</b>, respectively, and they should
 show up in your path when the package has been built.

@author Thomas.Kittelmann@cern.ch

*/
