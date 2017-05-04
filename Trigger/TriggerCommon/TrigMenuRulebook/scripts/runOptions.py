# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------
doUseOnline = False
#--------------------------
# check runRuleBook.py before enabling this option
doDBRuleBookDownload = False # False python rule book is used, True DB rules are used
#--------------------------
doPhysics = True
#sub-options for Physics (choose only one)
doEB = False

#--------------------------
doCosmic = False
#--------------------------
doStandby = False
#--------------------------

#--------------------------
EnableL1Topo_HIMenuOnly = False
#--------------------------

doVdM = False  
#sub-options for vdM_rules file (choose only one)
#Fill 1
doATLAS = False # to include HLT_lumipeb_L1MBTS_1_1 and HLT_lumipeb_L1MBTS_1_1_UNPAIRED_ISO, set doOffset = True
doCMS = False
doOffset = False
#Fill 2
doAliceLHCb=False
dolowmu = False

#--------------------------
# lumi points to generate:
# default for phyiscs if [] is 
# [300, 400, 600, 800, 1000, 1200, 1400, 1500, 1600, 1800, 2000, 2500, 3000, 3500, 4000, 5000] 
lumi_points = []
if doPhysics and doEB:
    lumi_points = [100,200,300,400,600,800,1000,1200,1400,1500,1600,1800,2000,2500,3000,3500,4000,4500]
else:
    lumi_points = []

# Proceed if errors are detected? This should only be used for debugging - never for online! 
ignoreErrors = False




#--------------------------
#BGRP for any rate estimation with 2015 EB Run280500 (only filled_bunches matters)
#filled_bunches = 881 #1
#empty_bunches = 2368 #3
#empty_after_filled_bunches = 190 #6
#unp_iso_bunches = 24 #4
#unp_noniso_bunches = 0 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches=10   # afterglow, so not important for physics
#bgrp10_bunches=4 # not used anywhere
#bgrp11_bunches=18 # vdm scan
#bgrp12_bunches=3473
#



# Bunch structure for 1452
#filled_bunches = 48 #1
#empty_bunches = 2836 #3
#empty_after_filled_bunches = 267 #6
#unp_iso_bunches = 1 #4
#unp_noniso_bunches = 96 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches= 1   # afterglow, so not important for physics
#bgrp10_bunches=4 # not used
#bgrp11_bunches= 48 # vdm scan
#bgrp12_bunches=3473

## Bunch structure for 1460
#filled_bunches = 424 #1
#empty_bunches = 761 #3  
#empty_after_filled_bunches = 1988 #6
#unp_iso_bunches = 2 #4 
#unp_noniso_bunches = 98 #5 
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches= 6   # afterglow, so not important for physics 
#bgrp10_bunches=4 # not used
#bgrp11_bunches= 424 # vdm scan
#bgrp12_bunches=3473


## Bunch structure for 1455
#filled_bunches = 492 #1
#empty_bunches = 770 #3  
#empty_after_filled_bunches = 2073 #6
#unp_iso_bunches = 1 #4 
#unp_noniso_bunches = 49 #5 
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches= 1   # afterglow, so not important for physics 
#bgrp10_bunches=4 # not used
#bgrp11_bunches= 492 # vdm scan
#bgrp12_bunches=3473

# Bunch structure for 1457 (VdM scans)
#filled_bunches = 400 #1
#empty_bunches = 663 #3
#empty_after_filled_bunches = 2236 #6
#unp_iso_bunches = 2 #4
#unp_noniso_bunches = 48 #5  
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches= 300   # afterglow, so not important for physics
#bgrp10_bunches=4 # not used
#bgrp11_bunches= 600 # vdm scan
#bgrp12_bunches=3473

## Bunch structure for 1417
#filled_bunches = 400 #1
#empty_bunches = 663 #3
#empty_after_filled_bunches = 2236 #6
#unp_iso_bunches = 2 #4
#unp_noniso_bunches = 48 #5  
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches= 6   # afterglow, so not important for physics
#bgrp10_bunches=4 # not used
#bgrp11_bunches= 400 # vdm scan
#bgrp12_bunches=3473

