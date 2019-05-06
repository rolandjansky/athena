
* The files are only for the positive (eta, phi) quadrant, can be extended to the full area by duplicating with ((+,+), (+,-), (-,+), (-,-))
* The ge / gt logic needs to be hard-coded rather than part of the maps - it needs to be swapped for (-,-) and rotated for (+,-) and (-,+)
* Propose to merge the maps and hard-code the logic
* Values of phi > pi are to be interpreted as phi = -pi + phi
* The first entry in each file is the centre of the jet / seed or the seed  one is evaluating local maxima for.




For more information, contact Marcel Weirich <maweiric@uni-mainz.de>


Hi Will,

attached you'll find a first version of the lists for the Sliding
Window Algorithm in the forward region (for jFEX it's 1.6 < eta < 4.9).
The files with "_1.dat" cover 0 < phi < 1.6, the files with "_2.dat"
cover 1.6 < phi < 3.2 (you can merge them if you want ... for me/us
it's more convenient).
As you might know the FCal is symmetric by 180 degree rotation and
fully symmetric from the A to the C side.

Each line in each file corresponds to a single list for a given tower
position.
The first column gives you the phi coordinate of the tower positions,
the second gives you eta.
The remaining values are pairs of phi and eta coordinates.
All values are separated using tabs.

The files "seed_#.dat" contain towers you need to build seed energies.
The files "searchG_#.dat" contain seeds (central tower position of a
seed) you need to compare to the central seed (first coordinate pair
in each line) with a ">" (greater) operator.
The files "searchGe_#.dat" contain seeds (central tower position of a
seed) you need to compare to the central seed (first coordinate pair
in each line) with a ">=" (greater or equal) operator.
The files "jet_#.dat" contain towers you need to build jet energies.

Please let me know if you see problems with the format etc.

Cheers,
Marcel