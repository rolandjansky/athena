 
f_get_joboptions() { 
	local cur 
  COMPREPLY=()
  cur=${COMP_WORDS[COMP_CWORD]}
	COMPREPLY=( $( compgen -W "`lookup_joboptions.sh`"  $cur ) )
	return 0
}

f_get_local_joboptions() { 
    COMPREPLY=( $( compgen -W "`lookup_local_joboptions.sh`" -- $2 ) )
    return 0
}

f_get_txtjoboptions() { 
	local cur 
  COMPREPLY=()
  cur=${COMP_WORDS[COMP_CWORD]}
	COMPREPLY=( $( compgen -W "`lookup_txtjoboptions.sh`"  $cur ) )
	return 0
}

f_get_pyjoboptions() { 
	local cur 
  COMPREPLY=()
  cur=${COMP_WORDS[COMP_CWORD]}
	COMPREPLY=( $( compgen -W "`lookup_pyjoboptions.sh`"  $cur ) )
	return 0
}

f_get_data() { 
	local cur 
  COMPREPLY=()
  cur=${COMP_WORDS[COMP_CWORD]}
	COMPREPLY=( $( compgen -W "`lookup_data.sh`"  $cur ) )
	return 0
}

f_get_scripts() {
	local cur 
  COMPREPLY=()
  cur=${COMP_WORDS[COMP_CWORD]}
	COMPREPLY=( $( compgen -W "`lookup_scripts.sh`"  $cur ) )
	return 0
}

f_get_xmls() { 
	local cur 
  COMPREPLY=()
  cur=${COMP_WORDS[COMP_CWORD]}
	COMPREPLY=( $( compgen -W "`lookup_xmls.sh`"  $cur ) )
	return 0
}


f_get_files() {
	local cur prev
	COMPREPLY=()

	cur=${COMP_WORDS[COMP_CWORD]}
	prev=${COMP_WORDS[COMP_CWORD-1]}

	if [[ ${cur} == -* ]] ; then
		COMPREPLY=( $(compgen -W "-jo -scripts -data -xmls" -- $cur) )	
		return 0
	fi
	
	case "$prev" in
		-jo)
		COMPREPLY=( $( compgen -W "`lookup_joboptions.sh`"  $cur ) )
		return 0			
	;;

		-scripts)
		COMPREPLY=( $( compgen -W "`lookup_scripts.sh`"  $cur ) )
		return 0	
	;;

		-data)
		COMPREPLY=( $( compgen -W "`lookup_data.sh`"  $cur ) )
		return 0		
	;;
	
		-xmls)
		COMPREPLY=( $( compgen -W "`lookup_xmls.sh`"  $cur ) )
		return 0	
	;;

	esac
}

if [ "${DO_EXT_ATHENA_TAB_COMPLETION:-}" = 1 ]; then 
#    echo "do expanded completion for athena"
    complete -C lookup_joboptions.sh -X '!&*' athena athena.py
else
#    echo "do only local completion for athena"
    complete -C lookup_local_joboptions.sh -X '!&*' -o dirnames athena athena.py
fi
complete -F f_get_txtjoboptions check_joboptions
#complete -F f_get_pyjoboptions athena.py
complete -F f_get_joboptions get_joboptions
complete -F f_get_files get_files

