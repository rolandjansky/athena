

function f_get_joboptions () { reply=(`lookup_joboptions.sh`);}
function f_get_local_joboptions { reply=(`lookup_local_joboptions.sh`);}
function f_get_txtjoboptions { reply=(`lookup_txtjoboptions.sh`);}
function f_get_pyjoboptions { reply=(`lookup_pyjoboptions.sh`);}
function f_get_data { reply=(`lookup_data.sh`);}
function f_get_scripts { reply=(`lookup_scripts.sh`);}
function f_get_xmls { reply=(`lookup_xmls.sh`);}

if compctl &> /dev/null; then
   if [ "$DO_EXT_ATHENA_TAB_COMPLETION" = '1' ]; then 
   #    echo "do expanded completion for athena"
      compctl -K f_get_joboptions athena 
      compctl -K f_get_joboptions athena.py
   else #    echo "do only local completion for athena"
      compctl -K f_get_local_joboptions athena 
      compctl -K f_get_local_joboptions athena.py
  fi
  compctl -K f_get_txtjoboptions check_joboptions

  compctl -K f_get_joboptions get_joboptions

  compctl -k "(-jo -scripts -data -xmls -symlink -copy)" \
          -x 'r[-jo,-]' -K f_get_joboptions - \
             'r[-scripts,-]' -K f_get_scripts - \
             'r[-data,-]' -K f_get_data - \
             'r[-xmls,-]' -K f_get_xmls -- get_files
fi

