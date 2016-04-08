#!/bin/zsh

if [[ $# -ne 1 ]] ; then
  echo Usage: $0 run_number
  exit -1
fi

noiseFile=/afs/cern.ch/user/i/idcalib/w0/Pixel_Calibration/NoiseMaps/NoiseMap_run${1}_calibration_PixelNoise.root
hitFile=/afs/cern.ch/user/i/idcalib/w0/Pixel_Calibration/OccupancyMaps/OccupancyMap_run${1}_express_express.root
macroFile=checkModules.C
macroDir=$TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/Macro

if [[ ! -r $noiseFile ]] ; then
  echo Not existing NoiseMap file: $noiseFile
  echo Aborting!
  exit -2
fi
if [[ ! -r $hitFile ]] ; then
  echo Not existing OccupancyMap file: $hitFile
  echo Aborting!
  exit -3
fi
if [[ ! -r $macroDir/$macroFile ]] ; then
  echo Not existing macro file: $macroFile
  echo You can retrieve it from the PixelCalibAlgs package:
  echo https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/trunk/Macro
  echo or from $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/Macro/$macroFile
  echo Aborting!
  exit -4
fi

echo In the opening window, call the function 
echo checkModule\(\"modulename\"\) 
echo to display some debugging histograms of a module.
echo The frame will be automatically saved as run${1}_modulename.png

xterm -e root -l $noiseFile $hitFile $macroDir/$macroFile & 
