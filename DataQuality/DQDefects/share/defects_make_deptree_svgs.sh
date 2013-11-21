#/bin/bash

SELF="${0##*/}"
OUTPUT="."
TAG="HEAD"
LANG="svg"
EXT=".svg"

OPT=$(getopt -n "${SELF}" -o ho:t:z --l help,output:,tag:,gzip -- "$@")
[[ $? -eq 0 ]] || exit 1
eval set -- "${OPT}"

while true ; do
    case "$1" in
        -h )            echo "Usage: ${SELF} [ -o OUTPUT ] [ -t TAG ] [ -z ]" ; exit 0 ;;
        --help )        echo "Usage: ${SELF} [ --output OUTPUT ] [ --tag TAG ] [ --gzip ]" ; exit 0 ;;
        -o | --output ) OUTPUT="$2" ; shift 2 ;;
        -t | --tag )    TAG="$2" ; shift 2 ;;
        -z | --gzip )   LANG="svgz" ; EXT=".svg.gz" ; shift 1 ;;
        -- )            shift 1 ; break ;;
    esac
done

defects_dump_graph.py -o "${OUTPUT}" -t "${TAG}" || exit 1
find "${OUTPUT}/${TAG}" -name \*.dot | sed 's/\.dot$//' | xargs -ri -n1 -P8 dot -T"${LANG}" -o{}"${EXT}" {}.dot || exit 1
find "${OUTPUT}/${TAG}" -name \*.dot | xargs -r rm
