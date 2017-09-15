/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page InDetRecStatistics_page InDetRecStatistics Package
@author Sven.Vahsen@stop.the.spam.cern.ch lauren.tompkins@stop.the.spam.cern.ch

@section InDetRecStatistics_MyPackageIntro Introduction
The Algorithm InDetRecStatistics produces a text summary of track-finding performance at the end of your Athena job. In addition, it can provide an ntuple with detailed tracking information. InDetRecStatistics is automatically configured and executed by InDetRecExample, and will provide statistics on any trackfinder that is turned on there.

@section InDetRecStatistics_MyPackageStatTable Trackfinding Summary Table
You can obtain a sample statistics table by running InDetRecExample/jobOptions.py for any software release or nightly.  Be sure to check the release/nightly recipes pages for any needed tags.  

- <i>Overview</i>: The table is devided into four main sections. The top section labeled <b>Summary</b> lists the total number of objects (tracks, hits etc) that have been analyzed, problematic objects encountered, as well as the TrackCollections and TrackTruthCollections that have been used to obtain the reconstructed tracks and their associated truth information. These input collections are automatically configured by InDetRecExample, depending on which trackfinders are set to run there. The section <b>Ntuple Settings and Cuts</b> shows the value of InDetRecStatistics parameters that affect the ntuple: Whether to create it or not, whether to save detailed hit info, whether to save detailed TRT info, and preselection cuts that determine which tracks are saved to the ntuple. These track preselection cuts are independent of the track selection cuts used for efficiency and fake-rate calculation, described next. The section <b>Cuts for Statistics Table</b> shows the value of cuts used in the efficiency and fake-rate calculation. The bottom section shows track and hit multiplicities, effiencies and fake-rates. This information is shown seperately for each TrackCollection. In the example above each TrackCollection corresponds to a single track-find algorithm: xKalman, iPatRec, and newTrackig.

- <i>Reconstruction efficiencies</i>: Reconstruction efficiencies are calculated by starting from generated tracks, and hit-matching them to reconstructed tracks. Only tracks passing the "Signal" p_T and eta cuts are considered. The generated tracks are devided into three (user-adjustable) eta regions ("barrel", "transition region", and "endcap") and matched to the entire eta range of reconstructed tracks, in order to avoid edge effects. The average number of reconstructed tracks per event in each eta region is shown in the column <b>n/event</b>. If a generated track is the dominant source of more than 20% of the hits belonging to a reconstructed track, then this track is counted as succesfully reconstructed. (20% is just the default "Good reco track" cut, which is user-adjustable.) The resulting track-finding efficiency is given in the column labeled <b>eff</b>. 
    - The efficiency <b>eff</b> is a "physics" efficiency, i.e. it is lowered if the generated particles decay or interact hadronically. It should be near 1.00 for high p_T muons, but will be lower for other particle species and secondary particles. 
    - <b>SEff</b> is <b>Eff</b> restriced to the GenParticle of the first GenEvent, the signal event in events with pileup. In non-pileup events they should be identical. <b>S+Eff</b> is the efficiency for particle in the signal event plus in-time minbias events. (Generally their truth information is keep, and the inner detector has a chance of reconstructing them.)
    - A second measure of track-finding efficiency, intended to exclude these physics effects, is given in the column <b>SEff.pr</b>. This efficiency is calculated by considering only the subset of generated tracks (from the signal event) with production and decay vertices that pass the <b>Primary track start</b> and <b>Primary track end</b> cuts. The default cuts select particles produced inside the innermost pixel layer (the b-layer), that do not decay or interact inelastically until they have passed the 2nd Semiconductor Tracker (SCT) layer. 

