package atlantis.utils;

import java.util.ArrayList;

/**
 * This class contains methods for extracting various properties from the 32 bit
 * identifiers used for hits. It works very similarly to the IdHelpers in Athena,
 * hence its name.
 *
 * @author Eric Jansen
 */
public class AIdHelper {
    
    private static ANewIdHelper idHelper = null;
    
    /**
     * Returns the decoded athena identifier (id) as a string, resp. an
     * array of strings: first item is the full id expanded (decoded) and
     * particular subparts are separated by "/" in one sting, further
     * interesting subparts of the decoded id are provided in the following
     * items of the array with an explanation given.
     * So far (2009-01-20), this is implemented only for LAr and Tile, for
     * other subsystems only first array item is returned with the expanded
     * id.
     * @param id int compact identifier
     * @return String decoded identifier
     */
    public static String[] getFullIdentifier(long id)
    {
        // more than 10 items should not be necessary
        ArrayList<String> r = new ArrayList<String>(10);
        String s = null;

        try 
        {
            switch (subDetector(id))
            {
                case 2:
                    switch (indetPart(id))
                    {
                        case 1:
                            // Pixel identifiers are complicated and we don't use them anyway.
                            throw new AAtlantisException("Pixel identifiers not implemented: " + id);
                        case 2:
                            s = "2/2/" + sctBarrelEndcap(id) + "/" +
                                sctLayerDisk(id) + "/" + sctPhiModule(id) + "/" +
                                sctEtaModule(id) + "/" + sctPhiIndex(id) + "/" +
                                sctEtaIndex(id); 
                            r.add(s);
                            return r.toArray(new String[r.size()]);
                        case 3:
                            
                            int part = 3;
                            int barrelEndcap = trtBarrelEndcap(id);
                            int layerWheel = trtLayerWheel(id);
                            int phiModule = trtPhiModule(id);
                            int strawLayer = trtStrawLayer(id);
                            int straw = trtStraw(id);
                                   
                            s = "2" + "/" + part + "/" + barrelEndcap + "/" +
                                layerWheel + "/" + phiModule + "/" + strawLayer +
                                "/" + straw;
                            r.add(s);
                            r.add("barrel-endcap: " + barrelEndcap);
                            r.add("layer wheel: " + layerWheel);
                            r.add("phi module: " + phiModule);
                            r.add("straw layer: " + strawLayer);
                            r.add("straw: " + straw);
                            return r.toArray(new String[r.size()]);
                            
                        default:
                            throw new AAtlantisException("Invalid identifier: " + id);
                    }
                case 4:
                    int part = larPart(id);
                    int sampling = part < 3 ? 0 : larSampling(id);
                    int eta = larEta(id);
                    int phi = larPhi(id);
                    
                    if(part == 1) // LAr barrel, LAr endcap
                    {
                        int barrelEndcap = larBarrelEndcap(id);
                        int region = larRegion(id);
                        s = "4/" + part + "/" + barrelEndcap + "/" +
                            sampling + "/" + region + "/" + eta + "/" + phi;
                        r.add(s);
                        if(barrelEndcap == 1)
                        {
                            //r.add("barrel A");
                            r.add("EMBA");
                        }
                        else if(barrelEndcap == -1)
                        {
                            //r.add("barrel C");
                            r.add("EMBC");
                        }
                        else if(barrelEndcap == 2 || barrelEndcap == 3)
                        {
                            //r.add("endcap A");
                            r.add("EMECA");
                        }
                        else if(barrelEndcap == -2 || barrelEndcap == -3)
                        {
                            //r.add("endcap C");
                            r.add("EMECC");
                        }
                    }
                    else if(part == 2) // HEC
                    {
                        int posNeg = larBarrelEndcap(id);
                        int region = larRegion(id);
                        s = "4/" + part + "/" + posNeg + "/" + sampling + "/" +
                            region + "/" + eta + "/" + phi;
                        r.add(s);
                        if(posNeg == 2)
                        {
                            //r.add("A");
                            r.add("HECA");
                        }
                        else if(posNeg == -2)
                        {
                            //r.add("C");
                            r.add("HECC");
                        }
                    }
                    else if(part == 3) // FCAL
                    {
                        int posNeg = larBarrelEndcap(id);
                        s = "4/" + part + "/" + posNeg + "/" + sampling + "/" +
                            eta + "/" + phi;
                        r.add(s);
                        if(posNeg == 2)
                        {
                            //r.add("A");
                            r.add("FCALA");
                        }
                        else if(posNeg == -2)
                        {
                            //r.add("C");
                            r.add("FCALC");
                        }                        
                    }
                    else
                    {
                        throw new AAtlantisException("Invalid identifier: " + id);
                    }
                    
                    r.add("sampling: " + sampling); // sampling remains          
                    
                    return r.toArray(new String[r.size()]);
                    
                case 5:
                    // tile calorimeter (first number of full id is 5)
                    int section = tileSection(id);
                    int side = tileSide(id);
                    int module = tileModule(id);
                    int tower = tileTower(id);
                    sampling = tileSampling(id);
                    int pmt = tilePmt(id);
                    int adc = tileAdc(id);
                    
                    s = "5/" + section + "/" + side + "/" + module + "/" + 
                        tower + "/" + sampling + "/" + pmt + "/" + adc;
                    r.add(s);
                    
                    r.add("section: " + section);
                    r.add("side: " + side);
                    r.add("module: " + module);
                    r.add("tower: " + tower);
                    r.add("sampling: " + sampling);
                    r.add("PMT: " + pmt);
                    r.add("ADC: " + adc);
                    
                    return r.toArray(new String[r.size()]);
                    
                case 7:
                    String technology = technology(id);
                    if (technology.equals("MDT")) 
                    {
                        s = "7/" + stationName(id) + "/" + stationEta(id) +
                            "/" + stationPhi(id) + "/" + "MDT/" +
                            mdtMultiLayer(id) + "/" + mdtTubeLayer(id) + "/" +
                            mdtTube(id);
                        r.add(s);
                        return r.toArray(new String[r.size()]);
                    }
                    else if (technology.equals("RPC"))
                    {
                        s = "7/" + stationName(id) + "/" + stationEta(id) + "/" +
                            stationPhi(id) + "/" + "RPC/" + rpcDoubletR(id) + "/" +
                            rpcDoubletZ(id) + "/" + rpcDoubletPhi(id) + "/" +
                            rpcGasGap(id) + "/" + rpcMeasuresPhi(id) + "/" +
                            rpcStrip(id);
                        r.add(s);
                        return r.toArray(new String[r.size()]);
                    }
                    else if (technology == "TGC")
                    {
                        s = "7/" + stationName(id) + "/" + stationEta(id) +
                            "/" + stationPhi(id) + "/" + "TGC/" + tgcGasGap(id) +
                            "/" + tgcIsStrip(id) + "/" + tgcChannel(id);
                        r.add(s);
                        return r.toArray(new String[r.size()]);                        
                    }
                    else if (technology == "CSC")
                    {
                        s = "7/" + stationName(id) + "/" + stationEta(id) + "/" +
                            stationPhi(id) + "/" + "CSC/" + cscChamberLayer(id) +
                            "/" + cscWireLayer(id) + "/" + cscMeasuresPhi(id) +
                            "/" + cscStrip(id);
                        r.add(s);
                        return r.toArray(new String[r.size()]);                                                
                    }
                    else
                    {
                        throw new AAtlantisException("Invalid identifier: " + id);
                    }
                default:
                    throw new AAtlantisException("Not yet implemented for identifier: " + id);
            }
        } 
        catch (AAtlantisException e) 
        {
            s = id + " (unable to decode: " + e.getMessage() + ")";
            r.add(s);
            return r.toArray(new String[r.size()]);                                                
        }
        
    } // getFullIdentifier() ------------------------------------------------

    
    
