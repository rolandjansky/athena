#!/bin/bash --norc
#
# Author Paolo Bagnaia <paolo.bagnaia@cern.ch>
# and Elena Solfaroli Camillocci <elena.solfaroli.camillocci@cern.ch>
# [Manual in CreateMDTConfig_readme.sh]
#
# a simple way to produce some standard output files - PB 100108 v 3.1.2
#
# ./CreateMDTConfig_files.sh "file_tag"
#
# e.g.
# ./CreateMDTConfig_files.sh a_detect.config | sed -r s/___dete/"Barrel_A"/
#
# echo "CreateMDTConfig_files.sh for \"$1\"" >> /dev/stderr
# echo "##### $1"
case "$1" in
################################################################################
#### Region/MDT/AllSectors
   'a_detect.config')  cat << E_O_F
    ## DeadElements, ResidualsMean, ResidualsWidth
    ## adc_Fit_, hits_perML_belowADCCUT,
    ## time_Fit_
    hist all_in_dir {
      algorithm = GatherData
      reference = BasicRef
      weight    = 1
   }
    hist A_HitsPerML_extra {
      algorithm = MDTOverview_Station
      display   = LogZ
      reference = BasicRef
      weight    = 1
    }
    hist A_HitsPerML_Inner {
      algorithm = MDTOverview_Station
      display   = LogZ
      reference = BasicRef
      weight    = 1
    }
    hist A_HitsPerML_Middle {
      algorithm = MDTOverview_Station
      display   = LogZ
      reference = BasicRef
      weight    = 1
    }
    hist A_HitsPerML_Outer {
      algorithm = MDTOverview_Station
      display   = LogZ
      reference = BasicRef
      weight    = 1
    }
    hist DeadElements {
      algorithm = GatherData
      display = Draw=(COL),Draw=(TEXT)
    }
    hist TubeEfficiency_extra {
      algorithm = MDTpercent_under_thresh
      display   = LogY
      weight    = 0
    }
    hist TubeEfficiency_Inner {
      algorithm = MDTpercent_under_thresh
      display   = LogY
      weight    = 0
    }
    hist TubeEfficiency_Middle {
      algorithm = MDTpercent_under_thresh
      display   = LogY
      weight    = 0
    }
    hist TubeEfficiency_Outer {
      algorithm = MDTpercent_under_thresh
      display   = LogY
      weight    = 0
    }
    hist t0PerSector_extra {
      algorithm = GatherData
      display   = NoNorm,AxisRange(350.,700.,"Y")
    }
    hist t0PerSector_Inner {
      algorithm = GatherData
      display   = NoNorm,AxisRange(350.,700.,"Y")
    }
    hist t0PerSector_Middle {
      algorithm = GatherData
      display   = NoNorm,AxisRange(350.,700.,"Y")
    }
    hist t0PerSector_Outer {
      algorithm = GatherData
      display   = NoNorm,AxisRange(350.,700.,"Y")
    }
    hist tdriftPerSector_extra {
      algorithm = GatherData
      display   = NoNorm,AxisRange(700.,800.,"Y")
    }
    hist tdriftPerSector_Inner {
      algorithm = GatherData
      display   = NoNorm,AxisRange(700.,800.,"Y")
    }
    hist tdriftPerSector_Middle {
      algorithm = GatherData
      display   = NoNorm,AxisRange(700.,800.,"Y")
    }
    hist tdriftPerSector_Outer {
      algorithm = GatherData
      display   = NoNorm,AxisRange(700.,800.,"Y")
    }
E_O_F
      exit;;
