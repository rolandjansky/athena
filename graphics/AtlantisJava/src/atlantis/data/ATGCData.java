package atlantis.data;

import atlantis.event.*;
import atlantis.graphics.ACoord;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.AParameterUtilities;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AHashMap;
import atlantis.utils.AIdHelper;
import atlantis.utils.ALogger;
import atlantis.utils.AMath;

public class ATGCData extends AMuonHitData
{
    float[] swidth;
    float[] lwidth;
    float[] length;
    int[] gasGap;

    private static ALogger logger = ALogger.getLogger(ATGCData.class);    

    public String getParameterGroup()
    {
        return "TGC";
    }

    public String getName()
    {
        return "TGC";
    }

    public String getNameScreenName()
    {
        return "TGC";
    }

    ATGCData(AHashMap p, AEvent e)
    {
        super(p,e);
        swidth = p.getFloatArray("swidth");
        length = p.getFloatArray("length");
        lwidth = p.getFloatArray("lwidth");
        gasGap = new int[numData];
        for (int i = 0; i < numData; i++)
            gasGap[i] = getGasGap(i);
    }

    protected int getStation(int index)
    {
        try {
            String stationName = AIdHelper.stationName(id[index]);
            return (stationName.charAt(1) - '0') % 4;
        } catch (AAtlantisException e) {
            logger.error("Problem decoding TGC identifier", e);
        }
        
        return 0;
    }

    protected int getSub(int index)
    {
        try {
            if (AIdHelper.stationEta(id[index]) < 0) {
                return 0;
            } else {
                return 1;
            }

        } catch (AAtlantisException e) {
            logger.error("Problem decoding TGC identifier", e);
        }
        
        return 0;
    }

    public int getSector(int index)
    {
        try {
            String stationName = AIdHelper.stationName(id[index]);
            int stationPhi = AIdHelper.stationPhi(id[index]);

            int retval;
            if (stationName.equals("T4E")) {
                int temp = stationPhi % 24;
                retval = 2 * (temp / 3);
            } else if (stationName.charAt(2) == 'F') {
                int temp = stationPhi % 24;
                retval = 2 * (temp / 3);
                if (temp % 3 > 1) retval += 1;
            } else {
                int temp = (stationPhi + 1) % 48;
                retval = 2 * (temp / 6);
                if (temp % 6 > 3) retval += 1;
            }
            return retval;
 
        } catch (AAtlantisException e) {
            logger.error("Problem decoding TGC identifier", e);
        }
        
        return 0;

    }

    protected int getGasGap(int index)
    {
        try {
            return AIdHelper.tgcGasGap(id[index]);
        } catch (AAtlantisException e) {
            logger.error("Problem decoding TGC identifier", e);
        }
        
        return 0;
    }

    protected boolean getMeasuresPhi(int index)
    {
        try {
            if (AIdHelper.tgcIsStrip(id[index]) == 1) {
                return true;
            }
        } catch (AAtlantisException e) {
            logger.error("Problem decoding TGC identifier", e);
        }
        
        return false;
    }

    public void applyCuts()
    {
        super.applyCuts();
        if (parameterStore.get("CutsATLAS", "CutPhi").getStatus())
            cutPhi(phi, getDPhi());
        if (parameterStore.get("CutsATLAS", "CutEta").getStatus())
            cutEtaDRho(rho, z, getDRho());
    }

    // used for cuts

    private float[] getDPhi()
    {
        // only roughly correct
        // must create all
        float[] dphi = new float[numData];

        // need only fill for those in draw list
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];

