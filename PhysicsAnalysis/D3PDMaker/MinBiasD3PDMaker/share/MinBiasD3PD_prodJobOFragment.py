# $Id$

# This jobO should not be included more than once:
include.block( "MinBiasD3PDMaker/MinBiasD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
MinBiasD3PDStream_msg = logging.getLogger( "MinBiasD3PD_prodJobOFragment" )

# Check if the configuration makes sense:
if prodFlags.WriteMinBiasD3PD.isVirtual:
    MinBiasD3PDStream_msg.error( "The MinBias D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "MinBias D3PD set to be a virtual stream" )

# Construct the stream and file names:
streamName = prodFlags.WriteMinBiasD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteMinBiasD3PD )
MinBiasD3PDStream_msg.info( "Configuring MinBias D3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

# Create the D3PD stream:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
MinBiasD3PDStream = MSMgr.NewRootStream( streamName, fileName, "MinBiasTree" )



def MinBiasD3PD (d3pdalg = None,
              file = 'minbias.root',
              tuplename = 'MinBiasD3PD',
              streamname = 'd3pdstream',
              **kw):


   # MinBias flags
   from MinBiasD3PDMaker.MinBiasD3PDMakerFlags import minbiasD3PDflags
   from IOVDbSvc.CondDB import conddb
   from AthenaCommon.GlobalFlags import globalflags

   if minbiasD3PDflags.doPixelTracklets():
      MinBiasD3PDStream_msg.info( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> TRACKLETS" )
      pass

   ## Don't do truth on data
   if globalflags.DataSource == 'data':
      minbiasD3PDflags.doMBTruth = False
      pass

   #--------------------------------------------------------------
   # Configure Beam spot service
   #--------------------------------------------------------------

#   from AthenaCommon.GlobalFlags import globalflags
#   if globalflags.DataSource == 'data':
#      include("InDetBeamSpotService/BeamCondSvc.py")
#      conddb.addOverride("/Indet/Beampos", 
#minbiasD3PDflags.BSDBFolderName())
#      pass

   #--------------------------------------------------------------
   # Configure the MinBiasD3PDMaker
   #--------------------------------------------------------------

   import TrackD3PDMaker
   import TrigMbD3PDMaker
   import TriggerD3PDMaker
   import D3PDMakerCoreComps
   import EventCommonD3PDMaker

   ## Event Info
   from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject

   ## Trigger part
   from TrigMbD3PDMaker.BcmRdoD3PDObject import BcmRdoD3PDObject
   from TrigMbD3PDMaker.CtpRdoD3PDObject import CtpRdoD3PDObject
   from TrigMbD3PDMaker.CtpDecisionD3PDObject import CtpDecisionD3PDObject
   from TrigMbD3PDMaker.MbtsContainerD3PDObject import MbtsContainerD3PDObject
   from TrigMbD3PDMaker.MbtsLvl2D3PDObject import MbtsLvl2D3PDObject
   from TrigMbD3PDMaker.SpLvl2D3PDObject import SpLvl2D3PDObject
   from TrigMbD3PDMaker.TrtLvl2D3PDObject import TrtLvl2D3PDObject
   from TrigMbD3PDMaker.TrkCntsEfD3PDObject import TrkCntsEfD3PDObject
   from TriggerD3PDMaker.TrigDecisionD3PDObject import TrigDecisionD3PDObject
   from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata

   ## Tracking part
   #-- TrackD3PDMaker Flags
   from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
   TrackD3PDFlags.doTruth.set_Value_and_Lock(minbiasD3PDflags.doMBTruth())
   TrackD3PDFlags.storeDiagonalCovarianceAsErrors.set_Value_and_Lock(True)
   TrackD3PDFlags.storeHitTruthMatching.set_Value_and_Lock(minbiasD3PDflags.doMBTruth())
   TrackD3PDFlags.storeDetailedTruth.set_Value_and_Lock(minbiasD3PDflags.doMBTruth())
   TrackD3PDFlags.storePullsAndResiduals.set_Value_and_Lock(False)
   TrackD3PDFlags.storeBLayerHitsOnTrack.set_Value_and_Lock(False)
   TrackD3PDFlags.storePixelHitsOnTrack.set_Value_and_Lock(True)
   TrackD3PDFlags.storeSCTHitsOnTrack.set_Value_and_Lock(True)
   TrackD3PDFlags.storeTRTHitsOnTrack.set_Value_and_Lock(True)
   TrackD3PDFlags.storeBLayerOutliersOnTrack.set_Value_and_Lock(False)
   TrackD3PDFlags.storePixelOutliersOnTrack.set_Value_and_Lock(True)
   TrackD3PDFlags.storeSCTOutliersOnTrack.set_Value_and_Lock(True)
   TrackD3PDFlags.storeTRTOutliersOnTrack.set_Value_and_Lock(True)
   TrackD3PDFlags.storeBLayerHolesOnTrack.set_Value_and_Lock(False)
   TrackD3PDFlags.storePixelHolesOnTrack.set_Value_and_Lock(True)
   TrackD3PDFlags.storeSCTHolesOnTrack.set_Value_and_Lock(True)
   TrackD3PDFlags.storeTRTHolesOnTrack.set_Value_and_Lock(True)
   TrackD3PDFlags.storeVertexAssociation.set_Value_and_Lock(False)
   TrackD3PDFlags.storeTrackPredictionAtBLayer.set_Value_and_Lock(True)
   TrackD3PDFlags.storeTrackInfo.set_Value_and_Lock(True)
   TrackD3PDFlags.storeTrackFitQuality.set_Value_and_Lock(True)
   TrackD3PDFlags.storeTrackSummary.set_Value_and_Lock(True)
   TrackD3PDFlags.storeTrackSummary.IDHits = True
   TrackD3PDFlags.storeTrackSummary.IDHoles = True
   TrackD3PDFlags.storeTrackSummary.IDSharedHits = True
   TrackD3PDFlags.storeTrackSummary.IDOutliers = True
   TrackD3PDFlags.storeTrackSummary.PixelInfoPlus = False
   TrackD3PDFlags.storeTrackSummary.SCTInfoPlus = False
   TrackD3PDFlags.storeTrackSummary.TRTInfoPlus = False
   TrackD3PDFlags.storeTrackSummary.InfoPlus = False
   TrackD3PDFlags.storeTrackSummary.MuonHits = False
   TrackD3PDFlags.storeTrackSummary.MuonHoles = False
   TrackD3PDFlags.storeTrackSummary.ExpectBLayer = True
   TrackD3PDFlags.storeTrackSummary.HitSum = True
   TrackD3PDFlags.storeTrackSummary.HoleSum = True
   TrackD3PDFlags.storeTrackSummary.HitPattern = True
   TrackD3PDFlags.storeTrackSummary.SiHits = False
   TrackD3PDFlags.storeTrackSummary.TRTRatio = False
   TrackD3PDFlags.storeTrackSummary.PixeldEdx = True #  HI
   TrackD3PDFlags.storeTrackSummary.ElectronPID = False
   TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails.set_Value_and_Lock(3)
   TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails.set_Value_and_Lock(3)
   TrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails.set_Value_and_Lock(3)
   TrackD3PDFlags.storeTrackUnbiasedIPAtPV.set_Value_and_Lock(False)
   TrackD3PDFlags.storeTrackMomentum.set_Value_and_Lock(True)
   TrackD3PDFlags.vertexPositionLevelOfDetails.set_Value_and_Lock(3)
   TrackD3PDFlags.storeVertexFitQuality.set_Value_and_Lock(True)
   TrackD3PDFlags.storeVertexKinematics.set_Value_and_Lock(True)
   TrackD3PDFlags.storeVertexPurity.set_Value_and_Lock(minbiasD3PDflags.doMBTruth())
   TrackD3PDFlags.storeVertexTrackAssociation.set_Value_and_Lock(False)
   TrackD3PDFlags.storeVertexTrackIndexAssociation.set_Value_and_Lock(True)

   #-- Enhanced vertex info configuration
   from TrackD3PDMaker.VertexD3PDAnalysisFlags import VertexD3PDAnalysisFlags
   VertexD3PDAnalysisFlags.useEventInfo=False # No Evt Info reread in Vtx
   VertexD3PDAnalysisFlags.useTruth = minbiasD3PDflags.doMBTruth()
   VertexD3PDAnalysisFlags.useAllVertexCollections = True
   VertexD3PDAnalysisFlags.useTracks=False #Already included in the MinBias D3PD
   VertexD3PDAnalysisFlags.useBeamspot=False  # Already included in the MinBias D3PD
   VertexD3PDAnalysisFlags.useBackgroundWord=True # Components mostly included, just to be safe
   VertexD3PDAnalysisFlags.useTrigger=False # Already included in the MinBias D3PD
   VertexD3PDAnalysisFlags.useSecondaryVertex=False
   VertexD3PDAnalysisFlags.useMET=False
   VertexD3PDAnalysisFlags.useElectrons=False
   VertexD3PDAnalysisFlags.useMuons=False
   VertexD3PDAnalysisFlags.usePhotons=False
   VertexD3PDAnalysisFlags.useJets=False
   VertexD3PDAnalysisFlags.useTaus=False

   #-- TrackD3PDMaker configuration
   from TrackD3PDMaker.TruthTrackD3PDObject import TruthTrackD3PDObject
   from TrackD3PDMaker.TruthVertexD3PDObject import TruthVertexD3PDObject
   from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject
   from TrackD3PDMaker.TrackD3PDObject import PixelTrackD3PDObject
   from TrackD3PDMaker.TrackD3PDObject import SCTTrackD3PDObject
   from TrackD3PDMaker.TrackD3PDObject import TRTTrackD3PDObject
   from TrackD3PDMaker.TrackD3PDObject import ResolvedTracksD3PDObject
   from TrackD3PDMaker.VertexD3PDObject import PrimaryVertexD3PDObject
   from TrackD3PDMaker.VertexD3PDObject import BuildVertexD3PDObject
   SecVertexD3PDObject = BuildVertexD3PDObject(_prefix='secVx_',
                                             _label='secVx',
                                             _sgkey='SecVertices',
                                             trackTarget='trk',
                                             trackPrefix='trk_',
                                             trackType='Rec::TrackParticleContainer')
   from TrackD3PDMaker.V0D3PDObject import V0D3PDObject
   from TrackD3PDMaker.BeamSpotD3PDObject import BeamSpotD3PDObject

   ## MinBias part
   from MinBiasD3PDMaker.UnassociatedHitsD3PDObject import UnassociatedHitsD3PDObject


   if not d3pdalg:
      d3pdalg = MSMgr.NewRootStream(StreamName = streamname, FileName = file, TreeName = tuplename)


   ## Add blocks to the tree
   # d3pdalg += EventInfoD3PDObject(10, prefix='ei_')
   d3pdalg += EventInfoD3PDObject(10) 
   d3pdalg += TrackParticleD3PDObject(10)
   d3pdalg += PrimaryVertexD3PDObject(10)
   if minbiasD3PDflags.doBeamSpot():
      d3pdalg += BeamSpotD3PDObject(10)
   if minbiasD3PDflags.doUnassociatedHits():
      d3pdalg += UnassociatedHitsD3PDObject(10)
   if minbiasD3PDflags.doTrigger():
      d3pdalg += CtpRdoD3PDObject(10)
      d3pdalg += CtpDecisionD3PDObject(10)
      d3pdalg += MbtsContainerD3PDObject(10)
      d3pdalg += TrigDecisionD3PDObject(10)
      addTrigConfMetadata( d3pdalg )
   if minbiasD3PDflags.doDetailedTrigger():
      d3pdalg += BcmRdoD3PDObject(10)
      d3pdalg += MbtsLvl2D3PDObject(10)
      d3pdalg += SpLvl2D3PDObject(10)
      d3pdalg += TrtLvl2D3PDObject(10)
      d3pdalg += TrkCntsEfD3PDObject(10)
   if minbiasD3PDflags.doMBTruth():
      d3pdalg += TruthTrackD3PDObject(10)
      d3pdalg += TruthVertexD3PDObject(10)
   if minbiasD3PDflags.doPixelTracklets():
      d3pdalg += PixelTrackD3PDObject(10)
   if minbiasD3PDflags.doSCTTracklets():
      d3pdalg += SCTTrackD3PDObject(10)
   if minbiasD3PDflags.doTRTTracklets():
      d3pdalg += TRTTrackD3PDObject(10)
   if minbiasD3PDflags.doResolvedTracklets():
      d3pdalg += ResolvedTracksD3PDObject(10)
   if minbiasD3PDflags.doV0s():
      d3pdalg += V0D3PDObject(10, sgkey="V0Candidates")
      d3pdalg += SecVertexD3PDObject(10)

   if minbiasD3PDflags.doLucid():
      from TrigMbD3PDMaker.LucidRawD3PDObject import LucidRawD3PDObject
      d3pdalg += LucidRawD3PDObject(10)
      if globalflags.DataSource=='geant4':
          from TrigMbD3PDMaker.LucidDigitD3PDObject import LucidDigitD3PDObject
          d3pdalg += LucidDigitD3PDObject(10)

   if minbiasD3PDflags.doZDC() and globalflags.DataSource == "data":
      ##ZDC object
      from ForwardDetectorsD3PDMaker.ZdcD3PDObject import ZdcD3PDObject
      from ForwardDetectorsD3PDMaker.ZdcDigitsD3PDObject import ZdcDigitsD3PDObject
      d3pdalg += ZdcD3PDObject(10)
      d3pdalg += ZdcDigitsD3PDObject(10)

   from CaloD3PDMaker.MBTSTimeD3PDObject import MBTSTimeD3PDObject
   d3pdalg += MBTSTimeD3PDObject (10)

   from TrackD3PDMaker.VertexGroupD3PD import VertexGroupD3PD
   VertexGroupD3PD(d3pdalg)


   def _args (level, name, kwin, **kw):
      kw = kw.copy()
      kw['level'] = level
      for (k, v) in kwin.items():
         if k.startswith (name + '_'):
               kw[k[len(name)+1:]] = v
      return kw

   #--------------------------------------------------------------
   # Clusters
   #--------------------------------------------------------------

   from CaloD3PDMaker.ClusterD3PDObject   import ClusterD3PDObject 

   if minbiasD3PDflags.doClusterHad():
      # define clusters
      from CaloD3PDMaker import ClusterMomentFillerTool as CMFT
      myMoments = [ CMFT.CENTER_LAMBDA, 'center_lambda',
                     CMFT.LATERAL,      'lateral',
                     CMFT.LONGITUDINAL, 'longitudinal',
                     CMFT.ISOLATION,    'isolation',
                     CMFT.SIGNIFICANCE,     'significance',
                     CMFT.CELL_SIGNIFICANCE,  'cellSignificance',
                     CMFT.CELL_SIG_SAMPLING,  'cellSigSampling'
                  ]

      d3pdalg += ClusterD3PDObject (**_args(0, 'CaloCalTopoCluster', kw,prefix='cl_had_',
                                   include='Moments',
                                    Kinematics_WriteE=True,Moments_Moments=myMoments))


   if minbiasD3PDflags.doClusterEM():
      d3pdalg += ClusterD3PDObject (**_args(0, 'CaloCalTopoCluster', kw,prefix='cl_em_',
                                                Kinematics_WriteE=True,Kinematics_SignalState=0))
   

   #--------------------------------------------------------------
   # Alfa
   #--------------------------------------------------------------

   if globalflags.DataSource == "data":
      IOVDbSvc = Service( "IOVDbSvc" )

      from IOVDbSvc.CondDB import conddb
      ####for other possible servers see dbreplica.config in Athena
   #     #installation
   #   IOVDbSvc.dbConnection="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200"
      if not conddb.folderRequested('/RPO/DCS/BLM'):
         conddb.addFolder("DCS_OFL","/RPO/DCS/BLM")
      if not conddb.folderRequested('/RPO/DCS/FECONFIGURATION'):
         conddb.addFolder("DCS_OFL","/RPO/DCS/FECONFIGURATION")
      if not conddb.folderRequested('/RPO/DCS/HVCHANNEL'):
         conddb.addFolder("DCS_OFL","/RPO/DCS/HVCHANNEL")
      if not conddb.folderRequested('/RPO/DCS/LOCALMONITORING'):
         conddb.addFolder("DCS_OFL","/RPO/DCS/LOCALMONITORING")
      if not conddb.folderRequested('/RPO/DCS/MOVEMENT'):
         conddb.addFolder("DCS_OFL","/RPO/DCS/MOVEMENT")
      if not conddb.folderRequested('/RPO/DCS/RADMON'):
         conddb.addFolder("DCS_OFL","/RPO/DCS/RADMON")
      if not conddb.folderRequested('/RPO/DCS/TRIGGERRATES'):
         conddb.addFolder("DCS_OFL","/RPO/DCS/TRIGGERRATES")
      if not conddb.folderRequested('/RPO/DCS/TRIGGERSETTINGS'):
         conddb.addFolder("DCS_OFL","/RPO/DCS/TRIGGERSETTINGS")

   if jobproperties.Rec.doAlfa:
      from ForwardDetectorsD3PDMaker.AlfaD3PDObject import AlfaD3PDObject
      # d3pdalg += EventInfoD3PDObject(10)
      d3pdalg += AlfaD3PDObject(0)
      from ForwardDetectorsD3PDMaker import AlfaEventHeaderFillerTool
      if globalflags.DataSource == "data":
            AlfaEventHeaderFillerTool.DataType = 1
            from ForwardDetectorsD3PDMaker.AlfaDCSMetadata import addAlfaDcsMetadata
            addAlfaDcsMetadata(d3pdalg)
      elif globalflags.DataSource == "geant4":
            AlfaEventHeaderFillerTool.DataType = 0



MinBiasD3PD( MinBiasD3PDStream )