################################################################################
#### Region/MDT/SectorXX/ChamberXXX/Chamber
   'b_chamber.config')  cat << E_O_F
      dir ___cham {
        output = ___dete/MDT/___sect/___cham/Chamber
	hist A_TDC_ML1 {
	  algorithm = MDTTDCspectrum
          weight    = 1
	}
	hist A_TDC_ML2 {
	  algorithm = MDTTDCspectrum
          weight    = 1
	}
	hist B_ChamberHitMultiplicity {
	  algorithm = MDTmultiplicity
          weight    = 0
	}
	hist C_ADC_ML1 {
	  algorithm = MDTADCspectrum
          weight    = 1
	}
	hist C_ADC_ML2 {
	  algorithm = MDTADCspectrum
          weight    = 1
	}
	hist D_ChamberHitsOnSegment {
	  algorithm = MDTmultiplicity
          weight    = 0
	}
	hist a_HitsPerTube {
	  algorithm = MDTcluster_all_chamber_tubes
          weight    = 0
	}
	hist a_HitsPerTubeAdcCut {
	  algorithm = MDTcluster_all_chamber_tubes
          weight    = 2
	}
	hist b_DeadTubeStatus {
	  algorithm = MDTTubeCheck
	  display   = NoNorm,AxisRange(0.,1.1,"Y")
          weight    = 2
	}
	hist b_EfficiencyPerTube {
	  algorithm = MDTTubeCheckError
	  display   = NoNorm,AxisRange(0.,1.1,"Y")
          weight    = 2
	}
        ## a_ChamberHitOccupancy
        ## b_SegResidVsRadius
        ## Residuals
        ## tdc_Vs_adc_mezz_XX
	hist all_in_dir {
	  algorithm = GatherData
	  reference = BasicRef
          weight    = 0
	}
        dir Occupancy {
	  hist all_in_dir {  ## HitsPerTube_MLX_LX
	    algorithm = MDTcluster
	    reference = BasicRef
            output = ___dete/MDT/___sect/___cham/Occupancy
	  }
        }
        dir DeadStatus {
          output = ___dete/MDT/___sect/___cham/DeadStatus
          hist ChamberDeadTubes {
	    algorithm = GatherData
	    reference = BasicRef
	  }
	  hist all_in_dir {  ## DeadTubes_MLX_LX
	    algorithm = MDTTubeCheck
	    reference = BasicRef
	    display = NoNorm,AxisRange(0.,1.1,"Y")
	  }
	}
        dir Efficiency {
	  hist all_in_dir {  ## EffiPerTube_MLX_LX
	    algorithm = MDTTubeCheckError
	    reference = BasicRef
            output = ___dete/MDT/___sect/___cham/Efficiency
	    display = NoNorm,AxisRange(0.,1.1,"Y")
	  }
	}
      } ### End of ___cham
E_O_F
      exit;;
################################################################################
#### Region/MDT/SectorXX/AllChambers
   'c_overview1.config')  cat << E_O_F
      dir OVERVIEW {
        hist all_in_dir {
          algorithm = GatherData
          reference = BasicRef
	}
        hist TDC_AllChambers_Inner {
          algorithm = MDTTDCspectrum_AllChambers
          weight    = 0
        }
        hist TDC_AllChambers_Middle {
          algorithm = MDTTDCspectrum_AllChambers
          weight    = 0
        }
        hist TDC_AllChambers_Outer {
          algorithm = MDTTDCspectrum_AllChambers
          weight    = 0
        }
        hist TDC_AllChambers_extra {
          algorithm = MDTTDCspectrum_AllChambers
          weight    = 0
        }
        hist HitsPerML_Inner {   ##MDTMLoverview
          algorithm = Chi2_per_NDF  
          reference = BasicRef
          weight    = 1
        }
        hist HitsPerML_Middle {   ##MDTMLoverview
          algorithm = Chi2_per_NDF  
          reference = BasicRef
          weight    = 1
        }
        hist HitsPerML_Outer {   ##MDTMLoverview
          algorithm = Chi2_per_NDF  
          reference = BasicRef
          weight    = 1
        }
        hist HitsPerML_extra {   ##MDTMLoverview
          algorithm = Chi2_per_NDF
          reference = BasicRef
          weight    = 1
        }
E_O_F
      exit;;
################################################################################
'd_overview2.config')  cat << E_O_F
        hist t0PerML___type {  ##t0BinsLTThre&t0BinsGTThre
          algorithm = t0tdriftMDTTubeCheckError  
	  display   = NoNorm,AxisRange(350.,700.,"Y")
          weight    = 1
        }
        hist tdriftPerML___type {  ##t0BinsLTThre&t0BinsGTThre
          algorithm = t0tdriftMDTTubeCheckError  
	  display   = NoNorm,AxisRange(700.,800.,"Y")
          weight    = 1
        }
        hist HitsOnSegment___type {
          algorithm = MDTmultiplicity
          reference = BasicRef
          weight    = 0
        }
