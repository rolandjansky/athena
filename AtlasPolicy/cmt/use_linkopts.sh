
UL=
for p in `echo ${CMTPATH} | sed -e 's#:# #g'`; do 
  if test -d ${p}/InstallArea; then
    UL="${UL} -L${p}/InstallArea/${CMTCONFIG}/lib"
  fi
done
echo ${UL}

