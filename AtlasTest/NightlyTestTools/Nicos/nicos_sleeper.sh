#!/bin/sh

quant=600
#quant=1
LIMIT=80
comname=`basename $0`
if [ "$comname" = "nicos_sleeper.sh" ]; then
exitcom="exit"
else
exitcom="return"
fi

if [ "${NICOS_NOWAIT}" = "yes" ]; then
eval ${exitcom} 0
fi

if [ "${NICOS_DOMINANT_PROJECTS}" = "" -a "${NICOS_DOMINANT_PARALLEL}" = "" ]; then
eval ${exitcom} 0
fi

strng="TOOLINIT"
if [ "$#" -eq 1 ]; then
strng=$1; shift
strng_par="$strng"
elif [ "$#" -eq 2 ]; then
strng=$1; shift
strng_par=$1; shift
else
echo "nicos_sleeper: error: sourced with 0 or (3 or more) arguments" 
eval ${exitcom} 1
fi

possible=("CONFIG" "TOOLINIT" "CO" "PROJCONF" "BUILD" "QA" "UNIT" "INT" "ERROR" "ALL" "COPY_BUILD" "COPY_TESTS")

el_c=${#possible[@]}
found=`(\
for (( ind=0; ind < el_c; ind++ )); \
do \
  if [ "${possible[$ind]}" = "$strng" ]; then echo "1"; break; fi; \
done)`

if [ "$found" != "1" ]; then
echo "nicos_sleeper: error: invalid command line parameter $strng"
eval ${exitcom} 1
fi

a_st=(`echo "${NICOS_DOMINANT_PROJECTS}"`)
el_c=${#a_st[@]}
b_st=(`echo "${NICOS_DOMINANT_PARALLEL}"`)
el_cb=${#b_st[@]}

if [ "$el_c" -ne 0 -o "$el_cb" -ne 0 ]; then

a=1
while [ "$a" -le $LIMIT ]
do
let "a+=1"

not_found=""
not_found=`(\
for (( ind=0; ind < el_c; ind++ )); \
do \
  f_st="${NICOS_STAMPS_HOME}/nicos_stamp${a_st[$ind]}_${strng}"; \
  if [ ! -f $f_st ]; then echo "$f_st not found"; break; fi; \
  df=$(${NICOS_HOME}/nicos_mtime_diff.pl $f_st); \
  if [ "$df" -gt 16 ]; then echo "$f_st mtime $df"; break; fi;\
done)`

not_found_b=""
not_found_b=`(\
for (( ind=0; ind < el_cb; ind++ )); \
do \
  f_st1="${NICOS_STAMPS_HOME}/nicos_stamp${b_st[$ind]}_${strng_par}";\
  if [ ! -f $f_st1 ]; then echo "$f_st1 not found"; break; fi; \
  df1=$(${NICOS_HOME}/nicos_mtime_diff.pl $f_st1); \
  if [ "$df1" -gt 16 ]; then echo "$f_st1 mtime $df1"; break; fi;\
done)`

DD=`date`
if [ "$not_found" != "" -o "$not_found_b" != "" ]; then
echo "nicos_sleeper: sleeping $((a-1)): file $not_found $not_found_b : $DD"
sleep $quant
else
echo "nicos_sleeper: breaking sleep $not_found $not_found_b : $DD : ls :"
echo `ls -lt ${NICOS_STAMPS_HOME}/nicos_stamp*_${strng}`
echo `ls -lt ${NICOS_STAMPS_HOME}/nicos_stamp*_${strng_par}`  
break
fi

if [ "$a" -gt "$LIMIT" ]; then
echo "nicos_sleeper: error: number of sleep periods reached the limit $LIMIT"
eval ${exitcom} 1
fi

done  # while [ "$a" -le $LIMIT ]

else #if [ "$el_c" -ne 0 ]; then
echo "NICOS_DOMINANT_PROJECTS list empty $el_c"
fi #if [ "$el_c" -ne 0 ]; then
