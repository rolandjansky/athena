# This script is to be sourced by bash or zsh.

# determine path to this script
# http://stackoverflow.com/questions/59895/can-a-bash-script-tell-what-directory-its-stored-in
SOURCE_TCU_SETUP="${BASH_SOURCE[0]:-$0}"

DIR_TCU_SETUP="$( dirname "$SOURCE_TCU_SETUP" )"
while [ -h "$SOURCE_TCU_SETUP" ]
do 
  SOURCE_TCU_SETUP="$(readlink "$SOURCE_TCU_SETUP")"
  [[ $SOURCE_TCU_SETUP != /* ]] && SOURCE_TCU_SETUP="$DIR_TCU_SETUP/$SOURCE_TCU_SETUP"
  DIR_TCU_SETUP="$( cd -P "$( dirname "$SOURCE_TCU_SETUP"  )" && pwd )"
done
DIR_TCU_SETUP="$( cd -P "$( dirname "$SOURCE_TCU_SETUP" )" && pwd )"

export LD_LIBRARY_PATH=${DIR_TCU_SETUP}/standalone${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
export TCU_SHARE_DIR=${DIR_TCU_SETUP}/share
