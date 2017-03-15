
# specify the ROOT histogram files containing the monitoring plots here that you want to compare
# Anything from 1 file to 4 files can be specified
rootFileNames = ["",   #Put here the cosmic sample
                 "",   #Put here the collision sample
                 "",
                 "",
                 ""
                 ]



# this sets the output directory where plot gifs will be saved. REMEMBER TO CREATE THE DIRECTORY
outputDir = "HandsOn" #Change this name if you want

normaliseHistos = True # set to true if you want to normalise to same area
unitArea = False # set to true if you want to draw residual histos to unit area

#Example on how to fill the residualsDir. Watch out for the order. It must respect the inputFiles order
residualsDir = ["/IDAlignMon/CombinedInDetTracks_alignSelection/Residuals/",
                "/IDAlignMon/ExtendedTracks_alignSelection/Residuals/"]

genericTracksDir = [] #You have to fill this
hitEffDir=[] #You have to fill this

#following array determines ROOT file directory from which TRT residual plots are taken
#for each of the (up to) 4 files

 
    

#following array determines ROOT file directory from which the BeamSpot plots are taken
#for each of the (up to) 4 files
markerStyles = [kOpenCircle,kFullCircle,kOpenSquare, kFullSquare] #determines marker styles for plots
markerColors = [kRed,kBlue,kBlack,kBlack] #determines marker colors for plots
legendTitles = ["","","",""] #determines legend titles
canvasText = [","","",""] #specifies additional canvas text
##----------------------------
