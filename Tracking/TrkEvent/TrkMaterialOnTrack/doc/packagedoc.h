/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkMaterialOnTrack_page The TrkMaterialOnTrack package
provides the base class Trk::MaterialEffectsBase and its
extensions to handle material effects in the ATLAS tracking EDM

@author Common Tracking SW Group<br>
        W. Liebig, E. Moyse, A. Salzburger <http://consult.cern.ch/xwho/>

@section TrkMaterialOnTrack_IntroTrkMaterialOnTrack Introduction

 The class structure in this package handles information about estimated
 material effects of charged particles in the detector, i.e. energy
 loss and multiple scattering. Since most material effects corrections
 are pure numbers which have no guarantee for consistent interpretation,
 a polymorphic design has been chosen which groups the predicted/measured
 quantities in sub-classes owned by the material effects containers class.
 Its main use is twofold: as information passed through interfaces
 of e.g. material effects calculators and as part of the Trk::Track,
 under which it can be stored in ESD/AOD. Note that material effects
 are never calculated by any of the EDM classes themselves: following
 the ATLAS EDM guidelines, material effects are calculated in
 dynamically configurable tools provided by TrkExTools and TrkDetDescrTools.

 The persistent representation of this package is under
 Tracking/TrkEventCnv/TrkEventTPCnv/.../TrkMaterialOnTrack

@section TrkMaterialOnTrack_ExtrasTrkMaterialOnTrack Extra Pages

*/
