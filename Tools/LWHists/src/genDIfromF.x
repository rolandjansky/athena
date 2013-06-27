#!/bin/bash
for d in ./ and ../LWHists/; do
    for H in TH1 TH2; do
	for T in D I; do
	    for suf in h cxx; do
		SOURCEBASE="${H}F"
		TARGETBASE="${H}${T}"
		SOURCE="${d}${H}F_LW.${suf}"
		TARGET="${d}${H}${T}_LW.${suf}"
		SOURCETYPE=float
		TARGETTYPE=double
		if [ "x$T" == "xI" ]; then
		    TARGETTYPE=int
		fi
		if [ -f $SOURCE ]; then
		    echo "Autogenerating $TARGET from $SOURCE (changing $SOURCEBASE -> $TARGETBASE and $SOURCETYPE -> $TARGETTYPE)"
		    if [ -f $TARGET ]; then
			TARGETBACK="${TARGET}_backup"
			while [ -f $TARGETBACK ]; do
			TARGETBACK="${TARGETBACK}_"
			done
			cat $SOURCE \
			    |sed 's#float\*#mustbefloat*#g' \
			    |sed 's#float \*#mustbefloat *#g' \
			    |sed 's#'"${SOURCEBASE}"'#'"${TARGETBASE}"'#g' \
			    |sed 's#'"${SOURCETYPE}"'#'"${TARGETTYPE}"'#g' \
			    |sed 's#mustbe'"${TARGETTYPE}"'#float#g' \
			    |sed 's#mustbefloat#float#g' \
			    > tmptarget
			diff -q tmptarget $TARGET >/dev/null
			if [ $? != 0 ]; then
			    mv $TARGET $TARGETBACK
			    mv tmptarget $TARGET
			    echo "  --> Changed"
			else
			    rm -f tmptarget
			    echo "  --> No changes"
			fi
		    fi
		fi
	    done
	done
    done
done