            dphi[list] = (float) Math.abs(Math.atan2((swidth[list] + lwidth[list]) / 4., rho[list]));
        }
        return dphi;
    }

    // used for cuts

    private float[] getDRho()
    {
        // only roughly correct
        // must create all
        float[] drho = new float[numData];

        // need only fill for those in draw list
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];

            drho[list] = (float) (length[list] / 2.);
        }
        return drho;
    }

    protected ACoord getFZUser()
    {
        makeDrawList();
        cutArray(measuresPhi, true, " Strip");
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];

        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];

            index[i] = list;
            double deltaPhi = ((swidth[list] + lwidth[list]) / 4.) / rho[list];

            hv[0][0][i] = z[list];
            hv[1][0][i] = Math.toDegrees(phi[list] - deltaPhi);
            hv[0][1][i] = z[list];
            hv[1][1][i] = Math.toDegrees(phi[list] + deltaPhi);
        }
        return new ACoord(hv, index, this, ACoord.LINES).includePhiWrapAround("FZ");
    }

    private void makeDrawListYX()
    {
        int mode = parameterStore.get("YX", "Mode").getI();

        if (mode == 0 || mode >= 5)
        {
            numDraw = 0;
        }
        else
        {
            makeDrawList();
            int num = 0;

            for (int i = 0; i < numDraw; ++i)
                if (mode == station[listdl[i]] + 1)
                    listdl[num++] = listdl[i];
            numDraw = num;
        }
        cut("YX", "TGCGasGap", " TGC Gas Gap", gasGap);
    }

    protected ACoord getYXUser()
    {
        return getYXUser(0);
    }

    protected ACoord getYXUser(int flag)
    {
        makeDrawListYX();
        int[] split = { 6, 1, 6, 1 };
        int numPoints = 4;
        boolean splitIt = parameterStore.get("YX", "FishEye").getStatus() || flag == 1;

        if (splitIt)
            numPoints = 14;
        double[] temp = new double[4];
        double[][][] hv = new double[2][numDraw][numPoints];
        int[] index = new int[numDraw];

        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];

            index[i] = list;
            double cosPhi = Math.cos(phi[list]);
            double sinPhi = Math.sin(phi[list]);
            double rMid = rho[list];
            double dR = length[list] / 2.;
            double r = rMid - dR;
            double d = swidth[list] / 2.;
            double x = r * cosPhi;
            double y = r * sinPhi;
            double dx = d * sinPhi;
            double dy = d * cosPhi;

            hv[0][i][0] = x + dx;
            hv[1][i][0] = y - dy;
            hv[0][i][1] = x - dx;
            hv[1][i][1] = y + dy;
            r = rMid + dR;
            x = r * cosPhi;
            y = r * sinPhi;
            d = lwidth[list] / 2.;
            dx = d * sinPhi;
            dy = d * cosPhi;
            hv[0][i][2] = x - dx;
            hv[1][i][2] = y + dy;
            hv[0][i][3] = x + dx;
            hv[1][i][3] = y - dy;
            if (splitIt)
                for (int j = 0; j < 2; ++j)
                {
                    for (int k = 0; k < 4; ++k)
                        temp[k] = hv[j][i][k];
                    AMath.splitArrayIntoPieces(temp, hv[j][i], split);
                }
        }
        return new ACoord(hv, index, this);
    }

    protected ACoord getFRUser()
    {
        return getYXUser(1).convertYXToFR().includePhiWrapAround("FR");
    }

    protected ACoord getXZRZUser(int sign[]) 
    {
        double[][][] hv = new double[2][numDraw][];
        int index[] = new int[numDraw];
        
        try {
            for (int i=0; i<numDraw; i++) {
                int j = listdl[i];
                index[i] = j;

                double rho = Math.sqrt(x[j]*x[j] + y[j]*y[j]);
                
                if (AIdHelper.tgcIsStrip(id[j]) == 1) {
                    double drho = length[j]/2.;
                    hv[0][i] = new double[] { z[j], z[j] };
                    hv[1][i] = new double[] { sign[i] * (rho - drho),
                                              sign[i] * (rho + drho) };
                } else {
                    hv[0][i] = new double[] { z[j] };
                    hv[1][i] = new double[] { sign[i] * rho };
                }
            }

            return new ACoord(hv, index, this);
        } catch (AAtlantisException e) {
            AOutput.append("Error decoding TGC identifier: " + e.getMessage(), ALogInterface.BAD_COMMAND);
            return ACoord.NO_DATA;
        }
    }

    protected ACoord getXZUser() {
        makeDrawList();
        cutMuonSector(sector);
        
        int[] sign = new int[numDraw];
        int sect = (int) Math.round(parameterStore.get("XZ", "Phi").getD() / 22.5);
        
        for(int i=0; i<numDraw; i++) {
            if (sector[listdl[i]] == sect)
                sign[i] = 1;
            else
                sign[i] = -1;
        }
        
        return getXZRZUser(sign);
    }
    
    protected ACoord getRZUser() {
        makeDrawList();
        
        int[] sign = new int[numDraw];
        for (int i=0; i<numDraw; i++) {
            int j = listdl[i];
            sign[i] = AParameterUtilities.getRhoSign(x[j], y[j]);
        }
        
        return getXZRZUser(sign);
    }
}
