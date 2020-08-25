# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

""" Missing ET slice signatures """

__author__ = "J. Burr"
__version__ = ""
__doc__ = "Reimplementation of MET trigger configuration code for code rewrite. """

"""
    First, a quick note. The old code used this 'L2EFChainDict' object, but the
    only method of this that was actually used anywhere else was the
    generateHLTChainDef function called in the generateMETChainDefs module.
    Therefore I'm moving the code away from this older class as it just seemed
    to obfuscate a lot of things.
"""


from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.Logging import logging
log = logging.getLogger("TriggerMenu.met.MissingETDef")

import TrigEFMissingET.TrigEFMissingETConfig as FexConf
from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__FexSum
from TrigMissingETHypo.TrigMissingETHypoConfig import (
        METHypo,
        TEHypo,
        XSHypo)
from TriggerMenu.menu.ChainDef import ChainDef
from TriggerMenu.menu import DictFromChainName
from TriggerMenu.jet.JetDef import generateHLTChainDef as generateJetChainDef
from TriggerMenu.bjet.generateBjetChainDefs import (
        generateChainDefs as generateBJetChainDef)
from TriggerMenu.muon.generateMuonChainDefs import (
        generateChainDefs as generateMuonChainDef)
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo
from TrigInDetConf.TrigInDetFTKSequence import TrigInDetFTKSequence
from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
import re

