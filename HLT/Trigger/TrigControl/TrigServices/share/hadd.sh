#!/bin/bash
# generate the script for ROOT hadd.C

#script which will be generated in order to do the merging job
sums=thishadd.C

# find out where hadd.C is located
haddmacro=$( dirname $(which hadd.sh))"/hadd.C"
#echo $haddmacro

if [ ! -f $haddmacro ]
    then
    echo "$haddmacro macro could not be found"
    echo "It is expected to be in the same directory as hadd.sh"
    echo "Check your installation!"
    exit 0
fi


# make local copy of the macro
cp $haddmacro .


if [ $# -lt 2 ]
    then
    echo "Usage: hadd.sh destination source1|source2|..."
    exit 127
fi

output=$1
shift 1

#build the actual script 
echo "{
gROOT->LoadMacro(\"hadd.C+\");
TFile *foutput = TFile::Open(\"$output\",\"NEW\");
if ( foutput == 0 ) {
    cout << \"output file already exist, exitting ...\" << endl;
    gROOT->ProcessLine(\".q\");
}
TList *inputs = new TList();

" > $sums



echo "INPUT FILES: " `ls $@` 
echo "OUTPUT FILE: "  $output

# trick to expand wildcards
for f in `ls $@`
do
  echo "inputs->AddLast( TFile::Open (\"$f\") );" >> $sums
done

echo "
MergeRootfile(foutput , inputs);
foutput->Close();
gROOT->ProcessLine(\".q\");
}
" >> $sums
echo "EXECUTING ROOT to merge files: "
root -l -b $sums
# do cleanup
rm -f $sums hadd.C
echo "done"