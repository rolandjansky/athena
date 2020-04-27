# DumpGeo - Dump the ATLAS GeoModel to a local file

 * [Intro](#intro) 
 * [Setup](#build)
 * [Run](#run)
 * [Documentation](#documentation)

## Intro

`DumpGeo` is an Athena algorithm inheriting from the class AthAlgorithm. Internally, it calls the package `GeoExporter` to dump 
the ATLAS GeoModel into a local file.

*Note:* At the time of writing, the local file is a SQLite file).

Instructions are meant for a `lxplus`-like machine (CC7 or SLC6) where the Athena framework is installed.




## Setup 

You should setup Athena, as usual; for example you can setup the latest nightly build:

```bash
setupATLAS
asetup Athena,master,latest,slc6
```

## Run

`dump-geo` can be used both as a shell command or as an Athena jobOption, in command-line or embedded within your own jobOption.

### Basic use - Run as a terminal command

At the prompt, run the command `dump-geo`:

```bash
dump-geo
```

this will dump the default geometry tag (`ATLAS-R2-2016-01-00-01`, at the time of writing) 
into a local file named `geometry-TAG.db`, where `TAG` is replaced with the specific geometry tag being dumped .

Optionally, you can specify which geometry tag to be dumped by adding the `-detdescr` flag and a valid ATLAS Geometry tag; for example:

```bash
dump-geo -detdescr=ATLAS-R2-2016-01-00-01
```

After issueing the command, a file named `geometry-ATLAS-R2-2016-01-00-01.db` will be created in the run folder.


### Run it as an Athena jobOption

You can also run `dump-geo` as an Athena jobOption. For example:

```bash
athena DumpGeo/dump-geo.py -c "DetDescrVersion='ATLAS-R3-2021-01-00-00‘"
```

You can even embed it into your own workflow, within your own jobOption.


### Additional Options

You can use several flags to steer the dump mechanism. You can see the full list of options by using:

```bash
dump-geo -h
```

For example, you can exclude subsystems by using the `-noX` flags, for example:

```bash
dump-geo -nomuon -nocalo
```

will dump only the Inner Detector geometry into the output `geometry.db` file.

## Documentation

You can take a look at the full documentation for `dump-geo`:

- by running `dump-geo -h`
- by visiting the GeoModel documentation website: https://cern.ch/geomodel

 

