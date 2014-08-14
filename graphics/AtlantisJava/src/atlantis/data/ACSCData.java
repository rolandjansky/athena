package atlantis.data;

import atlantis.event.*;
import atlantis.graphics.ACoord;
import atlantis.graphics.AGraphics;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AHashMap;
import atlantis.utils.AIdHelper;
import atlantis.utils.ALogger;

public class ACSCData extends AMuonHitData 
{    
    float[] sigma;
    
    private static ALogger logger = ALogger.getLogger(ACSCData.class);    
    
    public String getParameterGroup()
    {
        return "CSC";
    }
    
    public String getName()
    {
        return "CSC";
    }
    
    public String getNameScreenName()
    {
        return "CSC";
    }
    
    ACSCData(AHashMap p, AEvent e) 
    {
        super(p,e);
        sigma=p.getFloatArray("sigma");
    }

    protected int getStation(int index) 
    {
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
            
            if (stationName.charAt(2) == 'L') {
                return 2 * (AIdHelper.stationPhi(id[index]) - 1);
            } else {
                return 2 * (AIdHelper.stationPhi(id[index]) - 1) + 1;
            }
        } catch (AAtlantisException e) {
            logger.error("Problem decoding CSC identifier", e);
        }
        
        return 0;
    }
    
    protected boolean getMeasuresPhi(int index) 
    {
        try {
            if (AIdHelper.cscMeasuresPhi(id[index]) == 1) {
                return true;
            }
        } catch (AAtlantisException e) {
            logger.error("Problem decoding CSC identifier", e);
        }
        
        return false;
    }
    
    protected void applyCuts() 
    {
        super.applyCuts();
        cutPhi(phi);
        cutEta(rho, z);
    }
    
    protected ACoord getYXUser() 
    {
        makeDrawList();
        int mode=parameterStore.get("YX", "Mode").getI();
        if(mode!=5)
            numDraw=0;
        
        double[] h=new double[numDraw];
        double[] v=new double[numDraw];
        int[] index=new int[numDraw];
        
        for(int i=0; i<numDraw; i++) 
        {
            int list=listdl[i];
            h[i]=x[list];
            v[i]=y[list];
            index[i]=list;
        }
        return new ACoord(h, v, index, this);
    }
    
    protected ACoord getXZRZUser(int[] sign) 
    {
        int numPoints=2;
        
        double[][][] hv=new double[2][numDraw][numPoints];
        int[] index=new int[numDraw];
        
        // space point resolution is 60um in rz and 5mm in rphi (due to no uniform b field)
        // from muonTDR page 201
        // double dRho=0.254;
        // double dZMax=1.25;
        
        double alpha=-11.59;
        double sinAlphaCopy=Math.sin(Math.toRadians(alpha));
        double cosAlphaCopy=Math.cos(Math.toRadians(alpha));
        
        for(int i=0; i<numDraw; i++) 
        {
            int list=listdl[i];
            
            double rMid=sign[i]*getSectorRho(sector[list], rho[list], phi[list]);
            double zMid=z[list];
            // double dZ = 2. * dZMax * Math.min(charge[list] / (25. * 75000.), 0.9);
            double dRho=sigma[list];
            
            double sinAlpha=sinAlphaCopy;
            double cosAlpha=cosAlphaCopy;
            
            if(zMid>0.)
                cosAlpha*=-1.;
            if(rMid<0.)
                sinAlpha*=-1.;
            
            hv[0][i][0]=zMid-sinAlpha*dRho;
            hv[0][i][1]=zMid+sinAlpha*dRho;
            hv[1][i][0]=rMid+cosAlpha*dRho;
            hv[1][i][1]=rMid-cosAlpha*dRho;
            index[i]=list;
        }
        return new ACoord(hv, index, this, ACoord.POLYLINES);
    }
    
    protected int getDrawOrFill() 
    {
        return AGraphics.DRAW;
    }
        
    protected ACoord getRZUser() 
    {
        makeDrawList();
        int[] sign=new int[numDraw];
        double phiMid=Math.toRadians(parameterStore.get("RZ", "Phi").getD());
        
        for(int i=0; i<numDraw; i++) 
        {
            double phiDiff=Math.abs(phi[listdl[i]]-phiMid);
            
            sign[i]=-1;
            if(phiDiff<Math.PI/2.||phiDiff>3*Math.PI/2.)
                sign[i]=1;
        }
        return getXZRZUser(sign);
    }
    
    protected ACoord getXZUser() 
    {
        makeDrawList();
        cutMuonSector(sector);
        int[] sign=new int[numDraw];
        int sect=(int)Math.round(parameterStore.get("XZ", "Phi").getD() / 22.5);
        
        for(int i=0; i<numDraw; i++) 
        {
            sign[i]=1;
            if(sector[listdl[i]]!=sect)
                sign[i]=-1;
        }
        return getXZRZUser(sign);
    }
    
}
