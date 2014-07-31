
for f in `echo ${DATAPATH} | sed -e 's#:# #g'`; do
  if test -d ${f} ; then find ${f} -name '*' | egrep -v "(~|share/(bin|lib)|CVS)" | sed -e 's#'"${f}"'/##' -e 's#'"${f}"'##'; fi
done
