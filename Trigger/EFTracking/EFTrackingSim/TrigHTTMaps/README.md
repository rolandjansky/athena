# TrigHTTMaps

This package manages mapping and other geometry-related tasks for HTT.
The primary purpose is to read files that map detector geometry to HTT geometry.
Almost all functionality on the C side is handled by a single service, `TrigHTTMappingSvc`.

## Map Files

There are several maps stored as columnar text files, and we have a class to read each.
More documentation can be found in the header files of each class.

1.  Plane map: Assigns the logical layers.
2.  Region map: Splits the detector into regions as defined by the hardware.

Note the following definitions of different 'layers':

- detector layer: Equivalent to the **FILLME** variable in InDet, and identifies portions of the detector with similar geometry.
- physical layer: A layer index used in HTT, a different 0-indexed series for each (pixel/strip) X (barrel/endcap) combination.
        Each detector layer can be uniquely identified by pixel/strip, barrel/encap, and the physical layer number.
        TODO this should probably be renamed, and is questionable if we really need this (why not just use detector layer?).
- logical layer: The layers used in pattern matching. Note that a logical layer can be made up of
        multiple detector layers.
- section: If you have multiple detector layers contributing to the same logical layer, use the section number to
        help distinguish them (this should count from 0 for each logical layer).
        TODO rename this?

Throughout HTT, 'layer' with no other specifiers typically refers to a logical layer.

### Plane Map

An excerpt from an example pmap:

```
ATLAS-P2-ITK-22-02-00
8 logical_s1
13 logical_s2
5 pixel barrel
4 pixel endcap+
4 pixel endcap-
8 Strip barrel
12 Strip endcap+
12 Strip endcap-

region 0
pixel 0     -1    0          plane1 -1    plane2 0  phi 16  eta 44
pixel 0     -1    1          plane1 -1    plane2 1  phi 20  eta 38
pixel 0     -1    2          plane1 -1    plane2 2  phi 30  eta 44
pixel 0     -1    3          plane1 -1    plane2 3  phi 40  eta 50
pixel 0     -1    4          plane1 0     plane2 4  phi 50  eta 52
pixel 1     0     0          plane1 -1    plane2 4  phi 22  eta 15
pixel 1     1     1          plane1 -1    plane2 4  phi 32  eta 10
pixel 1     2     2          plane1 -1    plane2 4  phi 44  eta 8
pixel 1     3     3          plane1 -1    plane2 4  phi 52  eta 9
pixel 2     0     0          plane1 -1    plane2 4  phi 22  eta 15
pixel 2     1     1          plane1 -1    plane2 4  phi 32  eta 10
pixel 2     2     2          plane1 -1    plane2 4  phi 44  eta 8
pixel 2     3     3          plane1 -1    plane2 4  phi 52  eta 9
SCT   0     -1    0 stereo 0 plane1 1     plane2 5  phi 28  eta 112
SCT   0     -1    1 stereo 1 plane1 -1    plane2 6  phi 28  eta 112
SCT   0     -1    2 stereo 0 plane1 2     plane2 7  phi 40  eta 112
SCT   0     -1    3 stereo 1 plane1 3     plane2 8  phi 40  eta 112
...
```

The first line contains the ITk Geometry version that the map depicts. The next two lines of the header describe how many logical layers are used in the first stage and second stage of track fitting respectively.
The next six lines describe the number of pixel and strip layers in the barrel, negative endcap, and positive endcap that are contained in the file. 

The region ID is given after the header. Each pmap can have multiple regions, as long as each has the same number of layers.

After the region ID, there is a row for each detector layer, each with up to 14 fields.

1. **Silicon**: pixel or strip layer
2. **Barrel/Encap**: 0 identifies the barrel, 1 the  positive endcap, and 2 the negative endcap
3. **physical disk number**: Not really sure what this is...
4. **Physical layer ID**: HTT physical layer index
5. **stereo keyword**: unique to strip layers - the keyword `stereo`
6. **stereo side**: unique to strip layers - 1 denotes the side closer to the beamline, 0 is the opposite side.
7. **plane1 keyword**: the word `plane1`
8. **First stage logical layer ID**: logical layer ID used in first stage of track fitting. -1 means this detector layer is not used. Set by hand.
9. **plane 2keyword**: the word `plane2`
10. **Second stage logical layer ID**: logical layer ID used in second stage track fitting. Set by hand
11. **phi keyword**: the word `phi`
12. **Number of modules in phi**: total number of modules in the phi direction
13. **eta keyword**: the word `eta`
14. **Number of modules in eta**: total number of modules in the eta direction

### Region Map
The region map divides the detector into multiple regions as defined by the hardware.
A region is roughly an eta/phi slice that handled by the same TP board (??? not quite sure if this is right).
The region map delineates the modules in each region based on InDetIdentifier ids.

Region map format:
1. The first line should read
   ```
   towers <NTOWERS> phi <NPHI>
   ```
where NTOWERS is the number of regions, and NPHI is ???. TODO rename tower to region
2. A list of the region definitions with the below format, seperated by empty lines, in order.

Region definition format:
1. A line with the region number.
2. A line for each detector layer, which is a space-separated list with the below values.

Line values (i.e. columns):
1. isPixel (1/0)
2. isEndcap (1/0)
3. physical layer (see HTTPlaneMap)
4. minimum phi id   ] -- in this detector layer in this region
5. maximum phi id   ]
6. total phi modules  -- in this detector layer among all regions
7. minimum eta id
8. maximum eta id
9. total eta modules

The min and max ids are the important fields that actually define the boundary
of the region, and are inclusive.
TODO there is no reason to store total phi/eta module in each region.
Each region can be thought of as a slice in eta and phi, and are identifie

Here is an excerpt from an example rmap:

```
towers 96 phi 16

0
1 0 0 0 15 16 -22 22 44
1 0 1 0 19 20 -19 19 38
1 0 2 0 29 30 -22 22 44
1 1 0 0 21 22 0 14 15
1 1 1 0 31 32 0 9 10
1 1 3 0 51 52 0 8 9
0 0 0 0 31 32 -56 56 112
0 0 1 0 27 28 -56 56 112
...

1
1 0 0 0 15 16 -22 22 44
1 0 1 0 19 20 -19 19 38
...
```


## Mapping Service

The interface to the mapping service is defined by `ITrigHTTMappingSvc.h`, and is implemented by `TrigHTTMappingSvc`.
The service can be configured at python-level using the tag system (see HTT readme) via `HTTMapConfig.py`.
The service handles reading all the map files at initialization, and exposes pointers to the map object classes.


## Geometry Constants

Some geometry numbers are hard-coded in geometry_constants.h, which defines the geometry_constants namespace. 
For example, the inner radii of each strip endcap row, from the TDR.
