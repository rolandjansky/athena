
f=`pwd`
if test -d ${f} ; then find ${f} -name '*.txt' | sed -e 's#'"${f}"'/##'; fi

for f in `echo ${JOBOPTSEARCHPATH} | sed -e 's#,# #g'`; do
  if test -d ${f} ; then find ${f} -name '*.txt' | sed -e 's#'"${f}"'/##'; fi
done
