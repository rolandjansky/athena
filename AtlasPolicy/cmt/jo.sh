
unset JO
for p in `echo ${CMTPATH} | sed -e 's#:# #g'`; do 
  if test -d ${p}/InstallArea/jobOptions; then
    export JO=${JO},"${p}/InstallArea/jobOptions"
  fi
done
echo ${JO}