#### Bunch structure for 1410
#filled_bunches = 32 #1
#empty_bunches = 3023 #3  
#empty_after_filled_bunches = 180 #6
#unp_iso_bunches = 36 #4 
#unp_noniso_bunches = 0 #5 
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches= 6   # afterglow, so not important for physics 
#bgrp10_bunches=4 # not used
#bgrp11_bunches= 32 # vdm scan
#bgrp12_bunches=3473


#### Bunch structure for 1452
#filled_bunches = 48 #1
#empty_bunches = 2836 #3  
#empty_after_filled_bunches = 267 #6
#unp_iso_bunches = 0 #4 
#unp_noniso_bunches = 96 #5 
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches= 6   # afterglow, so not important for physics 
#bgrp10_bunches=4 # not used
#bgrp11_bunches= 48 # vdm scan
#bgrp12_bunches=3473

# Bunch structure for 1411
#filled_bunches = 216 #1
#empty_bunches = 1666 #3
#empty_after_filled_bunches = 1205 #6
#unp_iso_bunches = 48 #4
#unp_noniso_bunches = 20 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches= 1   # afterglow, so not important for physics
#bgrp10_bunches=4 # not used
#bgrp11_bunches= 216 # vdm scan
#bgrp12_bunches=3473

## Bunch structure for 1451
#filled_bunches = 33 #1
#empty_bunches = 3013 #3
#empty_after_filled_bunches = 189 #6
#unp_iso_bunches = 36 #4
#unp_noniso_bunches = 1 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches= 62   # afterglow, so not important for physics
#bgrp10_bunches=4 # not used
#bgrp11_bunches= 33 # vdm scan
#bgrp12_bunches=3473

## Bunch structure for 1444
#filled_bunches = 5 #1
#empty_bunches = 3308 #3
#empty_after_filled_bunches = 50 #6
#unp_iso_bunches = 10 #4
#unp_noniso_bunches = 0 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches=126   # afterglow, so not important for physics
#bgrp10_bunches=4 # not used
#bgrp11_bunches=5 # vdm scan
#bgrp12_bunches=3473
#

### Bunch structure for 1003 [cosmic/stanby during MD/TS]
filled_bunches = 3474 #1
empty_bunches = 3474 #3
empty_after_filled_bunches = 1479 #6
unp_iso_bunches = 18 #4
unp_noniso_bunches = 80 #5
abortgapnotcalib_bunches= 1 #8
bgrp9_bunches=1   # afterglow, so not important for physics
bgrp10_bunches=1 # not used
bgrp11_bunches=1 # vdm scan
bgrp12_bunches=1


###BGRP 1389, 2232b
#filled_bunches = 2232 #1
#empty_bunches = 629 #3
#empty_after_filled_bunches = 576 #6
#unp_iso_bunches = 16 #4
#unp_noniso_bunches = 8 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches=6   # afterglow, so not important for physics
#bgrp10_bunches=4 # not used anywhere
#bgrp11_bunches=62 # vdm scan
#bgrp12_bunches=3473

####BGRP 1349, 1800b
#filled_bunches = 1813 #1
#empty_bunches = 1357 #3
#empty_after_filled_bunches = 270 #6
#unp_iso_bunches = 16 #4
#unp_noniso_bunches = 7 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches=6   # afterglow, so not important for physics
#bgrp10_bunches=4 # not used anywhere
#bgrp11_bunches=26 # vdm scan
#bgrp12_bunches=3473

##BGRP 1385
#filled_bunches = 1581 #1
#empty_bunches = 1559 #3
#empty_after_filled_bunches = 300 #6
#unp_iso_bunches = 16 #4
#unp_noniso_bunches = 7 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches=6   # afterglow, so not important for physics
#bgrp10_bunches=4 # not used anywhere
#bgrp11_bunches=29 # vdm scan
#bgrp12_bunches=3473

