""" This file takes an alignment output (or input) file and writes out the alignment constants with respect to the nominal (0,0,0,0,0,0) positions in the Cartesian frame, from the euler angles"""

import sys

from ROOT import *

if sys.argv.__len__() == 1:
    print "You have to give an alignmentinput.txt file"
    sys.exit(-1)

onlyL1 = False
onlyL2B = False
onlyL2EC = False
onlyL2ECA = False
onlyL2ECC = False

if sys.argv.__len__() == 3:
    if sys.argv[2] == "-L1":
        onlyL1 = True
    elif sys.argv[2] == "-L2B":
        onlyL2B = True
    elif sys.argv[2] == "-L2EC":
        onlyL2EC = True
    elif sys.argv[2] == "-L2ECC":
        onlyL2ECC = True
    elif sys.argv[2] == "-L2ECA":
        onlyL2ECA = True
    
    
inputFileName = str( sys.argv[1])
inputFile = open(inputFileName, "r")

AlignmentContainer = ""

for line in inputFile:
    words = line.split()
    
    # Get the alignment container
    if words.__len__() == 1:
        AlignmentContainer = words[0].split("/")[3]
        
    
    # Get the lines with the alignable transforms
    if words.__len__() == 9:

        #Get the L1 alignable transforms
        # Barrel
        if AlignmentContainer == 'TRT':
            if words[0] == '-1':
                print "====================================="
                print "The L1 alignment for the Barrel is: "
            elif words[0] == '-2':
                print "====================================="
                print "The L1 alignment for the Endcap C is: "
            elif words[0] == '2':
                print "====================================="
                print "The L1 alignment for the Endcap A is: "
        elif AlignmentContainer == "B0":
            if onlyL1:
                sys.exit()
            elif onlyL2EC or onlyL2ECA or onlyL2ECC:
                continue
            print "==========================="
            print " The L2 alignment for Layer 0, Phi Sector", words[2],"is"
        elif AlignmentContainer == "B1":
            if onlyL1:
                sys.exit()
            elif onlyL2EC or onlyL2ECA or onlyL2ECC:
                continue
            print "==========================="
            print " The L2 alignment for Layer 1, Phi Sector", words[2],"is"
        elif AlignmentContainer == "B2":
            if onlyL1:
                sys.exit()
            elif onlyL2EC or onlyL2ECA or onlyL2ECC:
                continue
            print "==========================="
            print " The L2 alignment for Layer 2, Phi Sector", words[2],"is" 
        elif AlignmentContainer == "L2A":
            if onlyL1:
                sys.exit()
            elif onlyL2B or onlyL2ECC:
                continue
            print "==========================="
            print " The L2 alignment for Endcap A, Wheel",words[1],"StrayLayer",words[2],"is"
        elif AlignmentContainer == "L2C":
            if onlyL1:
                sys.exit()
            elif onlyL2B or onlyL2ECA:
                continue
            print "==========================="
            print " The L2 alignment for Endcap C, Wheel",words[1],"StrayLayer",words[2],"is"
                
        dx =    float(words[3])
        dy =    float(words[4])
        dz =    float(words[5])
        Phi =   float(words[6])
        Theta = float(words[7])
        Psi =   float(words[8])
        
        rotation = TRotation()
        rotation.SetXEulerAngles(Phi, Theta, Psi)

        print "Dx = ",dx
        print "Dy = ",dy
        print "Dz = ",dz
                
        # To get rotation around X
        #============================
        vectorAlongZ = TVector3(0,0,1)
        rotatedVectorAlongZ = rotation * vectorAlongZ
        rotatedVectorProjectedYZPlane = TVector3(0,
                                                 rotatedVectorAlongZ.Y(),
                                                 rotatedVectorAlongZ.Z())
        sign = 1.0
        # clockwise is negative
        if rotatedVectorProjectedYZPlane.Y() < 0:
            sign = -1.0

        rotationAboutX = sign * vectorAlongZ.Angle(rotatedVectorProjectedYZPlane)
        print "Rotx = ",rotationAboutX
        
        # To get rotation around Y
        #============================
        rotatedVectorProjectedXZPlane = TVector3(rotatedVectorAlongZ.X(),
                                                 0,
                                                 rotatedVectorAlongZ.Z())

        sign = 1.0
        # clockwise is negative
        if rotatedVectorProjectedXZPlane.X() < 0:
            sign = -1.0
        
        rotationAboutY = sign*vectorAlongZ.Angle(rotatedVectorProjectedXZPlane)
        print "Roty = ",rotationAboutY
        
        # To get rotation around Z
        #============================
        vectorAlongX = TVector3(1,0,0)
        rotatedVectorAlongX = rotation * vectorAlongX
        rotatedVectorProjectedXYPlane = TVector3(rotatedVectorAlongX.X(),
                                                 rotatedVectorAlongX.Y(),
                                                 0)

        sign = 1.0
        # clockwise is negative
        if rotatedVectorProjectedXYPlane.Y() < 0:
            sign = -1.0
        
                
        rotationAboutZ = sign*vectorAlongX.Angle(rotatedVectorProjectedXYPlane)
        print "Rotz = ",rotationAboutZ
        


        
print sys.argv

