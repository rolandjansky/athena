/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page BTagging_page BTagging

Package for b-tagging.

@author Andreas.Wildauer@cern.ch

@section BTagging_introductionBTagging Introduction
The BTagging package is an example package of how to use the new JetTagging structure under
PhysicsAnlaysis/JetTagging for b-tagging. It contains one algorithm - the BJetBuilder - and
one algtool - the BTagTool.

@section BTagging_howitworksBTagging How it works
The BJetBuilder is a prototype algorithm on how to use the BTagTool for b-tagging. For more information
on the BJetBuilder and the BTagTool please refer to the class description. The functionality of the BJetBuilder
and the BTagTool described below can completely be steered via jobOptions. Prototype jobOption files are
included in this package.

The BJetBuilder mainly handles input/output from/to StoreGate (e.g. reads in Jets/ParticleJets, TrackParticles;
and writes a JetTagContainer). Next to that, the only import task is to make sure the Jets
or ParticleJets have TrackParticles associated to them. If it reads in a JetCollection created with a
cone jet or ktjet algorithm it will call the TrackParticleAndJetMerger algtool. If it reads in jets done
with TrackParticles it will only copy them into a new container (this is done because of deletion troubles
with Jets which are in StoreGate and Jets which are not ...).

The BTagTool does the actual work. It can call a Jet truth matching algtool to label the jets with
truth (this info is saved in a TruthInfo object) and can also perform some basic jet selection (number of
tracks e.g.). After that it calls one tag tool after the other as specified in the jobOs. For detailed info
on the tools please refer to PhysicsAnalysis/JetTagging/JetTagTools. At the end the caller (e.g. the BJetBuilder)
has a pointer to a tagged JetTag object. For more details on the JetTag object please refer to
PhysicsAnalysis/JetTagging/JetTagEvent.

@section BTagging_jettagInfoFlowBTagging Info flow in the new JetTagging structure
@image html InfoFlowNew.jpg "JetTagging Information Flow" width=10cm

@section BTagging_packagecontentBTagging Package Contents
BTagging contains the following files/classes:
- BJetBuilder ... algorithm to to handle storegate I/O and to call the BTagTool.
- BTagTool    ... algtool to call the actual tag tools

@section BTagging_steeringOfBJetBuilderBTagging Steering of the BJetBuilder and BTagTools
The BTagging package contains one main jobOptions file called BTagging_jobOptions.py.
In principal, if someone wants to use standard b-tagging one only has to include this
file in his/her jobOptions file. For details on what the jobOptions file does please
have a look into it.

@subsection btaggingflagsBTagging The BTaggingFlags python class
In addition to the jobOptions file the BTagging package contains python class called
BTaggingFlags. The private datamembers of this class can be used to steer the main
jobOptions file from outside without the need of going into the jobO file itself.

In order to use these flags you need to import them. This is done the usual way with the lines:
<PRE>if not 'BTaggingFlags' in dir():
  from BTagging.BTaggingFlags import BTaggingFlags</PRE>
  
The first line makes sure that the flags are not imported twice and the second actually imports the flags.
Every flag described below can be accessed with
<PRE>BTaggingFlags.thisFlag = "something"</PRE>

The following flags can be used before the include line of the BTagging_jobOptions file
<TABLE>
  <TR>
    <TD>Flag</TD> <TD>Type</TD> <TD>Possible Values</TD> <TD>Purpose</TD> 
  </TR>
  <TR>
    <TD>Runmodus</TD> <TD>string</TD> <TD>analysis<BR>reference</TD> <TD>Switch between reference and analysis mode</TD> 
  </TR>
  <TR>
    <TD>
      lifetime1D<BR>
      lifetime2D<BR>
      lifetime3D<BR>
      secVtxFitBU<BR>
      secVtxFitTD
    </TD>
    <TD> bool </TD> <TD> ON/OFF </TD> <TD>Turn on/off tools</TD> 
  </TR>
  <TR>
    <TD>jetFinderBasedOn</TD> <TD>string</TD> <TD>Cells<BR>TrackParticles</TD> <TD>Decide to use jets<br>based on Cells or TrackParticleJets</TD> 
  </TR>
  <TR>
    <TD>doJetTruthMatching</TD> <TD>bool</TD> <TD>ON/OFF</TD> <TD>turn jet truth matching on/off</TD> 
  </TR>
  <TR>
    <TD>doBTagCBNT</TD> <TD>bool</TD> <TD>ON/OFF</TD> <TD>turn BTag CBNT on/off</TD> 
  </TR>
  <TR>
    <TD>doBTagJetCBNT</TD> <TD>bool</TD> <TD>ON/OFF</TD> <TD>in case TrackParticles were used<br>for jets you can turn on an additional CBNT alg.</TD> 
  </TR>
</TABLE> 
    
@section BTagging_jobOptionsBTagging The jobOptions files
There are several jobOptions files in this package:
- BTagging_jobOptions.py      ... the main jobOptions file
- BTagJetFinder_jobOptions.py ... in case jetfinding wants to be done on TrackParticles
- CBNT_BTagJetFinder_jobOptions.py ... jobOptions fragment for the JetTag CBNT
- BTagging_SecVtxTagTD.py     
- BTagging_SecVtxTagBU.py
- BTagging_LifetimeTag1D.py
- BTagging_LifetimeTag3D.py
- BTagging_LifetimeTag2D.py

@section BTagging_referenceHistosBTagging Reference histograms
A standard set of reference histograms is provided by BTaggingUtils. It
has reference histos for background events (2056, \f$ WH \rightarrow \mu \nu uu \f$)
and for signal events (2055, \f$ WH \rightarrow \mu \nu bb \f$).
If running in analysis mode the b tagger needs to find the
reference histograms <i>Bkg_ref.root</i> and <i>Sig_ref.root</i>. The originals
are located in PhysicsAnalysis/BTagging/BTaggingUtils/share and are copied
to the release InstallArea on compiling.<br>


*/
