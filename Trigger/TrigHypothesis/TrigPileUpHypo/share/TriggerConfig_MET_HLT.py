include.block("TrigMissingETHypo/TriggerConfig_MET_HLT.py")

from TrigL2MissingET.TrigL2MissingETConfig import T2MissingET
from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Met
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypo_met_xx

def TriggerConfig_MET_HLT( L1TE , L1ITEM, sliceId, chain_number, METcut, SETcut, CutType, sigval ):

	l2chain_name = "L2_"
	l2chain_name += sliceId
	l2chain_name += CutType
	l2chain = HLTChain( chain_name=l2chain_name, chain_counter=chain_number, lower_chain_name=L1ITEM, level="L2", prescale="1", pass_through="0")
	l2chain.addTriggerTypeBit(1)
	l2chain.addStreamTag("met")

	efchain_name = "EF_"
	efchain_name += sliceId
	efchain_name += CutType
	efchain = HLTChain( chain_name=efchain_name, chain_counter=chain_number, lower_chain_name=l2chain_name, level="EF", prescale="1", pass_through="0")
	efchain.addTriggerTypeBit(1)
	efchain.addStreamTag("met")

	sequence=[]

	if CutType=="OR" :
		AlgCut = -2.0
	else:
		if CutType=="AND" :
			AlgCut = -1.0
		else :
			AlgCut = sigval

	if TriggerFlags.METSlice.doL2Calo():

		# get configurables of L2 algorithms (FEX, HYPO):
		l2FEXinstance = "L2METfex_"
		l2FEXinstance += "ALLchain"
		#l2FEXinstance += sliceId
		l2Topo = T2MissingET(l2FEXinstance)
		l2HYPOinstance = "L2METhypo_"
		l2HYPOinstance += sliceId
		l2Hypo = EFMetHypo_met_xx(l2HYPOinstance)

		# Set cut values [MeV] and OR of MET and SumET cuts
		l2Hypo.MissingETCut=METcut
		l2Hypo.SumETCut=SETcut
		l2Hypo.CutType=AlgCut

		# set hypo to use L2 feature:
		l2Hypo.METLabel = "T2MissingET"


		if TriggerFlags.MuonSlice.doL2Muon() :
			TE=triggerPythonConfig.addSequence([L1TE,"T2muFastMuon_mu6"], [ l2Topo, l2Hypo], sliceId+"_L2")
		else:
			print "WARNING : cannot setup LVL2 MissingET trigger to use muons, since the muons are not configured (yet) !!!"
			TE=triggerPythonConfig.addSequence([L1TE], [ l2Topo, l2Hypo], sliceId+"_L2")

		print "L2 output TE= %s = %s"%(TE,sliceId+"_L2")
		l2chain.addHLTSignature(TE)

	if TriggerFlags.METSlice.doEFCalo():
		# get configurables of L2 algorithms (FEX, HYPO):
		efFEXinstance = "EFMETfex_"
		#efFEXinstance += "ALLchain"
		efFEXinstance += sliceId
		efcaloMET     = EFMissingET_Met(efFEXinstance)
		print efcaloMET
		efHYPOinstance = "EFMEThypo_"
		efHYPOinstance += sliceId
		efcaloMEThypo = EFMetHypo_met_xx(efHYPOinstance)

		# Set cut values [MeV] and OR of MET and SumET cuts.
		efcaloMEThypo.MissingETCut=METcut
		efcaloMEThypo.SumETCut=SETcut
		efcaloMEThypo.CutType=AlgCut
		print efcaloMEThypo

		if TriggerFlags.MuonSlice.doEFMuon() :
			TE=triggerPythonConfig.addSequence( [TE, "EFID_mu6"], [ efcaloMET, efcaloMEThypo ], sliceId+"_EF")
		else:
			print "WARNING : cannot setup EF MissingET trigger to use muons, since the muons are not configured (yet) !!!"
			TE=triggerPythonConfig.addSequence( [TE], [ efcaloMET, efcaloMEThypo ], sliceId+"_EF")

		efchain.addHLTSignature(TE)

	# --------------------------------------------------------------------
	# Add single electron signature

	triggerPythonConfig.addHLTChain(l2chain)
	triggerPythonConfig.addHLTChain(efchain)
