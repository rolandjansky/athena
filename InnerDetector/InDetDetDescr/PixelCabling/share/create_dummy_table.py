"""
Create an fill dummy cabling maps
> python create_dummy_table.py

Author: Steffen Maeland
"""

def write_layer(fout, barrel_ec, layer, phirange, etarange):

    robcounter = 16000 + layer*1000

    for phi in phirange:

        fmt40counter = 0
        link40counter = -2
        fmt80counter = 0
        link80counter = -1
        robcounter += 1

        for eta in etarange:

            # Set link/fmt, these must not exceed 0xF
            link40counter += 2
            link80counter += 2
            if link40counter > 0xF:
                link40counter = 1
                fmt40counter += 1
            if link80counter > 0xF:
                link80counter = 0
                fmt80counter += 1

            # Flip sign of Barrel_EC identifier for C-side endcap modules
            bec = barrel_ec
            if eta < 0:
                bec *= -1

            # Unique but meaningless DCS ID
            dcsid = ''
            if bec:
                dcsid = 'D%dA' % layer if layer >= 0 else 'D%dC' % (-layer)
            else:
                dcsid = 'L%d' % layer
            dcsid += '_S%d' % phi
            dcsid += '_M%dA' % eta if eta >= 0 else '_M%dC' % (-eta)


            # Write to file
            fout.write('%d\t%d\t%d\t%d\t%d\t%d\t%X\t%X\t%X\t%X\t%s\n' % (
                        bec,
                        layer,
                        phi,
                        eta,
                        robcounter,
                        robcounter,
                        fmt40counter,
                        link40counter,
                        fmt80counter,
                        link80counter,
                        dcsid
                        ))




