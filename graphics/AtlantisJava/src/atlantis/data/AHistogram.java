package atlantis.data;

import atlantis.event.AData;
import atlantis.graphics.ACoord;
import atlantis.utils.AHashMap;
import atlantis.utils.ALogger;
import atlantis.utils.AVector;

/**
 * Used to create, fill, and draw the calorimeter histograms.
 */
public class AHistogram extends AData
{
    private static ALogger logger = ALogger.getLogger(AHistogram.class);
    
    public static final int UP = 0;
    public static final int DOWN = 1;
    
    public static final int HORIZONTAL = 0;
    public static final int VERTICAL = 1;
    
    public final static int RIGHT = 0;
    public final static int LEFT = 1;
    
    private float leftLimit;
    private float rightLimit;
    private float gran;
    private float granSafety;
    private float[] towers;
    
    private double eta0, z, r;
    private double factor;
    private AData detector;
    
    
    AHistogram(double leftLimit, double rightLimit, double gran, double factor, AData detector)
    { 
        super(new AHashMap(1).put("numData", 0),detector.getEvent());
        
        this.leftLimit = (float) leftLimit;
        this.rightLimit = (float) rightLimit;
        this.gran = (float) gran;
        this.factor = factor;
        this.detector = detector;
        granSafety = (float) (this.gran * 0.1);
        towers = new float[(int) Math.abs(Math.round((rightLimit - leftLimit) / gran))];
        numData = towers.length;
    }
    
    public float getGranularity()
    {
        return gran;
    }
    
    public int getTowersCount()
    {
        return towers.length;
    }
    
    public float[] getTowers()
    {
        return towers;
    }
    
    public void fill(double v1, double v2, double q)
    {
        int t1 = (int) ((v1 + granSafety - leftLimit) / gran);
        int t2 = (int) ((v2 - granSafety - leftLimit) / gran);
        int i = -999;
        
        if (Math.min(t1, t2) < 0 || Math.max(t1, t2) >= towers.length)
        {
            logger.error("Histogram binning problem " + t1 + " " + t2 + " " + towers.length + " " + v1 + " " + v2);
            return;
        }
        
        if (t1 == t2)
            towers[t1] += q;
        else
            for (i = t1; i <= t2; i++)
                towers[i] += q / (t2 - t1 + 1);
    }
    
    public void setGranularity(double newGran)
    {
        AHistogram newHist = new AHistogram(leftLimit, rightLimit, newGran, factor, detector);
        
        for (int i = 0; i < towers.length; i++)
            newHist.fill(leftLimit + i * gran, leftLimit + (i + 1) * gran, towers[i]);
        gran = (float) newGran;
        towers = newHist.towers;
    }
    
    public void add(AHistogram h)
    {
        float min = Math.min(leftLimit, h.leftLimit);
        float max = Math.max(rightLimit, h.rightLimit);
        
        AHistogram sum = new AHistogram(min, max, gran, factor, detector);
        
        // first I add this histogram
        for (int i = 0; i < towers.length; i++)
            sum.fill(leftLimit + i * gran, leftLimit + (i + 1) * gran, towers[i]);
        
        // and the "h" histogram
        for (int i = 0; i < h.towers.length; i++)
            sum.fill(h.leftLimit + i * h.gran, h.leftLimit + (i + 1) * h.gran, h.towers[i]);
        
        // copy the sum histogram parameters into old ones
        towers = sum.towers;
        leftLimit = min;
        rightLimit = max;
    }
    
    public AHistogram getRegion(double v1, double v2)
    {
        AHistogram hRegion = new AHistogram(v1, v2, gran, factor, detector);
        
        int n1 = (int) Math.round((v1 - leftLimit) / gran);
        int n2 = (int) Math.round((v2 - leftLimit) / gran);
        
        for (int i = n1; i < n2; i++)
            hRegion.fill(v1 + (i - n1) * gran, v1 + (i - n1 + 1) * gran, towers[i]);
        
        return hRegion;
    }
    
    private int getCode()
    {
        int c1, c2;
        
        if (leftLimit < -eta0)
            c1 = 1;
        else if (leftLimit > +eta0)
            c1 = 3;
        else
            c1 = 2;
        
        if (rightLimit < -eta0)
            c2 = 1;
        else if (rightLimit > +eta0)
            c2 = 3;
        else
            c2 = 2;
        
        return c1 * c2;
    }
    
