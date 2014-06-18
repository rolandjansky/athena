#!/bin/sh

if [ $# -lt 5 ]; then 
  echo "usage: merge.sh maindir outputdir run_string Nsubregions region [sublist]"
  echo
  echo ./merge.sh $FTKDIR/disk06a/ output raw_7L_TestAlbe2 16 $reg $sub
  exit -1;
fi
if [ \(  $5 -lt 0 \) -o \( $5 -ge 8 \) ]; then
  echo "usage: merge.sh maindir run_string Nsubregions 0<=region<8 [sublist]"
  exit -1;
fi

#MAINDIR=$FTKDIR/disk06a/MERGE
#RUN_STRING=raw_7L_12x10x36_mergePatt
#REAL_NSUBS=128

MAINDIR=$1
#OUTDIR=$MAINDIR/output/
OUTDIR=$2
RUN_STRING=$3
REAL_NSUBS=$4
REGION=$5
PREFIX=patterns_${RUN_STRING}_${REAL_NSUBS}M_reg$REGION


#_GCON='gcon.bz2'

echo $REAL_NSUBS
echo $PREFIX
echo
echo

realNsubsMinus1=`expr $REAL_NSUBS - 1`
#realNsubsMinus1=0
SUBLIST=`bash -c "echo {0..$realNsubsMinus1}"`
if [ $# -ge 6 ]; then 
    shift;     shift;     shift;     shift;     shift;
    SUBLIST=$@
fi

echo SUBLIST is $SUBLIST

#use a non existing file name if $SUBLIST is empty
NEW_FILE=MISSING_FILE.ERROR

for sub in $SUBLIST; do
    NMERGED=0;
    NtoMERGE=0;
    MERGELIST=""

    for qwe in $OUTDIR/${PREFIX}_sub${sub}_*; do 
	file ${qwe} | grep -q bz2 && bunzip2 -d ${qwe};
	#check files size and existence
	UNZIPPED_FILENAME=`echo $qwe | sed -e "s,\.patt.bz2,.patt,"`
	echo UNZIPPED_FILENAME=$UNZIPPED_FILENAME
	ls -lsh $UNZIPPED_FILENAME
	head -n3 $UNZIPPED_FILENAME
	tail -n3 $UNZIPPED_FILENAME
	if [ -s $UNZIPPED_FILENAME ]; then
	    NtoMERGE=$(( $NtoMERGE + 1 ))
	    MERGELIST="${MERGELIST}_"`echo $qwe | sed -e "s,.*_run,," | sed -e "s,.patt.*,,"`;
	    if [ $NtoMERGE -ge 7 ]; then
		echo merge ./ftk.py -I ./ftk.py -q silent -m$REAL_NSUBS -a oldpatmerge${REGION}_${sub}_${MERGELIST} -t $MAINDIR/ -r $OUTDIR -d ${RUN_STRING}_mergePatt; # -o _GCON=$_GCON;
		./ftk.py -I ./ftk.py -q silent -m$REAL_NSUBS -a oldpatmerge${REGION}_${sub}_${MERGELIST} -t $MAINDIR/ -r $OUTDIR -d ${RUN_STRING}_mergePatt; # -o _GCON=$_GCON;
#	    rm $OUTDIR/${PREFIX}_sub${sub}_run$(( 1000000 + $NMERGED)).patt;
		for NRUN in `echo $MERGELIST | sed -e "s,_, ,g"`; do 
		    rm $OUTDIR/${PREFIX}_sub${sub}_run${NRUN}.patt;
		done
		NMERGED=$(($NMERGED + 1));
		NEW_FILE=${PREFIX}_sub${sub}_run$(( 1000000 + $NMERGED)).patt;
		if [ ! -s $OUTDIR/${PREFIX}_sub${sub}.patt ]; then
		    echo ERROR merge.sh: missing output file $OUTDIR/${PREFIX}_sub${sub}.patt;
		    tail -n 100 patt/run/${RUN_STRING}_mergePatt/patmerge/${REGION}/${sub}/stdout.*.log
		    tail -n 100 patt/run/${RUN_STRING}_mergePatt/patmerge/${REGION}/${sub}/stderr.*.log
		    echo exiting with error code -1;
		    exit -1;
		fi;
		mv -i $OUTDIR/${PREFIX}_sub${sub}.patt $OUTDIR/$NEW_FILE
		echo
		ls -l $OUTDIR/
		echo  ls -l $OUTDIR/$NEW_FILE
		ls -l $OUTDIR/$NEW_FILE
		head -n2 $OUTDIR/$NEW_FILE
		du -sch *
		echo
		
		NtoMERGE=0;
		MERGELIST="_"$(( 1000000 + $NMERGED ));
		echo;
	    fi; # end of "if [ $NtoMERGE -ge 7 ]" 
	    echo > /dev/null
	fi; #enf of "check files size >0"
    done;
    if [ $NtoMERGE -gt 0 ]; then
	    echo merge ./ftk.py -I ./ftk.py -q silent -m$REAL_NSUBS -a oldpatmerge${REGION}_${sub}_${MERGELIST} -t $MAINDIR/ -r $OUTDIR -d ${RUN_STRING}_mergePatt; # -o _GCON=$_GCON;
	    ./ftk.py -I ./ftk.py -q silent -m$REAL_NSUBS -a oldpatmerge${REGION}_${sub}_${MERGELIST} -t $MAINDIR/ -r $OUTDIR -d ${RUN_STRING}_mergePatt; # -o _GCON=$_GCON;
	    rm $OUTDIR/${PREFIX}_sub${sub}_run$(( 1000000 + $NMERGED)).patt;
	    for NRUN in `echo $MERGELIST | sed -e "s,_, ,g"`; do 
		rm $OUTDIR/${PREFIX}_sub${sub}_run${NRUN}.patt;
	    done
	    NMERGED=$(($NMERGED + 1));
	    NEW_FILE=${PREFIX}_sub${sub}_run$(( 1000000 + $NMERGED)).patt;
	    if [ ! -s $OUTDIR/${PREFIX}_sub${sub}.patt ]; then
		echo ERROR merge.sh: missing output file $OUTDIR/${PREFIX}_sub${sub}.patt;
		tail -n 100 patt/run/${RUN_STRING}_mergePatt/patmerge/${REGION}/${sub}/stdout.*.log
		tail -n 100 patt/run/${RUN_STRING}_mergePatt/patmerge/${REGION}/${sub}/stderr.*.log
		echo exiting with error code -1;
		exit -1;
	    fi;
	    mv -i $OUTDIR/${PREFIX}_sub${sub}.patt $OUTDIR/$NEW_FILE
	    echo
	    ls -l $OUTDIR/
	    echo  ls -l $OUTDIR/$NEW_FILE
	    ls -l $OUTDIR/$NEW_FILE
	    head -n2 $OUTDIR/$NEW_FILE
	    du -sch *
	    echo
    fi;
    echo;
    echo produced file $OUTDIR/$NEW_FILE;
    echo;
    ls -l $OUTDIR/$NEW_FILE
    NPATT=`head -n 1 $OUTDIR/$NEW_FILE  | cut -d \  -f 2`
    mkdir -p tmp
    awk 'BEGIN{i=0;nhit[0]=0;npatt[0]=0;};NF==2{L=$2;TOTPATT=$1;printf "TOTPATT=%d L=%d\n",TOTPATT,L;};(NF==L+3){NHIT=$NF; if (0){printf "%d  \n", NHIT}; if (NHIT!=nhit[i]) { if (nhit[i]!=0) { npatt[i+1]=$1; if(0){printf "i=%d  nhit[i]=%d  npatt[i+1]=%d\n", i, nhit[i], npatt[i+1]}; i++; nhit[i]=NHIT; }; if (i==0) { nhit[i]=NHIT; };  }; };END{npatt[i+1]=TOTPATT; for (j=0;j<=i;j++) { printf "nhit=%4d\tnpatt=%8d\tdelta=%8d\tfrac=%f\n",nhit[j],npatt[j+1], npatt[j+1]-npatt[j], 1.*(npatt[j+1]-npatt[j])/TOTPATT };}' $OUTDIR/$NEW_FILE > tmp/countPatternHits
    cat tmp/countPatternHits
    echo
    echo INFO: NPATT=$NPATT
    echo INFO: `tail -n 3 tmp/countPatternHits | head -1`
    echo INFO: `tail -n 2 tmp/countPatternHits | head -1`
    echo INFO: `tail -n 1 tmp/countPatternHits | head -1`
done    

if [ ! -s $OUTDIR/$NEW_FILE ]; then
    echo ERROR merge.sh: missing output file $OUTDIR/$NEW_FILE;
    echo exiting with error code -1;
    exit -1;
fi;


