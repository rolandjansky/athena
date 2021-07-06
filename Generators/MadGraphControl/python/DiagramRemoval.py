# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Code to perform Diagram Removal 1 or 2, either for the matrix element (do_DRX) or for MadSpin (do_MadSpin_DRX)
# Works for tW, tWZ and tWH. Removes the overlap with tt, ttZ and ttH respectively.
# DR1: without interference. DR2: with interference.
# Does not work for tWgamma and tWll.
# Contact: olga.bylund@cern.ch
# Please let me know if you have any questions or problems.

import os,re,glob,shutil,sys,fileinput
from AthenaCommon import Logging
drlog = Logging.logging.getLogger('DiagramRemoval')

#General functions
def find_matrix_files(process_dir):
    return glob.glob(process_dir+"/P*_*/matrix_*.f")


def get_process(infile,test_string='Process:'):
    output = []
    with open(infile,'r') as f:
        for line in f:
            if test_string in line:
                output += [line]
    return output


def return_out_index(process): #which index does the final state particle have
    p_in,p_out = process.split(" > ")
    p_in = p_in.split(" ")
    p_out = p_out.split(" ")
    particles = p_in+p_out

    windex = -1
    bindex = -1
    for i,particle in enumerate(particles):
        if (particle in (["w-","w+"])) and i>=len(p_in): #is it a FS particle?
            windex = i+1
        elif (particle in (["b","b~"])) and i>=len(p_in):
            bindex = i+1

    return bindex, windex


# Finds the amplitudes that overlap with tt(+X)
def find_W_prepare_DRXhack(mfile,bindex,windex,redefine_twidth,which_DR):
    W_vector=[]
    updated_vector=[]
    the_W=""
    with open(mfile,"r") as f, open("mytmp.txt","w") as tmpf:
        to_replace={}
        mylines = f.readlines()
        for i,line in enumerate(mylines):
            if "1,"+str(bindex)+"))" in line.split("(")[-1] and "XXXXX" not in mylines[i]: # is b quark being rewritten? Beware if >10
                bindex=-1
            elif "1,"+str(windex)+"))" in line.split("(")[-1]  and "XXXXX" not in mylines[i]: # is w boson being rewritten? Beware if >10
                windex=-1

            if W_vector!=[]:
                for j in range(len(W_vector)):
                    if W_vector[j] in line and "AMP(" in line: #save AMP
                        amplitude = line.split(",")[-1]
                        if which_DR==1:
                            tmpf.write("      "+amplitude[:-2]+"=(0d0,0d0)\n") #set to zero (DR)
                        elif which_DR==2:
                            tmpf.write(amplitude[:-2]+"\n") #save amplitude
                    elif W_vector[j] in line and "AMP" not in line:
                        if W_vector[j]+")" in line: # W is redefined, stop keeping track of it
                            updated_vector.remove(W_vector[j])
                        else: # a new W is defined, containing a flagged W. keep track of it
                            the_W = line.split(",")[-2]+","+line.split(",")[-1][0:-2]
                            updated_vector.append(the_W)
                W_vector=updated_vector

            if "W(1,"+str(bindex)+")" in mylines[i] and "W(1,"+str(windex)+")" in mylines[i] and "MDL_WT" in mylines[i]:
                the_W = line.split(",")[-2]+","+line.split(",")[-1][0:-2]
                W_vector.append(the_W)
                updated_vector=W_vector

                if redefine_twidth:
                    edited_line = line.replace("MDL_WT","MDL_WT_nonzero")
                    to_replace[i]=edited_line
    return to_replace

#-------------------------------------
def do_DR1_hacks(mfile,tmpfile):
    drlog.info("performing DR1 for file "+mfile)
    with open(tmpfile,"r") as mytmp:
        for line in fileinput.input(mfile, inplace=True):
            # fileinput redirects the print output to mfile
            if "JAMP(1)=" in line:
                print("\nC     DR hack")
                for hackline in mytmp:
                    print(hackline)
                print("C     End DR hack. \n")
            print(line)

#-------------------------------
#DR2 helper functions
def do_driver_hacks(driver_filename): #takes care of negative weights in code for driver.f for madspin
    test_string1 = "weight.gt."
    repl_string1 = "abs(weight).gt."

    test_string2 = "maxweight=weight"
    repl_string2 = "maxweight=abs(weight)"

    source = open(driver_filename,"r")
    mylines = source.readlines()
    with open(driver_filename,"w") as source:
        for i,line in enumerate(mylines):
            if test_string1 in line:
                mylines[i] = line.replace(test_string1,repl_string1)
            if test_string2 in line:
                mylines[i] = line.replace(test_string2,repl_string2)
            source.write(mylines[i])


