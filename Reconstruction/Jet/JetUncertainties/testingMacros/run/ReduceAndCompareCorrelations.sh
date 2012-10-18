#!/bin/sh
# Takes an input configuration file, reduces it, and generates correlation matrices for comparison as specified


# Paths and directories of use, should only need to be set once
startPath=`pwd`
basePath="$startPath/../.."
JESprovPath="$basePath/JetUncertainties"
JESsharePath="$JESprovPath/share"
ReduceToolPath="$basePath/ToolsNuisanceParameters"
ComparisonPath="$JESprovPath/testingMacros"


# Compilation executuables, source, and flags, should only need to be set once
flags=$($ROOTSYS/bin/root-config --cflags --glibs)
includes="-I$JESprovPath -L$JESprovPath/StandAlone -lJetUncertainties"

if [[ ! $LD_LIBRARY_PATH = *$JESprovPath/StandAlone* ]] ; then
  export LD_LIBRARY_PATH=$JESprovPath/StandAlone:$LD_LIBRARY_PATH
fi
if [[ ! $DYLD_LIBRARY_PATH = *$JESprovPath/StandAlone/* ]] ; then
  export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$JESprovPATH/StandAlone/ # For mac
fi

reduceExec="np_reduction.exe"
reduceSource="ReduceNP.C"

compareExec="jes_corr_compare.exe"
compareSource="CompareCorrelations.C"

combineFilesExec="CombineRootAndConfigFilesAfterReduction.py"


# Control variables, change as desired for each run
numberOfReducedParam=5  # Number does not include residual term
                        # "5" would give 5 reduced + 1 residual + 5 non-nuisance (such as eta calib) = 11 parameters
smoothing=1
algorithms="AntiKt4TopoEM,AntiKt4LCTopo,AntiKt6TopoEM,AntiKt6LCTopo" #Comma-separated list of jet algorithms to run
tempRootFile="ReducedNP_file.root" # Reduced param file, to be combined with the others described above, then deleted
MCtype="MC11b" # MC uncertainty to use for comparisons


# Config and root file variables, change as desired for each run
fileNameSuffix="AlternativeCorrelation3"

configOnlyNPfile="InsituJES2011_${fileNameSuffix}_OnlyNP.config"
configFileToReduce="InsituJES2011_${fileNameSuffix}.config"
configFileReduced="InsituJES2011_$(($numberOfReducedParam+6))NP_$fileNameSuffix.config"
configFileForComparison="InsituJES2011_11NP_Preliminary.config"

rootFileToReduce="May14_AllNuisanceParameters.root"
rootFileReduced="JESUncertainty_$(($numberOfReducedParam+6))NP_$fileNameSuffix.root"
rootFileForComparison="JESUncertainty_Preliminary.root"

# configOnlyNPfile="InsituJES2011_${fileNameSuffix}_OnlyNP.config"
# configFileToReduce="InsituJES2011_${fileNameSuffix}.config"
# configFileReduced="InsituJES2011_$(($numberOfReducedParam+6))NP_$fileNameSuffix.config"
# configFileForComparison="InsituJES2011_${fileNameSuffix}.config"
# 
# rootFileToReduce="JESUncertainty_Preliminary.root"
# rootFileReduced="JESUncertainty_$(($numberOfReducedParam+6))NP_$fileNameSuffix.root"
# rootFileForComparison="JESUncertainty_Preliminary.root"


# Useful methods
linkfile() {
  if test $# -eq 2 ; then
    if [[ ! -e $1 ]] ; then
      ln -s $2/$1
    fi
    ls $1
  else
    "Wrong number of arguments to linkfile"
  fi
}

linkFileFromShare() {
  linkfile $1 $JESsharePath
}

makeDir() {
  if [[ ! -d $1 ]] ; then
    mkdir $1
  fi
}


# Compile the provider and executables
cd $JESprovPath/cmt
make -f Makefile.Standalone && {
  echo "Successfully compiled JES Provider"
  cd -
} || {
  echo "Failed to compile JES Provider.  Exiting..."
  cd -
  exit -1
}

cd $ReduceToolPath
g++ -o $reduceExec $reduceSource $flags $includes && {
  echo "Successfully compiled reduction code"
  cd -
} || {
  echo "Failed to compile reduction code.  Exiting..."
  cd -
  exit -2
}

cd $ComparisonPath
g++ -o $compareExec $compareSource $flags $includes && {
  echo "Successfully compiled comparison code"
  cd -
} || {
  echo "Failed to compile comparison code.  Exiting..."
  cd -
  exit -3
}
echo "Successfully compiled all necessary libraries/programs!  Will now run..."


# Link config/root files, make the expected directory structure, and run the reduction
cd $ReduceToolPath

linkFileFromShare $configOnlyNPfile
linkFileFromShare $rootFileToReduce

makeDir Plots
makeDir SmoothCorrectionFiles
makeDir SmoothCorrectionFiles/ReducedNP

echo -e "\nExecuting reduction program...\n"
./$reduceExec $configOnlyNPfile $numberOfReducedParam $algorithms $tempRootFile $smoothing
if test $? -ne 0 ; then
  echo -e "\nUnsuccessful execution of reduction program.  Exiting.\n"
  exit 1
fi
echo -e "\nFinished executing reduction program!\n"
mv $tempRootFile $ComparisonPath/.


# Link config/root files, then create the new root/config files from the old and reduced files
cd $ComparisonPath

linkFileFromShare $configFileToReduce
linkFileFromShare $rootFileToReduce

echo -e "\nExecuting combination program...\n"
python $combineFilesExec $configFileToReduce $rootFileToReduce $tempRootFile $configFileReduced $rootFileReduced $numberOfReducedParam
if test $? -ne 0 ; then
  echo -e "\nUnsuccessful execution of combination program.  Exiting.\n"
  exit 2
fi
echo -e "\nFinished executing combination program!\n"

rm $tempRootFile
mv $configFileReduced $JESsharePath/.
mv $rootFileReduced $JESsharePath/.

# Link config/root files, then create and compare correlation matrices
cd $ComparisonPath

linkFileFromShare $configFileReduced
linkFileFromShare $rootFileReduced
linkFileFromShare $configFileForComparison
linkFileFromShare $rootFileForComparison

echo -e "\nExecuting correlation matrix comparison program...\n"
./$compareExec $configFileForComparison $configFileReduced $algorithms $MCtype
#./$compareExec $configFileReduced $configFileToReduce $algorithms $MCtype
if test $? -ne 0 ; then
  echo -e "\nUnsuccessful execution of comparison program.  Exiting.\n"
  exit 3
fi
echo -e "\nFinished executing correlation matrix comparison program!\n"


# Done!
cd $startPath