    public ACoord[] getRZUser(double z1, double r1, int upORdown)
    {
        this.z = z1;
        this.r = r1;
        double expEta0, k = z / r;
        int N;
        
        eta0 = Math.abs(Math.log(k + Math.sqrt(k * k + 1)));
        
        switch (getCode())
        {
            case 1:
                return new ACoord[] { getRZVerticalUser(z, upORdown) };
                
            case 4:
                return new ACoord[] { getRZHorizontalUser(r, upORdown) };
                
            case 9:
                return new ACoord[] { getRZVerticalUser(z, upORdown) };
                
            case 2:
                N = (int) Math.abs(Math.round((eta0 - leftLimit) / gran));
                eta0 = leftLimit + N * gran;
                expEta0 = Math.exp(eta0);
                this.r = 2 * this.z / (expEta0 - 1 / expEta0);
                
                return new ACoord[] { getRegion(leftLimit, -eta0).getRZVerticalUser(z, upORdown), getRegion(-eta0, rightLimit).getRZHorizontalUser(r, upORdown) };
                
            case 6:
                N = (int) Math.abs(Math.round((eta0 - leftLimit) / gran));
                eta0 = leftLimit + N * gran;
                expEta0 = Math.exp(eta0);
                this.r = 2 * this.z / (expEta0 - 1 / expEta0);
                
                return new ACoord[] { getRegion(leftLimit, +eta0).getRZHorizontalUser(r, upORdown), getRegion(+eta0, rightLimit).getRZVerticalUser(z, upORdown) };
                
            case 3:
                // FIXME -- round() replaced by floor() here to make sure
                // histograms do not end up inside the calorimeter outline. 
                // They should be right at the edge regardless of the binning 
                // though. This should be changed. -- EJ
                N = (int) Math.abs(Math.floor((eta0 - leftLimit) / gran));
                eta0 = leftLimit + N * gran;
                expEta0 = Math.exp(eta0);
                this.r = 2 * this.z / (expEta0 - 1 / expEta0);
                
                return new ACoord[] { getRegion(leftLimit, -eta0).getRZVerticalUser(z, upORdown), getRegion(-eta0, +eta0).getRZHorizontalUser(r, upORdown), getRegion(+eta0, rightLimit).getRZVerticalUser(z, upORdown) };
                
            default:
                return null;
        }
    }
    
    public ACoord getRZHorizontalUser(double r, int place)
    {
        double[][][] hv = new double[2][towers.length][4];
        int[] index = new int[towers.length];
        double eta, exp1, exp2, zSign, rSign;
        
        for (int i = 0; i < towers.length; i++)
        {
            exp1 = Math.exp(leftLimit + i * gran);
            exp2 = Math.exp(leftLimit + (i + 1) * gran);
            rSign = (int) Math.pow(-1, place);
            
            eta = leftLimit + (i + 0.5) * gran;
            if (eta != 0)
                zSign = eta / Math.abs(eta);
            else
                zSign = 1;
            
            hv[0][i][0] = zSign * Math.abs(r * (exp1 - 1 / exp1) / 2);
            hv[1][i][0] = rSign * r;
            
            hv[0][i][1] = zSign * Math.abs(r * (exp2 - 1 / exp2) / 2);
            hv[1][i][1] = rSign * r;
            
            hv[0][i][2] = 0;
            hv[1][i][2] = 0;
            
            index[i] = i;
        }
        
        ACoord c = new ACoord(hv, index);
        
        c.source = this;
        return c;
    }
    
    public ACoord getRZVerticalUser(double z, int place)
    {
        double[][][] hv = new double[2][towers.length][4];
        int[] index = new int[towers.length];
        double exp1, exp2, zSign, rSign;
        
        for (int i = 0; i < towers.length; i++)
        {
            exp1 = Math.exp(leftLimit + i * gran);
            exp2 = Math.exp(leftLimit + (i + 1) * gran);
            rSign = (int) Math.pow(-1, place);
            
            if ((leftLimit > 0) && (rightLimit > 0))
                zSign = +1;
            else
                zSign = -1;
            
            hv[0][i][0] = zSign * z;
            hv[1][i][0] = rSign * Math.abs(2 * z / (exp2 - 1 / exp2));
            
            hv[0][i][1] = zSign * z;
            hv[1][i][1] = rSign * Math.abs(2 * z / (exp1 - 1 / exp1));
            
            hv[0][i][2] = 0;
            hv[1][i][2] = 0;
            
            index[i] = i;
        }
        
        ACoord c = new ACoord(hv, index);
        
        c.source = this;
        return c;
    }
    
