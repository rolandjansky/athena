#
# collect parameter from McAtNlo parameter file 
# and set the values for the inout file of the interface
# C. Ay  ... originally conversion script from A.Bocci (package Baur_i)
#

import os,string,sys,time,glob,linecache
def usage():
    print "Usage: %s <eventfile>" % (sys.argv[0])

def checkProcess(string1):
    foundProcess=False

    if not string1.split()[0].isdigit():
        return foundProcess

    string2=string1.replace("/"," ")
    string3=string2.replace("="," ")
    string4=string3.rpartition("-->")
    for i in string4[2].split():
        if string4[0].find(i)>=0 and not foundProcess:
            foundProcess=True
            break
    return foundProcess

if len(sys.argv)==1:
    usage()
    sys.exit()

if os.path.exists(sys.argv[1]):
    eventfile = sys.argv[1]

#------------------------------------------------------------------------------

outputfile = 'inparmMcAtNlo.dat'

#------------------------------------------------------------------------------

inf = open(eventfile,"r")

#-------------------------------------------------------------------------
# Collect the Parameter from McAtNlo calculation
firstLine=False

for line in inf:

    if firstLine:
        break

    pos_excla=line.find("-->")

    if pos_excla<0:
        firstLine=True
        evtnumber=line.split()[0]

    if checkProcess(line):
        process=line.split()[0]
        process_label=line.rpartition("-->")
        
    if line.find("CM energy",pos_excla)>=0:
        energy=line.split()

    if line.find("colliding",pos_excla)>=0 and line.find("particles",pos_excla)>=0:
        particles=line.split()

    if line.find("Lambda_5",pos_excla)>=0 and line.find("scheme",pos_excla)>=0:
        lambda5_scheme=line.split()

    if line.find("PDF group",pos_excla)>=0 and line.find("id number",pos_excla)>=0:
        pdfgroup_id=line.split()

    if line.find("M_top",pos_excla)>=0 and line.find("Gamma_top",pos_excla)>=0:
        Top_mass_width=line.split()

    if line.find("M_W",pos_excla)>=0 and line.find("Gamma_W",pos_excla)>=0:
        W_mass_width=line.split()

    if line.find("M_Z",pos_excla)>=0 and line.find("Gamma_Z",pos_excla)>=0:
        Z_mass_width=line.split()

    if line.find("M_V",pos_excla)>=0 and line.find("Ga_V",pos_excla)>=0 and line.find("GammaX(V)",pos_excla)>=0:
        V_mass_width=line.split()

    if line.find("M_H",pos_excla)>=0 and line.find("Ga_H",pos_excla)>=0 and line.find("GammaX(H)",pos_excla)>=0:
        H_mass_width=line.split()

    if line.find("IL1",pos_excla)>=0 and line.find("IL2",pos_excla)>=0:
        decay=line.split()

    if line.find("IV",pos_excla)>=0 and line.find("IL",pos_excla)>=0:
        decay=line.split()

    if line.find("quark",pos_excla)>=0 and line.find("gluon",pos_excla)>=0 and line.find("masses",pos_excla)>=0:
        massqg=line.split()

    if line.find("Format of v3.1 and higher",pos_excla)>=0:
        format=line.split()[0]

inf.close()

# check if Z mass and width is set
try:
    Z_mass_width
except NameError:
    print "\nZ mass and width not set in eventfile! ... use default!"
    displayZmass=False
else:
    displayZmass=True

# check if W mass and width is set
try:
    W_mass_width
except NameError:
    print "W mass and width not set in eventfile! ... use default!"
    displayWmass=False
else:
    displayWmass=True

# check if H mass and width is set
try:
    H_mass_width
except NameError:
    print "Higgs mass and width not set in eventfile! ... use default!"
    displayHmass=False
else:
    displayHmass=True

# check if Top mass and width is set
displayTopmass=True
try:
    Top_mass_width
except NameError:
    print "Top mass and width not set in eventfile! ... use default!"
    Top_mass_width = list()
    Top_mass_width.append("0.1725E+03")
    Top_mass_width.append("0.1700E+01")
    displayTopmass=False

outf = open(outputfile,"w")
outf.write(" "+"\'"+eventfile+"\'"+"        ! event file\n")
outf.write(" "+evtnumber+"        ! number of events\n")
outf.write(" 1                        ! 0->Herwig PDFs, 1 otherwise\n")
outf.write(" "+"\'"+particles[0]+"\'"+" "+"\'"+particles[1]+"\'"+"        ! hadron types\n")
outf.write(" "+str(float(energy[0])/float(2))+" "+str(float(energy[0])/float(2))+"        ! beam momenta\n")
outf.write(" -"+process+"        ! "+process_label[2])
outf.write(" "+"\'"+pdfgroup_id[0]+"\'"+"        ! PDF group (1)\n")
outf.write(" "+pdfgroup_id[1]+"        ! PDF id number (1)\n")
outf.write(" "+"\'"+pdfgroup_id[0]+"\'"+"        ! PDF group (2)\n")
outf.write(" "+pdfgroup_id[1]+"        ! PDF id number (2)\n")
if float(lambda5_scheme[0]) == 0.2260E+00:
    lambda5_scheme[0] = "-1"
outf.write(" "+lambda5_scheme[0]+"        ! Lambda_5, < 0 for default\n")
outf.write(" "+Top_mass_width[0]+"        ! M_Q\n")
outf.write(" "+massqg[0]+" "+massqg[1]+" "+massqg[2]+" "+massqg[3]+" "+massqg[4]+" "+massqg[5]+"        ! quark and gluon masses\n")
outf.close()


print
print "Some Parameters also need to be set in the jobOption file:"
print "..."
if displayTopmass:
    print "Herwig.HerwigCommand += [ \"rmass 6 "+Top_mass_width[0]+"\"]"
if displayWmass:
    print "Herwig.HerwigCommand += [ \"rmass 198 "+W_mass_width[0]+"\"]"
    print "Herwig.HerwigCommand += [ \"rmass 199 "+W_mass_width[0]+"\"]"
    print "Herwig.HerwigCommand += [ \"gamw "+W_mass_width[1]+"\"]"
if displayZmass:
    print "Herwig.HerwigCommand += [ \"rmass 200 "+Z_mass_width[0]+"\"]"
    print "Herwig.HerwigCommand += [ \"gamz "+Z_mass_width[1]+"\"]"
if displayHmass:
    print "Herwig.HerwigCommand += [ \"rmass 201 "+H_mass_width[0]+"\"]"
    print "Herwig.HerwigCommand += [ \"gamh "+H_mass_width[1]+"\"]"
print "Herwig.HerwigCommand += [ \"modpdf "+pdfgroup_id[1]+"\"]"
print "Herwig.HerwigCommand += [ \"autpdf "+pdfgroup_id[0]+"\"]"
print "..."

    






    



