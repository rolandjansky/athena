
for f in `echo ${XMLPATH} | sed -e 's#:# #g'`; do
  if test -d ${f} ; then 
    find ${f} -type f -o -type l | \
      egrep -v 'cmtref' | \
      sed -e 's#'"${f}"'/##' -e 's#'"${f}"'##'
  fi
done
