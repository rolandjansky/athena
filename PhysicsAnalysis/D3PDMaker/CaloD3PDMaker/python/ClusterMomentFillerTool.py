# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file CaloD3PDMaker/python/ClusterMomentFillerTool.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Helper to create a ClusterMomentFillerTool.
#
# ClusterMomentFillerTool takes a list of (enum, variable) pairs
# describing what to write.  These are packed into a vector<string>.
#
# However, if we try to use a python list containing (int,string) pairs
# for this, Gaudi will barf.  So we use this to convert everything
# in this list to a string.'
#


from CaloD3PDMaker import CaloD3PDMakerConf


class ClusterMomentFillerTool (CaloD3PDMakerConf.D3PD__ClusterMomentFillerTool):
    # SummaryType codes copied from CaloClusterMoment.h.
    # FIXME: get these from a dictionary.
    FIRST_PHI         = 101 #**< First Moment in \f$\phi\f$.*/      
    FIRST_ETA         = 102 #**< First Moment in \f$\eta\f$.*/            
    SECOND_R          = 201 #**< Second Moment in \f$r\f$.*/                  
    SECOND_LAMBDA     = 202 #**< Second Moment in \f$\lambda\f$.*/                  
    DELTA_PHI         = 301 #**< Angular shower axis deviation (\f$\phi\f$) from IP-to-Center. */
    DELTA_THETA       = 302 #**< Angular shower axis deviation (\f$\theta\f$) from IP-to-Center. */
    DELTA_ALPHA       = 303 #**< Angular shower axis deviation from IP-to-Center. */
    CENTER_X          = 401 #**< Cluster Centroid (\f$x\f$). */
    CENTER_Y          = 402 #**< Cluster Centroid (\f$y\f$). */
    CENTER_Z          = 403 #**< Cluster Centroid (\f$z\f$). */
    CENTER_MAG        = 404 #**< Cluster Centroid (\f$\sqrt(x^2+y^2+z^2)\f$). */
    CENTER_LAMBDA     = 501 #**< Shower depth at Cluster Centroid. */
    LATERAL           = 601 #**< Normalized lateral moment. */
    LONGITUDINAL      = 602 #**< Normalized longitudinal moment. */
    ENG_FRAC_EM       = 701 #**< Energy fraction in EM calorimeters. */
    ENG_FRAC_MAX      = 702 #**< Energy fraction of hottest cell. */
    ENG_FRAC_CORE     = 703 #**< Energy fraction of the sum of the hottest cells in each sampling. */
    FIRST_ENG_DENS    = 804 #**< First Moment in E/V. */
    SECOND_ENG_DENS   = 805 #**< Second Moment in E/V. */
    ISOLATION         = 806 #**< energy weighted fraction of non-clustered perimeter cells. */
    SIGNIFICANCE      = 823 #**< Cluster significance. */                                                                                                    
    CELL_SIGNIFICANCE = 824 #**< Cell significance = E/sig of the cell with the largest |E|/sig. */                                                          
    CELL_SIG_SAMPLING = 825 #**< CaloSample of the cell with the largest |E|/sig. */
    EM_PROBABILITY    = 900 #**< classification probability to be em-like. */
    HAD_WEIGHT        = 901 #**< hadronic weight (E_w/E_em). */
    OOC_WEIGHT        = 902 #**< out-of-cluster weight (E_ooc/E_w). */
    DM_WEIGHT         = 903 #**< dead-material weight (E_dm/E_ooc). */
    VERTEX_FRACTION   = 1000 #**< Vertex fraction of this cluster wrt. primary vertex of the event. Calculated in CaloRec/CaloClusterVertexFractionMaker.cxx */
    NVERTEX_FRACTION  = 1001 #**< slightly updated vertex fraction more pile up independent (similar to nJVF) */
    ENG_CALIB_TOT     = 10001 #**< Calibration Hit energy inside the cluster.
    ENG_CALIB_OUT_L   = 10010 #**< Attached Calibration Hit energy
			      #	  outside clusters but inside the calorimeter
			      #	  with loose matching (Angle < 1.0)
			      # */  
    ENG_CALIB_OUT_M   = 10011 #**< Attached Calibration Hit energy
			      #	  outside clusters but inside the
			      #	  calorimeter with medium matching
			      #	  (Angle < 0.5). */  
    ENG_CALIB_OUT_T   = 10012 #**< Attached Calibration Hit energy
			      #	  outside clusters but inside the
			      #	  calorimeter with tight matching
			      #	  (Angle < 0.3). */  
    ENG_CALIB_DEAD_L  = 10020 #**< Attached Calibration Hit energy in
			      #	  dead material with loose matching
			      #	  (Angle < 1.0). */  
    ENG_CALIB_DEAD_M  = 10021 #**< Attached Calibration Hit energy in
			      #	  dead material with medium matching
			      #	  (Angle < 0.5). */  
    ENG_CALIB_DEAD_T  = 10022 #**< Attached Calibration Hit energy in
			      #	  dead material with tight matching
			      #	  (Angle < 0.3). */  

    ENG_CALIB_EMB0         = 10030 #**< Calibration Hit energy inside the cluster barrel presampler. */
    ENG_CALIB_EME0         = 10031 #**< Calibration Hit energy inside the cluster endcap presampler. */
    ENG_CALIB_TILEG3       = 10032 #**< Calibration Hit energy inside the cluster scintillator. */
    ENG_CALIB_DEAD_TOT     = 10040 #**< Attached Calibration Hit energy in dead material. */
    ENG_CALIB_DEAD_EMB0    = 10041 #**< Attached Calibration Hit energy in dead material before EMB0, between EMB0 and EMB1 */
    ENG_CALIB_DEAD_TILE0   = 10042 #**< Attached Calibration Hit energy in dead material between EMB3 and TILE0 */
    ENG_CALIB_DEAD_TILEG3  = 10043 #**< Attached Calibration Hit energy in dead material before scintillator */
    ENG_CALIB_DEAD_EME0    = 10044 #**< Attached Calibration Hit energy in dead material before EME0, between EME0 and EME1 */
    ENG_CALIB_DEAD_HEC0    = 10045 #**< Attached Calibration Hit energy in dead material between EME3 and HEC0 */
    ENG_CALIB_DEAD_FCAL    = 10046 #**< Attached Calibration Hit energy in dead material before FCAL, between FCAL and HEC */
    ENG_CALIB_DEAD_LEAKAGE = 10047 #**< Attached Calibration Hit energy in dead material behind calorimeters */
    ENG_CALIB_DEAD_UNCLASS = 10048 #**< Attached Calibration Hit energy in dead material in unclassified areas of the detector */

    ENG_BAD_CELLS    = 20001
    N_BAD_CELLS      = 20002
    BADLARQ_FRAC     = 20003
    ENG_POS          = 20004
    SIGNIFICANCE     = 20005
    AVG_LAR_Q        = 20006
    AVG_TILE_Q       = 20007
 
    def __init__ (self,
                  name,
                  Moments = [],
                  **kwargs):
        Moments = [str(x) for x in Moments]
        CaloD3PDMakerConf.D3PD__ClusterMomentFillerTool.__init__ \
          (self,
           name,
           Moments = Moments,
           **kwargs)
        