#BGRP 1357 - 2041b_2029colliding
#filled_bunches = 2029 #1
#empty_bunches = 1111 #3
#empty_after_filled_bunches = 300 #6
#unp_iso_bunches = 16 #4
#unp_noniso_bunches = 7 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches=6   # afterglow, so not important for physics
#bgrp10_bunches=4 # not used anywhere
#bgrp11_bunches=29 # vdm scan
#bgrp12_bunches=3473
#

#BGRP 1377
#filled_bunches = 517 #1
#empty_bunches = 2803 #3
#empty_after_filled_bunches = 109 #6
#unp_iso_bunches = 24 #4
#unp_noniso_bunches = 6 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches=6   # afterglow, so not important for physics
#bgrp10_bunches=4 # not used anywhere
#bgrp11_bunches=23 # vdm scan
#bgrp12_bunches=3473

#BGRP 1346
#filled_bunches = 1596 #1
#empty_bunches = 1604 #3
#empty_after_filled_bunches = 299 #6
#unp_iso_bunches = 24 #4
#unp_noniso_bunches = 6 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches=6   # afterglow, so not important for physics
#bgrp10_bunches=4 # not used anywhere
#bgrp11_bunches=23 # vdm scan
#bgrp12_bunches=3473
#

#### Bunch structure for 1358
# filled_bunches = 2 #1 from Catrin, only two colliding bunches
# empty_bunches = 3221 #3
# empty_after_filled_bunches = 29 #6
# unp_iso_bunches = 20 #4
# unp_noniso_bunches = 6 #5
# abortgapnotcalib_bunches= 29 #8
# bgrp9_bunches=6   # afterglow, so not important for physics
# bgrp11_bunches=3 # vdm scan
# bgrp12_bunches=3473

#### Bunch structure for 1339
#filled_bunches = 3 #1
#empty_bunches = 3419 #3
#empty_after_filled_bunches = 29 #6
#unp_iso_bunches = 2 #4
#unp_noniso_bunches = 6 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches=126   # afterglow, so not important for physics
#bgrp11_bunches=3 # vdm scan
#bgrp12_bunches=3473

#### Bunch structure for 1423
#filled_bunches = 2 #1
#empty_bunches = 2814 #3
#empty_after_filled_bunches = 179 #6
#unp_iso_bunches = 40 #4
#unp_noniso_bunches = 6 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches=6   # afterglow, so not important for physics
#bgrp10_bunches=6
#bgrp11_bunches=20 # vdm scan
#bgrp12_bunches=3473
#

#### Bunch structure for 1346 [cosmic/stanby during MD/TS]
#filled_bunches = 1596 #1
#empty_bunches = 1604 #3
#empty_after_filled_bunches = 229 #6
#unp_iso_bunches = 24 #4
#unp_noniso_bunches = 6 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches=6   # afterglow, so not important for physics
#bgrp11_bunches=23 # vdm scan
#bgrp12_bunches=3473

#### Bunch structure for 1349 [cosmic/stanby during MD/TS]
#filled_bunches = 1813 #1
#empty_bunches = 1357 #3
#empty_after_filled_bunches = 270 #6
#unp_iso_bunches = 16 #4
#unp_noniso_bunches = 7 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches=6   # afterglow, so not important for physics
#bgrp11_bunches=23 # vdm scan
#bgrp12_bunches=3473

#### Bunch structure for 1344 - 2029b_2029colliding
#filled_bunches = 2029 #1
#empty_bunches = 1144 #3
#empty_after_filled_bunches = 300 #6
#unp_iso_bunches = 6 #4
#unp_noniso_bunches = 0 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches=6   # afterglow, so not important for physics
#bgrp10_bunches=1 # not used anywhere
#bgrp11_bunches=23 # vdm scan
#bgrp12_bunches=3473

#### Bunch structure for 1356
#25ns8b4e_529b_517_291_433_112bpi8inj
#filled_bunches = 517 #1
#empty_bunches = 2803 #3
#empty_after_filled_bunches = 109 #6
#unp_iso_bunches = 24 #4
#unp_noniso_bunches = 0 #5
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches=6   # afterglow, so not important for physics
#bgrp11_bunches=23 # vdm scan
#bgrp12_bunches=3473   

