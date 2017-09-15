/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetTrackScoringTools_page InDetTrackScoringTools Package

@author Martin Siebel <Martin.Siebel@CERN.ch>

@section InDetTrackScoringTools_MyTestIntro Introduction

This Package contains the InDetAmbiScoringTool which is mainly used in the Ambiguity Processing step of the Inner Detector Track Reconstruction. The Ambiguity Processor needs to assign a score to every track candidate to reflect its quality. The score is provided by this Tool. 

The score is obtained by invoking the method score(). This method created a Trk::TrackSummary of the Trk::Track in question and invokes in turn the method simpleScore() with this summary. 
The arguments of the score method are a pointer to the track in question and a boolean which allows to deactivate the hole-search procedure of the InDet::InDetTrackSummaryHelperTool 
in order to save time. However, this also reduces the reliability of the obtained track score.

The invoked simpleScore() method applies first some basic cuts on the track in question. If the track fails to fulfill the cuts a Trk::TrackScore of zero is returned. After these cuts, the
track score is calculated. For this two methods are provided: The intrinsic scoring of simpleScore() and the score provided by the ambiScore() method. Which of these scores is 
returned is governed by a boolean property, useAmbigFcn, which activates the ambiScore() method.

The intrinsic score of simpleScore() is mainly based on a basic score obtained from the chi2-probability of the track fit. This basic score is then modified by adding several
Trk::TrackSummary counts (# of hits, # of holes, ...) times a modifying factor specific for this track summary information. Obviously, the modifying factors for good properties 
(# of hits) are positive and increase the score, while the modifying factors for bad properties (# of holes) are negative and reduce the score. Note, that this scoring is 
by construction  strictly proportional to the corresponding track summary counter. The values of these scores are nearly not restricted, they can be positive as well as --for very bad track candidates -- negative.

The ambiScore() method returns a score which is based on likelihood ratios. This score is always positive of zero. A score larger than one indicates a rather good track candidate, 
while a score between zero and one indicates a rather bad track candidate. Starting with a score of 1., the score is multiplied by a modifier for every considered observable 
(i.e. # odf hits, # of holes, P(chi2), ....).  The modifiers are calculated during the initialisation phase of the tool by the method setupScoreModifiers. They are calculated by 
the use of normalised histograms of the given observable by dividing the contents of a given bin in the respective histogram for good tracks by the corresponding bin content for bad tracks. These histograms have been produced using the Package InDet::InDetAmbiProcStatistics in InDetValidation. In this way, each modifier is a ratio of the likelihood that a given track with a certain observable value (e.g. # of hits) is a good track divided by the likelihood that it is a bad track. 

There are two exceptions to this approach: 

a) Hits in the TRT are not relevant for the ambiguity processing in the InDet reconstructions, because the track candidates have only Si hits. However, the scoring Tool is also used for the decision if a given ResolvedTrack schould be extended into the TRT. In order to make this work properly, TRT hits are in general taken into account if they are present. The presence of TRT hits automatically increases the score by a fixed bonus multiplier. After this the score is multiplied by the corresponding modifier for the given number of TRT hits, taken to the power of 2.5 in order to give it more emphasise.

b) In the case that a given Track has hits on the first and the third Pixel layer, two track candidates could be formed: One including the innermost hit and one excluding it. The inclusion of the innermost pixel hit automatically introduces also a hole (the missing second layer hit) to the track. The score of a track with two pixel hits and a hole is smaller than the score of a track with only one pixel hit but no holes. This would generally prefer the track without the b-layer information and therefore have a negative impact on the impact parameter measurement. Therefore, a correction is applied which penalizes the solution without the b-Layer hit in this situation.




*/
