/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrkRIO_OnTrackCreator_page 

@image html RIOonTrackCreatorLogo.png

@section TrkRIO_OnTrackCreator_TrkRotCreatorDocIntro Introduction

The RIO_OnTrack Creator encapsulates detector specific calibrations done
at the time of the track reconstruction. For precision detectors, calibrations
to the measured coordinates can sometimes only be made when additional
geometrical information from the track prediction is available. Such
calibrations correct, for example, for wire sagging, signal propagation
time or varying pitch between microstrips. Hence they fully depend on
the detector technology and geometry. The RIO_OnTrack Creator provides
a common interface and a wrapper so that the track fitting code can be
written in a very general and compact way. High flexibility is maintained
by offering a configurable way of including the differen calibrations,
and modularity given by removing any dependence on subdetector code
from the fitting methods. 

@section TrkRIO_OnTrackCreator_TrkRotCreatorDocFromPrdToRot From <nop>PrepRawData to <nop>RIO_OnTrack

The ATLAS EDM distinguishes two types of objects describing measurement hits:
Trk::PrepRawData and, further downstream in the reconstruction chain,
Trk::RIO_OnTrack. While <nop>PrepRawData provides the measured detector
hit as input to any track reconstruction algorithm, the algorithm and
detector-specific modifications such as calibration, ambiguity solutions
and 3D position information are stored in RIO_OnTrack objects.
They are generated during the pattern recognition or fitting and are included
in the resulting Trk::Track, which owns the objects.
The PrepRawData objects themselves are not (allowed to be!) modified by
the tracking algorithms. 

The following diagram shows the packages involved in the RIO_OnTrackCreator.
By producing Trk::RIO_OnTrack objects from <nop>PrepRawData, the creator tool
encapsulates detector specific corrections using a Trk::TrackParameters object
to obtain the necessary extra information about the track prediction.

@image html RIO_OnTrackCreator.jpg


The interface to the calibration tool is defined by a base class,
IRIO_OnTrackCreator, and concrete implementations are available for
the different detectors by inheriting from this base class.
A master tool, RIO_OnTrackCreator, uses those detector specific tools
at run-time and provides an elegant interface implementation that
allows for all subsystems in one tool. 

This interface takes a Trk::PrepRawData and a Trk::TrackParameters object
(and hence their derived classes) as input, and the detector-specific
tools will then create a new <nop>PixelClusterOnTrack,
   <nop>RPC_ClusterOnTrack, <nop>TRT_DriftCircleOnTrack,
... object which is passed back as a pointer
to the corresponding base class, Trk::RIO_OnTrack.

The Trk::RIO_OnTrackCreator package also contains a tool to correct the
intrinsic measurement errors centrally for all track reco code.
The corrections - a factor and a term added in quadrature - can tune
the tracking with respect to residual miscalibration and misalignments,
respectively. They are provided centrally by two COOL folders,
/Indet/TrkErrorScaling and /MUON/TrkErrorScaling the scaling is performed
by the detector-specific tools at the time of making the <nop>ClusterOnTrack
and <nop>DriftCircleOnTrack objects. The scale factors are distinguished
by sub-detector and barrel/endcap devices. To avoid wasting computing time,
the tool is not called if the job has been started without data from
the COOL folder, and the intrinsic errors are left unchanced.


The following calibration corrections are currently implemented: 
- <b>Pixel:</b> correct pixel cluster positions using time-over-threshold
  information<br>
  The package is <tt><b>SiClusterOnTrackTool</b></tt>
  in InnerDetector/InDetRecTools
(<a href="http://isscvs.cern.ch/cgi-bin/viewcvs-all.cgi/offline/InnerDetector/InDetRecAlgs/SiClusterOnTrackTool/?cvsroot=atlas">CVS</a> link).
<br>&nbsp;
- <b>SCT:</b> forward modules: makes the error a function of the local
       pitch and rotates cov matrix by stereo angle.<br>
  The package is <tt><b>SiClusterOnTrackTool</b></tt>
  in InnerDetector/InDetRecTools
(<a href="http://isscvs.cern.ch/cgi-bin/viewcvs-all.cgi/offline/InnerDetector/InDetRecAlgs/SiClusterOnTrackTool/?cvsroot=atlas">CVS</a> link).
<br>&nbsp;
	- <b>TRT:</b> boundary check, set z position in barrel+forward, solves L-R ambiguity. A separate tool is available to create TRT measurements
  which do not use the drift time at all.<br>
  The package is <tt><b>TRT_DriftCircleOnTrackTool</b></tt> in
  InnerDetector/InDetRecTools
(<a href="http://isscvs.cern.ch/cgi-bin/viewcvs-all.cgi/offline/InnerDetector/InDetRecAlgs/TRT_DriftCircleOnTrackTool/?cvsroot=atlas">CVS</a> link).
<br>&nbsp;
- <b>MDT:</b> call MDT calibration service, set global wire position,
  resolve left-right ambiguity.<br>
