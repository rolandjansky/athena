#!/bin/sh
# Takes an input configuration file, reduces it, and generates correlation matrices for comparison as specified


# Paths and directories of use, should only need to be set once
startPath=`pwd`
basePath="${startPath%/JetUncertainties/testingMacros/run}"

JESprovPath="$basePath/JetUncertainties"
JESsharePath="$JESprovPath/share"

ReduceToolPath="$basePath/ToolsNuisanceParameters"
CombinePath="$JESprovPath/testingMacros/python_scripts"
ComparisonPath="$JESprovPath/testingMacros/code"


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


# Global parameters, change as desired for each run
algorithms="AntiKt4TopoEM,AntiKt4LCTopo" #Comma-separated list of jet algorithms to run
#algorithms="AntiKt4TopoEM,AntiKt4LCTopo,AntiKt6TopoEM,AntiKt6LCTopo" #Comma-separated list of jet algorithms to run
MCtype="MC12a" #"MC11b"                      # MC uncertainty to use for comparisons

# Reduction parameters, change as desired for each run
#categoriesToReduce="Statistical=2,Modelling=3,Detector=1,Mixed=1" # Best for 2011 AllNuisanceParameters
#categoriesToReduce="Statistical=2,Modelling=2,Detector=1,Mixed=1" # Best for 2011 StrongerCorrelations
#categoriesToReduce="Statistical=2,Modelling=3,Detector=2,Mixed=1" # Best for 2011 WeakerCorrelations
#categoriesToReduce="Statistical=2,Modelling=3,Detector=2,Mixed=1" # Best for Moriond2013 AllParam
#categoriesToReduce="Statistical=2,Modelling=2,Detector=2,Mixed=1" # Best for Moriond2013 StrongerCorrelations
categoriesToReduce="Statistical=3,Modelling=3,Detector=2,Mixed=3" # Best for Final 2012
#categoriesToReduce="global=5"
numDimForReduction=1
smoothing=1
tempRootFile="ReducedNP_file.root"  # Reduced param file, to be combined with the others described above, then deleted
doComparison=0


# Config and root file variables, change as desired for each run
#fileNamePath="JES_2012/Moriond2013" #"JES_2011/Final"
fileNamePath="JES_2012/Final"
fileNamePrefix="InsituJES2012" #"InsituJES2011"
fileNameSuffix="AllNuisanceParameters" #"StrongerCorrelations"

reductionLinkWords="ReducedFrom"
if [[ $categoriesToReduce = *global* ]] ; then
  reductionLinkWords="GlobalReductionFrom"
else
  reductionLinkWords="CategoryReductionFrom"
fi

configFileToReduce="${fileNamePath}/${fileNamePrefix}_${fileNameSuffix}.config"
#configFileReduced="${fileNamePath}/${fileNamePrefix}_$(($numberOfReducedParam+6))NP_From${fileNameSuffix}.config"
configFileReduced="${fileNamePath}/${fileNamePrefix}_${reductionLinkWords}_${fileNameSuffix}.config"
configFileForComparison=$configFileToReduce

rootFileToReduce="${fileNamePath}/JESUncertainty_BaseComponents_2012.root" #2011.root"
#rootFileReduced="${fileNamePath}/JESUncertainty_$(($numberOfReducedParam+6))NP_From${fileNameSuffix}.root"
rootFileReduced="${fileNamePath}/JESUncertainty_${reductionLinkWords}_${fileNameSuffix}.root"

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



#############
# Reduction #
#############

cd $ReduceToolPath

# Reduction expects a specific directory structure, so make it if necessary
makeDir Plots
makeDir SmoothCorrectionFiles
makeDir SmoothCorrectionFiles/ReducedNP

# Perform the reduction
echo -e "\nRunning the reduction program...\n"
./$reduceExec $JESsharePath $configFileToReduce $tempRootFile $categoriesToReduce $algorithms $MCtype $smoothing $numDimForReduction
if test $? -ne 0 ; then
  echo -e "\n Unsuccessful execution of reduction program.  Exiting...\n"
  exit 1
fi
echo -e "\nFinished the reduction program!\n"

cp $tempRootFile $CombinePath/.


###############
# Combination #
###############

cd $CombinePath

# Perform the combination
echo -e "\nRunning the combination program...\n"
python $combineFilesExec $JESsharePath $configFileToReduce $rootFileToReduce $tempRootFile $configFileReduced $rootFileReduced
if test $? -ne 0 ; then
  echo -e "\nUnsuccessful execution of combination program.  Exiting...\n"
  exit 2
fi
echo -e "\nFinished the combination program!\n"

rm $tempRootFile


##############
# Comparison #
##############


if test $doComparison -ne 0 ; then
  cd $ComparisonPath
  
  echo -e "\nRunning correlation matrix comparison program...\n"
  ./$compareExec $JESsharePath $configFileForComparison $configFileReduced $algorithms $MCtype
  if test $? -ne 0 ; then
    echo -e "\nUnsuccessful execution of comparison program.  Exiting...\n"
    exit 3
  fi
  echo -e "\nFinished executing correlation matrix comparison program!\n"
fi


# Done!
cd $startPath







