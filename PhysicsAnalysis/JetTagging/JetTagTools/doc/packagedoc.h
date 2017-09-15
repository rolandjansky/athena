/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page JetTagTools_page JetTagTools

Package for tag tool and helper tools for jet tagging.

@author Andreas.Wildauer@cern.ch

@section JetTagTools_introductionJetTagTools Introduction:
JetTagTools is a package which contains algtool(s) to calculate variables relevant
for jet-tagging and also helper tools for easier handling of histograms and likelihood calculations. 

@section JetTagTools_implementedtoolsJetTagTools Algtools:
@subsection tagToolsJetTagTools Jet Tag Tools
Different jet tag tools are implemented:<br>
<ul>
  <li>LifetimeTag1D to 3D</li>
  <li>SecVtxTag build up and tear down</li>
</ul>
All taggers inherit from an ITagTool interface class which specifies the basic interfaces (e.g. the tagJet(...)
method. For more information have a look at the different tag classes.

@subsection helperToolJetTagTools Helper Tools
These helper tools are implemented:
<ul>
  <li>HistoHelper</li>
  <li>LikelihoodTool</li>
  <li>CombinerTool</li>
</ul>
The HistoHelper makes it easier to book and fill histos. In addition it takes
care that all values are filled inside the histogram range (too low value are filled into
the first bin, too high values into the last bin).

The LikelihoodTool helps to calculate a likelihood per tag tool. In addition in takes
care of reading in the histograms (signal + N background)


\subsection bjetlifetimeTag LifetimeTag
The %B lifetime tag method is based on the fact that tracks coming
from long lived particles (as %B's) have a substantially bigger impact
parameter than the ones coming from the primary vertex. The variable to
discriminate between b jets and jets originating from lighter quarks is
constructed by use of the impact parameter, its error and
the so called lifetime sign of the track. The variable itself is the
so called <i>significance</i>:<br>
\f$ significance = sign \times \frac{d_0}{\sigma_{d_0}} \f$<br>
The sign of the significance is as follows:<br>
<ul>
  <li><b>positive</b> if the point of closest approach of the track to the
  jet axis is in the hemisphere given by the jet direction and the primary vertex.</li>
  <li><b>negative</b> if the point of closest approach of the track to the
  jet axis is on the other side.</li>
</ul>
Have a look at the following picture to clarify the sign of the significance:<br>
(blue is the jet axis, green is the primary vertex, red are tracks from a sec.vtx.)<br>
\image html tracksignificance.gif
<br>
Since LifetimeTag is an algtool it does not have an execute() method. All its
methods (except for initialize() and finalize()) can be executed on demand by a client
(e.g. an athena algorithm like BJetBuilder). There are various set and get methods
explained below. However, the main method to do b-tagging is:
<p>
<b>tagJet(const Jet& jetToTag, BJet& jetWithBInfo)</b>
</p>
<br>There are two arguments to this method: %A %Jet object (as input) and an (empty)
BJet object in which results are written.<br>
LifetimeTag calculates and stores the signed impact parameter (signed same as the
significance) and  the significance for each used track. In addition, if in ANALYSIS
mode it also calculates the b-jet likelihood and the so called NTrack probability
(implemented but not working so far).<br>
<b>Other methods:</b>
<ul>
  <li>initialize():<br>
  This method is called automatically at the beginning of the event cycle. It starts
  with a printout of the parameter settings and histograms used for this run. Then, depending
  in which mode the algtool is, it either books reference histograms to be filled later
  (reference mode) or it loads already produced histograms to do likelihood calculations
  aso. (analysis mode).
  </li>
  <li>finalize():<br>
  If in reference mode, it normalizes the produced reference histograms to 1. In principal this
  works but the reference histos seemed to be stored to disk before finalize() is called. Therefore
  it has no effect on the histos on disk ... To be sure, all read in reference histos (when in
  analysis mode) are normalized again (if not already).
  </li>
</ul>

@section JetTagTools_packagecontentJetTagTools Package Contents:
JetTagTools contains the following files/classes:
- LifetimeTag ... algtool to calculate b-tag lifetime variables 
- SecVtxTag ... algtool to calculate b-tag secondary vertex variables 
- Likelihood ... helper class to calculate the likelihood

- share/Bkg.ref ... background reference histos for analysis mode (H->uu)
- share/Sig.ref ... signal reference histos for analysis mode (H->bb)

@section JetTagTools_jobOptionsJetTagTools The jobOptions file:
There are no jobOptions files for this package. Examples how to use these
algtools are given in the package BTaggingAlgs.



*/
