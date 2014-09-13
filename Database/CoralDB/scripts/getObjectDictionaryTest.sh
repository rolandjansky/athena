#!/bin/bash

set -e

#CONNECTION=oracle://devdb10/ATLAS_PIXELDT_ANDR
CONNECTION=sqlite_file:test.db


showids() {
    echo "Listing all ids ================================================================"
    ccdb --db $CONNECTION --idTag IDTAG01 ids
    echo "--------------- ================================================================"

    echo "Listing connectivity ids ================================================================"
    ccdb --db $CONNECTION --idTag IDTAG01 --ctag=CTAG1 ids
    echo "--------------- ================================================================"

    echo "Listing alias ids ================================================================"
    ccdb --db $CONNECTION --idTag IDTAG01 --atag=ATAG1 ids
    echo "--------------- ================================================================"
    
    echo "Listing data ids ================================================================"
    ccdb --db $CONNECTION --idTag IDTAG01 --dtag=DTAG1 ids
    echo "--------------- ================================================================"
    
    echo "Listing c+a ids ================================================================"
    ccdb --db $CONNECTION --idTag IDTAG01 --ctag=CTAG1 --atag=ATAG1  ids
    echo "--------------- ================================================================"
    
    echo "Listing c+d ids ================================================================"
    ccdb --db $CONNECTION --idTag IDTAG01 --ctag=CTAG1 --dtag=DTAG1  ids
    echo "--------------- ================================================================"
    
    echo "Listing a+d ids ================================================================"
    ccdb --db $CONNECTION --idTag IDTAG01 --dtag=DTAG1 --atag=ATAG1  ids
    echo "--------------- ================================================================"
    
    echo "Listing c+a+d ids ================================================================"
    ccdb --db $CONNECTION --idTag IDTAG01 --ctag=CTAG1 --atag=ATAG1 --dtag=DTAG1 ids
    echo "--------------- ================================================================"

}


echo "Using CONNECTION = $CONNECTION"
echo "dropping schema"
ccdb --db $CONNECTION  dropSchemaAndDeleteAllData
echo "creating schema"
ccdb --db $CONNECTION  create

echo "defining objectDictionary tags"
ccdb --db $CONNECTION  makeNewObjectDictionaryTag IDTAG01

echo "defining other tags"
ccdb --db $CONNECTION  --idTag IDTAG01 makeNewConnectivityTag CTAG1
ccdb --db $CONNECTION  --idTag IDTAG01 makeNewAliasTag ATAG1
ccdb --db $CONNECTION  --idTag IDTAG01 makeNewDataTag DTAG1

echo "Listing tags"
ccdb --db $CONNECTION getExistingTags

showids

echo "inserting objects"
ccdb --db $CONNECTION  --idTag IDTAG01 --ctag=CTAG1 insertObject RRR TYPER
ccdb --db $CONNECTION  --idTag IDTAG01 --ctag=CTAG1 insertObject AAA TYPEA
ccdb --db $CONNECTION  --idTag IDTAG01 --ctag=CTAG1 insertObject BBB TYPEB
ccdb --db $CONNECTION  --idTag IDTAG01 --ctag=CTAG1 insertObject CCC TYPEC

ccdb --db $CONNECTION  --idTag IDTAG01 --ctag=CTAG1 insertObject ALON TYPE1
ccdb --db $CONNECTION  --idTag IDTAG01 --ctag=CTAG1 insertObject CLON TYPE1
ccdb --db $CONNECTION  --idTag IDTAG01 --ctag=CTAG1 insertObject UNUSED TYPE1

showids

echo "inserting masterlist"
ccdb --db $CONNECTION --idTag IDTAG01 --ctag=CTAG1 insertMaster RRR

showids

echo "inserting connections"
ccdb --db $CONNECTION --idTag IDTAG01 --ctag=CTAG1 insertConnection AAA BSLOT BBB ASLOT

showids

echo "more connections"
ccdb --db $CONNECTION --idTag IDTAG01 --ctag=CTAG1 insertConnection RRR ASLOT AAA UP
ccdb --db $CONNECTION --idTag IDTAG01 --ctag=CTAG1 insertConnection RRR BSLOT BBB UP
ccdb --db $CONNECTION --idTag IDTAG01 --ctag=CTAG1 insertConnection AAA CSLOT CCC UP

showids

echo "inserting aliases"
ccdb --db $CONNECTION  --idTag IDTAG01 --atag=ATAG1 insertAlias AAAALIAS CONVENTION AAA
ccdb --db $CONNECTION  --idTag IDTAG01 --atag=ATAG1 insertAlias ALONALIAS CONVENTION ALON
ccdb --db $CONNECTION  --idTag IDTAG01 --atag=ATAG1 insertAlias BBBALIAS CONVENTION BBB

showids

echo "inserting clobs"
ccdb --db $CONNECTION --idTag IDTAG01 --dtag=DTAG1  insertCLOB AAA field1 'int 5'
ccdb --db $CONNECTION --idTag IDTAG01 --dtag=DTAG1  insertCLOB CCC cfield 'char c'
ccdb --db $CONNECTION --idTag IDTAG01 --dtag=DTAG1  insertCLOB CLON clfield 'sample'

showids

echo "$0 DONE"
