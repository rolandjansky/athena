/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage MuonResidualPullCalculators Package

This package extends the residual calculation tools from
the Tracking realm towards MuonSpectrometer-specific
functionality, i.e. it allows to recognise different
eta/phi chamber orientation and stereo angles between strips
in order to calculate the residual correctly.

@author  Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>

@section MuonResidualPullCalculatorsUsage Usage

To have hit residuals and pull calculated for any kind
of hits on (Muon) tracks, please use from the Tracking
realm:
<pre>
  ToolHandle<Trk::IResidualPullCalculator>
</pre>
configured as

<pre>
  from TrkValTools.TrkValToolsConf import Trk__ResidualPullCalculator
  MuonResPullCalculator = Trk__ResidualPullCalculator(
      name = 'MuonResPullCalculator',
      ResidualPullCalculatorForSCT = None,
      ResidualPullCalculatorForRPC = 'Muon::RPC_ResidualPullCalculator/RPC_ResidualPullCalculator',
      ResidualPullCalculatorForTGC = 'Muon::TGC_ResidualPullCalculator/TGC_ResidualPullCalculator')
  ToolSvc += MuonResPullCalculator
</pre>

@section ExtrasMuonResidualPullCalculators Extra Pages

 - @ref UsedMuonResidualPullCalculators
 - @ref RequirementsMuonResidualPullCalculators
*/

/**
@page UsedMuonResidualPullCalculators Used Packages
@htmlinclude used_packages.html
*/

/**
@page RequirementsMuonResidualPullCalculators Requirements
@include requirements
*/
