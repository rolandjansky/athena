
##set -x

unset localpathvalue
for p in `echo ${CMTPATH} | sed -e 's#:# #g'`; do 
  if test -d ${p}/InstallArea; then
    export localpathvalue=${localpathvalue}:"${p}/InstallArea/${CMTCONFIG}/bin"
  fi
done
echo ${localpathvalue}