E_O_F
      exit;;
################################################################################
'e_overview3.config')  cat << E_O_F
        ##
        hist z_HitsVsMezzanine {
          algorithm = GatherData
        }
      } ### End of OVERVIEW
E_O_F
      exit;;
################################################################################
#### Region/MDTvsRPC or MDTvsTGC
'f_trigdaq.config')  cat << E_O_F
    dir ___sect {
      output = ___dete/MDTvs___/___sect
      hist all_in_dir {
        algorithm = GatherData
        #reference histo is mandatory for all_in_dir
        reference = BasicRef
      }
    } ### End of ___sect
E_O_F
      exit;;
################################################################################
'h_refe_algo_thr.config')  cat << E_O_F
#############
# References
#############

# references need not be nested; "default" is an optional name
reference BasicRef {
  file = ___refroot

  ## the next line specifies the reference histogram to have the
  ## same name as the input histogram;  one may also
  ## provide a specific name, for example if the same reference is
  ## to be used for many input histograms
  name = same_name
}


#############
# Algorithms
#############

algorithm MDTADCspectrum {
  libname = libdqm_algorithms.so
  name = MDTADCspectrum
  reference = BasicRef
  thresholds = MDTADCspectrumThre
  # setting the parameters
  thresh = 50
  MinStat = 1500
}

algorithm MDTpercent_under_thresh {
  libname = libdqm_algorithms.so
  name = MDTpercent_under_thresh
  thresholds = MDTpercent_under_threshThre
  # setting the parameters
  Thresh = 0.95
  MinStat = 1000
}

algorithm MDTcluster {
  libname = libdqm_algorithms.so
  name = MDTcluster
  reference = BasicRef
  thresholds = MDTclusterThre
  # setting the parameters
  N_sigma = 3
  MinStat = 1000
}

algorithm MDTcluster_all_chamber_tubes {
  libname = libdqm_algorithms.so
  name = MDTcluster
  reference = BasicRef
  thresholds = MDTcluster_all_chamber_tubesThre
  # setting the parameters
  N_sigma = 3
  MinStat = 2500
}

algorithm MDTMLoverview {
  libname = libdqm_algorithms.so
  name = MDTMLoverview
  reference = BasicRef
  thresholds = MDTMLoverviewThre
  # setting the parameters
  thresh = 10
  MinStat = 2500
  ref = 1
}

algorithm Chi2_per_NDF {
  libname = libdqm_algorithms.so
  name = Chi2Test_Chi2_per_NDF
  reference = BasicRef
  thresholds = Chi2_per_NDFThre
  # setting the parameters
  MinStat = 2500
}

algorithm MDTChi2 {
  libname = libdqm_algorithms.so
  name = MDTchi2
  reference = BasicRef
  thresholds = MDTchi2Thre
  # setting the parameters
  MinStat = 2500
}

algorithm MDTmultiplicity {
  libname = libdqm_algorithms.so
  name = MDTmultiplicity
  reference = BasicRef
  thresholds = MDTmultiplicityThre
  # setting the parameters
  MinStat = 2500
  IsolationThresh = 75  # percentage
  StatisticThresh = 15  # percentage
  NumberOfPeaks = 2
  FirstPeakPosition = 4
  SecondPeakPosition = 7
#  ThirdPeakPosition = -1
  UseRef = 1 #using 1 with the same run as reference it set green flag
}

algorithm MDTOverview_Global {
  libname = libdqm_algorithms.so
  name = MDTOverview_Global
  reference = BasicRef
  thresholds = MDTOverview_GlobalThre
  thresh = 10  # percentage
  MinStat = 5000
  using_ref_for_flag = 0
}

algorithm MDTOverview_Station {
  libname = libdqm_algorithms.so
  name = MDTOverview_Station
  reference = BasicRef
  thresholds = MDTOverview_StationThre
  thresh = 1  # percentage
  MinStat = 5000
  using_ref_for_flag = 1
}