The package is <tt><b>MdtDriftCircleOnTrackCreator</b></tt>
in MuonSpectrometer/MuonReconstruction/MuonRIO_OnTrackCreators
(<a href="http://isscvs.cern.ch/cgi-bin/viewcvs-all.cgi/offline/MuonSpectrometer/MuonReconstruction/MuonRIO_OnTrackCreators/MdtDriftCircleOnTrackCreator/?cvsroot=atlas">CVS</a> link)
     <br>&nbsp;
- <b>CSC,RPC,TGC:</b> none (copies PRD into RIO_OnTrack)<br>
  The package is <tt><b>MuonClusterOnTrackCreator</b></tt>
in  MuonSpectrometer/MuonReconstruction/MuonRIO_OnTrackCreators
(<a href="http://isscvs.cern.ch/cgi-bin/viewcvs-all.cgi/offline/MuonSpectrometer/MuonReconstruction/MuonRIO_OnTrackCreators/MuonClusterOnTrackCreator/?cvsroot=atlas">CVS</a> link)
 


@section TrkRIO_OnTrackCreator_TrkRotCreatorDocUsage Usage and jobOptions
Since this generic tool and the detector-specific dependants derive
from the same interface, the steps to integrate ROT creators are common for
both.<br>
Add to the requirements:
<pre>
  use TrkToolInterfaces TrkToolInterfaces-* Tracking/TrkTools
</pre>

add to header file
<pre>
  #include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
  private:
  ToolHandle<Trk::IRIO_OnTrackCreator> m_rotcreator;
</pre>

pre-initialised this tool-handle in your code as:
<pre>
  m_rotcreator("Trk::RIO_OnTrackCreator/RIO_OnTrackCreator")
</pre>

This package follows Gaudi-Configurable standards and therefore
a RIO_OnTrackCreator is passed to client code via
<pre>
    from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
    InDetRotCreator = Trk__RIO_OnTrackCreator(name = 'InDetRotCreator',
                                              Mode = 'indet')
    ToolSvc += InDetRotCreator
</pre>
this <tt>Mode</tt> simplifies configuration by instructing the
Trk::RIO_OnTrackCreator to automatically retrieve the detector-specific
sub-tools. It takes as parameters the tracking detector realm, so
<tt>"all"</tt>, <tt>"indet"</tt> or <tt>"muons"</tt> in case that
it should be restricted to a given tracking system.
The static job options for releases 8 to 12 have been removed.

Two additional python scripts are provided for creating a local
SQlite DB, so that the error scaling can be tested and later submitted
to the central conditions DB.
<ul>
<li><b>make_IndetTrkError.py</b><br>
     writes the scaling values for Pixel, SCT, TRT to local db.
</li>
<li><b>make_MuonTrkError.py</b><br>
     writes the scaling values for MDT, TGC, RPC, CSC to local db.
</li>
<li><b>RIO_OnTrackErrorScaling.py</b><br>
     is obsolete!
</li>
</ul>

This tool is operation since ATLAS offline release 8.3.0.
 

@section TrkRIO_OnTrackCreator_TrkRotCreatorDocDesign Design Issues
    The technical idea behind the <nop>RIO_OnTrack creator tool is to have
    a central, configurable and modular tool to transform Trk::PrepRawData
    objects into Trk::RIO_OnTrack objects. The tool should be fully usable
    in the tracking core, i.e. by algorithms which only operate on
    the EDM base classes, and at the same time not create an explicit
    dependency on detector-specific technologies.

    Therefore a master tool is provided in Tracking, which identifies
    a <nop>PrepRawData object to be Pixel, SCT, ... and calls the
    appropriate detector-specific
    tool at run-time, so that an explicit code dependency on
    the InnerDetector or MuonSpectrometer realms is avoided.
    The detector-specific tools are under the responsibility of
    the sub-groups who take care to provide optimal calibration
    corrections commonly usable to all existing and future tracking
    algorithms. The master tool and the detector-specific tools all
    use the same AlgTool interface and inherit from Trk::IRIO_OnTrackCreator,
    which is in full agreement with the above design goals.

    The main ingredient of providing a detector-technology recognition
    for each <nop>PrepRawData object is the use of AtlasDetectorID
    (as opposed to a dynamic_cast, which would create a compile-time
    dependency). Therefore the offline identifiers provided by
    the <nop>PrepRawData object have to be known to the AtlasDetectorID.

    The interface class and the master tool are both contained in
    the same package. This choice is only motivated by the lack
    of an extra package to host the interfaces of tracking tools,
    at the time of deploying the <nop>RIO_OnTrack creation.

@author
    Wolfgang Liebig http://consult.cern.ch/xwho/people/54608

@section TrkRIO_OnTrackCreator_ExtrasTrkRotCreator Extra Pages

*/
