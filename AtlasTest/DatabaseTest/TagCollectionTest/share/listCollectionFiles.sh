#!/bin/sh

if [ $# -lt 1 -o "$1" = '-h' ]; then
    echo 'Usage: createFileCatalogFromCollection.sh \'
    echo '       <collection name> \'
    echo '       [collection type (DEFAULT: "ExplicitROOT")] \'
    echo '       [collection database connection (DEFAULT: "")] \'
    echo '       [catalog name (DEFAULT: "xmlcatalog_file:PoolFileCatalog.xml")]' \
    exit
fi

type=${2:-'ExplicitROOT'}
connect=${3:-''}
catalog=${4:-'xmlcatalog_file:PoolFileCatalog.xml'}

export POOL_OUTMSG_LEVEL=7

case "${type}" in
    ExplicitROOT )
        type=RootCollection
        ;;
    ExplicitMySQL )
        type=MySQLCollection
        ;;
    ExplicitMySQLlt )
        type=MySQLltCollection
        ;;
    * )
        echo ERROR: listCollectionFiles.sh: Unexpected collection type. Use ExplicitROOT, ExplicitMySQL or ExplicitMySQLlt
        exit 1
esac

filelist=`CollListPFN.exe -src "$1" "${type}" -dbconnect "${connect}" -fcwrite "${catalog}"`

echo ${filelist}







