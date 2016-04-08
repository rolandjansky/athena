ByteStreamCnvSvc = Service( "ByteStreamCnvSvc" )

if Tile2004:
    # frag -> ROD remapping for 2004
    #
    ByteStreamCnvSvc.ROD2ROBmap = [
        # ROB id contains "input card number" and do not match ROD id
        # mapping given here is frag-to-ROB mapping for eformat V3
        # Tile Beam ROD
        "0x000", "0x500002",
        "0x001", "0x500002",
        "0x002", "0x500002",
        "0x003", "0x500002",
        "0x004", "0x500002",
        "0x005", "0x500002",
        "0x006", "0x500002",
        "0x007", "0x500002",
        "0x0FF", "0x500002",
        # Common Beam ROD
        "0x010", "0x700001",
        "0x011", "0x700001",
        "0x012", "0x700001",
        "0x013", "0x700001",
        "0x014", "0x700001",
        "0x015", "0x700001",
        # Tile RODs, this mapping does not follow ATLAS conventions
        "0x100", "0x510000",
        "0x101", "0x510001",
        "0x102", "0x510002",
        "0x200", "0x510000",
        "0x201", "0x510001",
        "0x202", "0x510002",
        "0x300", "0x510000",
        "0x301", "0x510001",
        "0x302", "0x510002",
        "0x400", "0x510000",
        "0x401", "0x510001",
        "0x402", "0x510002" ] 
else:
    # frag -> ROD remapping for most of TileCal standalone runs in 2003
    #
    ByteStreamCnvSvc.ROD2ROBmap = [
        # Beam ROD
        "0x000", "0x320064",
        "0x001", "0x320064",
        "0x002", "0x320064",
        "0x003", "0x320064",
        "0x004", "0x320064",
        "0x005", "0x320064",
        "0x006", "0x320064",
        "0x007", "0x320064",
        "0x008", "0x320064",
        "0x00A", "0x320064",
        "0x00B", "0x320064",
        "0x00C", "0x320064",
        "0x00D", "0x320064",
        "0x00E", "0x320064",
        "0x0FF", "0x320064",
        # ROD 1
        "0x100", "0x320066",
        "0x101", "0x320066",
        "0x102", "0x320066",
        # ROD 2
        "0x200", "0x320065",
        "0x201", "0x320065",
        "0x202", "0x320065" ]
