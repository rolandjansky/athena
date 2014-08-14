package atlantis.data;

import atlantis.event.*;
import atlantis.graphics.ACoord;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AHashMap;
import atlantis.utils.AIdHelper;
import atlantis.utils.ALogger;
import atlantis.utils.AMath;

/**
 * for comments see AMDTData
 */

public class ARPCData extends AMuonHitData
{
    protected float[] width;
    protected float[] length;
    protected int[] gasGap;
    
    private static ALogger logger = ALogger.getLogger(ARPCData.class);
    
    public String getParameterGroup()
    {
        return "RPC";
    }
    
    public String getName()
    {
        return "RPC";
    }
    
    public String getNameScreenName()
    {
        return "RPC";
    }
    
    ARPCData(AHashMap p, AEvent e)
    {
        super(p,e);
        length = p.getFloatArray("length");
        width = p.getFloatArray("width");
        gasGap = new int[numData];
        for (int i = 0; i < numData; ++i)
            gasGap[i] = getGasGap(i);
    }
          
    protected int getStation(int index)
    {
        try {
            String stationName = AIdHelper.stationName(id[index]);
            
            if (stationName.charAt(1) == 'M') {
                return AIdHelper.rpcDoubletR(id[index]) - 1;
            } else {
                return AIdHelper.rpcDoubletR(id[index]) + 1;
            }
        } catch (AAtlantisException e) {
            logger.error("Problem decoding RPC identifier", e);
        }
        
        return 0;
    }
    
    protected int getSub(int index)
    {
        return 0;
    }
    
    public int getSector(int index)
    {
        try {
            String stationName = AIdHelper.stationName(id[index]);
            
            if (stationName.charAt(2) == 'L') {
                return 2 * (AIdHelper.stationPhi(id[index]) - 1);
            } else {
                return 2 * (AIdHelper.stationPhi(id[index]) - 1) + 1;
            }
        } catch (AAtlantisException e) {
            logger.error("Problem decoding RPC identifier", e);
        }
        
        return 0;
    }
    
    protected int getGasGap(int index)
    {
        try {
            return AIdHelper.rpcGasGap(id[index]);
        } catch (AAtlantisException e) {
            logger.error("Problem decoding RPC identifier", e);
        }
        
        return 0;
    }
    
    protected boolean getMeasuresPhi(int index)
    {
        try {
            if (AIdHelper.rpcMeasuresPhi(id[index]) == 1) {
                return true;
            }
        } catch (AAtlantisException e) {
            logger.error("Problem decoding RPC identifier", e);
        }
        
        return false;
    }
    
    protected void applyCuts()
    {
        super.applyCuts();
        if (parameterStore.get("CutsATLAS", "CutPhi").getStatus())
            cutPhi(phi, getDPhi());
        if (parameterStore.get("CutsATLAS", "CutEta").getStatus())
            cutEtaDZ(rho, z, getDZ());
    }
    
