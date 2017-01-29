
================
SUSYTools
================

**Current Main Developers**: martin.tripiana@cern.ch, zach.marshall@cern.ch

------------------------------------

.. contents:: Table of contents


------------------------------------

------------------------------------
Recommended tags
------------------------------------

**Rel 20.7 Samples :**   Base,2.4.25 + SUSYTools-00-08-41

**Rel 20.1 Samples :**   Base,2.3.8 + SUSYTools-00-07-58


------------------------------------

------------------------
RootCore Setup
------------------------
Check the latest recommended AnalysisBase release::

   setupATLAS
   rcSetup Base,2.4.25

SUSYTools is now shipped with it, but you can also check a specific tag::

   rc checkout_pkg $SVNOFF/PhysicsAnalysis/SUSYPhys/SUSYTools/tags/SUSYTools-00-XX-YY

or the trunk version::

   rc checkout_pkg $SVNOFF/PhysicsAnalysis/SUSYPhys/SUSYTools/trunk
   
Additional packages needed on top of the recommended release are documented in ``doc/packages.txt``, and can be easily checked out as::
   
   rc checkout SUSYTools/doc/packages.txt
	   
Finally, compile everything::

   rc find_packages   
   rc clean   
   rc compile
	 
and you are ready to go!

Testing
--------------

Get your favourite benchmark sample (e.g. mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.DAOD_SUSY1.e3698_s2608_s2183_r7725_r7676_p2596/), and run::
   
   SUSYToolsTester <myAOD.pool.root> 100 isData=0 isAtlfast=0 Debug=0 NoSyst=0 2>&1 | tee log
     
The standard ``SUSYToolsTester`` code is meant to test the configuration of all the relevant CP tools and apply a minimal analysis flow. It includes the recalibration and correction of all physics objects, the recalculation of the MissingET, the extraction of SFs and other specific functionalities that are enabled based on the DxAOD stream. All systematic variations available in the central registry are tested by default, and a cutflow is displayed for each of them. This can be disabled by setting ``NoSyst=1``.

A set of unit tests have been defined in addition, which are regularly checked to ensure the framework is stable against the release evolution. To get a list of the available tests do::

   rc test_ut --list --package=SUSYTools

To run one test (as from the list above) you then do::

   rc test_ut <theTest>

or well just:: 

   rc test_ut --package=SUSYTools

to run all tests in the package (i.e. ~ everything under test/ directory)

------------------------------------

------------------------
CMT Setup
------------------------
Check the latest recommended AthAnalysisBase release::

   setupATLAS
   asetup AthAnalysisBase,2.4.25

SUSYTools is now shipped with it, but you can also check a specific tag::
	  
	  pkgco.py SUSYTools-00-XX-YY

or the trunk version::

   pkgco.py -A SUSYTools
   
Additional packages needed on top of the recommended release are documented in ``doc/packages.txt``, and can be easily checked out as::
	   
	   for file in `cat PhysicsAnalysis/SUSYPhys/SUSYTools/doc/packages.txt | grep -v '#' `; do pkgco.py $(basename $file); done 
	   
Then create a work area and build it::

   setupWorkArea.py
   cd WorkArea/cmt
   cmt config; cmt bro cmt config; cmt bro make

and you are ready to go!

Testing
--------------

Just run the test jobOption as::

   cd ..
   athena.py SUSYTools/minimalExampleJobOptions_mc.py
   
which is the athena-friendly equivalent of the ``SUSYToolsTester`` code above for running on MC.  You can also change "mc" to "data" or "atlfast" to run on data
or fast simulation if you would prefer.


------------------------------------

--------------------------------
Retrieving Cross sections
--------------------------------
All cross-sections are stored in text files in the directories like ``data/mc15_13TeV/``.
The ``Backgrounds.txt`` file contains all cross-sections per background sample (characterized by its dataset (DS) id).
Each signal grid has a corresponding text file with the cross-sections per grid point and per process (characterized by its dataset id and process id (finalState)).

A parser is provided with ``SUSYTools`` to access it::

   root -l $ROOTCOREDIR/scripts/load_packages.C
   root [1] SUSY::CrossSectionDB myDB("SUSYTools/data/mc15_13TeV/");
   root [2] myDB.xsectTimesEff(410000)
   (const float)1.37378997802734375e+02
   root [3] myDB.xsectTimesEff(410000,0)
   (const float)1.37378997802734375e+02
   root [4] myDB.xsectTimesEff(164949, 111)
   (const float)8.08409880846738815e-04
   root [5] myDB.xsectTimesEff(164949, 1000022, 1000022)
   (const float)8.08409880846738815e-04


------------------------------------

--------------------------------
Developers Notes
--------------------------------

To make a new tag do::

   svn cp svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/SUSYPhys/SUSYTools/trunk svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/SUSYPhys/SUSYTools/tags/SUSYTools-XX-YY-ZZ -m "Learning to tag SUSYTools"

and make sure this file is up-to-date with the tags recommendations and extra packages needed.