class METChainBuilder(object):
    def __init__(self, chainDict):
        """ Create the chain def. """
        self.chainDict = chainDict
        self.chainPart = chainDict["chainParts"]
        self._dictBuilder = DictFromChainName.DictFromChainName()

        log.info("Creating chain def for chain HLT_{0}".format(self.chainDict["chainName"]) )

        # Create the chain def object that will be output
        self.chain_def = ChainDef(
                chain_name="HLT_{0}".format(self.chainDict["chainName"]),
                level="HLT",
                lower_chain_name=self.chainDict["L1item"],
                chain_counter=chainDict["chainCounter"])

        # Keep track of which input items we've added to our sequence
        self._added_inputs = []

        # Add the cell preselection if we need it
        self.add_cell_presel()

        # Get the right fex
        fex = FexConf.getFEXFromDict(self.chainPart)

        # Work out which inputs we need
        fex_inputs = self.assemble_fex_inputs(fex)

        # Create and add the main fex
        fex_te = self.add_fex(fex, fex_inputs)

        # See if we need the muon correction
        if self.chainPart["EFmuonCorr"]:
            muon_fex = FexConf.MuonFex()
            muon_fex_inputs = self.assemble_fex_inputs(muon_fex)
            muon_fex_te = self.add_fex(muon_fex, muon_fex_inputs)
            # We need to create a good name for this sum
            name = fex_te + "_muon_sum"
            if name.startswith("EF_xe_"):
                name = name[6:]
            # Now create the sum
            sum_fex = HLT__MET__FexSum(
                    "METFexSum_"+name,
                    MissingETOutputKey="TrigEFMissingET_"+name)
            fex_te = self.add_fex(sum_fex, [fex_te, muon_fex_te])

            # Make sure that the hypo applies to this fex
            fex = sum_fex

        self.add_hypo(
                fex=fex,
                int_threshold=int(self.chainPart["threshold"]),
                fex_te=fex_te,
                hypo_type=self.chainPart["trigType"])


    def add_cell_presel(self):
        """ Add the cell preselection, if necessary """
        if self.chainPart["chainPartName"] != self.chainDict["chainName"]:
            # Only add the preselection to single-step chains
            return
        if self.chainPart["EFrecoAlg"] == "cell":
            # No sense adding a cell preselection to a cell trigger
            return
        if 'v5' in TriggerFlags.triggerMenuSetup() or 'v6' in TriggerFlags.triggerMenuSetup():
            # Don't do this in old menu versions
            return
        # Try to figure out the L1 threshold
        match = re.match(r"L1_XE(\d+)", self.chainDict["L1item"])
        if not match:
            log.info("Pure MET chain doesn't have an L1_XE seed! Will not apply the cell preselection")
            return
        l1_threshold = int(match.group(1) )
        if l1_threshold < 50:
            log.info("L1 threshold below 50, will not apply cell presel")
            return

        presel_fex = FexConf.getFEX("cell")
        presel_te = self.add_fex(presel_fex, [])
        self.add_hypo(presel_fex, fex_te=presel_te, int_threshold=50)

    def assemble_fex_inputs(self, fex):
        """
            Get the inputs for a fex.

            The necessary sequences will be added to the list.
        """
        # Figure out which inputs we need
        full_input_list = fex.request_inputs()
        fex_inputs = []
        for input_type in full_input_list:
            if input_type == "clusters":
                fex_inputs += self._add_cluster_inputs()
            elif input_type == "jets":
                fex_inputs += self._add_jet_inputs()
            elif input_type == "roi_tracks":
                fex_inputs += self._add_jetroi_track_inputs()
            elif input_type == "fs_tracks":
                fex_inputs += self._add_fs_track_inputs()
            elif input_type == "muons":
                fex_inputs += self._add_muon_inputs()
            else:
                log.error("Unexpected input {0} requested - skipping!".format(input_type) )
                log.error("This will probably break things later...")
        return fex_inputs

    def add_fex(self, fex, fex_inputs):
        """ 
            Add a new FEX to the sequence.

            Returns the output TE created for the new fex
        """
        # Work out what we want to call the output
        te_name = self._te_name_from_fex(fex)
        if "LRT" in self.chainPart["addInfo"]:
            te_name = te_name+"_lrt"
        # Add the fex to our sequences
        self.chain_def.addSequence(
                [fex],
                te_in=fex_inputs if fex_inputs else [""],
                te_out=te_name)
        # Add the new TE to our signatures
        self.chain_def.addSignature(len(self.chain_def.signatureList)+1, [te_name])
        return te_name

    def add_hypo(self, fex, int_threshold, fex_te=None, hypo_type="xe"):
        """
            Add a hypo whose decision is based on the output of the provided FEX

            Return the name of the hypo's TE
        """
        if fex_te is None:
            # Assume we made it with the name that this class would have
            # generated
            fex_te = self._te_name_from_fex(fex)
        base_name = fex_te
        if base_name.startswith("EF_xe_"):
            base_name = base_name[6:]
        if hypo_type == "xe":
            hypo_cls = METHypo
            threshold = int_threshold * GeV
        elif hypo_type == "te":
            hypo_cls = TEHypo
            threshold = int_threshold * GeV
        elif hypo_type == "xs":
            hypo_cls = XSHypo
            threshold = int_threshold / 10.
        else:
            log.error("Unsupported hypo type!! No hypo will be created")
            return
        hypo = hypo_cls(
                "HLT_HYPO_{0}_{1}{2}".format(base_name, hypo_type, int_threshold),
                Threshold = threshold)
        self._connect_hypo(hypo=hypo, fex=fex)
        hypo_te_name = "{0}_{1}{2}".format(fex_te, hypo_type, int_threshold)
        # Add the hypo to our sequences
        self.chain_def.addSequence(
                [hypo],
                te_in=[fex_te],
                te_out=hypo_te_name)
        # Add the new TE to our signatures
        self.chain_def.addSignature(len(self.chain_def.signatureList) + 1, [hypo_te_name])
        return hypo_te_name



    def _add_cluster_inputs(self):
        """ Add the cluster inputs """
        chain = ["j0_{calib}_{jetCalib}".format(**self.chainPart), '', [], ["Main"], ["RATE:SingleJet", "BW:Jet"], -1]
        jet_dict = self._dictBuilder.getChainDict(chain)
        # We also only need up to the third term
        seq = generateJetChainDef(jet_dict).sequenceList[:3]
        if "clusters" not in self._added_inputs:
            self._copy_input_sequence(seq)
            self._added_inputs.append("clusters")
        return [seq[-1]["output"]]

    def _add_jet_inputs(self):
        """ Add the jet inputs """
        chain = ["j0_{calib}_{jetCalib}".format(**self.chainPart), '', [], ["Main"], ["RATE:SingleJet", "BW:Jet"], -1]
        jet_def = generateJetChainDef(self._dictBuilder.getChainDict(chain))
        # Sequence list is
        # - RoI creator: create a dummy unseeded RoI to work in the fullscan region
        # - TrigCaloCellMaker: collect all the cells in a single container - maybe
        #   this is how we should actually be doing cell...?
        # - TrigCaloClusterMaker: Make the clusters!
        # - TrigHLTEnergyDensity: Calculates the overall energy density (I think)
        # - TrigHLTJetRecFromCluster: Make and calibrate the jets
        seq = jet_def.sequenceList[:5]
        if "clusters" not in self._added_inputs:
            self._copy_input_sequence(seq[:3])
            self._added_inputs.append("clusters")
        if "jets" not in self._added_inputs:
            self._copy_input_sequence(seq[3:])
            self._added_inputs.append("jets")
        return [seq[-1]["output"]]

    def _add_jetroi_track_inputs(self):
        chain = ["j20_{calib}_{jetCalib}_boffperf_split".format(**self.chainPart), '', [], ["Main"], ["RATE:SingleBJet", "BW:BJet"], -1]
        bjet_dict = self._dictBuilder.getChainDict(chain)
        bjet_dict["topoThreshold"] = None
        bjet_def = generateBJetChainDef(bjet_dict)
        seq = bjet_def.sequenceList[:-2]
        if "clusters" not in self._added_inputs:
            self._copy_input_sequence(seq[:3])
            self._added_inputs.append("clusters")
        if "jets" not in self._added_inputs:
            self._copy_input_sequence(seq[3:5])
            self._added_inputs.append("jets")
        if "roi_tracks" not in self._added_inputs:
            self._copy_input_sequence(seq[5:])
            self._added_inputs.append("roi_tracks")
        return [seq[-1]["output"], seq[-5]["output"]]
        
    def _add_fs_track_inputs(self):
        # Note, this approach explicitly disallows using both fullscan and FTK
        # tracks in a sequence. I think this is fine though
        # Create a dummy algorithm - I think this is to make a dummy RoI?
        algorithms = [PESA__DummyUnseededAllTEAlgo("EF_DummyFEX_xe")]
        if "FTK" in self.chainPart["addInfo"]:
            trk_algs = TrigInDetFTKSequence(
                    "FullScan", "fullScan", sequenceFlavour=["FTKVtx"]).getSequence()
            algorithms += trk_algs[0] + trk_algs[1]
            te_out = "EF_xe_FTKTrk"
        elif "LRT" in self.chainPart["addInfo"]:
            trk_algs = TrigInDetSequence(
                    "FullScan", "fullScan", 
                    "IDTrig" , sequenceFlavour=["2step"]).getSequence()
            algorithms += trk_algs[0]
            lrt_algorithms = trk_algs[1]
            te_out = "EF_xe_FSTrk"
        else:
            trk_algs = TrigInDetSequence(
                    "FullScan", "fullScan", 
                    "IDTrig", sequenceFlavour=["FTF"]).getSequence()
            algorithms += trk_algs[0]
            te_out = "EF_xe_FSTrk"

        if "fs_tracks" not in self._added_inputs:
            self.chain_def.addSequence(
                    algorithms,
                    te_in=[''],
                    te_out=te_out)
            if "LRT" in self.chainPart["addInfo"]:
                te_in=te_out
                te_out="EF_xe_FSlrtTrk"
                self.chain_def.addSequence(
                    lrt_algorithms,
                    te_in=te_in,
                    te_out=te_out)
            self._added_inputs.append("fs_tracks")
        return [te_out]

    def _add_muon_inputs(self):
        chain = ['mu8', 'L1_MU6',  [], ["Main"], ['RATE:SingleMuon', 'BW:Muon'], -1]
        muon_dict = self._dictBuilder.getChainDict(chain)
        muon_dict["chainCounter"] = 9999
        muon_def = generateMuonChainDef(muon_dict)
        seq = muon_def.sequenceList
        if "muons" not in self._added_inputs:
            self._copy_input_sequence(seq)
            self._added_inputs.append("muons")
        return [seq[-1]["output"]]

    def _copy_input_sequence(self, seq):
        for inp in seq:
            self.chain_def.addSequence(
                    inp["algorithm"],
                    inp["input"],
                    inp["output"],
                    inp["topo_start_from"])

    def _te_name_from_fex(self, fex):
        # Rely on the fact that all of the FEXs have to produce a unique output
        # name for the EDM. By default this only applies to the FEXs that will
        # write something out so we need to ensure elsewhere that it's true for
        # the FexSum (for example)
        
        # Use this unique name
        name = fex.MissingETOutputKey
        # Remove the TrigEFMissingET part
        if name.startswith("TrigEFMissingET"):
            name = name[len("TrigEFMissingET"):]
        # If this is now empty, it must have been cell so call it that
        if name == "":
            name = "_cell"
        # I don't like using EF given that we haven't called the HLT the Event
        # Filter for a very long time, however every other TE is named like
        # 'L2_' or 'EF_' so I won't break that convention
        return "EF_xe"+name

    def _connect_hypo(self, hypo, fex):
        hypo.METLabel = fex.MissingETOutputKey