    private float[] getDPhi()
    {
        // only roughly correct
        // must create all
        float[] dphi = new float[numData];
        
        // need only fill for those in draw list
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            if (measuresPhi[list])
                dphi[list] = (float) Math.abs(Math.atan2(width[list] / 2., rho[list]));
            else
                dphi[list] = (float) Math.abs(Math.atan2(length[list] / 2., rho[list]));
        }
        return dphi;
    }
    
    private float[] getDZ()
    {
        // must create all
        float[] dz = new float[numData];
        
        // need only fill for those in draw list
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            
            if (measuresPhi[list])
                dz[list] = (float) (length[list] / 2.);
            else
                dz[list] = (float) (width[list] / 2.);
        }
        return dz;
    }
    
    protected ACoord getYXUser()
    {
        makeDrawList();
        cutArray(measuresPhi, true, " Strip");
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double r = rho[list];
            double cosPhi = Math.cos(phi[list]);
            double sinPhi = Math.sin(phi[list]);
            double phiSector = sector[list] * AMath.PI_BY_8;
            double cosPhiSector = Math.cos(phiSector);
            double sinPhiSector = Math.sin(phiSector);
            double d = width[list] / 2.;
            if (!measuresPhi[list])
                d = length[list] / 2.;
            double x = r * cosPhi;
            double y = r * sinPhi;
            double dx = d * sinPhiSector;
            double dy = d * cosPhiSector;
            
            hv[0][0][i] = x + dx;
            hv[1][0][i] = y - dy;
            hv[0][1][i] = x - dx;
            hv[1][1][i] = y + dy;
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES);
    }
    
    protected ACoord getRZUser()
    {
        makeDrawList();
        cutArray(measuresPhi, false, " Strip");
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
        
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double phiDiff = Math.abs(phi[list] - phiMid);
            int sign = -1;
            
            if (phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.)
                sign = 1;
            double rC = sign * AMDTData.getSectorRho(sector[list], rho[list], phi[list]);
            double zC = z[list];
            double w = width[list] / 2.;
            
            hv[0][0][i] = zC - w;
            hv[1][0][i] = rC;
            hv[0][1][i] = zC + w;
            hv[1][1][i] = rC;
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES);
    }
    
    protected ACoord getXZUser()
    {
        makeDrawList();
        cutMuonSector(sector);
        cutArray(measuresPhi, false, " Strip");
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        int sect=(int)Math.round(parameterStore.get("XZ", "Phi").getD() / 22.5);
        
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            int sign = 1;
            
            if (sector[list] != sect)
                sign = -1;
            double rC = sign * AMDTData.getSectorRho(sector[list], rho[list], phi[list]);
            double zC = z[list];
            double w = width[list] / 2.;
            
            hv[0][0][i] = zC - w;
            hv[1][0][i] = rC;
            hv[0][1][i] = zC + w;
            hv[1][1][i] = rC;
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES);
    }
    
    protected ACoord getFRUser()
    {
        makeDrawList();
        cutArray(measuresPhi, true, " Strip");
        return getYXUser().convertYXToFR().includePhiWrapAround("FR");
    }
    
    private void makeDrawListFZ()
    {
        int mode = parameterStore.get("FZ", "Mode").getI();
        
        if (mode == 0 || mode >= 4)
        {
            numDraw = 0;
        }
        else
        {
            makeDrawList();
            cut(" Station", station, "==", mode - 1);
        }
        cut("FZ", "RPCGasGap", " RPC Gas Gap", gasGap);
    }
    
    protected ACoord getFZUser()
    {
        makeDrawListFZ();
        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];
        
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            
            index[i] = list;
            if (measuresPhi[list])
            {
                hv[0][i] = AMath.xBox(z[list], length[list] / 2.);
                hv[1][i] = AMath.yBox(Math.toDegrees(phi[list]), Math.toDegrees((width[list] / 2.) / rho[list]));
            }
            else
            {
                double zC = z[list];
                double r = rho[list];
                double cosPhi = Math.cos(phi[list]);
                double sinPhi = Math.sin(phi[list]);
                double phiSector = sector[list] * AMath.PI_BY_8;
                double cosPhiSector = Math.cos(phiSector);
                double sinPhiSector = Math.sin(phiSector);
                double d = length[list] / 2.;
                double w = width[list] / 2.;
                double x = r * cosPhi;
                double y = r * sinPhi;
                double dx = d * sinPhiSector;
                double dy = d * cosPhiSector;
                double phi1 = Math.toDegrees(Math.atan2(y - dy, x + dx));
                double phi2 = Math.toDegrees(Math.atan2(y + dy, x - dx));
                hv[0][i] = AMath.xBox(zC, w);
                hv[1][i] = AMath.yBox((phi1 + phi2) / 2., (phi1 - phi2) / 2.);
                
            }
        }
        return new ACoord(hv, index, this).includePhiWrapAround("FZ");
    }
    
}
