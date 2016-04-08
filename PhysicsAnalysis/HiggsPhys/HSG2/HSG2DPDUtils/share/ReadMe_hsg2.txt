The recommended (and maintained/supported) way of running this DPD making code is through Reco_trf.

The actual DPD making is wrapped into specific scripts in share/hsg2_*maker*.py. these are not meant
to be run as standalone jobOptions. They must the included in a more general RecExCommon job by means
of rec.DPDMakerScripts

Such inclusions are done in the files share/hsg2_*preInclude.py


Presently, dilepton filters can be configured with a specific set of flags:

diLeptonMuQual
diLeptonEQual
diLeptonMuPtCuts
diLeptonEPtCuts
diLeptonMixPtCuts
diLeptonMixQual

They are all lists, and store the desired quality and pt/et cut. In the case
of the mixed sample, the dpd makers assume that the first item in the list
refers to the electron, the second one being for muons.

Default four-lepton skims use the same properties for all the leptons. These
are defined by the flags:

eQual
muQual
fourLeptonPtCut

One can choose whether to accept forwd electrons in the skims, separately for di-lepton and 4-lepton, using the properties

diLeptonAcceptFrwdE
fourLeptonAcceptFrwdE

Some example Reco_trf commands:

1) start from AODs, create both DAODs and D3PDs with 2-lepton and 4-lepton
skims

Reco_trf.py inputAODFile=/tmp/disimone/data10_7TeV.00167607.physics_Egamma.merge.AOD.f341_m745/data10_7TeV.00167607.physics_Egamma.merge.AOD.f341_m745._lb0404._0001.1
outputDAOD_HSG2File=/tmp/disimone/trashDAOD_4L.root
outputNTUP_2LHSG2File=/tmp/disimone/trashD3PD_2L.root
outputNTUP_HSG2File=/tmp/disimone/trashD3PD_4L.root
outputDAOD_2LHSG2File=/tmp/disimone/trashDAOD_2L.root
autoConfiguration=everything
preInclude=HSG2DPDUtils/hsg2_d3pd_2lepton.preInclude.py,HSG2DPDUtils/hsg2_daod_2lepton.preInclude.py,HSG2DPDUtils/hsg2_d3pd_4lepton.preInclude.py,HSG2DPDUtils/hsg2_daod_4lepton.preInclude.py 
preExec='jobproperties.HSG2.fourLeptonPtCut=7*GeV;jobproperties.HSG2.eQual="Loose";jobproperties.HSG2.muQual="inMS";jobproperties.HSG2.fourLeptonMassCut=0*GeV;jobproperties.HSG2.diLeptonMixPtCuts=[10*GeV,10*GeV];jobproperties.HSG2.diLeptonMixQual=["Loose","inMS"];jobproperties.HSG2.diLeptonEPtCuts=[10*GeV,10*GeV];jobproperties.HSG2.diLeptonEQual=["Loose","Loose"];jobproperties.HSG2.diLeptonMuQual=["inMS","inMS"];jobproperties.HSG2.diLeptonMuPtCuts=[10*GeV,10*GeV];jobproperties.HSG2.diLeptonMassCut=0*GeV;jobproperties.HSG2.diLeptonAcceptFrwdE=False;jobproperties.HSG2.fourLeptonAcceptFrwdE=True' maxEvents=100

The choice of outputs is done by the preInclude argument. Each file added
there activates a specific output. It is important that you specify the
corresponding output*File argument as well, or the job will crash.

This simpler example produces only D3PDs starting from AODs, with a di-lepton
skim:

Reco_trf.py inputAODFile=/tmp/disimone/data10_7TeV.00167607.physics_Egamma.merge.AOD.f341_m745/data10_7TeV.00167607.physics_Egamma.merge.AOD.f341_m745._lb0404._0001.1  outputNTUP_2LHSG2File=/tmp/disimone/trashD3PD_2L.root autoConfiguration=everything preInclude=HSG2DPDUtils/hsg2_d3pd_2lepton.preInclude.py  preExec='jobproperties.HSG2.diLeptonMixPtCuts=[10*GeV,10*GeV];jobproperties.HSG2.diLeptonMixQual=["Loose","inMS"];jobproperties.HSG2.diLeptonEPtCuts=[10*GeV,10*GeV];jobproperties.HSG2.diLeptonEQual=["Loose","Loose"];jobproperties.HSG2.diLeptonMuQual=["inMS","inMS"];jobproperties.HSG2.diLeptonMuPtCuts=[10*GeV,10*GeV];jobproperties.HSG2.diLeptonMassCut=0*GeV' maxEvents=500


2) start from DAODs, create D3PDs with no skim

Reco_trf.py outputNTUP_1LHSG2File=/tmp/disimone/trashD3PD.root inputAODFile=/tmp/disimone/trashDAOD.root autoConfiguration=everything preInclude=HSG2DPDUtils/hsg2_d3pd_plain.preInclude.py  preExec='rec.doApplyAODFix.set_Value_and_Lock(False)' maxEvents=100

If you are reading DAODs created from rel15 files, it is important to switch off AODFix using rec.doApplyAODFix.set_Value_and_Lock(False)

3) for MC create plain D3PDs, without any skimming

Reco_trf.py inputAODFile=/tmp/disimone/data10_7TeV.00167607.physics_Egamma.merge.AOD.f341_m745/data10_7TeV.00167607.physics_Egamma.merge.AOD.f341_m745._lb0404._0001.1 outputNTUP_HSG2File=/tmp/disimone/trashD3PD_2L.root autoConfiguration=everything preInclude=HSG2DPDUtils/hsg2_d3pd_plain.preInclude.py  maxEvents=500