#### Bunch structure for 
# filled_bunches = 1200 #1
# empty_bunches = 2583 #3
# empty_after_filled_bunches = 120 #6
# unp_iso_bunches = 24 #4
# unp_noniso_bunches = 1 #5
# abortgapnotcalib_bunches= 29 #8
# bgrp9_bunches=129   # afterglow, so not important for physics
# bgrp11_bunches=10 # vdm scan
# bgrp12_bunches=0

# #### Bunch structure for 446 bunches/48btl #BGRP Set 1264
# filled_bunches = 446 #1
# empty_bunches = 2883 #3
# empty_after_filled_bunches = 120 #6
# unp_iso_bunches = 24 #4
# unp_noniso_bunches = 1 #5
# abortgapnotcalib_bunches= 29 #8
# bgrp9_bunches=129   # afterglow, so not important for physics
# bgrp11_bunches=10 # vdm scan
# bgrp12_bunches=0

#### Bunch structure for 447 bunches/72btl #BGRP Set 1227
# filled_bunches = 447 #1
# empty_bunches = 2882 #3
# empty_after_filled_bunches = 100 #6
# unp_iso_bunches = 24 #4
# unp_noniso_bunches = 1 #5
# abortgapnotcalib_bunches= 29 #8
# bgrp9_bunches=129   # afterglow, so not important for physics
# bgrp11_bunches=10 # vdm scan
# bgrp12_bunches=0

# filled_bunches = 302 #1
# empty_bunches = 3058 #3
# empty_after_filled_bunches = 69 #6
# unp_iso_bunches = 24 #4
# unp_noniso_bunches = 6 #5
# abortgapnotcalib_bunches= 29 #8
# bgrp9_bunches=126   # afterglow so not important for physics
# bgrp11_bunches=7  # vdm scan
# bgrp12_bunches=0

# filled_bunches = 3 #1
# empty_bunches = 3221 #3
# empty_after_filled_bunches = 29 #6
# unp_iso_bunches = 20 #4
# unp_noniso_bunches = 6 #5
# abortgapnotcalib_bunches= 29 #8
# bgrp9_bunches=6   # afterglow so not important for physics
# bgrp10_bunches=4  # ALFA
# bgrp11_bunches=3  # vdm scan
# bgrp12_bunches=3473

# filled_bunches = 671 #1
# empty_bunches = 451 #3
# empty_after_filled_bunches = 2253 #6
# unp_iso_bunches = 15 #4
# unp_noniso_bunches = 12 #5
# abortgapnotcalib_bunches= 29 #8
# bgrp9_bunches=6   # afterglow so not important for physics
# bgrp10_bunches=10  # ALFA
# bgrp11_bunches=671  # vdm scan
# bgrp12_bunches=3473

#--------------------------
### VDM settings 
if doVdM == True:
    if dolowmu==False and doAliceLHCb==False: #Fill 1 #1257
        filled_bunches = 29 #1
        empty_bunches = 2846 #3
        empty_after_filled_bunches = 69 #6
        unp_iso_bunches = 28 #4
        unp_noniso_bunches = 6 #5
        abortgapnotcalib_bunches= 2 #8
        bgrp9_bunches=5   # afterglow so not important for physics
        bgrp11_bunches=15  # vdm scan
        bgrp12_bunches=0
        print "Using vdM settings with %s filled bunches" % (str(filled_bunches))
    elif doVdM==True and doATLAS==False and doCMS==False and doOffset==False: #Fill 2 1269
        filled_bunches = 8 #1
        empty_bunches = 2440 #3
        empty_after_filled_bunches = 79 #6
        unp_iso_bunches = 86 #4
        unp_noniso_bunches = 1 #5
        abortgapnotcalib_bunches= 29 #8
        bgrp9_bunches=4   # afterglow so not important for physics
        bgrp11_bunches=12  # vdm scan
        bgrp12_bunches=0
        print "Using low mu vdM settings with %s filled bunches" % (str(filled_bunches))
    else:
        print "Choose exaclty 1 of the vdM sub-options please!!"
