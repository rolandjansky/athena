

for f in `echo ${PATH} | sed -e 's#:# #g'`; do
  a=`echo ${f} | grep InstallArea/share/bin`
  if test ! "${a}" = "" ; then
    if test -d ${f} ; then 
      find ${f} -name '*' | sed -e 's#'"${f}"'/##' -e 's#'"${f}"'##'
    fi
  fi
done