    protected ACoord getYXUser(double r)
    {
        int n = towers.length;
        double[][][] hv = new double[2][n][4];
        int[] index = new int[n];
        
        for (int i = 0; i < n; i++)
        {
            double phi1 = leftLimit + i * gran;
            double phi2 = leftLimit + (i + 1) * gran;
            
            // first point
            hv[0][i][0] = r * Math.cos(phi1);
            hv[1][i][0] = r * Math.sin(phi1);
            
            // second point
            hv[1][i][1] = r * Math.sin(phi2);
            hv[0][i][1] = r * Math.cos(phi2);
            
            // third point (the center)
            hv[0][i][2] = 0;
            hv[1][i][2] = 0;
            
            index[i] = i;
        }
        
        ACoord c = new ACoord(hv, index);
        
        c.source = this;
        return c;
    }
    
    public ACoord completeTowers(ACoord c)
    {
        AVector u = new AVector(0, 0);
        
        for (int i = 0; i < c.hv[0].length; i++)
        {
            double x0 = (c.hv[0][i][0] + c.hv[0][i][1]) / 2;
            double y0 = (c.hv[1][i][0] + c.hv[1][i][1]) / 2;
            double f = factor * towers[c.index[i]];
            
            u.set(x0 - c.hv[0][i][2], y0 - c.hv[1][i][2]).makeUnitary().scale(f);
            
            c.hv[0][i][2] = c.hv[0][i][1] + u.dx;
            c.hv[1][i][2] = c.hv[1][i][1] + u.dy;
            
            c.hv[0][i][3] = c.hv[0][i][0] + u.dx;
            c.hv[1][i][3] = c.hv[1][i][0] + u.dy;
        }
        
        return c;
    }
    
    protected ACoord getFRUser(double r)
    {
        int n = towers.length;
        double[][][] hv = new double[2][n][4];
        int[] index = new int[n];
        
        for (int i = 0; i < n; i++)
        {
            hv[0][i][0] = r;
            hv[1][i][0] = Math.toDegrees(leftLimit + i * gran);
            
            hv[0][i][1] = r;
            hv[1][i][1] = Math.toDegrees(leftLimit + (i + 1) * gran);
            
            hv[0][i][2] = 0;
            hv[1][i][2] = Math.toDegrees(leftLimit + (i + 0.5) * gran);
            
            index[i] = i;
        }
        
        ACoord c = new ACoord(hv, index);
        
        c.source = this;
        return c;
    }
    
    protected ACoord getFZUser(double z, int alignment)
    {
        int n = towers.length;
        double[][][] hv = new double[2][n][4];
        int[] index = new int[n];
        
        double zSign = Math.pow(-1, alignment);
        
        for (int i = 0; i < n; i++)
        {
            hv[0][i][0] = zSign * z;
            hv[1][i][0] = Math.toDegrees(leftLimit + i * gran);
            
            hv[0][i][1] = zSign * z;
            hv[1][i][1] = Math.toDegrees(leftLimit + (i + 1) * gran);
            
            hv[0][i][2] = 0;
            hv[1][i][2] = Math.toDegrees(leftLimit + (i + 0.5) * gran);
            
            index[i] = i;
        }
        
        ACoord c = new ACoord(hv, index);
        
        c.source = this;
        return c;
    }
    
    // implementation of AData
    public String getParameterGroup()
    {
        return null;
    }
    
    public String getName()
    {
        return null;
    }
    
    public String getNameScreenName()
    {
        return detector.getParameterGroup() + " Histogram Tower";
    }
    
    protected int internalColor()
    {
        return 0;
    }
    
    protected void applyCuts()
    {}
    
    public String getHitInfo(int index)
    {
        StringBuffer msg = new StringBuffer(getNameScreenName());
        msg.append(" (index: " + index + ")");
        msg.append("\n ET = ");
        msg.append(String.format("%.2f",towers[index]));
        msg.append(" GeV");

        return msg.toString();
    }
    
    public int getIdFromIndex(int index)
    {
        // Histograms have only an internal ID, override this method here to avoid problems
        return index;
    }

}
