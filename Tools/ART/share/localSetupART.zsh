SOURCE=${(%):-%N}
SCRIPTPATH="$( cd "$( dirname "$SOURCE" )" && pwd )"
export PATH=${SCRIPTPATH}/../scripts:${PATH}
export PYTHONPATH=${SCRIPTPATH}/../python:${PYTHONPATH}

