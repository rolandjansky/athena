SCRIPTPATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export PYTHONPATH=${SCRIPTPATH}/../python:${PYTHONPATH}
