# DumpGeo - Dump the ATLAS GeoModel to SQLite

 * [Intro](#intro) 
 * [Get the code](#get-the-code)
 * [Setup build](#setup-build)
 * [Build](#build)
 * [Set debug output](#set-debug-output)
 * [Run](#run)
 * [Sample data](#sample-data)

## Intro

`DumpGeo` is an Athena algorithm inheriting from the class AthAlgorithm.

Run inside Athena, it calls the package `GeoExporter` to dump 
the ATLAS GeoModel into a SQLite file.

Instructions are meant for a **SLC6 machine** (or `lxplus`)

## Get the code

    git clone https://:@gitlab.cern.ch:8443/rbianchi/athena.git # https://gitlab.cern.ch/rbianchi/athena.git
    cd athena
    git checkout dump-geomodel
    cd ..

## Setup build

    mkdir build
    cd build
    cp ../athena/Projects/WorkDir/package_filters_example.txt ../package_filters.txt
    
    ### replace the example path with the GeoModelStandalone path, plus VP1Utils and VP1Base
    sed -i '/Control/c\+ DetectorDescription/GeoModel/GeoModelStandalone/.*\n+ graphics/VP1/VP1Utils\n+ graphics/VP1/VP1Base' ../package_filters.txt

## Build

    setupATLAS
    asetup Athena,master,latest,slc6
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DATLAS_PACKAGE_FILTER_FILE=../package_filters.txt ../athena/Projects/WorkDir
    make
    source x86_64-slc6-gcc62-opt/setup.sh

## Set debug output

This is **OPTIONAL**

    export VP1_VERBOSE_OUTPUT=1
    export VP1_DEBUG_OUTPUT=1

## Run

At the prompt, run the command `dump-geo`, followed by the `-detdescr` flag and a valid ATLAS Geometry tag; for example:

    dump-geo -detdescr=ATLAS-R2-2016-01-00-01

a file named `geometry-ATLAS-R2-2016-01-00-01.db` will be created in the run folder.

## Sample data

If you want, you can downlaod a sample datafile (draft):

     wget https://atlas-vp1.web.cern.ch/atlas-vp1/doc_new/sample_datafiles/geometry-ATLAS-R2-2016-01-00-01.db
