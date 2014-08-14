package atlantis.data;

import atlantis.event.*;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.AProjectionXZ;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;

/**
 * Generic track segment class.
 *
 * @author Eric Jansen
 */
public abstract class ASegmentData extends AData {

    protected float[] x;
    protected float[] y;
    protected float[] z;
    protected float[] theta;
    protected float[] phi;
    protected int[] hits;
    protected int[] numHits;

    ASegmentData(AHashMap p, AEvent e) {
        super(p,e);

        x = p.getFloatArray("x");
        y = p.getFloatArray("y");
        z = p.getFloatArray("z");
        phi = p.getFloatArray("phi");
        theta = p.getFloatArray("theta");
        numHits = p.getIntArray("numHits");

        hits = p.getIntArray("hits");
    }

    protected int internalColor() {
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();

        switch(colorFunction) {
            case 0:
                colorByConstant();
                break;
            case 1:
                colorByIndex();
                break;
        }

        return 1;
    }

    
    protected void applyCuts()
    {
        cutIndex();
        
    }

    
    public String getHitInfo(int index) {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0) return getNameScreenName()+" index: " + index+"\n"+
        AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phi[index]))+AMath.DEGREES;
        

        return getName() + " (index: " + index + ")\n"
            + " storegate key = " + storeGateKey + "\n"
            + " " + AMath.PHI + " = " + String.format("%.3f",Math.toDegrees(phi[index]))
            + "  (" + String.format("%.3f",phi[index]) + " rad)" + "\n"
            + " tL = " + String.format("%.3f",Math.toDegrees(1./Math.tan(theta[index]))) + "\n"
            + " x = " + String.format("%.3f",x[index]) + " cm\n"
            + " y = " + String.format("%.3f",y[index]) + " cm\n"
            + " z = " + String.format("%.3f",z[index]) + " cm\n"
            + " hits = " + numHits[index] + "\n";
    }

    protected ACoord getYXUser() {
        makeDrawList();

        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];
        double length = parameterStore.get(PARAMETER_GROUP, "SegmentLength").getD();

        for (int i=0; i<numDraw; i++) {
            int j = listdl[i];

            double dx = length * Math.sin(theta[j]) * Math.cos(phi[j]);
            double dy = length * Math.sin(theta[j]) * Math.sin(phi[j]);

            hv[0][i] = new double [] {x[j]-dx, x[j]-dx/2., x[j], x[j]+dx/2., x[j]+dx};
            hv[1][i] = new double [] {y[j]-dy, y[j]-dy/2., y[j], y[j]+dy/2., y[j]+dy};

            index[i] = j;
        }

        return new ACoord(hv, index, this, ACoord.SMOOTH_POLYLINES);
    }

    protected ACoord getFRUser() {
        return getYXUser().convertYXToFR().includePhiWrapAround("FR");
    }

    protected ACoord getRZUser() {
        makeDrawList();

        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];
        double length = parameterStore.get(PARAMETER_GROUP, "SegmentLength").getD();

        for (int i=0; i<numDraw; i++) {
            int j = listdl[i];

            double dx = length * Math.sin(theta[j]) * Math.cos(phi[j]);
            double dy = length * Math.sin(theta[j]) * Math.sin(phi[j]);
            double dz = length * Math.cos(theta[j]);

            int sign = AParameterUtilities.getRhoSign(x[j], y[j]);

            hv[0][i] = new double [] {z[j]-dz, z[j]+dz};
            hv[1][i] = new double [] {
                sign * Math.hypot(x[i]-dx, y[i]-dy), 
                sign * Math.hypot(x[i]+dx, y[i]+dy)
            };

            index[i] = j;
        }

        return new ACoord(hv, index, this, ACoord.POLYLINES);
    }

    protected ACoord getXZUser() {
        makeDrawList();

        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];
        double length = parameterStore.get(PARAMETER_GROUP, "SegmentLength").getD();
        double phi0 = Math.toRadians(AProjectionXZ.getPhi());

        for (int i=0; i<numDraw; i++) {
            int j = listdl[i];

            double dx = length * Math.sin(theta[j]) * Math.cos(phi[j]);
            double dy = length * Math.sin(theta[j]) * Math.sin(phi[j]);
            double dz = length * Math.cos(theta[j]);

            double s = x[j] * Math.cos(phi0) + y[j] * Math.sin(phi0);
            double ds = dx * Math.cos(phi0) + dy * Math.sin(phi0);

            if (z[j] < 0) {
                hv[0][i] = new double [] {z[j]+dz, z[j]-dz};
                hv[1][i] = new double [] {s-ds, s+ds};
            } else {
                hv[0][i] = new double [] {z[j]-dz, z[j]+dz};
                hv[1][i] = new double [] {s-ds, s+ds};
            }

            index[i] = j;
        }

        return new ACoord(hv, index, this, ACoord.POLYLINES);
    }
}
