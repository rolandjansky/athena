if not 'PhotonRecovery_egDetail_output'    in dir():
    PhotonRecovery_egDetail_output    = "egDetailAODReco"

# Set up the cut based photon ID tool
from egammaPIDTools.egammaPhotonCutIDToolBase import egammaPhotonCutIDToolBase
thephotoncutidtool = egammaPhotonCutIDToolBase("thephotoncutidtool")
thephotoncutidtool.EMShowerContainerName = PhotonRecovery_egDetail_output
thephotoncutidtool.EMConversionContainerName = PhotonRecovery_egDetail_output
ToolSvc += thephotoncutidtool
print      thephotoncutidtool

# egammaElectronCutIDTool
from egammaPIDTools.egammaElectronCutIDToolBase import egammaElectronCutIDToolBase
therecoelectroncutid=egammaElectronCutIDToolBase("therecoelectroncutid")
ToolSvc+=therecoelectroncutid
print    therecoelectroncutid

# egammaSoftElectronCutIDTool
from egammaPIDTools.egammaSoftElectronCutIDToolBase import egammaSoftElectronCutIDToolBase
therecosoftelectroncutid=egammaSoftElectronCutIDToolBase("therecosoftelectroncutid")            
ToolSvc+=therecosoftelectroncutid
print    therecosoftelectroncutid

# EPi tool
from egammaPIDTools.egammaEPiVariablesToolBase import egammaEPiVariablesToolBase
therecoepivariables = egammaEPiVariablesToolBase("therecoepivariables")
therecoepivariables.EMShowerContainerName = PhotonRecovery_egDetail_output
therecoepivariables.EMTrackMatchContainerName= PhotonRecovery_egDetail_output
ToolSvc += therecoepivariables
print      therecoepivariables

from egammaPIDTools.egammaIsolationMVATopToolBase import egammaIsolationMVATopToolBase
theIsolationMVATopTool=egammaIsolationMVATopToolBase("theIsolationMVATopTool")
theIsolationMVATopTool.EMShowerContainerName = PhotonRecovery_egDetail_output
ToolSvc += theIsolationMVATopTool
print      theIsolationMVATopTool

# LogLikelihood
from egammaPIDTools.egammaEPiLogLikelihoodToolBase import egammaEPiLogLikelihoodToolBase
therecologlikelihood = egammaEPiLogLikelihoodToolBase("recologlikelihood")
therecologlikelihood.egammaEPiVariablesToolName = therecoepivariables.getFullName()
ToolSvc += therecologlikelihood
print      therecologlikelihood

# NeuralNetwork
from egammaPIDTools.egammaEPiNeuralNetworkToolBase import egammaEPiNeuralNetworkToolBase
thereconeuralnetwork = egammaEPiNeuralNetworkToolBase("recoNeuralNetwork")
thereconeuralnetwork.egammaEPiVariablesToolName = therecoepivariables.getFullName()
ToolSvc += thereconeuralnetwork
print      thereconeuralnetwork
    
# LogLikelihoodToolSE
from egammaPIDTools.egammaEPiLogLikelihoodSEToolBase import egammaEPiLogLikelihoodSEToolBase
therecologlikelihoodse = egammaEPiLogLikelihoodSEToolBase("recoLogLikelihoodSE")
therecologlikelihoodse.EMShowerContainerName = PhotonRecovery_egDetail_output
therecologlikelihoodse.EMTrackMatchContainerName = PhotonRecovery_egDetail_output
ToolSvc += therecologlikelihoodse
print      therecologlikelihoodse

# Hmatrix
from egammaPIDTools.egammaHmatrixToolBase import egammaHmatrixToolBase
therecohmatrix=egammaHmatrixToolBase("recoHmatrix")
therecohmatrix.EMShowerContainerName = PhotonRecovery_egDetail_output
ToolSvc += therecohmatrix
print      therecohmatrix
        
# Boosted decision tree
from egammaPIDTools.egammaEPiBoostedDecisionTreeToolBase import egammaEPiBoostedDecisionTreeToolBase
therecoEPiBoostedDecisionTreeTool=egammaEPiBoostedDecisionTreeToolBase("recoEPiBoostedDecisionTreeTool")
therecoEPiBoostedDecisionTreeTool.egammaEPiVariablesToolName=therecoepivariables.getFullName()
ToolSvc += therecoEPiBoostedDecisionTreeTool
print      therecoEPiBoostedDecisionTreeTool
    
# Fisher tool
from egammaPIDTools.egammaEPiFisherToolBase import egammaEPiFisherToolBase
therecoEPiFisherTool=egammaEPiFisherToolBase("recoEPiFisherTool")
therecoEPiFisherTool.egammaEPiVariablesToolName=therecoepivariables.getFullName()
ToolSvc += therecoEPiFisherTool
print      therecoEPiFisherTool

# Gamma Loglikelihood
from egammaPIDTools.egammaGammaLogLikelihoodToolBase import egammaGammaLogLikelihoodToolBase
therecoGammaLogLikelihoodTool= egammaGammaLogLikelihoodToolBase("recoGammaLogLikelihood")
therecoGammaLogLikelihoodTool.EMShowerContainerName = PhotonRecovery_egDetail_output
ToolSvc += therecoGammaLogLikelihoodTool
print      therecoGammaLogLikelihoodTool
     
# Reconfigure the PID tools
from egammaTools.egammaToolsConf import EMPIDBuilder
PhotonPIDBuilder=EMPIDBuilder("PhotonPIDBuilder")
PhotonPIDBuilder.egammaEPiRingerCaloTool = None
PhotonPIDBuilder.EMShowerContainerName     = PhotonRecovery_egDetail_output
PhotonPIDBuilder.EMTrackMatchContainerName = PhotonRecovery_egDetail_output
PhotonPIDBuilder.EMConversionContainerName = PhotonRecovery_egDetail_output
PhotonPIDBuilder.egammaEPiLogLikelihoodTool = therecologlikelihood.getFullName()
PhotonPIDBuilder.egammaEPiNeuralNetworkTool = thereconeuralnetwork.getFullName()
PhotonPIDBuilder.egammaEPiLogLikelihoodSETool = therecologlikelihoodse.getFullName()
PhotonPIDBuilder.egammaIsolationMVATopTool = theIsolationMVATopTool.getFullName()
PhotonPIDBuilder.egammaHmatrixTool = therecohmatrix.getFullName()
PhotonPIDBuilder.egammaEPiBoostedDecisionTreeTool = therecoEPiBoostedDecisionTreeTool.getFullName()
PhotonPIDBuilder.egammaEPiFisherTool = therecoEPiFisherTool.getFullName()
PhotonPIDBuilder.egammaGamLogLikelihoodTool = therecoGammaLogLikelihoodTool.getFullName()
PhotonPIDBuilder.egammaSoftElectronCutIDTool = therecosoftelectroncutid.getFullName()
###PhotonPIDBuilder.egammaElectronCutIDTool = ""#therecoelectroncutid.getFullName()
PhotonPIDBuilder.egammaPhotonCutIDTools = [thephotoncutidtool.getFullName()]
#PhotonPIDBuilder.Extrapolator = myInDetExtrapolator
ToolSvc += PhotonPIDBuilder
print      PhotonPIDBuilder
       
