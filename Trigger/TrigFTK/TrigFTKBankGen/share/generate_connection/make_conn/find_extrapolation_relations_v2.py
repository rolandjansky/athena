#!/usr/bin/python

import ftkutils
import sys
import os.path

#### OPTION ####
###############
XL='8'
XL2nd = '12'
Config1stStage = [0,1,2,3,None,4,None,5,6,   7,None]

# standard conversion map from 1st stage to 2nd stage
convXLtoXL2nd = [None] + Config1stStage
###############





###########################################################################
# Print statement handler
#--------------------------------------------------------------------------
#
def Print(arg):
    print sys.argv[0],':   ', arg
#--------------------------------------------------------------------------



###########################################################################
# Help
#--------------------------------------------------------------------------
def help():
    print 'example of running the code'
    print 'python %s sectors_raw_8L_16M_reg0_sub4.patt.bz2 16'%sys.argv[0]


if len(sys.argv)<2:
    help()
    sys.exit()


    

###########################################################################
# Passed argument handling
#--------------------------------------------------------------------------


# Standard arguments
filename_XL_sector = sys.argv[1]          # filename of 8L sector e.g. sectors_raw_8L_16M_reg7_sub14.patt.bz2
nsubregions        = int(sys.argv[2])     # number of subregions to be dealt with, 0-16

# Output to a File? or STDOUT?
try:
    sys.argv[3]
    output = open(sys.argv[3],'w')
except:
    output = sys.stdout
#--------------------------------------------------------------------------



###########################################################################
# Load PyROOT
#--------------------------------------------------------------------------

try:
    import ROOT
    hasROOT = True
except ImportError :
    Print('ROOT library not found, histogram book keeping disabled')
    hasROOT = False
#--------------------------------------------------------------------------





###########################################################################
# Load 8L sector bank
#--------------------------------------------------------------------------
Print('Load sector in ' + filename_XL_sector)

LX_sector_bank = ftkutils.PatternBank(filename_XL_sector)
LX_sector_bank.LoadPatterns()

# create a KD-Tree representation for this bank
LX_kdtree_bank = LX_sector_bank.BuildKDTree()
#--------------------------------------------------------------------------



###########################################################################
# Parse region ID and subregion ID from the 8L sector bank name
#   e.g. from 1st passed arg: sectors_raw_8L_16M_reg'7'_sub'14'.patt.bz2
#--------------------------------------------------------------------------

regid = None
subid = None

filename_XL_sector_items = filename_XL_sector.split('_')

for item in filename_XL_sector_items :

    if item[0:3] == 'reg' :
        regid = int(item.split('.')[0][3:])

    elif item[0:3] == 'sub' :
        subid = int(item.split('.')[0][3:])

# Print
Print('Region ' + str(regid))
Print('Sub-region ' + str(subid))
#--------------------------------------------------------------------------





###########################################################################
# Set the output file
#--------------------------------------------------------------------------

# starting from the 8L sector name
tmp = os.path.basename(filename_XL_sector)    #sectors_raw_8L_16M_reg'7'_sub'14'.patt.bz2

# strip additional extension
tmp = tmp.rstrip('.bz2')                      #sectors_raw_8L_16M_reg'7'_sub'14'.patt

filename_conn = tmp.replace('.patt','.conn')  #sectors_raw_8L_16M_reg'7'_sub'14'.conn
filename_otoc = tmp.replace('.patt','.otoc')  #sectors_raw_8L_16M_reg'7'_sub'14'.otoc

tmp = tmp.replace('sectors','corrgen')
tmp = tmp.replace('.patt','.gcon')

filename_gcon = tmp.replace(XL+'L',XL+XL2nd+'L')      #corrgen_raw_712L_16M_reg'7'_sub'14'.gcon

# Open up each output file
file_conn = open(filename_conn,'w')
file_otoc = open(filename_otoc,'w')
file_gcon = open(filename_gcon,'w')

# print
Print('Output in: ' + filename_conn)
Print('Output in: ' + filename_otoc)
Print('Output in: ' + filename_gcon)
#--------------------------------------------------------------------------




###########################################################################
# Set the output file
#--------------------------------------------------------------------------
if hasROOT :
    if subid != None:
        rootfile = ROOT.TFile.Open("relation_histo_reg%d_sub%d.root" % (regid,subid),"recreate")
    else:
        rootfile = ROOT.TFile.Open("relation_histo_reg%d.root" % (regid),"recreate")
    histo_nrelations = ROOT.TH1F("histo_nrelations","Relations",
                                8,-.5,7.5);
    histo_nrelations.SetXTitle("# "+XL+"L #rightarrow "+XL2nd+"L relations")
    histo_nrelations.SetYTitle("# "+XL+"L sectors")
    histo_nrelsub = []
    for i in xrange(nsubregions) :
        histo_nrelsub.append(ROOT.TH1F("histo_nrelsub%d" % i, "Relations sub %d" % i, 8,-.5,7.5))