    /** @deprecated */
    public static int subDetector(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("subdet", id);
    }

    /** @deprecated */
    public static int indetPart(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("part", id);
    }

    /** @deprecated */
    public static int trtBarrelEndcap(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("barrel_endcap", id);
    }


    /** @deprecated */
    public static int trtLayerWheel(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("layer_or_wheel", id);
    }

    /** @deprecated */
    public static int trtPhiModule(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("phi_sector", id);
    }

    /** @deprecated */
    public static int trtStrawLayer(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("straw_layer", id);
    }

    /** @deprecated */
    public static int trtStraw(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("straw", id);
    }

    /** @deprecated */
    public static final int sctBarrelEndcap(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("barrel_endcap", id);
    }

    /** @deprecated */
    public static int sctLayerDisk(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("layer", id);
    }

    /** @deprecated */
    public static int sctPhiModule(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("phi_module", id);
    }

    /** @deprecated */
    public static int sctEtaModule(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("eta_module", id);
    }

    /** @deprecated */
    public static int sctPhiIndex(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("side", id); // This seems odd, phi_index <-> side?
    }

    /** @deprecated */
    public static int sctEtaIndex(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("strip", id);
    }

    /** @deprecated */
    public static int larPart(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("part", id);
    }

    /** @deprecated */
    public static int larBarrelEndcap(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("barrel-endcap", id);
    }

    /** @deprecated */
    public static int larPosNeg(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("pos_neg", id);
    }

    /** @deprecated */
    public static int larSampling(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("sampling", id);
    }

    /** @deprecated */
    public static int larRegion(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("region", id);
    }

    /** @deprecated */
    public static int larEta(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        if (larPart(id) < 3) {
            return idHelper.get("eta", id);
        } else {
            return idHelper.get("eta-fcal", id);
        }
    }

    /** @deprecated */
    public static int larPhi(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        if (larPart(id) < 3) {
            return idHelper.get("phi", id);
        } else {
            return idHelper.get("phi-fcal", id);
        }
    }