def do_fks_hacks(pdir): #takes care of negative weights for event generation code
    shutil.copyfile(pdir+"fks_singular.f", pdir+"fks_singular.f~" )

    test_string = "wgt.lt.0.d0"
    end_string = "return"

    with open(pdir+"fks_singular.f","w") as destination, open(pdir+"fks_singular.f~") as source:
        mylines = source.readlines()
        dont_comment = True
        for i,line in enumerate(mylines):
            if dont_comment:
                if test_string in line:
                    dont_comment=False
                    destination.write("C"+line)
                else:
                    destination.write(line)
            else:
                if end_string in line:
                    dont_comment=True
                    destination.write(line)
                else:
                    destination.write("C"+line)

    os.remove(pdir+"fks_singular.f~")

def do_coupl_hacks(pdir): #define the width of the top quark for the event generation code
    shutil.copyfile(pdir+"coupl.inc", pdir+"coupl.inc~" )

    test_string1 = "MDL_WT"
    test_string2 = "WIDTHS"

    with open(pdir+"../Cards/param_card.dat","r") as pcard:
        the_lines = pcard.readlines()
        for line in the_lines:
            if "DECAY" in line and " 6 " in line:
                top_width = line.split(" 6 ")[-1].rstrip()
                break

    with open(pdir+"coupl.inc","w") as destination, open(pdir+"coupl.inc~") as source:
        mylines = source.readlines()
        for i,line in enumerate(mylines):
            destination.write(line)
            if test_string1 in line and test_string2 in line:
                destination.write("\n      DOUBLE PRECISION MDL_WT_nonzero\n")
                destination.write("      PARAMETER (MDL_WT_nonzero="+top_width+")\n")

    os.remove(pdir+"coupl.inc~")

#-------------------------
# DR2 specific
def find_jamp(mfile,helperfile,test_string,end_string):

    with open(mfile,"r") as source:
        mylines = source.readlines()
        dont_store = True
        my_jamp = ""
        for i,line in enumerate(mylines):
            if dont_store:
                if test_string in line:
                    dont_store=False
                    my_jamp += line

            else:
                if end_string in line:
                    dont_store=True
                else:
                    my_jamp += line

    with open(helperfile,"w") as destination:
        destination.write(my_jamp)
        drlog.info("my_jamp = "+str(my_jamp))


def do_DR2_hack(mfile, tmpfile, suffix,to_replace,should_replace): #to_replace and should_replace refer to whether to put top propagator on shell - do so for madgraph code but not for madspin
    shutil.copyfile(mfile, mfile+"~" )
    with open(mfile,"w") as destination, open(mfile+"~") as source, open(tmpfile,"r") as mytmp, open(mfile+".jamp1","r") as saved_jamp1, open(mfile+".jamp2","r") as saved_jamp2:
        tmp_content = mytmp.readlines()
        for i,line in enumerate(source):
            if i in to_replace.keys() and should_replace==1:
                destination.write(to_replace[i])
            else:
                destination.write(line)

            if "JAMP(NCOLOR)" in line and "COMPLEX*16" in line:
                destination.write("      REAL*8 MATRIX"+suffix+"_res\n")
                destination.write("      COMPLEX*16 JAMP_res(NCOLOR)\n")
            elif "JAMP(1)=" in line:
                drlog.info("performing DR2 for file "+mfile)
                jamplines = saved_jamp1.readlines()
                my_jamp1 = ""
                for j,jline in enumerate(jamplines):
                        my_jamp1 +=  jline
                jamp_res1 = "JAMP_res(1)="+write_jamp_res(my_jamp1,tmp_content)
            elif "JAMP(2)=" in line:
                jamplines = saved_jamp2.readlines()
                my_jamp2 = ""
                for j,jline in enumerate(jamplines):
                        my_jamp2 +=  jline

                jamp_res2 = "JAMP_res(2)="+write_jamp_res(my_jamp2,tmp_content)

            elif "DENOM(I)" in line:
                line=next(source)
                destination.write(line)
                destination.write("\n      "+jamp_res1)
                destination.write("      "+jamp_res2+"\n")
                hardcode = "      MATRIX"+suffix+"_res = 0.D0\n      DO I = 1, NCOLOR\n        ZTEMP = (0.D0,0.D0)\n        DO J = 1, NCOLOR\n          ZTEMP = ZTEMP + CF(J,I)*JAMP_res(J)\n        ENDDO\n        MATRIX"+suffix+"_res = MATRIX"+suffix+"_res+ZTEMP*DCONJG(JAMP_res(I))/DENOM(I)\n      ENDDO\n      MATRIX"+suffix+" = MATRIX"+suffix+" - MATRIX"+suffix+"_res\n"
                destination.write(hardcode)

    os.remove(mfile+"~")

