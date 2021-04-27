###
# Copyright (C) 2021 CERN for the benefit of the ATLAS collaboration
###

# Create Physics Validation Web page
# Author: Arnaud Duperrin <duperrin@cppm.in2p3.fr> - March 2021

# python CreatePhysValWebPage.py -h
# python CreatePhysValWebPage.py -i ROC

# Import os module to read directory
import getopt,os,glob,argparse,sys

parser = argparse.ArgumentParser(description='Create the Physics Validation web page:', usage='%(prog)s -i ROC/ ')
parser.add_argument("-i", "--input_dir", help="path to the folder holding the plots (default: ./)", default=os.getcwd())
parser.add_argument("-n", "--number_of_histo", help="number of histo per raw (default is 4)", default=4)
#parser.add_argument("-o", "--output_dir", help="path for the output (default: ./FTAG_PhysValWebPage.html", default=os.getcwd()+"/FTAG_PhysValWebPage.html")
parser.add_argument("-f", "--format", help='format for the plots (default: "*.png")', default="*.png")


##############################################################################################################################
def row_major(alist, sublen):      
  return [alist[i:i+sublen] for i in range(0, len(alist), sublen)]

def col_major(alist, sublen):
  numrows = (len(alist)+sublen-1) // sublen 
  return [alist[i::sublen] for i in range(numrows)]

#example:
#L = ['one','two','three','four','five','six','seven','eight','nine']
#for r in row_major(L, 3): print r
#for r in col_major(L, 3): print r
#for r in row_major(L, 4): print r

def html_table(lol):
  string  = '<html><table>'
  for sublist in lol:
    string += '<tr><th> '
    #print (sublist)
    list_len = len(sublist)

    #First the headers
    for file in sublist:
     idx = sublist.index(file)   
     print("Histo: " + sublist[idx])
     list_end = list_len - idx
     if list_end != 1:
         next_idx = idx + 1
         string += '<p style=\"color:#0000FF\";>'+ str(file)+ '<p> </th><th>'
     else:
         #print("End Of List!")
         string += '<p style=\"color:#0000FF\";>'+ str(file)+ '<p> </th></tr> ' 

    #Then the plots
    string += '<tr><td> '
    for file in sublist:
     idx = sublist.index(file)   
     list_end = list_len - idx
     if list_end != 1:
         next_idx = idx + 1
         string += '<a href="'+str(file)+'"> <img alt="'+str(file)+'" src="'+str(file)+'" width="400" height="300"></a> </td><th>'
     else:
         string += '<a href="'+str(file)+'"> <img alt="'+str(file)+'" src="'+str(file)+'" width="400" height="300"></a> </td><tr>'

    #print("string= "+string)
    #print(".........")     
  string  +=  '</table></html>'
  return string

def list_to_html_table(alist, sublength, column_major=False):
  if column_major:
    lol = col_major(alist, sublength)
  else:
    lol = row_major(alist, sublength)
  return(html_table(lol))
##############################################################################################################################
  

#############################
args = parser.parse_args()
folder = os.path.abspath(args.input_dir)

if not os.path.isdir(folder):
  print("The folder " + folder + " doesn't exist !")
  sys.exit()

#out = args.output_dir
format = args.format
nHisto = int(args.number_of_histo)

#origDir = os.getcwd()
#output_file = os.path.abspath(args.output_dir)


os.chdir(folder)

files = glob.glob(format)

#print ("nHisto = " + str(nHisto))

print("=================================================")
print("Processing histo in directory "+folder)
print("=================================================")
#Start by main ROC directory:
my_htm_page= list_to_html_table(files, nHisto)
#print(my_htm_page)
hs = open("FTAG_PhysValWebPage.html", 'w')
hs.write(my_htm_page)
print("")
print("See "+folder+"/FTAG_PhysValWebPage.html")

folder_eff= folder+"/eff_vs_Lxy"
if (os.path.isdir(folder_eff)):
  os.chdir(folder_eff)
  files = glob.glob(format)
  my_htm_page= list_to_html_table(files, nHisto)
  hs = open("FTAG_PhysValWebPage.html", 'w')
  hs.write(my_htm_page)
  #print("")
  #print("See "+folder_eff+"/FTAG_PhysValWebPage.html")
else:
  print("There is no Efficiency vs Lxy directory: " + folder_eff)


folder_pt= folder+"/eff_vs_pt_ttbar"
is_ttbar = 1
if (os.path.isdir(folder_pt)):
  os.chdir(folder_pt)
  files = glob.glob(format)
  my_htm_page= list_to_html_table(files, nHisto)
  hs = open("FTAG_PhysValWebPage.html", 'w')
  hs.write(my_htm_page)
  #print("")
  #print("See "+folder_eff+"/FTAG_PhysValWebPage.html")
else:
 is_ttbar = 0   
 folder_pt= folder+"/eff_vs_pt_Zprime"
 if (os.path.isdir(folder_pt)):
  os.chdir(folder_pt)
  files = glob.glob(format)
  my_htm_page= list_to_html_table(files, nHisto)
  hs = open("FTAG_PhysValWebPage.html", 'w')
  hs.write(my_htm_page)
  #print("")
  #print("See "+folder_pt+"/FTAG_PhysValWebPage.html")
 else:
  print("There is no Efficiency vs pT directory (ttbar or Z'): " + folder_pt)

#Create the master web page  
os.chdir(folder)
mypage = "<html><table>"
mypage += '<tr><th><p style=\"color:#0000FF\";> FTAG Physics validation web page <p> </th> </tr>'
mypage += '<tr><td> <a href="FTAG_PhysValWebPage.html">' +str(args.input_dir)+'</a>  </td></tr> '
mypage += '<tr><td> <ul> <li> <a href="eff_vs_Lxy/FTAG_PhysValWebPage.html"> Efficiency versus Lxy </a>  </li>  '
if (is_ttbar): 
 mypage += '              <li> <a href="eff_vs_pt_ttbar/FTAG_PhysValWebPage.html"> Efficiency versus pT </a>  </li> </td></tr> '
else:
 mypage += '              <li> <a href="eff_vs_pt_Zprime/FTAG_PhysValWebPage.html"> Efficiency versus pT </a>  </li> </td></tr> '
mypage += "</table></html>"

#print(mypage)
hs = open("FTAG_PhysVal.html", 'w')
hs.write(mypage)
print("See "+folder+"/FTAG_PhysVal.html")