algorithm MDTTDCspectrum_AllChambers {
  libname = libdqm_algorithms.so
  name = MDTTDCspectrum
  reference = BasicRef
  thresholds = MDTTDCspectrumThre
  # setting the parameters
  MinStat = 5000
  chi2 = 100
}

algorithm MDTTDCspectrum {
  libname = libdqm_algorithms.so
  name = MDTTDCspectrum
  reference = BasicRef
  thresholds = MDTTDCspectrumThre
  # setting the parameters
  MinStat = 1500
  chi2 = 20
#  max_shift = 20
}

algorithm MDTTubeCheckError {
  libname = libdqm_algorithms.so
  name = MDTTubeCheckError
  reference = BasicRef
  thresholds = MDTTubeCheckErrThre
  BinThreshold = 0.85
  nErrBin = 5
  LowStatErr = 0.2
  LowStatThre = 0.4
  ReferenceCheck = 1 #0=NO
  MinStat = 10
#  xmin = 1
#  xmax = 30
}

algorithm MDTTubeCheck {
  libname = libdqm_algorithms.so
  name = MDTTubeCheck
  reference = BasicRef
  thresholds = MDTTubeCheckThre
  BinThreshold = 0.3
  LowStatLevel = 0.5
  ReferenceCheck = 1 #0=NO
  MinStat = 10
#  xmin = 1
#  xmax = 30
}

algorithm t0tdriftMDTTubeCheckError {
  libname = libdqm_algorithms.so
  name = MDTTubeCheckError
  reference = BasicRef
  thresholds = t0tdriftMDTTubeCheckErrThre
  BinThreshold = 700
  nErrBin = 5
  ReferenceCheck = 1 #0=NO
  MinStat = 5
#  xmin = 1
#  xmax = 30
}


compositeAlgorithm tdriftBinsLTThre&tdriftBinsGTThre {
  subalgs = Bins_LessThan_Threshold,Bins_GreaterThan_Threshold
  libnames = libdqm_algorithms.so,libdqm_algorithms.so
}
algorithm tdriftBinsLTThre&tdriftBinsGTThre {

  name = tdriftBinsLTThre&tdriftBinsGTThre

  Bins_LessThan_Threshold|BinThreshold = 600
  Bins_LessThan_Threshold|thresholds = tdriftBinsLTThre
  Bins_LessThan_Threshold|MinStat = 1
  Bins_GreaterThan_Threshold|BinThreshold = 800
  Bins_GreaterThan_Threshold|thresholds = tdriftBinsGTThre
  Bins_GreaterThan_Threshold|MinStat = 1
}

compositeAlgorithm t0BinsLTThre&t0BinsGTThre {
  subalgs = Bins_LessThan_Threshold,Bins_GreaterThan_Threshold
  libnames = libdqm_algorithms.so,libdqm_algorithms.so
}
algorithm t0BinsLTThre&t0BinsGTThre {

  name = t0BinsLTThre&t0BinsGTThre

  Bins_LessThan_Threshold|BinThreshold = 200
  Bins_LessThan_Threshold|thresholds = t0BinsLTThre
  Bins_LessThan_Threshold|MinStat = 1
  Bins_GreaterThan_Threshold|BinThreshold = 1000
  Bins_GreaterThan_Threshold|thresholds = t0BinsGTThre
  Bins_GreaterThan_Threshold|MinStat = 1
}


algorithm t0BinsLessThanThreshold {
  libname = libdqm_algorithms.so
  name = Bins_LessThan_Threshold
  # setting the parameters
  BinThreshold = 400
  thresholds = t0BinsLTThre
  MinStat = 3
}

algorithm tdriftBinsLessThanThreshold {
  libname = libdqm_algorithms.so
  name = Bins_LessThan_Threshold
  # setting the parameters
  BinThreshold = 600
  thresholds = tdriftBinsLTThre
  MinStat = 3
}

algorithm GatherData {
  libname = libdqm_algorithms.so
  name = GatherData
  reference = BasicRef
}

algorithm SimpleSummary {
  libname = libdqm_summaries.so
  name = SimpleSummary
}

