.. -*- mode: rst -*-

Compilation within Athena
=========================

To compile TauDiscriminant for use in Athena::

   cd cmt
   make
   cd ..


Standalone Compilation
======================

Note: this does not require Athena in any way (setup or installed)
Note: that means **DO NOT SETUP ATHENA BEFORE DOING THE FOLLOWING**
(See "Known Problems" below)

To compile a standalone library for use with ROOT and Python::

   cd cmt
   make -f Makefile.Standalone
   cd ..

This produces the shared object lib/libTauDiscriminant.so
Put this shared object in your ``LD_LIBRARY_PATH`` by executing::

   source setup.sh

The library may now be loaded in CINT like this::

   gSystem->Load("libTauDiscriminant.so");

or in Python like this::

   import ROOT
   ROOT.gSystem.Load("libTauDiscriminant.so")

Now you may follow the same procedure as in run/tauid-redo::
    
   import ROOT
   ROOT.gSystem.Load("libTauDiscriminant.so")
   from ROOT import TauID
   reader = TauID.TauIDReader(True)
   # etc.


Recalculate Tau ID on D3PD (ROOT ntuples)
=========================================

Compile TauDiscriminant by one of the two methods above and source setup.sh
Then execute tauid-redo::

   tauid-redo *.root

This will create new D3PDs with a suffix appended to the filenames
which contain branches for the newly calculated discriminants.
For more information execute::

   tauid-redo --help


Known Problems
==============

When running tauid-redo after a standalone compilation it might crash with::

    AttributeError: type object 'TauID' has no attribute 'Types'

This is most likely because you have Athena setup. Don't setup Athena.
Just use the default Python/ROOT installation on your system.


Additional Tools
================

Tools used to train BDTs, apply MVAs to ROOT ntuples, make performance plots,
convert TMVA BDT xml or MethodCuts files into TauDiscriminant format, etc.
are in the "taumva" package. taumva is now available from TauDiscriminantTools.
