#
# Convert Baur output to LHE output format (like e.g. Madgraph)
# in order to use initmadgraph.F routine by Baur_i
#
# A.Bocci (2/29/2008)

import os,string,sys,time,glob,linecache

#------------------------------------------------------------------------------

baurfile = 'zgamma_zmm_LHC.dat'
#baurfile = 'testb'

madtemplate = 'madtemplate.dat'
madaddition = 'madadd.dat'
evefile = 'event_file.dat'
sumfile = 'Baur_Summary.dat'

outputfile = 'events.lhe'



#------------------------------------------------------------------------------
print 'Start Conversion Baur to Madgraph format'
print 'NOTE: Madgraph format is the new one'


inf = open(baurfile,"r")
madf = open(madaddition,"w")
sumf =  open(sumfile,"r")
outf = open(evefile,"w")

#-------------------------------------------------------------------------
# Collect the information from Baur calculation
# In particular we need the number of events and the cross setion

accepted_events = sumf.readline()
cross_section = sumf.readline()
error_cs = sumf.readline()


w = float(cross_section.split()[0])/float(accepted_events.split()[0])

print w

#-------------------------------------------------------------------------
# Write the final part of the incipit

madf.write("<MGGenerationInfo> \n")
madf.write("#  Number of Events       :"+str(accepted_events))
madf.write("#  Integrated weight (pb) : "+str(cross_section))
madf.write("#  Truncated wgt (pb)     : "+str(error_cs))
madf.write("#  Unit wgt               :  .12471E+01 \n")
madf.write("""</MGGenerationInfo>
</header>
<init>
  2212  2212  0.70000000000E+04  0.70000000000E+04 0 0 10042 10042 3  1
  0.46362161027E+03  0.13386993187E+01  0.46615000000E-01 100
</init>  \n  """)


#madf.write("#********************************************************************\n")


numevent = 0
# Write out the event information
found = False
for line in inf:
    if line.find("#")>=0 and not found:
        found=True
    elif found:
        numevent += 1
        event = line.split()
        outf.write(" <event> \n")

        outf.write("      6 "+"   100   " +"   "+str(w)+"   "+event[66]+ "   " +event[67] + "   " +event[68] +" \n")
        outf.write("   " +event[0]+ "  -1  " + "  0  " + "  0  " +  event[18]+ "   " + event[19]+"   "+ event[36]+"   "+ event[37]+"   "+ event[38]+"   "+ event[39]+"   "+ event[40]+"  0  " +event[30] +"\n" )
        outf.write("   " +event[1]+ "  -1  " + "  0  " + "  0  " +  event[20] + "   "+ event[21] +"   "+ event[41]+"   "+ event[42]+"   "+ event[43]+"   "+ event[44]+"   "+ event[45]+ "  0  " +event[31]+"\n")
        outf.write("   " +event[2]+ "   "+event[6] + "   "+event[10]+ "   "+ event[11]+ "   "+ "   " + event[22]+ "   " + event[23] +"   "+ event[46]+"   "+ event[47]+"   "+ event[48]+"   "+ event[49]+"   "+ event[50] + "  0  " +event[32]+"\n")
        outf.write("   " +event[3]+ "   "+event[7]+ "   " +event[12]+ "   "+ event[13]+ "   " + event[24]+ "   " + event[25] +"   "+ event[51]+"   "+ event[52]+"   "+ event[53]+"   "+ event[54]+"   "+ event[55] + "  0  " +event[33] +"\n")
        outf.write("   " +event[4]+ "   "+event[8]+ "   " +event[14]+ "   "+ event[15]+ "   " + event[26]+ "   " + event[27] +"   "+ event[56]+"   "+ event[57]+"   "+ event[58]+"   "+ event[59]+"   "+ event[60]+ "  0  " +event[34]+ "\n")
        outf.write("   " +event[5]+ "   "+event[9]+ "   " +event[16]+ "   "+ event[17]+ "   " + event[28]+ " " + event[29] +"   "+ event[61]+"   "+ event[62]+"   "+ event[63]+"   "+ event[64]+"   "+ event[65]+ "  0  " +event[35]+"\n")

#        outf.write(" "+event[0]+" "+event[1]+" "+event[2]+" "+event[3]+" "+event[4]+" "+event[5]+"\n")
#        outf.write(" 0 0 "+" "+event[10]+" "+event[12]+" "+event[14]+" "+event[16]+"\n")
#        outf.write(" 0 0 "+" "+event[11]+" "+event[13]+" "+event[15]+" "+event[17]+"\n")
#        outf.write(" "+event[18]+" "+event[20]+" "+event[22]+" "+event[24]+" "+event[26]+" "+event[28]+"\n")
#        outf.write(" "+event[19]+" "+event[21]+" "+event[23]+" "+event[25]+" "+event[27]+" "+event[29]+"\n")
#        outf.write(" -1 -1 "+event[6]+" "+event[7]+" "+event[8]+" "+event[9]+"\n")
#        # outf.write(" "+event[30]+" "+event[31]+" "+event[32]+" "+event[33]+" "+event[34]+" "+event[35]+"\n")
#        outf.write(" -1 -1 0 -1 1 0 \n")
#        i=36
#        k=0
#        max=5*6
#        while k<max:
#            if k%5-4:
#                if not k%5:
#                    outf.write(repr(k/5+1))
#                outf.write(" "+event[i+k])
#            else:
#                outf.write(" "+event[i+k]+"\n")
#            k +=1
#
        outf.write(" </event> \n")

outf.write(" </event> \n")
outf.write(" </LesHouchesEvents> \n")

madf.close()
inf.close()
sumf.close()
outf.close()

# Append the three legs of the file
# 1- Madgraph incipit
# 2- summary
# 3- events

if os.access(outputfile,os.F_OK):
    os.system("rm "+outputfile)
os.system("cat "+ madtemplate +" "+madaddition+" "+evefile+" > "+outputfile)