- <i>Fake-rates</i>: The fake-rate</i></b> calculation is similar to the effiency calculation, only that here we start with reconstructed tracks, and hit-match them to generated tracks. Any reconstructed track that has less than 90% (70%) of it's hits from a single generated tracks is considered fake or 'low probability' in the column <b>lowProb1</b> (<b>lowProb2</b>). Again these cuts can be set via jobOptions by the user. There is a third class of tracks, for which no truth information is available. (To be precise: for which HepMcParticleLink.isValid()==false.) This can include tracks due to pile-up, pure noise tracks, and tracks where the truth is not kept for some other reason. The rate of such tracks is given in the column <b>noLink</b>. Since the handling of tracks from pile-up, and the retention of truth infomation has been changing with releases, this fake fraction may have different meaning in different releases. This is why we chose to report it seperately.
    - Pileup considerations: in <b>SLowP1</b> and <b>SLowP2</b> both the (truth links from the) numerator and denominator are required to be from the signal event. Note that <b>noLink</b> with pileup events is generally very high and nearly meaningless, as the standard settings for production of pileup events throws away much of the truth information.

-<i>Multiple Matches</i>: There is additionally a category "multM" which is a measure of how often a given generated track is matched to a reconstructed track.  It is defined as (# of reco tracks matched to a single generated particle - 1)/ (# of generated tracks)

@section InDetRecStatistics_MyPackageSettings How to change the InDetRecStatistics settings in InDetRecExample
The setting shown in the table and discussed above can be changed by overriding InDetRecStatistics's default parameters in InDetRecExample's jobOptions.py  InDetRecExample already has InDetRecStatistics configured so all you need to is include at the end of jobOptions.py:

InDetRecStatistics.MyProperty = value;

For example:

InDetRecStatistics.NtupleSaveHits = true;

@section InDetRecStatistics_MyPackageSettings How to run InDetRecStatistics in Your Athena Job

InDetRecStatistics can be easily integrated into your athena job.  First you need a list of your TrackCollection Keys and a list of your Truth Track Collection Keys.  You must have on truth collection for every reconstructed collection and they must already be in store gate.  You also need a Track summary tool configured.  Lastly you need to have an extrapolator in the tool service.  Once you have this set up all you need to is make the necessary changes to share/InDetRecStatistics_fragment.py and include it in your jobOptions. If you'd like to change the properties you can follow the same instructions as for changing them in InDetRecExample. 

@include InDetRecStatistics_fragment.py

@section InDetRecStatistics_MyPackageNtuple Statistics Ntuple
The ntuple contains detailed information on generated tracks, reconstructed tracks, hits. This ntuple is used by the automated validation package InDetRTT to test nightly releases. We also use it for more manual release validation.  In the process of this validation work, we have produced a collection of ROOT scripts that can be used to plot many quantities related to tracking performance: tracking efficiency, fake-rate, pulls, residuals and so on. The scripts can be found in the scripts/ directory; more on scripts below.

Variables in default ntuple:

- <i>Event:</i> -

event_ntracks: number of tracks per event

event_nhits: number of hits per event

event_nmctrack: number of generated tracks per event 

event_eventNumber: event number

event_runNumber: run number

-<i> Reconstructed Tracks </i>-

track_ind: index of track in the full Track Collection.x

track_qoverpt: track charge over pt       

track_eta: track eta             

track_theta: track theta           

track_cot: track cotangent theta             

track_phi: track phi             

track_d0: track impact parameter              

track_z0: track position along beam pipe              

track_error_theta: error on theta taken from track perigee error maxtix     

track_error_d0:  error on impact parameter taken from track perigee error maxtix         

track_error_z0: error on beam pipe position taken from track perigee error matrix        

track_error_phi: error on phi taken from track perigee error maxtix  

track_error_cot:  error on cotangent theta taken from track perigee error maxtix        

track_error_qoverpt:  error on charge/pt taken from track perigee error maxtix    

track_chi2: track fit chi2            

track_dof: track fit number of degrees of freedom             

track_author: track author          

track_hitpattern: bit map of hit layers per track      

track_npixhits: number of pixel hits per track        

track_nscthits: number of sct hits        

track_ntrthits: number of trt hits

track_nhits: number of generated hits           

-<i> Track Summary Information (from Track Summary Tool)</i>-
m_track_nblayer: number of blayer hits  

m_track_nbla_sh: number of blayer shared hits

m_track_npix_sh: number of pixel shared hits

m_track_nsct_sh: number of SCT shared hits

m_track_nbla_out: number of blayer outliers

m_track_nganged: number of ganged hits

m_track_npix_ho: number of pixel holes

m_track_nsct_ho: number of SCT holes
 
m_track_ntrt_highthr: number of high threshold trt hits

m_track_ntrt_highthrout: number of high threshold outliers

-<i> Truth Track Information for Truth Tracks Matching Reconstructed Tracks </i>-        

track_truth_index: index of truth track in generated track collection.     

track_truth_barcode: barcode of particle which created track   

track_truth_pdgid: pdg id of particle that created track     

track_truth_isvalid: is truth particle valid?

track_truth_prob: probability that track match is good  

track_truth_available: if track matched = 1

track_truth_d0: impact parameter of generated track        

track_truth_z0: generated track position along beam pipe        

track_truth_phi: phi of generated track       

track_truth_eta: eta of generated track       

track_truth_theta: theta of generated track     

track_truth_cot: cotangent theta of generated track       

track_truth_qoverpt: generated charge/pt   

track_truth_hasbeginvertex: 1 if generated track has begin vertex, 0 otherwise

track_truth_beginvertexx: x of begin vertex  

track_truth_beginvertexy: y of begin vertex  

track_truth_beginvertexz: z of begin vertex  

track_truth_hasendvertex: 1 if track has end vertex, 0 otherwise  

track_truth_endvertexx: x of end vertex    

track_truth_endvertexy: y of end vertex    

track_truth_endvertexz: z of end vertex    

-<i> Vertex </i>- NtupleSavePV = true

event_reco_pvx: reconstructed x of primary vertex

event_reco_pvy: reconstructed y of primary vertex

event_reco_pvz: reconstructed z of primary vertex

event_true_pvx: generated x of primary vertex

event_true_pvy: generated y of primary vertex

event_true_pvz: generated z of primary vertex

-<i>TRT</i>- NtupleSaveTRT = true

track_nHLhits: number of high threshold hits : currently not filled

track_nLLhits: number of low threshold hits : currently not filled

track_HLratio: ratio of high to low threshold hits : currently not filled

-<i>Reconstructed Hit</i>- NtupleSaveHits = true

hit_type:          

hit_recindex:  event number 
    
hit_globalx: Hit global x position      

hit_globaly: Hit global y position      

hit_globalz: Hit global z position      

hit_dettype: Hit detector type location      

hit_layer: Layer of hit        

hit_isbarrel: if hit is in barrel = 1; otherwise 0     

hit_errorcode:     

hit_phi: phi of hit          

hit_eta: eta of hit          

hit_biasedresidualx: biased residual (no track refit) in x

hit_biasedresidualy: biased residual (no track refit) in y

hit_localxerror: local hit error in x    

hit_localyerror: local hit error in y    

hit_residualx: currently not filled    

hit_residualy: currently not filled   
   
-<i> TRT Hit </i>- NtupleSaveHits && NtupleSaveTRT

hit_driftRadius:  hit drift radius

hit_trt_Residual : Residual of TRT-hit

hit_trt_Rtrack: Local R track prediction      

hit_trt_Pull :  Pull in R of TRT-hit

hit_trt_ErrTrk: local error of track perp. to straw

hit_trt_ErrHit: error of trt hit perp. to straw

hit_trt_highlevel:  

hit_trt_strawlayer: strawlayer of hit

hit_trt_straw: straw of hit

hit_trt_module_phi: module phi (=sector) of hit

hit_trt_straw_x: x position of the wire

hit_trt_straw_y: y position of the wire

hit_trt_drifttime_raw: raw drifttime of TRT driftcircle
  
-<i> Track Truth infrmation for all Truth Tracks <i>    

mctrack_eta           

mctrack_theta         

mctrack_cot           

mctrack_phi           

mctrack_recindex: this variable is incorrect, do not use it

mctrack_recindices: an array of indices of reconstructed tracks which are matched to the truth track

mctrack_recColl_id: an array of track COLLECTION indices of reco tracks matched to truth tracks

mctrack_pdgid         

mctrack_barcode       

mctrack_qoverpt       

mctrack_d0            

mctrack_z0            

mctrack_perigeeok     

mctrack_truth_available: number of reco tracks matched to mctrack

mctrack_truth_probs: array of hit matching fractions to reco tracks which are matched, 
                    the corresponding reco track index is in mctrack_recindices

mctrack_truth_prob: the highest probability of some reco track to match to mctrack (no info on which reco track)

mctrack_hasbeginvertex

mctrack_beginvertexx  

mctrack_beginvertexy  

mctrack_beginvertexz  

mctrack_hasendvertex  

mctrack_endvertexx    

mctrack_endvertexy    

mctrack_endvertexz    

@section InDetRecStatistics_MyPackageNtupleScruots Scripts for Statistics Ntuple

These are found in the /scripts directory, and are designed to run on the statistics ntuple to produce various plots.

- <b>doall.cc</b> - Used to run all other scripts; by default it runs all others.  To run it, type root -b -q 'doall.cc(file,plotdir,plotext,histname,doplot,compile,abseta)'  All arguments are optional, but presumably you will at least wish to specify the input file.  (Without -b you will see the plots being made, including intermediate plots.  This is useful only for debugging usually.)   The arguments are:
   - <i>file</i> - a string containing the path to the input file.  Default is a test file in castor.
   - <i>plotdir</i> - the directory where output plots should go.  Default is "." i.e. the present run directory.
   - <i>plotext</i> - the extension for the plot names.  Default is ".eps" and other extensions have not been tested and may not work; in particular the ROOT "Print" function does not work for gif files in batch (-b) mode.  If you give "" for this argument, no plots are produced.
   - <i>histname</i> - path of a file to be produced containing all histograms.  By default this is "" meaning that no histogram file is produced.
   - <i>doplot</i> - variable that selects only a subset of plots to be done.  By default this is "" meaning that all plots are produced.  Otherwise only a particular plotting script corresponding to the given string is produced.  Meaningful arguments are: eff, fake, vtx, int, res[param], pull[param], narres[param], pull[author], res[author].  Here [param] = phi,pt,cot,d0,z0 and [author] = ip,xk,nt. (i.e. iPatRec, xKalman, or default tracking)   More details on the plots are given in the script descriptions below.
   - <i>compile</i> - if set to 1, the scripts are compiled before running.  This is mostly useful for debugging, and the default is 0.
   - <i>abseta</i> - if set to 1, the res[param], pull[param], and narres[param] plots are done as a function of abs(eta) rather than eta.  Default is 0.
- <b>eff.cc</b> (doplot = "eff") - Makes track-finding efficiency plots as a function of pt, eta, phi; uses default tracking.  See script or plot titles for details of cuts.
- <b>fake.cc</b> (doplot = "fake") - Make track fake rate plots as a function of pt, eta, phi, d0 z0; uses default tracking.  See script for details of cuts.
- <b>interaction.cc</b> (doplot = "int") - Generator interaction positions for electrons and pions in the r-z plane.  For geometry checking.
- <b>pulls.cc</b> (doplot = "pull[author]")  - 1D pull histgrams for each track parameter - (X_reco - X_truth) / sigma_X
- <b>residual.cc</b> (doplot = "res[author]") - 1D hit residual histograms for each layer - x_reco - x_track (and y_reco - y_track for pixels)
- <b>resolution_[param].cc (doplot = "res[param]", "pull[param]", "narres[param]") - average value for parameter resoltuions (pulls) as a function of eta, for abs(eta) < 2.5 (or abs(eta) < 1.5 for narres);  resolution = X_reco - X_truth, pull as above
- <b>vertex.cc</b> (doplot = "vtx") - plot generated, and reconstructed z0 vertex, and the difference 

*/