def main():

    # write_layer(file, bec, layer, phi range, eta range)
    


    # === ITk_Atlas_IdMapping_ExtBrl32.dat ===
    # Barrel layers {0, 1, 2, 3, 4}, with eta_module range [-X, -1], [1, X] where X = {16, 27, 18, 18, 18}, and phi module from 0 to {15, 15, 31, 43, 53}
    # Endcap layers {0, 1, 2} with eta_module range from 0 to {13, 15, 15}, and phi module from 0 to {35, 47, 59}
    f = open('ITk_Atlas_IdMapping_ExtBrl32.dat', 'w')
    f.write('#Barrel_EC\tLayer_Disk\tPhi_module\tEta_module\tROBID\tRODID\t40FMT\t40Link\t80FMT\t80Link\tDCS Geographical ID\n')

    write_layer(f, 0, 0, range(0,16), range(-16, 0)+range(1,17))
    write_layer(f, 0, 1, range(0,16), range(-27, 0)+range(1,28))
    write_layer(f, 0, 2, range(0,32), range(-18, 0)+range(1,19))
    write_layer(f, 0, 3, range(0,44), range(-18, 0)+range(1,19))
    write_layer(f, 0, 4, range(0,54), range(-18, 0)+range(1,19))
    
    write_layer(f, 2, 0, range(0,36), range(0, 14))
    write_layer(f, 2, 1, range(0,48), range(0, 16))
    write_layer(f, 2, 2, range(0,60), range(0, 16))
    f.close()


    # === ITk_Atlas_IdMapping_ExtBrl4.dat ===
    # Barrel layers {0, 1, 2, 3, 4}, with eta_module range [-X, -1], [1, X] where X = {30, 30, 18, 18, 18}, and phi module from 0 to {15, 15, 31, 43, 53}
    # Endcap layers {0, 1, 2, 3} with eta_module range from 0 to {16, 18, 15, 15}, and phi module from 0 to {23, 35, 47, 59}
    f = open('ITk_Atlas_IdMapping_ExtBrl4.dat', 'w')
    f.write('#Barrel_EC\tLayer_Disk\tPhi_module\tEta_module\tROBID\tRODID\t40FMT\t40Link\t80FMT\t80Link\tDCS Geographical ID\n')

    write_layer(f, 0, 0, range(0,16), range(-30, 0)+range(1,31))
    write_layer(f, 0, 1, range(0,16), range(-30, 0)+range(1,31))
    write_layer(f, 0, 2, range(0,32), range(-18, 0)+range(1,19))
    write_layer(f, 0, 3, range(0,44), range(-18, 0)+range(1,19))
    write_layer(f, 0, 4, range(0,54), range(-18, 0)+range(1,19))
    
    write_layer(f, 2, 0, range(0,24), range(0, 17))
    write_layer(f, 2, 1, range(0,36), range(0, 19))
    write_layer(f, 2, 2, range(0,48), range(0, 16))
    write_layer(f, 2, 3, range(0,60), range(0, 16))
    f.close()


    # === ITk_Atlas_IdMapping_IExtBrl4.dat ===
    # Barrel layers {0, 1, 2, 3, 4}, with eta_module range [-X, -1], [1, X] where X = {21, 23, 18, 18, 18} + an eta_module 0 in layer 0, and phi module from 0 to {17, 17, 31, 43, 53}
    # Endcap layers {0, 1, 2, 3} with eta_module range from 0 to {16, 18, 15, 15}, and phi module from 0 to {23, 35, 47, 59}
    f = open('ITk_Atlas_IdMapping_IExtBrl4.dat', 'w')
    f.write('#Barrel_EC\tLayer_Disk\tPhi_module\tEta_module\tROBID\tRODID\t40FMT\t40Link\t80FMT\t80Link\tDCS Geographical ID\n')

    write_layer(f, 0, 0, range(0,18), range(-21, 22))
    write_layer(f, 0, 1, range(0,18), range(-23, 0)+range(1,24))
    write_layer(f, 0, 2, range(0,32), range(-18, 0)+range(1,19))
    write_layer(f, 0, 3, range(0,44), range(-18, 0)+range(1,19))
    write_layer(f, 0, 4, range(0,54), range(-18, 0)+range(1,19))
    
    write_layer(f, 2, 0, range(0,24), range(0, 17))
    write_layer(f, 2, 1, range(0,36), range(0, 19))
    write_layer(f, 2, 2, range(0,48), range(0, 16))
    write_layer(f, 2, 3, range(0,60), range(0, 16))
    f.close()


    # === ITk_Atlas_IdMapping_InclBrl4.dat ===
    # Barrel layers {0, 1, 2, 3, 4}, with eta_module range [-X, -1], [1, X] where X = {21, 23, 19, 20, 21} + an eta_module 0 in layer 0, and phi module from 0 to {17, 17, 31, 43, 53}
    # Endcap layers {0, 1, 2, 3} with eta_module range from 0 to {16, 18, 15, 15}, and phi module from 0 to {23, 35, 47, 59}
    f = open('ITk_Atlas_IdMapping_InclBrl4.dat', 'w')
    f.write('#Barrel_EC\tLayer_Disk\tPhi_module\tEta_module\tROBID\tRODID\t40FMT\t40Link\t80FMT\t80Link\tDCS Geographical ID\n')

    write_layer(f, 0, 0, range(0,18), range(-21, 22))
    write_layer(f, 0, 1, range(0,18), range(-23, 0)+range(1,24))
    write_layer(f, 0, 2, range(0,32), range(-19, 0)+range(1,20))
    write_layer(f, 0, 3, range(0,44), range(-20, 0)+range(1,21))
    write_layer(f, 0, 4, range(0,54), range(-21, 0)+range(1,22))
    
    write_layer(f, 2, 0, range(0,24), range(0, 17))
    write_layer(f, 2, 1, range(0,36), range(0, 19))
    write_layer(f, 2, 2, range(0,48), range(0, 16))
    write_layer(f, 2, 3, range(0,60), range(0, 16))
    f.close()


    # === ITk_Atlas_IdMapping_InclBrl4_OptRing.dat ===
    f = open('ITk_Atlas_IdMapping_InclBrl4_OptRing.dat', 'w')
    f.write('#Barrel_EC\tLayer_Disk\tPhi_module\tEta_module\tROBID\tRODID\t40FMT\t40Link\t80FMT\t80Link\tDCS Geographical ID\n')

    write_layer(f, 0, 0, range(0,18), range(-21, 22))
    write_layer(f, 0, 1, range(0,18), range(-23, 0)+range(1,24))
    write_layer(f, 0, 2, range(0,32), range(-19, 0)+range(1,20))
    write_layer(f, 0, 3, range(0,44), range(-20, 0)+range(1,21))
    write_layer(f, 0, 4, range(0,54), range(-21, 0)+range(1,22))
    
    write_layer(f, 2, 0, range(0,24), range(0, 15))
    write_layer(f, 2, 1, range(0,36), range(0, 13))
    write_layer(f, 2, 2, range(0,48), range(0, 10))
    write_layer(f, 2, 3, range(0,60), range(0, 12))
    f.close()


    # === ITk_Atlas_IdMapping_InclBrl4_InclinedQuads_step2p0.dat ===
    f = open('ITk_Atlas_IdMapping_InclBrl4_InclinedQuads_step2p0.dat', 'w')
    f.write('#Barrel_EC\tLayer_Disk\tPhi_module\tEta_module\tROBID\tRODID\t40FMT\t40Link\t80FMT\t80Link\tDCS Geographical ID\n')

    write_layer(f, 0, 0, range(0,16), range(-22, 0)+range(1,23))
    write_layer(f, 0, 1, range(0,20), range(-18, 0)+range(1,19))
    write_layer(f, 0, 2, range(0,32), range(-16, 0)+range(1,17))
    write_layer(f, 0, 3, range(0,42), range(-18, 0)+range(1,19))
    write_layer(f, 0, 4, range(0,56), range(-19, 0)+range(1,20))

    write_layer(f, 2, 0, range(0,22), range(0, 1))
    write_layer(f, 2, 0, range(0,16), range(1, 2)) # L0
    write_layer(f, 2, 0, range(0,22), range(2, 3))
    write_layer(f, 2, 0, range(0,16), range(3, 4)) # L0
    write_layer(f, 2, 0, range(0,22), range(4, 5))
    write_layer(f, 2, 0, range(0,16), range(5, 6)) # L0
    write_layer(f, 2, 0, range(0,22), range(6, 7))
    write_layer(f, 2, 0, range(0,16), range(7, 8)) # L0
    write_layer(f, 2, 0, range(0,22), range(8, 15))
    write_layer(f, 2, 1, range(0,32), range(0, 10))
    write_layer(f, 2, 2, range(0,44), range(0, 8))
    write_layer(f, 2, 3, range(0,52), range(0, 9))

    #write_layer(f, 2, 0, range(0,16), range(0, 4))
    #write_layer(f, 2, 1, range(0,22), range(0, 11))
    #write_layer(f, 2, 2, range(0,32), range(0, 10))
    #write_layer(f, 2, 3, range(0,44), range(0, 8))
    #write_layer(f, 2, 4, range(0,52), range(0, 9))
    f.close()


    # === ITk_Atlas_IdMapping_InclBrl4_InclinedQuads_step2p2.dat ===
    f = open('ITk_Atlas_IdMapping_InclBrl4_InclinedQuads_step2p2.dat', 'w')
    f.write('#Barrel_EC\tLayer_Disk\tPhi_module\tEta_module\tROBID\tRODID\t40FMT\t40Link\t80FMT\t80Link\tDCS Geographical ID\n')

    write_layer(f, 0, 0, range(0,16), range(-22, 0)+range(1,23))
    write_layer(f, 0, 1, range(0,20), range(-19, 0)+range(1,20))
    write_layer(f, 0, 2, range(0,32), range(-16, 0)+range(1,17))
    write_layer(f, 0, 3, range(0,42), range(-18, 0)+range(1,19))
    write_layer(f, 0, 4, range(0,54), range(-19, 0)+range(1,20))

    write_layer(f, 2, 0, range(0,22), range(0, 1))
    write_layer(f, 2, 0, range(0,16), range(1, 2)) # L0
    write_layer(f, 2, 0, range(0,22), range(2, 3))
    write_layer(f, 2, 0, range(0,16), range(3, 4)) # L0
    write_layer(f, 2, 0, range(0,22), range(4, 5))
    write_layer(f, 2, 0, range(0,16), range(5, 6)) # L0
    write_layer(f, 2, 0, range(0,22), range(6, 7))
    write_layer(f, 2, 0, range(0,16), range(7, 8)) # L0
    write_layer(f, 2, 0, range(0,22), range(8, 15))
    write_layer(f, 2, 1, range(0,32), range(0, 10))
    write_layer(f, 2, 2, range(0,44), range(0, 8))
    write_layer(f, 2, 3, range(0,52), range(0, 9))

    #write_layer(f, 2, 0, range(0,16), range(0, 4))
    #write_layer(f, 2, 1, range(0,22), range(0, 11))
    #write_layer(f, 2, 2, range(0,32), range(0, 10))
    #write_layer(f, 2, 3, range(0,44), range(0, 8))
    #write_layer(f, 2, 4, range(0,52), range(0, 9))
    f.close()


    # === ITk_Atlas_IdMapping_InclBrl4_InclinedDuals_step2p2.dat ===
    f = open('ITk_Atlas_IdMapping_InclBrl4_InclinedDuals_step2p2.dat', 'w')
    f.write('#Barrel_EC\tLayer_Disk\tPhi_module\tEta_module\tROBID\tRODID\t40FMT\t40Link\t80FMT\t80Link\tDCS Geographical ID\n')

    write_layer(f, 0, 0, range(0,16), range(-22, 0)+range(1,23))
    write_layer(f, 0, 1, range(0,20), range(-19, 0)+range(1,20))
    write_layer(f, 0, 2, range(0,30), range(-22, 0)+range(1,23))
    write_layer(f, 0, 3, range(0,40), range(-25, 0)+range(1,26))
    write_layer(f, 0, 4, range(0,50), range(-26, 0)+range(1,27))

    write_layer(f, 2, 0, range(0,22), range(0, 1))
    write_layer(f, 2, 0, range(0,16), range(1, 2)) # L0
    write_layer(f, 2, 0, range(0,22), range(2, 3))
    write_layer(f, 2, 0, range(0,16), range(3, 4)) # L0
    write_layer(f, 2, 0, range(0,22), range(4, 5))
    write_layer(f, 2, 0, range(0,16), range(5, 6)) # L0
    write_layer(f, 2, 0, range(0,22), range(6, 7))
    write_layer(f, 2, 0, range(0,16), range(7, 8)) # L0
    write_layer(f, 2, 0, range(0,22), range(8, 15))
    write_layer(f, 2, 1, range(0,32), range(0, 10))
    write_layer(f, 2, 2, range(0,44), range(0, 8))
    write_layer(f, 2, 3, range(0,52), range(0, 9))

    #write_layer(f, 2, 0, range(0,16), range(0, 4))
    #write_layer(f, 2, 1, range(0,22), range(0, 11))
    #write_layer(f, 2, 2, range(0,32), range(0, 10))
    #write_layer(f, 2, 3, range(0,44), range(0, 8))
    #write_layer(f, 2, 4, range(0,52), range(0, 9))
    f.close()


    # === ITk_Atlas_IdMapping_LoI.dat ===
    f = open('ITk_Atlas_IdMapping_LoI.dat', 'w')
    f.write('#Barrel_EC\tLayer_Disk\tPhi_module\tEta_module\tROBID\tRODID\t40FMT\t40Link\t80FMT\t80Link\tDCS Geographical ID\n')

    write_layer(f, 0, 0, range(0,16), range(-11, 0)+range(1,12))
    write_layer(f, 0, 1, range(0,16), range(-18, 0)+range(1,19))
    write_layer(f, 0, 2, range(0,32), range(-17, 18))
    write_layer(f, 0, 3, range(0,52), range(-17, 18))
    
    for ilayer in range(0, 4):
        write_layer(f, 2, ilayer, range(0,60), range(0,1))
        write_layer(f, 2, ilayer, range(0,48), range(1,2))
        write_layer(f, 2, ilayer, range(0,36), range(2,3))
    write_layer(f, 2, 4, range(0,60), range(0,1))
    write_layer(f, 2, 4, range(0,48), range(1,2))
    write_layer(f, 2, 4, range(0,40), range(2,3))
    write_layer(f, 2, 5, range(0,60), range(0,1))
    write_layer(f, 2, 5, range(0,48), range(1,2))
    f.close()


    # === ITk_Atlas_IdMapping_LoIVF.dat ===
    f = open('ITk_Atlas_IdMapping_LoIVF.dat', 'w')
    f.write('#Barrel_EC\tLayer_Disk\tPhi_module\tEta_module\tROBID\tRODID\t40FMT\t40Link\t80FMT\t80Link\tDCS Geographical ID\n')

    write_layer(f, 0, 0, range(0,16), range(-11, 0)+range(1,12))
    write_layer(f, 0, 1, range(0,16), range(-18, 0)+range(1,19))
    write_layer(f, 0, 2, range(0,32), range(-17, 18))
    write_layer(f, 0, 3, range(0,52), range(-17, 18))
    
    for ilayer in range(0, 8):
        write_layer(f, 2, ilayer, range(0,22), range(0,1))
        write_layer(f, 2, ilayer, range(0,30), range(1,2))
        write_layer(f, 2, ilayer, range(0,38), range(2,3))
        write_layer(f, 2, ilayer, range(0,26), range(3,4))
        write_layer(f, 2, ilayer, range(0,36), range(4,5))
        write_layer(f, 2, ilayer, range(0,48), range(5,6))
        write_layer(f, 2, ilayer, range(0,60), range(6,7))
    for ilayer in range(8, 12):
        write_layer(f, 2, ilayer, range(0,30), range(0,1))
        write_layer(f, 2, ilayer, range(0,38), range(1,2))
        write_layer(f, 2, ilayer, range(0,26), range(2,3))
        write_layer(f, 2, ilayer, range(0,36), range(3,4))
        write_layer(f, 2, ilayer, range(0,48), range(4,5))
        write_layer(f, 2, ilayer, range(0,60), range(5,6))
    f.close()


main()
