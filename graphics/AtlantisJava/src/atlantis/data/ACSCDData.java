package atlantis.data;

import atlantis.event.*;
import atlantis.graphics.ACoord;
import atlantis.graphics.AGraphics;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.AParameter;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.AProjectionYX;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AHashMap;
import atlantis.utils.AIdHelper;
import atlantis.utils.ALogger;

public class ACSCDData extends AMuonHitData 
{
    private static final double CSC_TILT_ANGLE = Math.toRadians(-11.59);
    
    float[] charge;
    float[] length;
    int[] gasGap;
    
    private static ALogger logger = ALogger.getLogger(ACSCDData.class);    
    
    public String getParameterGroup()
    {
        return "CSC";
    }
    
    public String getName()
    {
        return "CSCD";
    }
    
    public String getNameScreenName()
    {
        return "CSC Digits";
    }
    
    ACSCDData(AHashMap p, AEvent e) 
    {
        super(p,e);
        
        if (p.get("length") != null) {
            length=p.getFloatArray("length");
        } else {
            // If no length is given, just use an arbitrary minimum length
            length = new float[numData];
            for (int i=0; i<numData; i++)
                length[i] = 10.f;
        }
        
        charge=p.getFloatArray("charge");
        
        gasGap = new int[numData];
        for (int i=0; i<numData; i++) {
            try {
                gasGap[i] = AIdHelper.cscWireLayer(id[i]);
            } catch (AAtlantisException aex) {
                gasGap[i] = 0;
            }
        }
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
            logger.error("Problem decoding CSCD identifier", e);
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
            logger.error("Problem decoding CSCD identifier", e);
        }
        
        return false;
    }
    
    protected void applyCuts() 
    {
        super.applyCuts();
        cut("CutsMuon", "Charge", " Charge", charge);
        cutPhi(phi);
        cutEta(rho, z);
    }
        
    protected int getDrawOrFill() 
    {
        return AGraphics.FILL;
    }
    
    protected ACoord getYXUser() 
    {
        makeDrawList();
        if (parameterStore.get("YX", "Mode").getI() != AProjectionYX.MODE_MDT_INNER)
            return ACoord.NO_DATA;
        
        AParameter cscGasGap = parameterStore.get("YX", "CSCGasGap");
        if (cscGasGap.getStatus())
            cutArray(gasGap, cscGasGap.getI(), "CSC Gas Gap");
        
        double[][][] hv = new double[2][numDraw][2];
        int index[] = new int[numDraw];
        
        try {
            for (int i=0; i<numDraw; i++) {
                int j = listdl[i];
                index[i] = j;

                if (AIdHelper.cscMeasuresPhi(id[j]) == 1) {
                    double phi = Math.atan2(y[j], x[j]);
                    double rho = Math.sqrt(x[j]*x[j] + y[j]*y[j]);
                    double drho = length[j]/2.;
                
                    hv[0][i][0] = (rho - Math.cos(CSC_TILT_ANGLE) * drho) * Math.cos(phi);
                    hv[1][i][0] = (rho - Math.cos(CSC_TILT_ANGLE) * drho) * Math.sin(phi);
                    hv[0][i][1] = (rho + Math.cos(CSC_TILT_ANGLE) * drho) * Math.cos(phi);
                    hv[1][i][1] = (rho + Math.cos(CSC_TILT_ANGLE) * drho) * Math.sin(phi);
                } else {
                    double phi = Math.atan2(y[j], x[j]);
                    double rho = Math.sqrt(x[j]*x[j] + y[j]*y[j] + length[j]*length[j]/4.);
                    double dphi = Math.asin((length[j]/2.) / rho);

                    hv[0][i][0] = rho * Math.cos(phi - dphi);
                    hv[1][i][0] = rho * Math.sin(phi - dphi);
                    hv[0][i][1] = rho * Math.cos(phi + dphi);
                    hv[1][i][1] = rho * Math.sin(phi + dphi);
                }
            }

            return new ACoord(hv, index, this);
        } catch (AAtlantisException e) {
            AOutput.append("Error decoding CSC identifier: " + e.getMessage(), ALogInterface.BAD_COMMAND);
            return ACoord.NO_DATA;
        }
    }
    
    protected ACoord getFRUser() 
    {
        return getYXUser().convertYXToFR().includePhiWrapAround("FR");
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
                
                if (AIdHelper.cscMeasuresPhi(id[j]) == 1) {
                    double drho = length[j]/2.;
		    if(z[j] < 0){
			hv[0][i] = new double[] {z[j] - sign[i]*Math.sin(CSC_TILT_ANGLE) * drho,
						 z[j] + sign[i]*Math.sin(CSC_TILT_ANGLE) * drho };

			hv[1][i] = new double[] { sign[i] * (rho + sign[i]*Math.cos(CSC_TILT_ANGLE) * drho),
						  sign[i] * (rho - sign[i]*Math.cos(CSC_TILT_ANGLE) * drho) };

		    }else{
			hv[0][i] = new double[] {z[j] - Math.sin(CSC_TILT_ANGLE) * drho,
						 z[j] + Math.sin(CSC_TILT_ANGLE) * drho };
		    

			hv[1][i] = new double[] { sign[i] * (rho - Math.cos(CSC_TILT_ANGLE) * drho),
						  sign[i] * (rho + Math.cos(CSC_TILT_ANGLE) * drho) };
		    }
		

		    
                } else {
                    hv[0][i] = new double[] { z[j] };
                    hv[1][i] = new double[] { sign[i] * rho };
                }
	
	    }
	
            return new ACoord(hv, index, this);
	}
         catch (AAtlantisException e) {
            AOutput.append("Error decoding CSC identifier: " + e.getMessage(), ALogInterface.BAD_COMMAND);
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

    protected ACoord getFZUser() 
    {
        double[][][] hv = new double[2][numDraw][];
        int index[] = new int[numDraw];
        
        try {
            for (int i=0; i<numDraw; i++) {
                int j = listdl[i];
                index[i] = j;

                double rho = Math.sqrt(x[j]*x[j] + y[j]*y[j]);
                double phi = Math.toDegrees(Math.atan2(y[j], x[j]));
                
                if (AIdHelper.cscMeasuresPhi(id[j]) == 1) {
                    double drho = length[j]/2.;
                
                    hv[0][i] = new double[] { z[j] - Math.sin(CSC_TILT_ANGLE) * drho,
                                              z[j] + Math.sin(CSC_TILT_ANGLE) * drho };
                    hv[1][i] = new double[] { phi, phi };
                } else {
                    double dphi = Math.toDegrees(Math.atan2(length[j]/2., rho));
                    
                    hv[0][i] = new double[] { z[j], z[j] };
                    hv[1][i] = new double[] { phi-dphi, phi+dphi };
                }
            }

            return new ACoord(hv, index, this);
        } catch (AAtlantisException e) {
            AOutput.append("Error decoding CSCD identifier: " + e.getMessage(), ALogInterface.BAD_COMMAND);
            return ACoord.NO_DATA;
        }
    }
        
    // only constant color possible for the strips
    protected int internalColor() 
    {
        int constantColor=parameterStore.get(PARAMETER_GROUP, "Strip").getI();
        for(int i=0; i<numData; i++)
            color[i]=(byte)constantColor;
        return 0;
    }
    
}