algorithm WorstCaseSummary {
  libname = libdqm_summaries.so
  name = WorstCaseSummary
}

algorithm WorstCaseYellow {
  libname = libdqm_summaries.so
  name = WorstCaseYellow
}


#############
# Thresholds
#############

thresholds MDTADCspectrumThre {
  limits Limits {
    warning = 0.15
    error = 0.3
  }
}

thresholds MDTpercent_under_threshThre {
  limits Percent {
    warning = 95
    error = 90
  }
}

thresholds MDTclusterThre {
  limits ClusterSize {
    warning = 6
    error = 80
 }
}

thresholds MDTcluster_all_chamber_tubesThre {
  limits ClusterSize {
    warning = 8
    error = 100
 }
}

thresholds MDTMLoverviewThre {
  limits Limits {
    warning = 1
    error = 2
 }
}

thresholds Chi2_per_NDFThre {
  limits Chi2_per_NDF {
    warning = 5000
    error = 10000
 }
}
thresholds MDTchi2Thre {
  limits Limits {
    warning = 0.1
    error = 0.05
 }
}

thresholds MDTmultiplicityThre {
  limits PeakShift {
    warning = 3
    error = 10
 }
}

thresholds MDTOverview_StationThre {
  limits Limits {
    warning = 0
    error = 1
 }
}

thresholds MDTOverview_GlobalThre {
  limits Percentage {
    warning = 90
    error = 90
 }
}

thresholds MDTTDCspectrumThre {
  limits Limits {
    warning = 4
    error = 7
 }
}
thresholds MDTTubeCheckErrThre {
  limits NBins {
    warning = 0 # Max value for green
    error = 1   # Value for red
  }
}

thresholds MDTTubeCheckThre {
  limits NBins {
    warning = 0 # Max value for green
    error = 1   # Value for red
  }
}


thresholds t0tdriftMDTTubeCheckErrThre {
  limits NBins {
    warning = 0 # Max value for green
    error = 1   # Value for red
  }
}

thresholds t0BinsLTThre {
  limits NBins {
    warning = 1
    error = 2
  }
}

thresholds tdriftBinsLTThre {
  limits NBins {
    warning = 1
    error = 2
  }
}

thresholds t0BinsGTThre {
  limits NBins {
    warning = 1
    error = 2
  }
}

thresholds tdriftBinsGTThre {
  limits NBins {
    warning = 1
    error = 2
  }
}
E_O_F
      exit;;
################################################################################
'user_algos.txt')  cat << E_O_F
#
#-pb - 090208 - default
#
/*/*/*/*/*/                     algo_error            -777.
/*/MDT/*/*/Chamber/             WorstCaseSummary    1.
/*/MDT/*/*/Occupancy/           WorstCaseSummary    0.
/*/MDT/*/*/DeadStatus/          WorstCaseSummary    0.
/*/MDT/*/*/Efficiency/          WorstCaseSummary    0.
/*/MDT/*/*/NULL/                WorstCaseSummary    1. # chamberXX
/*/MDT/*/AllChambers/NULL/      WorstCaseSummary    1. 
/*/MDT/*/NULL/NULL/             WorstCaseYellow     1. # sectorXX
/*/MDT/AllSectors/NULL/NULL/    WorstCaseSummary    1. 
/*/MDT/NULL/NULL/NULL/          WorstCaseYellow     1.
/*/NULL/NULL/NULL/NULL/         WorstCaseSummary    0. # Detector
/ALLMDTs/NULL/NULL/NULL/NULL/   WorstCaseSummary    1.
/top_level/NULL/NULL/NULL/NULL/ WorstCaseSummary    1.
/*/MDTvsRPC/*/NULL/NULL/        SimpleSummary       1. 
/*/MDTvsTGC/*/NULL/NULL/        SimpleSummary       1. 
/*/MDTvsRPC/NULL/NULL/NULL/     WorstCaseYellow     0.
/*/MDTvsTGC/NULL/NULL/NULL/     WorstCaseYellow     0.
# end
E_O_F
      exit;;
################################################################################
*)  cat << E_O_F
error error error error in CreateMDTConfig_files.sh called with argument "$1"
E_O_F
      exit;;
esac
exit
