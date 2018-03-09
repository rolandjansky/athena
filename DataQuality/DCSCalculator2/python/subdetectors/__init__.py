# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from csc import CSC
from lar import LAr
from lucid import Lucid
from mdt import MDT
from pixels import Pixels
from rpc import RPC
from sct import SCT
from tdq import TDQ
from tile import Tile
from trt import TRT
from tgc import TGC

# Non-detector flags
from idbs import IDBS
from magnets import Magnets
from global_system import Global
from trig import Trigger

ALL_SYSTEMS = [CSC, LAr, Lucid, MDT, Pixels, RPC, SCT, TDQ, Tile, TGC, TRT,  IDBS, Magnets, Global, Trigger]
SYS_NAMES = ", ".join(map(lambda x: x.__name__, ALL_SYSTEMS))

SYSTEM_MAP = dict((x.__name__, x) for x in ALL_SYSTEMS)
