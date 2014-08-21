/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage The TrkExTools package

This is the core package for track extrapolation tools in the new RTF reconstruction scheme.


@section TrkExToolsIntroduction Introduction
The extrapolation process is organised by two different layers:
  - propagation of parameters and covariances: This is done by the Trk::IPropagator AlgTools,
    different types of propagators are available for different magnetic field resp. material setups
  - extrapolation: this includes navigation, material update and makes use of the propagation. The 
    main tool combining this is the Trk::IExtrapolator.
  
@section TrkExToolsToolDescr Description of AlgTools

Following interface classes for AlgTools have are defined in this package:

<table>
<tr>
    <td><b>Interface Class</b></td>
    <td><b>Description</b></td>
</tr>
<tr>
    <td>Trk::IExtrapolator</td>
    <td>Defines the interface of the Trk::Extrapolator AlgTool, a concrete implementation can be found as the Trk::Extrapolator class.
    </td>
</tr>
<tr>
    <td>Trk::IPropagator</td>
    <td>Defines the interface for all Propagator types, each mathematical propagator has to inherit from the Trk::IPropagator class.
    The Trk::IExtrapolator retrieves different implementation of Trk::IPropagator AlgTools.
    </td>
</tr>
<tr>
    <td>Trk::INavigator</td>
    <td>Defines the interface of the Trk::Navigator AlgTool, a concrete implementation can be found as the Trk::Navigator class.
    </td>
</tr>
<tr>
    <td>Trk::IMaterialEffectsUpdator</td>
    <td>Defines the interface of the Trk::MaterialEffectUpdator class, a concrete implementation can be found as the Trk::MaterialEffectsUpdator.
    </td>
</tr>
<tr>
    <td>Trk::IEnergyLossUpdator</td>
    <td>Defines the interface of the Trk::EnergyLossUpdator class, a concrete implementation can be found as the Trk::EnergyLossUpdator.
    </td>
</tr>
<tr>
    <td>Trk::IMultipleScatteringUpdator</td>
    <td>Defines the interface of the Trk::MultipleScatteringUpdator class, a concrete implementation can be found as the Trk::MultipleScatteringUpdator.
    </td>
</tr>
</table>

In addition, following concrete implementations can be found in this scope:
<table>
<tr>
    <td><b>AlgTool</b></td>
    <td><b>Description</b></td>
</tr>
<tr>
    <td>Trk::Extrapolator</td>
    <td>The main AlgTool for extrapolation in the new RTF reconstruction scheme, it retrieves and makes use of Trk::IPropagator,
    Trk::INavigator, Trk::IMaterialEffectsUpdatpor AlgTools. The Trk::Extrapolator can be used in a configured mode or in a strategy pattern design.</td>
</tr>
<tr>
    <td>Trk::Navigator</td>
    <td>The implementation of a Navigation AlgTool, it makes use of the Trk::GeometryBuilder, of different Trk::IPropagator implementations etc.</td>
</tr>
<tr>
    <td>Trk::MaterialEffectsUpdator</td>
    <td>Implementation of an AlgTool retrieving the Trk::EnergyLossUpdator and the Trk::MultipleScatteringUpdator.
    </td>
</tr>
<tr>
    <td>Trk::EnergyLossUpdator</td>
    <td>Implementation of an AlgTool performing a surface-based material effects update covering energy loss effects.
    </td>
</tr>
<tr>
    <td>Trk::MultipleScatteringUpdator</td>
    <td>Implementation of an AlgTool performing a surface-based material effects update covering multiple scattering effects.
    </td>
</tr>
</table>


@section TrkExToolsComments Comments

Please let me know of any errors, or if anything is unclear.
@author Andreas.Salzburger@cern.ch


@section requirements Requirements
The libaray type is a pure component library.
@include requirements
*/

/**
\namespace Trk
A namespace to enclose the new Track class, and related tools.
*/