    /** @deprecated */
    public static int larModule(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("module", id);
    }


    /** @deprecated */
    public static boolean larIsBarrel(long id) throws AAtlantisException
    {
        switch(Math.abs(larBarrelEndcap(id)))
        {
            case 1:
                return true;  // yes, it's barrel
            case 2:
                return false; // is endcap
            case 3:
                return false; // is endcap
            default:
                throw new AAtlantisException("Not a LAr identifier: " + id);
        }
    }

    /** @deprecated */
    public static int tileSection(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("section", id);
    }

    /** @deprecated */
    public static int tileSide(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("side", id);
    }

    /** @deprecated */
    public static int tileModule(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("module", id);
    }

    /** @deprecated */
    public static int tileTower(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("tower", id);
    }

    /** @deprecated */
    public static int tileSampling(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("sampling", id);
    }

    /** @deprecated */
    public static int tilePmt(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("pmt", id);
    }

    /** @deprecated */
    public static int tileAdc(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("adc", id);
    }


    /**
     * Decodes Athena 32-bit ID to following form (5 is Tile):
     * 5/section/side/module/tower/sampling/pmt/adc - official identifier
     *
     * Identifier used by TileCal collaboration, conversion is clear from the
     * code and is explained in Sasha Solodkov's email on 2006-09-18
     *
     * @param id int
     * @return String
     */
    public static String getDecodedTileIndentifier(long id)
    {
        try
        {
            int sec = tileSection(id);
            int side = tileSide(id);
            int mod = tileModule(id);
            int tow = tileTower(id);
            int samp = tileSampling(id);

            // section/side/module into decoded form
            String secSideMod = "\n[could not work out section/side]";
            mod++;
            if(sec == 1 && side == 1)  secSideMod = "LBA" + mod;
            if(sec == 1 && side == -1) secSideMod = "LBC" + mod;
            if(sec == 2 && side == 1)  secSideMod = "EBA" + mod;
            if(sec == 3 && side == 1)  secSideMod = "EBA" + mod;
            if(sec == 2 && side == -1) secSideMod = "EBC" + mod;
            if(sec == 3 && side == -1) secSideMod = "EBC" + mod;

            // sampling/tower into decoded form
            String sampTow = "\n[could not work out sampling/tower]";
            if(samp == 0) sampTow = "A" + (int)(tow + 1);
            if(samp == 1) sampTow = "B" + (int)(tow + 1);
            if(samp == 2) sampTow = "D" + (int)(tow/2);
            if(samp == 3)
            {
                if(tow == 10) sampTow = "E1";
                if(tow == 11) sampTow = "E2";
                if(tow == 13) sampTow = "E3";
                if(tow == 15) sampTow = "E4";
            }

            return secSideMod + " " + sampTow;
        }
        catch (AAtlantisException e)
        {
            return id + " (unable to decode: " + e.getMessage() + ")";
        }
    }


    /** @deprecated */
    public static String stationName(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.getString("stationName", id);
    }

    /** @deprecated */
    public static String technology(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.getString("technology", id);
    }

    /** @deprecated */
    public static int stationEta(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("stationEta", id);
    }

    /** @deprecated */
    public static int stationPhi(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("stationPhi", id);
    }

    /** @deprecated */
    public static int mdtMultiLayer(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("multiLayer", id);
    }

    /** @deprecated */
    public static int mdtTubeLayer(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("tubeLayer", id);
    }

    /** @deprecated */
    public static int mdtTube(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("tube", id);
    }

    /** @deprecated */
    public static int rpcDoubletR(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("doubletR", id);
    }

    /** @deprecated */
    public static int rpcDoubletZ(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("doubletZ", id);
    }

    /** @deprecated */
    public static int rpcDoubletPhi(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("doubletPhi", id);
    }

    /** @deprecated */
    public static int rpcGasGap(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("rpcGasGap", id);
    }

    /** @deprecated */
    public static int rpcMeasuresPhi(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("rpcMeasuresPhi", id);
    }

    /** @deprecated */
    public static int rpcStrip(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("rpcStrip", id);
    }

    /** @deprecated */
    public static int tgcGasGap(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("tgcGasGap", id);
    }

    /** @deprecated */
    public static int tgcIsStrip(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("isStrip", id);
    }

    /** @deprecated */
    public static int tgcChannel(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("channel", id);
    }

    /** @deprecated */
    public static int cscChamberLayer(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("chamberLayer", id);
    }

    /** @deprecated */
    public static int cscWireLayer(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("wireLayer", id);
    }

    /** @deprecated */
    public static int cscMeasuresPhi(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("cscMeasuresPhi", id);
    }

    /** @deprecated */
    public static int cscStrip(long id) throws AAtlantisException {
        if (idHelper == null) idHelper = ANewIdHelper.instance();
        return idHelper.get("cscStrip", id);
    }
}
