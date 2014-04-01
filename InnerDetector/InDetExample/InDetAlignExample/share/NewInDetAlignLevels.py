# Add Comments

## alignment geometry
#trtAlignBarrel  = True
#trtAlignEndcaps = True
### alignment parameters Barrel
#trtAlignBarrelX = True
#trtAlignBarrelY = True
#trtAlignBarrelZ = True
#trtAlignBarrelRotX = True
#trtAlignBarrelRotY = True
#trtAlignBarrelRotZ = True
### alignment parameters Endcap
#trtAlignEndcapX = True
#trtAlignEndcapY = True
#trtAlignEndcapZ = True
#trtAlignEndcapRotX = True
#trtAlignEndcapRotY = True
#trtAlignEndcapRotZ = True

# when running local Chi2 method we have to switch off insensitive
# DoF for TRT
#if "trtAlignmentLevel" in dir() and trtAlignmentLevel == 1:
#    trtAlignBarrelZ = False
#if "trtAlignmentLevel" in dir() and trtAlignmentLevel == 2:
#    trtAlignBarrelY = False

## Pixel alignment DoFs   
#pixelAlignBarrel  = True
#pixelAlignEndcaps = True
### alignment parameters Barrel   
#pixelAlignBarrelX = True
#pixelAlignBarrelY = True
#pixelAlignBarrelZ = True
#pixelAlignBarrelRotX = True
#pixelAlignBarrelRotY = True
#pixelAlignBarrelRotZ = True
### alignment parameters Endcap   
#pixelAlignEndcapX = True
#pixelAlignEndcapY = True
#pixelAlignEndcapZ = True
#pixelAlignEndcapRotX = True
#pixelAlignEndcapRotY = True
#pixelAlignEndcapRotZ = True

if "pixelAlignmentLevelBarrel" in dir() and pixelAlignmentLevelBarrel == 3:
    pixelAlignBarrelY = False
    pixelAlignBarrelZ = False
    pixelAlignBarrelRotX = False
    pixelAlignBarrelRotY = False

if "pixelAlignmentLevelEndcaps" in dir() and pixelAlignmentLevelEndcaps == 3:
    pixelAlignEndcapZ = False
    pixelAlignEndcapRotX = False
    pixelAlignEndcapRotY = False

#if "pixelAlignmentLevelBarrel" in dir() and pixelAlignmentLevelBarrel == 22:
   
    
if "pixelAlignmentLevelEndcaps" in dir() and pixelAlignmentLevelEndcaps == 2:
    pixelAlignEndcapZ = False
    pixelAlignEndcapRotX = False
    pixelAlignEndcapRotY = False


   
### SCT alignment DoFs   
#sctAlignBarrel  = True
#sctAlignEndcaps = True
### alignment parameters Barrel   
#sctAlignBarrelX = True
#sctAlignBarrelY = True
#sctAlignBarrelZ = True
#sctAlignBarrelRotX = True
#sctAlignBarrelRotY = True
#sctAlignBarrelRotZ = True
### alignment parameters Endcap   
#sctAlignEndcapX = True
#sctAlignEndcapY = True
#sctAlignEndcapZ = True
#sctAlignEndcapRotX = True
#sctAlignEndcapRotY = True
#sctAlignEndcapRotZ = True

if "sctAlignmentLevelBarrel" in dir() and sctAlignmentLevelBarrel == 3:
    sctAlignBarrelY =  False
    sctAlignBarrelZ = False
    sctAlignBarrelRotX = False
    sctAlignBarrelRotY = False

if "sctAlignmentLevelEndcaps" in dir() and sctAlignmentLevelEndcaps == 3:
    sctAlignEndcapZ = False
    sctAlignEndcapRotX = False
    sctAlignEndcapRotY = False



#if "sctAlignmentLevelBarrel" in dir() and sctAlignmentLevelBarrel == 2:
if "sctAlignmentLevelEndcaps" in dir() and sctAlignmentLevelEndcaps == 2:
    sctAlignEndcapZ = False
    sctAlignEndcapRotX = False
    sctAlignEndcapRotY = False

### alignment parameters
#siAlignX = True
#siAlignY = True
#siAlignZ = True
#siAlignRotX = True
#siAlignRotY = True
#siAlignRotZ = True
#
### alignment parameters
#indetAlignX = True
#indetAlignY = True
#indetAlignZ = True
#indetAlignRotX = True
#indetAlignRotY = True
#indetAlignRotZ = True

