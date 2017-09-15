/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page PyUtils_page PyUtils Package
@author Sebastien Binet

@section PyUtils_PyUtilsIntro Introduction

This package holds various python classes and scripts of general use.

@section PyUtils_PyUtilsOverview Class Overview
  The PyUtils package contains the following classes:

  - python::PoolFile::PoolFile : a python class to model and extract informations about the content of a POOL file. The informations are stored into a python::PoolFile::PoolRecord (memory and compressed size, name of the container, number of such items into that POOL file). A script (@c checkFile.py) is provided to easily inspect and dump the content of such POOL files:
@code
lxplus> checkFile.py --help
lxplus> checkFile.py my.aod.pool
@endcode

  - python::Dso::DsoDb : a python class to gather and store informations about the shared libraries (data shared objects, DSO) an Athena release contains. Informations about dictionaries, component factories are gathered from the <i>rootmap</i> files in such a way that queries like duplicates searches and libraries capabilities can be issued. A wrapper script (@c checkPlugins.py) is also provided:
@code
lxplus> checkPlugins.py --help
lxplus> checkPlugins.py --check-dict-dups
@endcode

  - coverage.py : Do coverage testing on a python module.  When you run
regression tests on a python module, coverage.py will flag any lines
that are not executed by the tests.  Suggested usage is to create
a separate file for regression tests.  This module will then end
with the lines
@code
from PyUtils import coverage
c = coverage.Coverage ('MOD.NAME')
c.doctest_cover ()
@endcode
where MOD.NAME is the source file that you are testing.
Put your doctests in this regression test file (any doctests in the
original source file will also be executed).  After the regression
tests are run, if there are any lines in the source file that
were not executed, a message will be printed, and a .cover
file will be written that calls out the specific lines that
were not executed.

@section PyUtils_PyUtilsScripts Scripts
  The PyUtils package provides some useful scripts:

  - @c checkFile.py : as mentionned above this script can inspect a POOL file and dump on screen its 'high level' content. It will tell you what kind of containers have been stored (eg: @c ElectronContainer, @c TruthParticleContainer,...) but it won't tell you more detailed properties (such as pt,eta distributions). That's however already useful and can be used for example to quickly check your algorithm did its job if it was supposed to process 100 events and create 100 @c MyZeeBosonContainer. <b>Ex:</b>
@code
lxplus> checkFile.py atlfast.aod.pool
## opening file [atlfast.aod.pool]...
## importing ROOT...
## importing ROOT... [DONE]
## opening file [OK]
File:atlfast.aod.pool
Size:    56164.401 kb
Nbr Events: 2000

================================================================================
     Mem Size       Disk Size        Size/Evt      items  (X) Container Name (X=Tree|Branch)
================================================================================
    3662.681 kb      196.654 kb        0.098 kb     2000  (T) DataHeader
--------------------------------------------------------------------------------
      75.394 kb        1.835 kb        0.001 kb     2000  (B) MissingET_AtlfastMissingEt
     117.521 kb        4.528 kb        0.002 kb     2000  (B) TruthParticleContainer_p5_SpclMC
     430.735 kb       13.803 kb        0.007 kb     2000  (B) EventInfo_p2_McEventInfo
     185.332 kb       22.253 kb        0.011 kb     2000  (B) INav4MomAssocs_p1_AtlfastMcAodAssocs
     349.459 kb       32.290 kb        0.016 kb     2000  (B) PhotonContainer_p1_AtlfastPhotonCollection
     411.476 kb       39.623 kb        0.020 kb     2000  (B) ElectronContainer_p1_AtlfastElectronCollection
     664.601 kb       64.972 kb        0.032 kb     2000  (B) MuonContainer_p1_AtlfastNonIsoMuonCollection
     687.872 kb       68.858 kb        0.034 kb     2000  (B) MuonContainer_p1_AtlfastMuonCollection
    3076.368 kb      861.268 kb        0.431 kb     2000  (B) ParticleJetContainer_p1_AtlfastParticleJetContainer
  138092.949 kb    53942.971 kb       26.971 kb     2000  (B) McEventCollection_p3_GEN_AOD
================================================================================
  147754.388 kb    55249.055 kb       27.625 kb     2000  TOTAL (POOL containers)
================================================================================
## Bye.
@endcode

  - @c magnifyPoolFile.py : even if the @c checkFile.py is rather useful it relies on ROOT to provide sensible informations. It may happen however that (for ROOT technical reasons) the reported disk sizes be inaccurate. This ROOT shortcoming can be worked around via the @c magnifyPoolFile.py script which will create a new POOL file whose content is the same than an input POOL file, only being many times replicated. Informations displayed by @c checkFile.py for this magnified POOL file should be more accurate.
@code
lxplus> magnifyPoolFile.py --help
lxplus> magnifyPoolFile.py 100 my.input.pool magnified.pool
lxplus> checkFile.py magnified.pool
@endcode

  - @c diffPoolFiles.py : little script to compare the content of 2 POOL files. It will check that 2 given POOL files have the same container names and for each matching container name, will check that the memory sizes are the same.
@code
lxplus> diffPoolFiles.py --help
lxplus> diffPoolFiles.py mc1.event.pool mc2.event.pool
## opening file [mc1.event.pool]...
## importing ROOT...
## importing ROOT... [DONE]
## opening file [OK]
## opening file [mc2.event.pool]...
## importing ROOT...
## importing ROOT... [DONE]
## opening file [OK]
================================================================================
::: Comparing POOL files...
 chk : mc1.event.pool
 ref : mc2.event.pool
--------------------------------------------------------------------------------
::: comparing common content (mem-size)...
 [OK]      430.735 kb                 EventInfo_p2_McEventInfo
 [OK]   196782.670 kb                 McEventCollection_p3_GEN_EVENT
================================================================================
## Comparison : [OK]
@endcode




*/
