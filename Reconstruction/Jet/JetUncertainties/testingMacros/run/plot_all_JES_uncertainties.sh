#!/bin/sh
exec=bin/jes_plots.exe
code=code/MakeUncertaintyPlots.C
mkdir -p bin/

flags=$($ROOTSYS/bin/root-config --cflags --glibs)
includes="-I../ -L../StandAlone -I. -lJetUncertainties"

export LD_LIBRARY_PATH=../StandAlone:$LD_LIBRARY_PATH
  # needed for mac
export DYLD_LIBRARY_PATH=../StandAlone:$DYLD_LIBRARY_PATH

cd ../cmt
make -f Makefile.StandAlone
cd -

echo ; echo "Will compile $code"
g++ -o $exec $code $flags $includes && {
    echo "Compilation successful!"
    echo
    echo "Will now run..."
    configs=$(find ../share -name '*config' | awk -F '../share/' '{print $2}')
    for conf in $configs ; do
	[[ $conf = *Multi* ]] && continue
	fn=$(basename $conf)
	mconf=$(ls ../share/${conf/$fn/}Mul*ig | head -1 | awk -F '../share/' '{print $2}')
	[[ -z $mconf ]] && continue
	[[ $conf = *TopPre* ]] && continue
	./$exec $conf $mconf 
    done
} || {
    echo "Compilation failed :("
    echo
}