def write_jamp_res(my_jamp_line,tmp_c):
    line = my_jamp_line
    my_jamp_line = my_jamp_line.replace("+",",+")
    my_jamp_line = my_jamp_line.replace("-",",-")
    split_line = my_jamp_line.split(",")[1:]
    jamp_resX = ""
    for amp in tmp_c:
        for term in split_line:
            if amp[:-1] in term:
                jamp_resX = jamp_resX+term

    jamp_prefix = line.split("=")[1].split("*(")
    if len(jamp_prefix)>1:
        jamp_resX = jamp_prefix[0]+"*("+jamp_resX
        if jamp_resX[-2:]!="\n":
            if "))" not in jamp_resX[-4:]:
                jamp_resX=jamp_resX+")\n"
            else:
                jamp_resX=jamp_resX+"\n"

    else:
        if "\n" not in jamp_resX:
            jamp_resX=jamp_resX+"\n"

    return jamp_resX


#####################################################
# Perform DR for matrix element                     #
#####################################################

def do_DRX(DRmode, process_dir):
    drlog.info("in do_DRX, DRmode="+str(DRmode)+", process_dir = "+process_dir)
    pdir=process_dir+'/SubProcesses/'

    if DRmode==2:
        do_fks_hacks(pdir)
        do_coupl_hacks(pdir) 

    mfiles = find_matrix_files(pdir)

    for mfile in mfiles:
        matrix_idx =  re.compile(r"matrix_(\d+).f").search(mfile).group(1)
        process = get_process(mfile)

        for myline in process:
            m2 = myline.split("Process: ")[1]
            the_process = m2.split(" WEIGHTED")[0]

            bindex, windex = return_out_index(the_process)  

        if DRmode==1:
            redefine_twidth=False
        elif DRmode==2:
            redefine_twidth=True
        to_replace = find_W_prepare_DRXhack(mfile,bindex,windex,redefine_twidth,DRmode)

        if os.path.getsize("mytmp.txt")>0: #if there are hacks to make
            if DRmode==1:
                do_DR1_hacks(mfile,"mytmp.txt")
            elif DRmode==2:
                find_jamp(mfile,mfile+".jamp1","JAMP(1)","JAMP(2)")
                find_jamp(mfile,mfile+".jamp2","JAMP(2)","MATRIX")
                do_DR2_hack(mfile, "mytmp.txt","_"+matrix_idx,to_replace,1)

                os.remove(mfile+".jamp1")
                os.remove(mfile+".jamp2")

############################################
#   Perform Diagram Removal for MadSpin    #
############################################
def do_MadSpin_DRX(DRmode, msdirname):

    my_ms_dir=msdirname
    pdir=my_ms_dir+'/production_me/SubProcesses/'
    fdir=my_ms_dir+'/full_me/SubProcesses/'
    full_files=os.listdir(fdir)
    mfiles = find_matrix_files(pdir)

    for mfile in mfiles:
        process = get_process(mfile)

        for myline in process:
            m2 = myline.split("Process: ")[1]
            the_process = m2.split("WEIGHTED")[0]

            bindex, windex = return_out_index(the_process)

        redefine_twidth=False #not needed to redefine top width for MadSpin part
        to_replace = find_W_prepare_DRXhack(mfile,bindex,windex,redefine_twidth,DRmode)

        if os.path.getsize("mytmp.txt")>0: #if there are hacks to make
            if DRmode==1:
                do_DR1_hacks(mfile,"mytmp.txt")
            elif DRmode==2:
                find_jamp(mfile,mfile+".jamp1","JAMP(1)","JAMP(2)")
                find_jamp(mfile,mfile+".jamp2","JAMP(2)","MATRIX")
                do_DR2_hack(mfile,"mytmp.txt","_PROD",to_replace,0) #do not put top propagator on shell

            prefix = mfile.replace("/matrix_prod.f","")
            prefix_full = prefix.replace("production_me","full_me")

            for f_file in full_files:
                if prefix_full in fdir+f_file:
                    full_file_matrix = fdir+f_file+"/matrix.f"
                    full_file_matrix_prod = fdir+f_file+"/matrix_prod.f"
                    if DRmode==1:
                        do_DR1_hacks(full_file_matrix, "mytmp.txt")
                        do_DR1_hacks(full_file_matrix_prod, "mytmp.txt")
                    elif DRmode==2:
                        find_jamp(full_file_matrix,full_file_matrix+".jamp1","JAMP(1)","JAMP(2)")
                        find_jamp(full_file_matrix,full_file_matrix+".jamp2","JAMP(2)","MATRIX")
                        find_jamp(full_file_matrix_prod,full_file_matrix_prod+".jamp1","JAMP(1)","JAMP(2)")
                        find_jamp(full_file_matrix_prod,full_file_matrix_prod+".jamp2","JAMP(2)","MATRIX")

                        do_DR2_hack(full_file_matrix, "mytmp.txt","",to_replace,0) #do not put top propagator on shell
                        do_DR2_hack(full_file_matrix_prod, "mytmp.txt","_PROD",to_replace,0) #do not put top propagator on shell
                        do_driver_hacks(fdir+f_file+"/driver.f")

    drlog.info("finished do_MadSpin_DRX")
    sys.stdout.flush()
