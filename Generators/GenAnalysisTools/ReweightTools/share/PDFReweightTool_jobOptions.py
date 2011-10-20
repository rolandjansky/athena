
include.block("ReweightTools/PDFReweightTool_jobOptions.py")

from AthenaCommon.AppMgr import ToolSvc
from ReweightTools.ReweightToolsConf import PDFReweightTool
ToolSvc += PDFReweightTool("pdfRwTool")

##Default propearty values. No re-weighting will be performed
##
# If re-weigthing is during event generation then set True 
ToolSvc.pdfRwTool.GeneratorUse = False 
# If re-weighting to the different c.m. is required set True
ToolSvc.pdfRwTool.DifferentCMEnergies = False
# If re-qeighting between the different c.m. energies, then
# these two proppierties set the original beam energy used in
# events generation and the new beam energy to what the events
# are required to be re-weighted
ToolSvc.pdfRwTool.OrigBeamEnergy = 0.	#5.0 #TeV
ToolSvc.pdfRwTool.NewBeamEnergy = 0.	#3.5 #TeV
# Original PDF set name used in events generation
ToolSvc.pdfRwTool.OrigPDFSetName = ''	#'cteq66.LHgrid'
# New PDF set name to which events are wanted to be re-weighted
# Leave it empty if events are required to be re-weighted using 
# the original error PDF set
ToolSvc.pdfRwTool.PDFSetName = ''	#MRST2006nnlo.LHgrid 
# Set PDF set central value LHAGLUE number to be stored in an events 
# weight container. The number of PDF set members will be stored as well
# If the new PDF set is not specified, then set the original PDF LHAGLUE value.
# Otherwise, set the new PDF set central value PDF LHAGLUE number
ToolSvc.pdfRwTool.PDFSetID = 0	#10550 #20550
# Set the input McEventCollection name. Required if run on a POOL file
ToolSvc.pdfRwTool.McEventCollection = ''	#'GEN_EVENT' #'GEN_AOD'
# Set the transient McEventCollection name. It will hold the re-weighted events for
# a superwising Athena algorithm. It can be stored in an output POOL file (if the 
# supervisor algorithm produces it) or its content can be dumped into a ntuple
# Relevant when running on the POOL file.
ToolSvc.pdfRwTool.McEventCollectionOut = '' 	#'GEN_EVENT_PDFrw' #'GEN_AOD_PDFrw'
# Acts when re-weitghing goes on the existing McEventCollection. 
# Set True if re-writing of the existing weights is required.
# typically, this is not needed and works properly for Herwig samples only
ToolSvc.pdfRwTool.RewriteWeights = False 
ToolSvc.pdfRwTool.OutputLevel = ERROR 