#--------------------------------------------------------------------------





###########################################################################
# Load sector and constants for 12 layer
#--------------------------------------------------------------------------

XL2nd_sector_banks = []
XL2nd_kdtree_banks = []

# Loop over all subregions (between 0-16 depending on the sys.argv[2])
# then load all the 12L sector banks for finding relations
for i_subreg in xrange(nsubregions) :

    # Following creates sectors_raw_12L_16M_reg'7'_sub'i_subreg'.patt.bz2
    tmp = filename_XL_sector.replace(XL+"L",XL2nd+"L")
    if subid != None:
        tmp = tmp.replace("sub%d" % subid,"sub%d" % i_subreg)
    filename_XL2nd_sector = tmp

    Print("Load sector in "+ filename_XL2nd_sector)

    tmpbank = ftkutils.PatternBank(filename_XL2nd_sector)
    tmpbank.LoadPatterns()

    # append to the global sector bank list
    XL2nd_sector_banks.append( tmpbank )

    # create a KD-Tree representation for this bank
    XL2nd_kdtree_banks.append( tmpbank.BuildKDTree() )

# load constants from the constant files (12-layer constant)

XL2nd_constant_banks = []

for i in xrange(nsubregions) : 

    # Following creates /path/to/my/bank/gcon/corrgen_raw_12L_16M_reg'7'_sub'i_subreg'.gcon.bz2
    tmp = os.path.abspath(filename_XL_sector)
    tmpbase = os.path.basename(tmp)
    tmpdirpath = os.path.dirname(tmp)
    tmpbase = tmpbase.replace(XL+"L",XL2nd+"L")
    if subid != None:
        tmpbase = tmpbase.replace("sub%d" % subid,"sub%d" % i)
    tmpbase = tmpbase.replace(".patt",".gcon")
    tmpbase = tmpbase.replace("sectors","corrgen")
    tmpdirpath = tmpdirpath.replace("/patt","/gcon")
    filename_XL2nd_sector_gcon = os.path.join(tmpdirpath, tmpbase)

    Print("Load constant in "+filename_XL2nd_sector_gcon)

    tmpbank = ftkutils.ConstantBank(filename_XL2nd_sector_gcon)
    tmpbank.LoadConstants()

    for i, constant in enumerate(tmpbank.constants):
        if constant.pid != str(i):
            print 'pid does not match the index', constant.pid, 

    # Append to global constant bank list
    XL2nd_constant_banks.append( tmpbank )
#--------------------------------------------------------------------------





###########################################################################
# Write header for gcon files
#--------------------------------------------------------------------------
file_gcon.write("! ***           RECONSTRUCTION GEOMETRY CONSTANTS               ***\n")
file_gcon.write("\n")
file_gcon.write("Version 2       ! File format version number\n")
file_gcon.write("\n")
file_gcon.write("! ***           PHI is now in GLOBAL coordinate system          ***\n")
file_gcon.write(" NPLANES\n")
file_gcon.write(" %s\n"%XL2nd)
file_gcon.write(" NSECTORS\n")
file_gcon.write(str(LX_sector_bank.npatterns) +"\n")
file_gcon.write(" NDIM\n")
file_gcon.write(" 2\n")
#--------------------------------------------------------------------------




###########################################################################
# Heart of this file
#--------------------------------------------------------------------------

# deprecated__________________________
#inversion_list = [4,6] # list of 8L layer that in 11L ECP map are stereo
# deprecated__________________________

# create the elements used to search the similar
tocheck = ftkutils.Pattern(int(XL2nd))


###########################################################################
# Diagonosis plots
#--------------------------------------------------------------------------

if hasROOT:
    h_n_conn = ROOT.TH1F('h_n_conn','h_n_conn',12,-.5,11.5);
    h_coverage = ROOT.TH1F('h_coverage','h_coverage',11,0.5,11.5);


# global counter of sector ID for new constant set
counter = 0;
gcon_counter = 0;

