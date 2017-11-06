SCRIPTPATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export PATH=${SCRIPTPATH}/../scripts:${PATH}
export PYTHONPATH=${SCRIPTPATH}/../python:${PYTHONPATH}

