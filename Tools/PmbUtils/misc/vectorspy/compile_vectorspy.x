#!/bin/bash
if [ "x${LIBUNWIND_CFLAGS}" == "x" -o "x${LIBUNWIND_LDFLAGS}" == "x" ]; then
    AFSLOC=/afs/cern.ch/atlas/project/pmb/sw/libunwind-1.0.1
    if [ -f $AFSLOC/setup.sh ]; then
	echo "NOTICE: LIBUNWIND_CFLAGS and LIBUNWIND_LDFLAGS not set."
	echo " => Using libunwind from $AFSLOC"
	source $AFSLOC/setup.sh
	echo "NOTICE: To get libunwind.so in your LD_LIBRARY_PATH you might need to source this before preloading libvectorspy.so:"
	echo " => $AFSLOC/setup.sh"
    else
	echo "NOTICE: Libunwind LIBUNWIND_CFLAGS and LIBUNWIND_LDFLAGS not set. Attempting to use system installed versions."
	LIBUNWIND_CFLAGS=""
	LIBUNWIND_LDFLAGS="-lunwind"
    fi
fi
g++ -g -Wall -fPIC -c ${LIBUNWIND_CFLAGS} vectorspy.cxx && \
g++  -shared -Wl,-soname,libvectorspy.so vectorspy.o -ldl ${LIBUNWIND_LDFLAGS} -o libvectorspy.so && rm -f vectorspy.o 