# loop of the reference 8L bank
for LX_pattern in LX_sector_bank.patterns :

    # Print the current LX_pattern information (SSID, coverage)
    # Looks like. "Check: ID 0 SSs  17007 30007 40007 25007 31007 37007 43007 in 0 cov 9661"
    print ''
    print ''
    print 'Initial LX_pattern... : ',
    LX_pattern.PrintToFile(output)


    # ----------------------------------------------------------------
    # expand the 8L in an 12L pattern
    # Following for loop creates something like
    # [SSID0,SSID1,SSID2,SSID3,-1,SSID4,-1,SSID5,-1,SSID6,-1]
    #
    for i in xrange(int(XL2nd)) :
        if convXLtoXL2nd[i] == None :
            tocheck.ssid[i] = -1
        else :
            tocheck.ssid[i] = LX_pattern.ssid[convXLtoXL2nd[i]]

    stereoID = [0,5,7,11]
    stereoLayer_indices = [0,1,3]

    # Printing
    print "   |"
    print "   processed into L%s_pattern"%XL2nd
    print "   |"
    print " Final L%s_pattern... "%XL2nd,
    tocheck.PrintToFile(output)
    print "Stereo layers:", stereoID

    
    # ----------------------------------------------------------------
    # loop over all the subregions 12L banks
    #
    matched_XL2nd_sector_infos = []   # This will hold matched 12L sectors
    for i_subreg in xrange(nsubregions) :

        # return the list of similar patterns
        matched_XL2nd_sector_infos_sub = XL2nd_kdtree_banks[i_subreg].FindNode(tocheck)

        if hasROOT :
            histo_nrelsub[i_subreg].Fill(len(matched_XL2nd_sector_infos_sub))
            
        # add the similar pattern in this sector in the global list
        for node in matched_XL2nd_sector_infos_sub :
            isin = False
            for iold in xrange(len(matched_XL2nd_sector_infos)) :
                if matched_XL2nd_sector_infos[iold][1].pattern.coverage < node.pattern.coverage :
                    matched_XL2nd_sector_infos[iold:iold] = [[i_subreg,node]]
                    isin = True
                    break
            if not isin :
                matched_XL2nd_sector_infos.append([i_subreg,node])

    if hasROOT :
        histo_nrelations.Fill(len(matched_XL2nd_sector_infos))
 
    # Start the line for the connection file "SECTORID NCONN"
    num_matched_XL2nd = len(matched_XL2nd_sector_infos)
    file_conn.write("%d %d" % (LX_pattern.pid, num_matched_XL2nd))
   

    # ----------------------------------------------------------------
    # Here we process each LX_pattern_pid and finish up writing the content
    #
    file_otoc.write("%d" % (LX_pattern.pid))

    if hasROOT:
        h_n_conn.Fill(len(matched_XL2nd_sector_infos))
        for i in xrange(len(matched_XL2nd_sector_infos)):
            #print matched_XL2nd_sector_infos[i][1].pattern.coverage
            h_coverage.Fill(i+1,matched_XL2nd_sector_infos[i][1].pattern.coverage)

    # If we have connections
    if len(matched_XL2nd_sector_infos)!=0:
        # loop over connection
        for iconn in xrange(len(matched_XL2nd_sector_infos)):
            # L12_sector with largest coverage is the first element of matched_XL2nd_sector_infos
            XL2nd_info_lg_cov     = matched_XL2nd_sector_infos[iconn]
            sub_id_lg_cov         = XL2nd_info_lg_cov[0]
            matched_XL2nd_lg_cov  = XL2nd_info_lg_cov[1].pattern

            # L12 info is written here.
            file_otoc.write(" %d" % sub_id_lg_cov)
            file_otoc.write(" %d" % matched_XL2nd_lg_cov.pid)

            for i in matched_XL2nd_lg_cov.ssid:
                    file_otoc.write(" "+str(i))

            file_otoc.write(" %d" % XL2nd_info_lg_cov[1].pattern.coverage)
            file_otoc.write("\n")


            tmpcon = XL2nd_constant_banks[sub_id_lg_cov].FindConstant(matched_XL2nd_lg_cov.pid)
            # DEBUG ----->
            #if str(XL2nd_constant_banks[1].constants[5056].pid)!='5056':
            #    print 'Montor this :', LX_pattern.pid, XL2nd_constant_banks[1].constants[5056].pid
            #    sys.exit()
            # DEBUG -----|
            try:
                tmpcon.PrintToFile(file_gcon, gcon_counter)
                gcon_counter+=1
            except:
                print matched_XL2nd_lg_cov.pid, sub_id_lg_cov
                sys.exit()

    else:
        file_otoc.write("\n")

    for matched_XL2nd_sector_info in matched_XL2nd_sector_infos :
        print "Find: ",
        matched_XL2nd_sector_info[1].pattern.PrintToFile(output)
        file_conn.write(" %d %d" % (matched_XL2nd_sector_info[0],counter))
        counter+=1
        for ip in xrange(len(stereoID)) :
            file_conn.write(" "+str(matched_XL2nd_sector_info[1].pattern.ssid[stereoID[ip]]))
    file_conn.write("\n")
    if gcon_counter != counter:
        sys.exit("counters don't match")

# Print final message
Print('')
Print('')
Print('')
Print('This find relations session has sucessfully ended')

if hasROOT :
    h_n_conn.Write()
    h_coverage.Write()
    rootfile.Write()
    rootfile.Close()
file_conn.close()

# Fix the total number of sectors
print "fixing"
file_gcon_copy = open(filename_gcon,'r')
lines = file_gcon_copy.readlines()
file_gcon = open(filename_gcon,'w')
for i, line in enumerate(lines):
    if i==8:
        file_gcon.write("%d\n"%(gcon_counter-1));
    else:
        file_gcon.write(line);

#eof
